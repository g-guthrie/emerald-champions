// Temporary forced native revenge-window comparison, not AI-choice evidence.
#line 52000 "work/cindy_retaliate_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CindyRetaliatePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

DOUBLE_BATTLE_TEST("EC live review Cindy Retaliate: actual ally faint opens native revenge window", s16 damage, u16 slakothHp)
{
    bool32 retaliate;
    PARAMETRIZE { retaliate = FALSE; }
    PARAMETRIZE { retaliate = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_HELPING_HAND, MOVE_ROUND, MOVE_SUPER_FANG, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        CindyRetaliatePlayer(SPECIES_CHARMANDER, &charmander);
        CindyRetaliatePlayer(SPECIES_PACHIRISU, &pachirisu);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MILCERY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_FURFROU);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_CHOICE_BAND);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_TRUANT);
        // Retain Body Slam in both cases; only the proposed Night Slash slot
        // changes. All four original records start alive. No timer is injected.
        u16 move = retaliate ? MOVE_RETALIATE : MOVE_NIGHT_SLASH;
        u8 pp = GetMovePP(move);
        SetMonData(&party[3], MON_DATA_MOVE3, &move);
        SetMonData(&party[3], MON_DATA_PP3, &pp);
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_HELPING_HAND, target: playerLeft);
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerRight);
            MOVE(opponentRight, MOVE_RECOVER);
            SEND_OUT(opponentLeft, 3);
        }
        TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, retaliate ? MOVE_RETALIATE : MOVE_BODY_SLAM, target: playerRight,
                 hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_RECOVER);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, playerLeft);
        HP_BAR(opponentLeft, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, retaliate ? MOVE_RETALIATE : MOVE_BODY_SLAM, opponentLeft);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 0);
        EXPECT_EQ(opponentLeft->species, SPECIES_SLAKOTH);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 3);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], retaliate ? MOVE_RETALIATE : MOVE_BODY_SLAM);
        EXPECT_EQ(gBattleStruct->choicedMove[B_BATTLER_1], retaliate ? MOVE_RETALIATE : MOVE_BODY_SLAM);
        results[i].slakothHp = opponentLeft->hp;
        Test_MgbaPrintf("CINDY_RETALIATE retaliate=%d nativeTurns=%d damage=%d SlakothHP=%d MilceryHP=%d CharmHP=%d PachiHP=%d timerAfter=%d choice=%S",
            retaliate, gBattleResults.battleTurnCounter, results[i].damage, results[i].slakothHp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, gSideTimers[B_SIDE_OPPONENT].retaliateTimer,
            GetMoveName(gBattleStruct->choicedMove[B_BATTLER_1]));
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
        EXPECT_EQ(results[1].slakothHp, results[0].slakothHp);
    }
}
