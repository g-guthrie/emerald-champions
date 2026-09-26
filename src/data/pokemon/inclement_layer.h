// Inclement layer: Inclement Emerald's deliberate base stat changes and its
// extra Abilities, read only by Pokemon that are not trainer-owned (wild,
// player-owned, gifts, eggs; see IsMonTrainerOwned). Trainer, partner, facility
// and Champions Circuit Pokemon never read it.
//
// Base stats: the whole line in HP/Atk/Def/SpA/SpD/Spe order, using Inclement's
// exact value for every stat it deliberately changed (raised or lowered) and
// gSpeciesInfo's value otherwise. A species whose line would lose base stat
// total keeps gSpeciesInfo entirely and has no entry.
// Abilities: slots 0-2 always mean gSpeciesInfo. extraAbility is an Inclement
// Ability the species lacks, offered as ability slot 3 (ABILITY_SLOT_INCLEMENT),
// a normal (non-hidden) slot. A Mega keeps its official Ability; its extra is
// reached only when the base Pokemon uses slot 3.
// Not layered: official changes after Inclement (Cresselia, Shiftry, Zygarde),
// Pumpkaboo size stats, species this game authored itself, and the four
// existing custom Abilities.
#define INCLEMENT_BASE_STATS(hp, atk, def, spAtk, spDef, speed) \
    .hasBaseStats = TRUE, .baseStats = { [STAT_HP] = hp, [STAT_ATK] = atk, [STAT_DEF] = def, \
                                         [STAT_SPATK] = spAtk, [STAT_SPDEF] = spDef, [STAT_SPEED] = speed }

static const struct InclementSpeciesLayer sInclementLayer[NUM_SPECIES] =
{
    [SPECIES_BULBASAUR]         = { .extraAbility = ABILITY_EFFECT_SPORE },
    [SPECIES_IVYSAUR]           = { .extraAbility = ABILITY_EFFECT_SPORE },
    [SPECIES_CHARMANDER]        = { .extraAbility = ABILITY_FLAME_BODY },
    [SPECIES_CHARMELEON]        = { .extraAbility = ABILITY_FLAME_BODY },
    [SPECIES_SQUIRTLE]          = { .extraAbility = ABILITY_OVERCOAT },
    [SPECIES_WARTORTLE]         = { .extraAbility = ABILITY_OVERCOAT },
    [SPECIES_BEEDRILL]          = { .extraAbility = ABILITY_MERCILESS },
    [SPECIES_BEEDRILL_MEGA]     = { INCLEMENT_BASE_STATS( 65, 150,  60,  15,  90, 145) }, // was Def 40, SpD 80; BST 495->525
    [SPECIES_PIDGEY]            = { INCLEMENT_BASE_STATS( 40,  35,  40,  45,  35,  56), .extraAbility = ABILITY_NO_GUARD }, // was Atk 45, SpA 35; BST 251->251
    [SPECIES_PIDGEOTTO]         = { INCLEMENT_BASE_STATS( 63,  50,  55,  60,  50,  71), .extraAbility = ABILITY_NO_GUARD }, // was Atk 60, SpA 50; BST 349->349
    [SPECIES_PIDGEOT_MEGA]      = { INCLEMENT_BASE_STATS( 83,  80,  95, 135,  80, 121) }, // was Def 80; BST 579->594
    [SPECIES_RATICATE]          = { .extraAbility = ABILITY_STRONG_JAW },
    [SPECIES_RATICATE_ALOLA]    = { .extraAbility = ABILITY_STRONG_JAW },
    [SPECIES_ARBOK]             = { INCLEMENT_BASE_STATS( 80, 105,  79,  65,  79,  80) }, // was HP 60, Atk 95, Def 69; BST 448->488
    [SPECIES_PICHU_SPIKY_EARED] = { .extraAbility = ABILITY_LIGHTNING_ROD },
    [SPECIES_VULPIX]            = { .extraAbility = ABILITY_PYROMANCY },
    [SPECIES_NINETALES]         = { INCLEMENT_BASE_STATS( 73,  76,  75,  91, 100, 100), .extraAbility = ABILITY_PYROMANCY }, // was SpA 81; BST 505->515
    [SPECIES_NINETALES_ALOLA]   = { INCLEMENT_BASE_STATS( 73,  67,  75,  91, 100, 109) }, // was SpA 81; BST 505->515
    [SPECIES_JIGGLYPUFF]        = { INCLEMENT_BASE_STATS(105,  45,  20,  65,  35,  20), .extraAbility = ABILITY_FUR_COAT }, // was HP 115, SpA 45, SpD 25; BST 270->290
    [SPECIES_WIGGLYTUFF]        = { INCLEMENT_BASE_STATS(120,  70,  45, 105,  60,  45), .extraAbility = ABILITY_FUR_COAT }, // was HP 140, SpA 85, SpD 50; BST 435->445
    [SPECIES_VILEPLUME]         = { INCLEMENT_BASE_STATS( 85,  60,  95, 120, 100,  50) }, // was HP 75, Atk 80, Def 85, SpA 110, SpD 90; BST 490->510
    [SPECIES_BELLOSSOM]         = { INCLEMENT_BASE_STATS( 90,  60,  95, 110, 100,  50), .extraAbility = ABILITY_DANCER }, // was HP 75, Atk 80, SpA 90; BST 490->505
    [SPECIES_PARASECT]          = { INCLEMENT_BASE_STATS( 80, 115,  95,  60,  95,  30) }, // was HP 60, Atk 95, Def 80, SpD 80; BST 405->475
    [SPECIES_PERSIAN]           = { INCLEMENT_BASE_STATS( 65,  70,  60, 105,  65, 115), .extraAbility = ABILITY_SUPER_LUCK }, // was SpA 65; BST 440->480
    [SPECIES_PERSIAN_ALOLA]     = { INCLEMENT_BASE_STATS( 65,  60,  70, 105,  65, 115) }, // was Def 60, SpA 75; BST 440->480
    [SPECIES_GOLDUCK]           = { INCLEMENT_BASE_STATS( 80, 102,  78, 105,  80,  85), .extraAbility = ABILITY_TOUGH_CLAWS }, // was Atk 82, SpA 95; BST 500->530
    [SPECIES_PRIMEAPE]          = { INCLEMENT_BASE_STATS( 65, 115,  60,  60,  70, 115) }, // was Atk 105, Spe 95; BST 455->485
    [SPECIES_POLIWRATH]         = { INCLEMENT_BASE_STATS( 90, 115,  95,  70,  90,  70) }, // was Atk 95; BST 510->530
    [SPECIES_POLITOED]          = { INCLEMENT_BASE_STATS(110,  75,  75, 100, 100,  70) }, // was HP 90, SpA 90; BST 500->530
    [SPECIES_GOLEM]             = { INCLEMENT_BASE_STATS( 90, 135, 130,  55,  65,  45) }, // was HP 80, Atk 120; BST 495->520
    [SPECIES_GOLEM_ALOLA]       = { INCLEMENT_BASE_STATS( 90, 135, 130,  55,  65,  45) }, // was HP 80, Atk 120; BST 495->520
    [SPECIES_PONYTA]            = { .extraAbility = ABILITY_RECKLESS },
    [SPECIES_FARFETCHD]         = { INCLEMENT_BASE_STATS( 52,  90,  55,  58,  62, 100), .extraAbility = ABILITY_SUPER_LUCK }, // was Spe 60; BST 377->417
    [SPECIES_SEEL]              = { .extraAbility = ABILITY_FUR_COAT },
    [SPECIES_MUK]               = { INCLEMENT_BASE_STATS(105, 125,  75,  65, 100,  50), .extraAbility = ABILITY_GOOEY }, // was Atk 105; BST 500->520
    [SPECIES_GASTLY]            = { .extraAbility = ABILITY_CURSED_BODY },
    [SPECIES_HAUNTER]           = { .extraAbility = ABILITY_CURSED_BODY },
    [SPECIES_STEELIX]           = { INCLEMENT_BASE_STATS( 75, 105, 200,  55,  75,  20), .extraAbility = ABILITY_STRONG_JAW }, // was Atk 85, SpD 65, Spe 30; BST 510->530
    [SPECIES_STEELIX_MEGA]      = { INCLEMENT_BASE_STATS( 75, 145, 230,  55, 105,  20) }, // was Atk 125, SpD 95, Spe 30; BST 610->630
    [SPECIES_HYPNO]             = { INCLEMENT_BASE_STATS( 95,  73,  70, 115, 115,  67) }, // was HP 85, SpA 73; BST 483->535
    [SPECIES_EXEGGCUTE]         = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_EXEGGUTOR]         = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_EXEGGUTOR_ALOLA]   = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_HITMONTOP]         = { .extraAbility = ABILITY_CONTRARY },
    [SPECIES_LICKILICKY]        = { INCLEMENT_BASE_STATS(110,  95,  95,  90,  95,  50) }, // was Atk 85, SpA 80; BST 515->535
    [SPECIES_WEEZING]           = { INCLEMENT_BASE_STATS( 65,  90, 120,  95,  70,  60) }, // was SpA 85; BST 490->500
    [SPECIES_SEAKING]           = { INCLEMENT_BASE_STATS( 80, 112,  65,  65, 100,  98) }, // was Atk 92, SpD 80, Spe 68; BST 450->520
    [SPECIES_ELECTIVIRE]        = { .extraAbility = ABILITY_IRON_FIST },
    [SPECIES_TAUROS]            = { INCLEMENT_BASE_STATS( 75, 120,  95,  40,  70, 110), .extraAbility = ABILITY_RAMPAGE }, // was Atk 100; BST 490->510
    [SPECIES_GYARADOS]          = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_LAPRAS]            = { INCLEMENT_BASE_STATS(130,  85,  80,  95,  95,  60) }, // was SpA 85; BST 535->545
    [SPECIES_LAPRAS_GMAX]       = { INCLEMENT_BASE_STATS(130,  85,  80,  95,  95,  60) }, // was SpA 85; BST 535->545
    [SPECIES_FLAREON]           = { INCLEMENT_BASE_STATS( 95, 130,  60,  65,  65, 110) }, // was HP 65, SpA 95, SpD 110, Spe 65; BST 525->525
    [SPECIES_UMBREON]           = { .extraAbility = ABILITY_MAGIC_GUARD },
    [SPECIES_LEAFEON]           = { .extraAbility = ABILITY_SAP_SIPPER },
    [SPECIES_GLACEON]           = { .extraAbility = ABILITY_WHITEOUT },
    [SPECIES_PORYGON_Z]         = { .extraAbility = ABILITY_MOLD_BREAKER },
    [SPECIES_KABUTOPS]          = { .extraAbility = ABILITY_KEEN_EDGE },
    [SPECIES_ARTICUNO]          = { INCLEMENT_BASE_STATS( 90,  85, 100, 125,  95,  85) }, // was SpA 95, SpD 125; BST 580->580
    [SPECIES_CHIKORITA]         = { .extraAbility = ABILITY_FLOWER_VEIL },
    [SPECIES_BAYLEEF]           = { .extraAbility = ABILITY_FLOWER_VEIL },
    [SPECIES_CYNDAQUIL]         = { .extraAbility = ABILITY_SAND_RUSH },
    [SPECIES_QUILAVA]           = { .extraAbility = ABILITY_SAND_RUSH },
    [SPECIES_TYPHLOSION]        = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_TOTODILE]          = { .extraAbility = ABILITY_STRONG_JAW },
    [SPECIES_CROCONAW]          = { .extraAbility = ABILITY_STRONG_JAW },
    [SPECIES_FERALIGATR]        = { INCLEMENT_BASE_STATS( 85, 105, 100,  69,  83,  88), .extraAbility = ABILITY_STRONG_JAW }, // was SpA 79, Spe 78; BST 530->530
    [SPECIES_SENTRET]           = { .extraAbility = ABILITY_FUR_COAT },
    [SPECIES_FURRET]            = { INCLEMENT_BASE_STATS( 85,  76,  64,  45,  75, 110), .extraAbility = ABILITY_FUR_COAT }, // was SpD 55, Spe 90; BST 415->455
    [SPECIES_LEDYBA]            = { INCLEMENT_BASE_STATS( 40,  40,  30,  20,  80,  55), .extraAbility = ABILITY_AERILATE }, // was Atk 20, SpA 40; BST 265->265
    [SPECIES_LEDIAN]            = { INCLEMENT_BASE_STATS( 55,  95,  50,  35, 110, 105), .extraAbility = ABILITY_AERILATE }, // was Atk 35, SpA 55, Spe 85; BST 390->450
    [SPECIES_SPINARAK]          = { .extraAbility = ABILITY_MERCILESS },
    [SPECIES_ARIADOS]           = { INCLEMENT_BASE_STATS( 80, 110,  80,  60,  80,  40), .extraAbility = ABILITY_MERCILESS }, // was HP 70, Atk 90, Def 70, SpD 70; BST 400->450
    [SPECIES_MAREEP]            = { .extraAbility = ABILITY_MOLD_BREAKER },
    [SPECIES_FLAAFFY]           = { .extraAbility = ABILITY_MOLD_BREAKER },
    [SPECIES_AMPHAROS]          = { .extraAbility = ABILITY_MOLD_BREAKER },
    [SPECIES_JUMPLUFF]          = { INCLEMENT_BASE_STATS( 75,  75,  70,  55,  95, 130) }, // was Atk 55, Spe 110; BST 460->500
    [SPECIES_SUNFLORA]          = { INCLEMENT_BASE_STATS( 95,  75,  55, 125,  85,  30), .extraAbility = ABILITY_DROUGHT }, // was HP 75, SpA 105; BST 425->465
    [SPECIES_YANMEGA]           = { INCLEMENT_BASE_STATS( 86,  76,  86, 126,  56,  95) }, // was SpA 116; BST 515->525
    [SPECIES_MISDREAVUS]        = { .extraAbility = ABILITY_MAGICIAN },
    [SPECIES_GIRAFARIG]         = { INCLEMENT_BASE_STATS( 70,  90,  65, 100,  65, 105) }, // was Atk 80, SpA 90, Spe 85; BST 455->495
    [SPECIES_FORRETRESS]        = { INCLEMENT_BASE_STATS( 75, 100, 140,  60,  80,  20) }, // was Atk 90, SpD 60, Spe 40; BST 465->475
    [SPECIES_DUNSPARCE]         = { INCLEMENT_BASE_STATS(110,  80,  80,  65,  75,  45), .extraAbility = ABILITY_SIMPLE }, // was HP 100, Atk 70, Def 70, SpD 65; BST 415->455
    [SPECIES_URSARING]          = { INCLEMENT_BASE_STATS( 90, 130,  75,  75,  75,  75) }, // was Spe 55; BST 500->520
    [SPECIES_CORSOLA]           = { INCLEMENT_BASE_STATS( 85,  55,  95,  85,  95,  35), .extraAbility = ABILITY_SERENE_GRACE }, // was HP 65, SpA 65; BST 410->450
    [SPECIES_OCTILLERY]         = { INCLEMENT_BASE_STATS( 75,  65,  95, 125,  95,  45) }, // was Atk 105, Def 75, SpA 105, SpD 75; BST 480->500
    [SPECIES_DELIBIRD]          = { INCLEMENT_BASE_STATS( 45,  85,  45,  65,  45,  95), .extraAbility = ABILITY_REFRIGERATE }, // was Atk 55, Spe 75; BST 330->380
    [SPECIES_HOUNDOOM]          = { INCLEMENT_BASE_STATS( 75,  90,  50, 110,  80, 105) }, // was Spe 95; BST 500->510
    [SPECIES_HOUNDOOM_MEGA]     = { INCLEMENT_BASE_STATS( 75, 100,  90, 140,  90, 115) }, // was Atk 90; BST 600->610
    [SPECIES_DONPHAN]           = { INCLEMENT_BASE_STATS( 90, 130, 130,  60,  60,  50) }, // was Atk 120, Def 120; BST 500->520
    [SPECIES_STANTLER]          = { INCLEMENT_BASE_STATS( 73, 115,  62,  85,  65,  95) }, // was Atk 95, Spe 85; BST 465->495
    [SPECIES_PUPITAR]           = { .extraAbility = ABILITY_OVERCOAT },
    [SPECIES_TREECKO]           = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_GROVYLE]           = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_SCEPTILE]          = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_SCEPTILE_MEGA]     = { .extraAbility = ABILITY_CHLOROPLAST },
    [SPECIES_TORCHIC]           = { .extraAbility = ABILITY_QUICK_FEET },
    [SPECIES_COMBUSKEN]         = { .extraAbility = ABILITY_QUICK_FEET },
    [SPECIES_BLAZIKEN]          = { .extraAbility = ABILITY_QUICK_FEET },
    [SPECIES_MUDKIP]            = { .extraAbility = ABILITY_SWIFT_SWIM },
    [SPECIES_MARSHTOMP]         = { .extraAbility = ABILITY_SWIFT_SWIM },
    [SPECIES_SWAMPERT]          = { .extraAbility = ABILITY_SWIFT_SWIM },
    [SPECIES_MIGHTYENA]         = { INCLEMENT_BASE_STATS( 70, 110,  70,  60,  60,  90) }, // was Atk 90, Spe 70; BST 420->460
    [SPECIES_LINOONE]           = { INCLEMENT_BASE_STATS( 78, 100,  61,  50,  61, 110) }, // was Atk 70, Spe 100; BST 420->460
    [SPECIES_WURMPLE]           = { .extraAbility = ABILITY_POISON_POINT },
    [SPECIES_GARDEVOIR]         = { INCLEMENT_BASE_STATS( 68,  60,  65, 125, 115, 100) }, // was Atk 65, Spe 80; BST 518->533
    [SPECIES_GARDEVOIR_MEGA]    = { INCLEMENT_BASE_STATS( 68,  85,  80, 165, 135, 100) }, // was Def 65; BST 618->633
    [SPECIES_GALLADE]           = { INCLEMENT_BASE_STATS( 68, 125,  65,  60, 115, 100), .extraAbility = ABILITY_TRACE }, // was SpA 65, Spe 80; BST 518->533
    [SPECIES_GALLADE_MEGA]      = { INCLEMENT_BASE_STATS( 68, 165,  95,  65, 125, 115), .extraAbility = ABILITY_KEEN_EDGE }, // was SpD 115, Spe 110; BST 618->633
    [SPECIES_SLAKOTH]           = { .extraAbility = ABILITY_SLOW_START },
    [SPECIES_SLAKING]           = { .extraAbility = ABILITY_SLOW_START },
    [SPECIES_SHEDINJA]          = { INCLEMENT_BASE_STATS(  1, 100,  45, 100,  30,  40) }, // was Atk 90, SpA 30; BST 236->316
    [SPECIES_EXPLOUD]           = { INCLEMENT_BASE_STATS(104,  91,  63, 111,  83,  68) }, // was SpA 91, SpD 73; BST 490->520
    [SPECIES_PROBOPASS]         = { INCLEMENT_BASE_STATS( 60,  55, 145,  95, 150,  40) }, // was SpA 75; BST 525->545
    [SPECIES_DELCATTY]          = { INCLEMENT_BASE_STATS( 70,  85,  65,  85,  55, 110) }, // was Atk 65, SpA 55, Spe 90; BST 400->470
    [SPECIES_SABLEYE_MEGA]      = { INCLEMENT_BASE_STATS( 70,  85, 130,  85, 120,  20) }, // was Def 125, SpD 115; BST 500->510
    [SPECIES_MAWILE]            = { .extraAbility = ABILITY_HUGE_POWER },
    [SPECIES_MAWILE_MEGA]       = { INCLEMENT_BASE_STATS( 70, 105, 130,  55, 100,  50) }, // was Def 125, SpD 95; BST 500->510
    [SPECIES_MANECTRIC_MEGA]    = { INCLEMENT_BASE_STATS( 70,  75,  90, 135,  90, 135) }, // was Def 80, SpD 80; BST 575->595
    [SPECIES_PLUSLE]            = { .extraAbility = ABILITY_BATTERY },
    [SPECIES_MINUN]             = { .extraAbility = ABILITY_BATTERY },
    [SPECIES_VOLBEAT]           = { INCLEMENT_BASE_STATS( 65,  73,  85,  47,  85, 115) }, // was Def 75, Spe 85; BST 430->470
    [SPECIES_ILLUMISE]          = { INCLEMENT_BASE_STATS( 65,  47,  75,  93,  85, 105) }, // was SpA 73, Spe 85; BST 430->470
    [SPECIES_SWALOT]            = { INCLEMENT_BASE_STATS(100,  83,  93,  83,  93,  55) }, // was Atk 73, Def 83, SpA 73, SpD 83; BST 467->507
    [SPECIES_SHARPEDO_MEGA]     = { INCLEMENT_BASE_STATS( 70, 150,  70, 110,  65, 115) }, // was Atk 140, Spe 105; BST 560->580
    [SPECIES_CAMERUPT]          = { INCLEMENT_BASE_STATS( 90, 100,  80, 105,  85,  40), .extraAbility = ABILITY_SHEER_FORCE }, // was HP 70, Def 70, SpD 75; BST 460->500
    [SPECIES_CAMERUPT_MEGA]     = { INCLEMENT_BASE_STATS( 90, 120, 110, 145, 115,  20) }, // was HP 70, Def 100, SpD 105; BST 560->600
    [SPECIES_SPINDA]            = { INCLEMENT_BASE_STATS( 60,  90,  60,  90,  60,  90) }, // was Atk 60, SpA 60, Spe 60; BST 360->450
    [SPECIES_CACTURNE]          = { .extraAbility = ABILITY_SAND_RUSH },
    [SPECIES_ALTARIA]           = { INCLEMENT_BASE_STATS( 75,  90,  90,  70, 105,  80) }, // was Atk 70; BST 490->510
    [SPECIES_ALTARIA_MEGA]      = { INCLEMENT_BASE_STATS( 75, 110, 110, 110, 105, 100) }, // was Spe 80; BST 590->610
    [SPECIES_ZANGOOSE]          = { INCLEMENT_BASE_STATS( 73, 115,  70,  60,  70, 105) }, // was Def 60, SpD 60, Spe 90; BST 458->493
    [SPECIES_SEVIPER]           = { INCLEMENT_BASE_STATS( 73, 100,  60, 100,  60, 100), .extraAbility = ABILITY_MERCILESS }, // was Spe 65; BST 458->493
    [SPECIES_LUNATONE]          = { INCLEMENT_BASE_STATS( 90,  55,  65, 125, 115,  70) }, // was SpA 95, SpD 85; BST 460->520
    [SPECIES_SOLROCK]           = { INCLEMENT_BASE_STATS( 90, 125, 115,  55,  65,  70) }, // was Atk 95, Def 85; BST 460->520
    [SPECIES_WHISCASH]          = { .extraAbility = ABILITY_ADAPTABILITY },
    [SPECIES_CLAYDOL]           = { INCLEMENT_BASE_STATS( 70,  70, 105,  90, 120,  75) }, // was HP 60, SpA 70; BST 500->530
    [SPECIES_CRADILY]           = { INCLEMENT_BASE_STATS( 96,  81,  97,  91, 107,  43) }, // was HP 86, SpA 81; BST 495->515
    [SPECIES_ARMALDO]           = { INCLEMENT_BASE_STATS( 75, 125, 120,  70,  80,  45) }, // was Def 100; BST 495->515
    [SPECIES_MILOTIC]           = { .extraAbility = ABILITY_MULTISCALE },
    [SPECIES_CASTFORM_NORMAL]   = { INCLEMENT_BASE_STATS( 70,  70,  70, 100,  70, 100) }, // was SpA 70, Spe 70; BST 420->480
    [SPECIES_CASTFORM_SUNNY]    = { INCLEMENT_BASE_STATS( 70,  70,  70, 100,  70, 100) }, // was SpA 70, Spe 70; BST 420->480
    [SPECIES_CASTFORM_RAINY]    = { INCLEMENT_BASE_STATS( 70,  70,  70, 100,  70, 100) }, // was SpA 70, Spe 70; BST 420->480
    [SPECIES_CASTFORM_SNOWY]    = { INCLEMENT_BASE_STATS( 70,  70,  70, 100,  70, 100) }, // was SpA 70, Spe 70; BST 420->480
    [SPECIES_BANETTE]           = { INCLEMENT_BASE_STATS( 64, 125,  85,  65,  63,  83), .extraAbility = ABILITY_VENGEANCE }, // was Atk 115, Def 65, SpA 83, Spe 65; BST 455->485
    [SPECIES_BANETTE_MEGA]      = { INCLEMENT_BASE_STATS( 64, 165,  95,  75,  83, 103) }, // was Def 75, SpA 93, Spe 75; BST 555->585
    [SPECIES_DUSKNOIR]          = { INCLEMENT_BASE_STATS( 45, 120, 135,  60, 135,  45), .extraAbility = ABILITY_IRON_FIST }, // was Atk 100, SpA 65; BST 525->540
    [SPECIES_ABSOL]             = { .extraAbility = ABILITY_KEEN_EDGE },
    [SPECIES_ABSOL_MEGA]        = { INCLEMENT_BASE_STATS( 65, 160,  60, 125,  60, 115) }, // was Atk 150, SpA 115; BST 565->585
    [SPECIES_GLALIE]            = { INCLEMENT_BASE_STATS( 80, 100,  70,  80,  70, 100), .extraAbility = ABILITY_REFRIGERATE }, // was Atk 80, Def 80, SpD 80, Spe 80; BST 480->500
    [SPECIES_GLALIE_MEGA]       = { INCLEMENT_BASE_STATS( 80, 130,  80, 120,  80, 110) }, // was Atk 120, Spe 100; BST 580->600
    [SPECIES_FROSLASS]          = { INCLEMENT_BASE_STATS( 70,  70,  70, 110,  70, 110), .extraAbility = ABILITY_WHITEOUT }, // was Atk 80, SpA 80; BST 480->500
    [SPECIES_HUNTAIL]           = { .extraAbility = ABILITY_STRONG_JAW },
    [SPECIES_GOREBYSS]          = { .extraAbility = ABILITY_STORM_DRAIN },
    [SPECIES_TURTWIG]           = { .extraAbility = ABILITY_SOLID_ROCK },
    [SPECIES_GROTLE]            = { .extraAbility = ABILITY_SOLID_ROCK },
    [SPECIES_CHIMCHAR]          = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_MONFERNO]          = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_INFERNAPE]         = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_PIPLUP]            = { .extraAbility = ABILITY_SLUSH_RUSH },
    [SPECIES_PRINPLUP]          = { .extraAbility = ABILITY_SLUSH_RUSH },
    [SPECIES_EMPOLEON]          = { .extraAbility = ABILITY_SLUSH_RUSH },
    [SPECIES_KRICKETUNE]        = { INCLEMENT_BASE_STATS( 77,  65,  51, 100,  51,  85) }, // was Atk 85, SpA 55, Spe 65; BST 384->429
    [SPECIES_LUXRAY]            = { INCLEMENT_BASE_STATS( 90, 120,  79,  75,  79,  90), .extraAbility = ABILITY_STRONG_JAW }, // was HP 80, SpA 95, Spe 70; BST 523->533
    [SPECIES_CRANIDOS]          = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_RAMPARDOS]         = { INCLEMENT_BASE_STATS( 97, 165,  80,  65,  50,  58), .extraAbility = ABILITY_RAMPAGE }, // was Def 60; BST 495->515
    [SPECIES_BASTIODON]         = { INCLEMENT_BASE_STATS( 80,  52, 168,  47, 138,  30) }, // was HP 60; BST 495->515
    [SPECIES_WORMADAM_PLANT]    = { INCLEMENT_BASE_STATS( 80,  59,  85,  99, 105,  36) }, // was HP 60, SpA 79; BST 424->464
    [SPECIES_WORMADAM_SANDY]    = { INCLEMENT_BASE_STATS( 80,  99, 105,  59,  85,  36) }, // was HP 60, Atk 79; BST 424->464
    [SPECIES_WORMADAM_TRASH]    = { INCLEMENT_BASE_STATS( 80,  69, 105,  69, 105,  36) }, // was HP 60, Def 95, SpD 95; BST 424->464
    [SPECIES_MOTHIM_PLANT]      = { INCLEMENT_BASE_STATS( 70,  54,  70,  94,  70,  86) }, // was Atk 94, Def 50, SpD 50, Spe 66; BST 424->444
    [SPECIES_MOTHIM_SANDY]      = { INCLEMENT_BASE_STATS( 70,  54,  70,  94,  70,  86) }, // was Atk 94, Def 50, SpD 50, Spe 66; BST 424->444
    [SPECIES_MOTHIM_TRASH]      = { INCLEMENT_BASE_STATS( 70,  54,  70,  94,  70,  86) }, // was Atk 94, Def 50, SpD 50, Spe 66; BST 424->444
    [SPECIES_PACHIRISU]         = { INCLEMENT_BASE_STATS( 60,  45,  70,  75, 110,  95), .extraAbility = ABILITY_FUR_COAT }, // was SpA 45, SpD 90; BST 405->455
    [SPECIES_FLOATZEL]          = { INCLEMENT_BASE_STATS( 85, 120,  55, 100,  50, 115) }, // was Atk 105, SpA 85; BST 495->525
    [SPECIES_CHERUBI]           = { INCLEMENT_BASE_STATS( 45,  62,  45,  62,  53,  35) }, // was Atk 35; BST 275->302
    [SPECIES_CHERRIM_OVERCAST]  = { INCLEMENT_BASE_STATS( 70, 100,  70,  87,  78,  85) }, // was Atk 60; BST 450->490
    [SPECIES_CHERRIM_SUNSHINE]  = { INCLEMENT_BASE_STATS( 70, 100,  70,  87,  78, 115) }, // was Atk 60, Spe 85; BST 450->520
    [SPECIES_DRIFBLIM]          = { INCLEMENT_BASE_STATS(150,  80,  54,  90,  74,  80) }, // was Def 44, SpD 54; BST 498->528
    [SPECIES_LOPUNNY_MEGA]      = { INCLEMENT_BASE_STATS( 65, 136,  94,  74,  96, 135) }, // was SpA 54; BST 580->600
    [SPECIES_PURUGLY]           = { INCLEMENT_BASE_STATS( 71, 102,  74,  64,  69, 119) }, // was Atk 82, Def 64, SpD 59, Spe 112; BST 452->499
    [SPECIES_SPIRITOMB]         = { INCLEMENT_BASE_STATS( 70, 108, 108, 108, 108,  35) }, // was HP 50, Atk 92, SpA 92; BST 485->537
    [SPECIES_GARCHOMP_MEGA]     = { INCLEMENT_BASE_STATS(108, 170, 110, 120,  90, 102) }, // was Def 115, SpD 95, Spe 92; BST 700->700
    [SPECIES_DRAPION]           = { INCLEMENT_BASE_STATS( 70, 100, 120,  60,  75,  95) }, // was Atk 90, Def 110; BST 500->520
    [SPECIES_TOXICROAK]         = { INCLEMENT_BASE_STATS( 83, 106,  65,  86,  65,  95) }, // was Spe 85; BST 490->500
    [SPECIES_CARNIVINE]         = { INCLEMENT_BASE_STATS( 74, 100,  92,  90,  92,  46) }, // was Def 72, SpD 72; BST 454->494
    [SPECIES_FINNEON]           = { .extraAbility = ABILITY_DAZZLING },
    [SPECIES_LUMINEON]          = { INCLEMENT_BASE_STATS( 69,  69,  76, 109,  86,  91), .extraAbility = ABILITY_DAZZLING }, // was SpA 69; BST 460->500
    [SPECIES_ABOMASNOW]         = { INCLEMENT_BASE_STATS( 90, 102,  75, 102,  85,  60) }, // was Atk 92, SpA 92; BST 494->514
    [SPECIES_ABOMASNOW_MEGA]    = { INCLEMENT_BASE_STATS( 90, 142, 105, 142, 105,  30), .extraAbility = ABILITY_WHITEOUT }, // was Atk 132, SpA 132; BST 594->614
    [SPECIES_SNIVY]             = { .extraAbility = ABILITY_SHED_SKIN },
    [SPECIES_SERVINE]           = { .extraAbility = ABILITY_SHED_SKIN },
    [SPECIES_SERPERIOR]         = { .extraAbility = ABILITY_SHED_SKIN },
    [SPECIES_TEPIG]             = { .extraAbility = ABILITY_GLUTTONY },
    [SPECIES_PIGNITE]           = { .extraAbility = ABILITY_GLUTTONY },
    [SPECIES_EMBOAR]            = { INCLEMENT_BASE_STATS(110, 123,  80,  70,  80,  65), .extraAbility = ABILITY_GLUTTONY }, // was Def 65, SpA 100, SpD 65; BST 528->528
    [SPECIES_OSHAWOTT]          = { .extraAbility = ABILITY_KEEN_EDGE },
    [SPECIES_DEWOTT]            = { .extraAbility = ABILITY_KEEN_EDGE },
    [SPECIES_SAMUROTT]          = { .extraAbility = ABILITY_KEEN_EDGE },
    [SPECIES_WATCHOG]           = { INCLEMENT_BASE_STATS( 75,  95,  79,  60,  79,  77), .extraAbility = ABILITY_DAZZLING }, // was HP 60, Atk 85, Def 69, SpD 69; BST 420->465
    [SPECIES_STOUTLAND]         = { INCLEMENT_BASE_STATS( 85, 120,  90,  45,  90,  80) }, // was Atk 110; BST 500->510
    [SPECIES_SIMISAGE]          = { INCLEMENT_BASE_STATS( 75, 108,  63, 108,  63, 101) }, // was Atk 98, SpA 98; BST 498->518
    [SPECIES_SIMISEAR]          = { INCLEMENT_BASE_STATS( 75, 108,  63, 108,  63, 101) }, // was Atk 98, SpA 98; BST 498->518
    [SPECIES_SIMIPOUR]          = { INCLEMENT_BASE_STATS( 75, 108,  63, 108,  63, 101) }, // was Atk 98, SpA 98; BST 498->518
    [SPECIES_MUSHARNA]          = { INCLEMENT_BASE_STATS(116,  55,  85, 117,  95,  29), .extraAbility = ABILITY_PSYCHIC_SURGE }, // was SpA 107; BST 487->497
    [SPECIES_UNFEZANT]          = { INCLEMENT_BASE_STATS(100, 115,  80,  65,  55,  93), .extraAbility = ABILITY_TOUGH_CLAWS }, // was HP 80; BST 488->508
    [SPECIES_ZEBSTRIKA]         = { INCLEMENT_BASE_STATS( 85, 110,  63,  80,  63, 116) }, // was HP 75, Atk 100; BST 497->517
    [SPECIES_ROGGENROLA]        = { INCLEMENT_BASE_STATS( 55,  25,  85,  75,  25,  15) }, // was Atk 75, SpA 25; BST 280->280
    [SPECIES_BOLDORE]           = { INCLEMENT_BASE_STATS( 70,  50, 105, 105,  40,  20) }, // was Atk 105, SpA 50; BST 390->390
    [SPECIES_GIGALITH]          = { INCLEMENT_BASE_STATS( 85,  60, 130, 135,  80,  25), .extraAbility = ABILITY_SOLAR_POWER }, // was Atk 135, SpA 60; BST 515->515
    [SPECIES_AUDINO]            = { INCLEMENT_BASE_STATS(103,  60,  96,  80,  96,  50) }, // was Def 86, SpA 60, SpD 86; BST 445->485
    [SPECIES_AUDINO_MEGA]       = { INCLEMENT_BASE_STATS(103,  60, 126, 120, 126,  50), .extraAbility = ABILITY_FAIRY_AURA }, // was SpA 80; BST 545->585
    [SPECIES_SEISMITOAD]        = { INCLEMENT_BASE_STATS(105, 105,  75,  95,  75,  74) }, // was Atk 95, SpA 85; BST 509->529
    [SPECIES_LEAVANNY]          = { INCLEMENT_BASE_STATS( 75, 113,  80,  70,  80, 102) }, // was Atk 103, Spe 92; BST 500->520
    [SPECIES_MARACTUS]          = { INCLEMENT_BASE_STATS( 75,  56,  87, 106,  87,  60), .extraAbility = ABILITY_HUGE_POWER }, // was Atk 86, Def 67, SpD 67; BST 461->471
    [SPECIES_CARRACOSTA]        = { INCLEMENT_BASE_STATS( 74, 128, 133,  83,  65,  32) }, // was Atk 108; BST 495->515
    [SPECIES_ARCHEN]            = { .extraAbility = ABILITY_KLUTZ },
    [SPECIES_ARCHEOPS]          = { .extraAbility = ABILITY_KLUTZ },
    [SPECIES_DUCKLETT]          = { .extraAbility = ABILITY_COMPETITIVE },
    [SPECIES_SWANNA]            = { .extraAbility = ABILITY_COMPETITIVE },
    [SPECIES_VANILLUXE]         = { INCLEMENT_BASE_STATS( 71,  69,  85, 120,  95,  95) }, // was Atk 95, SpA 110, Spe 79; BST 535->535
    [SPECIES_KLINK]             = { .extraAbility = ABILITY_MOTOR_DRIVE },
    [SPECIES_KLANG]             = { .extraAbility = ABILITY_MOTOR_DRIVE },
    [SPECIES_KLINKLANG]         = { .extraAbility = ABILITY_MOTOR_DRIVE },
    [SPECIES_TYNAMO]            = { .extraAbility = ABILITY_SWIFT_SWIM },
    [SPECIES_EELEKTRIK]         = { .extraAbility = ABILITY_SWIFT_SWIM },
    [SPECIES_EELEKTROSS]        = { INCLEMENT_BASE_STATS( 85, 115,  90, 105,  90,  50), .extraAbility = ABILITY_SWIFT_SWIM }, // was Def 80, SpD 80; BST 515->535
    [SPECIES_BEHEEYEM]          = { .extraAbility = ABILITY_PSYCHIC_SURGE },
    [SPECIES_CHANDELURE]        = { .extraAbility = ABILITY_MOLD_BREAKER },
    [SPECIES_BEARTIC]           = { INCLEMENT_BASE_STATS( 95, 130,  80,  70,  80,  70) }, // was Spe 50; BST 505->525
    [SPECIES_ACCELGOR]          = { .extraAbility = ABILITY_DRY_SKIN },
    [SPECIES_STUNFISK]          = { .extraAbility = ABILITY_STORM_DRAIN },
    [SPECIES_DEINO]             = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_ZWEILOUS]          = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_HYDREIGON]         = { .extraAbility = ABILITY_RAMPAGE },
    [SPECIES_CHESPIN]           = { .extraAbility = ABILITY_STURDY },
    [SPECIES_QUILLADIN]         = { .extraAbility = ABILITY_STURDY },
    [SPECIES_CHESNAUGHT]        = { .extraAbility = ABILITY_STURDY },
    [SPECIES_FENNEKIN]          = { .extraAbility = ABILITY_PYROMANCY },
    [SPECIES_BRAIXEN]           = { .extraAbility = ABILITY_PYROMANCY },
    [SPECIES_DELPHOX]           = { .extraAbility = ABILITY_PYROMANCY },
    [SPECIES_FROAKIE]           = { .extraAbility = ABILITY_INFILTRATOR },
    [SPECIES_FROGADIER]         = { .extraAbility = ABILITY_INFILTRATOR },
    [SPECIES_GRENINJA]          = { .extraAbility = ABILITY_INFILTRATOR },
    [SPECIES_SKIDDO]            = { .extraAbility = ABILITY_LEAF_GUARD },
    [SPECIES_MEOWSTIC_M]        = { INCLEMENT_BASE_STATS( 94,  48,  76, 103,  81, 104), .extraAbility = ABILITY_SHEER_FORCE }, // was HP 74, SpA 83; BST 466->506
    [SPECIES_MEOWSTIC_F]        = { INCLEMENT_BASE_STATS( 74,  48,  76, 123,  81, 104), .extraAbility = ABILITY_SHEER_FORCE }, // was SpA 83; BST 466->506
    [SPECIES_MEOWSTIC_M_MEGA]   = { INCLEMENT_BASE_STATS( 94,  48,  76, 143, 101, 124) }, // was HP 74; BST 566->586
    [SPECIES_CLAWITZER]         = { INCLEMENT_BASE_STATS( 71,  73, 108, 120,  89,  59) }, // was Def 88; BST 500->520
    [SPECIES_TYRANTRUM]         = { INCLEMENT_BASE_STATS( 82, 121, 119,  69,  69,  81) }, // was SpD 59, Spe 71; BST 521->541
    [SPECIES_AURORUS]           = { INCLEMENT_BASE_STATS(123,  77,  72, 119,  92,  58) }, // was SpA 99; BST 521->541
    [SPECIES_DEDENNE]           = { INCLEMENT_BASE_STATS( 67,  58,  57, 101,  87, 101), .extraAbility = ABILITY_PIXILATE }, // was SpA 81, SpD 67; BST 431->471
    [SPECIES_PUMPKABOO_AVERAGE] = { INCLEMENT_BASE_STATS( 49,  44,  70,  66,  55,  51), .extraAbility = ABILITY_FLARE_BOOST }, // was Atk 66, SpA 44; BST 335->335
    [SPECIES_PUMPKABOO_SMALL]   = { INCLEMENT_BASE_STATS( 44,  44,  70,  66,  55,  56), .extraAbility = ABILITY_FLARE_BOOST }, // was Atk 66, SpA 44; BST 335->335
    [SPECIES_PUMPKABOO_LARGE]   = { INCLEMENT_BASE_STATS( 54,  44,  70,  66,  55,  46), .extraAbility = ABILITY_FLARE_BOOST }, // was Atk 66, SpA 44; BST 335->335
    [SPECIES_PUMPKABOO_SUPER]   = { INCLEMENT_BASE_STATS( 59,  44,  70,  66,  55,  41), .extraAbility = ABILITY_FLARE_BOOST }, // was Atk 66, SpA 44; BST 335->335
    [SPECIES_ROWLET]            = { .extraAbility = ABILITY_TINTED_LENS },
    [SPECIES_DARTRIX]           = { .extraAbility = ABILITY_TINTED_LENS },
    [SPECIES_LITTEN]            = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_TORRACAT]          = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_INCINEROAR]        = { .extraAbility = ABILITY_MOXIE },
    [SPECIES_POPPLIO]           = { .extraAbility = ABILITY_BERSERK },
    [SPECIES_BRIONNE]           = { .extraAbility = ABILITY_BERSERK },
    [SPECIES_PRIMARINA]         = { .extraAbility = ABILITY_BERSERK },
    [SPECIES_TOUCANNON]         = { INCLEMENT_BASE_STATS( 80, 120,  95,  75,  95,  60) }, // was Def 75, SpD 75; BST 485->525
    [SPECIES_GUMSHOOS]          = { INCLEMENT_BASE_STATS( 88, 110,  80,  55,  80,  45) }, // was Def 60, SpD 60; BST 418->458
    [SPECIES_CHARJABUG]         = { INCLEMENT_BASE_STATS( 57, 102,  95,  55,  75,  36) }, // was Atk 82; BST 400->420
    [SPECIES_VIKAVOLT]          = { INCLEMENT_BASE_STATS( 77,  95,  99, 145,  75, 109) }, // was Atk 70, Def 90, Spe 43; BST 500->600
    [SPECIES_VIKAVOLT_TOTEM]    = { INCLEMENT_BASE_STATS( 77,  95,  99, 145,  75, 109) }, // was Atk 70, Def 90, Spe 43; BST 500->600
    [SPECIES_CRABOMINABLE]      = { INCLEMENT_BASE_STATS( 97, 132,  97,  62,  87,  43) }, // was Def 77, SpD 67; BST 478->518
    [SPECIES_LYCANROC_MIDDAY]   = { INCLEMENT_BASE_STATS( 75, 125,  70,  55,  70, 112) }, // was Atk 115, Def 65, SpD 65; BST 487->507
    [SPECIES_LYCANROC_MIDNIGHT] = { INCLEMENT_BASE_STATS( 95, 115,  85,  55,  75,  82), .extraAbility = ABILITY_STURDY }, // was HP 85, Def 75; BST 487->507
    [SPECIES_LYCANROC_DUSK]     = { INCLEMENT_BASE_STATS( 75, 127,  70,  55,  70, 110) }, // was Atk 117, Def 65, SpD 65; BST 487->507
    [SPECIES_SHIINOTIC]         = { INCLEMENT_BASE_STATS( 80,  45,  80, 100, 110,  30) }, // was HP 60, SpA 90, SpD 100; BST 405->445
    [SPECIES_TSAREENA]          = { INCLEMENT_BASE_STATS( 72, 120,  90,  50,  90, 108) }, // was Def 98, SpD 98, Spe 72; BST 510->530
    [SPECIES_WIMPOD]            = { .extraAbility = ABILITY_RUN_AWAY },
    [SPECIES_GOLISOPOD]         = { .extraAbility = ABILITY_SHELL_ARMOR },
    [SPECIES_SILVALLY_NORMAL]   = { INCLEMENT_BASE_STATS(100, 100, 100, 100, 100, 100) }, // was HP 95, Atk 95, Def 95, SpA 95, SpD 95, Spe 95; BST 570->600
    [SPECIES_KOMALA]            = { INCLEMENT_BASE_STATS( 85, 115,  95,  55,  95,  65) }, // was HP 65, Def 65, SpA 75; BST 480->510
    [SPECIES_TURTONATOR]        = { .extraAbility = ABILITY_IRON_BARBS },
    [SPECIES_TOGEDEMARU]        = { INCLEMENT_BASE_STATS( 65, 108,  83,  40,  73, 106) }, // was Atk 98, Def 63, Spe 96; BST 435->475
    [SPECIES_DHELMISE]          = { INCLEMENT_BASE_STATS( 70, 131, 120,  76,  90,  40) }, // was Def 100, SpA 86; BST 517->527
};

#undef INCLEMENT_BASE_STATS
