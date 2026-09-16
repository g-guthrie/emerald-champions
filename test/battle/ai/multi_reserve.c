#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

// A two-trainer side shares one party array only in the link and tower multi
// formats. Everywhere else each trainer owns its own array, so the half-team
// index split hides that trainer's own slots three to five. Driving the E0409
// Mossdeep multi found the consequence: a fainted battler whose only live
// reserve sat at slot three was counted as having nothing to switch to, its
// absent flag was never cleared, no replacement was ever requested, and the
// battle ran for hundreds of turns with both opponents struggling at an empty
// side while the outcome stayed ongoing.
MULTI_BATTLE_TEST("EC multi reserve: a fainted lead is replaced from beyond the half-team split")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(1); MaxHP(200); Speed(5); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(1); MaxHP(200); Speed(4); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(1); MaxHP(200); Speed(3); Moves(MOVE_SPLASH); }
        // The only healthy member sits past the split.
        PLAYER(SPECIES_TOGEKISS) { Level(30); HP(200); MaxHP(200); Speed(2); Moves(MOVE_CELEBRATE); }
        PARTNER(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT_A(SPECIES_MACHOP) { Level(30); HP(200); MaxHP(200); Attack(200); Speed(100); Moves(MOVE_BRICK_BREAK); }
        OPPONENT_B(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(90); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_BRICK_BREAK, target: playerLeft);
            MOVE(opponentRight, MOVE_SPLASH);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            SEND_OUT(playerLeft, 3);
        }
        TURN {
            MOVE(opponentLeft, MOVE_BRICK_BREAK, target: playerLeft);
            MOVE(opponentRight, MOVE_SPLASH);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        // The replacement arrived and the turn after it is an ordinary turn,
        // not a side standing empty while the battle refuses to resolve.
        EXPECT_EQ(playerLeft->species, SPECIES_TOGEKISS);
        EXPECT(playerLeft->hp > 0);
    }
}

// The same index is what the AI's own switch search and the faint handler's
// "has anything to switch to" test both walk, so on a half-team multi every
// trainer on a two-trainer side was blind to its own slots three to five.
// Half teams here, so the old rule applied the split; the parties are still
// separate arrays, which is what decides it.
MULTI_BATTLE_TEST("EC multi reserve: a trainer with its own party reaches every slot on a half-team multi")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(30); Speed(5); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); Speed(4); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); Speed(3); Moves(MOVE_SPLASH); }
        PARTNER(SPECIES_MAGIKARP) { Level(30); Speed(2); Moves(MOVE_SPLASH); }
        PARTNER(SPECIES_MAGIKARP) { Level(30); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT_A(SPECIES_MAGIKARP) { Level(30); Speed(100); Moves(MOVE_SPLASH); }
        OPPONENT_B(SPECIES_MAGIKARP) { Level(30); Speed(90); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(!AreMultiPartiesFullTeams());
        EXPECT_EQ(GetAILastPartyIndex(B_BATTLER_0), PARTY_SIZE);
        EXPECT_EQ(GetAILastPartyIndex(B_BATTLER_1), PARTY_SIZE);
        EXPECT_EQ(GetAILastPartyIndex(B_BATTLER_2), PARTY_SIZE);
        EXPECT_EQ(GetAILastPartyIndex(B_BATTLER_3), PARTY_SIZE);
    }
}
