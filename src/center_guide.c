#include "global.h"
#include "center_guide.h"
#include "event_data.h"
#include "battle_setup.h"
#include "item.h"
#include "constants/items.h"
#include "constants/trainers.h"
#include "constants/emerald_champions.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "quest_states.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"

// These existing script queries own the party-cap and finale predicates.
void IsPlayerPartyBelowLevelCap(void);
u16 GetEmeraldChampionsFinaleStage(void);

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
    CENTER_GUIDE_CHECK_ARCEUS_GIFT,   // Devon's Arceus is still waiting
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

static const u8 sTip_MoveTutor[] = _("Every Pokémon Center has a Move\nTutor. No machines needed!\pBring any Pokémon, and the Tutor\nwill teach it any move it can learn.");
static const u8 sTip_TrickHouse[] = _("The Trick Master hides in his\nhouse on Route 110. Find him,\lsolve his maze and claim a prize.\pYour third Badge and every one\nafter it open a new puzzle.\lThe last waits until you are Champion.");
static const u8 sTip_BlobLost[] = _("A nurse on Route 111, just north\nof Mauville, has lost her\lChansey, Blob.\pHelp catch it, and every nurse\nwill fill your Poké Vial with\lan extra dose.");
static const u8 sTip_BlobChase[] = _("Blob ran off by the Route 112\ncable car. Follow it down\lJagged Pass, through Ember Path\land into the Ashen Woods.\pCatch it with a Heal Ball.\nVerdanturf's Pokémon Center\lsells them.");
static const u8 sTip_BlobFound[] = _("You caught Blob! Take it back\nto the nurse on Route 111 for\lyour reward.");
static const u8 sTip_Desert[] = _("With Go-Goggles from Lavaridge,\nyou can cross the Route 111 desert.\pMirage Tower there hides two\nfossils, but it crumbles once\lyou take one.\pDevon's lab in Rustboro can\nrevive fossils.");
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

// Every quest state is read through include/quest_states.h, the same
// predicates the quest scripts use, so the advice follows the quest.
static bool32 IsGuideCheckActive(u8 check)
{
    switch (check)
    {
    case CENTER_GUIDE_CHECK_BLOB_LOST:
        return GetChanseyQuestStage() == CHANSEY_STAGE_NEEDS_HELP;
    case CENTER_GUIDE_CHECK_BLOB_CHASE:
        return GetChanseyQuestStage() == CHANSEY_STAGE_CHASE;
    case CENTER_GUIDE_CHECK_BLOB_FOUND:
        return IsChanseyVialRewardAvailable();
    case CENTER_GUIDE_CHECK_VIAL_ROUTE133:
        return IsRoute133VialUpgradeAvailable();
    case CENTER_GUIDE_CHECK_TRICK_HOUSE:
        return !IsTrickHouseComplete();
    case CENTER_GUIDE_CHECK_ODD_KEYSTONE:
        return !IsOddKeystoneSpent();
    case CENTER_GUIDE_CHECK_ARCEUS_GIFT:
        return !IsLegendarySignCaught(LEGENDARY_SIGN_ARCEUS);
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
    const u8 *objective;
} sCenterGuideStory[] =
{
    {FLAG_DEFEATED_RIVAL_ROUTE103, COMPOUND_STRING("Meet Birch's kid on Route 103,\nnorth of Oldale, and win your\lfirst rival battle."), COMPOUND_STRING("Meet Birch's kid on Route 103,\nnorth of Oldale, for your battle.")},
    {FLAG_ADVENTURE_STARTED, COMPOUND_STRING("Return south through Oldale\nto Birch's lab in Littleroot.\lSpeak to Prof. Birch for your\lPokédex and Poké Balls."), COMPOUND_STRING("Return to Birch's lab in\nLittleroot for your send-off.")},
    {FLAG_BADGE01_GET, COMPOUND_STRING("Leave Petalburg west on Route\n104. Cross Petalburg Woods,\lthen follow Route 104 north\lto Rustboro City.\pEnter the Rustboro Gym and\ndefeat Roxanne for Badge one."), COMPOUND_STRING("Challenge Roxanne at Rustboro's\nGym for your first Badge.")},
    {FLAG_RECOVERED_DEVON_GOODS, COMPOUND_STRING("Leave Rustboro east on Route\n116. Enter Rusturf Tunnel at\lthe far east end.\pSpeak to the Aqua thief by\nPeeko and defeat him to\lrecover the Devon Goods."), COMPOUND_STRING("Recover the Devon Goods from\nthe thief in Rusturf Tunnel,\least of Rustboro on Route 116.")},
    {FLAG_RETURNED_DEVON_GOODS, COMPOUND_STRING("Return west along Route 116\nto Rustboro. Speak to the\lDevon employee near the\lcity's northeast exit.\pHe takes you to Devon. If\nyour Bag is full, make room\lfor his reward and try again."), COMPOUND_STRING("Return the Devon Goods to the\nemployee by Rustboro's northeast\lexit. Make room for his reward.")},
    {FLAG_RECEIVED_POKENAV, COMPOUND_STRING("Go to Devon Corporation in\nnorthwest Rustboro. Speak to\lMr. Stone on the third floor.\pAccept his Letter and PokéNav.\nIf your Bag is full, make\lroom and speak to him again."), COMPOUND_STRING("Visit Mr. Stone on Devon's\nthird floor in Rustboro for\lhis Letter and the PokéNav.")},
    {FLAG_DELIVERED_STEVEN_LETTER, COMPOUND_STRING("Go south through Petalburg\nWoods to Briney's cottage on\lsouth Route 104. Ask him to\lsail to Dewford.\pFrom Dewford, walk north along\nRoute 106 into Granite Cave.\lFollow 1F's passage west to\lits ladder and go down.\pOn B1F, follow the lower path\nnorth, east, then south to\lthe southeast ladder. Go down.\pOn B2F, follow the lower path\nwest around the dividing wall.\lTurn north, then follow the\lupper path east to its ladder.\pGo up to B1F. Walk left to\nthe next ladder and go up.\pBack on 1F, follow the passage\nwest, then south to Steven's\lroom. Give him the Letter."), COMPOUND_STRING("Deliver the Letter to Steven\nin Granite Cave, north of\lDewford. Briney can sail you there.")},
    {FLAG_DELIVERED_DEVON_GOODS, COMPOUND_STRING("Speak to Briney by Dewford's\nboat and choose Slateport.\lWalk north from Route 109\linto Slateport City.\pEnter the Oceanic Museum. Go\nupstairs, speak to Capt. Stern\land defeat Aqua's attackers\lto deliver the Devon Goods."), COMPOUND_STRING("Deliver the Devon Goods to\nCapt. Stern upstairs in\lSlateport's Oceanic Museum.")},
    {FLAG_HIDE_SLATEPORT_CITY_BRAWLY, COMPOUND_STRING("Speak to Brawly outside the\nOceanic Museum in Slateport.\lHe will return to his Gym\lin Dewford Town."), COMPOUND_STRING("Speak to Brawly outside\nSlateport's Oceanic Museum\lso he returns to Dewford's Gym.")},
    {FLAG_BADGE02_GET, COMPOUND_STRING("Go south from Slateport to\nBriney's boat on Route 109.\lChoose Dewford, then enter\lits Gym and defeat Brawly."), COMPOUND_STRING("Take Briney's boat to Dewford\nand defeat Brawly in its Gym.")},
    {FLAG_BADGE03_GET, COMPOUND_STRING("Enter Mauville's Gym and\ndefeat Wattson for Badge\lthree."), COMPOUND_STRING("Defeat Wattson in Mauville's\nGym for your third Badge.")},
    {FLAG_RECEIVED_HM06, COMPOUND_STRING("Visit the Rock Smash Dude in\nMauville's southeast house.\lHe grants your Rock Smash license.\pWith the Dynamo Badge and a\npartner able to learn it, you\lcan clear Route 111's rocks.\pYou don't need to teach the move."), COMPOUND_STRING("Get the Rock Smash license\nfrom the Rock Smash Dude in\lMauville's southeast house.")},
    {FLAG_MET_ARCHIE_METEOR_FALLS, COMPOUND_STRING("Go north from Mauville on\nRoute 111. Smash the rocks,\lthen go west onto Route 112.\pThe cable car is blocked. Go\nthrough Fiery Path, then\lfollow Route 113 west to\lFallarbor Town.\pGo west on Route 114 into\nMeteor Falls. Approach Aqua\land Magma inside the cave."), COMPOUND_STRING("Find Aqua and Magma inside\nMeteor Falls on Route 114,\lwest of Fallarbor.")},
    {FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, COMPOUND_STRING("Return through Fallarbor and\nFiery Path to Route 112. Ride\lthe now-open cable car.\pGo north on Mt. Chimney past\nthe battling teams. Defeat\lMaxie at the meteor machine."), COMPOUND_STRING("Ride Route 112's cable car.\nDefeat Maxie at Mt. Chimney's\lmeteor machine.")},
    {FLAG_BADGE04_GET, COMPOUND_STRING("Leave Mt. Chimney south down\nJagged Pass. At Route 112, go\lwest to Lavaridge Town.\pEnter its Gym and defeat\nFlannery for Badge four."), COMPOUND_STRING("Go down Jagged Pass and west\nto Lavaridge. Defeat Flannery\lin its Gym for Badge four.")},
    {FLAG_BADGE05_GET, COMPOUND_STRING("Return to Petalburg's Gym.\nDefeat a trainer in each room\lto open a path to Norman.\pSpeak to Norman and defeat\nhim for Badge five."), COMPOUND_STRING("Return to Petalburg's Gym and\ndefeat Norman for Badge five.")},
    {FLAG_RECEIVED_HM03, COMPOUND_STRING("Visit Wally's house beside the\nPetalburg Gym. His father\lgrants your Surf license.\pWith the Balance Badge and a\npartner able to learn Surf,\lyou can cross Route 118's river.\pYou don't need to teach the move."), COMPOUND_STRING("Get the Surf license from\nWally's father in the house\lbeside Petalburg's Gym.")},
    {FLAG_HIDE_ROUTE_119_TEAM_AQUA, COMPOUND_STRING("Go east from Mauville onto\nRoute 118. Surf across the\lriver, then follow Route 119\lnorth to the Weather Institute.\pEnter the Institute, climb\nupstairs and defeat Aqua\lAdmin Shelly to clear the\lblocked bridge outside."), COMPOUND_STRING("Free the Weather Institute\non Route 119. Defeat Aqua's\lShelly upstairs.")},
    {FLAG_RECEIVED_DEVON_SCOPE, COMPOUND_STRING("From Fortree, go east onto\nRoute 120 and south to the\lbridge. Speak to Steven and\laccept his Scope demonstration.\pMake room for the Devon Scope\nif he says your Bag is full."), COMPOUND_STRING("Find Steven on Route 120's\nbridge, east of Fortree.\lAccept his Devon Scope lesson.")},
    {FLAG_KECLEON_FLED_FORTREE, COMPOUND_STRING("Return to Fortree's Gym.\nFace its invisible entrance\lblocker and press A to use\lthe Devon Scope and clear it."), COMPOUND_STRING("Face Fortree Gym's invisible\nblocker and press A to clear\lit with the Devon Scope.")},
    {FLAG_BADGE06_GET, COMPOUND_STRING("Enter Fortree's Gym and\ndefeat Winona for Badge six."), COMPOUND_STRING("Defeat Winona in Fortree's Gym\nfor your sixth Badge.")},
    {FLAG_RECEIVED_RED_OR_BLUE_ORB, COMPOUND_STRING("Go south along Route 120,\nthen east onto Route 121.\lAt the Mt. Pyre pier, Surf\lsouth across Route 122.\pEnter Mt. Pyre, take the west\nexit on 1F, then climb the\loutside paths to the summit.\lApproach Archie there."), COMPOUND_STRING("Go to Mt. Pyre's summit,\nsouth of Route 121, and stop\lAqua's raid.")},
    {FLAG_RECEIVED_HM04, COMPOUND_STRING("You need the Strength license\nfor Team Magma's hideout.\pReturn to Rusturf Tunnel, west\nof Verdanturf. Use Rock Smash\lto reunite the couple inside.\pThe man grants Strength. Your\nHeat Badge and a compatible\lpartner let you move boulders."), COMPOUND_STRING("Smash Rusturf Tunnel's rocks\nto reunite the couple. The\lman grants your Strength license.")},
    {FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, COMPOUND_STRING("Ride Route 112's cable car\nto Mt. Chimney. Walk south\ldown Jagged Pass with the\lMagma Emblem in your Bag.\pApproach the rock wall where\nthe Magma guard stood. Enter\lthe hideout and use Strength\lto move its boulders.\pFind Maxie beside Groudon\ndeep inside and defeat him."), COMPOUND_STRING("Use the Magma Emblem to enter\nJagged Pass's hideout. Defeat\lMaxie beside Groudon inside.")},
    {FLAG_MET_TEAM_AQUA_HARBOR, COMPOUND_STRING("Return to Slateport's harbor\nin the northeast of the city.\lSpeak to Capt. Stern outside,\lthen follow him inside to\lsee Aqua steal the submarine."), COMPOUND_STRING("Speak to Capt. Stern outside\nSlateport's northeast harbor,\lthen follow him inside.")},
    {FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, COMPOUND_STRING("Go to Lilycove's northeast\nbeach. Surf into Aqua's cave\lentrance in the cove.\pUse its warp panels to reach\nthe submarine chamber. Defeat\lAqua Admin Matt there."), COMPOUND_STRING("Enter Aqua's hideout in\nLilycove's northeast cove.\lDefeat Matt by the submarine.")},
    {FLAG_BADGE07_GET, COMPOUND_STRING("Surf east from Lilycove across\nRoute 124 to Mossdeep City.\lEnter its Gym and defeat\lTate and Liza for Badge seven."), COMPOUND_STRING("Surf east across Route 124\nto Mossdeep. Defeat Tate and\lLiza in its Gym.")},
    {FLAG_DEFEATED_MAGMA_SPACE_CENTER, COMPOUND_STRING("Enter the Space Center on\nMossdeep's east hill. Defeat\lMagma's grunts on both floors.\pSpeak to Steven upstairs and\njoin his battle against\lMaxie and Tabitha."), COMPOUND_STRING("Help Steven upstairs in\nMossdeep's Space Center.\lDefeat Maxie and Tabitha.")},
    {FLAG_RECEIVED_HM08, COMPOUND_STRING("Visit Steven's house in\nnorthwest Mossdeep. He grants\lyour Dive license.\pWith Badge seven and a partner\nable to learn Dive, press A\lon dark water to dive."), COMPOUND_STRING("Get your Dive license from\nSteven at his house in\lnorthwest Mossdeep.")},
    {FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, COMPOUND_STRING("Surf south from Mossdeep on\nRoute 127 to Route 128. Dive\lin its dark water and enter\lthe underwater cave.\pSurface by Aqua's submarine.\nEnter Seafloor Cavern and use\lSurf, Strength and Rock Smash\lto reach Archie and defeat him."), COMPOUND_STRING("Dive into Route 128's cavern.\nSurface by the submarine,\lthen find and defeat Archie.")},
    {FLAG_WALLACE_GOES_TO_SKY_PILLAR, COMPOUND_STRING("Surf to Route 126 and Dive\nin the dark water around\lSootopolis's crater. Enter\lthe underwater opening, then\lsurface inside the city.\pSpeak to Steven in the city's\nnorthwest. Follow him to the\lCave of Origin, go down to\lWallace and choose Sky Pillar."), COMPOUND_STRING("Meet Steven in Sootopolis,\nthen Wallace in the Cave of\lOrigin. Choose Sky Pillar.")},
};

// Sky Pillar and the calm that follows (the Sootopolis crisis, see
// SOOTOPOLIS_STATE_* in constants/quest_states.h).
static const u8 sText_GuideSkyPillar[] = _("Surf south from Route 128,\nthen west along Routes 129\land 130 to Route 131.\pFind the opening in Route\n131's north rocks. Surf in\land follow the cave to Sky\lPillar. Wallace opens its door.\pClimb to the roof and approach\nRayquaza. Then return to\lSootopolis to calm the crisis.");
static const u8 sText_GuideLeaders[] = _("Return to Sootopolis and speak\nto both Maxie and Archie by\lthe Gym. Both must speak\lbefore they leave.");
static const u8 sText_GuideMaxie[] = _("Speak to Maxie beside the\nSootopolis Gym. You have\lalready spoken to Archie.");
static const u8 sText_GuideArchie[] = _("Speak to Archie beside the\nSootopolis Gym. You have\lalready spoken to Maxie.");
static const u8 sText_GuideWaterfall[] = _("Speak to Wallace outside the\nSootopolis Gym. He grants\lyour Waterfall license, then\lmoves aside for you to enter.");
static const u8 sText_GuideJuan[] = _("Juan leads the Sootopolis Gym.\nWin your last Badge there!");
static const u8 sText_GuideLeague[] = _("Surf east from Route 128 to\nEver Grande. With Badge eight\land a Waterfall-capable\lpartner, climb its waterfall.\pEnter Victory Road. Bring\nSurf, Strength and Rock Smash\lpartners. Cross its caves to\lthe north exit and the League.\pWin the Elite Four battles,\nthen defeat Champion Wallace.");
static const u8 sText_GuidePetalburg[] = _("Go west from Oldale on Route\n102 to Petalburg City. Enter\lthe Gym and speak to Norman.\pHelp Wally catch his Pokémon,\nthen finish speaking to\lNorman back at the Gym.");
static const u8 sText_GuideRoute110Rival[] = _("Follow Route 110 north from\nSlateport. Your rival waits\lon the road to Mauville.\pPrepare with the Leveler and\nMove Tutor before the battle.");
static const u8 sText_GuideMauvilleWally[] = _("Wally is outside the Mauville\nGym. Speak to him and accept\lhis battle, then face Wattson.");
static const u8 sText_GuideRoute119Rival[] = _("Cross the bridge east of the\nWeather Institute, then follow\lRoute 119 north. Defeat your\lrival on the path to Fortree.");
static const u8 sText_GuideMagmaEmblem[] = _("Speak to the old lady on\nMt. Pyre's summit for the\lMagma Emblem that opens the\lhideout on Jagged Pass.\pIf your Bag was full, make\nroom and speak to her again.");


static const u8 sText_GuideInitialTools[] = _("Enter Oldale's Poké Mart and\nstep inside the doorway. The\lvisiting nurse introduces\lyour tools and the shop clerk.\pIf tools could not fit, make\nroom and return to her, or\lspeak to a Pokémon Center\lnurse for the missing tools.");
static const u8 sText_GuideLevelerPC[] = _("Your Leveler is in the PC.\nAt a Center, open your own PC.\lChoose Item Storage, then\lWithdraw Item\lto put it back in your Bag.\pOpen Bag, then Key Items.\nChoose Leveler and USE it,\lthen meet your rival north\lof Oldale on Route 103.");
static const u8 sText_GuideInitialLeveler[] = _("Open Bag, then Key Items.\nChoose Leveler and USE it to\lraise every partner to the cap.\pThen go north from Oldale to\nRoute 103 and speak to your\lrival for your first battle.");
static const u8 sText_GuideWoods[] = _("Leave Petalburg west on Route\n104. Enter Petalburg Woods and\lfollow the path north.\pHelp the Devon researcher by\ndefeating the Aqua grunt,\lthen leave the Woods north\land continue to Rustboro.");
static const u8 sText_GuideGoodsRetry[] = _("You defeated Rusturf Tunnel's\nthief, but his Devon Goods\lstill need room in your Bag.\pMake room, then return to the\nAqua grunt beside Peeko in\lRusturf Tunnel and speak again.");
static const u8 sText_GuideNormanRing[] = _("Return to Petalburg's Gym.\nDefeat a trainer in each room\lto reach Norman at the back.\pSpeak to Norman for your Mega\nRing and partner stones. Make\lroom if your Bag is full.\pSpeak to him again to start\nyour fifth Gym battle.");
static const u8 sText_GuideLeagueWally[] = _("Surf east from Route 128 to\nEver Grande. Climb the falls\lwith a Waterfall-capable\lpartner, then enter Victory Road.\pWally challenges you just\ninside. Defeat him, then cross\lthe cave with Surf, Strength\land Rock Smash partners.\pThe north exit leads to the\nLeague. Defeat the Elite Four\land Champion Wallace.");

static const u8 *GetCenterGuideStoryText(bool32 detailed)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCenterGuideStory); i++)
    {
        if (FlagGet(sCenterGuideStory[i].flag))
            continue;
        // These mandatory conversations use native variables or trainer
        // flags, rather than another duplicated guide-progress state.
        switch (sCenterGuideStory[i].flag)
        {
        case FLAG_DEFEATED_RIVAL_ROUTE103:
            if (!CheckBagHasItem(ITEM_LEVELER, 1) && !CheckPCHasItem(ITEM_LEVELER, 1))
                return detailed ? sText_GuideInitialTools : COMPOUND_STRING("Get your travel tools from\nthe nurse in Oldale's Mart\lor any Pokémon Center.");
            IsPlayerPartyBelowLevelCap();
            if (gSpecialVar_Result)
            {
                if (!CheckBagHasItem(ITEM_LEVELER, 1))
                    return detailed ? sText_GuideLevelerPC : COMPOUND_STRING("Withdraw your Leveler from\nyour PC's Item Storage, then\lUSE it from Bag, Key Items.");
                return detailed ? sText_GuideInitialLeveler : COMPOUND_STRING("USE the Leveler in Bag, Key\nItems before your Route 103\lbattle. Raise the whole party.");
            }
            break;
        case FLAG_BADGE01_GET:
            if (VarGet(VAR_PETALBURG_GYM_STATE) < 2)
                return detailed ? sText_GuidePetalburg : COMPOUND_STRING("Visit Norman in Petalburg's\nGym and help Wally catch\lhis Pokémon.");
            if (VarGet(VAR_PETALBURG_WOODS_STATE) == 0)
                return detailed ? sText_GuideWoods : COMPOUND_STRING("Help the Devon researcher in\nPetalburg Woods by defeating\nthe Aqua grunt.");
            break;
        case FLAG_RECOVERED_DEVON_GOODS:
            if (HasTrainerBeenFought(TRAINER_GRUNT_RUSTURF_TUNNEL))
                return detailed ? sText_GuideGoodsRetry : COMPOUND_STRING("Make Bag space, then speak\nto the thief beside Peeko\lin Rusturf Tunnel again.");
            break;
        case FLAG_BADGE05_GET:
            if (!FlagGet(FLAG_SYS_RECEIVED_KEYSTONE))
                return detailed ? sText_GuideNormanRing : COMPOUND_STRING("Reach Norman in Petalburg's\nGym for your Mega Ring. Make\lBag space, then speak again.");
            break;
        case FLAG_BADGE03_GET:
            if (VarGet(VAR_ROUTE110_STATE) == 0)
                return detailed ? sText_GuideRoute110Rival : COMPOUND_STRING("Defeat your rival on Route\n110, between Slateport and\lMauville.");
            if (!FlagGet(FLAG_DEFEATED_WALLY_MAUVILLE))
                return detailed ? sText_GuideMauvilleWally : COMPOUND_STRING("Speak to Wally outside the\nMauville Gym and defeat him.");
            break;
        case FLAG_RECEIVED_DEVON_SCOPE:
            if (VarGet(VAR_ROUTE119_STATE) == 0)
                return detailed ? sText_GuideRoute119Rival : COMPOUND_STRING("Defeat your rival beyond the\nWeather Institute on the\lRoute 119 path to Fortree.");
            break;
        case FLAG_RECEIVED_RED_OR_BLUE_ORB:
            if (VarGet(VAR_MT_PYRE_STATE) >= 1)
                return detailed ? sText_GuideMagmaEmblem : COMPOUND_STRING("Accept the Magma Emblem from\nMt. Pyre's old lady. If your\lBag was full, speak again.");
            break;
        }
        return detailed ? sCenterGuideStory[i].text : sCenterGuideStory[i].objective;
    }
    if (!HasRayquazaCalmedSootopolis())
        return detailed ? sText_GuideSkyPillar : COMPOUND_STRING("Climb Sky Pillar off north\nRoute 131 and wake Rayquaza.\lThen return to Sootopolis.");
    if (!FlagGet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE))
    {
        if (FlagGet(FLAG_MET_ARCHIE_SOOTOPOLIS))
            return detailed ? sText_GuideMaxie : COMPOUND_STRING("Speak to Maxie beside the\nSootopolis Gym. Archie has\lalready spoken to you.");
        if (FlagGet(FLAG_MET_MAXIE_SOOTOPOLIS))
            return detailed ? sText_GuideArchie : COMPOUND_STRING("Speak to Archie beside the\nSootopolis Gym. Maxie has\lalready spoken to you.");
        return detailed ? sText_GuideLeaders : COMPOUND_STRING("Speak to both Maxie and\nArchie beside the Sootopolis\lGym so they leave.");
    }
    if (!FlagGet(FLAG_RECEIVED_HM07))
        return detailed ? sText_GuideWaterfall : COMPOUND_STRING("Speak to Wallace outside\nSootopolis's Gym for your\lWaterfall license.");
    if (!FlagGet(FLAG_BADGE08_GET))
        return detailed ? sText_GuideJuan : COMPOUND_STRING("Defeat Juan in Sootopolis's\nGym for your final Badge.");
    if (!FlagGet(FLAG_DEFEATED_WALLY_VICTORY_ROAD))
        return detailed ? sText_GuideLeagueWally : COMPOUND_STRING("Enter Victory Road at Ever\nGrande. Defeat Wally, then\lcross the cave to the League.");
    return detailed ? sText_GuideLeague : COMPOUND_STRING("Challenge the Elite Four and\nChampion Wallace at the League\lin north Ever Grande.");
}


// These are the same trainer flags GetEmeraldChampionsFinaleStage reads.
// Door positions come from SSTidalCorridor/Rooms map warps, not cabin names
// inferred from the occupants. The north row has no numbered signs.
static const struct
{
    u16 trainer;
    const u8 *text;
} sCenterGuideVoyage[] =
{
    {TRAINER_COLTON, COMPOUND_STRING("On the S.S. Tidal, enter the\nfar-left door on the north\lside of the corridor. Speak\lto Colton and defeat him.")},
    {TRAINER_MICAH, COMPOUND_STRING("On the S.S. Tidal, enter\nCabin 4: the far-right door\lon the south side of the\lcorridor. Defeat Micah inside.")},
    {TRAINER_THOMAS, COMPOUND_STRING("On the S.S. Tidal, enter the\nthird door from the left on\lthe north side of the corridor.\lDefeat Thomas inside.")},
    {TRAINER_LEA_AND_JED, COMPOUND_STRING("On the S.S. Tidal, enter\nCabin 1: the far-left door\lon the south side of the\lcorridor. Speak to Lea or\lJed and defeat their team.")},
    {TRAINER_NAOMI, COMPOUND_STRING("On the S.S. Tidal, enter the\nsecond door from the left on\lthe north side of the corridor.\lDefeat Naomi inside.")},
};

static const u8 *GetCenterGuideFinaleText(bool32 detailed)
{
    switch (GetEmeraldChampionsFinaleStage())
    {
    case EC_FINALE_WALLY:
        if (!detailed)
            return COMPOUND_STRING("Defeat Wally's Champion team\nat Victory Road's north exit,\ljust south of the League.");
        return COMPOUND_STRING("Go to the Pokémon League at\nnorth Ever Grande. Walk south\linto Victory Road's north\lexit and speak to Wally.\pDefeat his Champion challenge\nto begin your final expedition.");
    case EC_FINALE_VOYAGE:
        if (!FlagGet(FLAG_RECEIVED_SS_TICKET)
         && !CheckBagHasItem(ITEM_SS_TICKET, 1) && !CheckPCHasItem(ITEM_SS_TICKET, 1))
        {
            if (!FlagGet(FLAG_EC_EARNED_SS_TICKET))
                return COMPOUND_STRING("Return to your own house in\nLittleroot. Go downstairs to\lreceive the S.S. Ticket from\lyour father.");
            return COMPOUND_STRING("Your S.S. Ticket is still\nwaiting for room. Make room\lin your Bag's Key Items, then\lspeak to any Center nurse.\pSpeak to her for your earned\ntravel documents.");
        }
        if (!detailed)
        {
            for (u32 i = 0; i < ARRAY_COUNT(sCenterGuideVoyage); i++)
                if (!HasTrainerBeenFought(sCenterGuideVoyage[i].trainer))
                    return sCenterGuideVoyage[i].text;
        }
        // Append the first still-unbeaten cabin team, even if the player
        // beat the other cabins in a different order or sailed several times.
        StringCopy(gStringVar4, COMPOUND_STRING("Board at Slateport's northeast\nharbor or Lilycove's southwest\lharbor. Choose the other city\lto sail aboard the S.S. Tidal.\p"));
        for (u32 i = 0; i < ARRAY_COUNT(sCenterGuideVoyage); i++)
        {
            if (!HasTrainerBeenFought(sCenterGuideVoyage[i].trainer))
            {
                StringAppend(gStringVar4, sCenterGuideVoyage[i].text);
                break;
            }
        }
        StringAppend(gStringVar4, COMPOUND_STRING("\pThe bed in Cabin 2 heals you\nand advances the voyage. If\lyou reach port, board again\lto finish the remaining teams."));
        return gStringVar4;
    case EC_FINALE_STEVEN:
        if (!detailed)
        {
            if (HasTrainerBeenFought(TRAINER_STEVEN))
                return COMPOUND_STRING("Make Bag space, then collect\nyour Aurora Ticket from Steven\lin his Meteor Falls cave.");
            return COMPOUND_STRING("Defeat Steven deep in Meteor\nFalls for your Aurora Ticket.\lBring Surf and Waterfall partners.");
        }
        if (HasTrainerBeenFought(TRAINER_STEVEN))
            return COMPOUND_STRING("Steven still has your Aurora\nTicket. Make room in your\lBag, then speak to him again\lin his Meteor Falls cave.\pClimb the waterfall, enter the\nroom above it. Take its west\lladder down, then take B1F's\lfar-west ladder up to his cave.");
        return COMPOUND_STRING("Go west from Fallarbor along\nRoute 114 into Meteor Falls.\lBring Surf and Waterfall\lpartners and climb the falls.\pEnter the room above the falls.\nTake its west ladder down,\lthen take B1F's far-west ladder\lup. Enter the cave beside it.\pSpeak to Steven and defeat him\nfor your Aurora Ticket. Make\lroom and speak again if full.");
    case EC_FINALE_DEOXYS:
        if (!detailed)
            return COMPOUND_STRING("Take Lilycove's ferry to Birth\nIsland. Solve the triangle,\lthen catch or defeat Deoxys.\pFollow it using as few steps\nas you can. Save first: a\lknockout loses it forever.");
        return COMPOUND_STRING("At Lilycove's southwest harbor,\nspeak to the ferry attendant\land choose Birth Island.\lFollow the path to the triangle.\pEntering the island resets it.\nStand below it and press A\nfacing up. After each move,\lfollow these steps and press A\lfacing the direction shown.\pLeft 3, down 1: face left.\nRight 3, up 5: face up.\lRight 3, down 5: face right.\lLeft 5, up 3: face left.\pRight 4: face right.\nLeft 2, down 2: face down.\lLeft 3, down 1: face left.\lRight 6: face right.\pLeft 3: face down.\nUp 3: face up.\lExtra steps reset the puzzle.\pCatch or defeat Deoxys. A\nknockout loses it forever.\lFleeing lets you try again.");
    case EC_FINALE_BUFFEL:
        if (!detailed)
            return COMPOUND_STRING("Defeat Buffel upstairs in\nLilycove's Cove Lily Motel,\lsoutheast of its Pokémon Center.");
        return COMPOUND_STRING("Enter the Cove Lily Motel,\nsoutheast of Lilycove's Center.\lClimb its stairs and speak\lto Buffel upstairs.\pDefeat him to complete your\nfinal trial.");
    default:
        if (!detailed)
            return COMPOUND_STRING("Your final trial is complete!\nThe League stays open whenever\lyou want another challenge.");
        return COMPOUND_STRING("You completed the League and\nBuffel's final trial!\pThe League remains open at\nnorth Ever Grande whenever\lyou want to battle again.");
    }
}

static void BufferCenterGuideInstructions(bool32 detailed)
{
    const u8 *text = FlagGet(FLAG_SYS_GAME_CLEAR)
        ? GetCenterGuideFinaleText(detailed) : GetCenterGuideStoryText(detailed);
    if (text != gStringVar4)
        StringCopy(gStringVar4, text);
    gSpecialVar_Result = TRUE;
}

void BufferCenterGuideDirections(void)
{
    BufferCenterGuideInstructions(TRUE);
}

void BufferCenterGuideObjective(void)
{
    BufferCenterGuideInstructions(FALSE);
}
