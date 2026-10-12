#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_ai_record.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "battle_special.h"
#include "battle_z_move.h"
#include "data.h"
#include "event_data.h"
#include "frontier_util.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "menu.h"
#include "palette.h"
#include "recorded_battle.h"
#include "string_util.h"
#include "strings.h"
#include "test_runner.h"
#include "text.h"
#include "trainer_hill.h"
#include "trainer_slide.h"
#include "trainer_tower.h"
#include "window.h"
#include "line_break.h"
#include "constants/abilities.h"
#include "constants/battle_dome.h"
#include "constants/battle_string_ids.h"
#include "constants/frontier_util.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/trainer_hill.h"
#include "constants/weather.h"

// Battle text wording: the game shows vanilla Emerald's text, while the expansion's
// battle tests (TESTING) still match the upstream modern wording. Any string that
// differs is written as VANILLA_TEXT("modern text", "vanilla text"): the test build
// compiles the first argument, the game build the second. Vanilla text is copied
// from pokeemerald byte for byte where the message exists there; text for later
// mechanics follows the same style, with explicit \n, \l and \p breaks so every
// page fits the two-line, 208px message box.
#if TESTING
#define VANILLA_TEXT(modern, vanilla) modern
#else
#define VANILLA_TEXT(modern, vanilla) vanilla
#endif

struct BattleWindowText
{
    u8 fillValue;
    u8 fontId;
    u8 x;
    u8 y;
    union {
        struct {
            DEPRECATED("Use color.background instead") u8 bgColor;
            DEPRECATED("Use color.foreground instead") u8 fgColor;
            DEPRECATED("Use color.shadow instead") u8 shadowColor;
            DEPRECATED("Use color.accent instead") u8 accentColor;
        };
        union TextColor color;
    };
    u8 letterSpacing;
    u8 lineSpacing;
    u8 speed;
};

#if TESTING
EWRAM_DATA u16 sBattlerAbilities[MAX_BATTLERS_COUNT] = {0};
#else
static EWRAM_DATA u16 sBattlerAbilities[MAX_BATTLERS_COUNT] = {0};
#endif
EWRAM_DATA struct BattleMsgData *gBattleMsgDataPtr = NULL;

// todo: make some of those names less vague: attacker/target vs pkmn, etc.

static const u8 sText_EmptyString4[] = _("");

const u8 gText_PkmnShroudedInMist[] = _(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} surrounds itself with a protective mist!",
                                                     "{B_ATK_PREFIX2} became\nshrouded in MIST!"));
const u8 gText_PkmnGettingPumped[] = _(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is getting pumped!",
                                                    "{B_DEF_NAME_WITH_PREFIX} is getting\npumped!"));
const u8 gText_PkmnsXPreventsSwitching[] = _(VANILLA_TEXT("{B_BUFF1} is preventing switching out with its {B_LAST_ABILITY} Ability!\p",
                                                          "{B_BUFF1}'s {B_LAST_ABILITY}\nprevents switching!\p"));
const u8 gText_StatSharply[] = _(VANILLA_TEXT(" sharply", "sharply "));
const u8 gText_StatRose[] = _("rose!");
const u8 gText_StatFell[] = _("fell!");
const u8 gText_DefendersStatRose[] = _(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1} rose{B_BUFF2}!",
                                                    "{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1}\n{B_BUFF2}rose!"));
static const u8 sText_GotAwaySafely[] = _(VANILLA_TEXT("{PLAY_SE SE_FLEE}You got away safely!\p",
                                                       "{PLAY_SE SE_FLEE}Got away safely!\p"));
static const u8 sText_PlayerDefeatedLinkTrainer[] = _(VANILLA_TEXT("You defeated {B_LINK_OPPONENT1_NAME}!",
                                                                   "Player defeated\n{B_LINK_OPPONENT1_NAME}!"));
static const u8 sText_TwoLinkTrainersDefeated[] = _(VANILLA_TEXT("You defeated {B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!",
                                                                 "Player beat {B_LINK_OPPONENT1_NAME}\nand {B_LINK_OPPONENT2_NAME}!"));
static const u8 sText_PlayerLostAgainstLinkTrainer[] = _(VANILLA_TEXT("You lost against {B_LINK_OPPONENT1_NAME}!",
                                                                      "Player lost against\n{B_LINK_OPPONENT1_NAME}!"));
static const u8 sText_PlayerLostToTwo[] = _(VANILLA_TEXT("You lost to {B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!",
                                                         "Player lost to {B_LINK_OPPONENT1_NAME}\nand {B_LINK_OPPONENT2_NAME}!"));
static const u8 sText_PlayerBattledToDrawLinkTrainer[] = _(VANILLA_TEXT("You battled to a draw against {B_LINK_OPPONENT1_NAME}!",
                                                                        "Player battled to a draw against\n{B_LINK_OPPONENT1_NAME}!"));
static const u8 sText_PlayerBattledToDrawVsTwo[] = _(VANILLA_TEXT("You battled to a draw against {B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!",
                                                                  "Player battled to a draw against\n{B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!"));
static const u8 sText_WildFled[] = _("{PLAY_SE SE_FLEE}{B_LINK_OPPONENT1_NAME} fled!"); //not in gen 5+, replaced with match was forfeited text
static const u8 sText_TwoWildFled[] = _(VANILLA_TEXT("{PLAY_SE SE_FLEE}{B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME} fled!",
                                                     "{PLAY_SE SE_FLEE}{B_LINK_OPPONENT1_NAME} and\n{B_LINK_OPPONENT2_NAME} fled!")); //not in gen 5+, replaced with match was forfeited text
static const u8 sText_PlayerDefeatedLinkTrainerTrainer1[] = _(VANILLA_TEXT("You defeated {B_TRAINER1_NAME_WITH_CLASS}!\p",
                                                                           "Player defeated\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}!\p"));
static const u8 sText_OpponentMon1Appeared[] = _("{B_OPPONENT_MON1_NAME} appeared!\p");
static const u8 sText_WildPkmnAppeared[] = _(VANILLA_TEXT("You encountered a wild {B_OPPONENT_MON1_NAME}!\p",
                                                          "Wild {B_OPPONENT_MON1_NAME} appeared!\p"));
static const u8 sText_LegendaryPkmnAppeared[] = _(VANILLA_TEXT("You encountered a wild {B_OPPONENT_MON1_NAME}!\p",
                                                               "Wild {B_OPPONENT_MON1_NAME} appeared!\p"));
static const u8 sText_WildPkmnAppearedPause[] = _(VANILLA_TEXT("You encountered a wild {B_OPPONENT_MON1_NAME}!{PAUSE 127}",
                                                               "Wild {B_OPPONENT_MON1_NAME} appeared!{PAUSE 127}"));
static const u8 sText_TwoWildPkmnAppeared[] = _(VANILLA_TEXT("Oh! A wild {B_OPPONENT_MON1_NAME} and {B_OPPONENT_MON2_NAME} appeared!\p",
                                                             "Wild {B_OPPONENT_MON1_NAME} and\n{B_OPPONENT_MON2_NAME} appeared!\p"));
static const u8 sText_GhostAppearedCantId[] = _("The GHOST appeared!\pDarn!\nThe GHOST can't be ID'd!\p");
static const u8 sText_TheGhostAppeared[] = _("The GHOST appeared!\p");
static const u8 sText_Trainer1WantsToBattle[] = _(VANILLA_TEXT("You are challenged by {B_TRAINER1_NAME_WITH_CLASS}!\p",
                                                               "{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\nwould like to battle!\p"));
static const u8 sText_LinkTrainerWantsToBattle[] = _(VANILLA_TEXT("You are challenged by {B_LINK_OPPONENT1_NAME}!",
                                                                  "{B_LINK_OPPONENT1_NAME}\nwants to battle!"));
static const u8 sText_TwoLinkTrainersWantToBattle[] = _(VANILLA_TEXT("You are challenged by {B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!",
                                                                     "{B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}\nwant to battle!"));
static const u8 sText_Trainer1SentOutPkmn[] = _(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} sent out {B_OPPONENT_MON1_NAME}!",
                                                             "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} sent\nout {B_OPPONENT_MON1_NAME}!"));
static const u8 sText_Trainer1SentOutTwoPkmn[] = _(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} sent out {B_OPPONENT_MON1_NAME} and {B_OPPONENT_MON2_NAME}!",
                                                                "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} sent\nout {B_OPPONENT_MON1_NAME} and {B_OPPONENT_MON2_NAME}!"));
static const u8 sText_Trainer1SentOutPkmn2[] = _(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} sent out {B_BUFF1}!",
                                                              "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} sent\nout {B_BUFF1}!"));
static const u8 sText_LinkTrainerIntroSendOutPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} sent out {B_LINK_OPPONENT_MON1_NAME}!",
                                                                     "{B_LINK_OPPONENT1_NAME} sent out\n{B_LINK_OPPONENT_MON1_NAME}!"));
static const u8 sText_LinkTrainerSentOutPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} sent out {B_BUFF1}!",
                                                                "{B_LINK_OPPONENT1_NAME} sent out\n{B_BUFF1}!"));
static const u8 sText_LinkTrainer2SentOutPkmn2[] = _(VANILLA_TEXT("{B_LINK_OPPONENT2_NAME} sent out {B_BUFF1}!",
                                                                  "{B_LINK_OPPONENT2_NAME} sent out\n{B_BUFF1}!"));
static const u8 sText_LinkTrainerSentOutTwoPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} sent out {B_OPPONENT_MON1_NAME} and {B_OPPONENT_MON2_NAME}!",
                                                                   "{B_LINK_OPPONENT1_NAME} sent out\n{B_OPPONENT_MON1_NAME} and {B_OPPONENT_MON2_NAME}!"));
static const u8 sText_TwoLinkTrainersIntroSendOutPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} sent out {B_LINK_OPPONENT_MON1_NAME}! {B_LINK_OPPONENT2_NAME} sent out {B_LINK_OPPONENT_MON2_NAME}!",
                                                                         "{B_LINK_OPPONENT1_NAME} sent out {B_LINK_OPPONENT_MON1_NAME}!\n{B_LINK_OPPONENT2_NAME} sent out {B_LINK_OPPONENT_MON2_NAME}!"));
static const u8 sText_LinkTrainerSentOutPkmn2[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} sent out {B_LINK_OPPONENT_MON2_NAME}!",
                                                                 "{B_LINK_OPPONENT1_NAME} sent out\n{B_LINK_OPPONENT_MON2_NAME}!"));
static const u8 sText_LinkTrainerMultiSentOutPkmn[] = _(VANILLA_TEXT("{B_LINK_SCR_TRAINER_NAME} sent out {B_BUFF1}!",
                                                                     "{B_LINK_SCR_TRAINER_NAME} sent out\n{B_BUFF1}!"));
static const u8 sText_GoPkmn[] = _("Go! {B_PLAYER_MON1_NAME}!");
static const u8 sText_GoTwoPkmn[] = _(VANILLA_TEXT("Go! {B_PLAYER_MON1_NAME} and {B_PLAYER_MON2_NAME}!",
                                                   "Go! {B_PLAYER_MON1_NAME} and\n{B_PLAYER_MON2_NAME}!"));
static const u8 sText_GoPkmn2[] = _("Go! {B_BUFF1}!");
static const u8 sText_DoItPkmn[] = _(VANILLA_TEXT("You're in charge, {B_BUFF1}!", "Do it! {B_BUFF1}!"));
static const u8 sText_GoForItPkmn[] = _("Go for it, {B_BUFF1}!");
static const u8 sText_BeCarefulPkmn[] = _("Be careful, {B_PLAYER_MON1_NAME}!");
static const u8 sText_JustALittleMorePkmn[] = _(VANILLA_TEXT("Just a little more! Hang in there, {B_BUFF1}!",
                                                             "Just a little more!\nHang in there, {B_BUFF1}!")); //currently unused, will require code changes
static const u8 sText_YourFoesWeakGetEmPkmn[] = _(VANILLA_TEXT("Your opponent's weak! Get 'em, {B_BUFF1}!",
                                                               "Your foe's weak!\nGet 'em, {B_BUFF1}!"));
static const u8 sText_LinkPartnerSentOutPkmn1GoPkmn[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} sent out {B_LINK_PLAYER_MON1_NAME}! Go, {B_LINK_PLAYER_MON2_NAME}!",
                                                                       "{B_LINK_PARTNER_NAME} sent out {B_LINK_PLAYER_MON1_NAME}!\nGo! {B_LINK_PLAYER_MON2_NAME}!"));
static const u8 sText_LinkPartnerSentOutPkmn2GoPkmn[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} sent out {B_LINK_PLAYER_MON2_NAME}! Go, {B_LINK_PLAYER_MON1_NAME}!",
                                                                       "{B_LINK_PARTNER_NAME} sent out {B_LINK_PLAYER_MON2_NAME}!\nGo! {B_LINK_PLAYER_MON1_NAME}!"));
static const u8 sText_LinkPartnerSentOutPkmn1[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} sent out {B_BUFF1}!",
                                                                 "{B_LINK_PARTNER_NAME} sent out\n{B_BUFF1}!"));
static const u8 sText_LinkPartnerSentOutPkmn2[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} sent out {B_BUFF1}!",
                                                                 "{B_LINK_PARTNER_NAME} sent out\n{B_BUFF1}!"));
static const u8 sText_LinkPartnerWithdrewPkmn1[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} withdrew {B_LINK_PLAYER_MON1_NAME}!",
                                                                  "{B_LINK_PARTNER_NAME} withdrew\n{B_LINK_PLAYER_MON1_NAME}!"));
static const u8 sText_LinkPartnerWithdrewPkmn2[] = _(VANILLA_TEXT("{B_LINK_PARTNER_NAME} withdrew {B_LINK_PLAYER_MON2_NAME}!",
                                                                  "{B_LINK_PARTNER_NAME} withdrew\n{B_LINK_PLAYER_MON2_NAME}!"));
static const u8 sText_PkmnSwitchOut[] = _(VANILLA_TEXT("{B_BUFF1}, switch out! Come back!",
                                                       "{B_BUFF1}, switch out!\nCome back!")); //currently unused, I believe its used for when you switch on a Pokémon in shift mode
static const u8 sText_PkmnThatsEnough[] = _(VANILLA_TEXT("{B_BUFF1}, that's enough! Come back!",
                                                         "{B_BUFF1}, that's enough!\nCome back!"));
static const u8 sText_PkmnComeBack[] = _("{B_BUFF1}, come back!");
static const u8 sText_PkmnOkComeBack[] = _(VANILLA_TEXT("OK, {B_BUFF1}! Come back!", "{B_BUFF1}, OK!\nCome back!"));
static const u8 sText_PkmnGoodComeBack[] = _(VANILLA_TEXT("Good job, {B_BUFF1}! Come back!",
                                                          "{B_BUFF1}, good!\nCome back!"));
static const u8 sText_Trainer1WithdrewPkmn[] = _(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} withdrew {B_BUFF1}!",
                                                              "{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\nwithdrew {B_BUFF1}!"));
static const u8 sText_Trainer2WithdrewPkmn[] = _(VANILLA_TEXT("{B_TRAINER2_NAME_WITH_CLASS} withdrew {B_BUFF1}!",
                                                              "{B_TRAINER2_CLASS} {B_TRAINER2_NAME}\nwithdrew {B_BUFF1}!"));
static const u8 sText_LinkTrainer1WithdrewPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT1_NAME} withdrew {B_BUFF1}!",
                                                                  "{B_LINK_OPPONENT1_NAME} withdrew\n{B_BUFF1}!"));
static const u8 sText_LinkTrainer2WithdrewPkmn[] = _(VANILLA_TEXT("{B_LINK_OPPONENT2_NAME} withdrew {B_BUFF1}!",
                                                                  "{B_LINK_OPPONENT2_NAME} withdrew\n{B_BUFF1}!"));
static const u8 sText_WildPkmnPrefix[] = _(VANILLA_TEXT("The wild ", "Wild "));
static const u8 sText_FoePkmnPrefix[] = _(VANILLA_TEXT("The opposing ", "Foe "));
static const u8 sText_WildPkmnPrefixLower[] = _(VANILLA_TEXT("the wild ", "Wild "));
static const u8 sText_FoePkmnPrefixLower[] = _(VANILLA_TEXT("the opposing ", "Foe "));
static const u8 sText_EmptyString8[] = _("");
static const u8 sText_FoePkmnPrefix2[] = _(VANILLA_TEXT("Opposing", "Foe"));
static const u8 sText_AllyPkmnPrefix[] = _("Ally");
static const u8 sText_FoePkmnPrefix3[] = _(VANILLA_TEXT("Opposing", "Foe"));
static const u8 sText_AllyPkmnPrefix2[] = _("Ally");
static const u8 sText_FoePkmnPrefix4[] = _(VANILLA_TEXT("Opposing", "Foe"));
static const u8 sText_AllyPkmnPrefix3[] = _("Ally");
static const u8 sText_AttackerUsedX[] = _(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} used {B_BUFF3}!",
                                                       "{B_ATK_NAME_WITH_PREFIX} used\n{B_BUFF3}!"));
static const u8 sText_ExclamationMark[] = _("!");
static const u8 sText_ExclamationMark2[] = _("!");
static const u8 sText_ExclamationMark3[] = _("!");
static const u8 sText_ExclamationMark4[] = _("!");
static const u8 sText_ExclamationMark5[] = _("!");
static const u8 sText_HP[] = _("HP");
static const u8 sText_Attack[] = _(VANILLA_TEXT("Attack", "ATTACK"));
static const u8 sText_Defense[] = _(VANILLA_TEXT("Defense", "DEFENSE"));
static const u8 sText_Speed[] = _(VANILLA_TEXT("Speed", "SPEED"));
static const u8 sText_SpAttack[] = _(VANILLA_TEXT("Sp. Atk", "SP. ATK"));
static const u8 sText_SpDefense[] = _(VANILLA_TEXT("Sp. Def", "SP. DEF"));
static const u8 sText_Accuracy[] = _("accuracy");
static const u8 sText_Evasiveness[] = _("evasiveness");

const u8 *const gStatNamesTable[NUM_BATTLE_STATS] =
{
    [STAT_HP]      = sText_HP,
    [STAT_ATK]     = sText_Attack,
    [STAT_DEF]     = sText_Defense,
    [STAT_SPEED]   = sText_Speed,
    [STAT_SPATK]   = sText_SpAttack,
    [STAT_SPDEF]   = sText_SpDefense,
    [STAT_ACC]     = sText_Accuracy,
    [STAT_EVASION] = sText_Evasiveness,
};
const u8 *const gPokeblockWasTooXStringTable[FLAVOR_COUNT] =
{
    [FLAVOR_SPICY]  = COMPOUND_STRING("was too spicy!"),
    [FLAVOR_DRY]    = COMPOUND_STRING("was too dry!"),
    [FLAVOR_SWEET]  = COMPOUND_STRING("was too sweet!"),
    [FLAVOR_BITTER] = COMPOUND_STRING("was too bitter!"),
    [FLAVOR_SOUR]   = COMPOUND_STRING("was too sour!"),
};

static const u8 sText_Someones[] = _("someone's");
static const u8 sText_Lanettes[] = _("LANETTE's"); //no decapitalize until it is everywhere
static const u8 sText_Bills[] = _("BILL's");
static const u8 sText_EnigmaBerry[] = _("ENIGMA BERRY"); //no decapitalize until it is everywhere
static const u8 sText_BerrySuffix[] = _(" BERRY"); //no decapitalize until it is everywhere
const u8 gText_EmptyString3[] = _("");

static const u8 sText_TwoInGameTrainersDefeated[] = _(VANILLA_TEXT("You defeated {B_TRAINER1_NAME_WITH_CLASS} and {B_TRAINER2_NAME_WITH_CLASS}!\p",
                                                                   "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} and\n{B_TRAINER2_CLASS} {B_TRAINER2_NAME}\lwere defeated!\p"));

// New battle strings.
const u8 gText_drastically[] = _(VANILLA_TEXT(" drastically", "drastically "));
const u8 gText_severely[] = _("severely ");
static const u8 sText_TerrainReturnedToNormal[] = _("The terrain returned to normal!"); // Unused

const u8 *const gBattleStringsTable[STRINGID_COUNT] =
{
    [STRINGID_TRAINER1LOSETEXT]                     = COMPOUND_STRING("{B_TRAINER1_LOSE_TEXT}"),
    [STRINGID_PKMNGAINEDEXP]                        = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} gained{B_BUFF2} {B_BUFF3} EXP. Points!\p",
                                                                                   "{B_BUFF1} gained{B_BUFF2}\n{B_BUFF3} EXP. Points!\p")),
    [STRINGID_PKMNGREWTOLV]                         = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} grew to Lv. {B_BUFF2}!{WAIT_SE}\p",
                                                                                   "{B_BUFF1} grew to\nLV. {B_BUFF2}!{WAIT_SE}\p")),
    [STRINGID_PKMNLEARNEDMOVE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} learned {B_BUFF2}!{WAIT_SE}\p",
                                                                                   "{B_BUFF1} learned\n{B_BUFF2}!{WAIT_SE}\p")),
    [STRINGID_TRYTOLEARNMOVE1]                      = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} wants to learn the move {B_BUFF2}.\p",
                                                                                   "{B_BUFF1} is trying to\nlearn {B_BUFF2}.\p")),
    [STRINGID_TRYTOLEARNMOVE2]                      = COMPOUND_STRING(VANILLA_TEXT("However, {B_BUFF1} already knows four moves.\p",
                                                                                   "But, {B_BUFF1} can't learn\nmore than four moves.\p")),
    [STRINGID_TRYTOLEARNMOVE3]                      = COMPOUND_STRING(VANILLA_TEXT("Should another move be forgotten and replaced with {B_BUFF2}?",
                                                                                   "Delete a move to make\nroom for {B_BUFF2}?")),
    [STRINGID_PKMNFORGOTMOVE]                       = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} forgot {B_BUFF2}…\p",
                                                                                   "{B_BUFF1} forgot\n{B_BUFF2}.\p")),
    [STRINGID_STOPLEARNINGMOVE]                     = COMPOUND_STRING(VANILLA_TEXT("{PAUSE 32}Do you want to give up on having {B_BUFF1} learn {B_BUFF2}?",
                                                                                   "{PAUSE 32}Stop learning\n{B_BUFF2}?")),
    [STRINGID_DIDNOTLEARNMOVE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} did not learn {B_BUFF2}.\p",
                                                                                   "{B_BUFF1} did not learn\n{B_BUFF2}.\p")),
    [STRINGID_PKMNLEARNEDMOVE2]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} learned {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} learned\n{B_BUFF1}!")),
    [STRINGID_PKMNPROTECTEDITSELF]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} protected itself!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}\nprotected itself!")),
    [STRINGID_ITDOESNTAFFECT]                       = COMPOUND_STRING(VANILLA_TEXT("It doesn't affect {B_DEF_NAME_WITH_PREFIX2}…",
                                                                                   "It doesn't affect\n{B_DEF_NAME_WITH_PREFIX}…")),
    [STRINGID_ITDOESNTAFFECTSCR]                    = COMPOUND_STRING(VANILLA_TEXT("It doesn't affect {B_SCR_NAME_WITH_PREFIX2}…",
                                                                                   "It doesn't affect\n{B_SCR_NAME_WITH_PREFIX}…")),
    [STRINGID_BATTLERFAINTED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} fainted!\p",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}\nfainted!\p")),
    [STRINGID_PLAYERGOTMONEY]                       = COMPOUND_STRING(VANILLA_TEXT("You got ¥{B_BUFF1} for winning!\p",
                                                                                   "{B_PLAYER_NAME} got ¥{B_BUFF1}\nfor winning!\p")),
    [STRINGID_PLAYERWHITEOUT]                       = COMPOUND_STRING(VANILLA_TEXT("You have no more POKéMON that can fight!\p",
                                                                                   "{B_PLAYER_NAME} is out of\nusable POKéMON!\p")),
    [STRINGID_PLAYERWHITEOUT2_WILD]                 = COMPOUND_STRING(VANILLA_TEXT("You panicked and dropped ¥{B_BUFF1}…",
                                                                                   "{B_PLAYER_NAME} panicked and lost\n¥{B_BUFF1}…")),
    [STRINGID_PLAYERWHITEOUT2_TRAINER]              = COMPOUND_STRING(VANILLA_TEXT("You gave ¥{B_BUFF1} to the winner…",
                                                                                   "{B_PLAYER_NAME} paid ¥{B_BUFF1}\nto the winner…")),
    [STRINGID_PLAYERWHITEOUT3]                      = COMPOUND_STRING(VANILLA_TEXT("You were overwhelmed by your defeat!",
                                                                                   "{B_PLAYER_NAME} whited out!{PAUSE_UNTIL_PRESS}")),
    [STRINGID_PREVENTSESCAPE]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} prevents escape with {B_SCR_ABILITY}!\p",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} prevents\nescape with {B_SCR_ABILITY}!\p")),
    [STRINGID_HITXTIMES]                            = COMPOUND_STRING(VANILLA_TEXT("The POKéMON was hit {B_BUFF1} time{B_BUFF2}!",
                                                                                   "Hit {B_BUFF1} time(s)!")),
    [STRINGID_PKMNFELLASLEEP]                       = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} fell asleep!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}\nfell asleep!")),
    [STRINGID_PKMNMADESLEEP]                        = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} made {B_EFF_NAME_WITH_PREFIX2} sleep!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nmade {B_EFF_NAME_WITH_PREFIX} sleep!")), //not in gen 5+, ability popup
    [STRINGID_PKMNALREADYASLEEP]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is already asleep!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is\nalready asleep!")),
    [STRINGID_PKMNALREADYASLEEP2]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is already asleep!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is\nalready asleep!")),
    [STRINGID_PKMNWASPOISONED]                      = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was poisoned!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}\nwas poisoned!")),
    [STRINGID_PKMNPOISONEDBY]                       = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was poisoned by {B_SCR_NAME_WITH_PREFIX2}'s {B_BUFF1}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\npoisoned {B_EFF_NAME_WITH_PREFIX}!")), //not in gen 5+, ability popup
    [STRINGID_PKMNHURTBYPOISON]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by its poisoning!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby poison!")),
    [STRINGID_PKMNALREADYPOISONED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is already poisoned!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is already\npoisoned.")),
    [STRINGID_PKMNBADLYPOISONED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was badly poisoned!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} is badly\npoisoned!")),
    [STRINGID_PKMNENERGYDRAINED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} had its energy drained!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} had its\nenergy drained!")),
    [STRINGID_PKMNWASBURNED]                        = COMPOUND_STRING("{B_EFF_NAME_WITH_PREFIX} was burned!"),
    [STRINGID_PKMNBURNEDBY]                         = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} burned {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nburned {B_EFF_NAME_WITH_PREFIX}!")), //not in gen 5+, ability popup
    [STRINGID_PKMNHURTBYBURN]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by its burn!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby its burn!")),
    [STRINGID_PKMNWASFROZEN]                        = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was frozen solid!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was\nfrozen solid!")),
    [STRINGID_PKMNFROZENBY]                         = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} froze {B_EFF_NAME_WITH_PREFIX2} solid!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nfroze {B_EFF_NAME_WITH_PREFIX} solid!")), //not in gen 5+, ability popup
    [STRINGID_PKMNISFROZEN]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is frozen solid!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is\nfrozen solid!")),
    [STRINGID_PKMNWASDEFROSTED]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} thawed out!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was\ndefrosted!")),
    [STRINGID_PKMNWASDEFROSTEDBY]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE} melted the ice!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was\ndefrosted by {B_CURRENT_MOVE}!")),
    [STRINGID_PKMNWASPARALYZED]                     = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} is paralyzed, so it may be unable to move!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} is paralyzed!\nIt may be unable to move!")),
    [STRINGID_PKMNWASPARALYZEDBY]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} paralyzed {B_EFF_NAME_WITH_PREFIX2}, so it may be unable to move!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nparalyzed {B_EFF_NAME_WITH_PREFIX}!\lIt may be unable to move!")), //not in gen 5+, ability popup
    [STRINGID_PKMNISPARALYZED]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} couldn't move because it's paralyzed!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is paralyzed!\nIt can't move!")),
    [STRINGID_PKMNISALREADYPARALYZED]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is already paralyzed!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is\nalready paralyzed!")),
    [STRINGID_PKMNHEALEDPARALYSIS]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was cured of paralysis!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was\nhealed of paralysis!")),
    [STRINGID_STATSWONTINCREASE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} won't go any higher!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nwon't go higher!")),
    [STRINGID_STATSWONTDECREASE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} won't go any lower!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nwon't go lower!")),
    [STRINGID_PKMNISCONFUSED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is confused!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is\nconfused!")),
    [STRINGID_PKMNHEALEDCONFUSION]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} snapped out of its confusion!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} snapped\nout of confusion!")),
    [STRINGID_PKMNWASCONFUSED]                      = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} became confused!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} became\nconfused!")),
    [STRINGID_PKMNALREADYCONFUSED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is already confused!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is\nalready confused!")),
    [STRINGID_PKMNFELLINLOVE]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} fell in love!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}\nfell in love!")),
    [STRINGID_PKMNINLOVE]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is in love with {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is in love\nwith {B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNIMMOBILIZEDBYLOVE]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is immobilized by love!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is\nimmobilized by love!")),
    [STRINGID_PKMNCHANGEDTYPE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} transformed into the {B_BUFF1} type!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} transformed\ninto the {B_BUFF1} type!")),
    [STRINGID_PKMNFLINCHED]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} flinched and couldn't move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} flinched!")),
    [STRINGID_PKMNREGAINEDHEALTH]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s HP was restored.",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} regained\nhealth!")),
    [STRINGID_PKMNHPFULL]                           = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s HP is full!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s\nHP is full!")),
    [STRINGID_PKMNRAISEDSPDEF]                      = COMPOUND_STRING(VANILLA_TEXT("Light Screen made {B_ATK_TEAM2} side stronger against special moves!",
                                                                                   "{B_ATK_PREFIX2}'s {B_CURRENT_MOVE}\nraised SP. DEF!")),
    [STRINGID_PKMNRAISEDDEF]                        = COMPOUND_STRING(VANILLA_TEXT("Reflect made {B_ATK_TEAM2} side stronger against physical moves!",
                                                                                   "{B_ATK_PREFIX2}'s {B_CURRENT_MOVE}\nraised DEFENSE!")),
    [STRINGID_PKMNAURORAVEIL]                       = COMPOUND_STRING(VANILLA_TEXT("Aurora Veil made {B_ATK_TEAM2} side stronger against physical and special moves!",
                                                                                   "{B_ATK_PREFIX2}'s {B_CURRENT_MOVE}\nraised DEFENSE and SP. DEF!")),
    [STRINGID_PKMNCOVEREDBYVEIL]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TEAM1} side became cloaked in a mystical veil!",
                                                                                   "{B_ATK_PREFIX2}'s party is covered\nby a veil!")),
    [STRINGID_PKMNUSEDSAFEGUARD]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is protected by Safeguard!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s party is protected\nby SAFEGUARD!")),
    [STRINGID_PKMNSAFEGUARDEXPIRED]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TEAM1} side is no longer protected by the mystical veil!",
                                                                                   "{B_ATK_PREFIX3}'s party is no longer\nprotected by SAFEGUARD!")),
    [STRINGID_PKMNWENTTOSLEEP]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} went to sleep!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} went\nto sleep!")), //not in gen 5+
    [STRINGID_PKMNSLEPTHEALTHY]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} slept and restored its HP!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} slept and\nbecame healthy!")),
    [STRINGID_PKMNWHIPPEDWHIRLWIND]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} whipped up a whirlwind!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} whipped\nup a whirlwind!")),
    [STRINGID_PKMNTOOKSUNLIGHT]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} absorbed light!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} took\nin sunlight!")),
    [STRINGID_PKMNLOWEREDHEAD]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} tucked in its head!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} lowered\nits head!")),
    [STRINGID_PKMNFLEWHIGH]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} flew up high!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} flew\nup high!")),
    [STRINGID_PKMNDUGHOLE]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} burrowed its way under the ground!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} dug a hole!")),
    [STRINGID_PKMNSQUEEZEDBYBIND]                   = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was squeezed by {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was squeezed by\n{B_SCR_NAME_WITH_PREFIX}'s BIND!")),
    [STRINGID_PKMNTRAPPEDINVORTEX]                  = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} became trapped in the vortex!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was trapped\nin the vortex!")),
    [STRINGID_PKMNWRAPPEDBY]                        = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was wrapped by {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was WRAPPED by\n{B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNCLAMPED]                          = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} clamped down on {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} CLAMPED\n{B_EFF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNHURTBY]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hurt by {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby {B_BUFF1}!")),
    [STRINGID_PKMNFREEDFROM]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was freed from {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} was freed\nfrom {B_BUFF1}!")),
    [STRINGID_PKMNCRASHED]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} kept going and crashed!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} kept going\nand crashed!")),
    [STRINGID_PKMNSHROUDEDINMIST]                   = gText_PkmnShroudedInMist,
    [STRINGID_PKMNPROTECTEDBYMIST]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is protected by the mist!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is protected\nby MIST!")),
    [STRINGID_PKMNGETTINGPUMPED]                    = gText_PkmnGettingPumped,
    [STRINGID_PKMNHITWITHRECOIL]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was damaged by the recoil!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hit\nwith recoil!")),
    [STRINGID_PKMNPROTECTEDITSELF2]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} protected itself!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} protected\nitself!")),
    [STRINGID_PKMNBUFFETEDBYSANDSTORM]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is buffeted by the sandstorm!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is buffeted\nby the sandstorm!")),
    [STRINGID_PKMNPELTEDBYHAIL]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is buffeted by the hail!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is pelted\nby HAIL!")),
    [STRINGID_PKMNSEEDED]                           = COMPOUND_STRING("{B_DEF_NAME_WITH_PREFIX} was seeded!"),
    [STRINGID_PKMNAVOIDEDATTACK]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} avoided the attack!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} avoided\nthe attack!")),
    [STRINGID_BATTLERAVOIDEDATTACK]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} avoided the attack!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} avoided\nthe attack!")),
    [STRINGID_PKMNSAPPEDBYLEECHSEED]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s health is sapped by Leech Seed!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s health is\nsapped by LEECH SEED!")),
    [STRINGID_PKMNFASTASLEEP]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is fast asleep.",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is fast\nasleep.")),
    [STRINGID_PKMNWOKEUP]                           = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} woke up!"),
    [STRINGID_PKMNWOKEUPINUPROAR]                   = COMPOUND_STRING(VANILLA_TEXT("The uproar woke {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} woke up\nin the UPROAR!")),
    [STRINGID_PKMNCAUSEDUPROAR]                     = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} caused an uproar!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} caused\nan UPROAR!")),
    [STRINGID_PKMNMAKINGUPROAR]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is making an uproar!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is making\nan UPROAR!")),
    [STRINGID_PKMNCALMEDDOWN]                       = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} calmed down."),
    [STRINGID_PKMNSTOCKPILED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} stockpiled {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} STOCKPILED\n{B_BUFF1}!")),
    [STRINGID_PKMNCANTSLEEPINUPROAR2]               = COMPOUND_STRING(VANILLA_TEXT("But {B_DEF_NAME_WITH_PREFIX2} can't sleep in an uproar!",
                                                                                   "But {B_DEF_NAME_WITH_PREFIX} can't\nsleep in an UPROAR!")),
    [STRINGID_UPROARKEPTPKMNAWAKE]                  = COMPOUND_STRING(VANILLA_TEXT("But the uproar kept {B_DEF_NAME_WITH_PREFIX2} awake!",
                                                                                   "But the UPROAR kept\n{B_DEF_NAME_WITH_PREFIX} awake!")),
    [STRINGID_PKMNSTAYEDAWAKEUSING]                 = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} stayed awake!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} stayed awake\nusing its {B_DEF_ABILITY}!")),
    [STRINGID_PKMNSTORINGENERGY]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is storing energy!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is storing\nenergy!")),
    [STRINGID_PKMNUNLEASHEDENERGY]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} unleashed its energy!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} unleashed\nenergy!")),
    [STRINGID_PKMNFATIGUECONFUSION]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} became confused due to fatigue!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} became\nconfused due to fatigue!")),
    [STRINGID_PLAYERPICKEDUPMONEY]                  = COMPOUND_STRING(VANILLA_TEXT("You picked up ¥{B_BUFF1}!\p",
                                                                                   "{B_PLAYER_NAME} picked up\n¥{B_BUFF1}!\p")),
    [STRINGID_PKMNUNAFFECTED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is unaffected!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is\nunaffected!")),
    [STRINGID_PKMNTRANSFORMEDINTO]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} transformed into {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} transformed\ninto {B_BUFF1}!")),
    [STRINGID_PKMNMADESUBSTITUTE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} put in a substitute!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} made\na SUBSTITUTE!")),
    [STRINGID_PKMNHASSUBSTITUTE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} already has a substitute!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} already\nhas a SUBSTITUTE!")),
    [STRINGID_SUBSTITUTEDAMAGED]                    = COMPOUND_STRING(VANILLA_TEXT("The substitute took damage for {B_DEF_NAME_WITH_PREFIX2}!\p",
                                                                                   "The SUBSTITUTE took damage\nfor {B_DEF_NAME_WITH_PREFIX}!\p")),
    [STRINGID_PKMNSUBSTITUTEFADED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s substitute faded!\p",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s\nSUBSTITUTE faded!\p")),
    [STRINGID_PKMNMUSTRECHARGE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} must recharge!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} must\nrecharge!")),
    [STRINGID_PKMNRAGEBUILDING]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s rage is building!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s RAGE\nis building!")),
    [STRINGID_PKMNMOVEWASDISABLED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1} was disabled!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1}\nwas disabled!")),
    [STRINGID_PKMNMOVEISDISABLED]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE} is disabled!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE}\nis disabled!\p")),
    [STRINGID_PKMNMOVEDISABLEDNOMORE]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s move is no longer disabled!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is disabled\nno more!")),
    [STRINGID_PKMNGOTENCORE]                        = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} must do an encore!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} got\nan ENCORE!")),
    [STRINGID_PKMNGOTENCOREDMOVE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} can only use {B_CURRENT_MOVE}!\p",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} can only\nuse {B_CURRENT_MOVE}!\p")),
    [STRINGID_PKMNENCOREENDED]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} ended its encore!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s ENCORE\nended!")),
    [STRINGID_PKMNTOOKAIM]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} took aim at {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} took aim\nat {B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNSKETCHEDMOVE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} sketched {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} SKETCHED\n{B_BUFF1}!")),
    [STRINGID_PKMNTRYINGTOTAKEFOE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hoping to take its attacker down with it!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is trying\nto take its foe with it!")),
    [STRINGID_PKMNTOOKFOE]                          = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} took its attacker down with it!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} took\n{B_ATK_NAME_WITH_PREFIX} with it!")),
    [STRINGID_PKMNREDUCEDPP]                        = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} lost {B_BUFF2} PP from {B_BUFF1}!",
                                                                                   "Reduced {B_DEF_NAME_WITH_PREFIX}'s\n{B_BUFF1} by {B_BUFF2}!")),
    [STRINGID_PKMNSTOLEITEM]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} stole {B_BUFF2}'s {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} stole\n{B_BUFF2}'s {B_LAST_ITEM}!")),
    [STRINGID_TARGETCANTESCAPENOW]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} can no longer escape!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} can't\nescape now!")),
    [STRINGID_PKMNFELLINTONIGHTMARE]                = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} began having a nightmare!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} fell into\na NIGHTMARE!")),
    [STRINGID_PKMNLOCKEDINNIGHTMARE]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is locked in a nightmare!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is locked\nin a NIGHTMARE!")),
    [STRINGID_PKMNLAIDCURSE]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cut its own HP and put a curse on {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cut its own HP and\nlaid a CURSE on {B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNAFFLICTEDBYCURSE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is afflicted by the curse!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is afflicted\nby the CURSE!")),
    [STRINGID_SPIKESSCATTERED]                      = COMPOUND_STRING(VANILLA_TEXT("Spikes were scattered on the ground all around {B_DEF_TEAM2} side!",
                                                                                   "SPIKES were scattered all around\nthe opponent's side!")),
    [STRINGID_PKMNHURTBYSPIKES]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was hurt by the spikes!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is hurt\nby SPIKES!")),
    [STRINGID_PKMNIDENTIFIED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was identified!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} identified\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNPERISHCOUNTFELL]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s perish count fell to {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s PERISH count\nfell to {B_BUFF1}!")),
    [STRINGID_PKMNBRACEDITSELF]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} braced itself!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} braced\nitself!")),
    [STRINGID_PKMNENDUREDHIT]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} endured the hit!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} ENDURED\nthe hit!")),
    [STRINGID_MAGNITUDESTRENGTH]                    = COMPOUND_STRING(VANILLA_TEXT("Magnitude {B_BUFF1}!",
                                                                                   "MAGNITUDE {B_BUFF1}!")),
    [STRINGID_PKMNCUTHPMAXEDATTACK]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cut its own HP and maximized its Attack!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cut its own HP\nand maximized ATTACK!")),
    [STRINGID_PKMNCOPIEDSTATCHANGES]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} copied {B_EFF_NAME_WITH_PREFIX2}'s stat changes!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} copied\n{B_EFF_NAME_WITH_PREFIX}'s stat changes!")),
    [STRINGID_PKMNGOTFREE]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was freed from {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} got free of\n{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1}!")),
    [STRINGID_PKMNSHEDLEECHSEED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was freed from Leech Seed!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shed\nLEECH SEED!")),
    [STRINGID_PKMNBLEWAWAYSPIKES]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} blew away Spikes!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} blew away\nSPIKES!")), // Not in gen 5+
    [STRINGID_PKMNFLEDFROMBATTLE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} fled from battle!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} fled from\nbattle!")),
    [STRINGID_PKMNFORESAWATTACK]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} foresaw an attack!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} foresaw\nan attack!")),
    [STRINGID_PKMNTOOKATTACK]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} took the {B_BUFF1} attack!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} took the\n{B_BUFF1} attack!")),
    [STRINGID_PKMNATTACK]                           = COMPOUND_STRING("{B_BUFF1}'s attack!"), // Not in gen 5+
    [STRINGID_PKMNCENTERATTENTION]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} became the center of attention!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} became the\ncenter of attention!")),
    [STRINGID_PKMNCHARGINGPOWER]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} began charging power!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} began\ncharging power!")),
    [STRINGID_NATUREPOWERTURNEDINTO]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s Nature Power turned into {B_CURRENT_MOVE}!",
                                                                                   "NATURE POWER turned into\n{B_CURRENT_MOVE}!")),
    [STRINGID_PKMNSTATUSNORMAL]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s status returned to normal!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s status\nreturned to normal!")),
    [STRINGID_PKMNHASNOMOVESLEFT]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} has no moves left that it can use!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} has no\nmoves left!\p")),
    [STRINGID_PKMNSUBJECTEDTOTORMENT]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was subjected to torment!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was subjected\nto TORMENT!")),
    [STRINGID_PKMNCANTUSEMOVETORMENT]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can't use the same move twice in a row due to the torment!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't use the same\nmove in a row due to the TORMENT!\p")),
    [STRINGID_PKMNTIGHTENINGFOCUS]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is tightening its focus!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is tightening\nits focus!")),
    [STRINGID_PKMNFELLFORTAUNT]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} fell for the taunt!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} fell for\nthe TAUNT!")),
    [STRINGID_PKMNCANTUSEMOVETAUNT]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can't use {B_CURRENT_MOVE} after the taunt!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't use\n{B_CURRENT_MOVE} after the TAUNT!\p")),
    [STRINGID_PKMNREADYTOHELP]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is ready to help {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is ready to\nhelp {B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNSWITCHEDITEMS]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} switched items with its target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} switched\nitems with its opponent!")),
    [STRINGID_PKMNCOPIEDFOE]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} copied {B_DEF_NAME_WITH_PREFIX2}'s Ability!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} copied\n{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}!")),
    [STRINGID_PKMNWISHCAMETRUE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1}'s wish came true!",
                                                                                   "{B_BUFF1}'s WISH\ncame true!")),
    [STRINGID_PKMNPLANTEDROOTS]                     = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} planted its roots!"),
    [STRINGID_PKMNABSORBEDNUTRIENTS]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} absorbed nutrients with its roots!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} absorbed\nnutrients with its roots!")),
    [STRINGID_PKMNANCHOREDITSELF]                   = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} is anchored in place with its roots!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} anchored\nitself with its roots!")),
    [STRINGID_PKMNWASMADEDROWSY]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} grew drowsy!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} made\n{B_DEF_NAME_WITH_PREFIX} drowsy!")),
    [STRINGID_PKMNKNOCKEDOFF]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} knocked off {B_EFF_NAME_WITH_PREFIX2}'s {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} knocked off\n{B_EFF_NAME_WITH_PREFIX}'s {B_LAST_ITEM}!")),
    [STRINGID_PKMNSWAPPEDABILITIES]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} swapped Abilities with its target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} swapped abilities\nwith its opponent!")),
    [STRINGID_PKMNSEALEDOPPONENTMOVE]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} sealed any moves its target shares with it!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} sealed the\nopponent's move(s)!")),
    [STRINGID_PKMNCANTUSEMOVESEALED]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can't use its sealed {B_CURRENT_MOVE}!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't use the\nsealed {B_CURRENT_MOVE}!\p")),
    [STRINGID_PKMNWANTSGRUDGE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} wants its target to bear a grudge!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} wants the\nopponent to bear a GRUDGE!")),
    [STRINGID_PKMNLOSTPPGRUDGE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} lost all of {B_BUFF1}'s PP due to the grudge!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_BUFF1} lost\nall its PP due to the GRUDGE!")),
    [STRINGID_PKMNSHROUDEDITSELF]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} shrouded itself with Magic Coat!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shrouded\nitself in {B_CURRENT_MOVE}!")),
    [STRINGID_PKMNMOVEBOUNCED]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} bounced the {B_CURRENT_MOVE} back!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE}\nwas bounced back by MAGIC COAT!")),
    [STRINGID_PKMNWAITSFORTARGET]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is waiting for a target to make a move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} waits for a target\nto make a move!")),
    [STRINGID_PKMNSNATCHEDMOVE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} snatched {B_SCR_NAME_WITH_PREFIX2}'s move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} SNATCHED\n{B_SCR_NAME_WITH_PREFIX}'s move!")),
    [STRINGID_PKMNMADEITRAIN]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} made it rain!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nmade it rain!")), //not in gen 5+, ability popup
    [STRINGID_PKMNPROTECTEDBY]                      = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was protected by {B_DEF_ABILITY}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was protected\nby {B_DEF_ABILITY}!")), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSUSAGE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY} prevents {B_ATK_NAME_WITH_PREFIX2} from using {B_CURRENT_MOVE}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\nprevents {B_ATK_NAME_WITH_PREFIX}\lfrom using {B_CURRENT_MOVE}!")), //not in gen 5+, ability popup
    [STRINGID_PKMNRESTOREDHPUSING]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} had its HP restored.",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} restored HP\nusing its {B_SCR_ABILITY}!")),
    [STRINGID_PKMNCHANGEDTYPEWITH]                  = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX}'s type changed to {B_BUFF1}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}'s {B_EFF_ABILITY}\nmade it the {B_BUFF1} type!")),
    [STRINGID_PKMNPREVENTSROMANCEWITH]              = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY} prevents romance!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\nprevents romance!")), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSCONFUSIONWITH]            = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} cannot be confused!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nprevents confusion!")),
    [STRINGID_PKMNRAISEDFIREPOWERWITH]              = COMPOUND_STRING(VANILLA_TEXT("The power of {B_SCR_NAME_WITH_PREFIX}'s Fire-type moves rose!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nraised its FIRE power!")),
    [STRINGID_PKMNANCHORSITSELFWITH]                = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} is anchored in place with its suction cups!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} anchors\nitself with {B_EFF_ABILITY}!")),
    [STRINGID_PKMNPREVENTSSTATLOSSWITH]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s stats were not lowered!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nprevents stat loss!")),
    [STRINGID_PKMNHURTSWITH]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by {B_DEF_NAME_WITH_PREFIX2}'s {B_BUFF1}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1}\nhurt {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNTRACED]                           = COMPOUND_STRING(VANILLA_TEXT("It traced {B_BUFF1}'s {B_BUFF2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} TRACED\n{B_BUFF1}'s {B_BUFF2}!")),
    [STRINGID_STATSHARPLY]                          = gText_StatSharply,
    [STRINGID_STATHARSHLY]                          = COMPOUND_STRING("harshly "),
    [STRINGID_STATROSE]                             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} rose{B_BUFF2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\n{B_BUFF2}rose!")),
    [STRINGID_STATFELL]                             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} {B_BUFF2}fell!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\n{B_BUFF2}fell!")),
    [STRINGID_CRITICALHIT]                          = COMPOUND_STRING("A critical hit!"),
    [STRINGID_ONEHITKO]                             = COMPOUND_STRING("It's a one-hit KO!"),
    [STRINGID_123POOF]                              = COMPOUND_STRING(VANILLA_TEXT("One…{PAUSE 10}two…{PAUSE 10}and…{PAUSE 10}{PAUSE 20}{PLAY_SE SE_BALL_BOUNCE_1}ta-da!\p",
                                                                                   "{PAUSE 32}1, {PAUSE 15}2, and{PAUSE 15}… {PAUSE 15}… {PAUSE 15}… {PAUSE 15}{PLAY_SE SE_BALL_BOUNCE_1}Poof!\p")),
    [STRINGID_ANDELLIPSIS]                          = COMPOUND_STRING("And…\p"),
    [STRINGID_NOTVERYEFFECTIVE]                     = COMPOUND_STRING("It's not very effective…"),
    [STRINGID_SUPEREFFECTIVE]                       = COMPOUND_STRING("It's super effective!"),
    [STRINGID_GOTAWAYSAFELY]                        = sText_GotAwaySafely,
    [STRINGID_WILDPKMNFLED]                         = COMPOUND_STRING(VANILLA_TEXT("{PLAY_SE SE_FLEE}The wild {B_BUFF1} fled!",
                                                                                   "{PLAY_SE SE_FLEE}Wild {B_BUFF1} fled!")),
    [STRINGID_NORUNNINGFROMTRAINERS]                = COMPOUND_STRING(VANILLA_TEXT("No! There's no running from a TRAINER battle!\p",
                                                                                   "No! There's no running\nfrom a TRAINER battle!\p")),
    [STRINGID_CANTESCAPE]                           = COMPOUND_STRING(VANILLA_TEXT("You can't escape!\p",
                                                                                   "Can't escape!\p")),
    [STRINGID_DONTLEAVEBIRCH]                       = COMPOUND_STRING("PROF. BIRCH: Don't leave me like this!\p"), //no decapitalize until it is everywhere
    [STRINGID_BUTNOTHINGHAPPENED]                   = COMPOUND_STRING("But nothing happened!"),
    [STRINGID_BUTITFAILED]                          = COMPOUND_STRING("But it failed!"),
    [STRINGID_ITHURTCONFUSION]                      = COMPOUND_STRING(VANILLA_TEXT("It hurt itself in its confusion!",
                                                                                   "It hurt itself in its\nconfusion!")),
    [STRINGID_STARTEDTORAIN]                        = COMPOUND_STRING("It started to rain!"),
    [STRINGID_DOWNPOURSTARTED]                      = COMPOUND_STRING("A downpour started!"), // corresponds to DownpourText in pokegold and pokecrystal and is used by Rain Dance in GSC
    [STRINGID_RAINCONTINUES]                        = COMPOUND_STRING("Rain continues to fall."), //not in gen 5+
    [STRINGID_DOWNPOURCONTINUES]                    = COMPOUND_STRING("The downpour continues."), // unused
    [STRINGID_RAINSTOPPED]                          = COMPOUND_STRING("The rain stopped."),
    [STRINGID_SANDSTORMBREWED]                      = COMPOUND_STRING(VANILLA_TEXT("A sandstorm kicked up!",
                                                                                   "A sandstorm brewed!")),
    [STRINGID_SANDSTORMRAGES]                       = COMPOUND_STRING(VANILLA_TEXT("The sandstorm is raging.",
                                                                                   "The sandstorm rages.")),
    [STRINGID_SANDSTORMSUBSIDED]                    = COMPOUND_STRING("The sandstorm subsided."),
    [STRINGID_SUNLIGHTGOTBRIGHT]                    = COMPOUND_STRING(VANILLA_TEXT("The sunlight turned harsh!",
                                                                                   "The sunlight got bright!")),
    [STRINGID_SUNLIGHTSTRONG]                       = COMPOUND_STRING("The sunlight is strong."), //not in gen 5+
    [STRINGID_SUNLIGHTFADED]                        = COMPOUND_STRING("The sunlight faded."),
    [STRINGID_STARTEDHAIL]                          = COMPOUND_STRING("It started to hail!"),
    [STRINGID_HAILCONTINUES]                        = COMPOUND_STRING(VANILLA_TEXT("The hail is crashing down.",
                                                                                   "Hail continues to fall.")),
    [STRINGID_HAILSTOPPED]                          = COMPOUND_STRING("The hail stopped."),
    [STRINGID_STATCHANGESGONE]                      = COMPOUND_STRING(VANILLA_TEXT("All stat changes were eliminated!",
                                                                                   "All stat changes were\neliminated!")),
    [STRINGID_COINSSCATTERED]                       = COMPOUND_STRING(VANILLA_TEXT("Coins were scattered everywhere!",
                                                                                   "Coins scattered everywhere!")),
    [STRINGID_TOOWEAKFORSUBSTITUTE]                 = COMPOUND_STRING(VANILLA_TEXT("But it does not have enough HP left to make a substitute!",
                                                                                   "It was too weak to make\na SUBSTITUTE!")),
    [STRINGID_SHAREDPAIN]                           = COMPOUND_STRING(VANILLA_TEXT("The battlers shared their pain!",
                                                                                   "The battlers shared\ntheir pain!")),
    [STRINGID_BELLCHIMED]                           = COMPOUND_STRING("A bell chimed!"),
    [STRINGID_FAINTINTHREE]                         = COMPOUND_STRING(VANILLA_TEXT("All POKéMON that heard the song will faint in three turns!",
                                                                                   "All affected POKéMON will\nfaint in three turns!")),
    [STRINGID_NOPPLEFT]                             = COMPOUND_STRING(VANILLA_TEXT("There's no PP left for this move!\p",
                                                                                   "There's no PP left for\nthis move!\p")), //not in gen 5+
    [STRINGID_BUTNOPPLEFT]                          = COMPOUND_STRING(VANILLA_TEXT("But there was no PP left for the move!",
                                                                                   "But there was no PP left\nfor the move!")),
    [STRINGID_PLAYERUSEDITEM]                       = COMPOUND_STRING(VANILLA_TEXT("You used {B_LAST_ITEM}!",
                                                                                   "{B_PLAYER_NAME} used\n{B_LAST_ITEM}!")),
    [STRINGID_TRAINERBLOCKEDBALL]                   = COMPOUND_STRING(VANILLA_TEXT("The TRAINER blocked your POKé BALL!",
                                                                                   "The TRAINER blocked the BALL!")),
    [STRINGID_DONTBEATHIEF]                         = COMPOUND_STRING("Don't be a thief!"),
    [STRINGID_ITDODGEDBALL]                         = COMPOUND_STRING(VANILLA_TEXT("It dodged your thrown POKé BALL! This POKéMON can't be caught!",
                                                                                   "It dodged the thrown BALL!\nThis POKéMON can't be caught!")),
    [STRINGID_PKMNBROKEFREE]                        = COMPOUND_STRING(VANILLA_TEXT("Oh no! The POKéMON broke free!",
                                                                                   "Oh, no!\nThe POKéMON broke free!")),
    [STRINGID_ITAPPEAREDCAUGHT]                     = COMPOUND_STRING(VANILLA_TEXT("Aww! It appeared to be caught!",
                                                                                   "Aww!\nIt appeared to be caught!")),
    [STRINGID_AARGHALMOSTHADIT]                     = COMPOUND_STRING(VANILLA_TEXT("Aargh! Almost had it!",
                                                                                   "Aargh!\nAlmost had it!")),
    [STRINGID_SHOOTSOCLOSE]                         = COMPOUND_STRING(VANILLA_TEXT("Gah! It was so close, too!",
                                                                                   "Shoot!\nIt was so close, too!")),
    [STRINGID_GOTCHAPKMNCAUGHTPLAYER]               = COMPOUND_STRING(VANILLA_TEXT("Gotcha! {B_DEF_NAME} was caught!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}\p",
                                                                                   "Gotcha!\n{B_DEF_NAME} was caught!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}\p")),
    [STRINGID_GOTCHAPKMNCAUGHTWALLY]                = COMPOUND_STRING(VANILLA_TEXT("Gotcha! {B_DEF_NAME} was caught!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}{PAUSE 127}",
                                                                                   "Gotcha!\n{B_DEF_NAME} was caught!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}{PAUSE 127}")),
    [STRINGID_GIVENICKNAMECAPTURED]                 = COMPOUND_STRING(VANILLA_TEXT("Would you like to give {B_DEF_NAME} a nickname?",
                                                                                   "Give a nickname to the\ncaptured {B_DEF_NAME}?")),
    [STRINGID_PKMNDATAADDEDTODEX]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME}'s data has been added to the Pokédex!\p",
                                                                                   "{B_DEF_NAME}'s data was\nadded to the POKéDEX.\p")),
    [STRINGID_ITISRAINING]                          = COMPOUND_STRING(VANILLA_TEXT("It's raining!",
                                                                                   "It is raining.")),
    [STRINGID_SANDSTORMISRAGING]                    = COMPOUND_STRING(VANILLA_TEXT("The sandstorm is raging!",
                                                                                   "A sandstorm is raging.")),
    [STRINGID_CANTESCAPE2]                          = COMPOUND_STRING(VANILLA_TEXT("You couldn't get away!\p",
                                                                                   "Can't escape!\p")),
    [STRINGID_PKMNIGNORESASLEEP]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} ignored orders and kept sleeping!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} ignored\norders while asleep!")),
    [STRINGID_PKMNIGNOREDORDERS]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} ignored orders!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} ignored\norders!")),
    [STRINGID_PKMNBEGANTONAP]                       = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} began to nap!"),
    [STRINGID_PKMNLOAFING]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is loafing around!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is\nloafing around!")),
    [STRINGID_PKMNWONTOBEY]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} won't obey!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} won't\nobey!")),
    [STRINGID_PKMNTURNEDAWAY]                       = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} turned away!"),
    [STRINGID_PKMNPRETENDNOTNOTICE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} pretended not to notice!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} pretended\nnot to notice!")),
    [STRINGID_ENEMYABOUTTOSWITCHPKMN]               = COMPOUND_STRING(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} is about to send out {B_BUFF2}. Will you switch your POKéMON?",
                                                                                   "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} is\nabout to use {B_BUFF2}.\pWill {B_PLAYER_NAME} change\nPOKéMON?")),
    [STRINGID_CREPTCLOSER]                          = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_NAME} crept closer to {B_OPPONENT_MON1_NAME}!",
                                                                                   "{B_PLAYER_NAME} crept closer to\n{B_OPPONENT_MON1_NAME}!")), //safari
    [STRINGID_CANTGETCLOSER]                        = COMPOUND_STRING("{B_PLAYER_NAME} can't get any closer!"), //safari
    [STRINGID_PKMNWATCHINGCAREFULLY]                = COMPOUND_STRING(VANILLA_TEXT("{B_OPPONENT_MON1_NAME} is watching carefully!",
                                                                                   "{B_OPPONENT_MON1_NAME} is watching\ncarefully!")), //safari
    [STRINGID_PKMNCURIOUSABOUTX]                    = COMPOUND_STRING(VANILLA_TEXT("{B_OPPONENT_MON1_NAME} is curious about the {B_BUFF1}!",
                                                                                   "{B_OPPONENT_MON1_NAME} is curious about\nthe {B_BUFF1}!")), //safari
    [STRINGID_PKMNENTHRALLEDBYX]                    = COMPOUND_STRING(VANILLA_TEXT("{B_OPPONENT_MON1_NAME} is enthralled by the {B_BUFF1}!",
                                                                                   "{B_OPPONENT_MON1_NAME} is enthralled by\nthe {B_BUFF1}!")), //safari
    [STRINGID_PKMNIGNOREDX]                         = COMPOUND_STRING(VANILLA_TEXT("{B_OPPONENT_MON1_NAME} completely ignored the {B_BUFF1}!",
                                                                                   "{B_OPPONENT_MON1_NAME} completely ignored\nthe {B_BUFF1}!")), //safari
    [STRINGID_THREWPOKEBLOCKATPKMN]                 = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_NAME} threw a {POKEBLOCK} at the {B_OPPONENT_MON1_NAME}!",
                                                                                   "{B_PLAYER_NAME} threw a {POKEBLOCK}\nat the {B_OPPONENT_MON1_NAME}!")), //safari
    [STRINGID_OUTOFSAFARIBALLS]                     = COMPOUND_STRING(VANILLA_TEXT("{PLAY_SE SE_DING_DONG}ANNOUNCER: You're out of Safari Balls! Game over!\p",
                                                                                   "{PLAY_SE SE_DING_DONG}ANNOUNCER: You're out of\nSAFARI BALLS! Game over!\p")), //safari
    [STRINGID_PKMNSITEMCUREDPARALYSIS]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} cured its paralysis!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\ncured paralysis!")),
    [STRINGID_PKMNSITEMCUREDPOISON]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} cured its poison!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\ncured poison!")),
    [STRINGID_PKMNSITEMHEALEDBURN]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} cured its burn!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nhealed its burn!")),
    [STRINGID_PKMNSITEMDEFROSTEDIT]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} defrosted it!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\ndefrosted it!")),
    [STRINGID_PKMNSITEMWOKEIT]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} woke it up!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nwoke it from its sleep!")),
    [STRINGID_PKMNSITEMSNAPPEDOUT]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} snapped it out of its confusion!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nsnapped it out of confusion!")),
    [STRINGID_PKMNSITEMCUREDPROBLEM]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} cured its {B_BUFF1} problem!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\ncured its {B_BUFF1} problem!")), // Not in Gen 5+
    [STRINGID_PKMNSITEMRESTOREDHEALTH]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} restored its health using its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nrestored health!")),
    [STRINGID_PKMNSITEMRESTOREDPP]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} restored PP to its move {B_BUFF1} using its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nrestored {B_BUFF1}'s PP!")),
    [STRINGID_PKMNSITEMRESTOREDSTATUS]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} returned its stats to normal using its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nrestored its status!")),
    [STRINGID_PKMNSITEMRESTOREDHPALITTLE]           = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} restored a little HP using its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nrestored its HP a little!")),
    [STRINGID_ITEMALLOWSONLYYMOVE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_LAST_ITEM} only allows the use of {B_CURRENT_MOVE}!\p",
                                                                                   "{B_LAST_ITEM} allows the\nuse of only {B_CURRENT_MOVE}!\p")),
    [STRINGID_PKMNHUNGONWITHX]                      = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} hung on using its {B_LAST_ITEM}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} hung on\nusing its {B_LAST_ITEM}!")),
    [STRINGID_EMPTYSTRING3]                         = gText_EmptyString3,
    [STRINGID_PKMNSXRESTOREDHPALITTLE2]             = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} had its HP restored.",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY}\nrestored its HP a little!")),
    [STRINGID_PKMNSXWHIPPEDUPSANDSTORM]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} whipped up a sandstorm!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nwhipped up a sandstorm!")), //not in gen 5+, ability popup
    [STRINGID_PKMNSXPREVENTSYLOSS]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} was not lowered!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nprevents {B_BUFF1} loss!")),
    [STRINGID_PKMNSXINFATUATEDY]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} fell in love!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\ninfatuated {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNSXMADEYINEFFECTIVE]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s item cannot be removed!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nmade {B_CURRENT_MOVE} ineffective!")),
    [STRINGID_ITSUCKEDLIQUIDOOZE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} sucked up the liquid ooze!",
                                                                                   "It sucked up the\nLIQUID OOZE!")),
    [STRINGID_PKMNTRANSFORMED]                      = COMPOUND_STRING("{B_SCR_NAME_WITH_PREFIX} transformed!"),
    [STRINGID_ELECTRICITYWEAKENED]                  = COMPOUND_STRING(VANILLA_TEXT("Electricity's power was weakened!",
                                                                                   "Electricity's power was\nweakened!")),
    [STRINGID_FIREWEAKENED]                         = COMPOUND_STRING(VANILLA_TEXT("Fire's power was weakened!",
                                                                                   "Fire's power was\nweakened!")),
    [STRINGID_PKMNHIDUNDERWATER]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} hid underwater!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} hid\nunderwater!")),
    [STRINGID_PKMNSPRANGUP]                         = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} sprang up!"),
    [STRINGID_HMMOVESCANTBEFORGOTTEN]               = COMPOUND_STRING(VANILLA_TEXT("HM moves can't be forgotten now.\p",
                                                                                   "HM moves can't be\nforgotten now.\p")),
    [STRINGID_XFOUNDONEY]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} found one {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} found\none {B_LAST_ITEM}!")),
    [STRINGID_PLAYERDEFEATEDTRAINER1]               = sText_PlayerDefeatedLinkTrainerTrainer1,
    [STRINGID_SOOTHINGAROMA]                        = COMPOUND_STRING(VANILLA_TEXT("A soothing aroma wafted through the area!",
                                                                                   "A soothing aroma wafted\nthrough the area!")),
    [STRINGID_ITEMSCANTBEUSEDNOW]                   = COMPOUND_STRING("Items can't be used now.{PAUSE 64}"), // Not present in Gen 5+
    [STRINGID_USINGITEMSTATOFPKMNROSE]              = COMPOUND_STRING(VANILLA_TEXT("The {B_LAST_ITEM}{B_BUFF2} boosted {B_SCR_NAME_WITH_PREFIX2}'s {B_BUFF1}!",
                                                                                   "Using {B_LAST_ITEM}, the {B_BUFF1}\nof {B_SCR_NAME_WITH_PREFIX} {B_BUFF2}rose!")),
    [STRINGID_USINGITEMSTATOFPKMNFELL]              = COMPOUND_STRING(VANILLA_TEXT("The {B_LAST_ITEM}{B_BUFF2} lowered {B_SCR_NAME_WITH_PREFIX2}'s {B_BUFF1}!",
                                                                                   "Using {B_LAST_ITEM}, the {B_BUFF1}\nof {B_SCR_NAME_WITH_PREFIX} {B_BUFF2}fell!")), // This string does not exist in Gen 5+. Used to print more info that's otherwise obscured such as using Room Service
    [STRINGID_PKMNUSEDXTOGETPUMPED]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} used the {B_LAST_ITEM} to get pumped!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} used\n{B_LAST_ITEM} to get pumped!")),
    [STRINGID_PKMNSXMADEYUSELESS]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} made {B_CURRENT_MOVE} useless!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nmade {B_CURRENT_MOVE} useless!")), //not in gen 5+, ability popup
    [STRINGID_PKMNTRAPPEDBYSANDTOMB]                = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} became trapped by the quicksand!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was trapped\nby SAND TOMB!")),
    [STRINGID_EMPTYSTRING4]                         = COMPOUND_STRING(""),
    [STRINGID_ABOOSTED]                             = COMPOUND_STRING(" a boosted"),
    [STRINGID_PKMNSXINTENSIFIEDSUN]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} intensified the sun's rays!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nintensified the sun's rays!")), //not in gen 5+, ability popup
    [STRINGID_YOUTHROWABALLNOWRIGHT]                = COMPOUND_STRING(VANILLA_TEXT("You throw a Ball now, right? I… I'll do my best!",
                                                                                   "You throw a BALL now, right?\nI… I'll do my best!")),
    [STRINGID_PKMNSXTOOKATTACK]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} took the attack!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\ntook the attack!")),
    [STRINGID_PKMNCHOSEXASDESTINY]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} chose Doom Desire as its destiny!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} chose\n{B_CURRENT_MOVE} as its destiny!")),
    [STRINGID_PKMNLOSTFOCUS]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} lost its focus and couldn't move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} lost its\nfocus and couldn't move!")),
    [STRINGID_USENEXTPKMN]                          = COMPOUND_STRING("Use next POKéMON?"),
    [STRINGID_PKMNFLEDUSINGITS]                     = COMPOUND_STRING(VANILLA_TEXT("{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX} fled using its {B_LAST_ITEM}!\p",
                                                                                   "{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX} fled\nusing its {B_LAST_ITEM}!\p")),
    [STRINGID_PKMNFLEDUSING]                        = COMPOUND_STRING(VANILLA_TEXT("{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX} fled using {B_ATK_ABILITY}!\p",
                                                                                   "{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX} fled\nusing {B_ATK_ABILITY}!\p")), //not in gen 5+
    [STRINGID_PKMNWASDRAGGEDOUT]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was dragged out!\p",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was\ndragged out!\p")),
    [STRINGID_PKMNSITEMNORMALIZEDSTATUS]            = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} normalized its status!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nnormalized its status!")), // Not in Gen 5+
    [STRINGID_TRAINER1USEDITEM]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TRAINER_NAME_WITH_CLASS} used {B_LAST_ITEM}!",
                                                                                   "{B_ATK_TRAINER_NAME_WITH_CLASS}\nused {B_LAST_ITEM}!")),
    [STRINGID_BOXISFULL]                            = COMPOUND_STRING(VANILLA_TEXT("The Box is full! You can't catch any more!\p",
                                                                                   "The BOX is full!\nYou can't catch any more!\p")),
    [STRINGID_PKMNSXMADEITINEFFECTIVE]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} made it ineffective!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nmade it ineffective!")),
    [STRINGID_PKMNSXPREVENTSFLINCHING]              = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX}'s {B_EFF_ABILITY} prevents flinching!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}'s {B_EFF_ABILITY}\nprevents flinching!")), //not in gen 5+, ability popup
    [STRINGID_PKMNALREADYHASBURN]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is already burned!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} already\nhas a burn.")),
    [STRINGID_PKMNSXBLOCKSY]                        = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} blocks {B_CURRENT_MOVE}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nblocks {B_CURRENT_MOVE}!")), //not in gen 5+, ability popup
    [STRINGID_PKMNSXWOREOFF]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TEAM1} side's {B_BUFF1} wore off!",
                                                                                   "{B_ATK_PREFIX1}'s {B_BUFF1}\nwore off!")),
    [STRINGID_THEWALLSHATTERED]                     = COMPOUND_STRING("The wall shattered!"), //not in gen5+, uses "your teams light screen wore off!" etc instead
    [STRINGID_PKMNSXCUREDITSYPROBLEM]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} cured its {B_BUFF1} problem!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\ncured its {B_BUFF1} problem!")), //not in gen 5+, ability popup
    [STRINGID_ATTACKERCANTESCAPE]                   = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} can't escape!"),
    [STRINGID_PKMNOBTAINEDX]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} obtained {B_BUFF1}.",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} obtained\n{B_BUFF1}.")),
    [STRINGID_PKMNOBTAINEDX2]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} obtained {B_BUFF2}.",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} obtained\n{B_BUFF2}.")),
    [STRINGID_PKMNOBTAINEDXYOBTAINEDZ]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} obtained {B_BUFF1}.\p{B_DEF_NAME_WITH_PREFIX} obtained {B_BUFF2}.",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} obtained\n{B_BUFF1}.\p{B_DEF_NAME_WITH_PREFIX} obtained\n{B_BUFF2}.")),
    [STRINGID_BUTNOEFFECT]                          = COMPOUND_STRING("But it had no effect!"),
    [STRINGID_TWOENEMIESDEFEATED]                   = sText_TwoInGameTrainersDefeated,
    [STRINGID_TRAINER2LOSETEXT]                     = COMPOUND_STRING("{B_TRAINER2_LOSE_TEXT}"),
    [STRINGID_PKMNINCAPABLEOFPOWER]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} appears incapable of using its power!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} appears incapable\nof using its power!")),
    [STRINGID_GLINTAPPEARSINEYE]                    = COMPOUND_STRING(VANILLA_TEXT("A glint appears in {B_SCR_NAME_WITH_PREFIX2}'s eyes!",
                                                                                   "A glint appears in\n{B_SCR_NAME_WITH_PREFIX}'s eyes!")),
    [STRINGID_PKMNGETTINGINTOPOSITION]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is getting into position!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is getting into\nposition!")),
    [STRINGID_PKMNBEGANGROWLINGDEEPLY]              = COMPOUND_STRING("{B_SCR_NAME_WITH_PREFIX} began growling deeply!"),
    [STRINGID_PKMNEAGERFORMORE]                     = COMPOUND_STRING("{B_SCR_NAME_WITH_PREFIX} is eager for more!"),
    [STRINGID_DEFEATEDOPPONENTBYREFEREE]            = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_MON1_NAME} defeated the opponent {B_OPPONENT_MON1_NAME} in a REFEREE's decision!",
                                                                                   "{B_PLAYER_MON1_NAME} defeated the opponent\n{B_OPPONENT_MON1_NAME} in a REFEREE's decision!")),
    [STRINGID_LOSTTOOPPONENTBYREFEREE]              = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_MON1_NAME} lost to the opponent {B_OPPONENT_MON1_NAME} in a REFEREE's decision!",
                                                                                   "{B_PLAYER_MON1_NAME} lost to the opponent\n{B_OPPONENT_MON1_NAME} in a REFEREE's decision!")),
    [STRINGID_TIEDOPPONENTBYREFEREE]                = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_MON1_NAME} tied the opponent {B_OPPONENT_MON1_NAME} in a REFEREE's decision!",
                                                                                   "{B_PLAYER_MON1_NAME} tied the opponent\n{B_OPPONENT_MON1_NAME} in a REFEREE's decision!")),
    [STRINGID_QUESTIONFORFEITMATCH]                 = COMPOUND_STRING(VANILLA_TEXT("Would you like to forfeit the match and quit now?",
                                                                                   "Would you like to forfeit the match\nand quit now?")),
    [STRINGID_FORFEITEDMATCH]                       = COMPOUND_STRING(VANILLA_TEXT("The match was forfeited.",
                                                                                   "{B_PLAYER_NAME} forfeited the match!")),
    [STRINGID_PKMNTRANSFERREDSOMEONESPC]            = gText_PkmnTransferredSomeonesPC,
    [STRINGID_PKMNTRANSFERREDLANETTESPC]            = gText_PkmnTransferredLanettesPC,
    [STRINGID_PKMNBOXSOMEONESPCFULL]                = gText_PkmnTransferredSomeonesPCBoxFull,
    [STRINGID_PKMNBOXLANETTESPCFULL]                = gText_PkmnTransferredLanettesPCBoxFull,
    [STRINGID_TRAINER1WINTEXT]                      = COMPOUND_STRING("{B_TRAINER1_WIN_TEXT}"),
    [STRINGID_TRAINER2WINTEXT]                      = COMPOUND_STRING("{B_TRAINER2_WIN_TEXT}"),
    [STRINGID_ENDUREDSTURDY]                        = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} endured the hit using {B_DEF_ABILITY}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} ENDURED\nthe hit using {B_DEF_ABILITY}!")),
    [STRINGID_POWERHERB]                            = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became fully charged due to its {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became fully\ncharged due to its {B_LAST_ITEM}!")),
    [STRINGID_HURTBYITEM]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by the {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby the {B_LAST_ITEM}!")),
    [STRINGID_GRAVITYINTENSIFIED]                   = COMPOUND_STRING(VANILLA_TEXT("Gravity intensified!",
                                                                                   "GRAVITY intensified!")),
    [STRINGID_TARGETWOKEUP]                         = COMPOUND_STRING("{B_EFF_NAME_WITH_PREFIX} woke up!"),
    [STRINGID_TAILWINDBLEW]                         = COMPOUND_STRING(VANILLA_TEXT("A tailwind started blowing on {B_ATK_TEAM2} side!",
                                                                                   "A TAILWIND blew from behind\n{B_ATK_PREFIX1}'s party!")),
    [STRINGID_PKMNWENTBACK]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} went back to {B_ATK_TRAINER_NAME}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} went back\nto {B_ATK_TRAINER_NAME}!")),
    [STRINGID_PKMNCANTUSEITEMSANYMORE]              = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} can't use items anymore!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} can't use\nitems anymore!")),
    [STRINGID_PKMNFLUNG]                            = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} flung its {B_LAST_ITEM}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} flung its\n{B_LAST_ITEM}!")),
    [STRINGID_PKMNPREVENTEDFROMHEALING]             = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was prevented from healing!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was\nprevented from healing!")),
    [STRINGID_PKMNSWITCHEDATKANDDEF]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} switched its Attack and Defense!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} switched its\nATTACK and DEFENSE!")),
    [STRINGID_PKMNSABILITYSUPPRESSED]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s Ability was suppressed!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s ability\nwas suppressed!")),
    [STRINGID_SHIELDEDFROMCRITICALHITS]             = COMPOUND_STRING(VANILLA_TEXT("Lucky Chant shielded {B_ATK_TEAM2} side from critical hits!",
                                                                                   "LUCKY CHANT shielded {B_ATK_PREFIX1}'s\nparty from critical hits!")), // Currently not present in Champions
    [STRINGID_PKMNACQUIREDABILITY]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} acquired {B_DEF_ABILITY}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} acquired\n{B_DEF_ABILITY}!")),
    [STRINGID_POISONSPIKESSCATTERED]                = COMPOUND_STRING(VANILLA_TEXT("Toxic spikes were scattered on the ground all around {B_DEF_TEAM2} side!",
                                                                                   "TOXIC SPIKES were scattered all\naround the opponent's side!")),
    [STRINGID_PKMNSWITCHEDSTATCHANGES]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} switched stat changes with its target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} switched stat\nchanges with its opponent!")),
    [STRINGID_PKMNSURROUNDEDWITHVEILOFWATER]        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} surrounded itself with a veil of water!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} surrounded\nitself with a veil of water!")),
    [STRINGID_PKMNLEVITATEDONELECTROMAGNETISM]      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} levitated with electromagnetism!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} levitated on\nelectromagnetism!")),
    [STRINGID_PKMNTWISTEDDIMENSIONS]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} twisted the dimensions!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} twisted\nthe dimensions!")),
    [STRINGID_POINTEDSTONESFLOAT]                   = COMPOUND_STRING(VANILLA_TEXT("Pointed stones float in the air on {B_DEF_TEAM2} side!",
                                                                                   "Pointed stones float in the air\naround the opponent's side!")),
    [STRINGID_TRAPPEDBYSWIRLINGMAGMA]               = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} became trapped by swirling magma!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was trapped\nby swirling magma!")),
    [STRINGID_VANISHEDINSTANTLY]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} vanished instantly!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} vanished\ninstantly!")),
    [STRINGID_PROTECTEDTEAM]                        = COMPOUND_STRING(VANILLA_TEXT("{B_CURRENT_MOVE} now protects {B_ATK_TEAM2} side!",
                                                                                   "{B_CURRENT_MOVE} protected\n{B_ATK_PREFIX1}'s party!")),
    [STRINGID_SHAREDITSGUARD]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} shared its guard with the target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shared its\nguard with the target!")),
    [STRINGID_SHAREDITSPOWER]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} shared its power with the target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shared its\npower with the target!")),
    [STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON]         = COMPOUND_STRING(VANILLA_TEXT("It created a bizarre area in which Defense and Sp. Def stats are swapped!",
                                                                                   "It created a bizarre area in which\nDEFENSE and SP. DEF are swapped!")),
    [STRINGID_BECAMENIMBLE]                         = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} became nimble!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} became\nnimble!")),
    [STRINGID_HURLEDINTOTHEAIR]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was hurled into the air!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was hurled\ninto the air!")),
    [STRINGID_HELDITEMSLOSEEFFECTS]                 = COMPOUND_STRING(VANILLA_TEXT("It created a bizarre area in which POKéMON's held items lose their effects!",
                                                                                   "It created a bizarre area in which\nheld items lose their effects!")),
    [STRINGID_FELLSTRAIGHTDOWN]                     = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} fell straight down!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} fell\nstraight down!")),
    [STRINGID_TARGETCHANGEDTYPE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} transformed into the {B_BUFF1} type!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} transformed\ninto the {B_BUFF1} type!")),
    [STRINGID_KINDOFFER]                            = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} took the kind offer!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} took\nthe kind offer!")),
    [STRINGID_RESETSTARGETSSTATLEVELS]              = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX}'s stat changes were removed!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}'s stat changes\nwere removed!")),
    [STRINGID_ALLYSWITCHPOSITION]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} and {B_SCR_NAME_WITH_PREFIX2} switched places!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} and\n{B_SCR_NAME_WITH_PREFIX} switched places!")),
    [STRINGID_REFLECTTARGETSTYPE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became the same type as {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became the\nsame type as {B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_EMBARGOENDS]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can use items again!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can use\nitems again!")),
    [STRINGID_ELECTROMAGNETISM]                     = COMPOUND_STRING("electromagnetism"),
    [STRINGID_BUFFERENDS]                           = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} wore off!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nwore off!")),
    [STRINGID_TELEKINESISENDS]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was freed from the telekinesis!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} was freed\nfrom TELEKINESIS!")),
    [STRINGID_TAILWINDENDS]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TEAM1} side's tailwind petered out!",
                                                                                   "{B_ATK_PREFIX1}'s TAILWIND\npetered out!")),
    [STRINGID_LUCKYCHANTENDS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TEAM1} side's Lucky Chant wore off!",
                                                                                   "{B_ATK_PREFIX1}'s LUCKY CHANT\nwore off!")),
    [STRINGID_TRICKROOMENDS]                        = COMPOUND_STRING(VANILLA_TEXT("The twisted dimensions returned to normal!",
                                                                                   "The twisted dimensions\nreturned to normal!")),
    [STRINGID_WONDERROOMENDS]                       = COMPOUND_STRING(VANILLA_TEXT("Wonder Room wore off, and Defense and Sp. Def stats returned to normal!",
                                                                                   "WONDER ROOM wore off, and DEFENSE\nand SP. DEF returned to normal!")),
    [STRINGID_MAGICROOMENDS]                        = COMPOUND_STRING(VANILLA_TEXT("Magic Room wore off, and held items' effects returned to normal!",
                                                                                   "MAGIC ROOM wore off, and held\nitems' effects returned to normal!")),
    [STRINGID_MUDSPORTENDS]                         = COMPOUND_STRING(VANILLA_TEXT("The effects of Mud Sport have faded.",
                                                                                   "The effects of MUD SPORT\nhave faded.")),
    [STRINGID_WATERSPORTENDS]                       = COMPOUND_STRING(VANILLA_TEXT("The effects of Water Sport have faded.",
                                                                                   "The effects of WATER SPORT\nhave faded.")),
    [STRINGID_GRAVITYENDS]                          = COMPOUND_STRING(VANILLA_TEXT("Gravity returned to normal!",
                                                                                   "GRAVITY returned to normal!")),
    [STRINGID_AQUARINGHEAL]                         = COMPOUND_STRING(VANILLA_TEXT("A veil of water restored {B_ATK_NAME_WITH_PREFIX2}'s HP!",
                                                                                   "A veil of water restored\n{B_ATK_NAME_WITH_PREFIX}'s HP!")),
    [STRINGID_ELECTRICTERRAINENDS]                  = COMPOUND_STRING(VANILLA_TEXT("The electricity disappeared from the battlefield.",
                                                                                   "The electricity disappeared\nfrom the battlefield.")),
    [STRINGID_MISTYTERRAINENDS]                     = COMPOUND_STRING(VANILLA_TEXT("The mist disappeared from the battlefield.",
                                                                                   "The mist disappeared from\nthe battlefield.")),
    [STRINGID_PSYCHICTERRAINENDS]                   = COMPOUND_STRING(VANILLA_TEXT("The weirdness disappeared from the battlefield!",
                                                                                   "The weirdness disappeared\nfrom the battlefield!")),
    [STRINGID_GRASSYTERRAINENDS]                    = COMPOUND_STRING(VANILLA_TEXT("The grass disappeared from the battlefield.",
                                                                                   "The grass disappeared from\nthe battlefield.")),
    [STRINGID_TARGETABILITYSTATRAISE]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY} {B_BUFF2}raised its {B_BUFF1}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\n{B_BUFF2}raised its {B_BUFF1}!")), // Not in Gen 5+
    [STRINGID_STATWASMAXEDOUT]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} maxed its {B_BUFF1}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} maximized\nits {B_BUFF1}!")),
    [STRINGID_ATTACKERABILITYSTATRAISE]             = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY} {B_BUFF2}raised its {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY}\n{B_BUFF2}raised its {B_BUFF1}!")), // Not in Gen 5+
    [STRINGID_POISONHEALHPUP]                       = COMPOUND_STRING(VANILLA_TEXT("The poisoning healed {B_ATK_NAME_WITH_PREFIX2} a little bit!",
                                                                                   "The poisoning healed\n{B_ATK_NAME_WITH_PREFIX} a little!")), // Not in Gen 5+
    [STRINGID_BADDREAMSDMG]                         = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is tormented!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is\ntormented!")),
    [STRINGID_MOLDBREAKERENTERS]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} breaks the mold!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} breaks\nthe mold!")),
    [STRINGID_TERAVOLTENTERS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is radiating a bursting aura!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is radiating\na bursting aura!")),
    [STRINGID_TURBOBLAZEENTERS]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is radiating a blazing aura!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is radiating\na blazing aura!")),
    [STRINGID_SLOWSTARTENTERS]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is slow to get going!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is slow\nto get going!")),
    [STRINGID_SLOWSTARTEND]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} finally got its act together!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} finally got\nits act together!")),
    [STRINGID_SOLARPOWERHPDROP]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY} takes its toll!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY}\ntakes its toll!")), // Not in Gen 5+
    [STRINGID_PKMNWASHURT]                          = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} was hurt!"),
    [STRINGID_ANTICIPATIONACTIVATES]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} shuddered!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}\nshuddered!")),
    [STRINGID_FOREWARNACTIVATES]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_ABILITY} alerted {B_SCR_NAME_WITH_PREFIX2} to {B_EFF_NAME_WITH_PREFIX2}'s {B_BUFF1}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nalerted it to {B_EFF_NAME_WITH_PREFIX}'s\l{B_BUFF1}!")),
    [STRINGID_ICEBODYHPGAIN]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY} healed it a little bit!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY}\nrestored its HP a little!")), // Not in Gen 5+
    [STRINGID_SNOWWARNINGHAIL]                      = COMPOUND_STRING("It started to hail!"),
    [STRINGID_FRISKACTIVATES]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was frisked, revealing its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was frisked\nand {B_LAST_ITEM} was found!")),
    [STRINGID_UNNERVEENTERS]                        = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_TEAM1} side is too nervous to eat Berries!",
                                                                                   "{B_EFF_TEAM1} team is too nervous\nto eat BERRIES!")),
    [STRINGID_HARVESTBERRY]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} harvested its {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} harvested\none {B_LAST_ITEM}!")),
    [STRINGID_PROTEANTYPECHANGE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY} transformed it into the {B_BUFF1} type!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_ATK_ABILITY}\nmade it the {B_BUFF1} type!")),
    [STRINGID_SYMBIOSISITEMPASS]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} shared its {B_LAST_ITEM} with {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} shared its\n{B_LAST_ITEM} with\l{B_EFF_NAME_WITH_PREFIX}!")),
    [STRINGID_STEALTHROCKDMG]                       = COMPOUND_STRING(VANILLA_TEXT("Pointed stones dug into {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "Pointed stones dug into\n{B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_TOXICSPIKESABSORBED]                  = COMPOUND_STRING(VANILLA_TEXT("The toxic spikes disappeared from the ground around {B_EFF_TEAM2} side!",
                                                                                   "The TOXIC SPIKES disappeared\nfrom {B_EFF_TEAM2} side!")),
    [STRINGID_TOXICSPIKESPOISONED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was poisoned!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}\nwas poisoned!")),
    [STRINGID_TOXICSPIKESBADLYPOISONED]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was badly poisoned!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is badly\npoisoned!")),
    [STRINGID_STICKYWEBSWITCHIN]                    = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} was caught in a sticky web!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} was caught\nin a STICKY WEB!")),
    [STRINGID_HEALINGWISHCAMETRUE]                  = COMPOUND_STRING(VANILLA_TEXT("The healing wish came true for {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "The healing wish came true\nfor {B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_HEALINGWISHHEALED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} regained health!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} regained\nhealth!")),
    [STRINGID_LUNARDANCECAMETRUE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} became cloaked in mystical moonlight!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} became cloaked\nin mystical moonlight!")),
    [STRINGID_CURSEDBODYDISABLED]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_BUFF1} was disabled!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_BUFF1}\nwas disabled!")),
    [STRINGID_ATTACKERACQUIREDABILITY]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} acquired {B_ATK_ABILITY}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} acquired\n{B_ATK_ABILITY}!")), // Not in Gen 5+
    [STRINGID_TARGETABILITYSTATLOWER]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY} {B_BUFF2}lowered its {B_BUFF1}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_DEF_ABILITY}\n{B_BUFF2}lowered its {B_BUFF1}!")), // Not in Gen 5+
    [STRINGID_TARGETSTATWONTGOHIGHER]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1} won't go any higher!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_BUFF1}\nwon't go higher!")),
    [STRINGID_PKMNMOVEBOUNCEDABILITY]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE} was bounced back!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE}\nwas bounced back!")),
    [STRINGID_IMPOSTERTRANSFORM]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} transformed into {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} transformed\ninto {B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_ASSAULTVESTDOESNTALLOW]               = COMPOUND_STRING(VANILLA_TEXT("The effects of the {B_LAST_ITEM} prevent status moves from being used!\p",
                                                                                   "The {B_LAST_ITEM} prevents\nstatus moves from being used!\p")),
    [STRINGID_GRAVITYPREVENTSUSAGE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can't use {B_CURRENT_MOVE} because of gravity!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't use\n{B_CURRENT_MOVE} because of GRAVITY!\p")),
    [STRINGID_HEALBLOCKPREVENTSUSAGE]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was prevented from healing!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} was\nprevented from healing!\p")),
    [STRINGID_NOTDONEYET]                           = COMPOUND_STRING(VANILLA_TEXT("This move effect is not done yet!\p",
                                                                                   "This move effect is not\ndone yet!\p")),
    [STRINGID_STICKYWEBUSED]                        = COMPOUND_STRING(VANILLA_TEXT("A sticky web has been laid out on the ground on {B_DEF_TEAM2} side!",
                                                                                   "A STICKY WEB was laid out\naround the opponent's side!")),
    [STRINGID_QUASHSUCCESS]                         = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s move was postponed!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s move\nwas postponed!")),
    [STRINGID_PKMNBLEWAWAYTOXICSPIKES]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} blew away Toxic Spikes!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} blew away\nTOXIC SPIKES!")),
    [STRINGID_PKMNBLEWAWAYSTICKYWEB]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} blew away Sticky Web!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} blew away\nSTICKY WEB!")),
    [STRINGID_PKMNBLEWAWAYSTEALTHROCK]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} blew away Stealth Rock!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} blew away\nSTEALTH ROCK!")),
    [STRINGID_IONDELUGEON]                          = COMPOUND_STRING(VANILLA_TEXT("A deluge of ions showers the battlefield!",
                                                                                   "A deluge of ions showers\nthe battlefield!")),
    [STRINGID_TOPSYTURVYSWITCHEDSTATS]              = COMPOUND_STRING(VANILLA_TEXT("All stat changes on {B_DEF_NAME_WITH_PREFIX2} were inverted!",
                                                                                   "All stat changes on\n{B_DEF_NAME_WITH_PREFIX} were inverted!")),
    [STRINGID_TERRAINBECOMESMISTY]                  = COMPOUND_STRING(VANILLA_TEXT("Mist swirled around the battlefield!",
                                                                                   "Mist swirled about\nthe battlefield!")),
    [STRINGID_TERRAINBECOMESGRASSY]                 = COMPOUND_STRING(VANILLA_TEXT("Grass grew to cover the battlefield!",
                                                                                   "Grass grew to cover\nthe battlefield!")),
    [STRINGID_TERRAINBECOMESELECTRIC]               = COMPOUND_STRING(VANILLA_TEXT("An electric current ran across the battlefield!",
                                                                                   "An electric current runs\nacross the battlefield!")),
    [STRINGID_TERRAINBECOMESPSYCHIC]                = COMPOUND_STRING("The battlefield got weird!"),
    [STRINGID_TARGETELECTRIFIED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s moves have been electrified!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s moves\nhave been electrified!")),
    [STRINGID_MEGAEVOREACTING]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s {B_LAST_ITEM} is reacting to {B_ATK_TRAINER_NAME}'s Mega Ring!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nis reacting to {B_ATK_TRAINER_NAME}'s MEGA RING!")), //actually displays the type of mega ring in inventory, but we didnt implement them :(
    [STRINGID_MEGAEVOEVOLVED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} has Mega Evolved into Mega {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} has Mega Evolved\ninto MEGA {B_BUFF1}!")),
    [STRINGID_DRASTICALLY]                          = gText_drastically,
    [STRINGID_SEVERELY]                             = gText_severely,
    [STRINGID_INFESTATION]                          = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} has been afflicted with an infestation by {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} has been\nafflicted with an infestation\lby {B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_NOEFFECTONTARGET]                     = COMPOUND_STRING(VANILLA_TEXT("It won't have any effect on {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "It won't have any effect on\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_BURSTINGFLAMESHIT]                    = COMPOUND_STRING(VANILLA_TEXT("The bursting flames hit {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "The bursting flames hit\n{B_EFF_NAME_WITH_PREFIX}!")),
    [STRINGID_BESTOWITEMGIVING]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} received {B_LAST_ITEM} from {B_ATK_NAME_WITH_PREFIX2}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} received\n{B_LAST_ITEM} from\l{B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_THIRDTYPEADDED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} type was added to {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_BUFF1} type was added to\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_FELLFORFEINT]                         = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} fell for the feint!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} fell for\nthe FEINT!")),
    [STRINGID_POKEMONCANNOTUSEMOVE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cannot use {B_CURRENT_MOVE}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cannot use\n{B_CURRENT_MOVE}!")),
    [STRINGID_COVEREDINPOWDER]                      = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is covered in powder!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is covered\nin powder!")),
    [STRINGID_POWDEREXPLODES]                       = COMPOUND_STRING(VANILLA_TEXT("When the flame touched the powder on the POKéMON, it exploded!",
                                                                                   "When the flame touched the powder\non the POKéMON, it exploded!")),
    [STRINGID_BELCHCANTSELECT]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} hasn't eaten any held Berries, so it can't possibly belch!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} hasn't eaten a\nBERRY, so it can't BELCH!\p")),
    [STRINGID_SPECTRALTHIEFSTEAL]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} stole the target's boosted stats!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} stole the\ntarget's boosted stats!")),
    [STRINGID_GRAVITYGROUNDING]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} fell from the sky due to the gravity!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} fell from\nthe sky due to GRAVITY!")),
    [STRINGID_MISTYTERRAINPREVENTS]                 = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} surrounds itself with a protective mist!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} surrounds\nitself with a protective mist!")),
    [STRINGID_GRASSYTERRAINHEALS]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is healed by the grassy terrain!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is healed\nby the grassy terrain!")),
    [STRINGID_ELECTRICTERRAINPREVENTS]              = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} surrounds itself with electrified terrain!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} surrounds\nitself with electrified terrain!")),
    [STRINGID_PSYCHICTERRAINPREVENTS]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is protected by the Psychic Terrain!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is protected\nby the psychic terrain!")),
    [STRINGID_SAFETYGOGGLESPROTECTED]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is not affected thanks to its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is not affected\nthanks to its {B_LAST_ITEM}!")),
    [STRINGID_FLOWERVEILPROTECTED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} surrounded itself with a veil of petals!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} surrounded\nitself with a veil of petals!")),
    [STRINGID_FLOWERVEILPROTECTEDTARGET]            = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} surrounded itself with a veil of petals!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} surrounded\nitself with a veil of petals!")),
    [STRINGID_AROMAVEILPROTECTED]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is protected by an aromatic veil!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is protected\nby an aromatic veil!")),
    [STRINGID_CELEBRATEMESSAGE]                     = COMPOUND_STRING("Congratulations, {B_PLAYER_NAME}!"),
    [STRINGID_USEDINSTRUCTEDMOVE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} followed {B_ATK_NAME_WITH_PREFIX2}'s instructions!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} followed\n{B_ATK_NAME_WITH_PREFIX}'s instructions!")),
    [STRINGID_THROATCHOPENDS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can use sound-based moves again!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can use\nsound-based moves again!")),
    [STRINGID_PKMNCANTUSEMOVETHROATCHOP]            = COMPOUND_STRING(VANILLA_TEXT("The effects of Throat Chop prevent {B_ATK_NAME_WITH_PREFIX2} from using certain moves!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't use certain\nmoves due to THROAT CHOP!\p")),
    [STRINGID_LASERFOCUS]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} concentrated intensely!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} concentrated\nintensely!")),
    [STRINGID_GEMACTIVATES]                         = COMPOUND_STRING(VANILLA_TEXT("The {B_LAST_ITEM} strengthened {B_ATK_NAME_WITH_PREFIX2}'s power!",
                                                                                   "The {B_LAST_ITEM} strengthened\n{B_ATK_NAME_WITH_PREFIX}'s power!")),
    [STRINGID_BERRYDMGREDUCES]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} lessened the damage it took!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nlessened the damage!")),
    [STRINGID_AIRBALLOONFLOAT]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} floats in the air with its Air Balloon!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} floats in the\nair with its AIR BALLOON!")),
    [STRINGID_AIRBALLOONPOP]                        = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s Air Balloon popped!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s AIR BALLOON\npopped!")),
    [STRINGID_INCINERATEBURN]                       = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX}'s {B_LAST_ITEM} was burnt up!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nwas burned up!")),
    [STRINGID_BUGBITE]                              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} stole and ate its target's {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} stole and ate\nits target's {B_LAST_ITEM}!")),
    [STRINGID_ILLUSIONWOREOFF]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s illusion wore off!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s illusion\nwore off!")),
    [STRINGID_ATTACKERCUREDTARGETSTATUS]            = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cured {B_DEF_NAME_WITH_PREFIX2}'s problem!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cured\n{B_DEF_NAME_WITH_PREFIX}'s problem!")),
    [STRINGID_ATTACKERLOSTFIRETYPE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} burned itself out!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} burned\nitself out!")),
    [STRINGID_HEALERCURE]                           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was cured of {B_SCR_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} was cured\nof {B_SCR_NAME_WITH_PREFIX}!")),
    [STRINGID_SCRIPTINGABILITYSTATRAISE]            = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} {B_BUFF2}raised its {B_BUFF1}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\n{B_BUFF2}raised its {B_BUFF1}!")),
    [STRINGID_RECEIVERABILITYTAKEOVER]              = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} was taken over!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nwas taken over!")),
    [STRINGID_PKNMABSORBINGPOWER]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is absorbing power!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is absorbing\npower!")),
    [STRINGID_NOONEWILLBEABLETORUNAWAY]             = COMPOUND_STRING(VANILLA_TEXT("No one will be able to leave the battlefield during the next turn!",
                                                                                   "No one will be able to run\naway during the next turn!")),
    [STRINGID_DESTINYKNOTACTIVATES]                 = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} fell in love because of the {B_LAST_ITEM}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} fell in love\nfrom the {B_LAST_ITEM}!")),
    [STRINGID_CLOAKEDINAFREEZINGLIGHT]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became cloaked in a freezing light!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became cloaked\nin a freezing light!")),
    [STRINGID_CLEARAMULETWONTLOWERSTATS]            = COMPOUND_STRING(VANILLA_TEXT("The effects of the {B_LAST_ITEM} held by {B_SCR_NAME_WITH_PREFIX2} prevents its stats from being lowered!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nprevents stat loss!")),
    [STRINGID_FERVENTWISHREACHED]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_TRAINER_NAME}'s fervent wish has reached {B_ATK_NAME_WITH_PREFIX2}!",
                                                                                   "{B_ATK_TRAINER_NAME}'s fervent wish\nhas reached {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_AIRLOCKACTIVATES]                     = COMPOUND_STRING(VANILLA_TEXT("The effects of the weather disappeared.",
                                                                                   "The effects of weather\ndisappeared.")),
    [STRINGID_PRESSUREENTERS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is exerting its pressure!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is exerting\nits pressure!")),
    [STRINGID_DARKAURAENTERS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is radiating a dark aura!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is radiating\na dark aura!")),
    [STRINGID_FAIRYAURAENTERS]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is radiating a fairy aura!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is radiating\na fairy aura!")),
    [STRINGID_AURABREAKENTERS]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} reversed all other POKéMON's auras!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} reversed all\nother POKéMON's auras!")),
    [STRINGID_COMATOSEENTERS]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is drowsing!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is\ndrowsing!")),
    [STRINGID_SCREENCLEANERENTERS]                  = COMPOUND_STRING(VANILLA_TEXT("All screens on the field were cleansed!",
                                                                                   "All screens on the field\nwere cleansed!")),
    [STRINGID_FETCHEDPOKEBALL]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} found a {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} found\none {B_LAST_ITEM}!")),
    [STRINGID_ASANDSTORMKICKEDUP]                   = COMPOUND_STRING(VANILLA_TEXT("A sandstorm kicked up!",
                                                                                   "A sandstorm brewed!")),
    [STRINGID_PKMNSWILLPERISHIN3TURNS]              = COMPOUND_STRING(VANILLA_TEXT("Both POKéMON will faint in three turns!",
                                                                                   "Both POKéMON will faint\nin three turns!")),
    [STRINGID_AURAFLAREDTOLIFE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s aura flared to life!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s aura\nflared to life!")),
    [STRINGID_ASONEENTERS]                          = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} has two Abilities!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} has\ntwo abilities!")),
    [STRINGID_CURIOUSMEDICINEENTERS]                = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX}'s stat changes were removed!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX}'s stat changes\nwere removed!")),
    [STRINGID_CANACTFASTERTHANKSTO]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can act faster than normal, thanks to its {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s {B_BUFF1}\nlet it act faster than normal!")),
    [STRINGID_MICLEBERRYACTIVATES]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} boosted the accuracy of its next move using {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nboosted its next move's accuracy!")),
    [STRINGID_PKMNSHOOKOFFTHETAUNT]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} shook off the taunt!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} shook off\nthe TAUNT!")),
    [STRINGID_PKMNGOTOVERITSINFATUATION]            = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} got over its infatuation!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} got over\nits infatuation!")),
    [STRINGID_ITEMCANNOTBEREMOVED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s item cannot be removed!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s item\ncannot be removed!")),
    [STRINGID_STICKYBARBTRANSFER]                   = COMPOUND_STRING(VANILLA_TEXT("The {B_LAST_ITEM} attached itself to {B_ATK_NAME_WITH_PREFIX2}!",
                                                                                   "The {B_LAST_ITEM} attached\nitself to {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNBURNHEALED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX}'s burn was cured!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s burn\nwas healed!")),
    [STRINGID_REDCARDACTIVATE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} held up its Red Card against {B_ATK_NAME_WITH_PREFIX2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} held up its\nRED CARD against {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_EJECTBUTTONACTIVATE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is switched out with the {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is switched\nout with the {B_LAST_ITEM}!")),
    [STRINGID_ATKGOTOVERINFATUATION]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} cured its infatuation status using its {B_LAST_ITEM}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\ncured its infatuation!")),
    [STRINGID_TORMENTEDNOMORE]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is no longer tormented!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is no longer\ntormented!")),
    [STRINGID_HEALBLOCKEDNOMORE]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} is no longer prevented from healing!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} is no longer\nprevented from healing!")),
    [STRINGID_ATTACKERBECAMEFULLYCHARGED]           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became fully charged due to its bond with its trainer!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became fully\ncharged due to its bond with\lits TRAINER!\p")),
    [STRINGID_ATTACKERBECAMEASHSPECIES]             = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became Ash-Greninja!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became\nASH-GRENINJA!\p")),
    [STRINGID_EXTREMELYHARSHSUNLIGHT]               = COMPOUND_STRING(VANILLA_TEXT("The sunlight turned extremely harsh!",
                                                                                   "The sunlight turned\nextremely harsh!")),
    [STRINGID_EXTREMESUNLIGHTFADED]                 = COMPOUND_STRING(VANILLA_TEXT("The extremely harsh sunlight faded!",
                                                                                   "The extremely harsh sunlight\nfaded!")),
    [STRINGID_MOVEEVAPORATEDINTHEHARSHSUNLIGHT]     = COMPOUND_STRING(VANILLA_TEXT("The Water-type attack evaporated in the extremely harsh sunlight!",
                                                                                   "The WATER-type attack evaporated\nin the extremely harsh sunlight!")),
    [STRINGID_EXTREMELYHARSHSUNLIGHTWASNOTLESSENED] = COMPOUND_STRING(VANILLA_TEXT("The extremely harsh sunlight was not lessened at all!",
                                                                                   "The extremely harsh sunlight\nwas not lessened at all!")),
    [STRINGID_HEAVYRAIN]                            = COMPOUND_STRING("A heavy rain began to fall!"),
    [STRINGID_HEAVYRAINLIFTED]                      = COMPOUND_STRING("The heavy rain has lifted!"),
    [STRINGID_MOVEFIZZLEDOUTINTHEHEAVYRAIN]         = COMPOUND_STRING(VANILLA_TEXT("The Fire-type attack fizzled out in the heavy rain!",
                                                                                   "The FIRE-type attack fizzled out\nin the heavy rain!")),
    [STRINGID_NORELIEFROMHEAVYRAIN]                 = COMPOUND_STRING(VANILLA_TEXT("There is no relief from this heavy rain!",
                                                                                   "There is no relief from\nthis heavy rain!")),
    [STRINGID_MYSTERIOUSAIRCURRENT]                 = COMPOUND_STRING(VANILLA_TEXT("Mysterious strong winds are protecting Flying-type POKéMON!",
                                                                                   "Mysterious strong winds are\nprotecting FLYING-type POKéMON!")),
    [STRINGID_STRONGWINDSDISSIPATED]                = COMPOUND_STRING(VANILLA_TEXT("The mysterious strong winds have dissipated!",
                                                                                   "The mysterious strong winds\nhave dissipated!")),
    [STRINGID_MYSTERIOUSAIRCURRENTBLOWSON]          = COMPOUND_STRING(VANILLA_TEXT("The mysterious strong winds blow on regardless!",
                                                                                   "The mysterious strong winds\nblow on regardless!")),
    [STRINGID_ATTACKWEAKENEDBSTRONGWINDS]           = COMPOUND_STRING(VANILLA_TEXT("The mysterious strong winds weakened the attack!",
                                                                                   "The mysterious strong winds\nweakened the attack!")),
    [STRINGID_STUFFCHEEKSCANTSELECT]                = COMPOUND_STRING(VANILLA_TEXT("It can't use the move because it doesn't have a Berry!\p",
                                                                                   "It can't use the move because\nit doesn't have a BERRY!\p")),
    [STRINGID_PKMNREVERTEDTOPRIMAL]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s Primal Reversion! It reverted to its primal state!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s Primal Reversion!\pIt reverted to its primal state!")),
    [STRINGID_BUTPOKEMONCANTUSETHEMOVE]             = COMPOUND_STRING(VANILLA_TEXT("But {B_ATK_NAME_WITH_PREFIX2} can't use the move!",
                                                                                   "But {B_ATK_NAME_WITH_PREFIX} can't\nuse the move!")),
    [STRINGID_BUTHOOPACANTUSEIT]                    = COMPOUND_STRING(VANILLA_TEXT("But {B_ATK_NAME_WITH_PREFIX2} can't use it the way it is now!",
                                                                                   "But {B_ATK_NAME_WITH_PREFIX} can't use\nit the way it is now!")),
    [STRINGID_BROKETHROUGHPROTECTION]               = COMPOUND_STRING(VANILLA_TEXT("It broke through {B_EFF_NAME_WITH_PREFIX2}'s protection!",
                                                                                   "It broke through\n{B_EFF_NAME_WITH_PREFIX}'s protection!")),
    [STRINGID_ABILITYALLOWSONLYMOVE]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_ABILITY} only allows the use of {B_CURRENT_MOVE}!\p",
                                                                                   "{B_ATK_ABILITY} allows the\nuse of only {B_CURRENT_MOVE}!\p")),
    [STRINGID_SWAPPEDABILITIES]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} swapped Abilities with its target!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} swapped abilities\nwith its opponent!")),
    [STRINGID_PKMNHEALEDPOISON]                     = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} was cured of its poisoning!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} was cured\nof its poisoning!")),
    [STRINGID_BATTLERTYPECHANGEDTO]                 = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s type changed to {B_BUFF1}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s type\nchanged to {B_BUFF1}!")),
    [STRINGID_BOTHCANNOLONGERESCAPE]                = COMPOUND_STRING(VANILLA_TEXT("Neither POKéMON can run away!",
                                                                                   "Neither POKéMON can\nrun away!")),
    [STRINGID_CANTESCAPEDUETOUSEDMOVE]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} can no longer escape because it used No Retreat!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} can't escape\nbecause it used NO RETREAT!")),
    [STRINGID_PKMNBECAMEWEAKERTOFIRE]               = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} became weaker to fire!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} became\nweaker to FIRE!")),
    [STRINGID_ABOUTTOUSEPOLTERGEIST]                = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} is about to be attacked by its {B_LAST_ITEM}!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} is about to be\nattacked by its {B_LAST_ITEM}!")),
    [STRINGID_CANTESCAPEBECAUSEOFCURRENTMOVE]       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} can no longer escape because of Octolock!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} can't escape\nbecause of OCTOLOCK!")),
    [STRINGID_NEUTRALIZINGGASENTERS]                = COMPOUND_STRING(VANILLA_TEXT("Neutralizing gas filled the area!",
                                                                                   "Neutralizing gas filled\nthe area!")),
    [STRINGID_NEUTRALIZINGGASOVER]                  = COMPOUND_STRING(VANILLA_TEXT("The effects of the neutralizing gas wore off!",
                                                                                   "The effects of the neutralizing\ngas wore off!")),
    [STRINGID_TARGETTOOHEAVY]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} is too heavy to be lifted!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} is too heavy\nto be lifted!")),
    [STRINGID_PKMNTOOKTARGETHIGH]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} took {B_DEF_NAME_WITH_PREFIX2} into the sky!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} took\n{B_DEF_NAME_WITH_PREFIX} into the sky!")),
    [STRINGID_PKMNINSNAPTRAP]                       = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} got trapped by a snap trap!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} got trapped\nby a snap trap!")),
    [STRINGID_METEORBEAMCHARGING]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is overflowing with space power!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is overflowing\nwith space power!")),
    [STRINGID_HEATUPBEAK]                           = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} started heating up its beak!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} started\nheating up its beak!")),
    [STRINGID_COURTCHANGE]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} swapped the battle effects affecting each side of the field!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} swapped the\nbattle effects on each side!")),
    [STRINGID_ZPOWERSURROUNDS]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} surrounded itself with its Z-Power!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} surrounded\nitself with its Z-POWER!")),
    [STRINGID_ZMOVEUNLEASHED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} unleashes its full-force Z-Move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} unleashes its\nfull-force Z-MOVE!")),
    [STRINGID_ZMOVERESETSSTATS]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} returned its decreased stats to normal using its Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} returned its\nlowered stats to normal using\lits Z-POWER!")),
    [STRINGID_ZMOVEALLSTATSUP]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} boosted its stats using its Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} boosted its\nstats using its Z-POWER!")),
    [STRINGID_ZMOVEZBOOSTCRIT]                      = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} boosted its critical-hit ratio using its Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} boosted its\ncritical-hit ratio using\lits Z-POWER!")),
    [STRINGID_ZMOVERESTOREHP]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} restored its HP using its Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} restored its\nHP using its Z-POWER!")),
    [STRINGID_ZMOVESTATUP]                          = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} boosted its stats using its Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} boosted its\nstats using its Z-POWER!")),
    [STRINGID_ZMOVEHPTRAP]                          = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s HP was restored by the Z-Power!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s HP was\nrestored by the Z-POWER!")),
    [STRINGID_ATTACKEREXPELLEDTHEPOISON]            = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} managed to expel the poison so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} managed to\nexpel the poison so you\lwouldn't worry!")),
    [STRINGID_ATTACKERSHOOKITSELFAWAKE]             = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} shook itself awake so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shook itself\nawake so you wouldn't worry!")),
    [STRINGID_ATTACKERBROKETHROUGHPARALYSIS]        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} gathered all its energy to break through its paralysis so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} gathered all its\nenergy to break through its\lparalysis so you wouldn't worry!")),
    [STRINGID_ATTACKERHEALEDITSBURN]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cured its burn through sheer determination so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cured its burn\nthrough sheer determination\lso you wouldn't worry!")),
    [STRINGID_ATTACKERMELTEDTHEICE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} melted the ice with its fiery determination so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} melted the ice\nwith its fiery determination\lso you wouldn't worry!")),
    [STRINGID_TARGETTOUGHEDITOUT]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} toughed it out so you wouldn't feel sad!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} toughed it\nout so you wouldn't feel sad!")),
    [STRINGID_ATTACKERLOSTELECTRICTYPE]             = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} used up all its electricity!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} used up all\nits electricity!")),
    [STRINGID_ATTACKERSWITCHEDSTATWITHTARGET]       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} switched {B_BUFF1} with its target!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} switched\n{B_BUFF1} with its opponent!")),
    [STRINGID_BEINGHITCHARGEDPKMNWITHPOWER]         = COMPOUND_STRING(VANILLA_TEXT("Being hit by {B_CURRENT_MOVE} charged {B_EFF_NAME_WITH_PREFIX2} with power!",
                                                                                   "Being hit by {B_CURRENT_MOVE}\ncharged {B_EFF_NAME_WITH_PREFIX} with power!")),
    [STRINGID_SUNLIGHTACTIVATEDABILITY]             = COMPOUND_STRING(VANILLA_TEXT("The harsh sunlight activated {B_SCR_NAME_WITH_PREFIX2}'s Protosynthesis!",
                                                                                   "The harsh sunlight activated\n{B_SCR_NAME_WITH_PREFIX}'s PROTOSYNTHESIS!")),
    [STRINGID_ORICHALCUMPULSEACTIVATES]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} turned the sunlight harsh, sending its ancient pulse into a frenzy!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} turned the\nsunlight harsh, sending its\lancient pulse into a frenzy!")),
    [STRINGID_ORICHALCUMPULSEACTIVATESINSUN]        = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} basked in the sunlight, sending its ancient pulse into a frenzy!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} basked in the\nsunlight, sending its ancient\lpulse into a frenzy!")),
    [STRINGID_STATWASHEIGHTENED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1} was heightened!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_BUFF1}\nwas heightened!")),
    [STRINGID_ELECTRICTERRAINACTIVATEDABILITY]      = COMPOUND_STRING(VANILLA_TEXT("The Electric Terrain activated {B_SCR_NAME_WITH_PREFIX2}'s Quark Drive!",
                                                                                   "The electric terrain activated\n{B_SCR_NAME_WITH_PREFIX}'s QUARK DRIVE!")),
    [STRINGID_HADRONENGINEACTIVATES]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} turned the ground into Electric Terrain, energizing its futuristic engine!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} electrified the\nground, energizing its\lfuturistic engine!")),
    [STRINGID_HADRONENGINEACTIVATESINTERRAIN]       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} used the Electric Terrain to energize its futuristic engine!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} used the\nelectric terrain to energize\lits futuristic engine!")),
    [STRINGID_ABILITYWEAKENEDSURROUNDINGMONSSTAT]   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} weakened the {B_BUFF1} of all surrounding POKéMON!\p",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nweakened the {B_BUFF1} of\lall surrounding POKéMON!\p")),
    [STRINGID_ATTACKERGAINEDSTRENGTHFROMTHEFALLEN]  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} gained strength from the fallen!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} gained\nstrength from the fallen!")),
    [STRINGID_PKMNSABILITYPREVENTSABILITY]          = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY} prevents {B_DEF_NAME_WITH_PREFIX2}'s {B_DEF_ABILITY} from working!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_SCR_ABILITY}\nprevents {B_DEF_NAME_WITH_PREFIX}'s\l{B_DEF_ABILITY} from working!")), //not in gen 5+, ability popup
    [STRINGID_PREPARESHELLTRAP]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} set a shell trap!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} set a\nSHELL TRAP!")),
    [STRINGID_SHELLTRAPDIDNTWORK]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s shell trap didn't work!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s SHELL TRAP\ndidn't work!")),
    [STRINGID_SPIKESDISAPPEAREDFROMTEAM]            = COMPOUND_STRING(VANILLA_TEXT("The spikes disappeared from the ground around {B_ATK_TEAM2} side!",
                                                                                   "The SPIKES disappeared\nfrom {B_ATK_TEAM2} side!")),
    [STRINGID_TOXICSPIKESDISAPPEAREDFROMTEAM]       = COMPOUND_STRING(VANILLA_TEXT("The toxic spikes disappeared from the ground around {B_ATK_TEAM2} side!",
                                                                                   "The TOXIC SPIKES disappeared\nfrom {B_ATK_TEAM2} side!")),
    [STRINGID_STICKYWEBDISAPPEAREDFROMTEAM]         = COMPOUND_STRING(VANILLA_TEXT("The sticky web has disappeared from the ground on {B_ATK_TEAM2} side!",
                                                                                   "The STICKY WEB disappeared\nfrom {B_ATK_TEAM2} side!")),
    [STRINGID_STEALTHROCKDISAPPEAREDFROMTEAM]       = COMPOUND_STRING(VANILLA_TEXT("The pointed stones disappeared from {B_ATK_TEAM2} side!",
                                                                                   "The pointed stones disappeared\nfrom {B_ATK_TEAM2} side!")),
    [STRINGID_COULDNTFULLYPROTECT]                  = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} couldn't fully protect itself and got hurt!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} couldn't fully\nprotect itself and got hurt!")),
    [STRINGID_STOCKPILEDEFFECTWOREOFF]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s stockpiled effect wore off!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s STOCKPILE\neffect wore off!")),
    [STRINGID_PKMNREVIVEDREADYTOFIGHT]              = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} was revived and is ready to fight again!",
                                                                                   "{B_BUFF1} was revived and\nis ready to fight again!")),
    [STRINGID_ITEMRESTOREDSPECIESHEALTH]            = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} had its HP restored.",
                                                                                   "{B_BUFF1}'s HP was restored.")),
    [STRINGID_ITEMCUREDSPECIESSTATUS]               = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} had its status healed!",
                                                                                   "{B_BUFF1}'s status\nreturned to normal!")), // Not in Gen 5+
    [STRINGID_ITEMRESTOREDSPECIESPP]                = COMPOUND_STRING(VANILLA_TEXT("The PP of {B_BUFF1}'s {B_BUFF2} was restored!",
                                                                                   "The PP of {B_BUFF1}'s\n{B_BUFF2} was restored!")),
    [STRINGID_THUNDERCAGETRAPPED]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} trapped {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} trapped\n{B_EFF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNHURTBYFROSTBITE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by its frostbite!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby its frostbite!")),
    [STRINGID_PKMNGOTFROSTBITE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} got frostbite!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} got\nfrostbite!")),
    [STRINGID_PKMNSITEMHEALEDFROSTBITE]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM} cured its frostbite!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nhealed its frostbite!")),
    [STRINGID_ATTACKERHEALEDITSFROSTBITE]           = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} cured its frostbite through sheer determination so you wouldn't worry!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} cured its\nfrostbite through sheer\ldetermination so you wouldn't worry!")),
    [STRINGID_PKMNFROSTBITEHEALED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s frostbite was cured!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s frostbite\nwas healed!")),
    [STRINGID_PKMNFROSTBITEHEALEDBY]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE} cured its frostbite!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s {B_CURRENT_MOVE}\nhealed its frostbite!")),
    [STRINGID_MIRRORHERBCOPIED]                     = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} used its Mirror Herb to mirror its opponent's stat changes!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s MIRROR HERB\ncopied its opponent's stat changes!")),
    [STRINGID_STARTEDSNOW]                          = COMPOUND_STRING("It started to snow!"),
    [STRINGID_SNOWCONTINUES]                        = COMPOUND_STRING("Snow continues to fall."), //not in gen 5+ (lol)
    [STRINGID_SNOWSTOPPED]                          = COMPOUND_STRING("The snow stopped."),
    [STRINGID_SNOWWARNINGSNOW]                      = COMPOUND_STRING("It started to snow!"),
    [STRINGID_PKMNITEMMELTED]                       = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} corroded {B_DEF_NAME_WITH_PREFIX2}'s {B_LAST_ITEM}!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX}'s {B_LAST_ITEM}\nwas corroded by {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_ULTRABURSTREACTING]                   = COMPOUND_STRING(VANILLA_TEXT("Bright light is about to burst out of {B_ATK_NAME_WITH_PREFIX2}!",
                                                                                   "Bright light is about to burst\nout of {B_ATK_NAME_WITH_PREFIX}!")),
    [STRINGID_ULTRABURSTCOMPLETED]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} regained its true power through Ultra Burst!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} regained its\ntrue power through ULTRA BURST!")),
    [STRINGID_TEAMGAINEDEXP]                        = COMPOUND_STRING(VANILLA_TEXT("The rest of your team gained EXP. Points thanks to the EXP. SHARE!\p",
                                                                                   "The rest of your team gained EXP.\nPoints thanks to the EXP. SHARE!\p")),
    [STRINGID_CURRENTMOVECANTSELECT]                = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} cannot be used!\p",
                                                                                   "{B_BUFF1} can't be used!\p")),
    [STRINGID_TARGETISBEINGSALTCURED]               = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} is being salt cured!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} is being\nsalt cured!")),
    [STRINGID_TARGETISHURTBYSALTCURE]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hurt by {B_BUFF1}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby {B_BUFF1}!")),
    [STRINGID_TARGETCOVEREDINSTICKYCANDYSYRUP]      = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} got covered in sticky candy syrup!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} got covered\nin sticky candy syrup!")),
    [STRINGID_SHARPSTEELFLOATS]                     = COMPOUND_STRING(VANILLA_TEXT("Sharp-pointed pieces of steel started floating around {B_DEF_TEAM2} POKéMON!",
                                                                                   "Sharp pieces of steel started\nfloating around {B_DEF_TEAM2} team!")),
    [STRINGID_SHARPSTEELDMG]                        = COMPOUND_STRING(VANILLA_TEXT("The sharp steel bit into {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "The sharp steel bit into\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_PKMNBLEWAWAYSHARPSTEEL]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} blew away sharp steel!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} blew away\nthe sharp steel!")),
    [STRINGID_SHARPSTEELDISAPPEAREDFROMTEAM]        = COMPOUND_STRING(VANILLA_TEXT("The pieces of steel surrounding {B_ATK_TEAM2} POKéMON disappeared!",
                                                                                   "The pieces of steel around\n{B_ATK_TEAM2} team disappeared!")),
    [STRINGID_TEAMTRAPPEDWITHVINES]                 = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_TEAM1} POKéMON got trapped with vines!",
                                                                                   "{B_EFF_TEAM1} team got trapped\nwith vines!")),
    [STRINGID_PKMNHURTBYVINES]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hurt by G-Max Vine Lash's ferocious beating!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt by\nG-MAX VINE LASH's ferocious\lbeating!")),
    [STRINGID_TEAMCAUGHTINVORTEX]                   = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_TEAM1} POKéMON got caught in a vortex of water!",
                                                                                   "{B_EFF_TEAM1} team got caught\nin a vortex of water!")),
    [STRINGID_PKMNHURTBYVORTEX]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hurt by G-Max Cannonade's vortex!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt by\nG-MAX CANNONADE's vortex!")),
    [STRINGID_TEAMSURROUNDEDBYFIRE]                 = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_TEAM1} POKéMON were surrounded by fire!",
                                                                                   "{B_EFF_TEAM1} team was\nsurrounded by fire!")),
    [STRINGID_PKMNBURNINGUP]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is burning up within G-Max Wildfire's flames!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is burning up\nwithin G-MAX WILDFIRE's flames!")),
    [STRINGID_TEAMSURROUNDEDBYROCKS]                = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_TEAM1} POKéMON became surrounded by rocks!",
                                                                                   "{B_EFF_TEAM1} team became\nsurrounded by rocks!")),
    [STRINGID_PKMNHURTBYROCKSTHROWN]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is hurt by rocks thrown out by G-Max Volcalith!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt by rocks\nthrown out by G-MAX VOLCALITH!")),
    [STRINGID_MOVEBLOCKEDBYDYNAMAX]                 = COMPOUND_STRING(VANILLA_TEXT("The move was blocked by the power of Dynamax!",
                                                                                   "The move was blocked by\nthe power of DYNAMAX!")),
    [STRINGID_ZEROTOHEROTRANSFORMATION]             = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} underwent a heroic transformation!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} underwent a\nheroic transformation!")),
    [STRINGID_THETWOMOVESBECOMEONE]                 = COMPOUND_STRING(VANILLA_TEXT("The two moves have become one! It's a combined move!{PAUSE 16}",
                                                                                   "The two moves have become one!\nIt's a combined move!{PAUSE 16}")),
    [STRINGID_ARAINBOWAPPEAREDONSIDE]               = COMPOUND_STRING(VANILLA_TEXT("A rainbow appeared in the sky on {B_EFF_TEAM2} side!",
                                                                                   "A rainbow appeared in the sky\non {B_EFF_TEAM2} side!")),
    [STRINGID_THERAINBOWDISAPPEARED]                = COMPOUND_STRING(VANILLA_TEXT("The rainbow on {B_ATK_TEAM2} side disappeared!",
                                                                                   "The rainbow on {B_ATK_TEAM2} side\ndisappeared!")),
    [STRINGID_WAITINGFORPARTNERSMOVE]               = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is waiting for {B_ATK_PARTNER_NAME}'s move…{PAUSE 16}",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is waiting for\n{B_ATK_PARTNER_NAME}'s move…{PAUSE 16}")),
    [STRINGID_SEAOFFIREENVELOPEDSIDE]               = COMPOUND_STRING(VANILLA_TEXT("A sea of fire enveloped {B_EFF_TEAM2} side!",
                                                                                   "A sea of fire enveloped\n{B_EFF_TEAM2} side!")),
    [STRINGID_HURTBYTHESEAOFFIRE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} was hurt by the sea of fire!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is hurt\nby the sea of fire!")),
    [STRINGID_THESEAOFFIREDISAPPEARED]              = COMPOUND_STRING(VANILLA_TEXT("The sea of fire around {B_ATK_TEAM2} side disappeared!",
                                                                                   "The sea of fire around\n{B_ATK_TEAM2} side disappeared!")),
    [STRINGID_SWAMPENVELOPEDSIDE]                   = COMPOUND_STRING(VANILLA_TEXT("A swamp enveloped {B_EFF_TEAM2} side!",
                                                                                   "A swamp enveloped\n{B_EFF_TEAM2} side!")),
    [STRINGID_THESWAMPDISAPPEARED]                  = COMPOUND_STRING(VANILLA_TEXT("The swamp around {B_ATK_TEAM2} side disappeared!",
                                                                                   "The swamp around\n{B_ATK_TEAM2} side disappeared!")),
    [STRINGID_PKMNTELLCHILLINGRECEPTIONJOKE]        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is preparing to tell a chillingly bad joke!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is preparing to\ntell a chillingly bad joke!")),
    [STRINGID_HOSPITALITYRESTORATION]               = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} drank down all the matcha that {B_SCR_NAME_WITH_PREFIX2} made!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} drank down all\nthe matcha that\l{B_SCR_NAME_WITH_PREFIX} made!")),
    [STRINGID_ELECTROSHOTCHARGING]                  = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} absorbed electricity!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} absorbed\nelectricity!")),
    [STRINGID_ITEMWASUSEDUP]                        = COMPOUND_STRING(VANILLA_TEXT("The {B_LAST_ITEM} was used up…",
                                                                                   "The {B_LAST_ITEM}\nwas used up…")),
    [STRINGID_ATTACKERLOSTITSTYPE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_EFF_NAME_WITH_PREFIX} lost its {B_BUFF1} type!",
                                                                                   "{B_EFF_NAME_WITH_PREFIX} lost its\n{B_BUFF1} type!")),
    [STRINGID_SHEDITSTAIL]                          = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} shed its tail to create a decoy!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} shed its tail\nto create a decoy!")),
    [STRINGID_CLOAKEDINAHARSHLIGHT]                 = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} became cloaked in a harsh light!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} became cloaked\nin a harsh light!")),
    [STRINGID_SUPERSWEETAROMAWAFTS]                 = COMPOUND_STRING(VANILLA_TEXT("A supersweet aroma is wafting from the syrup covering {B_EFF_NAME_WITH_PREFIX2}!",
                                                                                   "A supersweet aroma is wafting from\nthe syrup covering\l{B_EFF_NAME_WITH_PREFIX}!")),
    [STRINGID_DIMENSIONSWERETWISTED]                = COMPOUND_STRING("The dimensions were twisted!"),
    [STRINGID_BIZARREARENACREATED]                  = COMPOUND_STRING(VANILLA_TEXT("A bizarre area was created in which POKéMON's held items lose their effects!",
                                                                                   "A bizarre area was created in which\nheld items lose their effects!")),
    [STRINGID_BIZARREAREACREATED]                   = COMPOUND_STRING(VANILLA_TEXT("A bizarre area was created in which Defense and Sp. Def stats are swapped!",
                                                                                   "A bizarre area was created in which\nDEFENSE and SP. DEF are swapped!")),
    [STRINGID_TIDYINGUPCOMPLETE]                    = COMPOUND_STRING("Tidying up complete!"),
    [STRINGID_PKMNTERASTALLIZEDINTO]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} terastallized into the {B_BUFF1} type!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} terastallized\ninto the {B_BUFF1} type!")), // Does not exist, meant to mimic form change strings
    [STRINGID_BOOSTERENERGYACTIVATES]               = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} used its {B_LAST_ITEM} to activate {B_SCR_ABILITY}!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} used its\n{B_LAST_ITEM} to activate\l{B_SCR_ABILITY}!")),
    [STRINGID_FOGCREPTUP]                           = COMPOUND_STRING("Fog crept up as thick as soup!"),
    [STRINGID_FOGISDEEP]                            = COMPOUND_STRING("The fog is deep…"),
    [STRINGID_FOGLIFTED]                            = COMPOUND_STRING("The fog lifted."),
    [STRINGID_PKMNMADESHELLGLEAM]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_NAME_WITH_PREFIX} made its shell gleam! It's distorting type matchups!",
                                                                                   "{B_DEF_NAME_WITH_PREFIX} made its\nshell gleam!\pIt's distorting type matchups!")),
    [STRINGID_FICKLEBEAMDOUBLED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is going all out for this attack!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is going all\nout for this attack!")),
    [STRINGID_COMMANDERACTIVATES]                   = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was swallowed by {B_BUFF1} and became {B_BUFF1}'s commander!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was swallowed\nby {B_BUFF1} and became\l{B_BUFF1}'s commander!")),
    [STRINGID_POKEFLUTECATCHY]                      = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_NAME} played the {B_LAST_ITEM}.\pNow, that's a catchy tune!",
                                                                                   "{B_PLAYER_NAME} played the\n{B_LAST_ITEM}.\pNow, that's a catchy tune!")),
    [STRINGID_POKEFLUTE]                            = COMPOUND_STRING(VANILLA_TEXT("{B_PLAYER_NAME} played the {B_LAST_ITEM}.",
                                                                                   "{B_PLAYER_NAME} played the\n{B_LAST_ITEM}.")),
    [STRINGID_MONHEARINGFLUTEAWOKE]                 = COMPOUND_STRING(VANILLA_TEXT("The POKéMON hearing the flute awoke!",
                                                                                   "The POKéMON hearing the FLUTE\nawoke!")),
    [STRINGID_SUNLIGHTISHARSH]                      = COMPOUND_STRING(VANILLA_TEXT("The sunlight is harsh!",
                                                                                   "The sunlight is strong.")),
    [STRINGID_ITISHAILING]                          = COMPOUND_STRING(VANILLA_TEXT("It's hailing!",
                                                                                   "It is hailing.")),
    [STRINGID_ITISSNOWING]                          = COMPOUND_STRING(VANILLA_TEXT("It's snowing!",
                                                                                   "It is snowing.")),
    [STRINGID_ISCOVEREDWITHGRASS]                   = COMPOUND_STRING(VANILLA_TEXT("The battlefield is covered with grass!",
                                                                                   "The battlefield is covered\nwith grass!")),
    [STRINGID_MISTSWIRLSAROUND]                     = COMPOUND_STRING(VANILLA_TEXT("Mist swirls around the battlefield!",
                                                                                   "Mist swirls around\nthe battlefield!")),
    [STRINGID_ELECTRICCURRENTISRUNNING]             = COMPOUND_STRING(VANILLA_TEXT("An electric current is running across the battlefield!",
                                                                                   "An electric current is running\nacross the battlefield!")),
    [STRINGID_SEEMSWEIRD]                           = COMPOUND_STRING("The battlefield seems weird!"),
    [STRINGID_WAGGLINGAFINGER]                      = COMPOUND_STRING(VANILLA_TEXT("Waggling a finger let it use {B_CURRENT_MOVE}!",
                                                                                   "Waggling a finger let it\nuse {B_CURRENT_MOVE}!")),
    [STRINGID_BLOCKEDBYSLEEPCLAUSE]                 = COMPOUND_STRING(VANILLA_TEXT("Sleep Clause kept {B_DEF_NAME_WITH_PREFIX2} awake!",
                                                                                   "SLEEP CLAUSE kept\n{B_DEF_NAME_WITH_PREFIX} awake!")),
    [STRINGID_SUPEREFFECTIVETWOFOES]                = COMPOUND_STRING(VANILLA_TEXT("It's super effective on {B_DEF_NAME_WITH_PREFIX2} and {B_DEF_PARTNER_NAME}!",
                                                                                   "It's super effective on\n{B_DEF_NAME_WITH_PREFIX} and {B_DEF_PARTNER_NAME}!")),
    [STRINGID_NOTVERYEFFECTIVETWOFOES]              = COMPOUND_STRING(VANILLA_TEXT("It's not very effective on {B_DEF_NAME_WITH_PREFIX2} and {B_DEF_PARTNER_NAME}.",
                                                                                   "It's not very effective on\n{B_DEF_NAME_WITH_PREFIX} and {B_DEF_PARTNER_NAME}…")),
    [STRINGID_ITDOESNTAFFECTTWOFOES]                = COMPOUND_STRING(VANILLA_TEXT("It doesn't affect {B_DEF_NAME_WITH_PREFIX2} and {B_DEF_PARTNER_NAME}…",
                                                                                   "It doesn't affect\n{B_DEF_NAME_WITH_PREFIX} and {B_DEF_PARTNER_NAME}…")),
    [STRINGID_SENDCAUGHTMONPARTYORBOX]              = COMPOUND_STRING(VANILLA_TEXT("Add {B_DEF_NAME} to your party?",
                                                                                   "Add {B_DEF_NAME} to\nyour party?")),
    [STRINGID_PKMNSENTTOPCAFTERCATCH]               = gText_PkmnSentToPCAfterCatch,
    [STRINGID_PKMNDYNAMAXED]                        = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} grew huge into its Dynamax form!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} grew huge\ninto its DYNAMAX form!")),
    [STRINGID_PKMNGIGANTAMAXED]                     = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} grew huge into its Gigantamax form!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} grew huge\ninto its GIGANTAMAX form!")),
    [STRINGID_TIMETODYNAMAX]                        = COMPOUND_STRING(VANILLA_TEXT("Time to Dynamax!",
                                                                                   "Time to DYNAMAX!")),
    [STRINGID_TIMETOGIGANTAMAX]                     = COMPOUND_STRING(VANILLA_TEXT("Time to Gigantamax!",
                                                                                   "Time to GIGANTAMAX!")),
    [STRINGID_QUESTIONFORFEITBATTLE]                = COMPOUND_STRING(VANILLA_TEXT("Would you like to give up on this battle and quit now? Quitting the battle is the same as losing the battle.",
                                                                                   "Quitting will count as a loss.\nGive up on this battle and quit?")),
    [STRINGID_POWERCONSTRUCTPRESENCEOFMANY]         = COMPOUND_STRING("You sense the presence of many!"),
    [STRINGID_POWERCONSTRUCTTRANSFORM]              = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} transformed into its Complete Forme!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} transformed\ninto its Complete Forme!")),
    [STRINGID_ABILITYSHIELDPROTECTS]                = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX}'s Ability is protected by the effects of its {B_LAST_ITEM}!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX}'s ability is\nprotected by its {B_LAST_ITEM}!")),
    [STRINGID_MONTOOSCAREDTOMOVE]                   = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} is too scared to move!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} is too scared\nto move!")),
    [STRINGID_GHOSTGETOUTGETOUT]                    = COMPOUND_STRING("GHOST: Get out…… Get out……"),
    [STRINGID_SILPHSCOPEUNVEILED]                   = COMPOUND_STRING("SILPH SCOPE unveiled the GHOST's\nidentity!"),
    [STRINGID_GHOSTWASMAROWAK]                      = COMPOUND_STRING("The GHOST was MAROWAK!\p"),
    [STRINGID_TRAINER1MON1COMEBACK]                 = COMPOUND_STRING("{B_TRAINER1_NAME}: {B_OPPONENT_MON1_NAME}, come back!"),
    [STRINGID_THREWROCK]                            = COMPOUND_STRING("{B_PLAYER_NAME} threw a ROCK\nat the {B_OPPONENT_MON1_NAME}!"),
    [STRINGID_THREWBAIT]                            = COMPOUND_STRING("{B_PLAYER_NAME} threw some BAIT\nat the {B_OPPONENT_MON1_NAME}!"),
    [STRINGID_PKMNANGRY]                            = COMPOUND_STRING("{B_OPPONENT_MON1_NAME} is angry!"),
    [STRINGID_PKMNEATING]                           = COMPOUND_STRING("{B_OPPONENT_MON1_NAME} is eating!"),
    [STRINGID_PKMNDISGUISEWASBUSTED]                = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s disguise was busted!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s disguise\nwas busted!")),
    [STRINGID_ZENMODETRIGGERED]                     = COMPOUND_STRING("{B_SCR_ABILITY} triggered!"),
    [STRINGID_ZENMODEENDED]                         = COMPOUND_STRING("{B_SCR_ABILITY} ended!"),
    [STRINGID_SCRCUREDPARALYSIS]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was cured of paralysis!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was\nhealed of paralysis!")),
    [STRINGID_SCRCUREDPOISON]                       = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} was cured of its poisoning!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} was cured\nof its poisoning!")),
    [STRINGID_SCRCUREDBURN]                         = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s burn was cured!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s burn\nwas healed!")),
    [STRINGID_SCRCUREDSLEEP]                        = COMPOUND_STRING("{B_SCR_NAME_WITH_PREFIX} woke up!"),
    [STRINGID_SCRCUREDCONFUSION]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX} snapped out of its confusion!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX} snapped\nout of confusion!")),
    [STRINGID_PARTYCUREDPARALYSIS]                  = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} was cured of paralysis!",
                                                                                   "{B_BUFF1} was\nhealed of paralysis!")),
    [STRINGID_PARTYCUREDPOISON]                     = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} was cured of its poisoning!",
                                                                                   "{B_BUFF1} was cured\nof its poisoning!")),
    [STRINGID_PARTYCUREDBURN]                       = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1}'s burn was cured!",
                                                                                   "{B_BUFF1}'s burn\nwas healed!")),
    [STRINGID_PARTYCUREDSLEEP]                      = COMPOUND_STRING("{B_BUFF1} woke up!"),
    [STRINGID_PARTYCUREDFREEZE]                     = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1} thawed out!",
                                                                                   "{B_BUFF1} was\ndefrosted!")),
    [STRINGID_PARTYCUREDFROSTBITE]                  = COMPOUND_STRING(VANILLA_TEXT("{B_BUFF1}'s frostbite was cured!",
                                                                                   "{B_BUFF1}'s frostbite\nwas healed!")),
    [STRINGID_PKMNATKNOTLOWERED]                    = COMPOUND_STRING(VANILLA_TEXT("{B_SCR_NAME_WITH_PREFIX}'s Attack was not lowered!",
                                                                                   "{B_SCR_NAME_WITH_PREFIX}'s ATTACK\nwas not lowered!")),
    [STRINGID_VICTORYCATCH]                         = COMPOUND_STRING("{B_DEF_NAME} is weak!\nThrow a POKé BALL now!"),
    [STRINGID_CANTUSEMOVE]                          = COMPOUND_STRING("This move can't be used!\p"),
    [STRINGID_REFLECTWOREOFF]                       = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_TEAM1} side's Reflect wore off!",
                                                                                   "{B_DEF_PREFIX1}'s REFLECT\nwore off!")),
    [STRINGID_LIGHTSCREENWOREOFF]                   = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_TEAM1} side's Light Screen wore off!",
                                                                                   "{B_DEF_PREFIX1}'s LIGHT SCREEN\nwore off!")),
    [STRINGID_AURORAVEILWOREOFF]                    = COMPOUND_STRING(VANILLA_TEXT("{B_DEF_TEAM1} side's Aurora Veil wore off!",
                                                                                   "{B_DEF_PREFIX1}'s AURORA VEIL\nwore off!")),
    [STRINGID_MOSTLYINEFFECTIVE]                    = COMPOUND_STRING("It's mostly ineffective…"),
    [STRINGID_EXTREMELYEFFECTIVE]                   = COMPOUND_STRING("It's extremely effective!"),
    [STRINGID_NOTVERYEFFECTIVEONDEF]                = COMPOUND_STRING(VANILLA_TEXT("It's not very effective on {B_DEF_NAME_WITH_PREFIX2}.",
                                                                                   "It's not very effective on\n{B_DEF_NAME_WITH_PREFIX}…")),
    [STRINGID_SUPEREFFECTIVEONDEF]                  = COMPOUND_STRING(VANILLA_TEXT("It's super effective on {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "It's super effective on\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_MOSTLYINEFFECTIVEONDEF]               = COMPOUND_STRING(VANILLA_TEXT("It's mostly ineffective on {B_DEF_NAME_WITH_PREFIX2}.",
                                                                                   "It's mostly ineffective on\n{B_DEF_NAME_WITH_PREFIX}…")),
    [STRINGID_EXTREMELYEFFECTIVEONDEF]              = COMPOUND_STRING(VANILLA_TEXT("It's extremely effective on {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "It's extremely effective on\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_EXTREMELYEFFECTIVETWOFOES]            = COMPOUND_STRING(VANILLA_TEXT("It's extremely effective on {B_DEF_NAME_WITH_PREFIX2} and {B_DEF_PARTNER_NAME}!",
                                                                                   "It's extremely effective on\n{B_DEF_NAME_WITH_PREFIX} and {B_DEF_PARTNER_NAME}!")),
    [STRINGID_MOSTLYINEFFECTIVETWOFOES]             = COMPOUND_STRING(VANILLA_TEXT("It's mostly ineffective on {B_DEF_NAME_WITH_PREFIX2} and {B_DEF_PARTNER_NAME}.",
                                                                                   "It's mostly ineffective on\n{B_DEF_NAME_WITH_PREFIX} and {B_DEF_PARTNER_NAME}…")),
    [STRINGID_CRITICALHITONDEF]                     = COMPOUND_STRING(VANILLA_TEXT("A critical hit on {B_DEF_NAME_WITH_PREFIX2}!",
                                                                                   "A critical hit on\n{B_DEF_NAME_WITH_PREFIX}!")),
    [STRINGID_S]                                    = COMPOUND_STRING("s"),
    [STRINGID_LOSTSOMEOFITSHP]                      = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} lost some of its HP!",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} lost some\nof its HP!")),
    [STRINGID_BELCHCANTUSE]                         = COMPOUND_STRING(VANILLA_TEXT("{B_ATK_NAME_WITH_PREFIX} hasn't eaten any held Berries, so it can't possibly belch!\p",
                                                                                   "{B_ATK_NAME_WITH_PREFIX} hasn't eaten a\nBERRY, so it can't BELCH!\p")),
};

const u16 gTrainerUsedItemStringIds[] =
{
    STRINGID_PLAYERUSEDITEM, STRINGID_TRAINER1USEDITEM
};

const u16 gZEffectStringIds[] =
{
    [B_MSG_Z_RESET_STATS] = STRINGID_ZMOVERESETSSTATS,
    [B_MSG_Z_ALL_STATS_UP]= STRINGID_ZMOVEALLSTATSUP,
    [B_MSG_Z_BOOST_CRITS] = STRINGID_ZMOVEZBOOSTCRIT,
    [B_MSG_Z_FOLLOW_ME]   = STRINGID_PKMNCENTERATTENTION,
    [B_MSG_Z_RECOVER_HP]  = STRINGID_ZMOVERESTOREHP,
    [B_MSG_Z_STAT_UP]     = STRINGID_ZMOVESTATUP,
    [B_MSG_Z_HP_TRAP]     = STRINGID_ZMOVEHPTRAP,
};

const u16 gMentalHerbCureStringIds[] =
{
    [B_MSG_MENTALHERBCURE_INFATUATION] = STRINGID_ATKGOTOVERINFATUATION,
    [B_MSG_MENTALHERBCURE_TORMENT]     = STRINGID_TORMENTEDNOMORE,
    [B_MSG_MENTALHERBCURE_DISABLE]     = STRINGID_PKMNMOVEDISABLEDNOMORE,
    [B_MSG_MENTALHERBCURE_HEALBLOCK]   = STRINGID_HEALBLOCKEDNOMORE,
    [B_MSG_MENTALHERBCURE_ENCORE]      = STRINGID_PKMNENCOREENDED,
    [B_MSG_MENTALHERBCURE_TAUNT]       = STRINGID_PKMNSHOOKOFFTHETAUNT,
};

const u16 gStartingStatusStringIds[B_MSG_STARTING_STATUS_COUNT] =
{
    [B_MSG_TERRAIN_SET_MISTY]    = STRINGID_TERRAINBECOMESMISTY,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_TERRAINBECOMESELECTRIC,
    [B_MSG_TERRAIN_SET_PSYCHIC]  = STRINGID_TERRAINBECOMESPSYCHIC,
    [B_MSG_TERRAIN_SET_GRASSY]   = STRINGID_TERRAINBECOMESGRASSY,
    [B_MSG_SET_TRICK_ROOM]       = STRINGID_DIMENSIONSWERETWISTED,
    [B_MSG_SET_MAGIC_ROOM]       = STRINGID_BIZARREARENACREATED,
    [B_MSG_SET_WONDER_ROOM]      = STRINGID_BIZARREAREACREATED,
    [B_MSG_SET_TAILWIND]         = STRINGID_TAILWINDBLEW,
    [B_MSG_SET_RAINBOW]          = STRINGID_ARAINBOWAPPEAREDONSIDE,
    [B_MSG_SET_SEA_OF_FIRE]      = STRINGID_SEAOFFIREENVELOPEDSIDE,
    [B_MSG_SET_SWAMP]            = STRINGID_SWAMPENVELOPEDSIDE,
    [B_MSG_SET_SPIKES]           = STRINGID_SPIKESSCATTERED,
    [B_MSG_SET_POISON_SPIKES]    = STRINGID_POISONSPIKESSCATTERED,
    [B_MSG_SET_STICKY_WEB]       = STRINGID_STICKYWEBUSED,
    [B_MSG_SET_STEALTH_ROCK]     = STRINGID_POINTEDSTONESFLOAT,
    [B_MSG_SET_SHARP_STEEL]      = STRINGID_SHARPSTEELFLOATS,
};

const u16 gTerrainStringIds[B_MSG_TERRAIN_COUNT] =
{
    [B_MSG_TERRAIN_SET_MISTY] = STRINGID_TERRAINBECOMESMISTY,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_TERRAINBECOMESELECTRIC,
    [B_MSG_TERRAIN_SET_PSYCHIC] = STRINGID_TERRAINBECOMESPSYCHIC,
    [B_MSG_TERRAIN_SET_GRASSY] = STRINGID_TERRAINBECOMESGRASSY,
    [B_MSG_TERRAIN_END_MISTY] = STRINGID_MISTYTERRAINENDS,
    [B_MSG_TERRAIN_END_ELECTRIC] = STRINGID_ELECTRICTERRAINENDS,
    [B_MSG_TERRAIN_END_PSYCHIC] = STRINGID_PSYCHICTERRAINENDS,
    [B_MSG_TERRAIN_END_GRASSY] = STRINGID_GRASSYTERRAINENDS,
};

const u16 gTerrainPreventsStringIds[] =
{
    [B_MSG_TERRAINPREVENTS_MISTY]    = STRINGID_MISTYTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_ELECTRIC] = STRINGID_ELECTRICTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_PSYCHIC]  = STRINGID_PSYCHICTERRAINPREVENTS
};

const u16 gHealingWishStringIds[] =
{
    STRINGID_HEALINGWISHCAMETRUE,
    STRINGID_LUNARDANCECAMETRUE
};

const u16 gDmgHazardsStringIds[] =
{
    [B_MSG_PKMNHURTBYSPIKES]   = STRINGID_PKMNHURTBYSPIKES,
    [B_MSG_STEALTHROCKDMG]     = STRINGID_STEALTHROCKDMG,
    [B_MSG_SHARPSTEELDMG]      = STRINGID_SHARPSTEELDMG,
    [B_MSG_POINTEDSTONESFLOAT] = STRINGID_POINTEDSTONESFLOAT,
    [B_MSG_SPIKESSCATTERED]    = STRINGID_SPIKESSCATTERED,
    [B_MSG_SHARPSTEELFLOATS]   = STRINGID_SHARPSTEELFLOATS,
};

const u16 gSwitchInAbilityStringIds[] =
{
    [B_MSG_SWITCHIN_MOLDBREAKER] = STRINGID_MOLDBREAKERENTERS,
    [B_MSG_SWITCHIN_TERAVOLT] = STRINGID_TERAVOLTENTERS,
    [B_MSG_SWITCHIN_TURBOBLAZE] = STRINGID_TURBOBLAZEENTERS,
    [B_MSG_SWITCHIN_SLOWSTART] = STRINGID_SLOWSTARTENTERS,
    [B_MSG_SWITCHIN_UNNERVE] = STRINGID_UNNERVEENTERS,
    [B_MSG_SWITCHIN_ANTICIPATION] = STRINGID_ANTICIPATIONACTIVATES,
    [B_MSG_SWITCHIN_FOREWARN] = STRINGID_FOREWARNACTIVATES,
    [B_MSG_SWITCHIN_PRESSURE] = STRINGID_PRESSUREENTERS,
    [B_MSG_SWITCHIN_DARKAURA] = STRINGID_DARKAURAENTERS,
    [B_MSG_SWITCHIN_FAIRYAURA] = STRINGID_FAIRYAURAENTERS,
    [B_MSG_SWITCHIN_AURABREAK] = STRINGID_AURABREAKENTERS,
    [B_MSG_SWITCHIN_COMATOSE] = STRINGID_COMATOSEENTERS,
    [B_MSG_SWITCHIN_SCREENCLEANER] = STRINGID_SCREENCLEANERENTERS,
    [B_MSG_SWITCHIN_ASONE] = STRINGID_ASONEENTERS,
    [B_MSG_SWITCHIN_CURIOUS_MEDICINE] = STRINGID_CURIOUSMEDICINEENTERS,
    [B_MSG_SWITCHIN_PASTEL_VEIL] = STRINGID_PKMNHEALEDPOISON,
    [B_MSG_SWITCHIN_NEUTRALIZING_GAS] = STRINGID_NEUTRALIZINGGASENTERS,
};

const u16 gNoEscapeStringIds[] =
{
    [B_MSG_CANT_ESCAPE]          = STRINGID_CANTESCAPE,
    [B_MSG_DONT_LEAVE_BIRCH]     = STRINGID_DONTLEAVEBIRCH,
    [B_MSG_PREVENTS_ESCAPE]      = STRINGID_PREVENTSESCAPE,
    [B_MSG_CANT_ESCAPE_2]        = STRINGID_CANTESCAPE2,
    [B_MSG_ATTACKER_CANT_ESCAPE] = STRINGID_ATTACKERCANTESCAPE
};

const u16 gMoveWeatherChangeStringIds[] =
{
    [B_MSG_STARTED_RAIN]      = STRINGID_STARTEDTORAIN,
    [B_MSG_STARTED_DOWNPOUR]  = STRINGID_DOWNPOURSTARTED, // Unused
    [B_MSG_WEATHER_FAILED]    = STRINGID_BUTITFAILED,
    [B_MSG_STARTED_SANDSTORM] = STRINGID_SANDSTORMBREWED,
    [B_MSG_STARTED_SUNLIGHT]  = STRINGID_SUNLIGHTGOTBRIGHT,
    [B_MSG_STARTED_HAIL]      = STRINGID_STARTEDHAIL,
    [B_MSG_STARTED_SNOW]      = STRINGID_STARTEDSNOW,
    [B_MSG_STARTED_FOG]       = STRINGID_FOGCREPTUP, // Unused, can use for custom moves that set fog
};

const u16 gAbilityWeatherChangeStringId[] =
{
    [B_MSG_STARTED_DRIZZLE]        = STRINGID_STARTEDTORAIN,
    [B_MSG_STARTED_SAND_STREAM]    = STRINGID_SANDSTORMBREWED,
    [B_MSG_STARTED_DROUGHT]        = STRINGID_SUNLIGHTGOTBRIGHT,
    [B_MSG_STARTED_HAIL_WARNING]   = STRINGID_STARTEDHAIL,
    [B_MSG_STARTED_SNOW_WARNING]   = STRINGID_STARTEDSNOW,
    [B_MSG_STARTED_DESOLATE_LAND]  = STRINGID_EXTREMELYHARSHSUNLIGHT,
    [B_MSG_STARTED_PRIMORDIAL_SEA] = STRINGID_HEAVYRAIN,
    [B_MSG_STARTED_STRONG_WINDS]   = STRINGID_MYSTERIOUSAIRCURRENT,
};

const u16 gWeatherEndsStringIds[B_MSG_WEATHER_END_COUNT] =
{
    [B_MSG_WEATHER_END_RAIN]                       = STRINGID_RAINSTOPPED,
    [B_MSG_WEATHER_END_SUN]                        = STRINGID_SUNLIGHTFADED,
    [B_MSG_WEATHER_END_SANDSTORM]                  = STRINGID_SANDSTORMSUBSIDED,
    [B_MSG_WEATHER_END_HAIL]                       = STRINGID_HAILSTOPPED,
    [B_MSG_WEATHER_END_SNOW]                       = STRINGID_SNOWSTOPPED,
    [B_MSG_WEATHER_END_FOG]                        = STRINGID_FOGLIFTED,
    [B_MSG_WEATHER_END_EXTREMELY_HARSH_SUNLIGHT]   = STRINGID_EXTREMESUNLIGHTFADED,
    [B_MSG_WEATHER_END_HEAVY_RAIN]                 = STRINGID_HEAVYRAINLIFTED,
    [B_MSG_WEATHER_END_STRONG_WINDS]               = STRINGID_STRONGWINDSDISSIPATED,
};

const u16 gWeatherTurnStringIds[] =
{
    [B_MSG_WEATHER_TURN_RAIN]         = STRINGID_RAINCONTINUES,
    [B_MSG_WEATHER_TURN_DOWNPOUR]     = STRINGID_DOWNPOURCONTINUES,
    [B_MSG_WEATHER_TURN_SUN]          = STRINGID_SUNLIGHTSTRONG,
    [B_MSG_WEATHER_TURN_SANDSTORM]    = STRINGID_SANDSTORMRAGES,
    [B_MSG_WEATHER_TURN_HAIL]         = STRINGID_HAILCONTINUES,
    [B_MSG_WEATHER_TURN_SNOW]         = STRINGID_SNOWCONTINUES,
    [B_MSG_WEATHER_TURN_FOG]          = STRINGID_FOGISDEEP,
    [B_MSG_WEATHER_TURN_STRONG_WINDS] = STRINGID_MYSTERIOUSAIRCURRENTBLOWSON,
};

const u16 gSandStormHailDmgStringIds[] =
{
    [B_MSG_SANDSTORM] = STRINGID_PKMNBUFFETEDBYSANDSTORM,
    [B_MSG_HAIL]      = STRINGID_PKMNPELTEDBYHAIL
};

const u16 gProtectLikeUsedStringIds[] =
{
    [B_MSG_PROTECTED_ITSELF] = STRINGID_PKMNPROTECTEDITSELF2,
    [B_MSG_BRACED_ITSELF]    = STRINGID_PKMNBRACEDITSELF,
    [B_MSG_PROTECTED_TEAM]   = STRINGID_PROTECTEDTEAM,
};

const u16 gBrokeProtectionStringIds[] =
{
    [B_MSG_FEINT]                 = STRINGID_FELLFORFEINT,
    [B_MSG_BROKE_THROUGH_PROTECT] = STRINGID_BROKETHROUGHPROTECTION,
};

const u16 gReflectLightScreenSafeguardStringIds[] =
{
    [B_MSG_SIDE_STATUS_FAILED]     = STRINGID_BUTITFAILED,
    [B_MSG_SET_REFLECT_SINGLE]     = STRINGID_PKMNRAISEDDEF,
    [B_MSG_SET_REFLECT_DOUBLE]     = STRINGID_PKMNRAISEDDEF,
    [B_MSG_SET_LIGHTSCREEN_SINGLE] = STRINGID_PKMNRAISEDSPDEF,
    [B_MSG_SET_LIGHTSCREEN_DOUBLE] = STRINGID_PKMNRAISEDSPDEF,
    [B_MSG_SET_SAFEGUARD]          = STRINGID_PKMNCOVEREDBYVEIL,
    [B_MSG_SET_AURORA_VEIL]        = STRINGID_PKMNAURORAVEIL,
};

const u16 gLeechSeedStringIds[] =
{
    [B_MSG_LEECH_SEED_SET]   = STRINGID_PKMNSEEDED,
    [B_MSG_LEECH_SEED_MISS]  = STRINGID_PKMNAVOIDEDATTACK,
    [B_MSG_LEECH_SEED_FAIL]  = STRINGID_ITDOESNTAFFECT,
    [B_MSG_LEECH_SEED_DRAIN] = STRINGID_PKMNSAPPEDBYLEECHSEED,
    [B_MSG_LEECH_SEED_OOZE]  = STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gRestUsedStringIds[] =
{
    [B_MSG_REST]          = STRINGID_PKMNWENTTOSLEEP,
    [B_MSG_REST_STATUSED] = STRINGID_PKMNSLEPTHEALTHY
};

const u16 gUproarOverTurnStringIds[] =
{
    [B_MSG_UPROAR_CONTINUES] = STRINGID_PKMNMAKINGUPROAR,
    [B_MSG_UPROAR_ENDS]      = STRINGID_PKMNCALMEDDOWN
};

const u16 gWokeUpStringIds[] =
{
    [B_MSG_WOKE_UP]        = STRINGID_PKMNWOKEUP,
    [B_MSG_WOKE_UP_UPROAR] = STRINGID_PKMNWOKEUPINUPROAR
};

const u16 gUproarAwakeStringIds[] =
{
    [B_MSG_CANT_SLEEP_UPROAR]  = STRINGID_PKMNCANTSLEEPINUPROAR2,
    [B_MSG_UPROAR_KEPT_AWAKE]  = STRINGID_UPROARKEPTPKMNAWAKE,
};

const u16 gStatUpStringIds[] =
{
    [B_MSG_STAT_CHANGED]            = STRINGID_STATROSE,
    [B_MSG_STAT_WONT_CHANGE]        = STRINGID_STATSWONTINCREASE,
    [B_MSG_STAT_MAXED]              = STRINGID_STATWASMAXEDOUT,
    [B_MSG_STAT_CHANGE_EMPTY]       = STRINGID_EMPTYSTRING3,
    [B_MSG_STAT_CHANGED_ITEM]       = STRINGID_USINGITEMSTATOFPKMNROSE,
    [B_MSG_STAT_CHANGED_BELLY_DRUM] = STRINGID_PKMNCUTHPMAXEDATTACK,
    [B_MSG_USED_DIRE_HIT]           = STRINGID_PKMNUSEDXTOGETPUMPED,
};

// Mostly redundant, combine with above in a future pr
const u16 gStatDownStringIds[] =
{
    [B_MSG_STAT_CHANGED]            = STRINGID_STATFELL,
    [B_MSG_STAT_WONT_CHANGE]        = STRINGID_STATSWONTDECREASE,
    [B_MSG_STAT_CHANGE_EMPTY]       = STRINGID_EMPTYSTRING3,
    [B_MSG_STAT_CHANGED_ITEM]       = STRINGID_USINGITEMSTATOFPKMNFELL,
    [B_MSG_STAT_CHANGED_BELLY_DRUM] = STRINGID_PKMNCUTHPMAXEDATTACK, // Message for contrary is still printed
};

// Index copied from move's index in sTrappingMoves
const u16 gWrappedStringIds[NUM_TRAPPING_MOVES] =
{
    [B_MSG_WRAPPED_BIND]        = STRINGID_PKMNSQUEEZEDBYBIND,     // MOVE_BIND
    [B_MSG_WRAPPED_WRAP]        = STRINGID_PKMNWRAPPEDBY,          // MOVE_WRAP
    [B_MSG_WRAPPED_FIRE_SPIN]   = STRINGID_PKMNTRAPPEDINVORTEX,    // MOVE_FIRE_SPIN
    [B_MSG_WRAPPED_CLAMP]       = STRINGID_PKMNCLAMPED,            // MOVE_CLAMP
    [B_MSG_WRAPPED_WHIRLPOOL]   = STRINGID_PKMNTRAPPEDINVORTEX,    // MOVE_WHIRLPOOL
    [B_MSG_WRAPPED_SAND_TOMB]   = STRINGID_PKMNTRAPPEDBYSANDTOMB,  // MOVE_SAND_TOMB
    [B_MSG_WRAPPED_MAGMA_STORM] = STRINGID_TRAPPEDBYSWIRLINGMAGMA, // MOVE_MAGMA_STORM
    [B_MSG_WRAPPED_INFESTATION] = STRINGID_INFESTATION,            // MOVE_INFESTATION
    [B_MSG_WRAPPED_SNAP_TRAP]   = STRINGID_PKMNINSNAPTRAP,         // MOVE_SNAP_TRAP
    [B_MSG_WRAPPED_THUNDER_CAGE]= STRINGID_THUNDERCAGETRAPPED,     // MOVE_THUNDER_CAGE
};

const u16 gMistUsedStringIds[] =
{
    [B_MSG_SET_MIST]    = STRINGID_PKMNSHROUDEDINMIST,
    [B_MSG_MIST_FAILED] = STRINGID_BUTITFAILED
};

const u16 gFocusEnergyUsedStringIds[] =
{
    [B_MSG_GETTING_PUMPED]      = STRINGID_PKMNGETTINGPUMPED,
    [B_MSG_FOCUS_ENERGY_FAILED] = STRINGID_BUTITFAILED
};

const u16 gTransformUsedStringIds[] =
{
    [B_MSG_TRANSFORMED]      = STRINGID_PKMNTRANSFORMEDINTO,
    [B_MSG_TRANSFORM_FAILED] = STRINGID_BUTITFAILED
};

const u16 gSubstituteUsedStringIds[] =
{
    [B_MSG_SET_SUBSTITUTE]    = STRINGID_PKMNMADESUBSTITUTE,
    [B_MSG_SUBSTITUTE_FAILED] = STRINGID_TOOWEAKFORSUBSTITUTE
};

const u16 gGotPoisonedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASPOISONED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNPOISONEDBY
};

const u16 gGotParalyzedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASPARALYZED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNWASPARALYZEDBY
};

const u16 gFellAsleepStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNFELLASLEEP,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNMADESLEEP,
};

const u16 gGotBurnedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASBURNED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNBURNEDBY
};

const u16 gGotFrostbiteStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNGOTFROSTBITE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNGOTFROSTBITE,
};

const u16 gFrostbiteHealedStringIds[] =
{
    [B_MSG_FROSTBITE_HEALED]         = STRINGID_PKMNFROSTBITEHEALED,
    [B_MSG_FROSTBITE_HEALED_BY_MOVE] = STRINGID_PKMNFROSTBITEHEALEDBY
};

const u16 gGotFrozenStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASFROZEN,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNFROZENBY
};

const u16 gGotDefrostedStringIds[] =
{
    [B_MSG_DEFROSTED]         = STRINGID_PKMNWASDEFROSTED,
    [B_MSG_DEFROSTED_BY_MOVE] = STRINGID_PKMNWASDEFROSTEDBY
};

const u16 gKOFailedStringIds[] =
{
    [B_MSG_KO_MISS]       = STRINGID_PKMNAVOIDEDATTACK,
    [B_MSG_KO_UNAFFECTED] = STRINGID_PKMNUNAFFECTED
};

const u16 gAttractUsedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNFELLINLOVE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNSXINFATUATEDY
};

const u16 gAbsorbDrainStringIds[] =
{
    [B_MSG_ABSORB]      = STRINGID_PKMNENERGYDRAINED,
    [B_MSG_ABSORB_OOZE] = STRINGID_ITSUCKEDLIQUIDOOZE
};

const u16 gSportsUsedStringIds[] =
{
    [B_MSG_WEAKEN_ELECTRIC] = STRINGID_ELECTRICITYWEAKENED,
    [B_MSG_WEAKEN_FIRE]     = STRINGID_FIREWEAKENED
};

const u16 gPartyStatusHealStringIds[] =
{
    [B_MSG_BELL]                     = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_ATTACKER] = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_PARTNER]  = STRINGID_BELLCHIMED,
    [B_MSG_BELL_BOTH_SOUNDPROOF]     = STRINGID_BELLCHIMED,
    [B_MSG_SOOTHING_AROMA]           = STRINGID_SOOTHINGAROMA
};

const u16 gFutureMoveUsedStringIds[] =
{
    [B_MSG_FUTURE_SIGHT] = STRINGID_PKMNFORESAWATTACK,
    [B_MSG_DOOM_DESIRE]  = STRINGID_PKMNCHOSEXASDESTINY
};

const u16 gBallEscapeStringIds[] =
{
    [BALL_NO_SHAKES]     = STRINGID_PKMNBROKEFREE,
    [BALL_1_SHAKE]       = STRINGID_ITAPPEAREDCAUGHT,
    [BALL_2_SHAKES]      = STRINGID_AARGHALMOSTHADIT,
    [BALL_3_SHAKES_FAIL] = STRINGID_SHOOTSOCLOSE
};

// Overworld weathers that don't have an associated battle weather default to "It is raining."
const u16 gWeatherStartsStringIds[] =
{
    [WEATHER_NONE]               = STRINGID_ITISRAINING,
    [WEATHER_SUNNY_CLOUDS]       = STRINGID_ITISRAINING,
    [WEATHER_SUNNY]              = STRINGID_ITISRAINING,
    [WEATHER_RAIN]               = STRINGID_ITISRAINING,
    [WEATHER_SNOW]               = (B_OVERWORLD_SNOW >= GEN_9 ? STRINGID_ITISSNOWING : STRINGID_ITISHAILING),
    [WEATHER_RAIN_THUNDERSTORM]  = STRINGID_ITISRAINING,
    [WEATHER_FOG_HORIZONTAL]     = STRINGID_FOGISDEEP,
    [WEATHER_VOLCANIC_ASH]       = STRINGID_ITISRAINING,
    [WEATHER_SANDSTORM]          = STRINGID_SANDSTORMISRAGING,
    [WEATHER_FOG_DIAGONAL]       = STRINGID_FOGISDEEP,
    [WEATHER_UNDERWATER]         = STRINGID_ITISRAINING,
    [WEATHER_SHADE]              = STRINGID_ITISRAINING,
    [WEATHER_DROUGHT]            = STRINGID_SUNLIGHTISHARSH,
    [WEATHER_DOWNPOUR]           = STRINGID_ITISRAINING,
    [WEATHER_UNDERWATER_BUBBLES] = STRINGID_ITISRAINING,
    [WEATHER_ABNORMAL]           = STRINGID_ITISRAINING
};

const u16 gTerrainStartsStringIds[] =
{
    [B_MSG_TERRAIN_SET_MISTY]    = STRINGID_MISTSWIRLSAROUND,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_ELECTRICCURRENTISRUNNING,
    [B_MSG_TERRAIN_SET_PSYCHIC]  = STRINGID_SEEMSWEIRD,
    [B_MSG_TERRAIN_SET_GRASSY]   = STRINGID_ISCOVEREDWITHGRASS,
};

const u16 gPrimalWeatherBlocksStringIds[] =
{
    [B_MSG_PRIMAL_WEATHER_FIZZLED_BY_RAIN]      = STRINGID_MOVEFIZZLEDOUTINTHEHEAVYRAIN,
    [B_MSG_PRIMAL_WEATHER_EVAPORATED_IN_SUN]    = STRINGID_MOVEEVAPORATEDINTHEHARSHSUNLIGHT,
};

const u16 gInobedientStringIds[] =
{
    [B_MSG_LOAFING]            = STRINGID_PKMNLOAFING,
    [B_MSG_WONT_OBEY]          = STRINGID_PKMNWONTOBEY,
    [B_MSG_TURNED_AWAY]        = STRINGID_PKMNTURNEDAWAY,
    [B_MSG_PRETEND_NOT_NOTICE] = STRINGID_PKMNPRETENDNOTNOTICE,
    [B_MSG_INCAPABLE_OF_POWER] = STRINGID_PKMNINCAPABLEOFPOWER
};

const u16 gSafariReactionStringIds[NUM_SAFARI_REACTIONS] =
{
    [B_MSG_MON_WATCHING] = STRINGID_PKMNWATCHINGCAREFULLY,
    [B_MSG_MON_ANGRY]    = STRINGID_PKMNANGRY,
    [B_MSG_MON_EATING]   = STRINGID_PKMNEATING
};

const u16 gSafariGetNearStringIds[] =
{
    [B_MSG_CREPT_CLOSER]    = STRINGID_CREPTCLOSER,
    [B_MSG_CANT_GET_CLOSER] = STRINGID_CANTGETCLOSER
};

const u16 gSafariPokeblockResultStringIds[] =
{
    [B_MSG_MON_CURIOUS]    = STRINGID_PKMNCURIOUSABOUTX,
    [B_MSG_MON_ENTHRALLED] = STRINGID_PKMNENTHRALLEDBYX,
    [B_MSG_MON_IGNORED]    = STRINGID_PKMNIGNOREDX
};

const u16 CureStatusBerryEffectStringID[] =
{
    [B_MSG_CURED_PARALYSIS] = STRINGID_PKMNSITEMCUREDPARALYSIS,
    [B_MSG_CURED_POISON] = STRINGID_PKMNSITEMCUREDPOISON,
    [B_MSG_CURED_BURN] = STRINGID_PKMNSITEMHEALEDBURN,
    [B_MSG_CURED_FREEZE] = STRINGID_PKMNSITEMDEFROSTEDIT,
    [B_MSG_CURED_FROSTBITE] = STRINGID_PKMNSITEMHEALEDFROSTBITE,
    [B_MSG_CURED_SLEEP] = STRINGID_PKMNSITEMWOKEIT,
    [B_MSG_CURED_CONFUSION] = STRINGID_PKMNSITEMSNAPPEDOUT,
};

const u16 gItemSwapStringIds[] =
{
    [B_MSG_ITEM_SWAP_TAKEN] = STRINGID_PKMNOBTAINEDX,
    [B_MSG_ITEM_SWAP_GIVEN] = STRINGID_PKMNOBTAINEDX2,
    [B_MSG_ITEM_SWAP_BOTH]  = STRINGID_PKMNOBTAINEDXYOBTAINEDZ
};

const u16 gFlashFireStringIds[] =
{
    [B_MSG_FLASH_FIRE_BOOST]    = STRINGID_PKMNRAISEDFIREPOWERWITH,
    [B_MSG_FLASH_FIRE_NO_BOOST] = STRINGID_PKMNSXMADEITINEFFECTIVE
};

const u16 gCaughtMonStringIds[] =
{
    [B_MSG_SENT_SOMEONES_PC]   = STRINGID_PKMNTRANSFERREDSOMEONESPC,
    [B_MSG_SENT_LANETTES_PC]   = STRINGID_PKMNTRANSFERREDLANETTESPC,
    [B_MSG_SOMEONES_BOX_FULL]  = STRINGID_PKMNBOXSOMEONESPCFULL,
    [B_MSG_LANETTES_BOX_FULL]  = STRINGID_PKMNBOXLANETTESPCFULL,
    [B_MSG_SWAPPED_INTO_PARTY] = STRINGID_PKMNSENTTOPCAFTERCATCH,
};

const u16 gRoomsStringIds[] =
{
    STRINGID_PKMNTWISTEDDIMENSIONS, STRINGID_TRICKROOMENDS,
    STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON, STRINGID_WONDERROOMENDS,
    STRINGID_HELDITEMSLOSEEFFECTS, STRINGID_MAGICROOMENDS,
    STRINGID_EMPTYSTRING3
};

const u16 gStatusConditionsStringIds[] =
{
    STRINGID_PKMNWASPOISONED, STRINGID_PKMNBADLYPOISONED, STRINGID_PKMNWASBURNED, STRINGID_PKMNWASPARALYZED, STRINGID_PKMNFELLASLEEP, STRINGID_PKMNGOTFROSTBITE
};

const u16 gDamageNonTypesStartStringIds[] =
{
    [B_MSG_TRAPPED_WITH_VINES]  = STRINGID_TEAMTRAPPEDWITHVINES,
    [B_MSG_CAUGHT_IN_VORTEX]    = STRINGID_TEAMCAUGHTINVORTEX,
    [B_MSG_SURROUNDED_BY_FIRE]  = STRINGID_TEAMSURROUNDEDBYFIRE,
    [B_MSG_SURROUNDED_BY_ROCKS] = STRINGID_TEAMSURROUNDEDBYROCKS,
};

const u16 gDamageNonTypesDmgStringIds[] =
{
    [B_MSG_HURT_BY_VINES]        = STRINGID_PKMNHURTBYVINES,
    [B_MSG_HURT_BY_VORTEX]       = STRINGID_PKMNHURTBYVORTEX,
    [B_MSG_BURNING_UP]           = STRINGID_PKMNBURNINGUP,
    [B_MSG_HURT_BY_ROCKS_THROWN] = STRINGID_PKMNHURTBYROCKSTHROWN,
};

const u16 gRemoveHazardsStringIds[] =
{
    [HAZARDS_SPIKES] = STRINGID_SPIKESDISAPPEAREDFROMTEAM,
    [HAZARDS_STICKY_WEB] = STRINGID_STICKYWEBDISAPPEAREDFROMTEAM,
    [HAZARDS_TOXIC_SPIKES] = STRINGID_TOXICSPIKESDISAPPEAREDFROMTEAM,
    [HAZARDS_STEALTH_ROCK] = STRINGID_STEALTHROCKDISAPPEAREDFROMTEAM,
    [HAZARDS_STEELSURGE] = STRINGID_SHARPSTEELDISAPPEAREDFROMTEAM,
};

const u16 gZenModeStringIds[] =
{
    [B_MSG_ZEN_MODE_TRIGGERED] = STRINGID_ZENMODETRIGGERED,
    [B_MSG_ZEN_MODE_ENDED] = STRINGID_ZENMODEENDED
};

const u16 gCureStatusStringIds[] =
{
    [B_MSG_CURED_PARALYSIS] = STRINGID_SCRCUREDPARALYSIS,
    [B_MSG_CURED_POISON] = STRINGID_SCRCUREDPOISON,
    [B_MSG_CURED_BURN] = STRINGID_SCRCUREDBURN,
    [B_MSG_CURED_SLEEP] = STRINGID_SCRCUREDSLEEP,
    [B_MSG_CURED_FREEZE] = STRINGID_PKMNWASDEFROSTED,
    [B_MSG_CURED_FROSTBITE] = STRINGID_PKMNFROSTBITEHEALED,
    [B_MSG_CURED_CONFUSION] = STRINGID_SCRCUREDCONFUSION,
    [B_MSG_CURED_INFATUATION] = STRINGID_PKMNGOTOVERITSINFATUATION,
    [B_MSG_CURED_TAUNT] = STRINGID_PKMNSHOOKOFFTHETAUNT,
};

const u16 gPartyCureStatusStringIds[] =
{
    [B_MSG_CURED_PARALYSIS] = STRINGID_PARTYCUREDPARALYSIS,
    [B_MSG_CURED_POISON] = STRINGID_PARTYCUREDPOISON,
    [B_MSG_CURED_BURN] = STRINGID_PARTYCUREDBURN,
    [B_MSG_CURED_SLEEP] = STRINGID_PARTYCUREDSLEEP,
    [B_MSG_CURED_FREEZE] = STRINGID_PARTYCUREDFREEZE,
    [B_MSG_CURED_FROSTBITE] = STRINGID_PARTYCUREDFROSTBITE,
    [B_MSG_CURED_CONFUSION] = STRINGID_SCRCUREDCONFUSION,
    [B_MSG_CURED_INFATUATION] = STRINGID_PKMNGOTOVERITSINFATUATION,
    [B_MSG_CURED_TAUNT] = STRINGID_PKMNSHOOKOFFTHETAUNT,
};

const u16 gHurtByStringIds[] =
{
    [B_MSG_HURT] = STRINGID_PKMNWASHURT,
    [B_MSG_HURT_BY_ITEM] = STRINGID_PKMNHURTSWITH,
};

const u16 gBreakScreensStringIds[] =
{
    [B_MSG_BREAK_REFLECT] = STRINGID_REFLECTWOREOFF,
    [B_MSG_BREAK_LIGHT_SCREEN] = STRINGID_LIGHTSCREENWOREOFF,
    [B_MSG_BREAK_AURORA_VEIL] = STRINGID_AURORAVEILWOREOFF,
};

const u8 gText_PkmnIsEvolving[] = _("What?\n{STR_VAR_1} is evolving!");
const u8 gText_CongratsPkmnEvolved[] = _("Congratulations! Your {STR_VAR_1}\nevolved into {STR_VAR_2}!{WAIT_SE}\p");
const u8 gText_PkmnStoppedEvolving[] = _("Huh? {STR_VAR_1}\nstopped evolving!\p");
const u8 gText_EllipsisQuestionMark[] = _("……?\p");
const u8 gText_WhatWillPkmnDo[] = _("What will\n{B_BUFF1} do?");
const u8 gText_WhatWillPkmnDo2[] = _("What will\n{B_PLAYER_NAME} do?");
const u8 gText_WhatWillWallyDo[] = _("What will\nWALLY do?");
const u8 gText_LinkStandby[] = _("{PAUSE 16}Link standby…");
const u8 gText_BattleMenu[] = _("FIGHT{CLEAR_TO 56}BAG\nPOKéMON{CLEAR_TO 56}RUN");
const u8 gText_SafariZoneMenu[] = _("BALL{CLEAR_TO 56}{POKEBLOCK}\nGO NEAR{CLEAR_TO 56}RUN");
const u8 gText_SafariZoneMenuFrlg[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW 13 14 15}BALL{CLEAR_TO 56}BAIT\nROCK{CLEAR_TO 56}RUN");
const u8 gText_MoveInterfacePP[] = _("PP ");
const u8 gText_MoveInterfaceType[] = _("TYPE/");
const u8 gText_MoveInterfacePPType[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}PP\nTYPE/");
const u8 gText_MoveInterfaceDynamicColors[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}");
const u8 gText_WhichMoveToForget4[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}Which move should\nbe forgotten?");
const u8 gText_BattleCatchOrNot[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}Catch\nDon't catch");
const u8 gText_BattleYesNoChoice[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}Yes\nNo");
const u8 gText_BattleSwitchWhich[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}Switch\nwhich?");
const u8 gText_BattleSwitchWhich2[] = _("{PALETTE 5}{BACKGROUND DYNAMIC_COLOR5}{TEXT_COLORS DYNAMIC_COLOR4 DYNAMIC_COLOR6 DYNAMIC_COLOR5}");
const u8 gText_BattleSwitchWhich3[] = _("{UP_ARROW}");
const u8 gText_BattleSwitchWhich4[] = _("{ESCAPE 4}");
const u8 gText_BattleSwitchWhich5[] = _("-");
const u8 gText_SafariBalls[] = _(VANILLA_TEXT("Safari Balls", "SAFARI BALLS"));
const u8 gText_SafariBallLeft[] = _("Left: $");
const u8 gText_Sleep[] = _("sleep");
const u8 gText_Poison[] = _("poison");
const u8 gText_Burn[] = _("burn");
const u8 gText_Paralysis[] = _("paralysis");
const u8 gText_Ice[] = _("ice");
const u8 gText_Confusion[] = _("confusion");
const u8 gText_Love[] = _("love");
const u8 gText_SpaceAndSpace[] = _(" and ");
const u8 gText_CommaSpace[] = _(", ");
const u8 gText_Space2[] = _(" ");
const u8 gText_LineBreak[] = _("\l");
const u8 gText_NewLine[] = _("\n");
const u8 gText_Are[] = _("are");
const u8 gText_Are2[] = _("are");
const u8 gText_BadEgg[] = _(VANILLA_TEXT("Bad Egg", "Bad EGG"));
const u8 gText_BattleWallyName[] = _("WALLY");
const u8 gText_Win[] = _("{BACKGROUND TRANSPARENT}{ACCENT TRANSPARENT}Win");
const u8 gText_Loss[] = _("{BACKGROUND TRANSPARENT}{ACCENT TRANSPARENT}Loss");
const u8 gText_Draw[] = _("{BACKGROUND TRANSPARENT}{ACCENT TRANSPARENT}Draw");
static const u8 sText_SpaceIs[] = _(" is");
static const u8 sText_ApostropheS[] = _("'s");
const u8 gText_BattleTourney[] = _("BATTLE TOURNEY");

const u8 *const gRoundsStringTable[DOME_ROUNDS_COUNT] =
{
    [DOME_ROUND1]    = COMPOUND_STRING("Round 1"),
    [DOME_ROUND2]    = COMPOUND_STRING("Round 2"),
    [DOME_SEMIFINAL] = COMPOUND_STRING("Semifinal"),
    [DOME_FINAL]     = COMPOUND_STRING("Final"),
};

const u8 gText_TheGreatNewHope[] = _("The great new hope!\p");
const u8 gText_WillChampionshipDreamComeTrue[] = _("Will the championship dream come true?!\p");
const u8 gText_AFormerChampion[] = _(VANILLA_TEXT("A former champion!\p", "A former CHAMPION!\p"));
const u8 gText_ThePreviousChampion[] = _(VANILLA_TEXT("The previous champion!\p", "The previous CHAMPION!\p"));
const u8 gText_TheUnbeatenChampion[] = _(VANILLA_TEXT("The unbeaten champion!\p", "The unbeaten CHAMPION!\p"));
const u8 gText_PlayerMon1Name[] = _("{B_PLAYER_MON1_NAME}");
const u8 gText_Vs[] = _("VS");
const u8 gText_OpponentMon1Name[] = _("{B_OPPONENT_MON1_NAME}");
const u8 gText_Mind[] = _("Mind");
const u8 gText_Skill[] = _("Skill");
const u8 gText_Body[] = _("Body");
const u8 gText_Judgment[] = _("{B_BUFF1}{CLEAR 13}Judgment{CLEAR 13}{B_BUFF2}");
static const u8 sText_TwoTrainersSentPkmn[] = _(VANILLA_TEXT("{B_TRAINER1_NAME_WITH_CLASS} sent out {B_OPPONENT_MON1_NAME}!\p{B_TRAINER2_NAME_WITH_CLASS} sent out {B_OPPONENT_MON2_NAME}!",
                                                             "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} sent\nout {B_OPPONENT_MON1_NAME}!\p{B_TRAINER2_CLASS} {B_TRAINER2_NAME} sent\nout {B_OPPONENT_MON2_NAME}!"));
static const u8 sText_Trainer2SentOutPkmn[] = _(VANILLA_TEXT("{B_TRAINER2_NAME_WITH_CLASS} sent out {B_BUFF1}!",
                                                             "{B_TRAINER2_CLASS} {B_TRAINER2_NAME} sent\nout {B_BUFF1}!"));
static const u8 sText_TwoTrainersWantToBattle[] = _(VANILLA_TEXT("You are challenged by {B_TRAINER1_NAME_WITH_CLASS} and {B_TRAINER2_NAME_WITH_CLASS}!\p",
                                                                 "{B_TRAINER1_CLASS} {B_TRAINER1_NAME} and\n{B_TRAINER2_CLASS} {B_TRAINER2_NAME}\lwant to battle!\p"));
static const u8 sText_InGamePartnerSentOutZGoN[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} sent out {B_PLAYER_MON2_NAME}! Go, {B_PLAYER_MON1_NAME}!",
                                                                  "{B_PARTNER_CLASS} {B_PARTNER_NAME} sent\nout {B_PLAYER_MON2_NAME}!\lGo, {B_PLAYER_MON1_NAME}!"));
static const u8 sText_InGamePartnerSentOutNGoZ[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} sent out {B_PLAYER_MON1_NAME}! Go, {B_PLAYER_MON2_NAME}!",
                                                                  "{B_PARTNER_CLASS} {B_PARTNER_NAME} sent\nout {B_PLAYER_MON1_NAME}!\lGo, {B_PLAYER_MON2_NAME}!"));
static const u8 sText_InGamePartnerSentOutPkmn1[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} sent out {B_PLAYER_MON1_NAME}!",
                                                                   "{B_PARTNER_CLASS} {B_PARTNER_NAME} sent\nout {B_PLAYER_MON1_NAME}!"));
static const u8 sText_InGamePartnerSentOutPkmn2[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} sent out {B_PLAYER_MON2_NAME}!",
                                                                   "{B_PARTNER_CLASS} {B_PARTNER_NAME} sent\nout {B_PLAYER_MON2_NAME}!"));
static const u8 sText_InGamePartnerWithdrewPkmn1[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} withdrew {B_PLAYER_MON1_NAME}!",
                                                                    "{B_PARTNER_CLASS} {B_PARTNER_NAME}\nwithdrew {B_PLAYER_MON1_NAME}!"));
static const u8 sText_InGamePartnerWithdrewPkmn2[] = _(VANILLA_TEXT("{B_PARTNER_NAME_WITH_CLASS} withdrew {B_PLAYER_MON2_NAME}!",
                                                                    "{B_PARTNER_CLASS} {B_PARTNER_NAME}\nwithdrew {B_PLAYER_MON2_NAME}!"));

const u16 gBattlePalaceFlavorTextTable[] =
{
    [B_MSG_GLINT_IN_EYE]   = STRINGID_GLINTAPPEARSINEYE,
    [B_MSG_GETTING_IN_POS] = STRINGID_PKMNGETTINGINTOPOSITION,
    [B_MSG_GROWL_DEEPLY]   = STRINGID_PKMNBEGANGROWLINGDEEPLY,
    [B_MSG_EAGER_FOR_MORE] = STRINGID_PKMNEAGERFORMORE,
};

const u8 *const gRefereeStringsTable[] =
{
    [B_MSG_REF_NOTHING_IS_DECIDED] = COMPOUND_STRING(VANILLA_TEXT("REFEREE: If nothing is decided in 3 turns, we will go to judging!",
                                                                  "REFEREE: If nothing is decided in\n3 turns, we will go to judging!")),
    [B_MSG_REF_THATS_IT]           = COMPOUND_STRING(VANILLA_TEXT("REFEREE: That's it! We will now go to judging to determine the winner!",
                                                                  "REFEREE: That's it! We will now go to\njudging to determine the winner!")),
    [B_MSG_REF_JUDGE_MIND]         = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judging category 1, Mind! The POKéMON showing the most guts!\p",
                                                                  "REFEREE: Judging category 1, Mind!\nThe POKéMON showing the most guts!\p")),
    [B_MSG_REF_JUDGE_SKILL]        = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judging category 2, Skill! The POKéMON using moves the best!\p",
                                                                  "REFEREE: Judging category 2, Skill!\nThe POKéMON using moves the best!\p")),
    [B_MSG_REF_JUDGE_BODY]         = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judging category 3, Body! The POKéMON with the most vitality!\p",
                                                                  "REFEREE: Judging category 3, Body!\nThe POKéMON with the most vitality!\p")),
    [B_MSG_REF_PLAYER_WON]         = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judgment: {B_BUFF1} to {B_BUFF2}! The winner is {B_PLAYER_NAME}'s {B_PLAYER_MON1_NAME}!\p",
                                                                  "REFEREE: Judgment: {B_BUFF1} to {B_BUFF2}!\nThe winner is {B_PLAYER_NAME}'s {B_PLAYER_MON1_NAME}!\p")),
    [B_MSG_REF_OPPONENT_WON]       = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judgment: {B_BUFF1} to {B_BUFF2}! The winner is {B_TRAINER1_NAME}'s {B_OPPONENT_MON1_NAME}!\p",
                                                                  "REFEREE: Judgment: {B_BUFF1} to {B_BUFF2}!\nThe winner is {B_TRAINER1_NAME}'s {B_OPPONENT_MON1_NAME}!\p")),
    [B_MSG_REF_DRAW]               = COMPOUND_STRING(VANILLA_TEXT("REFEREE: Judgment: 3 to 3! We have a draw!\p",
                                                                  "REFEREE: Judgment: 3 to 3!\nWe have a draw!\p")),
    [B_MSG_REF_COMMENCE_BATTLE]    = COMPOUND_STRING(VANILLA_TEXT("REFEREE: {B_PLAYER_MON1_NAME} VS {B_OPPONENT_MON1_NAME}! Commence battling!",
                                                                  "REFEREE: {B_PLAYER_MON1_NAME} VS {B_OPPONENT_MON1_NAME}!\nCommence battling!")),
};

static const u8 sText_Trainer1Fled[] = _(VANILLA_TEXT("{PLAY_SE SE_FLEE}{B_TRAINER1_NAME_WITH_CLASS} fled!",
                                                      "{PLAY_SE SE_FLEE}{B_TRAINER1_CLASS} {B_TRAINER1_NAME} fled!"));
static const u8 sText_PlayerLostAgainstTrainer1[] = _(VANILLA_TEXT("You lost to {B_TRAINER1_NAME_WITH_CLASS}!",
                                                                   "Player lost against\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}!"));
static const u8 sText_PlayerBattledToDrawTrainer1[] = _(VANILLA_TEXT("You battled to a draw against {B_TRAINER1_NAME_WITH_CLASS}!",
                                                                     "Player battled to a draw against\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}!"));
const u8 gText_RecordBattleToPass[] = _(VANILLA_TEXT("Would you like to record your battle\non your Frontier Pass?",
                                                     "Would you like to record your battle\non your FRONTIER PASS?"));
const u8 gText_BattleRecordedOnPass[] = _(VANILLA_TEXT("{B_PLAYER_NAME}'s battle result was recorded\non the Frontier Pass.",
                                                       "{B_PLAYER_NAME}'s battle result was recorded\non the FRONTIER PASS."));
static const u8 sText_LinkTrainerWantsToBattlePause[] = _(VANILLA_TEXT("You are challenged by {B_LINK_OPPONENT1_NAME}!\p",
                                                                       "{B_LINK_OPPONENT1_NAME}\nwants to battle!{PAUSE 49}"));
static const u8 sText_TwoLinkTrainersWantToBattlePause[] = _(VANILLA_TEXT("You are challenged by {B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}!\p",
                                                                          "{B_LINK_OPPONENT1_NAME} and {B_LINK_OPPONENT2_NAME}\nwant to battle!{PAUSE 49}"));
static const u8 sText_Your1[] = _("Your");
static const u8 sText_Opposing1[] = _(VANILLA_TEXT("The opposing", "The foe's"));
static const u8 sText_Your2[] = _("your");
static const u8 sText_Opposing2[] = _(VANILLA_TEXT("the opposing", "the foe's"));
static const u8 sText_EmptyStatus[] = _("$$$$$$$");

static const struct BattleWindowText sTextOnWindowsInfo_Normal[] =
{
    [B_WIN_MSG] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_PROMPT] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_MENU] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 13 : 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 15 : 11,
    },
    [B_WIN_DUMMY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP_REMAINING] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 2,
        .y = 1,
        .speed = 0,
        .color.foreground = 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 11,
    },
    [B_WIN_MOVE_TYPE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_SWITCH_PROMPT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_YESNO] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BOX] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = 32,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 2,
    },
    [B_WIN_VS_PLAYER] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_OPPONENT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_OUTCOME_DRAW] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_VS_OUTCOME_LEFT] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_VS_OUTCOME_RIGHT] = {
        .fillValue = PIXEL_FILL(0x0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_MOVE_DESCRIPTION] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .speed = 0,
        .color.foreground = TEXT_DYNAMIC_COLOR_4,
        .color.background = TEXT_DYNAMIC_COLOR_5,
        .color.accent = TEXT_DYNAMIC_COLOR_5,
        .color.shadow = TEXT_DYNAMIC_COLOR_6,
    },
    [B_CATCH_OR_NOT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
};

static const struct BattleWindowText sTextOnWindowsInfo_KantoTutorial[] =
{
    [B_WIN_MSG] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_PROMPT] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_MENU] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 13 : 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 15 : 11,
    },
    [B_WIN_DUMMY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP_REMAINING] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 2,
        .y = 1,
        .speed = 0,
        .color.foreground = 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 11,
    },
    [B_WIN_MOVE_TYPE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_SWITCH_PROMPT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_YESNO] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BOX] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = 32,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 2,
    },
    [B_WIN_VS_PLAYER] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_OPPONENT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_VS_OUTCOME_DRAW] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_VS_OUTCOME_LEFT] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_VS_OUTCOME_RIGHT] = {
        .fillValue = PIXEL_FILL(0x0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 6,
    },
    [B_WIN_MOVE_DESCRIPTION] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .speed = 0,
        .color.foreground = TEXT_DYNAMIC_COLOR_4,
        .color.background = TEXT_DYNAMIC_COLOR_5,
        .color.accent = TEXT_DYNAMIC_COLOR_5,
        .color.shadow = TEXT_DYNAMIC_COLOR_6,
    },
    [B_WIN_OAK_OLD_MAN] = {
        .fillValue = PIXEL_FILL(0x1),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 1,
        .speed = 1,
        .fgColor = 2,
        .bgColor = 1,
        .shadowColor = 3,
    },
};

static const struct BattleWindowText sTextOnWindowsInfo_Arena[] =
{
    [B_WIN_MSG] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_PROMPT] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.background = 15,
        .color.accent = 15,
        .color.shadow = 6,
    },
    [B_WIN_ACTION_MENU] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_MOVE_NAME_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 13 : 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 15 : 11,
    },
    [B_WIN_DUMMY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_PP_REMAINING] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 2,
        .y = 1,
        .speed = 0,
        .color.foreground = 12,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 11,
    },
    [B_WIN_MOVE_TYPE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_SWITCH_PROMPT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_YESNO] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BOX] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = 32,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.shadow = 2,
    },
    [ARENA_WIN_PLAYER_NAME] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 1,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_VS] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_OPPONENT_NAME] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_MIND] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_SKILL] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_BODY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_JUDGMENT_TITLE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .color.foreground = 13,
        .color.background = 14,
        .color.accent = 14,
        .color.shadow = 15,
    },
    [ARENA_WIN_JUDGMENT_TEXT] = {
        .fillValue = PIXEL_FILL(0x1),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .color.foreground = 2,
        .color.background = 1,
        .color.accent = 1,
        .color.shadow = 3,
    },
    [B_WIN_MOVE_DESCRIPTION] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .speed = 0,
        .color.foreground = TEXT_DYNAMIC_COLOR_4,
        .color.background = TEXT_DYNAMIC_COLOR_5,
        .color.accent = TEXT_DYNAMIC_COLOR_5,
        .color.shadow = TEXT_DYNAMIC_COLOR_6,
    },
};

static const struct BattleWindowText *const sBattleTextOnWindowsInfo[] =
{
    [B_WIN_TYPE_NORMAL] = sTextOnWindowsInfo_Normal,
    [B_WIN_TYPE_ARENA]  = sTextOnWindowsInfo_Arena,
    [B_WIN_TYPE_KANTO_TUTORIAL] = sTextOnWindowsInfo_KantoTutorial,
};

static const u8 sRecordedBattleTextSpeeds[] = {8, 4, 1, 0};

void BufferStringBattle(enum StringID stringID, enum BattlerId battler)
{
    s32 i;
    const u8 *stringPtr = NULL;

    gBattleMsgDataPtr = (struct BattleMsgData *)(&gBattleResources->bufferA[battler][4]);
    gLastUsedItem = gBattleMsgDataPtr->lastItem;
    gLastUsedAbility = gBattleMsgDataPtr->lastAbility;
    gBattleScripting.battler = gBattleMsgDataPtr->scrActive;
    gBattleStruct->scriptPartyIdx = gBattleMsgDataPtr->bakScriptPartyIdx;
    gBattleStruct->hpScale = gBattleMsgDataPtr->hpScale;
    gPotentialItemEffectBattler = gBattleMsgDataPtr->itemEffectBattler;
    gBattleStruct->stringMoveType = gBattleMsgDataPtr->moveType;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        sBattlerAbilities[i] = gBattleMsgDataPtr->abilities[i];
    }
    for (i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++)
    {
        gBattleTextBuff1[i] = gBattleMsgDataPtr->textBuffs[0][i];
        gBattleTextBuff2[i] = gBattleMsgDataPtr->textBuffs[1][i];
        gBattleTextBuff3[i] = gBattleMsgDataPtr->textBuffs[2][i];
    }

    switch (stringID)
    {
    case STRINGID_INTROMSG: // first battle msg
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                {
                    stringPtr = sText_TwoTrainersWantToBattle;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                {
                    stringPtr = sText_TwoLinkTrainersWantToBattle;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                    {
                        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
                            stringPtr = sText_LinkTrainerWantsToBattle;
                        else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                            stringPtr = sText_TwoTrainersWantToBattle;
                        else if (!(gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS))
                            stringPtr = sText_LinkTrainerWantsToBattlePause;
                        else
                            stringPtr = sText_TwoLinkTrainersWantToBattlePause;
                    }
                    else
                    {
                        stringPtr = sText_TwoLinkTrainersWantToBattle;
                    }
                }
                else
                {
                    if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
                        stringPtr = sText_Trainer1WantsToBattle;
                    else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                        stringPtr = sText_LinkTrainerWantsToBattlePause;
                    else
                        stringPtr = sText_LinkTrainerWantsToBattle;
                }
            }
            else
            {
                if (BATTLE_TWO_VS_ONE_OPPONENT)
                    stringPtr = sText_Trainer1WantsToBattle;
                else if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
                    stringPtr = sText_TwoTrainersWantToBattle;
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                    stringPtr = sText_TwoTrainersWantToBattle;
                else
                    stringPtr = sText_Trainer1WantsToBattle;
            }
        }
        else
        {
            if (IsGhostBattleWithoutScope())
                stringPtr = sText_GhostAppearedCantId;
            else if (gBattleTypeFlags & BATTLE_TYPE_GHOST)
                stringPtr = sText_TheGhostAppeared;
            else if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
                stringPtr = sText_LegendaryPkmnAppeared;
            else if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT))))
                stringPtr = sText_TwoWildPkmnAppeared;
            else if (gBattleTypeFlags & BATTLE_TYPE_CATCH_TUTORIAL)
                stringPtr = sText_WildPkmnAppearedPause;
            else
                stringPtr = sText_WildPkmnAppeared;
        }
        break;
    case STRINGID_INTROSENDOUT: // poke first send-out
        if (IsOnPlayerSide(battler))
        {
            if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(GetPartnerBattler(battler))))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI && (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK || gBattleTypeFlags & BATTLE_TYPE_LINK))
                {
                    if (BattlerIsPlayer(battler))
                        stringPtr = sText_LinkPartnerSentOutPkmn2GoPkmn; // Player is on left
                    else
                        stringPtr = sText_LinkPartnerSentOutPkmn1GoPkmn; // Link Partner on left
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                {
                    if (BattlerIsPlayer(battler) && (battler & BIT_FLANK) == B_FLANK_LEFT)
                        stringPtr = sText_InGamePartnerSentOutZGoN; // Player is on left
                    else
                        stringPtr = sText_InGamePartnerSentOutNGoZ; // Partner on left
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_RAID)
                {
                    stringPtr = sText_BeCarefulPkmn;
                }
                else
                {
                    stringPtr = sText_GoTwoPkmn;
                }
            }
            else
            {
                stringPtr = sText_GoPkmn;
            }
        }
        else
        {
            if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(GetPartnerBattler(battler))))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && BATTLE_TWO_VS_ONE_OPPONENT)
                    stringPtr = sText_LinkTrainerSentOutTwoPkmn;
                else if (BATTLE_TWO_VS_ONE_OPPONENT)
                    stringPtr = sText_Trainer1SentOutTwoPkmn;
                else if ((gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) || (gBattleTypeFlags & BATTLE_TYPE_MULTI && (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK || gBattleTypeFlags & BATTLE_TYPE_LINK)))
                    stringPtr = sText_TwoLinkTrainersIntroSendOutPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS || gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                    stringPtr = sText_TwoTrainersSentPkmn;
                else if (BattlerIsLink(battler) || (BattlerIsRecorded(battler) && BattlerIsOpponent(battler))) // Link Opponent doubles and test opponent
                    stringPtr = sText_LinkTrainerSentOutTwoPkmn;
                else
                    stringPtr = sText_Trainer1SentOutTwoPkmn;
            }
            else if (BattlerIsLink(battler) || (BattlerIsRecorded(battler) && BattlerIsOpponent(battler)))
            {
                if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
                    stringPtr = sText_Trainer1SentOutPkmn;
                else
                    stringPtr = sText_LinkTrainerIntroSendOutPkmn;
            }
            else
            {
                stringPtr = sText_Trainer1SentOutPkmn;
            }
        }
        break;
    case STRINGID_RETURNMON: // sending poke to ball msg
        if ((GetBattlerPosition(battler) & BIT_FLANK) == B_FLANK_LEFT) // battler 0 and 1
        {
            if (BattlerIsPlayer(battler) || BattlerIsWally(battler)) // Player
            {
                if (*(&gBattleStruct->hpScale) == 0)
                    stringPtr = sText_PkmnThatsEnough;
                else if (*(&gBattleStruct->hpScale) == 1 || IsDoubleBattle())
                    stringPtr = sText_PkmnComeBack;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_PkmnOkComeBack;
                else
                    stringPtr = sText_PkmnGoodComeBack;
            }
            else if (BattlerIsPartner(battler)) // Link or Ingame Partner
            {
                if (BattlerIsLink(battler) || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
                    stringPtr = sText_LinkPartnerWithdrewPkmn1;
                else
                    stringPtr = sText_InGamePartnerWithdrewPkmn1;
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT
             || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK) // Link Opponent A and test opponent
            {
                stringPtr = sText_LinkTrainer1WithdrewPkmn;
            }
            else // Opponent A
            {
                stringPtr = sText_Trainer1WithdrewPkmn;
            }
        }
        else // battler 2 and 3
        {
            if (BattlerIsPlayer(battler)) // Player
            {
                if (*(&gBattleStruct->hpScale) == 0)
                    stringPtr = sText_PkmnThatsEnough;
                else if (*(&gBattleStruct->hpScale) == 1 || IsDoubleBattle())
                    stringPtr = sText_PkmnComeBack;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_PkmnOkComeBack;
                else
                    stringPtr = sText_PkmnGoodComeBack;
            }
            else if (BattlerIsPartner(battler)) // Link or Ingame Partner
            {
                if (BattlerIsLink(battler) || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
                    stringPtr = sText_LinkPartnerWithdrewPkmn2;
                else
                    stringPtr = sText_InGamePartnerWithdrewPkmn2;
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT
             || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK) // Link Opponent B and test opponent
            {
                if (BattleSideHasTwoTrainers(B_SIDE_OPPONENT))
                    stringPtr = sText_LinkTrainer2WithdrewPkmn;
                else
                    stringPtr = sText_LinkTrainer1WithdrewPkmn;
            }
            else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) // Opponent B
            {
                stringPtr = sText_Trainer2WithdrewPkmn;
            }
            else // Opponent A
            {
                stringPtr = sText_Trainer1WithdrewPkmn;
            }
        }
        break;
    case STRINGID_SWITCHINMON: // switch-in msg
        if ((GetBattlerPosition(gBattleScripting.battler) & BIT_FLANK) == B_FLANK_LEFT) // battler 0 and 1
        {
            if (BattlerIsPlayer(gBattleScripting.battler)) // Player
            {
                if (*(&gBattleStruct->hpScale) == 0)
                    stringPtr = sText_GoPkmn2;
                else if (*(&gBattleStruct->hpScale) == 1 || IsDoubleBattle())
                    stringPtr = sText_DoItPkmn;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_GoForItPkmn;
                else
                    stringPtr = sText_YourFoesWeakGetEmPkmn;
            }
            else if (BattlerIsPartner(gBattleScripting.battler)) // Link or Ingame Partner
            {
                if (BattlerIsLink(gBattleScripting.battler) || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
                    stringPtr = sText_LinkPartnerSentOutPkmn1;
                else
                    stringPtr = sText_InGamePartnerSentOutPkmn1;
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT
             || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK) // Link Opponent A and test opponent
            {
                stringPtr = sText_LinkTrainerSentOutPkmn;
            }
            else // Opponent A
            {
                stringPtr = sText_Trainer1SentOutPkmn;
            }
        }
        else // battler 2 and 3
        {
            if (BattlerIsPlayer(gBattleScripting.battler)) // Player
            {
                if (*(&gBattleStruct->hpScale) == 0)
                    stringPtr = sText_GoPkmn2;
                else if (*(&gBattleStruct->hpScale) == 1 || IsDoubleBattle())
                    stringPtr = sText_DoItPkmn;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_GoForItPkmn;
                else
                    stringPtr = sText_YourFoesWeakGetEmPkmn;
            }
            else if (BattlerIsPartner(gBattleScripting.battler)) // Link or Ingame Partner
            {
                if (BattlerIsLink(gBattleScripting.battler) || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
                    stringPtr = sText_LinkPartnerSentOutPkmn2;
                else
                    stringPtr = sText_InGamePartnerSentOutPkmn2;
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT
             || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK) // Link Opponent B and test opponent
            {
                if (BattleSideHasTwoTrainers(B_SIDE_OPPONENT))
                    stringPtr = sText_LinkTrainer2SentOutPkmn2;
                else
                    stringPtr = sText_LinkTrainerSentOutPkmn2;
            }
            else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) // Opponent B
            {
                stringPtr = sText_Trainer2SentOutPkmn;
            }
            else // Single trainer double Opponent A
            {
                stringPtr = sText_Trainer1SentOutPkmn2;
            }
        }
        break;
    case STRINGID_USEDMOVE: // Pokémon used a move msg
        if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT
         && !IsZMove(gBattleMsgDataPtr->currentMove)
         && !IsMaxMove(gBattleMsgDataPtr->currentMove))
            StringCopy(gBattleTextBuff3, gTypesInfo[*(&gBattleStruct->stringMoveType)].generic);
        else
            StringCopy(gBattleTextBuff3, GetMoveName(gBattleMsgDataPtr->currentMove));
        stringPtr = sText_AttackerUsedX;
        break;
    case STRINGID_BATTLEEND: // battle end
        if (gBattleTextBuff1[0] & B_OUTCOME_LINK_BATTLE_RAN)
        {
            gBattleTextBuff1[0] &= ~(B_OUTCOME_LINK_BATTLE_RAN);
            if (!(BattlerIsPlayer(battler) || BattlerIsPlayer(GetPartnerBattler(battler))) && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

            if (gBattleTextBuff1[0] == B_OUTCOME_LOST || gBattleTextBuff1[0] == B_OUTCOME_DREW)
                stringPtr = sText_GotAwaySafely;
            else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                stringPtr = sText_TwoWildFled;
            else
                stringPtr = sText_WildFled;
        }
        else
        {
            if (!(BattlerIsPlayer(battler) || BattlerIsPlayer(GetPartnerBattler(battler))) && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                        stringPtr = sText_TwoInGameTrainersDefeated;
                    else
                        stringPtr = sText_TwoLinkTrainersDefeated;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostToTwo;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawVsTwo;
                    break;
                }
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_PlayerDefeatedLinkTrainerTrainer1;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostAgainstTrainer1;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawTrainer1;
                    break;
                }
            }
            else
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_PlayerDefeatedLinkTrainer;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostAgainstLinkTrainer;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawLinkTrainer;
                    break;
                }
            }
        }
        break;
    case STRINGID_TRAINERSLIDE:
        stringPtr = gBattleStruct->trainerSlideMsg;
        break;
    default: // load a string from the table
        if (stringID >= STRINGID_COUNT)
        {
            gDisplayedStringBattle[0] = EOS;
            return;
        }
        else
        {
            stringPtr = gBattleStringsTable[stringID];
        }
        break;
    }

    BattleStringExpandPlaceholdersToDisplayedString(stringPtr);
}

u32 BattleStringExpandPlaceholdersToDisplayedString(const u8 *src)
{
#ifndef NDEBUG
    u32 j, strWidth;
    u32 dstID = BattleStringExpandPlaceholders(src, gDisplayedStringBattle, sizeof(gDisplayedStringBattle));
    for (j = 1;; j++)
    {
        strWidth = GetStringLineWidth(0, gDisplayedStringBattle, 0, j, sizeof(gDisplayedStringBattle));
        if (strWidth == 0)
            break;
    }
    return dstID;
#else
    return BattleStringExpandPlaceholders(src, gDisplayedStringBattle, sizeof(gDisplayedStringBattle));
#endif
}

static const u8 *TryGetStatusString(u8 *src)
{
    u32 i;
    u8 status[8];
    u32 chars1, chars2;
    u8 *statusPtr;

    memcpy(status, sText_EmptyStatus, min(ARRAY_COUNT(status), ARRAY_COUNT(sText_EmptyStatus)));

    statusPtr = status;
    for (i = 0; i < ARRAY_COUNT(status); i++)
    {
        if (*src == EOS) break; // one line required to match -g
        *statusPtr = *src;
        src++;
        statusPtr++;
    }

    chars1 = *(u32 *)(&status[0]);
    chars2 = *(u32 *)(&status[4]);

    for (i = 0; i < ARRAY_COUNT(gStatusConditionStringsTable); i++)
    {
        if (chars1 == *(u32 *)(&gStatusConditionStringsTable[i][0][0])
            && chars2 == *(u32 *)(&gStatusConditionStringsTable[i][0][4]))
            return gStatusConditionStringsTable[i][1];
    }
    return NULL;
}

static void GetBattlerNick(enum BattlerId battler, u8 *dst)
{
    struct Pokemon *illusionMon = GetIllusionMonPtr(battler);
    struct Pokemon *mon = GetBattlerMon(battler);

    if (illusionMon != NULL)
        mon = illusionMon;
    GetMonData(mon, MON_DATA_NICKNAME, dst);
    StringGet_Nickname(dst);
}

#define HANDLE_NICKNAME_STRING_CASE(battler)                            \
    if (!IsOnPlayerSide(battler))                                       \
    {                                                                   \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefix;                                \
        else                                                            \
            toCpy = sText_WildPkmnPrefix;                               \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
    }                                                                   \
    GetBattlerNick(battler, text);                                      \
    toCpy = text;

#define HANDLE_NICKNAME_STRING_LOWERCASE(battler)                       \
    if (!IsOnPlayerSide(battler))                       \
    {                                                                   \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefixLower;                           \
        else                                                            \
            toCpy = sText_WildPkmnPrefixLower;                          \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
    }                                                                   \
    GetBattlerNick(battler, text);                                      \
    toCpy = text;

static const u8 *BattleStringGetOpponentNameByTrainerId(u16 trainerId, u8 *text, u8 multiplayerId, enum BattlerId battler)
{
    const u8 *toCpy = NULL;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
    {
        u32 i;
        for (i = 0; i < ARRAY_COUNT(gBattleResources->secretBase->trainerName); i++)
            text[i] = gBattleResources->secretBase->trainerName[i];
        text[i] = EOS;
        ConvertInternationalString(text, gBattleResources->secretBase->language);
        toCpy = text;
    }
    else if (trainerId == TRAINER_UNION_ROOM)
    {
        toCpy = gLinkPlayers[multiplayerId ^ BIT_SIDE].name;
    }
    else if (trainerId == TRAINER_LINK_OPPONENT)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battler)].name;
        else
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battler) & BIT_SIDE].name;
    }
    else if (trainerId == TRAINER_FRONTIER_BRAIN)
    {
        CopyFrontierBrainTrainerName(text);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
    {
        GetFrontierTrainerName(text, trainerId);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
    {
        GetTrainerTowerOpponentName(text);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
    {
        GetTrainerHillTrainerName(text, trainerId);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
    {
        GetEreaderTrainerName(text);
        toCpy = text;
    }
    else
    {
        enum TrainerClassID trainerClass = GetTrainerClassFromId(TRAINER_BATTLE_PARAM.opponentA);

        if (trainerClass == TRAINER_CLASS_RIVAL_EARLY_FRLG || trainerClass == TRAINER_CLASS_RIVAL_LATE_FRLG || trainerClass == TRAINER_CLASS_CHAMPION_FRLG)
            toCpy = GetExpandedPlaceholder(PLACEHOLDER_ID_RIVAL);
        else
            toCpy = GetTrainerNameFromId(trainerId);
    }

    assertf(DoesStringProperlyTerminate(toCpy, TRAINER_NAME_LENGTH + 1),"Opponent needs a valid name")
    {
        return sText_EmptyString4;
    }

    return toCpy;
}

static const u8 *BattleStringGetOpponentName(u8 *text, u8 multiplayerId, enum BattlerId battler)
{
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battler))
    {
    case B_POSITION_OPPONENT_LEFT:
        toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, battler);
        break;
    case B_POSITION_OPPONENT_RIGHT:
        if (gBattleTypeFlags & (BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_MULTI) && !BATTLE_TWO_VS_ONE_OPPONENT)
            toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, text, multiplayerId, battler);
        else
            toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, battler);
        break;
    default:
        break;
    }

    return toCpy;
}

static const u8 *BattleStringGetPlayerName(u8 *text, enum BattlerId battler)
{
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battler))
    {
    case B_POSITION_PLAYER_LEFT:
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            toCpy = gLinkPlayers[0].name;
        else
            toCpy = gSaveBlock2Ptr->playerName;
        break;
    case B_POSITION_PLAYER_RIGHT:
        if (((gBattleTypeFlags & BATTLE_TYPE_RECORDED) && !(gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER)))
            || gTestRunnerEnabled)
        {
            toCpy = gLinkPlayers[0].name;
        }
        else if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && gBattleTypeFlags & (BATTLE_TYPE_RECORDED | BATTLE_TYPE_MULTI))
        {
            toCpy = gLinkPlayers[2].name;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        {
            GetFrontierTrainerName(text, gPartnerTrainerId);
            toCpy = text;
        }
        else
        {
            toCpy = gSaveBlock2Ptr->playerName;
        }
        break;
    default:
        break;
    }

    return toCpy;
}

static const u8 *BattleStringGetTrainerName(u8 *text, u8 multiplayerId, enum BattlerId battler)
{
    if (IsOnPlayerSide(battler))
        return BattleStringGetPlayerName(text, battler);
    else
        return BattleStringGetOpponentName(text, multiplayerId, battler);
}

static const u8 *BattleStringGetOpponentClassByTrainerId(u16 trainerId)
{
    const u8 *toCpy;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
        toCpy = gTrainerClasses[GetSecretBaseTrainerClass()].name;
    else if (trainerId == TRAINER_UNION_ROOM)
        toCpy = gTrainerClasses[GetUnionRoomTrainerClass()].name;
    else if (trainerId == TRAINER_FRONTIER_BRAIN)
        toCpy = gTrainerClasses[GetFrontierBrainTrainerClass()].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        toCpy = gTrainerClasses[GetFrontierOpponentClass(trainerId)].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
        toCpy = gTrainerClasses[GetTrainerTowerOpponentClass()].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        toCpy = gTrainerClasses[GetTrainerHillOpponentClass(trainerId)].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
        toCpy = gTrainerClasses[GetEreaderTrainerClassId()].name;
    else if (trainerId == TRAINER_LINK_OPPONENT)
        toCpy = gTrainerClasses[TRAINER_NONE].name;
    else
        toCpy = gTrainerClasses[GetTrainerClassFromId(trainerId)].name;

    return toCpy;
}

// Ensure the defined length for an item name can contain the full defined length of a berry name.
// This ensures that custom Enigma Berry names will fit in the text buffer at the top of BattleStringExpandPlaceholders.
STATIC_ASSERT(BERRY_NAME_LENGTH + ARRAY_COUNT(sText_BerrySuffix) <= ITEM_NAME_LENGTH, BerryNameTooLong);

u32 BattleStringExpandPlaceholders(const u8 *src, u8 *dst, u32 dstSize)
{
    u32 dstID = 0; // if they used dstID, why not use srcID as well?
    const u8 *toCpy = NULL;
    u8 text[max(max(max(32, TRAINER_NAME_LENGTH + 1), POKEMON_NAME_LENGTH + 1), ITEM_NAME_LENGTH)];
    u8 *textStart = &text[0];
    u8 multiplayerId;
    u8 fontId = FONT_NORMAL;

    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
        multiplayerId = gRecordedBattleMultiplayerId;
    else
        multiplayerId = GetMultiplayerId();

    // Clear destination first
    while (dstID < dstSize)
    {
        dst[dstID] = EOS;
        dstID++;
    }

    dstID = 0;
    while (*src != EOS)
    {
        toCpy = NULL;

        if (*src == PLACEHOLDER_BEGIN)
        {
            src++;
            u32 classLength = 0;
            u32 nameLength = 0;
            const u8 *classString;
            const u8 *nameString;
            switch (*src)
            {
            case B_TXT_BUFF1:
                if (gBattleTextBuff1[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gStringVar1);
                    toCpy = gStringVar1;
                }
                else
                {
                    toCpy = TryGetStatusString(gBattleTextBuff1);
                    if (toCpy == NULL)
                        toCpy = gBattleTextBuff1;
                }
                break;
            case B_TXT_BUFF2:
                if (gBattleTextBuff2[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff2, gStringVar2);
                    toCpy = gStringVar2;
                }
                else
                {
                    toCpy = gBattleTextBuff2;
                }
                break;
            case B_TXT_BUFF3:
                if (gBattleTextBuff3[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff3, gStringVar3);
                    toCpy = gStringVar3;
                }
                else
                {
                    toCpy = gBattleTextBuff3;
                }
                break;
            case B_TXT_COPY_VAR_1:
                toCpy = gStringVar1;
                break;
            case B_TXT_COPY_VAR_2:
                toCpy = gStringVar2;
                break;
            case B_TXT_COPY_VAR_3:
                toCpy = gStringVar3;
                break;
            case B_TXT_PLAYER_MON1_NAME: // first player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON1_NAME: // first enemy poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_PLAYER_MON2_NAME: // second player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON2_NAME: // second enemy poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON1_NAME: // link first player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON1_NAME: // link first opponent poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON2_NAME: // link second player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON2_NAME: // link second opponent poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX_MON1: // Unused, to change into sth else.
                break;
            case B_TXT_ATK_PARTNER_NAME: // attacker partner name
                GetBattlerNick(GetPartnerBattler(gBattlerAttacker), text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX: // attacker name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerAttacker)
                break;
            case B_TXT_DEF_NAME_WITH_PREFIX: // target name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerTarget)
                break;
            case B_TXT_DEF_NAME: // target name
                GetBattlerNick(gBattlerTarget, text);
                toCpy = text;
                break;
            case B_TXT_DEF_PARTNER_NAME: // partner target name
                GetBattlerNick(GetPartnerBattler(gBattlerTarget), text);
                toCpy = text;
                break;
            case B_TXT_EFF_NAME_WITH_PREFIX: // effect battler name with prefix
                HANDLE_NICKNAME_STRING_CASE(gEffectBattler)
                break;
            case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX: // scripting active battler name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattleScripting.battler)
                break;
            case B_TXT_CURRENT_MOVE: // current move name
                if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT
                 && !IsZMove(gBattleMsgDataPtr->currentMove)
                 && !IsMaxMove(gBattleMsgDataPtr->currentMove))
                    toCpy = gTypesInfo[gBattleStruct->stringMoveType].generic;
                else
                    toCpy = GetMoveName(gBattleMsgDataPtr->currentMove);
                break;
            case B_TXT_LAST_MOVE: // originally used move name
                if (gBattleMsgDataPtr->originallyUsedMove >= MOVES_COUNT
                 && !IsZMove(gBattleMsgDataPtr->currentMove)
                 && !IsMaxMove(gBattleMsgDataPtr->currentMove))
                    toCpy = gTypesInfo[gBattleStruct->stringMoveType].generic;
                else
                    toCpy = GetMoveName(gBattleMsgDataPtr->originallyUsedMove);
                break;
            case B_TXT_LAST_ITEM: // last used item
                if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
                {
                    if (gLastUsedItem == ITEM_ENIGMA_BERRY_E_READER)
                    {
                        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
                        {
                            if ((gBattleScripting.multiplayerId != 0 && (gPotentialItemEffectBattler & BIT_SIDE))
                                || (gBattleScripting.multiplayerId == 0 && !(gPotentialItemEffectBattler & BIT_SIDE)))
                            {
                                StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                StringAppend(text, sText_BerrySuffix);
                                toCpy = text;
                            }
                            else
                            {
                                toCpy = sText_EnigmaBerry;
                            }
                        }
                        else
                        {
                            if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler)
                            {
                                StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                StringAppend(text, sText_BerrySuffix);
                                toCpy = text;
                            }
                            else
                            {
                                toCpy = sText_EnigmaBerry;
                            }
                        }
                    }
                    else
                    {
                        CopyItemName(gLastUsedItem, text);
                        toCpy = text;
                    }
                }
                else
                {
                    CopyItemName(gLastUsedItem, text);
                    toCpy = text;
                }
                break;
            case B_TXT_LAST_ABILITY: // last used ability
                toCpy = gAbilitiesInfo[gLastUsedAbility].name;
                break;
            case B_TXT_ATK_ABILITY: // attacker ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattlerAttacker]].name;
                break;
            case B_TXT_DEF_ABILITY: // target ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattlerTarget]].name;
                break;
            case B_TXT_SCR_ACTIVE_ABILITY: // scripting active ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattleScripting.battler]].name;
                break;
            case B_TXT_EFF_ABILITY: // effect battler ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gEffectBattler]].name;
                break;
            case B_TXT_TRAINER1_CLASS: // trainer class name
                toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                break;
            case B_TXT_TRAINER1_NAME: // trainer1 name
                toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
                break;
            case B_TXT_TRAINER1_NAME_WITH_CLASS: // trainer1 name with trainer class
                toCpy = textStart;
                classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, textStart, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_LINK_PLAYER_NAME: // link player name
                toCpy = gLinkPlayers[multiplayerId].name;
                break;
            case B_TXT_LINK_PARTNER_NAME: // link partner name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(GetPartnerPosition(gLinkPlayers[multiplayerId].id))].name;
                break;
            case B_TXT_LINK_OPPONENT1_NAME: // link opponent 1 name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(GetBattlerLeftFoe(gLinkPlayers[multiplayerId].id))].name;
                break;
            case B_TXT_LINK_OPPONENT2_NAME: // link opponent 2 name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(GetBattlerRightFoe(gLinkPlayers[multiplayerId].id))].name;
                break;
            case B_TXT_LINK_SCR_TRAINER_NAME: // link scripting active name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(gBattleScripting.battler)].name;
                break;
            case B_TXT_PLAYER_NAME: // player name
                toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
                break;
            case B_TXT_TRAINER1_LOSE_TEXT: // trainerA lose text
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
                {
                    GetTrainerTowerOpponentLoseText(gStringVar4, 0);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else
                {
                    toCpy = GetTrainerALoseText();
                }
                break;
            case B_TXT_TRAINER1_WIN_TEXT: // trainerA win text
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
                {
                    GetTrainerTowerOpponentWinText(gStringVar4, 0);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else
                {
                    toCpy = GetTrainerWonSpeech();
                }
                break;
            case B_TXT_26: // ?
                if (!IsOnPlayerSide(gBattleScripting.battler))
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        toCpy = sText_FoePkmnPrefix;
                    else
                        toCpy = sText_WildPkmnPrefix;
                    while (*toCpy != EOS)
                    {
                        dst[dstID] = *toCpy;
                        dstID++;
                        toCpy++;
                    }
                }
                GetMonData(&GetBattlerParty(gBattleScripting.battler)[gBattleStruct->scriptPartyIdx], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_PC_CREATOR_NAME: // lanette pc
                if (FlagGet(FLAG_SYS_PC_LANETTE))
                    toCpy = IS_FRLG ? sText_Bills : sText_Lanettes;
                else
                    toCpy = sText_Someones;
                break;
            case B_TXT_ATK_PREFIX2:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_DEF_PREFIX2:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_ATK_PREFIX1:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_DEF_PREFIX1:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_ATK_PREFIX3:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            case B_TXT_DEF_PREFIX3:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            case B_TXT_TRAINER2_CLASS:
                toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                break;
            case B_TXT_TRAINER2_NAME:
                toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
                break;
            case B_TXT_TRAINER2_NAME_WITH_CLASS:
                toCpy = textStart;
                classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, textStart, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_TRAINER2_LOSE_TEXT:
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
                {
                    GetTrainerTowerOpponentLoseText(gStringVar4, 1);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else
                {
                    toCpy = GetTrainerBLoseText();
                }
                break;
            case B_TXT_TRAINER2_WIN_TEXT:
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER && gMapHeader.regionMapSectionId == MAPSEC_TRAINER_TOWER_2)
                {
                    GetTrainerTowerOpponentWinText(gStringVar4, 1);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                break;
            case B_TXT_PARTNER_CLASS:
                toCpy = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                break;
            case B_TXT_PARTNER_NAME:
                toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
                break;
            case B_TXT_PARTNER_NAME_WITH_CLASS:
                toCpy = textStart;
                classString = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetPlayerName(textStart, GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_ATK_TRAINER_NAME:
                toCpy = BattleStringGetTrainerName(text, multiplayerId, gBattlerAttacker);
                break;
            case B_TXT_ATK_TRAINER_CLASS:
                switch (GetBattlerPosition(gBattlerAttacker))
                {
                case B_POSITION_PLAYER_RIGHT:
                    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                        toCpy = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                    break;
                case B_POSITION_OPPONENT_LEFT:
                    toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                    break;
                case B_POSITION_OPPONENT_RIGHT:
                    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
                        toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                    else
                        toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                    break;
                default:
                    break;
                }
                break;
            case B_TXT_ATK_TRAINER_NAME_WITH_CLASS:
                toCpy = textStart;
                if (gBattleTypeFlags & BATTLE_TYPE_CATCH_TUTORIAL)
                {
                    if (IS_FRLG)
                        textStart = StringCopy(textStart, COMPOUND_STRING("The old man"));
                    else
                        textStart = StringCopy(textStart, COMPOUND_STRING("WALLY"));
                }
                else if (GetBattlerPosition(gBattlerAttacker) == B_POSITION_PLAYER_LEFT)
                {
                    textStart = StringCopy(textStart, BattleStringGetTrainerName(textStart, multiplayerId, gBattlerAttacker));
                }
                else
                {
                    classString = NULL;
                    switch (GetBattlerPosition(gBattlerAttacker))
                    {
                    case B_POSITION_PLAYER_RIGHT:
                        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                            classString = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                        break;
                    case B_POSITION_OPPONENT_LEFT:
                        classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                        break;
                    case B_POSITION_OPPONENT_RIGHT:
                        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
                            classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                        else
                            classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                        break;
                    default:
                        break;
                    }
                    classLength = 0;
                    nameLength = 0;
                    while (classString[classLength] != EOS)
                    {
                        textStart[classLength] = classString[classLength];
                        classLength++;
                    }
                    textStart[classLength] = CHAR_SPACE;
                    textStart += 1 + classLength;
                    nameString = BattleStringGetTrainerName(textStart, multiplayerId, gBattlerAttacker);
                    if (nameString != textStart)
                    {
                        while (nameString[nameLength] != EOS)
                        {
                            textStart[nameLength] = nameString[nameLength];
                            nameLength++;
                        }
                        textStart[nameLength] = EOS;
                    }
                }
                break;
            case B_TXT_ATK_TEAM1:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_ATK_TEAM2:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_DEF_TEAM1:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_DEF_TEAM2:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_EFF_TEAM1:
                if (IsOnPlayerSide(gEffectBattler))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_EFF_TEAM2:
                if (IsOnPlayerSide(gEffectBattler))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattlerAttacker)
                break;
            case B_TXT_DEF_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattlerTarget)
                break;
            case B_TXT_EFF_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gEffectBattler)
                break;
            case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattleScripting.battler)
                break;
            }

            if (toCpy != NULL)
            {
                while (*toCpy != EOS)
                {
                    if (*toCpy == CHAR_SPACE)
                        dst[dstID] = CHAR_NBSP;
                    else
                        dst[dstID] = *toCpy;
                    dstID++;
                    toCpy++;
                }
            }

            if (*src == B_TXT_TRAINER1_LOSE_TEXT || *src == B_TXT_TRAINER2_LOSE_TEXT
                || *src == B_TXT_TRAINER1_WIN_TEXT || *src == B_TXT_TRAINER2_WIN_TEXT)
            {
                dst[dstID] = EXT_CTRL_CODE_BEGIN;
                dstID++;
                dst[dstID] = EXT_CTRL_CODE_PAUSE_UNTIL_PRESS;
                dstID++;
            }
        }
        else
        {
            dst[dstID] = *src;
            dstID++;
        }
        src++;
    }

    dst[dstID] = *src;
    dstID++;

    BreakStringAutomatic(dst, BATTLE_MSG_MAX_WIDTH, BATTLE_MSG_MAX_LINES, fontId, SHOW_SCROLL_PROMPT);

    return dstID;
}

static void IllusionNickHack(enum BattlerId battler, u32 partyId, u8 *dst)
{
    u32 id = PARTY_SIZE;
    struct Pokemon *party = GetBattlerParty(battler);
    struct Pokemon *mon = &party[partyId], *partnerMon;

    if (GetMonAbility(mon) == ABILITY_ILLUSION)
    {
        if (IsBattlerAlive(GetPartnerBattler(battler)))
            partnerMon = GetBattlerMon(GetPartnerBattler(battler));
        else
            partnerMon = mon;

        id = GetIllusionMonPartyId(party, mon, partnerMon, battler);
    }

    if (id != PARTY_SIZE)
        GetMonData(&party[id], MON_DATA_NICKNAME, dst);
    else
        GetMonData(mon, MON_DATA_NICKNAME, dst);
}

void ExpandBattleTextBuffPlaceholders(const u8 *src, u8 *dst)
{
    u32 srcID = 1;
    u32 value = 0;
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u16 hword;

    *dst = EOS;
    while (src[srcID] != B_BUFF_EOS)
    {
        switch (src[srcID])
        {
        case B_BUFF_STRING: // battle string
            hword = T1_READ_16(&src[srcID + 1]);
            StringAppend(dst, gBattleStringsTable[hword]);
            srcID += 3;
            break;
        case B_BUFF_NUMBER: // int to string
            switch (src[srcID + 1])
            {
            case 1:
                value = src[srcID + 3];
                break;
            case 2:
                value = T1_READ_16(&src[srcID + 3]);
                break;
            case 4:
                value = T1_READ_32(&src[srcID + 3]);
                break;
            }
            ConvertIntToDecimalStringN(dst, value, STR_CONV_MODE_LEFT_ALIGN, src[srcID + 2]);
            srcID += src[srcID + 1] + 3;
            break;
        case B_BUFF_MOVE: // move name
            StringAppend(dst, GetMoveName(T1_READ_16(&src[srcID + 1])));
            srcID += 3;
            break;
        case B_BUFF_TYPE: // type name
            StringAppend(dst, gTypesInfo[src[srcID + 1]].name);
            srcID += 2;
            break;
        case B_BUFF_MON_NICK_WITH_PREFIX: // poke nick with prefix
        case B_BUFF_MON_NICK_WITH_PREFIX_LOWER: // poke nick with lowercase prefix
            if (!IsOnPlayerSide(src[srcID + 1]))
            {
                if (src[srcID] == B_BUFF_MON_NICK_WITH_PREFIX_LOWER)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        StringAppend(dst, sText_FoePkmnPrefixLower);
                    else
                        StringAppend(dst, sText_WildPkmnPrefixLower);
                }
                else
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        StringAppend(dst, sText_FoePkmnPrefix);
                    else
                        StringAppend(dst, sText_WildPkmnPrefix);
                }
            }
            GetMonData(&GetBattlerParty(src[srcID + 1])[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
            StringGet_Nickname(nickname);
            StringAppend(dst, nickname);
            srcID += 3;
            break;
        case B_BUFF_STAT: // stats
            StringAppend(dst, gStatNamesTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_SPECIES: // species name
            StringCopy(dst, GetSpeciesName(T1_READ_16(&src[srcID + 1])));
            srcID += 3;
            break;
        case B_BUFF_MON_NICK: // poke nick without prefix
            if (src[srcID + 2] == gBattlerPartyIndexes[src[srcID + 1]])
            {
                GetBattlerNick(src[srcID + 1], dst);
            }
            else if (gBattleScripting.illusionNickHack) // for STRINGID_ENEMYABOUTTOSWITCHPKMN
            {
                gBattleScripting.illusionNickHack = 0;
                IllusionNickHack(src[srcID + 1], src[srcID + 2], dst);
                StringGet_Nickname(dst);
            }
            else
            {
                GetMonData(&GetBattlerParty(src[srcID + 1])[src[srcID + 2]], MON_DATA_NICKNAME, dst);
                StringGet_Nickname(dst);
            }
            srcID += 3;
            break;
        case B_BUFF_NEGATIVE_FLAVOR: // flavor table
            StringAppend(dst, gPokeblockWasTooXStringTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_ABILITY: // ability names
            StringAppend(dst, gAbilitiesInfo[T1_READ_16(&src[srcID + 1])].name);
            srcID += 3;
            break;
        case B_BUFF_ITEM: // item name
            hword = T1_READ_16(&src[srcID + 1]);
            if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            {
                if (hword == ITEM_ENIGMA_BERRY_E_READER)
                {
                    if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler)
                    {
                        StringCopy(dst, gEnigmaBerries[gPotentialItemEffectBattler].name);
                        StringAppend(dst, sText_BerrySuffix);
                    }
                    else
                    {
                        StringAppend(dst, sText_EnigmaBerry);
                    }
                }
                else
                {
                    CopyItemName(hword, dst);
                }
            }
            else
            {
                CopyItemName(hword, dst);
            }
            srcID += 3;
            break;
        }
    }
}

void BattlePutTextOnWindow(const u8 *text, u8 windowId)
{
    const struct BattleWindowText *textInfo = sBattleTextOnWindowsInfo[gBattleScripting.windowsType];
    bool32 copyToVram;
    struct TextPrinterTemplate printerTemplate;
    u8 speed;

    if (windowId & B_WIN_COPYTOVRAM)
    {
        windowId &= ~B_WIN_COPYTOVRAM;
        copyToVram = FALSE;
    }
    else
    {
        FillWindowPixelBuffer(windowId, textInfo[windowId].fillValue);
        copyToVram = TRUE;
    }

    printerTemplate.currentChar = text;
    printerTemplate.type = WINDOW_TEXT_PRINTER;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = textInfo[windowId].fontId;
    printerTemplate.x = textInfo[windowId].x;
    printerTemplate.y = textInfo[windowId].y;
    printerTemplate.currentX = printerTemplate.x;
    printerTemplate.currentY = printerTemplate.y;
    printerTemplate.letterSpacing = textInfo[windowId].letterSpacing;
    printerTemplate.lineSpacing = textInfo[windowId].lineSpacing;
    printerTemplate.color = textInfo[windowId].color;

    if (B_WIN_MOVE_NAME_1 <= windowId && windowId <= B_WIN_MOVE_NAME_4)
    {
        // We cannot check the actual width of the window because
        // B_WIN_MOVE_NAME_1 and B_WIN_MOVE_NAME_3 are 16 wide for
        // Z-move details.
        if (gBattleStruct->zmove.viewing && windowId == B_WIN_MOVE_NAME_1)
            printerTemplate.fontId = GetFontIdToFit(text, printerTemplate.fontId, printerTemplate.letterSpacing, 16 * TILE_WIDTH);
        else
            printerTemplate.fontId = GetFontIdToFit(text, printerTemplate.fontId, printerTemplate.letterSpacing, 8 * TILE_WIDTH);
    }

    if (printerTemplate.x == 0xFF)
    {
        u32 width = GetBattleWindowTemplatePixelWidth(gBattleScripting.windowsType, windowId);
        s32 alignX = GetStringCenterAlignXOffsetWithLetterSpacing(printerTemplate.fontId, printerTemplate.currentChar, width, printerTemplate.letterSpacing);
        printerTemplate.x = printerTemplate.currentX = alignX;
    }

    if (windowId == ARENA_WIN_JUDGMENT_TEXT || windowId == B_WIN_OAK_OLD_MAN)
        gTextFlags.useAlternateDownArrow = FALSE;
    else
        gTextFlags.useAlternateDownArrow = TRUE;

    if ((gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED)) || gTestRunnerEnabled || ((gBattleTypeFlags & BATTLE_TYPE_POKEDUDE) && windowId != B_WIN_OAK_OLD_MAN))
        gTextFlags.autoScroll = TRUE;
    else
        gTextFlags.autoScroll = FALSE;

    if (windowId == B_WIN_MSG || windowId == ARENA_WIN_JUDGMENT_TEXT || windowId == B_WIN_OAK_OLD_MAN)
    {
        if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            speed = 1;
        else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            speed = sRecordedBattleTextSpeeds[GetTextSpeedInRecordedBattle()];
        else
            speed = GetPlayerTextSpeedDelay();

        gTextFlags.canABSpeedUpPrint = 1;
    }
    else
    {
        speed = textInfo[windowId].speed;
        gTextFlags.canABSpeedUpPrint = 0;
    }

    AddTextPrinter(&printerTemplate, speed, NULL);

    if (copyToVram)
    {
        PutWindowTilemap(windowId);
        CopyWindowToVram(windowId, COPYWIN_FULL);
    }
}

void SetPPNumbersPaletteInMoveSelection(enum BattlerId battler)
{
    struct ChooseMoveStruct *chooseMoveStruct = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    const u16 *palPtr = gPPTextPalette;
    u8 var;

    if (!gBattleStruct->zmove.viewing)
        var = GetCurrentPPToMaxPPState(chooseMoveStruct->currentPP[gMoveSelectionCursor[battler]],
                         chooseMoveStruct->maxPP[gMoveSelectionCursor[battler]]);
    else
        var = 3;

    gPlttBufferUnfaded[BG_PLTT_ID(5) + 12] = palPtr[(var * 2) + 0];
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 11] = palPtr[(var * 2) + 1];

    CpuCopy16(&gPlttBufferUnfaded[BG_PLTT_ID(5) + 12], &gPlttBufferFaded[BG_PLTT_ID(5) + 12], PLTT_SIZEOF(1));
    CpuCopy16(&gPlttBufferUnfaded[BG_PLTT_ID(5) + 11], &gPlttBufferFaded[BG_PLTT_ID(5) + 11], PLTT_SIZEOF(1));
}

u8 GetCurrentPPToMaxPPState(u8 currentPP, u8 maxPP)
{
    if (maxPP == currentPP)
    {
        return 3;
    }
    else if (maxPP <= 2)
    {
        if (currentPP > 1)
            return 3;
        else
            return 2 - currentPP;
    }
    else if (maxPP <= 7)
    {
        if (currentPP > 2)
            return 3;
        else
            return 2 - currentPP;
    }
    else
    {
        if (currentPP == 0)
            return 2;
        if (currentPP <= maxPP / 4)
            return 1;
        if (currentPP > maxPP / 2)
            return 3;
    }

    return 0;
}
