#include "global.h"
#include "move.h"
#include "move_relearner.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/test.h"

// Moves of the Pokémon Center move tutor (MOVE_RELEARNER_ALL_LEVEL_UP_MOVES) and
// of the Fallarbor MOVE TUTOR (MOVE_RELEARNER_LEVEL_UP_MOVES).

static bool32 ListHasMove(const u16 *moves, u32 numMoves, enum Move move)
{
    for (u32 i = 0; i < numMoves; i++)
    {
        if (moves[i] == move)
            return TRUE;
    }
    return FALSE;
}

static void SetMoves(struct Pokemon *mon, enum Move m1, enum Move m2, enum Move m3, enum Move m4)
{
    enum Move moves[MAX_MON_MOVES] = {m1, m2, m3, m4};
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        u32 pp = GetMovePP(moves[i]);
        SetMonData(mon, MON_DATA_MOVE1 + i, &moves[i]);
        SetMonData(mon, MON_DATA_PP1 + i, &pp);
    }
}

TEST("Center tutor lists every level-up move of the current species, once, except known moves")
{
    u32 j;
    enum Species species = SPECIES_NONE;
    u8 level = 0;
    u16 moves[MAX_RELEARNER_MOVES];

    PARAMETRIZE { species = SPECIES_MUDKIP;        level = 5;   }
    PARAMETRIZE { species = SPECIES_MARSHTOMP;     level = 16;  }
    PARAMETRIZE { species = SPECIES_RALTS;         level = 1;   }
    PARAMETRIZE { species = SPECIES_GARDEVOIR;     level = 100; }
    PARAMETRIZE { species = SPECIES_DEOXYS_ATTACK; level = 30;  }
    PARAMETRIZE { species = SPECIES_MEW;           level = 50;  }

    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, level, 0, OTID_STRUCT_PRESET(0));
    struct BoxPokemon *boxMon = &gParties[B_TRAINER_PLAYER][0].box;
    u32 numMoves = GetRelearnableMoves(boxMon, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES, moves);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    // Every unknown learnset move, whatever its level, is listed.
    for (j = 0; learnset[j].move != LEVEL_UP_MOVE_END; j++)
    {
        if (BoxMonKnowsMove(boxMon, learnset[j].move))
            EXPECT(!ListHasMove(moves, numMoves, learnset[j].move));
        else
            EXPECT(ListHasMove(moves, numMoves, learnset[j].move));
    }
    // Nothing else is listed, and nothing twice.
    for (j = 0; j < numMoves; j++)
    {
        bool32 inLearnset = FALSE;
        for (u32 k = 0; learnset[k].move != LEVEL_UP_MOVE_END; k++)
        {
            if (learnset[k].move == moves[j])
                inLearnset = TRUE;
        }
        EXPECT(inLearnset);
        EXPECT(!ListHasMove(moves + j + 1, numMoves - j - 1, moves[j]));
    }
    EXPECT_EQ(numMoves > 0, HasMoveToRelearn(boxMon, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES));
}

TEST("Center tutor offers moves above the Pokémon's level")
{
    u16 moves[MAX_RELEARNER_MOVES];
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];

    CreateMon(mon, SPECIES_MUDKIP, 5, 0, OTID_STRUCT_PRESET(0));
    SetMoves(mon, MOVE_TACKLE, MOVE_GROWL, MOVE_WATER_GUN, MOVE_NONE);
    u32 numMoves = GetRelearnableMoves(&mon->box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES, moves);

    EXPECT(ListHasMove(moves, numMoves, MOVE_HYDRO_PUMP));
    EXPECT(ListHasMove(moves, numMoves, MOVE_SURF));
    EXPECT(ListHasMove(moves, numMoves, MOVE_ROCK_SMASH));
    EXPECT(!ListHasMove(moves, numMoves, MOVE_TACKLE));
    EXPECT(!ListHasMove(moves, numMoves, MOVE_GROWL));
    EXPECT(!ListHasMove(moves, numMoves, MOVE_WATER_GUN));
    // Listed in learnset (level) order.
    EXPECT_EQ(moves[0], MOVE_ROCK_SMASH);
    EXPECT_EQ(moves[numMoves - 1], MOVE_HYDRO_PUMP);

    // The Fallarbor relearner keeps its level limit.
    numMoves = GetRelearnableMoves(&mon->box, MOVE_RELEARNER_LEVEL_UP_MOVES, moves);
    EXPECT_EQ(numMoves, 0);
    EXPECT(!HasMoveToRelearn(&mon->box, MOVE_RELEARNER_LEVEL_UP_MOVES));
    EXPECT(HasMoveToRelearn(&mon->box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES));
}

TEST("Center tutor doesn't offer pre-evolution moves; the Fallarbor relearner does")
{
    u16 moves[MAX_RELEARNER_MOVES];
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u32 numMoves;

    ASSUME(P_PRE_EVO_MOVES == TRUE);
    // Surf is a Mudkip level-up move (30) that Marshtomp never learns by level.
    CreateMon(mon, SPECIES_MARSHTOMP, 30, 0, OTID_STRUCT_PRESET(0));
    SetMoves(mon, MOVE_MUD_SHOT, MOVE_WATER_GUN, MOVE_TACKLE, MOVE_GROWL);

    numMoves = GetRelearnableMoves(&mon->box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES, moves);
    EXPECT(!ListHasMove(moves, numMoves, MOVE_SURF));
    EXPECT(ListHasMove(moves, numMoves, MOVE_MUDDY_WATER));
    EXPECT(ListHasMove(moves, numMoves, MOVE_HYDRO_PUMP));

    numMoves = GetRelearnableMoves(&mon->box, MOVE_RELEARNER_LEVEL_UP_MOVES, moves);
    EXPECT(ListHasMove(moves, numMoves, MOVE_SURF));
    EXPECT(ListHasMove(moves, numMoves, MOVE_TAKE_DOWN));
    EXPECT(!ListHasMove(moves, numMoves, MOVE_MUDDY_WATER));
    EXPECT(!ListHasMove(moves, numMoves, MOVE_HYDRO_PUMP));
}

TEST("Center tutor has nothing to teach a Pokémon that knows its whole learnset")
{
    u16 moves[MAX_RELEARNER_MOVES];
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];

    // Magikarp: Splash, Tackle, Flail.
    CreateMon(mon, SPECIES_MAGIKARP, 5, 0, OTID_STRUCT_PRESET(0));
    SetMoves(mon, MOVE_SPLASH, MOVE_TACKLE, MOVE_FLAIL, MOVE_NONE);
    EXPECT_EQ(GetRelearnableMoves(&mon->box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES, moves), 0);
    EXPECT(!HasMoveToRelearn(&mon->box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES));
}

TEST("Center tutor mode is never cycled to outside its script")
{
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MUDKIP, 5, 0, OTID_STRUCT_PRESET(0));
    EXPECT(!CanBoxMonRelearnMoves(&gParties[B_TRAINER_PLAYER][0].box, MOVE_RELEARNER_ALL_LEVEL_UP_MOVES));
}

TEST("Reaching the level of a move the Pokémon already knows doesn't try to teach it again")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u32 level;
    enum Move move = MOVE_NONE;

    for (u32 j = 0; j < 2; j++)
    {
        // Rock Throw is Mudkip's level 9 move. With 3 moves it would be added,
        // with 4 the player would be asked to forget one.
        CreateMon(mon, SPECIES_MUDKIP, 8, 0, OTID_STRUCT_PRESET(0));
        if (j == 0)
            SetMoves(mon, MOVE_TACKLE, MOVE_ROCK_THROW, MOVE_GROWL, MOVE_NONE);
        else
            SetMoves(mon, MOVE_TACKLE, MOVE_GROWL, MOVE_HYDRO_PUMP, MOVE_ROCK_THROW);
        level = 9;
        SetMonData(mon, MON_DATA_LEVEL, &level);

        EXPECT_EQ(MonTryLearningNewMove(mon, TRUE), MON_ALREADY_KNOWS_MOVE);
        EXPECT_EQ(MonTryLearningNewMove(mon, FALSE), MOVE_NONE);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1), MOVE_TACKLE);
        move = (j == 0) ? MOVE_ROCK_THROW : MOVE_GROWL;
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE2), move);
        move = (j == 0) ? MOVE_NONE : MOVE_ROCK_THROW;
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE4), move);
    }
}
