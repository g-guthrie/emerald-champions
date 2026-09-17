#ifndef GUARD_CONTEST_H
#define GUARD_CONTEST_H

// Contests are retired (see the September 2026 dead-code cleanup). Everything
// that made a Contest run is gone; what is left here is the smallest stub that
// keeps the battle animation engine compiling. src/battle_anim*.c and
// src/battle_gfx_sfx_util.c branch on IsContest() (defined in battle_anim.c,
// TRUE whenever an animation plays outside a battle) and read the move-anim
// block through gContestResources. Nothing allocates gContestResources any
// more, so it stays NULL and those branches stay unreachable, exactly as they
// were while contests existed but none was running.

struct ContestMoveAnimData
{
    enum Species species;
    enum Species targetSpecies;
    bool8 hasTargetAnim:1;
    u8 isShiny:1;
    u8 targetIsShiny:1;
    u8 contestant;
    u32 personality;
    u32 otId;
    u32 targetPersonality;
};

struct ContestResources
{
    struct ContestMoveAnimData *moveAnim;
};

extern struct ContestResources *gContestResources;

// battle_anim.c
void LoadContestBgAfterMoveAnim(void);
bool8 IsSpeciesNotUnown(enum Species species);

#endif //GUARD_CONTEST_H
