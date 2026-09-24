#include "global.h"
#include "berry.h"
#include "daycare.h"
#include "emerald_champions_opening.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "legendary_signs.h"
#include "mega_stone_rewards.h"
#include "malloc.h"
#include "pokemon.h"
#include "script_menu.h"
#include "starter_choose.h"
#include "string_util.h"
#include "constants/emerald_champions.h"

struct HarvestIngredient { u8 berry; u8 count; };
static const struct
{
    enum Item item;
    u16 flag;
    struct HarvestIngredient recipe[6];
} sBerryStoneTrades[] =
{
    {ITEM_BAXCALIBRITE, FLAG_EC_BERRY_TRADE_BAXCALIBRITE,
        {{BERRY_ID_RAZZ, 6}, {BERRY_ID_BLUK, 6}, {BERRY_ID_NANAB, 4}, {BERRY_ID_WEPEAR, 4}}},
    {ITEM_DRAGONINITE, FLAG_EC_BERRY_TRADE_DRAGONINITE,
        {{BERRY_ID_CHERI, 6}, {BERRY_ID_CHESTO, 6}, {BERRY_ID_ORAN, 6}, {BERRY_ID_PECHA, 6}}},
    {ITEM_TYRANITARITE, FLAG_EC_BERRY_TRADE_TYRANITARITE,
        {{BERRY_ID_POMEG, 4}, {BERRY_ID_KELPSY, 4}, {BERRY_ID_QUALOT, 4},
         {BERRY_ID_HONDEW, 4}, {BERRY_ID_GREPA, 4}, {BERRY_ID_TAMATO, 4}}},
    // A permanent optional encounter invitation; retries never charge again.
    {ITEM_NONE, 0,
        {{BERRY_ID_PINAP, 8}, {BERRY_ID_SITRUS, 8}, {BERRY_ID_LUM, 4}, {BERRY_ID_LEPPA, 8}}},
};
STATIC_ASSERT(ARRAY_COUNT(sBerryStoneTrades) == EC_HARVEST_REWARD_COUNT, HarvestRewardCount);
STATIC_ASSERT(NUM_BERRIES + 1 <= 0x58, HarvestSaveCapacity);

u16 GetHarvestedBerryCount(u8 berry)
{
    return berry && berry <= NUM_BERRIES ? gSaveBlock2Ptr->pokedex.harvestedBerries[berry - 1] : 0;
}

// The pouch count saturates: picking is never refused just because a type's
// record is full, since most berry types are never spent by a trade.
void AddHarvestedBerries(u8 berry, u16 count)
{
    if (berry && berry <= NUM_BERRIES)
        gSaveBlock2Ptr->pokedex.harvestedBerries[berry - 1] = min(EC_HARVEST_LIMIT, GetHarvestedBerryCount(berry) + count);
}

static bool32 RewardClaimed(u32 choice)
{
    if (choice == 3)
        return gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked || IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI);
    return FlagGet(sBerryStoneTrades[choice].flag) || PlayerOwnsItem(sBerryStoneTrades[choice].item);
}

void BuildEmeraldChampionsHarvestChoices(void)
{
    for (u32 berry = 1; berry < NUM_BERRIES; berry++)
    {
        u8 *label = Alloc(32);
        u8 *end = CopyItemName(BerryTypeToItemId(berry), label);
        end = StringAppend(end, COMPOUND_STRING("  x"));
        ConvertIntToDecimalStringN(end, GetHarvestedBerryCount(berry), STR_CONV_MODE_LEFT_ALIGN, 3);
        MultichoiceDynamic_PushElement((struct ListMenuItem){label, berry});
    }
}

void BufferEmeraldChampionsHarvestRecipe(void)
{
    u32 choice = gSpecialVar_0x8004;
    if (choice >= ARRAY_COUNT(sBerryStoneTrades))
        return;
    if (choice == 3)
        StringCopy(gStringVar1, COMPOUND_STRING("Celebi's invitation"));
    else
        CopyItemName(sBerryStoneTrades[choice].item, gStringVar1);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_1}\nHarvested berries: have / need"));
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (!part->count)
            break;
        StringAppend(gStringVar4, i % 2 == 0 ? COMPOUND_STRING("\p") : COMPOUND_STRING("\n"));
        StringAppend(gStringVar4, GetBerryInfo(part->berry)->name);
        StringAppend(gStringVar4, COMPOUND_STRING(": "));
        ConvertIntToDecimalStringN(gStringVar2, GetHarvestedBerryCount(part->berry), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringAppend(gStringVar4, gStringVar2);
        StringAppend(gStringVar4, COMPOUND_STRING(" / "));
        ConvertIntToDecimalStringN(gStringVar2, part->count, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar4, gStringVar2);
    }
    gSpecialVar_0x800B = RewardClaimed(choice);
    StringAppend(gStringVar4, gSpecialVar_0x800B
        ? COMPOUND_STRING("\pAlready claimed. No more berries due.")
        : COMPOUND_STRING("\pBring these to the BERRY MASTER\non ROUTE 123. One of each reward!"));
}

void TradeEmeraldChampionsGardenBerries(void)
{
    u32 choice = gSpecialVar_0x8004;
    gSpecialVar_Result = EC_MEGA_BERRY_TRADE_INVALID;
    if (choice >= ARRAY_COUNT(sBerryStoneTrades))
        return;
    if (RewardClaimed(choice))
    {
        gSpecialVar_Result = EC_MEGA_BERRY_TRADE_ALREADY_DONE;
        return;
    }
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (GetHarvestedBerryCount(part->berry) < part->count)
        {
            gSpecialVar_Result = EC_MEGA_BERRY_TRADE_NOT_ENOUGH;
            return;
        }
    }
    // Delivery before debit/receipt: failed storage must preserve the entire recipe.
    if (choice != 3 && !AddBagItem(sBerryStoneTrades[choice].item, 1))
    {
        gSpecialVar_Result = EC_MEGA_BERRY_TRADE_BAG_FULL;
        return;
    }
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (part->count)
            gSaveBlock2Ptr->pokedex.harvestedBerries[part->berry - 1] -= part->count;
    }
    if (choice == 3)
        gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = TRUE;
    else
        FlagSet(sBerryStoneTrades[choice].flag);
    gSpecialVar_Result = EC_MEGA_BERRY_TRADE_SUCCESS;
}

// gardenCelebiUnlocked: 0 not yet, 1 waiting in the garden, 2 knocked out and lost.
#define GARDEN_CELEBI_LOST 2

void CheckEmeraldChampionsGardenCelebi(void)
{
    gSpecialVar_Result = IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI) ? 2
        : gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked == GARDEN_CELEBI_LOST ? 3
        : gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked ? 1 : 0;
}

// A knocked-out garden Celebi is gone for good, like every static legend.
void LoseEmeraldChampionsGardenCelebi(void)
{
    gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = GARDEN_CELEBI_LOST;
}

// The Champions archive is exactly the Mega Stones the campaign distributes,
// so one bit per entry is also the "every Mega seen" test.
static const u16 sMegaStoneArchive[] =
{
#include "data/emerald_champions_mega_stones.h"
};

STATIC_ASSERT(ARRAY_COUNT(sMegaStoneArchive) == 99, MegaArchiveIsNinetyNine);
STATIC_ASSERT(sizeof(((struct SaveBlock1 *)0)->megasWitnessed) * 8 >= ARRAY_COUNT(sMegaStoneArchive), MegasWitnessedFitsSaveBlock);

u32 EmeraldChampions_GetMegaArchiveCount(void)
{
    return ARRAY_COUNT(sMegaStoneArchive);
}

void EmeraldChampions_RecordMegaWitnessed(u32 item)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMegaStoneArchive); i++)
    {
        if (sMegaStoneArchive[i] != item)
            continue;
        gSaveBlock1Ptr->megasWitnessed[i / 8] |= 1u << (i % 8);
        return;
    }
}

u32 EmeraldChampions_CountMegasWitnessed(void)
{
    u32 count = 0;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMegaStoneArchive); i++)
    {
        if (gSaveBlock1Ptr->megasWitnessed[i / 8] & (1u << (i % 8)))
            count++;
    }
    return count;
}

// Starter Mega Stones follow the starter pair. Each stone has exactly one
// receipt flag, shared by every place that can hand it over:
// - Norman gives, with the Mega Ring, the stones of the pair's final forms that
//   have not reached the player yet (Swampertite when the pair has none);
// - a stone with a world home (a sparkle or a Gym Leader's gift) is handed over
//   there otherwise; the sparkle's object flag or the Leader's receipt is the
//   same flag, so Norman's copy closes that home and a home's copy closes his;
// - the Hoenn stones have no world home: those Norman did not give wait with
//   him until the player shows him that partner's line.
// scripts/verify_mega_stone_rewards.py reads this table. The three Hoenn
// receipts live on bits 0x2B8-0x2BA (include/constants/flags.h).
static const struct
{
    u16 species;
    u16 item;
    u16 flag;
} sStarterMegaStones[] =
{
    {SPECIES_VENUSAUR,   ITEM_VENUSAURITE,    FLAG_ITEM_PETALBURG_CITY_VENUSAURITE},
    {SPECIES_CHARIZARD,  ITEM_CHARIZARDITE_X, FLAG_ITEM_FIERY_PATH_CHARIZARDITE_X},
    {SPECIES_CHARIZARD,  ITEM_CHARIZARDITE_Y, FLAG_EMBER_PATH_CHARIZARDITE_Y},
    {SPECIES_BLASTOISE,  ITEM_BLASTOISINITE,  FLAG_SEASPRAY_CAVE_BLASTOISINITE},
    {SPECIES_MEGANIUM,   ITEM_MEGANIUMITE,    FLAG_ITEM_GRANITE_CAVE_B1F_TM65},
    {SPECIES_FERALIGATR, ITEM_FERALIGITE,     FLAG_RECEIVED_TM03},  // Juan
    {SPECIES_SCEPTILE,   ITEM_SCEPTILITE,     FLAG_EC_MEGA_GIFT_SCEPTILITE},
    {SPECIES_BLAZIKEN,   ITEM_BLAZIKENITE,    FLAG_EC_MEGA_GIFT_BLAZIKENITE},
    {SPECIES_SWAMPERT,   ITEM_SWAMPERTITE,    FLAG_EC_MEGA_GIFT_SWAMPERTITE},
    {SPECIES_EMBOAR,     ITEM_EMBOARITE,      FLAG_RECEIVED_TM08},  // Brawly
    {SPECIES_CHESNAUGHT, ITEM_CHESNAUGHTITE,  FLAG_EMBER_PATH_SMACK_DOWN},
    {SPECIES_DELPHOX,    ITEM_DELPHOXITE,     FLAG_RECEIVED_TM39},  // Roxanne
    {SPECIES_GRENINJA,   ITEM_GRENINJITE,     FLAG_ITEM_ROUTE_119_TM62_ACROBATICS},
};

// BufferNormanMegaGiftKind results; PetalburgCity_Gym/scripts.inc compares these numbers.
enum
{
    NORMAN_MEGA_GIFT_SEVERAL = 0,
    NORMAN_MEGA_GIFT_ONE = 1,
    NORMAN_MEGA_GIFT_FALLBACK = 2,
    NORMAN_MEGA_GIFT_ALREADY_HELD = 3,
};

static bool32 IsHoennStarterMegaStone(u32 i)
{
    return sStarterMegaStones[i].species == SPECIES_SCEPTILE
        || sStarterMegaStones[i].species == SPECIES_BLAZIKEN
        || sStarterMegaStones[i].species == SPECIES_SWAMPERT;
}

static bool32 StarterMegaStoneDelivered(u32 i)
{
    return FlagGet(sStarterMegaStones[i].flag) || PlayerOwnsItem(sStarterMegaStones[i].item);
}

static bool32 IsStarterPairForm(u32 i)
{
    u16 generation = VarGet(VAR_STARTER_GEN);
    enum Species first = GetStarterPokemonForGeneration(VarGet(VAR_STARTER_MON), generation);

    if (sStarterMegaStones[i].species == GetFinalEvolutionForStarter(first))
        return TRUE;
    return HasEmeraldChampionsSecondStarter()
        && sStarterMegaStones[i].species == GetFinalEvolutionForStarter(
            GetStarterPokemonForGeneration(GetEmeraldChampionsSecondStarterIndex(), generation));
}

static bool32 StarterPairHasMegaStone(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
    {
        if (IsStarterPairForm(i))
            return TRUE;
    }
    return FALSE;
}

// Norman's share for this save, delivered or not.
static bool32 IsNormanStarterMegaStone(u32 i)
{
    if (StarterPairHasMegaStone())
        return IsStarterPairForm(i);
    return sStarterMegaStones[i].item == ITEM_SWAMPERTITE;
}

// Next stone Norman still hands over with the Ring, or ITEM_NONE.
u16 GetNormanStarterMegaStone(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
    {
        if (IsNormanStarterMegaStone(i) && !StarterMegaStoneDelivered(i))
            return sStarterMegaStones[i].item;
    }
    return ITEM_NONE;
}

void BufferNormanStarterMegaStone(void)
{
    gSpecialVar_Result = GetNormanStarterMegaStone();
}

// Which line Norman says before handing stones over.
void BufferNormanMegaGiftKind(void)
{
    u32 pending = 0;

    for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
    {
        if (IsNormanStarterMegaStone(i) && !StarterMegaStoneDelivered(i))
            pending++;
    }
    if (pending == 0)
        gSpecialVar_Result = NORMAN_MEGA_GIFT_ALREADY_HELD;
    else if (!StarterPairHasMegaStone())
        gSpecialVar_Result = NORMAN_MEGA_GIFT_FALLBACK;
    else
        gSpecialVar_Result = pending == 1 ? NORMAN_MEGA_GIFT_ONE : NORMAN_MEGA_GIFT_SEVERAL;
}

// A Hoenn stone Norman did not give, for a partner line in the party, or ITEM_NONE.
// Buffers the partner's name and the stone's name for his line.
u16 GetNormanPartnerMegaStone(void)
{
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES);

        if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
            continue;
        for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
        {
            if (!IsHoennStarterMegaStone(i) || StarterMegaStoneDelivered(i)
                || GetFinalEvolutionForStarter(GetEggSpecies(species)) != sStarterMegaStones[i].species)
                continue;
            StringCopy(gStringVar1, GetSpeciesName(species));
            CopyItemName(sStarterMegaStones[i].item, gStringVar2);
            return sStarterMegaStones[i].item;
        }
    }
    return ITEM_NONE;
}

void BufferNormanPartnerMegaStone(void)
{
    gSpecialVar_Result = GetNormanPartnerMegaStone();
}

// Called after the item popup: closes every other home of the stone in VAR_0x8004.
void MarkStarterMegaStoneReceived(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
    {
        if (sStarterMegaStones[i].item == gSpecialVar_0x8004)
            FlagSet(sStarterMegaStones[i].flag);
    }
}

// The Ring and every stone Norman still owes must fit together.
bool32 CanReceiveNormanMegaGift(void)
{
    struct ItemSlot gifts[1 + ARRAY_COUNT(sStarterMegaStones)] = {{ITEM_MEGA_RING, 1}};
    u32 count = 1;

    for (u32 i = 0; i < ARRAY_COUNT(sStarterMegaStones); i++)
    {
        if (IsNormanStarterMegaStone(i) && !StarterMegaStoneDelivered(i))
            gifts[count++] = (struct ItemSlot){sStarterMegaStones[i].item, 1};
    }
    return CheckBagHasSpaceForItemBundle(gifts, count);
}
