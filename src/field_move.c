#include "global.h"
#include "event_data.h"
#include "field_move.h"
#include "fldeff.h"
#include "fldeff_misc.h"
#include "party_menu.h"
#include "pokemon.h"
#include "constants/battle.h"
#include "strings.h"
#include "string_util.h"
#include "move.h"
#include "constants/field_move.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/party_menu.h"

static bool32 IsAlwaysFalse(enum FieldMove fieldMove)
{
    return FALSE;
}

static bool32 IsAlwaysTrue(enum FieldMove fieldMove)
{
    return TRUE;
}

static const u16 sEmeraldHiddenMoveLicenses[FIELD_MOVES_COUNT] =
{
    [FIELD_MOVE_CUT]        = FLAG_RECEIVED_HM_CUT,
    [FIELD_MOVE_FLASH]      = FLAG_RECEIVED_HM_FLASH,
    [FIELD_MOVE_ROCK_SMASH] = FLAG_RECEIVED_HM_ROCK_SMASH,
    [FIELD_MOVE_STRENGTH]   = FLAG_RECEIVED_HM_STRENGTH,
    [FIELD_MOVE_SURF]       = FLAG_RECEIVED_HM_SURF,
    [FIELD_MOVE_FLY]        = FLAG_RECEIVED_HM_FLY,
    [FIELD_MOVE_DIVE]       = FLAG_RECEIVED_HM_DIVE,
    [FIELD_MOVE_WATERFALL]  = FLAG_RECEIVED_HM_WATERFALL,
};

bool32 FieldMove_IsHM(enum FieldMove fieldMove)
{
    return (u32)fieldMove < ARRAY_COUNT(sEmeraldHiddenMoveLicenses)
        && sEmeraldHiddenMoveLicenses[fieldMove] != 0;
}

static bool32 HasBadgeForFieldMove(enum FieldMove fieldMove)
{
    u16 licenseFlag = sEmeraldHiddenMoveLicenses[fieldMove];

    if (!FlagGet(gFieldMoveInfo[fieldMove].arg + FLAG_BADGE01_GET))
        return FALSE;
    if (licenseFlag == 0)
        return TRUE;
    return FlagGet(licenseFlag);
}

// Called after an HM action fails its badge, license, or party check.
// Report the reason without granting or changing access.
void BufferFieldMoveUnlockRequirement(void)
{
    static const u8 badgeNames[][14] = {
        _("Stone Badge"), _("Knuckle Badge"), _("Dynamo Badge"), _("Heat Badge"),
        _("Balance Badge"), _("Feather Badge"), _("Mind Badge"), _("Rain Badge"),
    };
    enum FieldMove fieldMove = gSpecialVar_0x8004;
    u32 badge = gFieldMoveInfo[fieldMove].arg;

    StringCopy(gStringVar1, GetMoveName(FieldMove_GetMoveId(fieldMove)));
    StringCopy(gStringVar2, badgeNames[badge]);
    if (!FlagGet(FLAG_BADGE01_GET + badge))
        gSpecialVar_Result = 0;
    else if (!IsFieldMoveUnlocked(fieldMove))
        gSpecialVar_Result = 1;
    else
        gSpecialVar_Result = 2; // Unlocked, but no compatible party member.
}

const struct FieldMoveUnlock gFieldMoveUnlocks[FIELD_MOVE_UNLOCK_COUNT] =
{
    [CANT_UNLOCK] =
    {
        .isUnlockedFunc = IsAlwaysFalse,
        .lockedMessage = gText_EmptyString2,
    },
    [ALWAYS_UNLOCKED] =
    {
        .isUnlockedFunc = IsAlwaysTrue,
        .lockedMessage = gText_EmptyString2,
    },
    [BADGE_UNLOCK] =
    {
        .isUnlockedFunc = HasBadgeForFieldMove,
        .lockedMessage = gText_CantUseUntilNewBadge,
    },
};

#define FLAG_TO_BADGE(flag) flag - FLAG_BADGE01_GET

// Field licenses replace HM moveslots, but a capable party member is still
// needed. Optional non-HM moves also use this species capability check.
bool32 SpeciesCanLearnFieldMove(enum Species species, enum Move move)
{
    const struct LevelUpMove *learnset;

    if (CanLearnTeachableMove(species, move))
        return TRUE;
    learnset = GetSpeciesLevelUpLearnset(species);
    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        if (learnset[i].move == move)
            return TRUE;
    }
    return FALSE;
}

u32 FieldMove_GetUserSlot(enum FieldMove fieldMove, bool32 doUnlockedCheck)
{
    // An HM needs its license, badge, and a compatible non-Egg party member.
    // Prefer a member that knows the move for the field animation, but knowing
    // it is not required.
    if (FieldMove_IsHM(fieldMove))
    {
        enum Move move = FieldMove_GetMoveId(fieldMove);
        u32 capable = PARTY_SIZE;

        if (!IsFieldMoveUnlocked(fieldMove))
            return PARTY_SIZE;
        for (u32 i = 0; i < PARTY_SIZE; i++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
            enum Species species = GetMonData(mon, MON_DATA_SPECIES);

            if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
                continue;
            if (SpeciesCanLearnFieldMove(species, move) && MonKnowsMove(mon, move))
                return i;
            if (capable == PARTY_SIZE && SpeciesCanLearnFieldMove(species, move))
                capable = i;
        }
        return capable;
    }

    enum Move move = FieldMove_GetMoveId(fieldMove);
    u32 fallback = PARTY_SIZE;

    if (doUnlockedCheck && !IsFieldMoveUnlocked(fieldMove))
        return PARTY_SIZE;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES);

        if (species == SPECIES_NONE)
            break;
        if (GetMonData(mon, MON_DATA_IS_EGG))
            continue;
        if (MonKnowsMove(mon, move))
            return i;
        if (FieldMove_IsCapabilityBased(fieldMove)
         && fallback == PARTY_SIZE && SpeciesCanLearnFieldMove(species, move))
            fallback = i;
    }
    if (fallback != PARTY_SIZE && IsFieldMoveUnlocked(fieldMove))
        return fallback;
    return PARTY_SIZE;
}

const struct FieldMoveInfo gFieldMoveInfo[FIELD_MOVES_COUNT] =
{
    [FIELD_MOVE_CUT] =
    {
        .fieldMoveFunc = SetUpFieldMove_Cut,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_CUT,
        .partyMsgID = PARTY_MSG_NOTHING_TO_CUT,
        .arg = FLAG_TO_BADGE(FLAG_BADGE01_GET),
        .hideIfLocked = TRUE,
        .capabilityInPartyMenu = TRUE, // Optional grass clearing keeps Inclement's party-menu action.
    },

    [FIELD_MOVE_FLASH] =
    {
        .fieldMoveFunc = SetUpFieldMove_Flash,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_FLASH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE02_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_ROCK_SMASH] =
    {
        .fieldMoveFunc = SetUpFieldMove_RockSmash,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_ROCK_SMASH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE03_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_STRENGTH] =
    {
        .fieldMoveFunc = SetUpFieldMove_Strength,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_STRENGTH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE04_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_SURF] =
    {
        .fieldMoveFunc = SetUpFieldMove_Surf,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_SURF,
        .partyMsgID = PARTY_MSG_CANT_SURF_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE05_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_FLY] =
    {
        .fieldMoveFunc = SetUpFieldMove_Fly,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_FLY,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE06_GET),
        .hideIfLocked = TRUE,
        // The Flight Beacon owns Fly: it is called from the Bag, never from a moveslot.
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_DIVE] =
    {
        .fieldMoveFunc = SetUpFieldMove_Dive,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_DIVE,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE07_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_WATERFALL] =
    {
        .fieldMoveFunc = SetUpFieldMove_Waterfall,
        .unlockType = BADGE_UNLOCK,
        .moveID = MOVE_WATERFALL,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .arg = FLAG_TO_BADGE(FLAG_BADGE08_GET),
        .hideIfLocked = TRUE,
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_TELEPORT] =
    {
        .fieldMoveFunc = SetUpFieldMove_Teleport,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_TELEPORT,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .capabilityInPartyMenu = TRUE,
    },

    [FIELD_MOVE_DIG] =
    {
        .fieldMoveFunc = SetUpFieldMove_Dig,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_DIG,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        // The Sealed Chamber wall asks for Dig itself; the Escape Rope covers the rest.
        .hideInPartyMenu = TRUE,
    },

    [FIELD_MOVE_SECRET_POWER] =
    {
        .fieldMoveFunc = SetUpFieldMove_SecretPower,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_SECRET_POWER,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_MILK_DRINK] =
    {
        .fieldMoveFunc = SetUpFieldMove_SoftBoiled,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_MILK_DRINK,
        .partyMsgID = PARTY_MSG_NOT_ENOUGH_HP,
    },

    [FIELD_MOVE_SOFT_BOILED] =
    {
        .fieldMoveFunc = SetUpFieldMove_SoftBoiled,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_SOFT_BOILED,
        .partyMsgID = PARTY_MSG_NOT_ENOUGH_HP,
    },

    [FIELD_MOVE_SWEET_SCENT] =
    {
        .fieldMoveFunc = SetUpFieldMove_SweetScent,
        .unlockType = ALWAYS_UNLOCKED,
        .moveID = MOVE_SWEET_SCENT,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .capabilityInPartyMenu = TRUE,
    },
    [FIELD_MOVE_ROCK_CLIMB] =
    {
        .fieldMoveFunc = SetUpFieldMove_RockClimb,
#if OW_ROCK_CLIMB_FIELD_MOVE
        .unlockType = ALWAYS_UNLOCKED,
#else
        .unlockType = CANT_UNLOCK,
#endif
        .moveID = MOVE_ROCK_CLIMB,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .hideIfLocked = TRUE,
    },
    [FIELD_MOVE_DEFOG] =
    {
        .fieldMoveFunc = SetUpFieldMove_Defog,
#if OW_DEFOG_FIELD_MOVE
        .unlockType = ALWAYS_UNLOCKED,
#else
        .unlockType = CANT_UNLOCK,
#endif
        .moveID = MOVE_DEFOG,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
        .hideIfLocked = TRUE,
    },
};
