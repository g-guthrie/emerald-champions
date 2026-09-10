// Temporary mechanics parity, not an opening-access opponent roster.
// Include after Billy reserve helpers. Only Ditto is Billy's actual team data.
#line 36000 "work/billy_imposter_ability_probe.inc"
#include "battle_ai_switch.h"
#include "battle_ai_main.h"
#include "random.h"

static u32 BillyImposterAbilityHashBytes(u32 hash, const void *data, u32 size)
{
    const u8 *bytes = data;
    for (u32 i = 0; i < size; i++)
        hash = (hash ^ bytes[i]) * 16777619u;
    return hash;
}

static u32 BillyImposterAbilityStateHash(void)
{
    u32 hash = 2166136261u;
    hash = BillyImposterAbilityHashBytes(hash, gBattleMons, sizeof(gBattleMons));
    hash = BillyImposterAbilityHashBytes(hash, gParties, sizeof(gParties));
    hash = BillyImposterAbilityHashBytes(hash, gBattleStruct, sizeof(*gBattleStruct));
    hash = BillyImposterAbilityHashBytes(hash, gAiLogicData, sizeof(*gAiLogicData));
    hash = BillyImposterAbilityHashBytes(hash, &gRngValue, sizeof(gRngValue));
    return BillyImposterAbilityHashBytes(hash, &gRng2Value, sizeof(gRng2Value));
}

DOUBLE_BATTLE_TEST("EC live review Billy Imposter: copied Hospitality later-phase parity", u16 allyHp, u16 allyMaxHp, u16 copiedAbility)
{
    bool32 native;
    PARAMETRIZE { native = FALSE; }
    PARAMETRIZE { native = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet poltchageist = {
            .moves = {MOVE_PROTECT}, .nature = NATURE_BOLD, .ability = ABILITY_HOSPITALITY,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        BillyReservePlayer(SPECIES_PACHIRISU, &sBillyReservePachirisu);
        BillyReservePlayer(SPECIES_POLTCHAGEIST, &poltchageist);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_DITTO);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_IMPOSTER);
        // Injured ally snapshot creates an observable Hospitality effect.
        u16 hp = GetMonData(&party[1], MON_DATA_MAX_HP) / 2;
        SetMonData(&party[1], MON_DATA_HP, &hp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_RAIN_DANCE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        if (native)
        {
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
                SWITCH(opponentLeft, 2);
                MOVE(opponentRight, MOVE_PROTECT);
            }
        }
    } THEN {
        struct SwitchCandidateSnapshot *state = NULL;
        u32 before = 0;
        if (!native)
        {
            before = BillyImposterAbilityStateHash();
            state = AI_SaveCandidateState();
            EXPECT(state != NULL);
            AI_LoadSwitchCandidate(B_BATTLER_1, 2, FALSE);
        }
        EXPECT(gBattleMons[B_BATTLER_1].volatiles.transformed);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_HOSPITALITY);
        results[i].allyHp = gBattleMons[B_BATTLER_3].hp;
        results[i].allyMaxHp = gBattleMons[B_BATTLER_3].maxHP;
        results[i].copiedAbility = GetBattlerAbility(B_BATTLER_1);
        EXPECT_EQ(results[i].allyHp, results[i].allyMaxHp / 2 + results[i].allyMaxHp / 4);
        if (!native)
            EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_1], ABILITY_HOSPITALITY);
        Test_MgbaPrintf("BILLY_IMPOSTER_ALLY native=%d copied=%d cached=%d allyHP=%d/%d",
            native, results[i].copiedAbility, gAiLogicData->abilities[B_BATTLER_1], results[i].allyHp, results[i].allyMaxHp);
        if (!native)
        {
            AI_RestoreCandidateState(state);
            AI_FreeCandidateState(state);
            EXPECT_EQ(BillyImposterAbilityStateHash(), before);
            Test_MgbaPrintf("BILLY_IMPOSTER_RESTORE before=%d after=%d", before, BillyImposterAbilityStateHash());
        }
    } FINALLY {
        EXPECT_EQ(results[0].allyHp, results[1].allyHp);
        EXPECT_EQ(results[0].copiedAbility, results[1].copiedAbility);
    }
}
