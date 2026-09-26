#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "pokemon.h"
#include "trainer_util.h"
#include "party_menu.h"
#include "string_util.h"
#include "text.h"
#include "test/battle.h"

// The Inclement layer (src/data/pokemon/inclement_layer.h) is read by wild and
// player-owned Pokemon; trainer-owned Pokemon read gSpeciesInfo only.

static u32 ExpectedSpeciesStat(struct Pokemon *mon, enum Species species, enum Stat stat, bool32 trainerOwned)
{
    return CalculateSpeciesStatForOwner(species,
                                        GetMonData(mon, MON_DATA_HIDDEN_NATURE),
                                        stat,
                                        GetMonData(mon, MON_DATA_LEVEL),
                                        GetMonData(mon, MON_DATA_HP_EV + stat),
                                        GetMonData(mon, MON_DATA_HP_IV + stat),
                                        trainerOwned);
}

static u32 ExpectedStat(struct Pokemon *mon, enum Stat stat, bool32 trainerOwned)
{
    return ExpectedSpeciesStat(mon, GetMonData(mon, MON_DATA_SPECIES), stat, trainerOwned);
}

static void GenerateTrainerMon(struct Pokemon *mon, enum Species species, enum Ability ability)
{
    struct TrainerGenerator generator = {0};
    struct TrainerMon trainerMon =
    {
        .species = species,
        .ability = ability,
        .lvl = 50,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .gender = TRAINER_MON_RANDOM_GENDER,
        .nature = NATURE_HARDY,
        .friendship = 0,
        .ball = POKEBALL_COUNT,
    };
    generator.otID = OTID_STRUCT_RANDOM_NO_SHINY;
    generator.localRngState = LocalRandomSeed(0x1234);
    GenerateMonFromTrainerMon(mon, &trainerMon, &generator);
}

TEST("Inclement layer keeps each Mega's HP equal to its base form's")
{
    ASSUME(GetInclementSpeciesBaseStat(SPECIES_CAMERUPT, STAT_HP) > GetSpeciesBaseStat(SPECIES_CAMERUPT, STAT_HP));
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_CAMERUPT_MEGA, STAT_HP), GetInclementSpeciesBaseStat(SPECIES_CAMERUPT, STAT_HP));
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_MEOWSTIC_M_MEGA, STAT_HP), GetInclementSpeciesBaseStat(SPECIES_MEOWSTIC_M, STAT_HP));
}

TEST("Player Pokemon use Inclement base stats; trainer Pokemon keep the species data")
{
    struct Pokemon player, trainer;

    ASSUME(GetInclementSpeciesBaseStat(SPECIES_ARBOK, STAT_HP) == 80);
    ASSUME(GetSpeciesBaseStat(SPECIES_ARBOK, STAT_HP) == 60);

    CreateMon(&player, SPECIES_ARBOK, 50, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&player);
    GenerateTrainerMon(&trainer, SPECIES_ARBOK, ABILITY_NONE);

    EXPECT(!IsMonTrainerOwned(&player));
    EXPECT(IsMonTrainerOwned(&trainer));
    for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
    {
        EXPECT_EQ(GetMonData(&player, MON_DATA_MAX_HP + stat), ExpectedStat(&player, stat, FALSE));
        EXPECT_EQ(GetMonData(&trainer, MON_DATA_MAX_HP + stat), ExpectedStat(&trainer, stat, TRUE));
    }
    EXPECT_GT(GetMonData(&player, MON_DATA_MAX_HP), ExpectedStat(&player, STAT_HP, TRUE));
}

TEST("Catching a wild Pokemon keeps the Inclement layer")
{
    struct Pokemon *wild = &gParties[B_TRAINER_OPPONENT_A][0];
    struct Pokemon *caught = &gParties[B_TRAINER_PLAYER][0];
    u32 slot = 0;

    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    gBattleTypeFlags = 0;
    // Wild Pokemon are generated with the player's OT, as in CreateWildMon.
    CreateMon(wild, SPECIES_ABSOL, 30, 0, OTID_STRUCT_PLAYER_ID);
    slot = ABILITY_SLOT_INCLEMENT;
    SetMonData(wild, MON_DATA_ABILITY_NUM, &slot);
    CalculateMonStats(wild);
    MarkTrainerBattlePartiesOwned();
    EXPECT(!IsMonTrainerOwned(wild));
    EXPECT_EQ(GetMonAbility(wild), ABILITY_KEEN_EDGE);

    EXPECT_EQ(GiveCapturedMonToPlayer(wild), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(caught, MON_DATA_SPECIES), SPECIES_ABSOL);
    EXPECT(!IsMonTrainerOwned(caught));
    EXPECT_EQ(GetMonAbility(caught), ABILITY_KEEN_EDGE);
    for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(caught, MON_DATA_MAX_HP + stat), ExpectedStat(caught, stat, FALSE));
}

TEST("A trainer battle marks both opponent parties and the partner party as trainer-owned")
{
    ZeroEnemyPartyMons();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ARBOK, 30, 0, OTID_STRUCT_RANDOM_NO_SHINY);
    CreateMon(&gParties[B_TRAINER_OPPONENT_B][0], SPECIES_ARBOK, 30, 0, OTID_STRUCT_RANDOM_NO_SHINY);
    CreateMon(&gParties[B_TRAINER_PARTNER][0], SPECIES_ARBOK, 30, 0, OTID_STRUCT_RANDOM_NO_SHINY);
    CalculateMonStats(&gParties[B_TRAINER_OPPONENT_A][0]);
    CalculateMonStats(&gParties[B_TRAINER_OPPONENT_B][0]);
    CalculateMonStats(&gParties[B_TRAINER_PARTNER][0]);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MAX_HP), ExpectedStat(&gParties[B_TRAINER_OPPONENT_A][0], STAT_HP, FALSE));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    MarkTrainerBattlePartiesOwned();
    EXPECT(IsMonTrainerOwned(&gParties[B_TRAINER_OPPONENT_A][0]));
    EXPECT(IsMonTrainerOwned(&gParties[B_TRAINER_OPPONENT_B][0]));
    EXPECT(IsMonTrainerOwned(&gParties[B_TRAINER_PARTNER][0]));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MAX_HP), ExpectedStat(&gParties[B_TRAINER_OPPONENT_A][0], STAT_HP, TRUE));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MAX_HP));
    gBattleTypeFlags = 0;
    ZeroPartyMons(gParties[B_TRAINER_PARTNER]);
}

SINGLE_BATTLE_TEST("Player and trainer Pokemon of the same species battle with their own base stats")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK);
        OPPONENT(SPECIES_ARBOK);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        struct Pokemon *playerMon = &gParties[B_TRAINER_PLAYER][0];
        struct Pokemon *opponentMon = &gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT(IsMonTrainerOwned(opponentMon));
        EXPECT_EQ(player->maxHP, ExpectedStat(playerMon, STAT_HP, FALSE));
        EXPECT_EQ(player->attack, ExpectedStat(playerMon, STAT_ATK, FALSE));
        EXPECT_EQ(player->defense, ExpectedStat(playerMon, STAT_DEF, FALSE));
        EXPECT_EQ(opponent->maxHP, ExpectedStat(opponentMon, STAT_HP, TRUE));
        EXPECT_EQ(opponent->attack, ExpectedStat(opponentMon, STAT_ATK, TRUE));
        EXPECT_EQ(opponent->defense, ExpectedStat(opponentMon, STAT_DEF, TRUE));
    }
}

WILD_BATTLE_TEST("A wild foe battles with Inclement base stats")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        struct Pokemon *wildMon = &gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT_EQ(opponent->maxHP, ExpectedStat(wildMon, STAT_HP, FALSE));
        EXPECT_EQ(opponent->attack, ExpectedStat(wildMon, STAT_ATK, FALSE));
        EXPECT_EQ(opponent->defense, ExpectedStat(wildMon, STAT_DEF, FALSE));
        EXPECT_GT(opponent->maxHP, ExpectedStat(wildMon, STAT_HP, TRUE));
    }
}

SINGLE_BATTLE_TEST("Mega Evolution uses each side's own stats")
{
    GIVEN {
        ASSUME(GetInclementSpeciesBaseStat(SPECIES_CAMERUPT_MEGA, STAT_DEF) > GetSpeciesBaseStat(SPECIES_CAMERUPT_MEGA, STAT_DEF));
        PLAYER(SPECIES_CAMERUPT) { Item(ITEM_CAMERUPTITE); }
        OPPONENT(SPECIES_CAMERUPT) { Item(ITEM_CAMERUPTITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } THEN {
        struct Pokemon *playerMon = &gParties[B_TRAINER_PLAYER][0];
        struct Pokemon *opponentMon = &gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT_EQ(player->species, SPECIES_CAMERUPT_MEGA);
        EXPECT_EQ(opponent->species, SPECIES_CAMERUPT_MEGA);
        EXPECT_EQ(player->defense, ExpectedSpeciesStat(playerMon, SPECIES_CAMERUPT_MEGA, STAT_DEF, FALSE));
        EXPECT_EQ(opponent->defense, ExpectedSpeciesStat(opponentMon, SPECIES_CAMERUPT_MEGA, STAT_DEF, TRUE));
        EXPECT_EQ(player->maxHP, ExpectedSpeciesStat(playerMon, SPECIES_CAMERUPT_MEGA, STAT_HP, FALSE));
        EXPECT_EQ(opponent->maxHP, ExpectedSpeciesStat(opponentMon, SPECIES_CAMERUPT_MEGA, STAT_HP, TRUE));
    }
}


TEST("Inclement layer uses Inclement's exact line and never lowers base stat total")
{
    // Flareon's redistribution comes over whole, including the stats it lowers.
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_HP), 95);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_ATK), 130);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_DEF), 60);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_SPATK), 65);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_SPDEF), 65);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_FLAREON, STAT_SPEED), 110);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_GIGALITH, STAT_ATK), 60);
    EXPECT_EQ(GetInclementSpeciesBaseStat(SPECIES_GIGALITH, STAT_SPATK), 135);
    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        u32 layered = 0, current = 0;
        if (!IsSpeciesEnabled(species))
            continue;
        for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
        {
            layered += GetInclementSpeciesBaseStat(species, stat);
            current += GetSpeciesBaseStat(species, stat);
        }
        EXPECT_GE(layered, current);
    }
}

TEST("Inclement Abilities are added in slot 3; slots 0-2 keep their meaning for everyone")
{
    struct Pokemon player, trainer;
    u32 slot;

    for (slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
    {
        EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_ABSOL, slot, FALSE), GetSpeciesAbility(SPECIES_ABSOL, slot));
        EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_ABSOL, slot, TRUE), GetSpeciesAbility(SPECIES_ABSOL, slot));
    }
    EXPECT_EQ(GetSpeciesAbility(SPECIES_ABSOL, 0), ABILITY_PRESSURE);
    EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_ABSOL, ABILITY_SLOT_INCLEMENT, FALSE), ABILITY_KEEN_EDGE);
    EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_ABSOL, ABILITY_SLOT_INCLEMENT, TRUE), ABILITY_NONE);
    EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_GALLADE, ABILITY_SLOT_INCLEMENT, FALSE), ABILITY_TRACE);
    EXPECT_EQ(GetSpeciesAbility(SPECIES_GALLADE, 1), ABILITY_SHARPNESS);
    EXPECT_EQ(GetSpeciesAbilityForOwner(SPECIES_BANETTE, ABILITY_SLOT_INCLEMENT, FALSE), ABILITY_VENGEANCE);
    EXPECT_EQ(GetSpeciesAbility(SPECIES_BANETTE, 1), ABILITY_FRISK);

    CreateMon(&player, SPECIES_ABSOL, 50, 0, OTID_STRUCT_PLAYER_ID);
    slot = ABILITY_SLOT_INCLEMENT;
    SetMonData(&player, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetMonAbility(&player), ABILITY_KEEN_EDGE);

    // A trainer can never hold the added slot: authoring Pressure picks slot 0,
    // and a Pokemon that had slot 3 loses it when it becomes trainer-owned.
    GenerateTrainerMon(&trainer, SPECIES_ABSOL, ABILITY_PRESSURE);
    EXPECT_EQ(GetMonData(&trainer, MON_DATA_ABILITY_NUM), 0);
    EXPECT(!FindSpeciesAbilitySlotForOwner(SPECIES_ABSOL, ABILITY_KEEN_EDGE, TRUE, &slot));
    SetMonTrainerOwned(&player, TRUE);
    EXPECT_NE(GetMonData(&player, MON_DATA_ABILITY_NUM), ABILITY_SLOT_INCLEMENT);
    EXPECT_NE(GetMonAbility(&player), ABILITY_KEEN_EDGE);
}

TEST("A species without an Inclement Ability resolves slot 3 as slot 0")
{
    struct Pokemon mon;
    u32 slot = ABILITY_SLOT_INCLEMENT;

    ASSUME(GetInclementAddedAbility(SPECIES_WOBBUFFET, ABILITY_SLOT_INCLEMENT) == ABILITY_NONE);
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetMonAbility(&mon), GetSpeciesAbility(SPECIES_WOBBUFFET, 0));
}

TEST("New Pokemon roll the Inclement slot like a normal slot; the hidden slot is never rolled")
{
    ASSUME(GetSpeciesAbility(SPECIES_ABSOL, 1) != ABILITY_NONE);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_ABSOL, 0), 0);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_ABSOL, 1), 1);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_ABSOL, 2), ABILITY_SLOT_INCLEMENT);
    // Species without an Inclement Ability keep the original personality bit.
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_WOBBUFFET, 5), GetSpeciesAbility(SPECIES_WOBBUFFET, 1) != ABILITY_NONE ? 1 : 0);
    for (u32 personality = 0; personality < 12; personality++)
        EXPECT_NE(RollNormalAbilitySlot(SPECIES_GALLADE, personality), 2);
}

TEST("The party Ability list offers every official Ability plus the Inclement one")
{
    struct Pokemon mon;
    u8 slots[NUM_OWNER_ABILITY_SLOTS];

    CreateMon(&mon, SPECIES_GALLADE, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetMonSelectableAbilitySlots(&mon, slots), 4);
    EXPECT_EQ(slots[0], 0);
    EXPECT_EQ(slots[1], 1);
    EXPECT_EQ(slots[2], ABILITY_SLOT_INCLEMENT);
    EXPECT_EQ(slots[3], 2);
    SetMonTrainerOwned(&mon, TRUE);
    EXPECT_EQ(GetMonSelectableAbilitySlots(&mon, slots), 3);
}

TEST("Ability Capsule cycles the normal slots, Inclement slot included; Ability Patch toggles hidden")
{
    struct Pokemon mon;
    u32 slot = 0;

    CreateMon(&mon, SPECIES_ABSOL, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), 1);
    slot = 1;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), ABILITY_SLOT_INCLEMENT);
    slot = ABILITY_SLOT_INCLEMENT;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), 0);
    EXPECT_EQ(GetAbilityPatchTargetSlot(&mon), 2);
    slot = 2;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), NUM_OWNER_ABILITY_SLOTS);
    EXPECT_EQ(GetAbilityPatchTargetSlot(&mon), 0);

    // With one official normal Ability, the Inclement one makes the Capsule useful.
    ASSUME(GetSpeciesAbility(SPECIES_GLACEON, 0) != GetSpeciesAbility(SPECIES_GLACEON, 1));
    CreateMon(&mon, SPECIES_EXEGGUTOR, 50, 0, OTID_STRUCT_PLAYER_ID);
    ASSUME(GetSpeciesAbility(SPECIES_EXEGGUTOR, 1) == ABILITY_NONE);
    slot = 0;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), ABILITY_SLOT_INCLEMENT);
}

SINGLE_BATTLE_TEST("Player Absol battles with Keen Edge in slot 3 while a trainer's Absol keeps Pressure")
{
    GIVEN {
        PLAYER(SPECIES_ABSOL) { Ability(ABILITY_KEEN_EDGE); }
        OPPONENT(SPECIES_ABSOL);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ABILITY_NUM), ABILITY_SLOT_INCLEMENT);
        EXPECT_EQ(player->ability, ABILITY_KEEN_EDGE);
        EXPECT_EQ(opponent->ability, ABILITY_PRESSURE);
    }
}

WILD_BATTLE_TEST("A wild Absol can already have Keen Edge")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ABSOL) { Ability(ABILITY_KEEN_EDGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!IsMonTrainerOwned(&gParties[B_TRAINER_OPPONENT_A][0]));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_ABILITY_NUM), ABILITY_SLOT_INCLEMENT);
        EXPECT_EQ(opponent->ability, ABILITY_KEEN_EDGE);
    }
}

SINGLE_BATTLE_TEST("A trainer's Mega Sceptile keeps Lightning Rod; a player's Chloroplast Sceptile Mega-evolves with Chloroplast")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_OVERGROW; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; }
    GIVEN {
        ASSUME(GetSpeciesAbility(SPECIES_SCEPTILE_MEGA, 0) == ABILITY_LIGHTNING_ROD);
        PLAYER(SPECIES_SCEPTILE) { Ability(ability); Item(ITEM_SCEPTILITE); }
        OPPONENT(SPECIES_SCEPTILE) { Item(ITEM_SCEPTILITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_SCEPTILE_MEGA);
        EXPECT_EQ(opponent->species, SPECIES_SCEPTILE_MEGA);
        // A Mega keeps its official Ability unless the base Pokemon uses its Inclement slot.
        EXPECT_EQ(player->ability, ability == ABILITY_CHLOROPLAST ? ABILITY_CHLOROPLAST : ABILITY_LIGHTNING_ROD);
        EXPECT_EQ(opponent->ability, ABILITY_LIGHTNING_ROD);
    }
}

WILD_BATTLE_TEST("A wild Glaceon can have Whiteout alongside its official Abilities")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLACEON) { Ability(ABILITY_WHITEOUT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetSpeciesAbility(SPECIES_GLACEON, 1), ABILITY_ICE_BODY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_ABILITY_NUM), ABILITY_SLOT_INCLEMENT);
        EXPECT_EQ(opponent->ability, ABILITY_WHITEOUT);
    }
}

TEST("Every Inclement added Ability is reachable by player and wild Pokemon and never by trainers")
{
    u32 added = 0, most = 0;

    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        struct Pokemon mon;
        u8 slots[NUM_OWNER_ABILITY_SLOTS];
        u32 perSpecies = 0;

        if (!IsSpeciesEnabled(species))
            continue;
        for (u32 slot = ABILITY_SLOT_INCLEMENT; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
        {
            enum Ability ability = GetInclementAddedAbility(species, slot);
            u32 found, count, listed = FALSE;

            if (ability == ABILITY_NONE)
                continue;
            added++;
            perSpecies++;
            EXPECT(FindSpeciesAbilitySlotForOwner(species, ability, FALSE, &found));
            EXPECT_EQ(found, slot);
            EXPECT(!FindSpeciesAbilitySlotForOwner(species, ability, TRUE, &found)
                || found < NUM_ABILITY_SLOTS);
            EXPECT_EQ(GetSpeciesAbilityForOwner(species, slot, TRUE), ABILITY_NONE);

            CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
            SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), slot);
            EXPECT_EQ(GetMonAbility(&mon), ability);
            count = GetMonSelectableAbilitySlots(&mon, slots);
            for (u32 i = 0; i < count; i++)
                listed |= slots[i] == slot;
            EXPECT(listed);

            // Becoming trainer-owned drops the added slot for an official one.
            SetMonTrainerOwned(&mon, TRUE);
            EXPECT_LT(GetMonData(&mon, MON_DATA_ABILITY_NUM), NUM_ABILITY_SLOTS);
        }
        most = max(most, perSpecies);
    }
    EXPECT_GT(added, 0);
    EXPECT_EQ(most, NUM_INCLEMENT_ABILITY_SLOTS);
}

TEST("The stored ability number holds every slot without disturbing its neighbours")
{
    struct Pokemon mon;
    u32 shadow = TRUE, fateful = TRUE;

    CreateMon(&mon, SPECIES_DELPHOX, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_IS_SHADOW, &shadow);
    SetMonData(&mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER, &fateful);
    for (u32 slot = 0; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
    {
        SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), slot);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), TRUE);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER), TRUE);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    }
    // Official slots keep their meaning for everyone.
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
    {
        SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
        EXPECT_EQ(GetMonAbility(&mon), GetAbilityBySpecies(SPECIES_DELPHOX, slot));
    }
}

TEST("Ability Capsule cycles every normal and added slot")
{
    struct Pokemon mon;
    u32 slot = 0;

    // Delphox: Blaze, then Inclement's Pyromancy and Magic Guard; Magician is hidden.
    ASSUME(GetSpeciesAbility(SPECIES_DELPHOX, 1) == ABILITY_NONE);
    ASSUME(GetInclementAddedAbility(SPECIES_DELPHOX, ABILITY_SLOT_INCLEMENT) == ABILITY_PYROMANCY);
    ASSUME(GetInclementAddedAbility(SPECIES_DELPHOX, ABILITY_SLOT_INCLEMENT + 1) == ABILITY_MAGIC_GUARD);
    CreateMon(&mon, SPECIES_DELPHOX, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), ABILITY_SLOT_INCLEMENT);
    slot = ABILITY_SLOT_INCLEMENT;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), ABILITY_SLOT_INCLEMENT + 1);
    slot = ABILITY_SLOT_INCLEMENT + 1;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), 0);
    EXPECT_EQ(GetAbilityPatchTargetSlot(&mon), 2);

    // Persian: Limber, Technician, Super Luck, Sniper.
    CreateMon(&mon, SPECIES_PERSIAN, 50, 0, OTID_STRUCT_PLAYER_ID);
    slot = 1;
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), ABILITY_SLOT_INCLEMENT);
    SetMonTrainerOwned(&mon, TRUE);
    EXPECT_EQ(GetAbilityCapsuleTargetSlot(&mon), 0);
}

TEST("New Pokemon roll every added slot like a normal slot")
{
    // Persian's normal slots: 0, 1 and two Inclement ones.
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_PERSIAN, 0), 0);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_PERSIAN, 1), 1);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_PERSIAN, 2), ABILITY_SLOT_INCLEMENT);
    EXPECT_EQ(RollNormalAbilitySlot(SPECIES_PERSIAN, 3), ABILITY_SLOT_INCLEMENT + 1);
}

static void ExpectAbilityMenuFits(const u8 *const *labels, u32 optionCount)
{
    struct PartyAbilityMenuLayout layout;
    u32 letterSpacing = GetFontAttribute(FONT_NORMAL, FONTATTR_LETTER_SPACING);

    GetPartyAbilityMenuLayout(labels, optionCount, &layout);
    // The list and its frame stay on the 30x20-tile screen.
    EXPECT_GE(layout.x, 1);
    EXPECT_LE(layout.x + layout.width, 29);
    EXPECT_GE(layout.y, 1);
    EXPECT_LE(layout.y + layout.height, 19);
    EXPECT_LE(layout.visibleRows, PARTY_ABILITY_MENU_MAX_ROWS);
    EXPECT_EQ(layout.visibleRows, min(optionCount, PARTY_ABILITY_MENU_MAX_ROWS));
    // Every label fits in its font.
    for (u32 i = 0; i < optionCount; i++)
        EXPECT_LE(GetStringWidth(GetPartyAbilityMenuLabelFont(labels[i], &layout), labels[i], letterSpacing), layout.textWidth);
    // The prompt keeps clear of the list (its right frame sits left of the
    // list's left frame) and never outgrows its tile budget.
    EXPECT_GE(layout.promptX, 1);
    EXPECT_GE(layout.promptWidth, 1);
    EXPECT_LE(layout.promptWidth, PARTY_ABILITY_PROMPT_MAX_WIDTH);
    EXPECT_LT(layout.promptX + layout.promptWidth, layout.x - 1);
}

// The prompt's wrapped text fits its box within the line cap, and the
// bottom-anchored box and its frame stay on screen.
static u32 ExpectAbilityPromptFits(const u8 *text, const struct PartyAbilityMenuLayout *layout)
{
    u8 wrapped[PARTY_ABILITY_PROMPT_LENGTH];
    u8 fontId;
    u32 lines, breaks = 0;

    EXPECT(text != NULL);
    EXPECT_LT(StringLength(text), PARTY_ABILITY_PROMPT_LENGTH);
    lines = WrapPartyAbilityMenuPrompt(text, layout->promptWidth * 8, wrapped, &fontId);
    for (u32 i = 0; wrapped[i] != EOS; i++)
        breaks += (wrapped[i] == CHAR_NEWLINE);
    EXPECT_GE(lines, 1);
    EXPECT_LE(lines, PARTY_ABILITY_PROMPT_MAX_LINES);
    EXPECT_EQ(lines, breaks + 1);
    EXPECT_LE(GetStringWidth(fontId, wrapped, GetFontAttribute(fontId, FONTATTR_LETTER_SPACING)), layout->promptWidth * 8);
    EXPECT_GE(19 - (s32)lines * 2, 1);
    return lines;
}

TEST("The party Ability list and its prompt fit on screen for every species and owner")
{
    u32 most = 0;

    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        for (u32 trainerOwned = 0; trainerOwned <= 1; trainerOwned++)
        {
            struct Pokemon mon;
            u8 slots[NUM_OWNER_ABILITY_SLOTS];
            const u8 *labels[PARTY_ABILITY_MENU_MAX_OPTIONS];
            u32 optionCount;

            if (!IsSpeciesEnabled(species))
                continue;
            CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
            SetMonTrainerOwned(&mon, trainerOwned);
            optionCount = GetPartyAbilityMenuOptions(&mon, slots, labels);
            most = max(most, optionCount);
            ExpectAbilityMenuFits(labels, optionCount);
        }
    }
    EXPECT_LE(most, PARTY_ABILITY_MENU_MAX_OPTIONS);
}

TEST("The party Ability prompt explains every selectable Ability within its box")
{
    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        for (u32 trainerOwned = 0; trainerOwned <= 1; trainerOwned++)
        {
            struct Pokemon mon;
            u8 slots[NUM_OWNER_ABILITY_SLOTS];
            const u8 *labels[PARTY_ABILITY_MENU_MAX_OPTIONS];
            struct PartyAbilityMenuLayout layout;
            u32 optionCount;

            if (!IsSpeciesEnabled(species))
                continue;
            CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
            SetMonTrainerOwned(&mon, trainerOwned);
            optionCount = GetPartyAbilityMenuOptions(&mon, slots, labels);
            GetPartyAbilityMenuLayout(labels, optionCount, &layout);
            // Each Ability shows its owner-aware description; Cancel asks.
            for (u32 option = 0; option < optionCount - 1; option++)
            {
                enum Ability ability = GetAbilityBySpeciesForOwner(species, slots[option], trainerOwned);
                EXPECT(GetPartyAbilityMenuPromptText(&mon, option) == gAbilitiesInfo[ability].description);
                ExpectAbilityPromptFits(GetPartyAbilityMenuPromptText(&mon, option), &layout);
            }
            EXPECT_EQ(StringCompare(GetPartyAbilityMenuPromptText(&mon, optionCount - 1), COMPOUND_STRING("Which Ability?")), 0);
            EXPECT_EQ(ExpectAbilityPromptFits(GetPartyAbilityMenuPromptText(&mon, optionCount - 1), &layout), 1);
        }
    }
}

TEST("The party Ability prompt wraps by word and falls back to narrower fonts")
{
    struct PartyAbilityMenuLayout layout = {.promptX = 1, .promptWidth = PARTY_ABILITY_MENU_MIN_WIDTH};
    u8 wrapped[PARTY_ABILITY_PROMPT_LENGTH];
    u8 fontId;

    const u8 *sentence = COMPOUND_STRING("Ups Ice moves in snow or hail.");
    u32 breaks = 0;

    // A sentence wider than the box wraps in the normal font, breaking only
    // at spaces.
    ASSUME(GetStringWidth(FONT_NORMAL, sentence, 0) > 80);
    EXPECT_EQ(WrapPartyAbilityMenuPrompt(sentence, 80, wrapped, &fontId), 2);
    EXPECT_EQ(fontId, FONT_NORMAL);
    for (u32 i = 0; sentence[i] != EOS; i++)
    {
        if (wrapped[i] == CHAR_NEWLINE)
        {
            EXPECT_EQ(sentence[i], CHAR_SPACE);
            breaks++;
        }
        else
        {
            EXPECT_EQ(wrapped[i], sentence[i]);
        }
    }
    EXPECT_EQ(breaks, 1);
    // A word too wide for the box, or text too long for the line cap, takes a
    // narrower font rather than overflowing.
    sentence = COMPOUND_STRING("Wwwwwwwwwwwwwww");
    ASSUME(GetStringWidth(FONT_NORMAL, sentence, 0) > layout.promptWidth * 8);
    ExpectAbilityPromptFits(sentence, &layout);
    WrapPartyAbilityMenuPrompt(sentence, layout.promptWidth * 8, wrapped, &fontId);
    EXPECT_NE(fontId, FONT_NORMAL);
    sentence = COMPOUND_STRING("Mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm mmm");
    ASSUME(GetStringWidth(FONT_NORMAL, sentence, 0) > PARTY_ABILITY_PROMPT_MAX_LINES * layout.promptWidth * 8);
    ExpectAbilityPromptFits(sentence, &layout);
    WrapPartyAbilityMenuPrompt(sentence, layout.promptWidth * 8, wrapped, &fontId);
    EXPECT_NE(fontId, FONT_NORMAL);
}

TEST("The party Ability list scrolls and shrinks fonts for a hypothetical overlong list")
{
    const u8 *labels[16];
    u32 widest = ABILITY_NONE + 1, widestWidth = 0;

    for (u32 ability = ABILITY_NONE + 1; ability < ABILITIES_COUNT; ability++)
    {
        u32 width = GetStringWidth(FONT_NORMAL, gAbilitiesInfo[ability].name, 0);
        if (width > widestWidth)
        {
            widestWidth = width;
            widest = ability;
        }
    }
    // Sixteen of the widest name: taller than the screen, so it scrolls.
    for (u32 i = 0; i < ARRAY_COUNT(labels); i++)
        labels[i] = gAbilitiesInfo[widest].name;
    ExpectAbilityMenuFits(labels, ARRAY_COUNT(labels));
    // A label far wider than the list falls back to a narrower font.
    labels[0] = COMPOUND_STRING("Wwwwwwwwwwwwwwww");
    ExpectAbilityMenuFits(labels, 2);
}
