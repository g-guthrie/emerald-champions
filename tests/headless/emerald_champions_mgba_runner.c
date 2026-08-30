#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/interface.h>
#include <mgba/core/log.h>
#include <mgba/gba/core.h>
#include <mgba/internal/gba/gba.h>
#include <mgba/internal/gba/input.h>
#include <mgba/internal/gba/savedata.h>
#include <mgba-util/vfs.h>

#define MAX_EVENTS 512
#define DEFAULT_RTC_EPOCH 946684800LL

struct KeyEvent
{
    unsigned frame;
    unsigned duration;
    uint32_t keys;
};

struct WriteEvent
{
    unsigned frame;
    unsigned width;
    uint32_t address;
    uint32_t value;
};

struct ReadRequest
{
    unsigned width;
    uint32_t address;
};

struct ScreenshotEvent
{
    unsigned frame;
    const char *path;
};

struct StopCondition
{
    bool enabled;
    unsigned width;
    uint32_t address;
    uint32_t mask;
    uint32_t value;
};

struct Options
{
    const char *romPath;
    const char *savePath;
    const char *saveOutPath;
    const char *screenshotPath;
    unsigned frames;
    int64_t rtcEpoch;
    struct KeyEvent keyEvents[MAX_EVENTS];
    unsigned keyEventCount;
    struct WriteEvent writeEvents[MAX_EVENTS];
    unsigned writeEventCount;
    struct ReadRequest reads[MAX_EVENTS];
    unsigned readCount;
    struct ScreenshotEvent screenshotEvents[MAX_EVENTS];
    unsigned screenshotEventCount;
    struct StopCondition stop;
};

static bool ParseUnsigned(const char *text, uint32_t *value);

static void QuietLog(struct mLogger *logger, int category, enum mLogLevel level,
                     const char *format, va_list args)
{
    (void)logger;
    (void)category;
    (void)level;
    (void)format;
    (void)args;
}

static void Usage(const char *program)
{
    fprintf(stderr,
            "usage: %s --rom FILE --frames N [options]\n"
            "  --save FILE                 attach a writable scratch save copy\n"
            "  --save-out FILE             dump final emulated save data\n"
            "  --screenshot FILE           write the final video frame as PNG\n"
            "  --screenshot-at FRAME:FILE  write an intermediate video frame as PNG\n"
            "  --rtc UNIX_SECONDS          fixed RTC epoch (default: 2000-01-01 UTC)\n"
            "  --key FRAME:DURATION:KEYS   hold comma-separated keys\n"
            "  --write FRAME:WIDTH:ADDR:VALUE\n"
            "  --read WIDTH:ADDR           print final memory value\n"
            "  --until WIDTH:ADDR:MASK:VALUE stop after a matching frame\n"
            "keys: A,B,START,SELECT,UP,DOWN,LEFT,RIGHT,L,R\n",
            program);
}

static bool ParseScreenshotEvent(char *text, struct ScreenshotEvent *event)
{
    char *separator = strchr(text, ':');
    uint32_t frame;

    if (separator == NULL || separator == text || separator[1] == '\0')
        return false;
    *separator = '\0';
    if (!ParseUnsigned(text, &frame))
        return false;
    event->frame = frame;
    event->path = separator + 1;
    return true;
}

static bool ParseUnsigned(const char *text, uint32_t *value)
{
    char *end;
    unsigned long parsed;

    errno = 0;
    parsed = strtoul(text, &end, 0);
    if (errno != 0 || end == text || *end != '\0' || parsed > UINT32_MAX)
        return false;
    *value = (uint32_t)parsed;
    return true;
}

static bool ParseSigned64(const char *text, int64_t *value)
{
    char *end;
    long long parsed;

    errno = 0;
    parsed = strtoll(text, &end, 0);
    if (errno != 0 || end == text || *end != '\0')
        return false;
    *value = (int64_t)parsed;
    return true;
}

static bool IsValidWidth(unsigned width)
{
    return width == 1 || width == 2 || width == 4;
}

static bool TokenEquals(const char *token, size_t length, const char *expected)
{
    size_t expectedLength = strlen(expected);
    size_t i;

    if (length != expectedLength)
        return false;
    for (i = 0; i < length; i++)
    {
        char a = token[i];
        char b = expected[i];
        if (a >= 'a' && a <= 'z')
            a -= 'a' - 'A';
        if (a != b)
            return false;
    }
    return true;
}

static bool ParseKeys(const char *text, uint32_t *keys)
{
    const char *token = text;
    const char *cursor = text;
    uint32_t parsed = 0;

    while (true)
    {
        if (*cursor == ',' || *cursor == '\0')
        {
            size_t length = (size_t)(cursor - token);
            if (TokenEquals(token, length, "A"))
                parsed |= 1u << GBA_KEY_A;
            else if (TokenEquals(token, length, "B"))
                parsed |= 1u << GBA_KEY_B;
            else if (TokenEquals(token, length, "START"))
                parsed |= 1u << GBA_KEY_START;
            else if (TokenEquals(token, length, "SELECT"))
                parsed |= 1u << GBA_KEY_SELECT;
            else if (TokenEquals(token, length, "UP"))
                parsed |= 1u << GBA_KEY_UP;
            else if (TokenEquals(token, length, "DOWN"))
                parsed |= 1u << GBA_KEY_DOWN;
            else if (TokenEquals(token, length, "LEFT"))
                parsed |= 1u << GBA_KEY_LEFT;
            else if (TokenEquals(token, length, "RIGHT"))
                parsed |= 1u << GBA_KEY_RIGHT;
            else if (TokenEquals(token, length, "L"))
                parsed |= 1u << GBA_KEY_L;
            else if (TokenEquals(token, length, "R"))
                parsed |= 1u << GBA_KEY_R;
            else
                return false;

            if (*cursor == '\0')
                break;
            token = cursor + 1;
        }
        cursor++;
    }

    *keys = parsed;
    return true;
}

static bool SplitFields(const char *text, char fields[][64], unsigned expected)
{
    unsigned field = 0;
    unsigned length = 0;
    const char *cursor;

    memset(fields, 0, sizeof(fields[0]) * expected);
    for (cursor = text; ; cursor++)
    {
        if (*cursor == ':' || *cursor == '\0')
        {
            if (field >= expected || length == 0)
                return false;
            fields[field][length] = '\0';
            field++;
            length = 0;
            if (*cursor == '\0')
                break;
            continue;
        }
        if (field >= expected || length + 1 >= sizeof(fields[0]))
            return false;
        fields[field][length++] = *cursor;
    }
    return field == expected;
}

static bool ParseKeyEvent(const char *text, struct KeyEvent *event)
{
    char fields[3][64];
    uint32_t frame;
    uint32_t duration;

    if (!SplitFields(text, fields, 3)
     || !ParseUnsigned(fields[0], &frame)
     || !ParseUnsigned(fields[1], &duration)
     || duration == 0
     || !ParseKeys(fields[2], &event->keys))
        return false;
    event->frame = frame;
    event->duration = duration;
    return true;
}

static bool ParseWriteEvent(const char *text, struct WriteEvent *event)
{
    char fields[4][64];
    uint32_t frame;
    uint32_t width;

    if (!SplitFields(text, fields, 4)
     || !ParseUnsigned(fields[0], &frame)
     || !ParseUnsigned(fields[1], &width)
     || !IsValidWidth(width)
     || !ParseUnsigned(fields[2], &event->address)
     || !ParseUnsigned(fields[3], &event->value))
        return false;
    event->frame = frame;
    event->width = width;
    return true;
}

static bool ParseReadRequest(const char *text, struct ReadRequest *request)
{
    char fields[2][64];
    uint32_t width;

    if (!SplitFields(text, fields, 2)
     || !ParseUnsigned(fields[0], &width)
     || !IsValidWidth(width)
     || !ParseUnsigned(fields[1], &request->address))
        return false;
    request->width = width;
    return true;
}

static bool ParseStopCondition(const char *text, struct StopCondition *condition)
{
    char fields[4][64];
    uint32_t width;

    if (!SplitFields(text, fields, 4)
     || !ParseUnsigned(fields[0], &width)
     || !IsValidWidth(width)
     || !ParseUnsigned(fields[1], &condition->address)
     || !ParseUnsigned(fields[2], &condition->mask)
     || !ParseUnsigned(fields[3], &condition->value))
        return false;
    condition->enabled = true;
    condition->width = width;
    return true;
}

static bool ParseOptions(int argc, char **argv, struct Options *options)
{
    int i;
    uint32_t frames;

    memset(options, 0, sizeof(*options));
    options->savePath = "-";
    options->rtcEpoch = DEFAULT_RTC_EPOCH;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--rom") == 0 && i + 1 < argc)
            options->romPath = argv[++i];
        else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
        {
            if (!ParseUnsigned(argv[++i], &frames) || frames == 0)
                return false;
            options->frames = frames;
        }
        else if (strcmp(argv[i], "--save") == 0 && i + 1 < argc)
            options->savePath = argv[++i];
        else if (strcmp(argv[i], "--save-out") == 0 && i + 1 < argc)
            options->saveOutPath = argv[++i];
        else if (strcmp(argv[i], "--screenshot") == 0 && i + 1 < argc)
            options->screenshotPath = argv[++i];
        else if (strcmp(argv[i], "--screenshot-at") == 0 && i + 1 < argc)
        {
            if (options->screenshotEventCount >= MAX_EVENTS
             || !ParseScreenshotEvent(argv[++i], &options->screenshotEvents[options->screenshotEventCount++]))
                return false;
        }
        else if (strcmp(argv[i], "--rtc") == 0 && i + 1 < argc)
        {
            if (!ParseSigned64(argv[++i], &options->rtcEpoch))
                return false;
        }
        else if (strcmp(argv[i], "--key") == 0 && i + 1 < argc)
        {
            if (options->keyEventCount >= MAX_EVENTS
             || !ParseKeyEvent(argv[++i], &options->keyEvents[options->keyEventCount++]))
                return false;
        }
        else if (strcmp(argv[i], "--write") == 0 && i + 1 < argc)
        {
            if (options->writeEventCount >= MAX_EVENTS
             || !ParseWriteEvent(argv[++i], &options->writeEvents[options->writeEventCount++]))
                return false;
        }
        else if (strcmp(argv[i], "--read") == 0 && i + 1 < argc)
        {
            if (options->readCount >= MAX_EVENTS
             || !ParseReadRequest(argv[++i], &options->reads[options->readCount++]))
                return false;
        }
        else if (strcmp(argv[i], "--until") == 0 && i + 1 < argc)
        {
            if (options->stop.enabled || !ParseStopCondition(argv[++i], &options->stop))
                return false;
        }
        else
            return false;
    }
    return options->romPath != NULL && options->frames != 0;
}

static uint32_t ReadMemory(struct mCore *core, unsigned width, uint32_t address)
{
    switch (width)
    {
    case 1: return core->busRead8(core, address);
    case 2: return core->busRead16(core, address);
    case 4: return core->busRead32(core, address);
    default: return 0;
    }
}

static void WriteMemory(struct mCore *core, const struct WriteEvent *event)
{
    switch (event->width)
    {
    case 1: core->busWrite8(core, event->address, event->value); break;
    case 2: core->busWrite16(core, event->address, event->value); break;
    case 4: core->busWrite32(core, event->address, event->value); break;
    }
}

static bool WriteScreenshot(struct mCore *core, const char *path)
{
    struct VFile *vf = VFileOpen(path, O_WRONLY | O_CREAT | O_TRUNC);
    bool success;

    if (vf == NULL)
        return false;
    success = mCoreTakeScreenshotVF(core, vf);
    vf->close(vf);
    return success;
}

static bool WriteSaveData(struct mCore *core, const char *path, size_t *sizeOut)
{
    struct GBASavedata *savedata = &((struct GBA *)core->board)->memory.savedata;
    size_t size = GBASavedataSize(savedata);
    struct VFile *vf;
    bool success;

    *sizeOut = size;
    if (size == 0)
        return false;
    vf = VFileOpen(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (vf == NULL)
        return false;
    success = GBASavedataClone(savedata, vf);
    success = vf->close(vf) && success;
    return success;
}

static uint64_t HashVideo(const color_t *pixels, size_t count, size_t *nonzeroOut)
{
    const uint8_t *bytes = (const uint8_t *)pixels;
    size_t byteCount = count * sizeof(*pixels);
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t nonzero = 0;
    size_t i;

    for (i = 0; i < count; i++)
    {
        if (pixels[i] != 0)
            nonzero++;
    }
    for (i = 0; i < byteCount; i++)
    {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    *nonzeroOut = nonzero;
    return hash;
}

int main(int argc, char **argv)
{
    struct Options options;
    struct mLogger logger = { .log = QuietLog };
    struct mCore *core = NULL;
    unsigned width = 240;
    unsigned height = 160;
    color_t *pixels = NULL;
    unsigned frame;
    unsigned framesRun = 0;
    bool stopMatched = false;
    uint32_t pc = 0;
    uint64_t videoHash;
    size_t nonzeroPixels;
    size_t saveSize = 0;
    unsigned i;
    int result = 0;
    bool configInitialized = false;

    if (!ParseOptions(argc, argv, &options))
    {
        Usage(argv[0]);
        return 2;
    }

    mLogSetDefaultLogger(&logger);
    core = GBACoreCreate();
    if (core == NULL || !core->init(core))
    {
        fprintf(stderr, "failed to initialize mGBA core\n");
        result = 3;
        goto cleanup;
    }
    if (!mCoreLoadFile(core, options.romPath))
    {
        fprintf(stderr, "failed to load ROM: %s\n", options.romPath);
        result = 4;
        goto cleanup;
    }

    mCoreInitConfig(core, NULL);
    configInitialized = true;
    /* Do not import the operator's desktop mGBA preferences. A deterministic
     * runner supplies its complete frontend-relevant configuration. */
    mCoreConfigSetOverrideIntValue(&core->config, "useBios", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "skipBios", 1);
    mCoreConfigSetOverrideIntValue(&core->config, "threadedVideo", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "hwaccelVideo", 0);
    mCoreLoadForeignConfig(core, &core->config);
    core->rtc.override = RTC_FIXED;
    core->rtc.value = options.rtcEpoch;

    core->desiredVideoDimensions(core, &width, &height);
    pixels = calloc((size_t)width * height, sizeof(*pixels));
    if (pixels == NULL)
    {
        result = 6;
        goto cleanup;
    }
    core->setVideoBuffer(core, pixels, width);
    core->reset(core);
    /* Emerald Champions uses Emerald's 1 Mbit flash. Force the cartridge geometry before
     * attaching scratch backing so a read-only boot still has clonable save
     * memory even before native code performs its first flash write. */
    GBASavedataForceType(&((struct GBA *)core->board)->memory.savedata, SAVEDATA_FLASH1M);
    if (strcmp(options.savePath, "-") != 0
     && !mCoreLoadSaveFile(core, options.savePath, false))
    {
        fprintf(stderr, "failed to load save: %s\n", options.savePath);
        result = 5;
        goto cleanup;
    }

    for (frame = 0; frame < options.frames; frame++)
    {
        uint32_t keys = 0;

        for (i = 0; i < options.writeEventCount; i++)
        {
            if (options.writeEvents[i].frame == frame)
                WriteMemory(core, &options.writeEvents[i]);
        }
        for (i = 0; i < options.keyEventCount; i++)
        {
            const struct KeyEvent *event = &options.keyEvents[i];
            if (frame >= event->frame && frame - event->frame < event->duration)
                keys |= event->keys;
        }
        core->setKeys(core, keys);
        core->runFrame(core);
        framesRun = frame + 1;

        for (i = 0; i < options.screenshotEventCount; i++)
        {
            const struct ScreenshotEvent *event = &options.screenshotEvents[i];
            if (event->frame == frame && !WriteScreenshot(core, event->path))
            {
                fprintf(stderr, "failed to write screenshot at frame %u: %s\n",
                        event->frame, event->path);
                result = 7;
                goto cleanup;
            }
        }

        if (options.stop.enabled)
        {
            uint32_t value = ReadMemory(core, options.stop.width, options.stop.address);
            if ((value & options.stop.mask) == (options.stop.value & options.stop.mask))
            {
                stopMatched = true;
                break;
            }
        }
    }

    if (options.screenshotPath != NULL && !WriteScreenshot(core, options.screenshotPath))
    {
        fprintf(stderr, "failed to write screenshot: %s\n", options.screenshotPath);
        result = 7;
        goto cleanup;
    }
    if (options.saveOutPath != NULL && !WriteSaveData(core, options.saveOutPath, &saveSize))
    {
        fprintf(stderr, "failed to write save data: %s\n", options.saveOutPath);
        result = 8;
        goto cleanup;
    }

    if (!core->readRegister(core, "pc", &pc))
        (void)core->readRegister(core, "r15", &pc);
    videoHash = HashVideo(pixels, (size_t)width * height, &nonzeroPixels);

    printf("RESULT frames=%u stop_matched=%u pc=%08" PRIx32
           " rtc=%" PRId64 " width=%u height=%u"
           " video_hash=%016" PRIx64 " nonzero_pixels=%zu save_bytes=%zu\n",
           framesRun, stopMatched, pc, options.rtcEpoch, width, height,
           videoHash, nonzeroPixels, saveSize);
    for (i = 0; i < options.readCount; i++)
    {
        uint32_t value = ReadMemory(core, options.reads[i].width, options.reads[i].address);
        printf("READ width=%u address=%08" PRIx32 " value=%08" PRIx32 "\n",
               options.reads[i].width, options.reads[i].address, value);
    }

cleanup:
    free(pixels);
    if (core != NULL)
    {
        if (configInitialized)
            mCoreConfigDeinit(&core->config);
        core->deinit(core);
    }
    return result;
}
