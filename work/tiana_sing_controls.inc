// Temporary native Sing mechanics, not unrestricted AI choice certification.
#line 26000 "work/tiana_sing_controls.inc"

static const struct EmeraldChampionsBattleSet sTianaSingSnivy = {
    .moves = {MOVE_TACKLE, MOVE_SUBSTITUTE, MOVE_GROWTH, MOVE_PROTECT},
    .item = ITEM_SAFETY_GOGGLES, .nature = NATURE_BOLD, .ability = ABILITY_OVERGROW,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sTianaSingWhismur = {
    .moves = {MOVE_POUND, MOVE_SUBSTITUTE, MOVE_SLEEP_TALK, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_SOUNDPROOF,
    .statPoints = {32, 0, 32, 0, 2, 0},
};

DOUBLE_BATTLE_TEST("EC live review Tiana Sing controls: native Grass Goggles Substitute sound and miss")
{
    u32 control;
    PARAMETRIZE { control = 0; } // Grass + Goggles do not block Sing.
    PARAMETRIZE { control = 1; } // A real Substitute also does not block Sing.
    PARAMETRIZE { control = 2; } // Soundproof does block Sing.
    PARAMETRIZE { control = 3; } // Explicit miss branch: Sing is not Spore's accuracy.
    GIVEN {
        TianaProbePlayer(SPECIES_PACHIRISU, &sTianaProbePachirisu);
        if (control == 2)
            TianaProbePlayer(SPECIES_WHISMUR, &sTianaSingWhismur);
        else
            TianaProbePlayer(SPECIES_SNIVY, &sTianaSingSnivy);
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_GT(GetMonData(&enemies[0], MON_DATA_SPEED), GetMonData(&players[1], MON_DATA_SPEED));
        // Snivy is a regional starter. Whismur is deliberately only a later-
        // access native mechanic control, not claimed reachable before Tiana.
        // All stats come from native level-14 generation and preparation.
    } WHEN {
        if (control == 1)
        {
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_SUBSTITUTE);
                MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
                MOVE(opponentRight, MOVE_PROTECT);
            }
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, control == 2 ? MOVE_POUND : MOVE_TACKLE,
                target: opponentLeft, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_SING, target: playerRight, hit: control != 3);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        if (control < 2)
            NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight); }
        else
            ANIMATION(ANIM_TYPE_MOVE, control == 2 ? MOVE_POUND : MOVE_TACKLE, playerRight);
    } THEN {
        Test_MgbaPrintf("TIANA_SING control=%d target=%d speed=%d sleep=%d substitute=%d item=%d SkittyHP=%d/%d targetLAST=%S",
            control, playerRight->species, playerRight->speed,
            playerRight->status1 & STATUS1_SLEEP, playerRight->volatiles.substitute,
            playerRight->item, opponentLeft->hp, opponentLeft->maxHP,
            GetMoveName(gLastMoves[B_BATTLER_2]));
        if (control < 2)
        {
            EXPECT(playerRight->status1 & STATUS1_SLEEP);
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(playerRight->item, ITEM_SAFETY_GOGGLES);
        }
        else
        {
            EXPECT_EQ(playerRight->status1 & STATUS1_SLEEP, 0);
            EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        }
        if (control == 1)
            EXPECT(playerRight->volatiles.substitute);
        // Forced hit/miss branches prove mechanics, not empirical 55% frequency.
    }
}
