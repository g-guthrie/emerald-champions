#ifndef GUARD_EMERALD_CHAMPIONS_STUDIO_H
#define GUARD_EMERALD_CHAMPIONS_STUDIO_H

#include "global.h"

#if EC_HEADLESS_FIXTURES
void EmeraldChampionsStudioPoll(void);
void EmeraldChampionsStudioText(const u8 *text);
#else
static inline void EmeraldChampionsStudioPoll(void) {}
static inline void EmeraldChampionsStudioText(const u8 *text) {(void)text;}
#endif

#endif
