// Temporary native counterfactuals. These are forced commands, not AI evidence.
#line 58000 "work/lyle_opening_control.inc"
DOUBLE_BATTLE_TEST("EC live review Lyle opening control: first-turn attack opportunity")
{
    u32 attack;
    bool32 partnerAttacks;
    PARAMETRIZE { attack = MOVE_PROTECT; partnerAttacks = FALSE; }
    PARAMETRIZE { attack = MOVE_FIRST_IMPRESSION; partnerAttacks = FALSE; }
    PARAMETRIZE { attack = MOVE_LEECH_LIFE; partnerAttacks = FALSE; }
    PARAMETRIZE { attack = MOVE_PROTECT; partnerAttacks = TRUE; }
    PARAMETRIZE { attack = MOVE_FIRST_IMPRESSION; partnerAttacks = TRUE; }
    PARAMETRIZE { attack = MOVE_LEECH_LIFE; partnerAttacks = TRUE; }
    GIVEN {
        LylePivotPlayer(SPECIES_EEVEE, &sLylePivotEevee);
        LylePivotPlayer(SPECIES_TIMBURR, &sLylePivotTimburr);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (attack == MOVE_PROTECT)
                MOVE(opponentLeft, MOVE_PROTECT);
            else
                MOVE(opponentLeft, attack, target: playerLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (partnerAttacks)
                MOVE(opponentRight, MOVE_SEED_BOMB, target: playerLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("LYLE_OPENING_FIXED move=%S partnerAttacks=%d ownHP=%d/%d foeHP=%d/%d firstTurn=%d",
            GetMoveName(attack), partnerAttacks, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, IsBattlersFirstTurn(B_BATTLER_1));
    }
}
