#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "pokemon.h"
#include "trainer_util.h"
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

TEST("Inclement layer never lowers a base stat")
{
    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        if (!IsSpeciesEnabled(species))
            continue;
        for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
            EXPECT_GE(GetInclementSpeciesBaseStat(species, stat), GetSpeciesBaseStat(species, stat));
    }
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

TEST("Player and wild Absol have Keen Edge in slot 1; a trainer's Absol keeps Pressure")
{
    struct Pokemon player, trainer;
    u32 slot = 0;

    CreateMon(&player, SPECIES_ABSOL, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&player, MON_DATA_ABILITY_NUM, &slot);
    EXPECT_EQ(GetMonAbility(&player), ABILITY_KEEN_EDGE);

    GenerateTrainerMon(&trainer, SPECIES_ABSOL, ABILITY_PRESSURE);
    EXPECT_EQ(GetMonData(&trainer, MON_DATA_ABILITY_NUM), 0);
    EXPECT_EQ(GetMonAbility(&trainer), ABILITY_PRESSURE);

    // The Pokedex and gSpeciesInfo keep the species' own Abilities.
    EXPECT_EQ(GetAbilityBySpecies(SPECIES_ABSOL, 0), ABILITY_PRESSURE);
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

SINGLE_BATTLE_TEST("Player Absol battles with Keen Edge while a trainer's Absol keeps Pressure")
{
    GIVEN {
        PLAYER(SPECIES_ABSOL);
        OPPONENT(SPECIES_ABSOL);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!IsMonTrainerOwned(&gParties[B_TRAINER_PLAYER][0]));
        EXPECT_EQ(player->ability, ABILITY_KEEN_EDGE);
        EXPECT_EQ(opponent->ability, ABILITY_PRESSURE);
    }
}

WILD_BATTLE_TEST("A wild Absol already has Keen Edge")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ABSOL);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!IsMonTrainerOwned(&gParties[B_TRAINER_OPPONENT_A][0]));
        EXPECT_EQ(opponent->ability, ABILITY_KEEN_EDGE);
    }
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

SINGLE_BATTLE_TEST("A trainer's Mega Sceptile keeps Lightning Rod while the player's gets Chloroplast")
{
    GIVEN {
        ASSUME(GetSpeciesAbility(SPECIES_SCEPTILE_MEGA, 0) == ABILITY_LIGHTNING_ROD);
        PLAYER(SPECIES_SCEPTILE) { Item(ITEM_SCEPTILITE); }
        OPPONENT(SPECIES_SCEPTILE) { Item(ITEM_SCEPTILITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_SCEPTILE_MEGA);
        EXPECT_EQ(opponent->species, SPECIES_SCEPTILE_MEGA);
        EXPECT_EQ(player->ability, ABILITY_CHLOROPLAST);
        EXPECT_EQ(opponent->ability, ABILITY_LIGHTNING_ROD);
    }
}

WILD_BATTLE_TEST("A wild Glaceon reads its Inclement Ability slot")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLACEON) { Ability(ABILITY_WHITEOUT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!IsMonTrainerOwned(&gParties[B_TRAINER_OPPONENT_A][0]));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_ABILITY_NUM), 1);
        EXPECT_EQ(opponent->ability, ABILITY_WHITEOUT);
    }
}

