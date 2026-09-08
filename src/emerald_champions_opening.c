#include "global.h"
#include "battle.h"
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
#include "constants/vars.h"

static const struct EmeraldChampionsBattleSet sRescueSets[] =
{
    {
        .moves = {MOVE_CRUNCH, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH, MOVE_HELPING_HAND},
        .item = ITEM_FOCUS_SASH,
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RATTLED,
        .statPoints = {2, 32, 0, 0, 0, 32},
    },
    {
        .moves = {MOVE_BELLY_DRUM, MOVE_EXTREME_SPEED, MOVE_SEED_BOMB, MOVE_PROTECT},
        .item = ITEM_SITRUS_BERRY,
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GLUTTONY,
        .statPoints = {2, 32, 0, 0, 0, 32},
    },
};

static void GetOpeningStarterSet(enum Species species, struct EmeraldChampionsBattleSet *preset)
{
    *preset = gEmeraldChampionsDefaultBattleSets[species];
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
            .statPoints = {2, 0, 0, 32, 0, 32},
        };
        *preset = sTorchic;
    }
}

static bool32 CreateOpeningStarter(struct Pokemon *mon, u16 choice)
{
    enum Species species = GetStarterPokemon(choice);
    struct EmeraldChampionsBattleSet preset;

    GetOpeningStarterSet(species, &preset);
    CreateRandomMonWithIVs(mon, species, 5, MAX_PER_STAT_IVS);
    return ApplyEmeraldChampionsScriptedSet(mon, &preset) == EC_BATTLE_SET_SUCCESS;
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
    RecordPlayerPartyMonHeldItemForRestoration(0);
    RecordPlayerPartyMonHeldItemForRestoration(1);
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

    return state >= EC_OPENING_PAIR_GRANTED && state <= EC_OPENING_COMPLETE
        && VarGet(VAR_EC_SECOND_STARTER) >= 1 && VarGet(VAR_EC_SECOND_STARTER) <= 3;
}

u16 GetEmeraldChampionsRivalStarterIndex(void)
{
    u16 first = VarGet(VAR_STARTER_MON);
    u16 second = GetEmeraldChampionsSecondStarterIndex();

    return 3 - first - second;
}

void BufferEmeraldChampionsRivalBranch(void)
{
    gSpecialVar_Result = (GetEmeraldChampionsRivalStarterIndex() + 2) % 3;
}

bool32 IsEmeraldChampionsBirchRescueBattle(void)
{
    return !IS_FRLG && (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
        && VarGet(VAR_EC_OPENING_STATE) == EC_OPENING_PAIR_GRANTED;
}

void CreateEmeraldChampionsBirchRescueParty(void)
{
    static const enum Species sSpecies[] = {SPECIES_POOCHYENA, SPECIES_ZIGZAGOON};

    ZeroEnemyPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(sSpecies); i++)
    {
        CreateRandomMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i], sSpecies[i], 5, MAX_PER_STAT_IVS);
        ApplyEmeraldChampionsScriptedSet(&gParties[B_TRAINER_OPPONENT_A][i], &sRescueSets[i]);
    }
    gPartiesCount[B_TRAINER_OPPONENT_A] = ARRAY_COUNT(sSpecies);
}

static bool32 RivalPartnerHoldsItem(struct Pokemon *party, u32 slot, enum Item item)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        if (i != slot && GetMonData(&party[i], MON_DATA_SPECIES) != SPECIES_NONE
            && GetMonData(&party[i], MON_DATA_HELD_ITEM) == item)
            return TRUE;
    return FALSE;
}

void ApplyEmeraldChampionsRegionalRivalSet(struct Pokemon *party, u32 slot, bool32 opening)
{
    enum Species species = GetMonData(&party[slot], MON_DATA_SPECIES);
    struct EmeraldChampionsBattleSet preset = gEmeraldChampionsDefaultBattleSets[species];

    if (opening)
    {
        GetOpeningStarterSet(species, &preset);
        if (preset.item == ITEM_EVIOLITE)
            preset.item = ITEM_SITRUS_BERRY;
    }
    else
    {
        switch (species)
        {
        case SPECIES_IVYSAUR:
            preset.moves[0] = MOVE_SLEEP_POWDER;
            preset.moves[1] = MOVE_GIGA_DRAIN;
            preset.moves[3] = MOVE_PROTECT;
            break;
        case SPECIES_CHARMELEON:
            preset.moves[1] = MOVE_DRAGON_PULSE;
            break;
        case SPECIES_BAYLEEF:
            preset.moves[0] = MOVE_GIGA_DRAIN;
            break;
        default:
            break;
        }
        if (RivalPartnerHoldsItem(party, slot, preset.item))
        {
            static const enum Item sOffenseItems[] =
            {
                ITEM_LIFE_ORB, ITEM_EXPERT_BELT, ITEM_SITRUS_BERRY,
                ITEM_LUM_BERRY, ITEM_LEFTOVERS, ITEM_COVERT_CLOAK,
            };
            static const enum Item sSupportItems[] =
            {
                ITEM_COVERT_CLOAK, ITEM_MENTAL_HERB, ITEM_LEFTOVERS,
                ITEM_LUM_BERRY, ITEM_SITRUS_BERRY,
            };
            bool32 support = max(preset.statPoints[1], preset.statPoints[3]) < 16;
            const enum Item *items = support ? sSupportItems : sOffenseItems;
            u32 count = support ? ARRAY_COUNT(sSupportItems) : ARRAY_COUNT(sOffenseItems);

            for (u32 i = 0; i < count; i++)
            {
                if (!RivalPartnerHoldsItem(party, slot, items[i]))
                {
                    preset.item = items[i];
                    break;
                }
            }
        }
    }
    ApplyEmeraldChampionsScriptedSet(&party[slot], &preset);
}

void GiveEmeraldChampionsOpeningBalls(void)
{
    gSpecialVar_Result = 0;
    if (AddBagItem(ITEM_POKE_BALL, 20))
        gSpecialVar_Result = 1;
    else if (AddPCItem(ITEM_POKE_BALL, 20))
        gSpecialVar_Result = 2;
    if (gSpecialVar_Result != 0)
    {
        VarSet(VAR_EC_OPENING_STATE, EC_OPENING_PRE_RIVAL_READY);
        VarSet(VAR_BIRCH_LAB_STATE, 3);
    }
}

void TopUpEmeraldChampionsOpeningBalls(void)
{
    u32 count = CountTotalItemQuantityInBag(ITEM_POKE_BALL);

    gSpecialVar_Result = 0;
    if (FlagGet(FLAG_DEFEATED_RIVAL_ROUTE103)
        || VarGet(VAR_EC_OPENING_STATE) != EC_OPENING_PRE_RIVAL_READY)
        return;
    for (u32 i = 0; i < PC_ITEMS_COUNT; i++)
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_POKE_BALL)
            count += gSaveBlock1Ptr->pcItems[i].quantity;
    if (count >= 10)
        return;
    if (AddBagItem(ITEM_POKE_BALL, 20 - count))
        gSpecialVar_Result = 1;
    else if (AddPCItem(ITEM_POKE_BALL, 20 - count))
        gSpecialVar_Result = 2;
}

void BufferEmeraldChampionsStarterNames(void)
{
    StringCopy(gStringVar1, GetSpeciesName(GetStarterPokemon(VarGet(VAR_STARTER_MON))));
    StringCopy(gStringVar2, GetSpeciesName(GetStarterPokemon(GetEmeraldChampionsSecondStarterIndex())));
}

void SelectEmeraldChampionsStarterForNaming(void)
{
    u16 choice = gSpecialVar_0x8004 == 0 ? VarGet(VAR_STARTER_MON) : GetEmeraldChampionsSecondStarterIndex();
    enum Species species = GetStarterPokemon(choice);

    gSpecialVar_Result = FALSE;
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == species)
        {
            gSpecialVar_0x8004 = i;
            StringCopy(gStringVar1, GetSpeciesName(species));
            gSpecialVar_Result = TRUE;
            return;
        }
    }
}
