#include "global.h"
#include "event_data.h"
#include "script.h"
#include "constants/songs.h"
#include "constants/script_commands.h"
#include "test/test.h"

extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];
extern const u8 RustboroCity_EventScript_MayBrineyHint[];
extern const u8 RustboroCity_EventScript_BrendanBrineyHint[];
extern const u8 RustboroCity_EventScript_RestoreBgm[];

TEST("Rustboro presentation: both rival farewells clear encounter music regardless of scratch state")
{
    const u8 *hint;
    PARAMETRIZE { hint = RustboroCity_EventScript_MayBrineyHint; }
    PARAMETRIZE { hint = RustboroCity_EventScript_BrendanBrineyHint; }
    static const u16 scratchValues[] = {0, 1, 0xFFFF};
    for (u32 i = 0; i < ARRAY_COUNT(scratchValues); i++)
    {
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, hint);
        gSpecialVar_0x8008 = scratchValues[i];
        gSaveBlock1Ptr->savedMusic = MUS_ENCOUNTER_BRENDAN;
        // Execute the compiled text selection, then model dismissal of msgbox.
        // This test covers post-dialogue routing, not the text printer or audio.
        EXPECT_EQ(*ctx.scriptPtr, SCR_OP_LOAD_WORD);
        u8 command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx));
        EXPECT_EQ(*ctx.scriptPtr, SCR_OP_CALL_STD);
        ctx.scriptPtr += 2;
        for (u32 step = 0; step < 6
            && ctx.scriptPtr != RustboroCity_EventScript_RestoreBgm
            && *ctx.scriptPtr != SCR_OP_RELEASEALL; step++)
        {
            command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, RustboroCity_EventScript_RestoreBgm);
        // Execute savebgm; stop before the hardware fade.
        command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx));
        EXPECT_EQ(gSaveBlock1Ptr->savedMusic, MUS_DUMMY);
    }
}
