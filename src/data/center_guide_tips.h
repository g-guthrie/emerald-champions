// Pokémon Center local guide: optional content near each city.
// TIP(city, minimum badges, flag that must be set, flag that retires the
// tip, extra state check, text). Each tip states its own requirements in
// prose; the guide never appends generated lines. Keep every line within
// 200px (test/legendary_sign_pipeline.c measures them).

#include "constants/emerald_champions.h"

#define TIP(city, badges, required, done, check_, text_) \
    {MAPSEC_##city, required, done, badges, CENTER_GUIDE_CHECK_##check_, text_}

TIP(OLDALE_TOWN, 0, 0, 0, NONE, sTip_MoveTutor),

TIP(PETALBURG_CITY, 0, 0, FLAG_RECEIVED_WAILMER_PAIL, NONE, COMPOUND_STRING("The flower shop on Route 104\ngives out a Wailmer Pail and a\lHarvest Pouch for growing Berries.")),

TIP(RUSTBORO_CITY, 0, 0, 0, NONE, sTip_MoveTutor),
TIP(RUSTBORO_CITY, 4, 0, 0, NONE, COMPOUND_STRING("Devon's lab on 2F can revive\nfossils. Many lie buried under\lthe Route 111 desert.")),
TIP(RUSTBORO_CITY, 0, FLAG_IS_CHAMPION, 0, ARCEUS_GIFT, COMPOUND_STRING("You're the Champion now!\nDevon's dream researcher on 2F\lhas an extraordinary gift\lwaiting for you.")),

TIP(SLATEPORT_CITY, 0, 0, FLAG_RECEIVED_6_SODA_POP, NONE, COMPOUND_STRING("Beat all three Trainers in the\nSeashore House on Route 109,\lsouth of town, and the owner\lwill treat you to Soda Pop.")),
TIP(SLATEPORT_CITY, 0, 0, 0, TRICK_HOUSE, sTip_TrickHouse),
TIP(SLATEPORT_CITY, 0, 0, 0, NONE, COMPOUND_STRING("The Battle Tent here lends you\nPokémon for Double Battles.\pWin three in a row to earn an\nitem like a Metal Coat or a\lLinking Cord.")),
TIP(SLATEPORT_CITY, 0, 0, 0, NONE, COMPOUND_STRING("A retired Pokéblock chef lives\nin the Name Rater's house. Bring\lone Berry of the kind he asks\lfor to change a Pokémon's Nature.")),
TIP(SLATEPORT_CITY, 5, 0, 0, ODD_KEYSTONE, COMPOUND_STRING("An Odd Keystone lies in the\nruins beneath the Route 111 desert.\pToss it into the trash can in\nthe Abandoned Ship's storage\lroom on Route 108, with a\lLickitung or Slugma along.\pA Spiritomb will appear. If you\nrun from it, you keep the\lKeystone to try again.\pThe storage room's key lies in\nthe ship's captain's office.")),
TIP(SLATEPORT_CITY, 7, 0, FLAG_EXCHANGED_SCANNER, NONE, COMPOUND_STRING("Dive beside the Abandoned Ship\non Route 108 to find its hidden floor.\pCapt. Stern at the harbor\ntrades Bottle Caps for the\lScanner you'll find there.")),

TIP(MAUVILLE_CITY, 3, 0, FLAG_RECEIVED_HM06, NONE, COMPOUND_STRING("The Rock Smash Dude lives in a\nhouse here in Mauville. Visit\lhim for the Rock Smash license.")),
TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_LOST, sTip_BlobLost),
TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_CHASE, sTip_BlobChase),
TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_FOUND, sTip_BlobFound),
TIP(MAUVILLE_CITY, 0, 0, FLAG_RECEIVED_LIFE_ORB, NONE, COMPOUND_STRING("The Winstrate family lives on\nRoute 111. Beat all four of\lthem in a row, and they'll\lreward you at their house.")),
TIP(MAUVILLE_CITY, 0, 0, 0, NONE, COMPOUND_STRING("The Game Corner trades Coins\nfor rare Pokémon, including\lstarters from every region.\pYou'll need a Coin Case. The\nlady next door trades one for\lan Ice Stone from Slateport.")),
TIP(MAUVILLE_CITY, 0, 0, 0, TRICK_HOUSE, sTip_TrickHouse),
TIP(MAUVILLE_CITY, 0, 0, 0, NONE, COMPOUND_STRING("Mauville's Iconic Move Tutor\nis in the house west of the Mart.\pHe teaches special moves that\nother tutors can't. More Badges\lopen more lessons.\pHis neighbor has a catalogue\nlisting each Pokémon's lessons\land the Badges they require.")),
TIP(MAUVILLE_CITY, 5, 0, FLAG_GOT_TM24_FROM_WATTSON, NONE, COMPOUND_STRING("After your fifth Badge, Wattson\nwaits outside his Gym. He needs\lhelp with New Mauville's generator.\pNew Mauville is a short Surf\nfrom Route 110. A Rotom lives\linside, so save before you go.")),
TIP(MAUVILLE_CITY, 5, 0, FLAG_ROUTE118_GYARADOSITE, NONE, COMPOUND_STRING("A fisherman across the river\non Route 118 loves Magikarp.\pBeat his team with a party of\nsix Magikarp, and he'll give\lyou a Gyaradosite.")),
TIP(MAUVILLE_CITY, 5, 0, 0, NONE, sTip_BerryMaster),
TIP(MAUVILLE_CITY, 4, 0, 0, NONE, sTip_Desert),

TIP(VERDANTURF_TOWN, 0, 0, 0, NONE, COMPOUND_STRING("The Day Care on Route 117, east\nof town, looks after two\lPokémon. A pair may even find\lan Egg.")),
TIP(VERDANTURF_TOWN, 0, 0, FLAG_RECEIVED_AUDINO, NONE, COMPOUND_STRING("A girl in Verdanturf Meadow,\njust south of town, gives an\lAudino to caring Trainers.")),
TIP(VERDANTURF_TOWN, 0, FLAG_RECEIVED_AUDINO, 0, NONE, COMPOUND_STRING("Verdanturf Meadow, just south\nof town, is home to many\lPsychic and Fairy Pokémon.")),
TIP(VERDANTURF_TOWN, 0, 0, FLAG_RECEIVED_HM04, NONE, COMPOUND_STRING("Rusturf Tunnel, west of town,\nleads to Rustboro. Smash the\lrocks inside, and a man there\lwill grant you Strength.")),

TIP(LAVARIDGE_TOWN, 0, 0, 0, BLOB_CHASE, sTip_BlobChase),
TIP(LAVARIDGE_TOWN, 4, 0, 0, NONE, sTip_Desert),
TIP(LAVARIDGE_TOWN, 0, 0, 0, NONE, COMPOUND_STRING("Any partner that has had Pokérus\ncan soak for 25 consecutive steps\lin the hot-spring water.\pThen speak to the woman inside.\nShe previews a permanent treatment\lthat ends spreading.\pIts favored Nature stat keeps\n15%; the other gains 5%.\lNeutral Natures stay neutral.")),

TIP(FALLARBOR_TOWN, 5, 0, 0, NONE, COMPOUND_STRING("After five Badges, the Fossil\nManiac's tunnel on Route 114\lopens into ruins under the\ldesert, full of fossils.\pDevon's lab in Rustboro can\nrevive them.")),
TIP(FALLARBOR_TOWN, 0, 0, 0, NONE, COMPOUND_STRING("Evie in Fallarbor previews\nand adjusts a whole EV spread.\lAdded EVs cost ¥" STR(EC_EVIE_PRICE_PER_EV) " each;\llowering is free.\pIvy lowers Attack or Speed IVs\nfor " STR(EC_IV_CHANGE_CAP_COST) " Bottle Cap. For " STR(EC_HIDDEN_POWER_CAP_COST) " Caps,\lshe changes Hidden Power's type.")),

TIP(FORTREE_CITY, 0, FLAG_VISITED_FORTREE_CITY, FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, NONE, COMPOUND_STRING("Strange storms bring visiting\nlegends to some routes. The\lWeather Institute on Route 119\ltracks where they are.")),

TIP(LILYCOVE_CITY, 0, 0, 0, NONE, COMPOUND_STRING("Draw a lottery ticket at the\nDepartment Store every day.\lMatch your Pokémon's ID numbers\lto win.\pThe top prize is a Master Ball!")),
TIP(LILYCOVE_CITY, 0, 0, 0, NONE, COMPOUND_STRING("The Safari Zone on Route 121\nis home to rare Pokémon.\pYou'll need a Pokéblock Case.\nThe Contest Hall here in town\lgives one out.")),
TIP(LILYCOVE_CITY, 0, 0, 0, NONE, sTip_BerryMaster),

TIP(MOSSDEEP_CITY, 0, 0, FLAG_SHOALCAVE_SLOWBRONITE, NONE, COMPOUND_STRING("Shoal Cave, on Route 125, floods\nand drains with the tides.\pBring the old man inside four\nShoal Salt and four Shoal\lShells for a Shell Bell.\pHis first Shell Bell comes\nwith a Slowbronite.")),
TIP(MOSSDEEP_CITY, 0, FLAG_SYS_GAME_CLEAR, FLAG_RECEIVED_MELTAN, NONE, COMPOUND_STRING("Steven left a Poké Ball in his\nhouse here. The Meltan inside\lis yours to keep.")),
TIP(MOSSDEEP_CITY, 0, FLAG_SYS_GAME_CLEAR, FLAG_RECEIVED_MYSTIC_TICKET, NONE, COMPOUND_STRING("Cynthia is visiting a house\nhere in Mossdeep. Win a battle\lwith her for the Mystic Ticket\lto Navel Rock.")),

TIP(SOOTOPOLIS_CITY, 0, FLAG_BADGE08_GET, 0, NONE, COMPOUND_STRING("Kiri, a girl here in town,\nhands out two Berries every day.")),
TIP(SOOTOPOLIS_CITY, 0, FLAG_SYS_GAME_CLEAR, 0, NONE, COMPOUND_STRING("Wallace waits in the Cave of\nOrigin, in Diancie's room.\lHe'd love a battle with the\lnew Champion.")),

TIP(PACIFIDLOG_TOWN, 0, 0, 0, VIAL_ROUTE133, COMPOUND_STRING("Blob's nurse is on Route 133,\nwest of town. Help Blob cross\lthe currents, and your Poké\lVial will hold another dose.")),

TIP(EVER_GRANDE_CITY, 0, FLAG_SYS_GAME_CLEAR, 0, NONE, COMPOUND_STRING("The Elite Four are always ready\nfor another match. Challenge\lthem whenever you like.")),

TIP(DEWFORD_TOWN, 0, 0, FLAG_ITEM_ROUTE119_BUTTERFRENITE, NONE, COMPOUND_STRING("Butterfree can Mega Evolve!\nLook for its stone on Route 119\lafter Norman gives you the\lMega Ring.")),
TIP(DEWFORD_TOWN, 0, 0, FLAG_ITEM_ROUTE_106_KINGLERITE, NONE, COMPOUND_STRING("Kingler can Mega Evolve, too!\nLook for its Mega Stone on\lRoute 106, north of Dewford.")),
TIP(FALLARBOR_TOWN, 5, 0, FLAG_ITEM_DESERT_UNDERPASS_FLYGONITE, NONE, COMPOUND_STRING("Flygon's Mega Stone lies in the\nDesert Underpass. After five\lBadges, explore the Fossil\lManiac's tunnel on Route 114.")),
TIP(MOSSDEEP_CITY, 0, 0, FLAG_ITEM_MOSSDEEP_CITY_MILOTICITE, NONE, COMPOUND_STRING("Milotic can Mega Evolve here!\nIts Mega Stone lies in Mossdeep.\lGive it to Milotic, then use\lyour Mega Ring in battle.")),
TIP(LILYCOVE_CITY, 0, 0, FLAG_ITEM_ROUTE_121_MACHAMPITE, NONE, COMPOUND_STRING("Machamp can Mega Evolve here!\nIts stone lies on Route 121,\lwest of Lilycove. Give it to\lMachamp for your Mega Ring.")),

#undef TIP
