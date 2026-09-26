#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_util.h"
#include "emerald_champions_battle_sets.h"
#include "emerald_champions_opening.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "starter_choose.h"
#include "string_util.h"
#include "constants/emerald_champions.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/opponents.h"
#include "constants/vars.h"

static void GetOpeningStarterSet(enum Species species, struct EmeraldChampionsBattleSet *preset)
{
    const struct EmeraldChampionsBattleSet *base = GetEmeraldChampionsRawBattleSet(species, 0);
    *preset = base != NULL ? *base : (struct EmeraldChampionsBattleSet){0};
    if (species == SPECIES_CHIKORITA)
        preset->moves[0] = MOVE_GIGA_DRAIN;
    else if (species == SPECIES_TORCHIC)
    {
        static const struct EmeraldChampionsBattleSet sTorchic =
        {
            .moves = {MOVE_FLAMETHROWER, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_SPEED_BOOST,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        *preset = sTorchic;
    }
}

// The player's pair arrives like any wild Pokémon: its natural moves for
// level 5 and no held item. (The rival's opening team keeps its authored set.)
static bool32 CreateOpeningStarter(struct Pokemon *mon, u16 choice)
{
    CreateRandomMonWithIVs(mon, GetStarterPokemon(choice), 5, MAX_PER_STAT_IVS);
    return TRUE;
}

bool32 GiveEmeraldChampionsStarterPair(u16 first, u16 second)
{
    struct Pokemon pair[2];

    // Commit both choices together before starting the rescue.
    if (first >= 3 || second >= 3 || first == second || CalculatePlayerPartyCount() != 0)
        return FALSE;
    if (!CreateOpeningStarter(&pair[0], first) || !CreateOpeningStarter(&pair[1], second))
        return FALSE;

    GiveScriptedMonToPlayer(&pair[0], 0);
    GiveScriptedMonToPlayer(&pair[1], 1);
    VarSet(VAR_STARTER_MON, first);
    VarSet(VAR_EC_SECOND_STARTER, second + 1);
    VarSet(VAR_EC_OPENING_STATE, EC_OPENING_PAIR_GRANTED);
    FlagSet(FLAG_SYS_POKEMON_GET);
    return TRUE;
}

u16 GetEmeraldChampionsSecondStarterIndex(void)
{
    return VarGet(VAR_EC_SECOND_STARTER) - 1;
}

bool32 HasEmeraldChampionsSecondStarter(void)
{
    u16 state = VarGet(VAR_EC_OPENING_STATE);

    return state >= EC_OPENING_PAIR_GRANTED && state <= EC_OPENING_RESCUE_WON
        && VarGet(VAR_EC_SECOND_STARTER) >= 1 && VarGet(VAR_EC_SECOND_STARTER) <= 3;
}

u16 GetEmeraldChampionsRivalStarterIndex(void)
{
    u16 first = VarGet(VAR_STARTER_MON) % 3;

    // Saves from before the paired grant hold one starter; the rival takes the next.
    if (!HasEmeraldChampionsSecondStarter())
        return (first + 1) % 3;
    return 3 - first - GetEmeraldChampionsSecondStarterIndex();
}

void BufferEmeraldChampionsRivalBranch(void)
{
    gSpecialVar_Result = (GetEmeraldChampionsRivalStarterIndex() + 2) % 3;
}

bool32 IsEmeraldChampionsBirchRescueBattle(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
        && VarGet(VAR_EC_OPENING_STATE) == EC_OPENING_PAIR_GRANTED;
}

void CreateEmeraldChampionsBirchRescueParty(void)
{
    // The wild pair chasing Birch: plain level-2 Pokémon with their natural moves.
    static const enum Species sSpecies[] = {SPECIES_POOCHYENA, SPECIES_ZIGZAGOON};

    ZeroEnemyPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(sSpecies); i++)
        CreateRandomMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i], sSpecies[i], 2, MAX_PER_STAT_IVS);
    gPartiesCount[B_TRAINER_OPPONENT_A] = ARRAY_COUNT(sSpecies);
}

void ApplyEmeraldChampionsRegionalRivalSet(struct Pokemon *party, u32 slot, bool32 opening)
{
    enum Species species = GetMonData(&party[slot], MON_DATA_SPECIES);
    const struct EmeraldChampionsBattleSet *base = GetEmeraldChampionsRawBattleSet(species, 0);
    struct EmeraldChampionsBattleSet preset = base != NULL ? *base : (struct EmeraldChampionsBattleSet){0};

    if (opening)
    {
        GetOpeningStarterSet(species, &preset);
        // These are the rival's generated alternatives, not changes to the
        // player's starter presets. The party has no automatic sun setter.
        switch (species)
        {
        case SPECIES_BULBASAUR:
            preset.ability = ABILITY_OVERGROW;
            preset.moves[0] = MOVE_PROTECT;
            break;
        case SPECIES_CHARMANDER:
            preset.ability = ABILITY_BLAZE;
            preset.moves[1] = MOVE_FLAMETHROWER;
            break;
        case SPECIES_ROWLET:
            preset.moves[3] = MOVE_PROTECT;
            break;
        case SPECIES_SOBBLE:
            // Native preparation does not grant Sobble Ice Beam. Mud Shot
            // replaces redundant Water Pulse with legal coverage and control.
            preset.moves[2] = MOVE_MUD_SHOT;
            break;
        default:
            break;
        }
        if (preset.item == ITEM_EVIOLITE)
            preset.item = ITEM_SITRUS_BERRY;
    }
    else
    {
        switch (species)
        {
        case SPECIES_IVYSAUR:
            preset.ability = ABILITY_OVERGROW;
            preset.moves[0] = MOVE_SLEEP_POWDER;
            preset.moves[1] = MOVE_GIGA_DRAIN;
            preset.moves[3] = MOVE_PROTECT;
            break;
        case SPECIES_CHARMELEON:
            preset.ability = ABILITY_BLAZE;
            preset.moves[1] = MOVE_DRAGON_PULSE;
            break;
        case SPECIES_BAYLEEF:
            preset.moves[0] = MOVE_GIGA_DRAIN;
            break;
        case SPECIES_MONFERNO:
            // Fire STAB remains usable beside the rival's Lightning Rod.
            preset.moves[3] = MOVE_FIRE_PUNCH;
            break;
        case SPECIES_PRINPLUP:
            preset.moves[0] = MOVE_HYDRO_PUMP;
            break;
        case SPECIES_DRIZZILE:
            // Drizzile does not inherit Inteleon's Ice coverage. Preserve
            // Sniper/Focus Energy and add legal Electric coverage and speed control.
            preset.moves[2] = MOVE_MUD_SHOT;
            break;
        default:
            break;
        }
        // Campaign battles have no Item Clause. Preserve the canonical item
        // even when a partner also needs it (especially Eviolite).
    }
    ApplyEmeraldChampionsScriptedSet(&party[slot], &preset);
}

// Derive progress from earned victories instead of maintaining a second quest
// state. Existing saves and a previously completed step therefore remain valid.
u16 GetEmeraldChampionsFinaleStage(void)
{
    static const u16 voyageTrainers[] = {
        TRAINER_COLTON, TRAINER_MICAH, TRAINER_THOMAS,
        TRAINER_LEA_AND_JED, TRAINER_NAOMI,
    };
    if (!FlagGet(FLAG_SYS_GAME_CLEAR))
        return EC_FINALE_LEAGUE;
    if (!HasTrainerBeenFought(TRAINER_WALLY_VR_2))
        return EC_FINALE_WALLY;
    for (u32 i = 0; i < ARRAY_COUNT(voyageTrainers); i++)
        if (!HasTrainerBeenFought(voyageTrainers[i]))
            return EC_FINALE_VOYAGE;
    if (!HasTrainerBeenFought(TRAINER_STEVEN) || !FlagGet(FLAG_RECEIVED_AURORA_TICKET))
        return EC_FINALE_STEVEN;
    if (!FlagGet(FLAG_EC_FINALE_DEOXYS_RESOLVED)
     && !FlagGet(FLAG_BATTLED_DEOXYS) && !FlagGet(FLAG_DEFEATED_DEOXYS))
        return EC_FINALE_DEOXYS;
    if (!HasTrainerBeenFought(TRAINER_BUFFEL))
        return EC_FINALE_BUFFEL;
    return EC_FINALE_COMPLETE;
}
