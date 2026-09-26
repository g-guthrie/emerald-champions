#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

#include "constants/species.h"
#include "constants/items.h"

extern bool8 gBikeCyclingChallenge;
extern u8 gBikeCollisions;
extern u16 gScrollableMultichoice_ScrollOffset;

u8 GetLeadMonIndex(void);
u16 GetPCBoxToSendMon(void);
bool8 InMultiPartnerRoom(void);
void UpdateTrainerFansAfterLinkBattle(void);
void IncrementBirthIslandRockStepCount(void);
bool8 AbnormalWeatherHasExpired(void);
bool8 ShouldDoBrailleRegicePuzzle(void);
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
void UpdateFrontierManiac(u16 daysSince);
void UpdateFrontierGambler(u16 daysSince);
void RefundRetiredFrontierGamblerBet(void);
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
void GiveFlowerShopBerryBundle(void);
bool32 CanReceiveWeatherInstituteRocks(void);
bool32 CanReceiveLanetteDolls(void);
bool32 CanReceiveBerryPair(void);
bool32 CanReceiveFrontierReward(void);
bool32 CanReceiveLatiStones(void);
bool32 CanReceiveNormanMegaGift(void);
bool32 CanReceiveGoGogglesGift(void);
void GiveEmeraldChampionsStarterBattleItems(void);
#if EC_HEADLESS_FIXTURES
bool32 IsScrollableMultichoiceHeadlessActive(u16 menu);
#endif
#if TESTING
u8 GiveEmeraldChampionsGameCornerPokemonForTesting(enum Species species, u16 flag);
u8 GiveEmeraldChampionsPreparedPokemonForTesting(enum Species species, u8 level);
#endif


void BufferEmeraldChampionsBondingPreview(void);
void ApplyEmeraldChampionsBonding(void);

bool32 IsEmeraldChampionsFreeCatalogueItem(enum Item item);


void ConvertEmeraldChampionsFiniteReward(void);

bool32 IsEmeraldChampionsBattleItemUnlocked(enum Item item);
void EmeraldChampions_UnlockBattleItem(enum Item item);


// Specials restored for the Inclement Emerald map scripts.
void PutZigzagoonInPlayerParty(void);
bool8 IsItemFossil(void);
bool8 DoesPlayerHaveFossil(void);
void FossilToSpecies(void);
void Bag_ChooseItem(void);
void Bag_ChoosePokeBall(void);
void ChangePokeBall(void);
void ChangeMonSpecies(void);
enum Species ScriptGetPartyMonSpecies(void);
bool8 CheckSpeciesInParty(void);
bool8 FoundBlackGlasses(void);
bool8 CheckMagikarpBattle(void);
void GetStaticEncounterLevel(void);
void CreateEventLegalEnemyMon(void);
void SetSpeciesAndEggMove(void);
void SetGiftEggMove(void);
bool8 LeadMonHasEffortRibbon(void);
void GiveLeadMonEffortRibbon(void);
bool8 GetDiancieFriendshipScore(void);
u8 CountPlayerMuseumPaintings(void);
bool32 IsTrainerRegistered(void);

void CheckPlayerCaughtSpecies(void);

#endif // GUARD_FIELD_SPECIALS_H
