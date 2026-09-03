#ifndef GUARD_EMERALD_CHAMPIONS_AI_H
#define GUARD_EMERALD_CHAMPIONS_AI_H

#include "battle_ai_main.h"

AiScoreFunc GetEmeraldChampionsDynamicAiFunc(u16 trainerId);

s32 AI_EC_TrickRoomDiscipline(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);
s32 AI_EC_FlanneryAfterYou(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);
s32 AI_EC_QuincyTruant(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);
s32 AI_EC_SnowScreen(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);
s32 AI_EC_RedirectionSetup(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);
s32 AI_EC_WallaceTerrain(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score);

#endif // GUARD_EMERALD_CHAMPIONS_AI_H
