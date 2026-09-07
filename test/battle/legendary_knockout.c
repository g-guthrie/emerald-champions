#include "global.h"
#include "legendary_signs.h"
#include "test/battle.h"

WILD_BATTLE_TEST("Legendary one-shot: every acquisition root is lost after a knockout or escape")
{
    enum Species species = SPECIES_NONE;
    bool32 escape = FALSE;
    static const enum Species nativeSpecies[] = {
        SPECIES_GROUDON, SPECIES_KYOGRE, SPECIES_RAYQUAZA,
        SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL,
        SPECIES_LATIAS, SPECIES_LATIOS, SPECIES_LUGIA, SPECIES_HO_OH,
        SPECIES_MEW, SPECIES_DEOXYS, SPECIES_JIRACHI, SPECIES_DIANCIE,
        SPECIES_HEATRAN, SPECIES_MOLTRES,
    };
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        PARAMETRIZE { species = gLegendarySignDefinitions[id].species; escape = FALSE; }
        PARAMETRIZE { species = gLegendarySignDefinitions[id].species; escape = TRUE; }
    }
    for (u32 nativeIndex = 0; nativeIndex < ARRAY_COUNT(nativeSpecies); nativeIndex++)
    {
        PARAMETRIZE { species = nativeSpecies[nativeIndex]; escape = FALSE; }
        PARAMETRIZE { species = nativeSpecies[nativeIndex]; escape = TRUE; }
    }
    GIVEN {
        memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
        PLAYER(SPECIES_PANGORO) { Speed(200); Ability(ABILITY_SCRAPPY); }
        OPPONENT(species) { HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        if (escape)
            TURN { USE_ITEM(player, ITEM_POKE_DOLL); }
        else
            TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        if (!escape)
            HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT(IsLegendaryEncounterLost(species));
        EXPECT(!IsLegendarySignCaught(GetLegendarySignIdBySpecies(species)));
    }
}

SINGLE_BATTLE_TEST("Legendary one-shot: trainer-owned legendaries do not remove wild encounters")
{
    GIVEN {
        memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_MEWTWO) { HP(1); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT(!IsLegendaryEncounterLost(SPECIES_MEWTWO));
    }
}

WILD_BATTLE_TEST("Legendary one-shot: Transform does not turn an ordinary wild faint into a lost legendary")
{
    GIVEN {
        memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
        PLAYER(SPECIES_MEWTWO) { Speed(200); }
        OPPONENT(SPECIES_DITTO) { HP(1); Speed(1); Moves(MOVE_TRANSFORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRANSFORM, opponent);
        HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT(!IsLegendaryEncounterLost(SPECIES_MEWTWO));
    }
}

WILD_BATTLE_TEST("Legendary one-shot: capture records success instead of a lost encounter")
{
    GIVEN {
        memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
        PLAYER(SPECIES_PANGORO);
        OPPONENT(SPECIES_MEWTWO);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_MASTER_BALL); }
    } THEN {
        EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_MEWTWO));
        EXPECT(!IsLegendaryEncounterLost(SPECIES_MEWTWO));
    }
}
