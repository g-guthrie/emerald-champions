#include "global.h"
#include "util.h"
#include "main.h"
#include "event_data.h"
#include "easy_chat.h"
#include "script.h"
#include "battle_special.h"
#include "string_util.h"
#include "new_game.h"
#include "mystery_gift.h"
#include "constants/mystery_gift.h"

static EWRAM_DATA bool32 sStatsEnabled = FALSE;

static void ClearSavedWonderNews(void);

#define CALC_CRC(data) CalcCRC16WithTable((void *)&(data), sizeof(data))

void ClearMysteryGift(void)
{
    InitQuestionnaireWords();
}

struct WonderNews *GetSavedWonderNews(void)
{
    return NULL;
}

struct WonderCard *GetSavedWonderCard(void)
{
    return NULL;
}

struct WonderCardMetadata *GetSavedWonderCardMetadata(void)
{
    return NULL;
}

struct WonderNewsMetadata *GetSavedWonderNewsMetadata(void)
{
    return NULL;
}

u16 *GetQuestionnaireWordsPtr(void)
{
    return NULL;
}

// Equivalent to ClearSavedWonderCardAndRelated, but nothing else to clear
void ClearSavedWonderNewsAndRelated(void)
{
    ClearSavedWonderNews();
}

bool32 SaveWonderNews(const struct WonderNews *news)
{
    return FALSE;
}

bool32 ValidateSavedWonderNews(void)
{
    return FALSE;
}


bool32 IsSendingSavedWonderNewsAllowed(void)
{
    return FALSE;
}

static void ClearSavedWonderNews(void)
{
}


bool32 IsWonderNewsSameAsSaved(const u8 *news)
{
    return FALSE;
}

void ClearSavedWonderCardAndRelated(void)
{
#if FREE_BATTLE_TOWER_E_READER == FALSE
    ClearEReaderTrainer(&gSaveBlock2Ptr->frontier.ereaderTrainer);
#endif //FREE_BATTLE_TOWER_E_READER
}

bool32 SaveWonderCard(const struct WonderCard *card)
{
    return FALSE;
}

bool32 ValidateSavedWonderCard(void)
{
    return FALSE;
}


bool32 IsSendingSavedWonderCardAllowed(void)
{
    return FALSE;
}


u16 GetWonderCardFlagID(void)
{

    return 0;
}

void DisableWonderCardSending(struct WonderCard *card)
{
    if (card->sendType == SEND_TYPE_ALLOWED)
        card->sendType = SEND_TYPE_DISALLOWED;
}

static bool32 IsWonderCardFlagIDInValidRange(u16 flagId)
{
    if (flagId >= WONDER_CARD_FLAG_OFFSET && flagId < WONDER_CARD_FLAG_OFFSET + NUM_WONDER_CARD_FLAGS)
        return TRUE;

    return FALSE;
}

static const u16 sReceivedGiftFlags[] =
{
    FLAG_RECEIVED_AURORA_TICKET,
    FLAG_RECEIVED_MYSTIC_TICKET,
    FLAG_RECEIVED_OLD_SEA_MAP,
    FLAG_WONDER_CARD_UNUSED_1,
    FLAG_WONDER_CARD_UNUSED_2,
    FLAG_WONDER_CARD_UNUSED_3,
    FLAG_WONDER_CARD_UNUSED_4,
    FLAG_WONDER_CARD_UNUSED_5,
    FLAG_WONDER_CARD_UNUSED_6,
    FLAG_WONDER_CARD_UNUSED_7,
    FLAG_WONDER_CARD_UNUSED_8,
    FLAG_WONDER_CARD_UNUSED_9,
    FLAG_WONDER_CARD_UNUSED_10,
    FLAG_WONDER_CARD_UNUSED_11,
    FLAG_WONDER_CARD_UNUSED_12,
    FLAG_WONDER_CARD_UNUSED_13,
    FLAG_WONDER_CARD_UNUSED_14,
    FLAG_WONDER_CARD_UNUSED_15,
    FLAG_WONDER_CARD_UNUSED_16,
    FLAG_WONDER_CARD_UNUSED_17,
};

bool32 IsSavedWonderCardGiftNotReceived(void)
{
    u16 value = GetWonderCardFlagID();
    if (!IsWonderCardFlagIDInValidRange(value))
        return FALSE;

    // If flag is set, player has received gift from this card
    if (FlagGet(sReceivedGiftFlags[value - WONDER_CARD_FLAG_OFFSET]) == TRUE)
        return FALSE;

    return TRUE;
}

static int GetNumStampsInMetadata(const struct WonderCardMetadata *data, int size)
{
    int numStamps = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        if (data->stampData[STAMP_ID][i] && data->stampData[STAMP_SPECIES][i] != SPECIES_NONE)
            numStamps++;
    }

    return numStamps;
}

static bool32 IsStampInMetadata(const struct WonderCardMetadata *metadata, const u16 *stamp, int maxStamps)
{
    int i;
    for (i = 0; i < maxStamps; i++)
    {
        if (metadata->stampData[STAMP_ID][i] == stamp[STAMP_ID])
            return TRUE;
        if (metadata->stampData[STAMP_SPECIES][i] == stamp[STAMP_SPECIES])
            return TRUE;
    }

    return FALSE;
}


bool32 MysteryGift_TrySaveStamp(const u16 *stamp)
{

    return FALSE;
}

#define GAME_DATA_VALID_VAR 0x101
#define GAME_DATA_VALID_GIFT_TYPE_1 (1 << 2)
#define GAME_DATA_VALID_GIFT_TYPE_2 (1 << 9)

void MysteryGift_LoadLinkGameData(struct MysteryGiftLinkGameData *data, bool32 isWonderNews)
{
}

bool32 MysteryGift_ValidateLinkGameData(const struct MysteryGiftLinkGameData *data, bool32 isWonderNews)
{
    if (data->validationVar != GAME_DATA_VALID_VAR)
        return FALSE;

    if (!(data->validationFlag1 & 1))
        return FALSE;

    if (!(data->validationFlag2 & 1))
        return FALSE;

    if (!isWonderNews)
    {
        if (!(data->validationGiftType1 & GAME_DATA_VALID_GIFT_TYPE_1))
            return FALSE;

        if (!(data->validationGiftType2 & (GAME_DATA_VALID_GIFT_TYPE_2 | 0x180)))
            return FALSE;
    }

    return TRUE;
}

u32 MysteryGift_CompareCardFlags(const u16 *flagId, const struct MysteryGiftLinkGameData *data, const void *unused)
{
    // Has a Wonder Card already?
    if (data->flagId == 0)
        return HAS_NO_CARD;

    // Has this Wonder Card already?
    if (*flagId == data->flagId)
        return HAS_SAME_CARD;

    // Player has a different Wonder Card
    return HAS_DIFF_CARD;
}

// This is referenced by the Mystery Gift server, but the instruction it's referenced in is never used,
// so the return values here are never checked by anything.
u32 MysteryGift_CheckStamps(const u16 *stamp, const struct MysteryGiftLinkGameData *data, const void *unused)
{
    int stampsMissing = data->maxStamps - GetNumStampsInMetadata(&data->cardMetadata, data->maxStamps);

    // Has full stamp card?
    if (stampsMissing == 0)
        return 1;

    // Already has stamp?
    if (IsStampInMetadata(&data->cardMetadata, stamp, data->maxStamps))
        return 3;

    // Only 1 empty stamp left?
    if (stampsMissing == 1)
        return 4;

    // This is a new stamp
    return 2;
}

bool32 MysteryGift_DoesQuestionnaireMatch(const struct MysteryGiftLinkGameData *data, const u16 *words)
{
    int i;
    for (i = 0; i < NUM_QUESTIONNAIRE_WORDS; i++)
    {
        if (data->questionnaireWords[i] != words[i])
            return FALSE;
    }

    return TRUE;
}

static int GetNumStampsInLinkData(const struct MysteryGiftLinkGameData *data)
{
    return GetNumStampsInMetadata(&data->cardMetadata, data->maxStamps);
}

u16 MysteryGift_GetCardStatFromLinkData(const struct MysteryGiftLinkGameData *data, u32 stat)
{
    switch (stat)
    {
    case CARD_STAT_BATTLES_WON:
        return data->cardMetadata.battlesWon;
    case CARD_STAT_BATTLES_LOST:
        return data->cardMetadata.battlesLost;
    case CARD_STAT_NUM_TRADES:
        return data->cardMetadata.numTrades;
    case CARD_STAT_NUM_STAMPS:
        return GetNumStampsInLinkData(data);
    case CARD_STAT_MAX_STAMPS:
        return data->maxStamps;
    default:
        AGB_ASSERT(0);
        return 0;
    }
}


u16 MysteryGift_GetCardStat(u32 stat)
{

    AGB_ASSERT(0);
    return 0;
}

void MysteryGift_DisableStats(void)
{
    sStatsEnabled = FALSE;
}

bool32 MysteryGift_TryEnableStatsByFlagId(u16 flagId)
{
    sStatsEnabled = FALSE;
    if (flagId == 0)
        return FALSE;

    if (!ValidateSavedWonderCard())
        return FALSE;


    sStatsEnabled = TRUE;
    return TRUE;
}

void MysteryGift_TryIncrementStat(u32 stat, u32 trainerId)
{
}


// Wonder News is removed, so there is never a saved news item to reward.
// Kept because an FRLG map script still calls this special.
u16 WonderNews_GetRewardInfo(void)
{
    return NEWS_REWARD_NONE;
}
