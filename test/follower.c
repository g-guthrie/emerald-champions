#include "global.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "follower_helper.h"
#include "pokemon.h"
#include "string_util.h"
#include "text.h"
#include "test/test.h"
#include "constants/vars.h"

static void ResetFollowerTestParty(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
    SetChosenFollowerMon(NULL);
}

static struct Pokemon *GiveFollowerTestMon(u32 slot, enum Species species, u32 personality)
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
    CreateMon(mon, species, 20, personality, OTID_STRUCT_PRESET(12345));
    CalculateMonStats(mon);
    gPartiesCount[B_TRAINER_PLAYER] = max(gPartiesCount[B_TRAINER_PLAYER], slot + 1);
    return mon;
}

static void SwapPartySlots(u32 a, u32 b)
{
    struct Pokemon tmp = gParties[B_TRAINER_PLAYER][a];
    gParties[B_TRAINER_PLAYER][a] = gParties[B_TRAINER_PLAYER][b];
    gParties[B_TRAINER_PLAYER][b] = tmp;
}

TEST("Follower defaults to the first conscious non-Egg party Pokemon")
{
    ResetFollowerTestParty();
    GiveFollowerTestMon(0, SPECIES_MUDKIP, 1);
    GiveFollowerTestMon(1, SPECIES_ZIGZAGOON, 2);
    GiveFollowerTestMon(2, SPECIES_WURMPLE, 3);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP, &(u16){0});
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_IS_EGG, &(bool8){TRUE});

    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][2]);
    EXPECT_EQ(GetFollowerMon(), GetFirstLiveMon());
}

TEST("Chosen follower is found by identity after the party is reordered")
{
    ResetFollowerTestParty();
    GiveFollowerTestMon(0, SPECIES_MUDKIP, 1);
    GiveFollowerTestMon(1, SPECIES_ZIGZAGOON, 2);
    GiveFollowerTestMon(2, SPECIES_WURMPLE, 3);
    SetChosenFollowerMon(&gParties[B_TRAINER_PLAYER][1]);

    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][1]);
    SwapPartySlots(1, 2);
    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][2]);
    EXPECT_EQ(GetMonData(GetFollowerMon(), MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    // The lead is unchanged, so non-follower "first live mon" semantics are kept
    EXPECT_EQ(GetFirstLiveMon(), &gParties[B_TRAINER_PLAYER][0]);
}

TEST("Chosen follower falls back to the default while fainted or away, and returns")
{
    struct Pokemon stored;
    ResetFollowerTestParty();
    GiveFollowerTestMon(0, SPECIES_MUDKIP, 1);
    GiveFollowerTestMon(1, SPECIES_ZIGZAGOON, 2);
    SetChosenFollowerMon(&gParties[B_TRAINER_PLAYER][1]);

    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP, &(u16){0});
    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][0]);
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP, &(u16){10});
    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][1]);

    stored = gParties[B_TRAINER_PLAYER][1];
    ZeroMonData(&gParties[B_TRAINER_PLAYER][1]);
    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][0]);
    gParties[B_TRAINER_PLAYER][1] = stored;
    EXPECT_EQ(GetFollowerMon(), &gParties[B_TRAINER_PLAYER][1]);
}

TEST("Follower choice is stored in saved vars")
{
    ResetFollowerTestParty();
    GiveFollowerTestMon(0, SPECIES_MUDKIP, 0x12345678);
    SetChosenFollowerMon(&gParties[B_TRAINER_PLAYER][0]);
    EXPECT_EQ(VarGet(VAR_FOLLOWER_MON_PID_LO), 0x5678);
    EXPECT_EQ(VarGet(VAR_FOLLOWER_MON_PID_HI), 0x1234);
    EXPECT_EQ(VarGet(VAR_FOLLOWER_MON_OTID_LO) | (VarGet(VAR_FOLLOWER_MON_OTID_HI) << 16), GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_OT_ID));
    SetChosenFollowerMon(NULL);
}

// Field message box: 27 tiles wide, text starts at x = 0
#define FOLLOWER_MSG_WIDTH (27 * 8)

static const u8 *GetWidestSpeciesName(void)
{
    u32 widest = 0;
    const u8 *name = NULL;
    for (enum Species species = 1; species < NUM_SPECIES; species++)
    {
        if (!IsSpeciesEnabled(species))
            continue;
        u32 width = GetStringWidth(FONT_NORMAL, GetSpeciesName(species), 0);
        if (width > widest)
        {
            widest = width;
            name = GetSpeciesName(species);
        }
    }
    return name;
}

static u32 GetFollowerMessageWidth(const u8 *text)
{
    StringCopy(gStringVar1, GetWidestSpeciesName());
    StringExpandPlaceholders(gStringVar4, text);
    return GetStringWidth(FONT_NORMAL, gStringVar4, 0);
}

TEST("Follower messages fit the field message box")
{
    u32 i, j;
    for (i = 0; i < FOLLOWER_EMOTION_LENGTH; i++)
    {
        for (j = 0; j < gFollowerBasicMessages[i].length; j++)
            EXPECT_LE(GetFollowerMessageWidth(gFollowerBasicMessages[i].messages[j].text), FOLLOWER_MSG_WIDTH);
    }
    for (i = 0; i < COND_MSG_COUNT; i++)
    {
        const struct FollowerMsgInfoExtended *info = &gFollowerConditionalMessages[i];
        if (info->text == NULL)
            continue;
        if (info->textSpread)
        {
            const u8 *const *texts = (const u8 *const *)info->text;
            for (j = 0; j < 4 && texts[j] != NULL; j++)
                EXPECT_LE(GetFollowerMessageWidth(texts[j]), FOLLOWER_MSG_WIDTH);
        }
        else
        {
            EXPECT_LE(GetFollowerMessageWidth(info->text), FOLLOWER_MSG_WIDTH);
        }
    }
}
