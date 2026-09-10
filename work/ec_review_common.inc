// Temporary reusable native authoring helpers; contains no registered tests.
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"
#line 50000 "work/ec_review_common.inc"

static const struct EmeraldChampionsBattleSet sReviewEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
    .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY, .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sReviewGurdurr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
    .nature = NATURE_ADAMANT, .ability = ABILITY_GUTS, .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sReviewXatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
    .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE, .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sReviewPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY,
    .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sReviewMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_TAUNT}, .item = ITEM_FOCUS_SASH,
    .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {4, 252, 0, 0, 0, 252},
};
static const struct EmeraldChampionsBattleSet sReviewLombre = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
    .nature = NATURE_MODEST, .ability = ABILITY_SWIFT_SWIM, .evs = {252, 0, 4, 252, 0, 0},
};

static void ReviewPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set, u32 level)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (set->moves[i] != MOVE_NONE) EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[i]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        bool32 fixedSpeed = FALSE;
        SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
    }
}

static void ReviewOpponent(u16 trainer, u32 badges, u32 reservePair)
{
    AuthoredOpponent(trainer, badges, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    for (u32 i = 0; i < gTrainers[DIFFICULTY_NORMAL][trainer].partySize; i++)
        Test_MgbaPrintf("REVIEW_INPUT trainer=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Spe=%d item=%d ability=%d", trainer, i,
            GetMonData(&party[i], MON_DATA_SPECIES), GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP),
            GetMonData(&party[i], MON_DATA_ATK), GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK),
            GetMonData(&party[i], MON_DATA_SPDEF), GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    if (reservePair) {
        EXPECT(reservePair * 2 + 1 < gTrainers[DIFFICULTY_NORMAL][trainer].partySize);
        for (u32 i = 0; i < 2; i++) {
            struct Pokemon mon = party[i]; party[i] = party[i + reservePair * 2]; party[i + reservePair * 2] = mon;
        }
        for (u32 i = 2; i < gTrainers[DIFFICULTY_NORMAL][trainer].partySize; i++) {
            u16 zero = 0; SetMonData(&party[i], MON_DATA_HP, &zero);
        }
    }
}

static void ReviewResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("REVIEW_AFTER board=%d turns=%d reserves=%d species=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d initialFrames=%d", board, turns, reserves,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter, gBattleStruct->aiDelayFrames);
    for (u32 i = 1; i < gBattlersCount; i += 2)
        Test_MgbaPrintf("REVIEW_STATE actor=%d move=%S PP=%d/%d/%d/%d Atk=%d Def=%d SpA=%d SpD=%d Spe=%d status=%d ability=%d item=%d", i,
            GetMoveName(gLastMoves[i] < MOVES_COUNT ? gLastMoves[i] : MOVE_NONE), gBattleMons[i].pp[0], gBattleMons[i].pp[1], gBattleMons[i].pp[2], gBattleMons[i].pp[3],
            gBattleMons[i].statStages[STAT_ATK], gBattleMons[i].statStages[STAT_DEF], gBattleMons[i].statStages[STAT_SPATK], gBattleMons[i].statStages[STAT_SPDEF],
            gBattleMons[i].statStages[STAT_SPEED], gBattleMons[i].status1, GetBattlerAbility(i), gBattleMons[i].item);
    EXPECT(gBattleStruct->aiDelayFrames <= 72);
    for (u32 i = 0; i < gBattlersCount; i += 2)
        Test_MgbaPrintf("REVIEW_PLAYER actor=%d species=%d move=%S PP=%d/%d/%d/%d status=%d", i, gBattleMons[i].species,
            GetMoveName(gLastMoves[i] < MOVES_COUNT ? gLastMoves[i] : MOVE_NONE), gBattleMons[i].pp[0], gBattleMons[i].pp[1],
            gBattleMons[i].pp[2], gBattleMons[i].pp[3], gBattleMons[i].status1);
}

static void ReviewTiming(void)
{
    u32 start = gMain.vblankCounter1;
    AssignUsableGimmicks(); SetShellSideArmCategory(); SetAiLogicDataForTurn(gAiLogicData);
    gAiLogicData->reverseBattlerLogicOrder = RandomPercentage(RNG_AI_REVERSE_BATTLER_LOGIC_ORDER,
        GetConfig(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE)) && IsDoubleBattle();
    u32 setup = gMain.vblankCounter1 - start;
    enum BattlerId first = gAiLogicData->reverseBattlerLogicOrder ? B_BATTLER_3 : B_BATTLER_1;
    ComputeAiBattlerDecisions(first); ComputeAiBattlerDecisions(GetPartnerBattler(first));
    u32 complete = gMain.vblankCounter1 - start;
    Test_MgbaPrintf("REVIEW_TIMING setup=%d bothActors=%d complete=%d", setup, complete - setup, complete);
    EXPECT(complete <= 72);
}
