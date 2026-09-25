// Pokémon Center local guide: optional content near each city.
// TIP(city, minimum badges, flag that must be set, flag that retires the
// tip, extra state check, text). Each tip states its own requirements in
// prose; the guide never appends generated lines. Keep every line within
// 200px (test/legendary_sign_pipeline.c measures them).

#define TIP(city, badges, required, done, check_, text_) \
    {MAPSEC_##city, required, done, badges, CENTER_GUIDE_CHECK_##check_, text_}

TIP(OLDALE_TOWN, 0, 0, 0, NONE, sTip_MoveTutor),

TIP(PETALBURG_CITY, 0, 0, FLAG_RECEIVED_WAILMER_PAIL, NONE, COMPOUND_STRING("The flower shop on Route 104\ngives out a Wailmer Pail and a\lHarvest Pouch for growing Berries.")),

TIP(RUSTBORO_CITY, 0, 0, 0, NONE, sTip_MoveTutor),
TIP(RUSTBORO_CITY, 4, 0, 0, NONE, COMPOUND_STRING("Devon's lab on 2F can revive\nfossils. Many lie buried under\lthe Route 111 desert.")),

TIP(SLATEPORT_CITY, 0, 0, FLAG_RECEIVED_6_SODA_POP, NONE, COMPOUND_STRING("Beat all three Trainers in the\nSeashore House on Route 109,\lsouth of town, and the owner\lwill treat you to Soda Pop.")),
TIP(SLATEPORT_CITY, 0, 0, 0, TRICK_HOUSE, sTip_TrickHouse),
TIP(SLATEPORT_CITY, 5, 0, 0, ODD_KEYSTONE, COMPOUND_STRING("An Odd Keystone lies in the\nruins beneath the Route 111 desert.\pToss it into the trash can in\nthe Abandoned Ship's storage\lroom on Route 108, with a\lLickitung or Slugma along.\pA Spiritomb will appear. If you\nrun from it, you keep the\lKeystone to try again.\pThe storage room's key lies in\nthe ship's captain's office.")),
TIP(SLATEPORT_CITY, 7, 0, FLAG_EXCHANGED_SCANNER, NONE, COMPOUND_STRING("Dive beside the Abandoned Ship\non Route 108 to find its hidden floor.\pCapt. Stern at the harbor\ntrades Bottle Caps for the\lScanner you'll find there.")),

TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_LOST, sTip_BlobLost),
TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_CHASE, sTip_BlobChase),
TIP(MAUVILLE_CITY, 0, 0, 0, BLOB_FOUND, sTip_BlobFound),
TIP(MAUVILLE_CITY, 0, 0, FLAG_RECEIVED_LIFE_ORB, NONE, COMPOUND_STRING("The Winstrate family lives on\nRoute 111. Beat all four of\lthem in a row, and they'll\lreward you at their house.")),
TIP(MAUVILLE_CITY, 0, 0, 0, NONE, COMPOUND_STRING("The Game Corner trades Coins\nfor rare Pokémon, including\lstarters from every region.\pYou'll need a Coin Case. The\nlady next door trades one for\lan Ice Stone from Slateport.")),
TIP(MAUVILLE_CITY, 0, 0, 0, TRICK_HOUSE, sTip_TrickHouse),
TIP(MAUVILLE_CITY, 5, 0, FLAG_GOT_TM24_FROM_WATTSON, NONE, COMPOUND_STRING("After your fifth Badge, Wattson\nwaits outside his Gym. He needs\lhelp with New Mauville's generator.\pNew Mauville is a short Surf\nfrom Route 110. A Rotom lives\linside, so save before you go.")),
TIP(MAUVILLE_CITY, 5, 0, FLAG_ROUTE118_GYARADOSITE, NONE, COMPOUND_STRING("A fisherman across the river\non Route 118 loves Magikarp.\pBeat his team with a party of\nsix Magikarp, and he'll give\lyou a Gyaradosite.")),
TIP(MAUVILLE_CITY, 5, 0, 0, NONE, sTip_BerryMaster),
TIP(MAUVILLE_CITY, 4, 0, 0, NONE, sTip_Desert),

TIP(VERDANTURF_TOWN, 0, 0, 0, NONE, COMPOUND_STRING("The Day Care on Route 117, east\nof town, looks after two\lPokémon. A pair may even find\lan Egg.")),
TIP(VERDANTURF_TOWN, 0, 0, FLAG_RECEIVED_AUDINO, NONE, COMPOUND_STRING("A girl in Verdanturf Meadow,\njust south of town, gives an\lAudino to caring Trainers.")),
TIP(VERDANTURF_TOWN, 0, FLAG_RECEIVED_AUDINO, 0, NONE, COMPOUND_STRING("The Audino girl in Verdanturf\nMeadow knows every Pokémon\lthat lives there. Ask her!")),

TIP(LAVARIDGE_TOWN, 0, 0, 0, BLOB_CHASE, sTip_BlobChase),
TIP(LAVARIDGE_TOWN, 4, 0, 0, NONE, sTip_Desert),

TIP(FALLARBOR_TOWN, 5, 0, 0, NONE, COMPOUND_STRING("After five Badges, the Fossil\nManiac's tunnel on Route 114\lopens into ruins under the\ldesert, full of fossils.\pDevon's lab in Rustboro can\nrevive them.")),
TIP(FALLARBOR_TOWN, 0, 0, 0, NONE, COMPOUND_STRING("Evie and Ivy live here in\nFallarbor. They raise EVs for\lmoney and change IVs for\lBottle Caps.")),

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

#undef TIP
