// Temporary actual mixed-pair mechanics and unrestricted observation.
// Include after GinaMiaLeadPlayer. Minun is defeated, not replaced by a new
// species; Volbeat takes its active slot and Illumise remains a live reserve.
#line 57000 "work/gina_mia_nuzzle_probe.inc"
#include "battle_main.h"

static void GinaMiaNuzzleBoard(bool32 cloak)
{
    const struct EmeraldChampionsBattleSet timburr = {
        .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
        .item = cloak ? ITEM_COVERT_CLOAK : ITEM_EVIOLITE,
        .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
        .statPoints = {2, 0, 0, 0, 32, 32},
    };
    GinaMiaLeadPlayer(SPECIES_TIMBURR, &timburr);
    GinaMiaLeadPlayer(SPECIES_PACHIRISU, &sGinaMiaPachirisu);
    AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
    u16 zero = 0;
    SetMonData(&party[1], MON_DATA_HP, &zero);
    struct Pokemon saved = party[1];
    party[1] = party[2];
    party[2] = saved;
    u32 foeSpeed = GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED);
    EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), foeSpeed);
    EXPECT_GT(foeSpeed, GetMonData(&party[1], MON_DATA_SPEED));
    EXPECT_LT(foeSpeed / 2, GetMonData(&party[1], MON_DATA_SPEED));
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("GINA_NUZZLE_INPUT cloak=%d owner=%d slot=%d species=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                cloak, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
        }
}

DOUBLE_BATTLE_TEST("EC live review Gina Nuzzle: native order paralysis and Cloak control", s16 replyDamage, s16 bugDamage)
{
    u32 control;
    PARAMETRIZE { control = 0; } // Nuzzle, successful subsequent action.
    PARAMETRIZE { control = 1; } // Thunderbolt without secondary paralysis.
    PARAMETRIZE { control = 2; } // Nuzzle contact lands; Cloak blocks paralysis.
    PARAMETRIZE { control = 3; } // Separate explicit full-paralysis outcome.
    GIVEN { GinaMiaNuzzleBoard(control == 2); }
    WHEN {
        TURN {
            // Named native RNG control is for deterministic mechanical
            // branches only, never for the unrestricted decision below.
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, control == 3));
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 1)
                MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentLeft, MOVE_NUZZLE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_BUG_BUZZ, target: playerLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, control == 1 ? MOVE_THUNDERBOLT : MOVE_NUZZLE, opponentLeft);
        if (control == 0 || control == 3)
        {
            STATUS_ICON(playerLeft, paralysis: TRUE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BUZZ, opponentRight);
            HP_BAR(playerLeft, captureDamage: &results[i].bugDamage);
            if (control == 0)
            {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
                HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
            }
            else
                MESSAGE("Timburr couldn't move because it's paralyzed!");
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
            HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BUZZ, opponentRight);
            HP_BAR(playerLeft, captureDamage: &results[i].bugDamage);
        }
    } THEN {
        EXPECT_EQ((playerLeft->status1 & STATUS1_PARALYSIS) != 0, control == 0 || control == 3);
        EXPECT_GT(results[i].bugDamage, 0);
        if (control == 3)
            EXPECT_EQ(results[i].replyDamage, 0);
        else
            EXPECT_GT(results[i].replyDamage, 0);
        Test_MgbaPrintf("GINA_NUZZLE_CONTROL control=%d bugDamage=%d replyDamage=%d foeHP=%d/%d ownHP=%d/%d foeSpeed=%d VolbeatSpeed=%d foeStatus=%d LAST=%S/%S foeLAST=%S faints=%d/%d",
            control, results[i].bugDamage, results[i].replyDamage, playerLeft->hp, playerRight->hp,
            opponentLeft->hp, opponentRight->hp,
            GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)),
            playerLeft->status1, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina Nuzzle: unrestricted mixed Plusle Volbeat board")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    PARAMETRIZE { turns = 5; }
    GIVEN { GinaMiaNuzzleBoard(FALSE); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("GINA_NUZZLE_AI turns=%d active=%d/%d slots=%d/%d ownHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownSpA=%d/%d foeStatus=%d/%d faints=%d/%d",
            gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            playerLeft->status1, playerRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // No prescribed support or setup: attacking or switching may be good.
    }
}
