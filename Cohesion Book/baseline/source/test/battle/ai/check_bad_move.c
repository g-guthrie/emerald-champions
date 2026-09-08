#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "move.h"

AI_SINGLE_BATTLE_TEST("AI will not try to lower opposing stats if target is protected by it's ability")
{
    enum Ability ability;
    enum Species species;
    enum Move move;

    PARAMETRIZE { ability = ABILITY_SPEED_BOOST;  species = SPECIES_TORCHIC; move = MOVE_SCARY_FACE; }
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; species = SPECIES_KRABBY;  move = MOVE_GROWL; }
    PARAMETRIZE { ability = ABILITY_BIG_PECKS;    species = SPECIES_PIDGEY;  move = MOVE_SCREECH; }
    PARAMETRIZE { ability = ABILITY_ILLUMINATE;   species = SPECIES_STARYU;  move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { ability = ABILITY_KEEN_EYE;     species = SPECIES_PIDGEY;  move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { ability = ABILITY_CONTRARY;     species = SPECIES_SNIVY;   move = MOVE_NOBLE_ROAR; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY;   species = SPECIES_BELDUM;  move = MOVE_NOBLE_ROAR; }

    GIVEN {
        WITH_CONFIG(B_ILLUMINATE_EFFECT, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, move); }
    } WHEN {
        TURN { SCORE_LT_VAL(opponent, move, AI_SCORE_DEFAULT); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not try to lower opposing stats if target is protected by Flower Veil")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_SCARY_FACE; }
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_SCREECH; }
    PARAMETRIZE { move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { move = MOVE_NOBLE_ROAR; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_COMFEY) { Ability(ABILITY_FLOWER_VEIL); }
        PLAYER(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SCORE_LT_VAL(opponentLeft, move, AI_SCORE_DEFAULT, target: playerRight); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI avoids Mind Reader and Lock-On while any target is locked on")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_MIND_READER; }
    PARAMETRIZE { move = MOVE_LOCK_ON; }

    GIVEN {
        ASSUME(GetMoveEffect(move) == EFFECT_LOCK_ON);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_DOUBLES_MOVE, SCORE_TIE_LO, 0);
        TIE_BREAK_TARGET(TARGET_TIE_LO, 0);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, move, target: playerLeft); }
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_SCRATCH);
            SCORE_LT_VAL(opponentLeft, move, AI_SCORE_DEFAULT, target: playerLeft);
            SCORE_LT_VAL(opponentLeft, move, AI_SCORE_DEFAULT, target: playerRight);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI attacks the target of its active Mind Reader or Lock-On")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_MIND_READER; }
    PARAMETRIZE { move = MOVE_LOCK_ON; }

    GIVEN {
        ASSUME(GetMoveEffect(move) == EFFECT_LOCK_ON);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_DOUBLES_MOVE, SCORE_TIE_LO, 0);
        TIE_BREAK_TARGET(TARGET_TIE_LO, 0);
        AI_FLAGS(AI_FLAG_SMART_TRAINER | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, move, target: playerLeft); }
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_HYDRO_PUMP, target: playerLeft);
            SCORE_GT_VAL(opponentLeft, MOVE_HYDRO_PUMP, AI_SCORE_DEFAULT + BEST_DAMAGE_MOVE, target: playerLeft);
            SCORE_EQ_VAL(opponentLeft, MOVE_HYDRO_PUMP, AI_SCORE_DEFAULT + BEST_DAMAGE_MOVE, target: playerRight);
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI sees No Guard affects semi-invulnerable moves")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_PHANTOM_FORCE) == EFFECT_SEMI_INVULNERABLE);
        ASSUME(GetMovePower(MOVE_PHANTOM_FORCE) == GetMovePower(MOVE_SPECTRAL_THIEF));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GOLURK) { Ability(ABILITY_NO_GUARD); Moves(MOVE_DYNAMIC_PUNCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SMEARGLE) { Moves(MOVE_PHANTOM_FORCE, MOVE_SPECTRAL_THIEF); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SPECTRAL_THIEF); }
    }
}

AI_SINGLE_BATTLE_TEST("AI predicts semi-invulnerable entry and chooses a move that can still hit")
{
    enum Move playerMove, expectedMove = MOVE_NONE;

    PARAMETRIZE { playerMove = MOVE_WATER_GUN; expectedMove = MOVE_THUNDERBOLT; }
    PARAMETRIZE { playerMove = MOVE_DIVE;      expectedMove = MOVE_SURF; } // Faster Dive should make AI avoid moves that miss underwater

    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DIVE) == EFFECT_SEMI_INVULNERABLE);
        ASSUME(GetTwoTurnMoveSemiInvulnerability(MOVE_DIVE) == STATE_UNDERWATER);
        ASSUME(!MoveDamagesUnderWater(MOVE_THUNDERBOLT));
        ASSUME(MoveDamagesUnderWater(MOVE_SURF));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_MAGIKARP) { Speed(2); Moves(playerMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_THUNDERBOLT, MOVE_SURF); }
    } WHEN {
        TURN {
            MOVE(player, playerMove);
            EXPECT_MOVE(opponent, expectedMove);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Protect: AI avoids Protect vs Unseen Fist contact (Single)")
{
    static const enum Move protectMoves[] =
    {
        MOVE_PROTECT,
        MOVE_DETECT,
        MOVE_SPIKY_SHIELD,
        MOVE_KINGS_SHIELD,
        MOVE_BANEFUL_BUNKER,
        MOVE_BURNING_BULWARK,
        MOVE_OBSTRUCT,
        MOVE_SILK_TRAP,
    };
    enum Species species = SPECIES_NONE;
    enum Ability ability = ABILITY_NONE;
    enum Move protectMove = MOVE_NONE;
    bool32 shouldProtect = FALSE;

    for (u32 paramIdx = 0; paramIdx < ARRAY_COUNT(protectMoves); paramIdx++)
    {
        PARAMETRIZE { species = SPECIES_PIKACHU; ability = ABILITY_STATIC;      shouldProtect = TRUE;  protectMove = protectMoves[paramIdx]; }
        PARAMETRIZE { species = SPECIES_URSHIFU; ability = ABILITY_UNSEEN_FIST; shouldProtect = FALSE; protectMove = protectMoves[paramIdx]; }
    }

    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        ASSUME(GetMoveEffect(protectMove) == EFFECT_PROTECT);
        ASSUME(MoveMakesContact(MOVE_TACKLE));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(species) { Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(protectMove, MOVE_SCRATCH); }
    } WHEN {
        if (shouldProtect)
        {
            TURN {
                MOVE(player, MOVE_TACKLE);
                SCORE_GT(opponent, protectMove, MOVE_SCRATCH);
            }
        }
        else
        {
            TURN {
                MOVE(player, MOVE_TACKLE);
                SCORE_LT(opponent, protectMove, MOVE_SCRATCH);
            }
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Protect: AI avoids Protect vs Unseen Fist contact (Doubles)")
{
    static const enum Move protectMoves[] =
    {
        MOVE_PROTECT,
        MOVE_DETECT,
        MOVE_SPIKY_SHIELD,
        MOVE_KINGS_SHIELD,
        MOVE_BANEFUL_BUNKER,
        MOVE_BURNING_BULWARK,
        MOVE_OBSTRUCT,
        MOVE_SILK_TRAP,
    };
    enum Species species = SPECIES_NONE;
    enum Ability ability = ABILITY_NONE;
    enum Move protectMove = MOVE_NONE;
    bool32 shouldProtect = FALSE;

    for (u32 paramIdx = 0; paramIdx < ARRAY_COUNT(protectMoves); paramIdx++)
    {
        PARAMETRIZE { species = SPECIES_PIKACHU; ability = ABILITY_STATIC;      shouldProtect = TRUE;  protectMove = protectMoves[paramIdx]; }
        PARAMETRIZE { species = SPECIES_URSHIFU; ability = ABILITY_UNSEEN_FIST; shouldProtect = FALSE; protectMove = protectMoves[paramIdx]; }
    }

    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        ASSUME(GetMoveEffect(protectMove) == EFFECT_PROTECT);
        ASSUME(MoveMakesContact(MOVE_TACKLE));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(species) { Ability(ability); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(protectMove, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        if (shouldProtect)
        {
            TURN {
                MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
                MOVE(playerRight, MOVE_CELEBRATE);
                SCORE_GT(opponentLeft, protectMove, MOVE_SCRATCH, target: playerLeft);
            }
        }
        else
        {
            TURN {
                MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
                MOVE(playerRight, MOVE_CELEBRATE);
                SCORE_LT(opponentLeft, protectMove, MOVE_SCRATCH, target: playerLeft);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Protect: AI avoids Protect vs moves that ignore protection (Single)")
{
    enum Move move = MOVE_NONE;
    bool32 shouldProtect = FALSE;

    PARAMETRIZE { move = MOVE_TACKLE; shouldProtect = TRUE; }
    PARAMETRIZE { move = MOVE_FEINT; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_SHADOW_FORCE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_PHANTOM_FORCE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_HYPERSPACE_HOLE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_HYPERSPACE_FURY; shouldProtect = FALSE; }

    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        if (shouldProtect)
            ASSUME(!MoveIgnoresProtect(move));
        else
            ASSUME(MoveIgnoresProtect(move));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT, MOVE_SCRATCH); }
    } WHEN {
        TURN {
            MOVE(player, move);
            if (shouldProtect)
                SCORE_GT(opponent, MOVE_PROTECT, MOVE_SCRATCH);
            else
                SCORE_LT(opponent, MOVE_PROTECT, MOVE_SCRATCH);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Protect: AI avoids Protect vs moves that ignore protection (Doubles)")
{
    enum Move move = MOVE_NONE;
    bool32 shouldProtect = FALSE;

    PARAMETRIZE { move = MOVE_TACKLE; shouldProtect = TRUE; }
    PARAMETRIZE { move = MOVE_FEINT; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_SHADOW_FORCE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_PHANTOM_FORCE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_HYPERSPACE_HOLE; shouldProtect = FALSE; }
    PARAMETRIZE { move = MOVE_HYPERSPACE_FURY; shouldProtect = FALSE; }

    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        if (shouldProtect)
            ASSUME(!MoveIgnoresProtect(move));
        else
            ASSUME(MoveIgnoresProtect(move));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (shouldProtect)
                SCORE_GT(opponentLeft, MOVE_PROTECT, MOVE_SCRATCH, target: playerLeft);
            else
                SCORE_LT(opponentLeft, MOVE_PROTECT, MOVE_SCRATCH, target: playerLeft);
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI penalizes Yawn when target can self-status with Flame/Toxic Orb")
{
    enum Item heldItem = ITEM_NONE;
    bool32 shouldYawn = FALSE;

    PARAMETRIZE { heldItem = ITEM_NONE;      shouldYawn = TRUE; }
    PARAMETRIZE { heldItem = ITEM_FLAME_ORB; shouldYawn = FALSE; }
    PARAMETRIZE { heldItem = ITEM_TOXIC_ORB; shouldYawn = FALSE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_YAWN) == EFFECT_YAWN);
        ASSUME(gItemsInfo[ITEM_FLAME_ORB].holdEffect == HOLD_EFFECT_FLAME_ORB);
        ASSUME(gItemsInfo[ITEM_TOXIC_ORB].holdEffect == HOLD_EFFECT_TOXIC_ORB);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(heldItem); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_YAWN, MOVE_SCRATCH); }
    } WHEN {
        TURN {
            if (shouldYawn)
                SCORE_GT(opponent, MOVE_YAWN, MOVE_SCRATCH);
            else
                SCORE_LT(opponent, MOVE_YAWN, MOVE_SCRATCH);
        }
    }
}
AI_SINGLE_BATTLE_TEST("AI rejects impossible status moves without penalizing legal ones")
{
    static const struct {
        enum Move move;
        enum Species species;
        enum Ability ability;
    } cases[] = {
        { MOVE_TOXIC, SPECIES_SNORLAX, ABILITY_IMMUNITY },
        { MOVE_TOXIC, SPECIES_BULBASAUR, ABILITY_OVERGROW },
        { MOVE_HYPNOSIS, SPECIES_HOOTHOOT, ABILITY_INSOMNIA },
        { MOVE_HYPNOSIS, SPECIES_TAPU_FINI, ABILITY_MISTY_SURGE },
        { MOVE_WILL_O_WISP, SPECIES_BUIZEL, ABILITY_WATER_VEIL },
        { MOVE_WILL_O_WISP, SPECIES_CHARMANDER, ABILITY_BLAZE },
        { MOVE_THUNDER_WAVE, SPECIES_HITMONLEE, ABILITY_LIMBER },
        { MOVE_THUNDER_WAVE, SPECIES_PIKACHU, ABILITY_STATIC },
    };
    u32 sample = 0;
    bool32 immune = FALSE;
    for (u32 caseIndex = 0; caseIndex < ARRAY_COUNT(cases); caseIndex++)
        for (u32 immuneCase = 0; immuneCase < 2; immuneCase++)
            PARAMETRIZE { sample = caseIndex; immune = immuneCase; }

    GIVEN {
        // Isolate legality, not a preferred strategy: ties select the status
        // move in slot zero. A missing immunity check then chooses it and fails.
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_SINGLES, SCORE_TIE_LO, 0);
        PLAYER(immune ? cases[sample].species : SPECIES_WOBBUFFET) {
            Ability(immune ? cases[sample].ability : ABILITY_SHADOW_TAG);
            Moves(MOVE_TACKLE, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(cases[sample].move, MOVE_TACKLE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            EXPECT_MOVE(opponent, immune ? MOVE_TACKLE : cases[sample].move);
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI distinguishes legal and illegal item exchanges")
{
    static const struct {
        enum Move move;
        enum Item userItem, targetItem;
        enum Ability targetAbility;
        bool32 substitute, allowed;
    } cases[] = {
        { MOVE_TRICK, ITEM_ORAN_BERRY, ITEM_LEFTOVERS, ABILITY_SHADOW_TAG, FALSE, TRUE },
        { MOVE_BESTOW, ITEM_ORAN_BERRY, ITEM_NONE, ABILITY_SHADOW_TAG, FALSE, TRUE },
        { MOVE_TRICK, ITEM_NONE, ITEM_NONE, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_BESTOW, ITEM_NONE, ITEM_NONE, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_BESTOW, ITEM_ORAN_BERRY, ITEM_LEFTOVERS, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_TRICK, ITEM_ORANGE_MAIL, ITEM_NONE, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_TRICK, ITEM_ORAN_BERRY, ITEM_ORANGE_MAIL, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_BESTOW, ITEM_ORANGE_MAIL, ITEM_NONE, ABILITY_SHADOW_TAG, FALSE, FALSE },
        { MOVE_TRICK, ITEM_ORAN_BERRY, ITEM_LEFTOVERS, ABILITY_STICKY_HOLD, FALSE, FALSE },
        { MOVE_TRICK, ITEM_ORAN_BERRY, ITEM_LEFTOVERS, ABILITY_SHADOW_TAG, TRUE, FALSE },
        { MOVE_BESTOW, ITEM_ORAN_BERRY, ITEM_NONE, ABILITY_SHADOW_TAG, TRUE, FALSE },
    };
    u32 sample = 0;
    for (u32 caseIndex = 0; caseIndex < ARRAY_COUNT(cases); caseIndex++)
        PARAMETRIZE { sample = caseIndex; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_SINGLES, SCORE_TIE_LO, 0);
        PLAYER(SPECIES_WOBBUFFET) {
            Speed(20);
            Item(cases[sample].targetItem);
            Ability(cases[sample].targetAbility);
            Moves(MOVE_SUBSTITUTE, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Speed(1);
            Attack(1);
            Item(cases[sample].userItem);
            Moves(cases[sample].move, MOVE_TACKLE);
        }
    } WHEN {
        if (cases[sample].substitute)
            TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            EXPECT_MOVE(opponent, cases[sample].allowed ? cases[sample].move : MOVE_TACKLE);
        }
    }
}
