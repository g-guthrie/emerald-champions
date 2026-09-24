#include "global.h"
#include "center_guide.h"
#include "event_data.h"
#include "item.h"
#include "overworld.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"

// The Pokémon Center local guide is the game's built-in walkthrough: the
// next story destination, the local side quests and (in legendary_signs.c)
// the rare Pokémon leads. Every text states its requirement once, in prose.
// This file only reads progress; it never unlocks or changes anything.

enum CenterGuideCheck
{
    CENTER_GUIDE_CHECK_NONE,
    CENTER_GUIDE_CHECK_BLOB_LOST,     // Route 111 nurse is still waiting for help
    CENTER_GUIDE_CHECK_BLOB_CHASE,    // Blob is on the run (Route 112 to Ashen Woods)
    CENTER_GUIDE_CHECK_BLOB_FOUND,    // Blob caught, reward not yet claimed
    CENTER_GUIDE_CHECK_VIAL_ROUTE133, // second Poké Vial upgrade is waiting
    CENTER_GUIDE_CHECK_TRICK_HOUSE,   // the last puzzle is still unsolved
    CENTER_GUIDE_CHECK_ODD_KEYSTONE,  // the Keystone has not been spent yet
};

struct CenterGuideTip
{
    u16 city;
    u16 requiredFlag;
    u16 doneFlag;
    u8 minimumBadges;
    u8 check;
    const u8 *text;
};

static const u8 sTip_MoveTutor[] = _("Every Pokémon Center has a move\ntutor. It teaches any move a\lPokémon can legally learn, and\lno machines are needed.");
static const u8 sTip_TrickHouse[] = _("The Trick Master hides in his\nhouse on Route 110. Find him,\lsolve his maze and claim a\lprize.\pYour third badge and every one\nafter it open a new puzzle.\lThe last waits until you are\lChampion.");
static const u8 sTip_BlobLost[] = _("A nurse on Route 111, just north\nof Mauville, has lost her\lChansey, Blob.\pHelp catch it, and every nurse\nwill fill your Poké Vial with\lan extra dose.");
static const u8 sTip_BlobChase[] = _("Blob ran off by the Route 112\ncable car. Follow it down\lJagged Pass, through Ember Path\land into the Ashen Woods.\pCatch it with a Heal Ball.\nVerdanturf's Pokémon Center\lsells them.");
static const u8 sTip_BlobFound[] = _("You caught Blob! Take it back\nto the nurse on Route 111 for\lyour reward.");
static const u8 sTip_Desert[] = _("With Go-Goggles from Lavaridge,\nyou can cross the Route 111\ldesert.\pMirage Tower there hides two\nfossils, but it crumbles once\lyou take one.\pDevon's lab in Rustboro can\nrevive fossils.");
static const u8 sTip_BerryMaster[] = _("The Berry Master lives on\nRoute 123. He hands out\lBerries every day and rewards\lthe Berries you harvest.");

static const struct CenterGuideTip sCenterGuideTips[] =
{
#include "data/center_guide_tips.h"
};

static u32 CountGuideBadges(void)
{
    u32 count = 0;

    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        count += FlagGet(FLAG_BADGE01_GET + badge) != 0;
    return count;
}

static bool32 IsGuideCheckActive(u8 check)
{
    u16 nurseState = VarGet(VAR_CHANSEY_NURSE_STATE);
    u16 vialCharges = VarGet(VAR_POKE_VIAL_MAX_CHARGES);

    switch (check)
    {
    case CENTER_GUIDE_CHECK_BLOB_LOST:
        return nurseState == 0 && vialCharges < 2;
    case CENTER_GUIDE_CHECK_BLOB_CHASE:
        return nurseState >= 1 && nurseState <= 5 && vialCharges < 2;
    case CENTER_GUIDE_CHECK_BLOB_FOUND:
        return nurseState == 6 && vialCharges < 2;
    case CENTER_GUIDE_CHECK_VIAL_ROUTE133:
        return vialCharges == 2;
    case CENTER_GUIDE_CHECK_TRICK_HOUSE:
        return VarGet(VAR_TRICK_HOUSE_LEVEL) < 8;
    case CENTER_GUIDE_CHECK_ODD_KEYSTONE:
        // Picked up (its object flag is set) and no longer carried: spent.
        return !FlagGet(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE)
            || CheckBagHasItem(ITEM_ODD_KEYSTONE, 1) || CheckPCHasItem(ITEM_ODD_KEYSTONE, 1);
    default:
        return TRUE;
    }
}

static bool32 IsGuideTipActive(const struct CenterGuideTip *tip)
{
    return CountGuideBadges() >= tip->minimumBadges
        && (tip->requiredFlag == 0 || FlagGet(tip->requiredFlag))
        && (tip->doneFlag == 0 || !FlagGet(tip->doneFlag))
        && IsGuideCheckActive(tip->check);
}

void BufferNextCenterGuideTip(void)
{
    for (u32 i = gSpecialVar_0x8004; i < ARRAY_COUNT(sCenterGuideTips); i++)
    {
        if (sCenterGuideTips[i].city != gMapHeader.regionMapSectionId
         || !IsGuideTipActive(&sCenterGuideTips[i]))
            continue;
        gSpecialVar_0x8004 = i + 1;
        StringCopy(gStringVar4, sCenterGuideTips[i].text);
        gSpecialVar_Result = TRUE;
        return;
    }
    gSpecialVar_0x8004 = ARRAY_COUNT(sCenterGuideTips);
    gSpecialVar_Result = FALSE;
}

// The main story from the Route 103 errand to the Hall of Fame, one step per
// row, in the order the campaign opens them. The first row whose flag is
// still clear is where the player should head next.
static const struct
{
    u16 flag;
    const u8 *text;
} sCenterGuideStory[] =
{
    {FLAG_DEFEATED_RIVAL_ROUTE103, COMPOUND_STRING("Prof. Birch's kid is studying\nPokémon on Route 103, north of\lOldale Town. Go and meet them!")},
    {FLAG_ADVENTURE_STARTED, COMPOUND_STRING("Head back to Prof. Birch's lab\nin Littleroot Town. He has\lsomething for you.")},
    {FLAG_BADGE01_GET, COMPOUND_STRING("Head west to Petalburg City and\nvisit its Gym. Then go north\lthrough Petalburg Woods to\lRustboro City.\pRoxanne, the Rustboro Gym\nLeader, has your first badge.")},
    {FLAG_RECOVERED_DEVON_GOODS, COMPOUND_STRING("A thief stole goods from Devon!\nHe ran east along Route 116\linto Rusturf Tunnel.")},
    {FLAG_RETURNED_DEVON_GOODS, COMPOUND_STRING("Bring the Devon Goods back to\nthe Devon employee waiting in\lRustboro City.")},
    {FLAG_RECEIVED_POKENAV, COMPOUND_STRING("Devon's president wants to\nthank you. Visit him on the top\lfloor of Devon Corporation.")},
    {FLAG_DELIVERED_STEVEN_LETTER, COMPOUND_STRING("Mr. Briney, whose cottage is on\nRoute 104, will sail you to\lDewford Town.\pBring the letter to Steven in\nGranite Cave, north of Dewford.")},
    {FLAG_DELIVERED_DEVON_GOODS, COMPOUND_STRING("Mr. Briney can sail you from\nDewford to Slateport City.\pBring the Devon Goods to\nCapt. Stern at the Oceanic\lMuseum there.")},
    {FLAG_HIDE_SLATEPORT_CITY_BRAWLY, COMPOUND_STRING("Brawly, Dewford's Gym Leader,\nwent to Slateport City. Look\lfor him near the Oceanic\lMuseum.")},
    {FLAG_BADGE02_GET, COMPOUND_STRING("Brawly is back at the Dewford\nGym. Mr. Briney sails there\lfrom Route 109, just south of\lSlateport.")},
    {FLAG_BADGE03_GET, COMPOUND_STRING("Take Route 110 north from\nSlateport to Mauville City.\lWattson leads the Gym there.")},
    {FLAG_MET_ARCHIE_METEOR_FALLS, COMPOUND_STRING("Team Magma is blocking the\ncable car on Route 112.\pGo through Fiery Path to Route\n113, then west to Fallarbor.\lMagma is up to something at\lMeteor Falls on Route 114.")},
    {FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, COMPOUND_STRING("Team Magma has gone up\nMt. Chimney. Ride the cable car\lfrom Route 112 and stop them!")},
    {FLAG_BADGE04_GET, COMPOUND_STRING("Head down Jagged Pass to\nLavaridge Town. Flannery leads\lthe Gym there.")},
    {FLAG_BADGE05_GET, COMPOUND_STRING("Return to Petalburg City.\nNorman will accept your Gym\lchallenge now.")},
    {FLAG_HIDE_ROUTE_119_TEAM_AQUA, COMPOUND_STRING("Surf east from Mauville along\nRoute 118, then go north on\lRoute 119.\pTeam Aqua has taken over the\nWeather Institute there!")},
    {FLAG_RECEIVED_DEVON_SCOPE, COMPOUND_STRING("Something unseen blocks the\nGym in Fortree City. Steven is\lwaiting on Route 120, just\least of Fortree.")},
    {FLAG_KECLEON_FLED_FORTREE, COMPOUND_STRING("Use the Devon Scope on what\nblocks the Fortree Gym. Then\lchallenge Winona!")},
    {FLAG_BADGE06_GET, COMPOUND_STRING("Winona leads the Fortree Gym.\nWin your sixth badge there!")},
    {FLAG_RECEIVED_RED_OR_BLUE_ORB, COMPOUND_STRING("Teams Aqua and Magma are after\nsomething on Mt. Pyre, south of\lRoute 121. Hurry to the summit!")},
    {FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, COMPOUND_STRING("The Magma Emblem opens Team\nMagma's hideout. Look for it\lon Jagged Pass.")},
    {FLAG_MET_TEAM_AQUA_HARBOR, COMPOUND_STRING("Team Aqua is after Capt.\nStern's submarine. Hurry to\lSlateport's harbor!")},
    {FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, COMPOUND_STRING("Team Aqua's hideout is in the\ncove on Lilycove City's east\lside. Surf there and stop them!")},
    {FLAG_BADGE07_GET, COMPOUND_STRING("Surf east from Lilycove to\nMossdeep City. Tate and Liza\llead the Gym there.")},
    {FLAG_DEFEATED_MAGMA_SPACE_CENTER, COMPOUND_STRING("Team Magma is storming the\nMossdeep Space Center! Hurry\land help Steven.")},
    {FLAG_RECEIVED_HM08, COMPOUND_STRING("Steven is waiting for you at\nhis house in Mossdeep City.")},
    {FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, COMPOUND_STRING("Team Aqua's submarine went\nunder Route 128. Dive there to\lreach the Seafloor Cavern.")},
    {FLAG_WALLACE_GOES_TO_SKY_PILLAR, COMPOUND_STRING("Sootopolis City sits inside the\ncrater on Route 126. Dive\lbelow it to find the way in.\pMeet Steven there, then find\nWallace in the Cave of Origin.")},
};

// Sky Pillar and the calm that follows: VAR_SOOTOPOLIS_CITY_STATE counts
// Wallace's walk (3-4), Rayquaza's awakening (5) and Juan's badge (6).
static const u8 sText_GuideSkyPillar[] = _("Wallace is waiting at Sky\nPillar on Route 131. Climb to\lthe top and wake Rayquaza.\pThe cracked floors inside call\nfor a Mach Bike.");
static const u8 sText_GuideLeaders[] = _("Rayquaza calmed the skies!\nBack in Sootopolis, hear what\lMaxie and Archie have to say.");
static const u8 sText_GuideWaterfall[] = _("Wallace is waiting for you in\nSootopolis City.");
static const u8 sText_GuideJuan[] = _("Juan leads the Sootopolis Gym.\nWin your last badge there!");
static const u8 sText_GuideLeague[] = _("Ever Grande City lies east of\nRoute 128. Climb its waterfall,\lcross Victory Road and take on\lthe Pokémon League!");

static const u8 *GetCenterGuideStoryText(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCenterGuideStory); i++)
    {
        if (!FlagGet(sCenterGuideStory[i].flag))
            return sCenterGuideStory[i].text;
    }
    if (VarGet(VAR_SOOTOPOLIS_CITY_STATE) < 5)
        return sText_GuideSkyPillar;
    if (!FlagGet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE))
        return sText_GuideLeaders;
    if (!FlagGet(FLAG_RECEIVED_HM07))
        return sText_GuideWaterfall;
    if (!FlagGet(FLAG_BADGE08_GET))
        return sText_GuideJuan;
    return sText_GuideLeague;
}

void BufferCenterGuideDirections(void)
{
    // After the Hall of Fame the shared finale directions take over.
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    StringCopy(gStringVar4, GetCenterGuideStoryText());
    gSpecialVar_Result = TRUE;
}
