#include "global.h"
#include "script.h"
#include "test/test.h"

TEST("Script context: initialization clears prior comparison and wait state")
{
    struct ScriptContext ctx;
    ScrCmdFunc commands[1] = {NULL};
    memset(&ctx, 0xFF, sizeof(ctx));
    InitScriptContext(&ctx, commands, commands + 1);
    EXPECT_EQ(ctx.comparisonResult, 0);
    EXPECT_EQ((u32)ctx.waitAfterCallNative, FALSE);
    EXPECT_EQ((u32)ctx.breakOnTrainerBattle, FALSE);
    EXPECT_EQ(ctx.stackDepth, 0);
    EXPECT_EQ(ctx.scriptPtr, NULL);
    EXPECT_EQ(ctx.cmdTable, commands);
    EXPECT_EQ(ctx.cmdTableEnd, commands + 1);
    EXPECT(!RunScriptCommand(&ctx));
}

TEST("Script context: byte readers handle unaligned input and advance only on reads")
{
    static const u8 bytes[] = {0, 0x12, 0x34, 0x56, 0xFE, 0xDC, 0xBA};
    struct ScriptContext ctx;
    InitScriptContext(&ctx, NULL, NULL);
    SetupBytecodeScript(&ctx, bytes + 1);
    EXPECT_EQ(ScriptPeekHalfword(&ctx), 0x3412);
    EXPECT_EQ(ctx.scriptPtr, bytes + 1);
    EXPECT_EQ(ScriptReadHalfword(&ctx), 0x3412);
    EXPECT_EQ(ctx.scriptPtr, bytes + 3);
    EXPECT_EQ(ScriptPeekWord(&ctx), 0xBADCFE56);
    EXPECT_EQ(ctx.scriptPtr, bytes + 3);
    EXPECT_EQ(ScriptReadWord(&ctx), 0xBADCFE56);
    EXPECT_EQ(ctx.scriptPtr, bytes + 7);
}

TEST("Script context: all return slots work and overflow preserves both stacks")
{
    struct ScriptContext ctx;
    struct ScriptStack stack;
    static const u8 addresses[SCRIPT_STACK_SIZE + 1] = {0};
    InitScriptContext(&ctx, NULL, NULL);
    InitScriptStack(&stack);
    for (u32 i = 0; i < SCRIPT_STACK_SIZE; i++)
    {
        EXPECT(!ScriptPush(&ctx, &addresses[i]));
        EXPECT(ScriptStackPush(&stack, &addresses[i]));
    }
    EXPECT(ScriptPush(&ctx, &addresses[SCRIPT_STACK_SIZE]));
    EXPECT(!ScriptStackPush(&stack, &addresses[SCRIPT_STACK_SIZE]));
    EXPECT_EQ(ctx.stackDepth, SCRIPT_STACK_SIZE);
    EXPECT_EQ(stack.stackDepth, SCRIPT_STACK_SIZE);
    for (u32 i = SCRIPT_STACK_SIZE; i > 0; i--)
    {
        EXPECT_EQ(ScriptPop(&ctx), &addresses[i - 1]);
        EXPECT_EQ(ScriptStackPop(&stack), &addresses[i - 1]);
    }
    EXPECT_EQ(ScriptPop(&ctx), NULL);
    EXPECT_EQ(ScriptStackPop(&stack), NULL);
}
