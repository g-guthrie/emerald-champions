#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_gimmick.h"
#include "emerald_champions_battle_plan.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "legendary_signs.h"
#include "constants/flags.h"

SINGLE_BATTLE_TEST("One star: restricted reserves hide another Pokemon's Mega option, including fainted reserves")
{
    enum Species star;
    bool32 allowed, fainted;
    PARAMETRIZE { star = SPECIES_DARKRAI; allowed = FALSE; fainted = FALSE; }
    PARAMETRIZE { star = SPECIES_KARTANA; allowed = FALSE; fainted = FALSE; }
    PARAMETRIZE { star = SPECIES_FLUTTER_MANE; allowed = FALSE; fainted = FALSE; }
    PARAMETRIZE { star = SPECIES_GHOLDENGO; allowed = FALSE; fainted = FALSE; }
    PARAMETRIZE { star = SPECIES_URSALUNA_BLOODMOON; allowed = FALSE; fainted = FALSE; }
    PARAMETRIZE { star = SPECIES_DARKRAI; allowed = FALSE; fainted = TRUE; }
    PARAMETRIZE { star = SPECIES_URSALUNA; allowed = TRUE; fainted = FALSE; }
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Item(ITEM_CHARIZARDITE_X); Moves(MOVE_CELEBRATE); }
        PLAYER(star) { HP(fainted ? 0 : 100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(CanMegaEvolve(B_BATTLER_0), allowed);
        EXPECT_EQ(gBattleStruct->gimmick.usableGimmick[B_BATTLER_0], allowed ? GIMMICK_MEGA : GIMMICK_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_CHARIZARDITE_X);
        EXPECT(PlayerPartyWithinRestrictedLimit());
    }
}

SINGLE_BATTLE_TEST("One star: the restricted Pokemon may Mega Evolve itself")
{
    GIVEN {
        PLAYER(SPECIES_DARKRAI) { Item(ITEM_DARKRANITE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD) { Item(ITEM_CHARIZARDITE_X); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_0), GIMMICK_NONE);
        EXPECT(HasTrainerUsedGimmick(B_BATTLER_0, GIMMICK_MEGA));
        EXPECT(!CanMegaEvolve(B_BATTLER_0));
        EXPECT(PlayerPartyWithinRestrictedLimit());
    }
}

SINGLE_BATTLE_TEST("One star: trainer opponents retain their Mega with a restricted reserve")
{
    GIVEN {
        PLAYER(SPECIES_GHOLDENGO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { Item(ITEM_CHARIZARDITE_X); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DARKRAI) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_CHARIZARD_MEGA_X);
    }
}

SINGLE_BATTLE_TEST("One star: the blocked-Mega explanation appears only once")
{
    bool32 heard;
    PARAMETRIZE { heard = FALSE; }
    PARAMETRIZE { heard = TRUE; }
    GIVEN {
        GIVE_PLAYER_ITEM(ITEM_MEGA_RING, 1);
        if (heard)
            FlagSet(FLAG_EC_STAR_RULE_EXPLAINED);
        else
            FlagClear(FLAG_EC_STAR_RULE_EXPLAINED);
        PLAYER(SPECIES_CHARIZARD) { Item(ITEM_CHARIZARDITE_X); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DARKRAI) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        if (heard)
            NOT MESSAGE("Your star blocks other Pokémon\nfrom Mega Evolving.");
        else
            MESSAGE("Your star blocks other Pokémon\nfrom Mega Evolving.");
    } THEN {
        EXPECT(FlagGet(FLAG_EC_STAR_RULE_EXPLAINED));
    }
}

TEST("One star: party-only stars keep ordinary encounter behavior")
{
    EXPECT(!IsLegendaryEncounterSpecies(SPECIES_GHOLDENGO));
    EXPECT(!IsLegendaryEncounterSpecies(SPECIES_URSALUNA_BLOODMOON));
    EXPECT(IsWildSlotSpeciesAcquirable(SPECIES_URSALUNA_BLOODMOON));
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_URSALUNA), RESTRICTED_PARTY_NONE);
}
