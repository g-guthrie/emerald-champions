// TEMPORARY DIAGNOSTIC ONLY. Include after AuthoredOpponent() in
// test/battle/ai/emerald_champions_plans.c. Do not make a permanent suite.
// Uses the compiled Rick party and actual native stat calculation. Case 0
// expects a useful same-turn Electroweb crossing; case 1 removes the crossing;
// case 2 makes Electroweb ineffective against two legitimately available
// Electric-immune abilities. Log target/action choices even for controls.
#line 4000 "work/rick_ai_probe.inc"
#include "emerald_champions_battle_sets.h"

static void RickProbePlayer(enum Species species, u8 nature,
                            enum Ability ability, enum Item item,
                            const u8 *points, const u16 *moves)
{
    struct Pokemon mon;
    struct EmeraldChampionsBattleSet set = { .nature = nature, .ability = ability, .item = item };
    memcpy(set.moves, moves, sizeof(set.moves));
    memcpy(set.evs, points, sizeof(set.evs));
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        // Required: the test runner otherwise invents speeds from TURN order.
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rick: actual-party speed crossing and immune controls")
{
    u32 control, turns;
    PARAMETRIZE { control = 0; turns = 1; }
    PARAMETRIZE { control = 1; turns = 1; }
    PARAMETRIZE { control = 1; turns = 2; }
    PARAMETRIZE { control = 2; turns = 1; }
    PARAMETRIZE { control = 3; turns = 1; }
    GIVEN {
        static const u8 charCross[NUM_STATS] = {0, 0, 0, 252, 104, 152};
        static const u8 birdCross[NUM_STATS] = {168, 252, 0, 0, 0, 88};
        static const u8 specialSlow[NUM_STATS] = {252, 0, 0, 252, 4, 0};
        static const u8 physicalSlow[NUM_STATS] = {252, 252, 4, 0, 0, 0};
        static const u8 squirrelPoints[NUM_STATS] = {252, 0, 160, 0, 0, 96};
        static const u8 dogPoints[NUM_STATS] = {88, 0, 0, 252, 0, 168};
        static const u16 charMoves[MAX_MON_MOVES] = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT};
        static const u16 birdMoves[MAX_MON_MOVES] = {MOVE_BRAVE_BIRD, MOVE_FACADE, MOVE_QUICK_ATTACK, MOVE_PROTECT};
        static const u16 squirrelMoves[MAX_MON_MOVES] = {MOVE_THUNDERBOLT, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT};
        static const u16 dogMoves[MAX_MON_MOVES] = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT};
        if (control >= 2)
        {
            RickProbePlayer(SPECIES_PACHIRISU, NATURE_CALM, ABILITY_VOLT_ABSORB, ITEM_SITRUS_BERRY, squirrelPoints, squirrelMoves);
            // Electrike is a current Route103 catch; the old Blitzle fixture
            // incorrectly treated the Route110 species as available here.
            RickProbePlayer(SPECIES_ELECTRIKE, NATURE_MODEST, ABILITY_LIGHTNING_ROD, ITEM_EVIOLITE, dogPoints, dogMoves);
            if (control == 3)
            {
                u32 hp = 20;
                SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_HP, &hp);
            }
        }
        else
        {
            RickProbePlayer(SPECIES_CHARMANDER, NATURE_MODEST, ABILITY_BLAZE, ITEM_CHARCOAL,
                            control == 0 ? charCross : specialSlow, charMoves);
            RickProbePlayer(SPECIES_TAILLOW, NATURE_JOLLY, ABILITY_GUTS, ITEM_SHARP_BEAK,
                            control == 0 ? birdCross : physicalSlow, birdMoves);
        }
        AuthoredOpponent(TRAINER_RICK, 0, FALSE);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        {
            u32 size = owner == B_TRAINER_PLAYER ? 2 : 4;
            for (u32 slot = 0; slot < size; slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("RICK_INPUT c=%d owner=%d slot=%d sp=%d lv=%d hp=%d atk=%d def=%d spa=%d spd=%d spe=%d item=%d ability=%d",
                    control, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
        }
        // Old flat-stat speed-crossing assertions are superseded. Inspect
        // the native input speeds and real actions; no old matchup is forced.
    } WHEN {
        TURN {
            if (control >= 2)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft);
                MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight);
                NOT_EXPECT_MOVE(opponentLeft, MOVE_ELECTROWEB);
            }
            else
            {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft);
                MOVE(playerRight, MOVE_BRAVE_BIRD, target: opponentLeft);
                // A native speed crossing is a premise, not proof that Web
                // beats every attack/target. Read the decision trace first.
                // No blanket ban in control 1: Electroweb can still be good
                // damage. It must not receive fictitious turn-order value.
            }
        }
        if (turns == 2)
            TURN {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft);
                MOVE(playerRight, MOVE_BRAVE_BIRD, target: opponentLeft);
            }
    } THEN {
        for (u32 battler = 0; battler < gBattlersCount; battler++)
            // The runner may already have selected turn 2. Only gLastMoves
            // describes execution; target decisions come from the parent's
            // temporary decision-time logger, not chosenTarget at this point.
            Test_MgbaPrintf("RICK_AFTER c=%d b=%d executedMove=%d currentParty=%d hp=%d speedStage=%d",
                control, battler, gLastMoves[battler], gBattlerPartyIndexes[battler],
                gBattleMons[battler].hp, gBattleMons[battler].statStages[STAT_SPEED]);
    }
}
