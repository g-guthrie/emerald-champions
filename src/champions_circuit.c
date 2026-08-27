#include "global.h"
#include "battle.h"
#include "battle_tower.h"
#include "champions_circuit.h"
#include "data.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "load_save.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "verdant_battle_sets.h"
#include "constants/abilities.h"
#include "constants/battle_frontier_trainers.h"
#include "constants/field_specials.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/vars.h"

enum ChampionsCircuitTheme
{
    CIRCUIT_THEME_BALANCED,
    CIRCUIT_THEME_RAIN,
    CIRCUIT_THEME_SUN,
    CIRCUIT_THEME_SAND,
    CIRCUIT_THEME_SNOW,
    CIRCUIT_THEME_TRICK_ROOM,
    CIRCUIT_THEME_TAILWIND,
    CIRCUIT_THEME_TERRAIN,
    CIRCUIT_THEME_COUNT,
};

struct CircuitTeamState
{
    u16 species[PARTY_SIZE];
    u16 items[PARTY_SIZE];
    u8 typeCounts[NUMBER_OF_MON_TYPES];
    u8 count;
    u8 legendaryCount;
    bool8 hasMega;
    bool8 hasSpeedControl;
    bool8 hasSupport;
};

static const u8 sCircuitThemeBalanced[] = _("balanced offense");
static const u8 sCircuitThemeRain[] = _("rain pressure");
static const u8 sCircuitThemeSun[] = _("sun pressure");
static const u8 sCircuitThemeSand[] = _("sand control");
static const u8 sCircuitThemeSnow[] = _("snow control");
static const u8 sCircuitThemeTrickRoom[] = _("Trick Room");
static const u8 sCircuitThemeTailwind[] = _("Tailwind tempo");
static const u8 sCircuitThemeTerrain[] = _("terrain control");

static const u8 *const sCircuitThemeNames[CIRCUIT_THEME_COUNT] =
{
    sCircuitThemeBalanced,
    sCircuitThemeRain,
    sCircuitThemeSun,
    sCircuitThemeSand,
    sCircuitThemeSnow,
    sCircuitThemeTrickRoom,
    sCircuitThemeTailwind,
    sCircuitThemeTerrain,
};

#define CHAMPIONS_CIRCUIT_ACTIVE_MARKER 0xEC

bool8 IsChampionsCircuitBattle(void)
{
    return gSaveBlock2Ptr->frontier.towerBattleOutcome == CHAMPIONS_CIRCUIT_ACTIVE_MARKER;
}

static bool8 PresetHasMove(const struct VerdantBattleSetPreset *preset, u16 move)
{
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
        if (preset->moves[i] == move)
            return TRUE;
    return FALSE;
}

static bool8 PresetHasSpeedControl(const struct VerdantBattleSetPreset *preset)
{
    return PresetHasMove(preset, MOVE_TAILWIND)
        || PresetHasMove(preset, MOVE_TRICK_ROOM)
        || PresetHasMove(preset, MOVE_ICY_WIND)
        || PresetHasMove(preset, MOVE_ELECTROWEB)
        || PresetHasMove(preset, MOVE_THUNDER_WAVE);
}

static bool8 PresetHasSupport(const struct VerdantBattleSetPreset *preset)
{
    return PresetHasSpeedControl(preset)
        || PresetHasMove(preset, MOVE_FAKE_OUT)
        || PresetHasMove(preset, MOVE_FOLLOW_ME)
        || PresetHasMove(preset, MOVE_RAGE_POWDER)
        || PresetHasMove(preset, MOVE_HELPING_HAND)
        || PresetHasMove(preset, MOVE_WIDE_GUARD);
}

static bool8 SetMatchesCircuitTheme(u16 species, const struct VerdantBattleSetPreset *preset, u8 theme, bool8 anchor)
{
    u16 ability = gBaseStats[species].abilities[preset->abilitySlot];
    u8 type1 = gBaseStats[species].type1;
    u8 type2 = gBaseStats[species].type2;

    switch (theme)
    {
    case CIRCUIT_THEME_BALANCED:
        return TRUE;
    case CIRCUIT_THEME_RAIN:
        if (ability == ABILITY_DRIZZLE || PresetHasMove(preset, MOVE_RAIN_DANCE))
            return TRUE;
        return !anchor && (type1 == TYPE_WATER || type2 == TYPE_WATER
                        || type1 == TYPE_ELECTRIC || type2 == TYPE_ELECTRIC);
    case CIRCUIT_THEME_SUN:
        if (ability == ABILITY_DROUGHT || PresetHasMove(preset, MOVE_SUNNY_DAY))
            return TRUE;
        return !anchor && (type1 == TYPE_FIRE || type2 == TYPE_FIRE
                        || type1 == TYPE_GRASS || type2 == TYPE_GRASS);
    case CIRCUIT_THEME_SAND:
        if (ability == ABILITY_SAND_STREAM || PresetHasMove(preset, MOVE_SANDSTORM))
            return TRUE;
        return !anchor && (type1 == TYPE_ROCK || type2 == TYPE_ROCK
                        || type1 == TYPE_GROUND || type2 == TYPE_GROUND
                        || type1 == TYPE_STEEL || type2 == TYPE_STEEL);
    case CIRCUIT_THEME_SNOW:
        if (ability == ABILITY_SNOW_WARNING || PresetHasMove(preset, MOVE_HAIL))
            return TRUE;
        return !anchor && (type1 == TYPE_ICE || type2 == TYPE_ICE);
    case CIRCUIT_THEME_TRICK_ROOM:
        if (PresetHasMove(preset, MOVE_TRICK_ROOM))
            return TRUE;
        return !anchor && gBaseStats[species].baseSpeed <= 70;
    case CIRCUIT_THEME_TAILWIND:
        if (PresetHasMove(preset, MOVE_TAILWIND))
            return TRUE;
        return !anchor && gBaseStats[species].baseSpeed >= 90;
    case CIRCUIT_THEME_TERRAIN:
        if (ability == ABILITY_ELECTRIC_SURGE
         || ability == ABILITY_GRASSY_SURGE
         || ability == ABILITY_MISTY_SURGE
         || ability == ABILITY_PSYCHIC_SURGE
         || PresetHasMove(preset, MOVE_ELECTRIC_TERRAIN)
         || PresetHasMove(preset, MOVE_GRASSY_TERRAIN)
         || PresetHasMove(preset, MOVE_MISTY_TERRAIN)
         || PresetHasMove(preset, MOVE_PSYCHIC_TERRAIN))
            return TRUE;
        return !anchor && (type1 == TYPE_ELECTRIC || type2 == TYPE_ELECTRIC
                        || type1 == TYPE_GRASS || type2 == TYPE_GRASS
                        || type1 == TYPE_PSYCHIC || type2 == TYPE_PSYCHIC
                        || type1 == TYPE_FAIRY || type2 == TYPE_FAIRY);
    }
    return FALSE;
}

static bool8 CircuitCandidateAllowed(
    const struct CircuitTeamState *team,
    u16 species,
    const struct VerdantBattleSetPreset *preset,
    bool8 requireMega)
{
    u16 nationalDex = SpeciesToNationalPokedexNum(species);
    u16 item = preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
    u16 baseStatTotal = gBaseStats[species].baseHP
                      + gBaseStats[species].baseAttack
                      + gBaseStats[species].baseDefense
                      + gBaseStats[species].baseSpeed
                      + gBaseStats[species].baseSpAttack
                      + gBaseStats[species].baseSpDefense;
    u8 type1 = gBaseStats[species].type1;
    u8 type2 = gBaseStats[species].type2;
    u8 i;

    if (gBaseStats[species].baseHP == 0 || nationalDex == 0)
        return FALSE;
    if (baseStatTotal < 400
     && item != ITEM_EVIOLITE
     && item != ITEM_FOCUS_SASH
     && !PresetHasSupport(preset))
        return FALSE;
    if (requireMega && preset->requiredItem == ITEM_NONE)
        return FALSE;
    if (team->hasMega && preset->requiredItem != ITEM_NONE)
        return FALSE;
    if (IsVerdantLegendarySpecies(species) && team->legendaryCount >= 2)
        return FALSE;
    for (i = 0; i < team->count; i++)
    {
        if (SpeciesToNationalPokedexNum(team->species[i]) == nationalDex)
            return FALSE;
        if (item != ITEM_NONE && team->items[i] == item)
            return FALSE;
    }
    if (type1 < NUMBER_OF_MON_TYPES && team->typeCounts[type1] >= 2)
        return FALSE;
    if (type2 != type1 && type2 < NUMBER_OF_MON_TYPES && team->typeCounts[type2] >= 2)
        return FALSE;
    return TRUE;
}

static bool8 ChooseCircuitSet(
    const struct CircuitTeamState *team,
    u8 theme,
    bool8 requireTheme,
    bool8 requireAnchor,
    bool8 requireSpeedControl,
    bool8 requireMega,
    u16 *speciesOut,
    u8 *choiceOut)
{
    u32 matches = 0;
    u16 species;

    for (species = 1; species < NUM_SPECIES; species++)
    {
        u8 rawCount = GetVerdantBattleSetRawCount(species);
        u8 choice;

        for (choice = 0; choice < rawCount; choice++)
        {
            const struct VerdantBattleSetPreset *preset = GetVerdantBattleSetRawPreset(species, choice);

            if (preset == NULL
             || !CircuitCandidateAllowed(team, species, preset, requireMega)
             || (requireTheme && !SetMatchesCircuitTheme(species, preset, theme, requireAnchor))
             || (requireSpeedControl && !PresetHasSpeedControl(preset)))
                continue;
            matches++;
            if (Random() % matches == 0)
            {
                *speciesOut = species;
                *choiceOut = choice;
            }
        }
    }
    return matches != 0;
}

static void AddCircuitSetToState(struct CircuitTeamState *team, u16 species, const struct VerdantBattleSetPreset *preset)
{
    u16 item = preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
    u8 type1 = gBaseStats[species].type1;
    u8 type2 = gBaseStats[species].type2;

    team->species[team->count] = species;
    team->items[team->count] = item;
    team->count++;
    if (type1 < NUMBER_OF_MON_TYPES)
        team->typeCounts[type1]++;
    if (type2 != type1 && type2 < NUMBER_OF_MON_TYPES)
        team->typeCounts[type2]++;
    if (IsVerdantLegendarySpecies(species))
        team->legendaryCount++;
    if (preset->requiredItem != ITEM_NONE)
        team->hasMega = TRUE;
    if (PresetHasSpeedControl(preset))
        team->hasSpeedControl = TRUE;
    if (PresetHasSupport(preset))
        team->hasSupport = TRUE;
}

static void ApplyCircuitEVs(struct Pokemon *mon, const struct VerdantBattleSetPreset *preset)
{
    u8 evs[NUM_STATS] = {0};
    u8 iv;
    u8 physical = 0;
    u8 special = 0;
    u8 damaging = 0;
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 move = preset->moves[i];

        if (move == MOVE_NONE || move >= MOVES_COUNT || gBattleMoves[move].power == 0)
            continue;
        damaging++;
        if (gBattleMoves[move].split == SPLIT_PHYSICAL)
            physical++;
        else if (gBattleMoves[move].split == SPLIT_SPECIAL)
            special++;
    }

    if (damaging <= 1)
    {
        evs[STAT_HP] = 252;
        evs[STAT_DEF] = 128;
        evs[STAT_SPDEF] = 128;
    }
    else if (PresetHasMove(preset, MOVE_TRICK_ROOM))
    {
        evs[STAT_HP] = 252;
        evs[physical > special ? STAT_ATK : STAT_SPATK] = 252;
        evs[STAT_SPDEF] = 4;
    }
    else
    {
        evs[STAT_SPEED] = 252;
        evs[physical > special ? STAT_ATK : STAT_SPATK] = 252;
        evs[STAT_HP] = 4;
    }

    for (i = 0; i < NUM_STATS; i++)
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    if (special > physical)
    {
        iv = 0;
        SetMonData(mon, MON_DATA_ATK_IV, &iv);
    }
    if (PresetHasMove(preset, MOVE_TRICK_ROOM))
    {
        iv = 0;
        SetMonData(mon, MON_DATA_SPEED_IV, &iv);
    }
}

static void NormalizeCircuitPlayerParty(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL);
        u8 level = 80;
        u32 exp;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;
        exp = gExperienceTables[gBaseStats[species].growthRate][level];
        SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);
        SetMonData(&gPlayerParty[i], MON_DATA_LEVEL, &level);
        CalculateMonStats(&gPlayerParty[i]);
    }
    HealPlayerParty();
}

void ChampionsCircuitCanEnter(void)
{
    u8 i;

    gSpecialVar_Result = TRUE;
    if (CalculatePlayerPartyCount() != PARTY_SIZE)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL) == SPECIES_EGG
         || GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) == 0)
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

void ChampionsCircuitBegin(void)
{
    SavePlayerParty();
    gSaveBlock2Ptr->frontier.towerNumWins = 0;
    gSaveBlock2Ptr->frontier.towerBattleOutcome = CHAMPIONS_CIRCUIT_ACTIVE_MARKER;
    NormalizeCircuitPlayerParty();
}

void ChampionsCircuitGenerateOpponent(void)
{
    struct CircuitTeamState team = {0};
    u16 wins = gSaveBlock2Ptr->frontier.towerNumWins;
    u8 theme = Random() % CIRCUIT_THEME_COUNT;
    u8 baseLevel = min(MAX_LEVEL, 80 + wins / PARTY_SIZE);
    u8 boostedSlots = wins % PARTY_SIZE;
    u8 slot;

    ZeroEnemyPartyMons();
    for (slot = 0; slot < PARTY_SIZE; slot++)
    {
        bool8 requireAnchor = slot == 0 && theme != CIRCUIT_THEME_BALANCED;
        bool8 requireTheme = requireAnchor || (slot < 4 && theme != CIRCUIT_THEME_BALANCED && Random() % 100 < 65);
        bool8 requireSpeed = slot == PARTY_SIZE - 2 && !team.hasSpeedControl;
        bool8 requireMega = slot == PARTY_SIZE - 1 && !team.hasMega;
        u16 species = SPECIES_NONE;
        u8 choice = 0;
        const struct VerdantBattleSetPreset *preset;
        u8 level = baseLevel;

        if (!ChooseCircuitSet(&team, theme, requireTheme, requireAnchor, requireSpeed, requireMega, &species, &choice)
         && !ChooseCircuitSet(&team, theme, FALSE, FALSE, requireSpeed, requireMega, &species, &choice)
         && !ChooseCircuitSet(&team, theme, FALSE, FALSE, FALSE, requireMega, &species, &choice)
         && !ChooseCircuitSet(&team, theme, FALSE, FALSE, FALSE, FALSE, &species, &choice))
            break;

        if (slot < boostedSlots && level < MAX_LEVEL)
            level++;
        CreateMon(&gEnemyParty[slot], species, level, MAX_PER_STAT_IVS, TRUE,
                  Random32(), OT_ID_RANDOM_NO_SHINY, 0);
        ApplyVerdantOpponentBattleSet(&gEnemyParty[slot], choice);
        preset = GetVerdantBattleSetRawPreset(species, choice);
        ApplyCircuitEVs(&gEnemyParty[slot], preset);
        CalculateMonStats(&gEnemyParty[slot]);
        AddCircuitSetToState(&team, species, preset);
    }
    CalculateEnemyPartyCount();
    StringCopy(gStringVar1, sCircuitThemeNames[theme]);
    ConvertIntToDecimalStringN(gStringVar2, wins + 1, STR_CONV_MODE_LEFT_ALIGN, 3);
    gSpecialVar_Result = team.count;
}

void ChampionsCircuitHandleBattleResult(void)
{
    gSpecialVar_Result = FALSE;
    if (gBattleOutcome == B_OUTCOME_WON)
    {
        u16 wins = gSaveBlock2Ptr->frontier.towerNumWins + 1;
        u16 total = VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS);

        gSaveBlock2Ptr->frontier.towerNumWins = wins;
        if (total != 0xFFFF)
            VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, total + 1);
        HealPlayerParty();
        gSpecialVar_Result = TRUE;
    }
    else
    {
        LoadPlayerParty();
        HealPlayerParty();
        gSaveBlock2Ptr->frontier.towerNumWins = 0;
        gSaveBlock2Ptr->frontier.towerBattleOutcome = 0;
    }
}

void ChampionsCircuitTryGiveReward(void)
{
    u16 wins = gSaveBlock2Ptr->frontier.towerNumWins;
    u8 rewardIndex = 0;
    u8 signId;

    gSpecialVar_Result = 0;
    for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];

        if (sign->source != LEGENDARY_SOURCE_CIRCUIT)
            continue;
        rewardIndex++;
        if (wins < rewardIndex * 5 || IsLegendarySignCaught(signId))
            continue;
        {
            struct Pokemon reward;
            u8 giveResult;
            u8 choiceCount;

            CreateMon(&reward, sign->species, 80, MAX_PER_STAT_IVS, TRUE,
                      Random32(), OT_ID_PLAYER_ID, 0);
            choiceCount = GetVerdantBattleSetRawCount(sign->species);
            if (choiceCount != 0)
                ApplyVerdantGiftBattleSet(&reward, Random() % choiceCount);
            giveResult = GiveMonToPlayer(&reward);
            if (giveResult == MON_CANT_GIVE)
            {
                gSpecialVar_Result = 3;
                return;
            }
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(sign->species), FLAG_SET_SEEN);
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(sign->species), FLAG_SET_CAUGHT);
            MarkLegendarySignCaughtBySpecies(sign->species);
            StringCopy(gStringVar1, gSpeciesNames[sign->species]);
            gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
            return;
        }
    }

    // The seventeenth Circuit species is earned at 85 wins. A player who
    // survives five more matches and has claimed every milestone completes
    // the facility's mastery track with Eternatus.
    if (wins >= 90 && !IsLegendarySignCaught(LEGENDARY_SIGN_ETERNATUS))
    {
        const struct LegendarySignDefinition *sign =
            &gLegendarySignDefinitions[LEGENDARY_SIGN_ETERNATUS];
        struct Pokemon reward;
        u8 giveResult;
        u8 choiceCount;

        for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
            if (gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_CIRCUIT
             && !IsLegendarySignCaught(signId))
                return;
        CreateMon(&reward, sign->species, 80, MAX_PER_STAT_IVS, TRUE,
                  Random32(), OT_ID_PLAYER_ID, 0);
        choiceCount = GetVerdantBattleSetRawCount(sign->species);
        if (choiceCount != 0)
            ApplyVerdantGiftBattleSet(&reward, Random() % choiceCount);
        giveResult = GiveMonToPlayer(&reward);
        if (giveResult == MON_CANT_GIVE)
        {
            gSpecialVar_Result = 3;
            return;
        }
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(sign->species), FLAG_SET_SEEN);
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(sign->species), FLAG_SET_CAUGHT);
        MarkLegendarySignCaughtBySpecies(sign->species);
        StringCopy(gStringVar1, gSpeciesNames[sign->species]);
        gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
    }
}

void ChampionsCircuitEnd(void)
{
    LoadPlayerParty();
    HealPlayerParty();
    gSaveBlock2Ptr->frontier.towerNumWins = 0;
    gSaveBlock2Ptr->frontier.towerBattleOutcome = 0;
}
