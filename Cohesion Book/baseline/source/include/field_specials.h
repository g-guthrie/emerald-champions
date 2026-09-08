#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

#include "constants/species.h"

extern bool8 gBikeCyclingChallenge;
extern u8 gBikeCollisions;
extern u16 gScrollableMultichoice_ScrollOffset;

u8 GetLeadMonIndex(void);
bool8 IsDestinationBoxFull(void);
u16 GetPCBoxToSendMon(void);
bool8 InMultiPartnerRoom(void);
void UpdateTrainerFansAfterLinkBattle(void);
void IncrementBirthIslandRockStepCount(void);
bool8 AbnormalWeatherHasExpired(void);
bool8 ShouldDoBrailleRegicePuzzle(void);
bool32 ShouldDoWallyCall(void);
bool32 ShouldDoScottFortreeCall(void);
bool32 ShouldDoScottBattleFrontierCall(void);
bool32 ShouldDoRoxanneCall(void);
bool32 ShouldDoRivalRayquazaCall(void);
bool32 CountSSTidalStep(u16 delta);
enum SSTidalLocation GetSSTidalLocation(s8 *mapGroup, s8 *mapNum, s16 *x, s16 *y);
void ShowScrollableMultichoice(void);
void FrontierGamblerSetWonOrLost(bool8 won);
u8 TryGainNewFanFromCounter(u8 incrementId);
bool8 InPokemonCenter(void);
void SetShoalItemFlag(u16 unused);
void UpdateFrontierManiac(u16 daysSince);
void UpdateFrontierGambler(u16 daysSince);
void ResetCyclingRoadChallengeData(void);
bool8 UsedPokemonCenterWarp(void);
void ResetFanClub(void);
bool8 ShouldShowBoxWasFullMessage(void);
void SetPCBoxToSendMon(u8 boxId);
void PreparePartyForSkyBattle(void);
void GetObjectPosition(u16*, u16*, u32, u32);
bool32 CheckObjectAtXY(u32, u32);
bool32 CheckPartyHasSpecies(enum Species);
bool8 CutMoveRuinValleyCheck(void);
void CutMoveOpenDottedHoleDoor(void);
void IsEmeraldChampionsGameCornerPokemonClaimed(void);
void GiveEmeraldChampionsGameCornerPokemon(void);
void GiveEmeraldChampionsPreparedPokemon(void);
void BufferSelectedMonEmeraldChampionsStatPointSummary(void);
void BufferSelectedMonEmeraldChampionsStatPointDetail(void);
void AdjustSelectedMonEmeraldChampionsStatPoints(void);
void ResetSelectedMonEmeraldChampionsStatPoints(void);
#if EC_HEADLESS_FIXTURES
bool32 IsScrollableMultichoiceHeadlessActive(u16 menu);
#endif
#if TESTING
u8 GiveEmeraldChampionsGameCornerPokemonForTesting(enum Species species, u16 flag);
u8 GiveEmeraldChampionsPreparedPokemonForTesting(enum Species species, u8 level);
#endif

#endif // GUARD_FIELD_SPECIALS_H
