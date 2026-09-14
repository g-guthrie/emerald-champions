#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba/core/serialize.h>
#include <mgba/core/blip_buf.h>
#include <mgba/gba/core.h>
#include <mgba/internal/gba/gba.h>
#include <mgba/internal/gba/savedata.h>
#include <mgba-util/vfs.h>

// A single native owner of the emulator. The browser never executes the ROM.
// Local pipe protocol: request {opcode, bytes}, response {status, bytes}.
// All fields little-endian; frames are RGBA8 + interleaved PCM16 at 32768 Hz.
static void quiet(struct mLogger *logger, int category, enum mLogLevel level, const char *format, va_list args)
{
    (void)logger; (void)category;
    if (level & (mLOG_FATAL | mLOG_ERROR)) { vfprintf(stderr, format, args); fputc('\n', stderr); }
}
static int read_exact(void *data, size_t size) { return fread(data, 1, size, stdin) == size; }
static void reply(uint32_t status, const void *data, uint32_t size)
{
    uint32_t header[] = {status, size};
    fwrite(header, sizeof(header), 1, stdout);
    if (size) fwrite(data, size, 1, stdout);
    fflush(stdout);
}
static uint64_t micros(void)
{
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000 + t.tv_nsec / 1000;
}
int main(int argc, char **argv)
{
    if (argc != 3) { fprintf(stderr, "core ROM SAVE-or-dash\n"); return 2; }
    struct mLogger logger = {.log = quiet};
    mLogSetDefaultLogger(&logger);
    struct mCore *core = GBACoreCreate();
    if (!core || !core->init(core) || !mCoreLoadFile(core, argv[1])) return 3;
    mCoreInitConfig(core, NULL);
    mCoreConfigSetOverrideIntValue(&core->config, "useBios", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "skipBios", 1);
    mCoreConfigSetOverrideIntValue(&core->config, "threadedVideo", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "hwaccelVideo", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "volume", 0x100);
    mCoreConfigSetOverrideIntValue(&core->config, "mute", 0);
    mCoreConfigSetOverrideIntValue(&core->config, "sampleRate", 32768);
    mCoreLoadForeignConfig(core, &core->config);
    unsigned width, height;
    core->desiredVideoDimensions(core, &width, &height);
    if (width != 240 || height != 160 || sizeof(color_t) != 4) return 4;
    color_t pixels[240 * 160];
    core->setVideoBuffer(core, pixels, width);
    // A reproducible sandbox clock; real campaign saves are never opened here.
    core->rtc.override = RTC_FIXED;
    core->rtc.value = 946684800000LL;
    core->setAudioBufferSize(core, 2048);
    for (int ch = 0; ch < 2; ch++) blip_set_rates(core->getAudioChannel(core, ch), core->frequency(core), 32768);
    core->reset(core);
    GBASavedataForceType(&((struct GBA *)core->board)->memory.savedata, SAVEDATA_FLASH1M);
    if (strcmp(argv[2], "-") && !mCoreLoadSaveFile(core, argv[2], false)) return 5;
    uint32_t header[2];
    uint8_t *payload = malloc(1048576);
    uint8_t *output = malloc(1048576);
    if (!payload || !output) return 6;
    while (read_exact(header, sizeof(header)))
    {
        if (header[1] > 1048575 || !read_exact(payload, header[1])) break;
        payload[header[1]] = 0;
        uint32_t *words = (uint32_t *)payload;
        uint32_t status = 0, size = 0;
        switch (header[0])
        {
        case 1: { // Frame batch; read addresses travel with every request.
            if (header[1] < 12 || words[0] > 120 || words[2] > 256 || header[1] != 12 + words[2] * 4) { status = 1; break; }
            uint64_t start = micros();
            int16_t pcm[4096]; int samples = 0;
            core->setKeys(core, words[1] & 1023);
            for (uint32_t f = 0; f < words[0]; f++)
            {
                core->runFrame(core);
                samples = blip_read_samples(core->getAudioChannel(core, 0), pcm, 2048, 1);
                int right = blip_read_samples(core->getAudioChannel(core, 1), pcm + 1, 2048, 1);
                if (right < samples) samples = right;
            }
            uint32_t meta[] = {core->frameCounter(core), (uint32_t)(micros() - start), (uint32_t)samples, words[2]};
            memcpy(output, meta, sizeof(meta));
            // mGBA's native little-endian XRGB byte order is RGBX.
            memcpy(output + 16, pixels, sizeof(pixels));
            for (unsigned i = 0; i < 240 * 160; i++) output[16 + i * 4 + 3] = 255;
            size = 16 + sizeof(pixels);
            memcpy(output + size, pcm, samples * 4); size += samples * 4;
            for (uint32_t i = 0; i < words[2]; i++)
            {
                uint32_t value = core->busRead32(core, words[3 + i]);
                memcpy(output + size, &value, 4); size += 4;
            }
            break;
        }
        case 2: case 3: { // Same-build state save/load. Host enforces ROM identity.
            struct VFile *vf = VFileOpen((char *)payload, header[0] == 2 ? O_WRONLY | O_CREAT | O_TRUNC : O_RDONLY);
            if (!vf) { status = 2; break; }
            bool ok = header[0] == 2 ? mCoreSaveStateNamed(core, vf, SAVESTATE_ALL) : mCoreLoadStateNamed(core, vf, SAVESTATE_ALL);
            vf->close(vf); status = !ok;
            break;
        }
        case 4: // Read memory.
            if (header[1] != 8 || words[1] > 1048576) { status = 1; break; }
            size = words[1];
            for (uint32_t i = 0; i < size; i++) output[i] = core->busRead8(core, words[0] + i);
            break;
        case 5: // Batched word writes to the development mailboxes.
            if (header[1] % 8) { status = 1; break; }
            for (uint32_t i = 0; i < header[1] / 4; i += 2) core->busWrite32(core, words[i], words[i + 1]);
            break;
        case 6: { // Export native flash, after the ROM has completed a save.
            struct GBASavedata *save = &((struct GBA *)core->board)->memory.savedata;
            struct VFile *file = VFileOpen((char *)payload, O_WRONLY | O_CREAT | O_TRUNC);
            status = !file || !GBASavedataSize(save);
            if (!status) status = !GBASavedataClone(save, file);
            if (file && !file->close(file)) status = 1;
            break;
        }
        default: status = 1;
        }
        reply(status, output, size);
    }
    free(payload); free(output);
    mCoreConfigDeinit(&core->config); core->deinit(core);
    return 0;
}
