// Temporary actual E0023 native observations; independent of prior encounters.
#line 61000 "work/guard_audit.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"
#include "emerald_champions_opening.h"
#include "constants/emerald_champions.h"
#include "constants/vars.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("EC live review Protect reset: failed attempt resets odds but remains the last move")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_CELEBRATE); }
        if (turns >= 2)
            TURN { MOVE(player, MOVE_PROTECT, WITH_RNG(RNG_PROTECT_FAIL, 2)); MOVE(opponent, MOVE_CELEBRATE); }
        if (turns >= 3)
            TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ((u32)player->volatiles.consecutiveMoveUses, turns == 2 ? 0 : 1);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_PROTECT);
        EXPECT_EQ(gLastResultingMoves[B_BATTLER_0], turns == 2 ? MOVE_UNAVAILABLE : MOVE_PROTECT);
        EXPECT_EQ(GetConsecutiveMoveSuccessDenominator(player->volatiles.consecutiveMoveUses), turns == 2 ? 1 : 3);
    }
}

static void GuardAuditPlayerAtLevel(enum Species species, const struct EmeraldChampionsBattleSet *set, u32 level)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
        if (set->moves[index] != MOVE_NONE)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[index]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static void GuardAuditPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    GuardAuditPlayerAtLevel(species, set, 14);
}

static const struct EmeraldChampionsBattleSet sGuardAuditEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sGuardAuditTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sGuardAuditPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sGuardAuditLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sGuardAuditMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



AI_DOUBLE_BATTLE_TEST("EC live review Protect audit opening: actual first trainers sustained pressure")
{
    u16 trainer; u32 turns;
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_MUDKIP; turns = 2; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_MUDKIP; turns = 4; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TORCHIC; turns = 2; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TORCHIC; turns = 3; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TREECKO; turns = 2; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TREECKO; turns = 4; }
    PARAMETRIZE { trainer = TRAINER_CALVIN_1; turns = 2; }
    PARAMETRIZE { trainer = TRAINER_CALVIN_1; turns = 4; }
    PARAMETRIZE { trainer = TRAINER_RICK; turns = 2; }
    PARAMETRIZE { trainer = TRAINER_RICK; turns = 4; }
    GIVEN {
        struct EmeraldChampionsBattleSet eevee = sGuardAuditEevee;
        eevee.moves[2] = MOVE_WISH;
        GuardAuditPlayer(SPECIES_PACHIRISU, &sGuardAuditPachirisu);
        GuardAuditPlayer(SPECIES_EEVEE, &eevee);
        AuthoredOpponent(trainer, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
            if ((turn & 1) == 0)
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerRight, MOVE_WISH);
        }
    } THEN {
        Test_MgbaPrintf("GUARD_AUDIT trainer=%d turns=%d HP=%d/%d foes=%d/%d last=%S/%S streaks=%d/%d resulting=%S/%S",
            trainer, turns, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
            opponentLeft->volatiles.consecutiveMoveUses, opponentRight->volatiles.consecutiveMoveUses,
            GetMoveName(gLastResultingMoves[1]), GetMoveName(gLastResultingMoves[3]));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Protect audit dead end: lone injured rival Mudkip without a guard payoff")
{
    GIVEN {
        GuardAuditPlayer(SPECIES_PACHIRISU, &sGuardAuditPachirisu);
        GuardAuditPlayer(SPECIES_EEVEE, &sGuardAuditEevee);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_TORCHIC, 0, FALSE);
        u16 zero = 0, one = 1;
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_MUDKIP);
        SetMonData(&party[0], MON_DATA_HP, &one);
        for (u32 slot = 1; slot < 4; slot++)
            SetMonData(&party[slot], MON_DATA_HP, &zero);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_HP, &zero);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE); SKIP_TURN(playerRight); }
    } THEN {
        Test_MgbaPrintf("GUARD_DEAD_END HP=%d foe=%d last=%S counter=%d turns=%d", opponentLeft->hp, playerLeft->hp,
            GetMoveName(gLastMoves[1]), opponentLeft->volatiles.consecutiveMoveUses, gBattleResults.battleTurnCounter);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Protect audit rescue: native rescue factory and fresh starter stats")
{
    u32 pair, turns;
    PARAMETRIZE { pair = 0; turns = 1; }
    PARAMETRIZE { pair = 0; turns = 2; }
    PARAMETRIZE { pair = 1; turns = 1; }
    PARAMETRIZE { pair = 1; turns = 2; }
    PARAMETRIZE { pair = 2; turns = 1; }
    PARAMETRIZE { pair = 2; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet torchic = {
            .moves = {MOVE_FLAMETHROWER, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_SPEED_BOOST,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        enum Species first = pair == 0 ? SPECIES_MUDKIP : SPECIES_TREECKO;
        enum Species second = pair == 2 ? SPECIES_MUDKIP : SPECIES_TORCHIC;
        GuardAuditPlayerAtLevel(first, &gEmeraldChampionsDefaultBattleSets[first], 5);
        GuardAuditPlayerAtLevel(second, second == SPECIES_TORCHIC ? &torchic : &gEmeraldChampionsDefaultBattleSets[second], 5);
        struct Pokemon *saved = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
        memcpy(saved, gParties[B_TRAINER_OPPONENT_A], sizeof(struct Pokemon) * PARTY_SIZE);
        u32 savedCount = gPartiesCount[B_TRAINER_OPPONENT_A];
        CreateEmeraldChampionsBirchRescueParty();
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_HP_AWARE | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_NONE;
        gBattleTestRunnerState->data.recordedBattle.battleFlags = BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_DOUBLE;
        gBattleTypeFlags = gBattleTestRunnerState->data.recordedBattle.battleFlags;
        VarSet(VAR_EC_OPENING_STATE, EC_OPENING_PAIR_GRANTED);
        for (u32 slot = 0; slot < 2; slot++) {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            OPPONENT(GetMonData(mon, MON_DATA_SPECIES)) {
                *gBattleTestRunnerState->data.currentMon = *mon;
                Nature(GetNature(mon)); Ability(GetMonAbility(mon)); Speed(GetMonData(mon, MON_DATA_SPEED));
                Moves(GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2), GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
        }
        memcpy(gParties[B_TRAINER_OPPONENT_A], saved, sizeof(struct Pokemon) * PARTY_SIZE);
        gPartiesCount[B_TRAINER_OPPONENT_A] = savedCount;
        Free(saved);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, pair == 0 ? MOVE_LIQUIDATION : MOVE_BULLET_SEED, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, pair == 2 ? MOVE_LIQUIDATION : MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("GUARD_RESCUE pair=%d turns=%d native=%d levels=%d/%d/%d/%d HP=%d/%d foes=%d/%d last=%S/%S counter=%d/%d",
            pair, turns, gBattleResults.battleTurnCounter, playerLeft->level, playerRight->level, opponentLeft->level, opponentRight->level,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
            opponentLeft->volatiles.consecutiveMoveUses, opponentRight->volatiles.consecutiveMoveUses);
    }
}
