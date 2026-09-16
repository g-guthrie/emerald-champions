#ifndef GUARD_FIELD_MOVE_H
#define GUARD_FIELD_MOVE_H

#include "global.h"
#include "constants/field_move.h"

struct FieldMoveUnlock
{
   bool32 (*isUnlockedFunc)(enum FieldMove);
   const u8 *lockedMessage;
};

enum FieldMoveUnlockType
{
    CANT_UNLOCK,
    ALWAYS_UNLOCKED,
    BADGE_UNLOCK,
    FIELD_MOVE_UNLOCK_COUNT
};

struct FieldMoveInfo
{
    bool32 (*fieldMoveFunc)(void);
    enum FieldMoveUnlockType unlockType:3;
    enum Move moveID:11;
    u32 partyMsgID:7;
    u32 arg:8;
    u32 hideIfLocked:1;
    // Emerald Champions: obstacle moves are used by pressing A on the
    // obstacle, so they never appear in a Pokémon's party menu.
    u32 hideInPartyMenu:1;
    // Emerald Champions: a field move never occupies a battle move slot. A move
    // marked here is offered in the party menu to any member whose species could
    // learn it, exactly as an obstacle move picks its user.
    u32 capabilityInPartyMenu:1;
    u32 padding:1;
};

extern const struct FieldMoveInfo gFieldMoveInfo[];
extern const struct FieldMoveUnlock gFieldMoveUnlocks[];

u32 FieldMove_GetUserSlot(enum FieldMove fieldMove, bool32 doUnlockedCheck);
bool32 SpeciesCanLearnFieldMove(enum Species species, enum Move move);

static inline bool32 SetUpFieldMove(enum FieldMove fieldMove)
{
    return gFieldMoveInfo[fieldMove].fieldMoveFunc();
}

static inline bool32 IsFieldMoveUnlocked(enum FieldMove fieldMove)
{
    return gFieldMoveUnlocks[gFieldMoveInfo[fieldMove].unlockType].isUnlockedFunc(fieldMove);
}

static inline const u8 *FieldMove_GetLockedMessage(enum FieldMove fieldMove)
{
    return gFieldMoveUnlocks[gFieldMoveInfo[fieldMove].unlockType].lockedMessage;
}

static inline enum Move FieldMove_GetMoveId(enum FieldMove fieldMove)
{
    return gFieldMoveInfo[fieldMove].moveID;
}

static inline u32 FieldMove_GetPartyMsgID(enum FieldMove fieldMove)
{
    return gFieldMoveInfo[fieldMove].partyMsgID;
}

static inline bool32 FieldMove_IsCapabilityBased(enum FieldMove fieldMove)
{
    return gFieldMoveInfo[fieldMove].capabilityInPartyMenu;
}

static inline bool32 FieldMove_IsVisible(enum FieldMove fieldMove)
{
    if (gFieldMoveInfo[fieldMove].hideInPartyMenu)
        return FALSE;
    return !gFieldMoveInfo[fieldMove].hideIfLocked || IsFieldMoveUnlocked(fieldMove);
}

#endif //GUARD_FIELD_MOVE_H
