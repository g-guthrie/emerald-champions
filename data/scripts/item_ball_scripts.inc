Common_EventScript_FindItem::
	callnative GetItemBallIdAndAmountFromTemplate
	copyvar VAR_0x8000, VAR_RESULT
	copyvar VAR_0x8001, VAR_0x8009
Common_EventScript_FindOrdinaryItem::
	finditem VAR_0x8000, VAR_0x8001
	end

@ Inclement Emerald's item ball scripts share Std_FindItem's finite reward
@ conversion, so both the generic wrapper and authored pickups use it.

Route102_EventScript_ItemPowerHerb:: @ 8290CD8
	finditem ITEM_POWER_HERB
	end

Route103_EventScript_ItemPearl:: @ 8290CE5
	finditem ITEM_PEARL
	end

Route103_EventScript_ItemPPUp:: @ 8290CF2
	finditem ITEM_PP_UP
	end

Route104_EventScript_ItemPPUp:: @ 8290CFF
	finditem ITEM_PP_UP
	end

Route104_EventScript_ItemFriendBall:: @ 8290D0C
	finditem ITEM_FRIEND_BALL
	end

Route104_EventScript_ItemShellBell:: @ 8290D19
	finditem ITEM_SHELL_BELL
	end

Route104_EventScript_ItemSootheBell:: @ 8290D26
	finditem ITEM_SOOTHE_BELL
	end

Route105_EventScript_ItemAdrenalineOrb:: @ 8290D33
	finditem ITEM_ADRENALINE_ORB
	end

Route106_EventScript_ItemKinglerite:: @ 8290D40
	finditem ITEM_KINGLERITE
	end

Route108_EventScript_ItemGolurkite:: @ 8290D4D
	finditem ITEM_GOLURKITE
	end

Route109_EventScript_ItemCrabominite:: @ 8290D5A
	finditem ITEM_CRABOMINITE
	end

Route109_EventScript_ItemZoomLens:: @ 8290D67
	finditem ITEM_ZOOM_LENS
	end

Route110_EventScript_ItemFalinksite:: @ 8290D74
	finditem ITEM_FALINKSITE
	end

Route110_EventScript_ItemAssaultVest:: @ 8290D81
	finditem ITEM_ASSAULT_VEST
	end

Route110_EventScript_ItemElixir:: @ 8290D8E
	finditem ITEM_ELIXIR
	end

Route111_EventScript_ItemRoomService:: @ 8290D9B
	finditem ITEM_ROOM_SERVICE
	end

Route111_EventScript_ItemStarPiece:: @ 8290DA8
	finditem ITEM_STAR_PIECE
	end

Route111_EventScript_ItemBigNugget:: @ 8290DB5
	finditem ITEM_BIG_NUGGET
	end

Route111_EventScript_ItemElixir:: @ 8290DC2
	finditem ITEM_ELIXIR
	end

Route112_EventScript_ItemHeavyDutyBoots:: @ 8290DCF
	finditem ITEM_HEAVY_DUTY_BOOTS
	end

Route113_EventScript_ItemMaxEther:: @ 8290DDC
	finditem ITEM_MAX_ETHER
	end

Route113_EventScript_ItemShinyStone:: @ 8290DE9
	finditem ITEM_SHINY_STONE
	end

Route113_EventScript_ItemHyperPotion:: @ 8290DF6
	finditem ITEM_HYPER_POTION
	end

Route114_EventScript_ItemNugget:: @ 8290E03
	finditem ITEM_NUGGET
	end

Route114_EventScript_ItemWeaknessPolicy:: @ 8290E10
	finditem ITEM_WEAKNESS_POLICY
	end

Route114_EventScript_ItemProtectivePads:: @ 8290E1D
	finditem ITEM_PROTECTIVE_PADS
	end

Route115_EventScript_ItemSoftSand:: @ 8290E2A
	finditem ITEM_SOFT_SAND
	end

Route115_EventScript_ItemDragalgite:: @ 8290E37
	finditem ITEM_DRAGALGITE
	end

Route115_EventScript_ItemMoonBall:: @ 8290E44
	finditem ITEM_MOON_BALL
	end

Route115_EventScript_ItemGreatBall:: @ 8290E51
	finditem ITEM_GREAT_BALL
	end

Route115_EventScript_ItemHeracronite:: @ 8290E5E
	finditem ITEM_HERACRONITE
	end

Route115_EventScript_ItemPPUp:: @ 8290E6B
	finditem ITEM_PP_UP
	end

Route116_EventScript_ItemWideLens:: @ 8290E78
	finditem ITEM_WIDE_LENS
	end

Route116_EventScript_ItemBlackSludge:: @ 8290E85
	finditem ITEM_BLACK_SLUDGE
	end

Route116_EventScript_ItemFocusBand:: @ 8290E92
	finditem ITEM_FOCUS_BAND
	end

Route116_EventScript_ItemMuscleBand:: @ 8290E9F
	finditem ITEM_MUSCLE_BAND
	end

Route116_EventScript_ItemBindingBand:: @ 8290EAC
	finditem ITEM_BINDING_BAND
	end

Route117_EventScript_ItemGreatBall:: @ 8290EB9
	finditem ITEM_GREAT_BALL
	end

Route117_EventScript_ItemRevive:: @ 8290EC6
	finditem ITEM_REVIVE
	end

Route118_EventScript_ItemHyperPotion:: @ 8290ED3
	finditem ITEM_HYPER_POTION
	end

Route119_EventScript_ItemToxicOrb:: @ 8290EE0
	finditem ITEM_TOXIC_ORB
	end

Route119_EventScript_ItemScizorite:: @ 8290EED
	finditem ITEM_SCIZORITE
	end

Route119_EventScript_ItemAudinite:: @ 8290EFA
	finditem ITEM_AUDINITE
	end

Route119_EventScript_ItemScraftinite:: @ 8290F07
	finditem ITEM_SCRAFTINITE
	end

Route119_EventScript_ItemNugget2:: @ 8290F14
	finditem ITEM_NUGGET
	end

Route119_EventScript_ItemButterfrenite:: @ 8290F21
	finditem ITEM_BUTTERFRENITE
	end

Route119_EventScript_ItemBeedrillite:: @ 8290F2E
	finditem ITEM_BEEDRILLITE
	end

Route119_EventScript_ItemScolipite:: @ 8290F3B
	finditem ITEM_SCOLIPITE
	end

Route120_EventScript_ItemMeganiumite:: @ 8290F48
	finditem ITEM_MEGANIUMITE
	end

Route120_EventScript_ItemPidgeotite:: @ 8290F55
	finditem ITEM_PIDGEOTITE
	end

Route120_EventScript_ItemPinsirite:: @ 8290F62
	finditem ITEM_PINSIRITE
	end

Route120_EventScript_ItemNestBall:: @ 8290F6F
	finditem ITEM_NEST_BALL
	end

Route120_EventScript_ItemAerodactylite:: @ 8290F7C
	goto_if_unset FLAG_BADGE06_GET, EventScript_MegaStoneBadgeLocked
	finditem ITEM_AERODACTYLITE
	end

Route121_EventScript_ItemAbilityShield:: @ 8290F89
	finditem ITEM_ABILITY_SHIELD
	end

Route121_EventScript_ItemSkarmorite:: @ 8290F96
	finditem ITEM_SKARMORITE
	end

Route121_EventScript_ItemEmboarite:: @ 8290FA3
	finditem ITEM_EMBOARITE
	end

Route123_EventScript_ItemWideLens:: @ 8290FB0
	finditem ITEM_WIDE_LENS
	end

Route123_EventScript_ItemMeadowPlate:: @ 8290FBD
	finditem ITEM_MEADOW_PLATE
	end

Route123_EventScript_ItemAbsoliteZ:: @ 8290FCA
	finditem ITEM_ABSOLITE_Z
	end

Route123_EventScript_ItemPPUp:: @ 8290FD7
	finditem ITEM_PP_UP
	end

Route123_EventScript_ItemHawluchanite:: @ 8290FE4
	finditem ITEM_HAWLUCHANITE
	end

Route124_EventScript_ItemFloettite:: @ 8290FF1
	finditem ITEM_FLOETTITE
	end

Route124_EventScript_ItemBottleCap:: @ 8290FFE
	finditem ITEM_BOTTLE_CAP
	end

Route124_EventScript_ItemYellowShard:: @ 829100B
	finditem ITEM_YELLOW_SHARD
	end

Route125_EventScript_ItemMawilite:: @ 8291018
	finditem ITEM_MAWILITE
	end

Route126_EventScript_ItemCometShard:: @ 8291025
	finditem ITEM_COMET_SHARD
	end

Route127_EventScript_ItemDragonFang:: @ 8291032
	finditem ITEM_DRAGON_FANG
	end

Route127_EventScript_ItemLaprasite:: @ 829103F
	finditem ITEM_LAPRASITE
	end

Route127_EventScript_ItemNugget:: @ 829104C
	finditem ITEM_NUGGET
	end

Route132_EventScript_ItemNugget:: @ 8291059
	finditem ITEM_NUGGET
	end

Route132_EventScript_ItemSharpBeak:: @ 8291066
	finditem ITEM_SHARP_BEAK
	end

Route133_EventScript_ItemStarminite:: @ 8291073
	finditem ITEM_STARMINITE
	end

Route133_EventScript_ItemMedichamite:: @ 8291080
	finditem ITEM_MEDICHAMITE
	end

Route133_EventScript_ItemMaxRevive:: @ 829108D
	finditem ITEM_MAX_REVIVE
	end

Route134_EventScript_ItemBlackBelt:: @ 829109A
	finditem ITEM_BLACK_BELT
	end

Route134_EventScript_ItemCometShard:: @ 82910A7
	finditem ITEM_COMET_SHARD
	end

PetalburgCity_EventScript_ItemAbomasite:: @ 82910B4
	finditem ITEM_ABOMASITE
	end

PetalburgCity_EventScript_ItemVenusaurite:: @ 82910C1
	finditem ITEM_VENUSAURITE
	end

MauvilleCity_EventScript_ItemUltraBall:: @ 82910CE
	finditem ITEM_ULTRA_BALL
	end

RustboroCity_EventScript_ItemWiseGlasses:: @ 82910DB
	finditem ITEM_WISE_GLASSES
	end

LilycoveCity_EventScript_ItemLightClay:: @ 82910E8
	finditem ITEM_LIGHT_CLAY
	end

MossdeepCity_EventScript_ItemMiloticite:: @ 82910F5
	finditem ITEM_MILOTICITE
	end

PetalburgWoods_EventScript_ItemRevive:: @ 8291102
	finditem ITEM_REVIVE
	end

PetalburgWoods_EventScript_ItemGreatBall:: @ 829110F
	finditem ITEM_GREAT_BALL
	end

PetalburgWoods_EventScript_ItemBrightPowder:: @ 829111C
	finditem ITEM_BRIGHT_POWDER
	end

PetalburgWoods_EventScript_ItemNugget:: @ 8291129
	finditem ITEM_NUGGET
	end

RusturfTunnel_EventScript_ItemPokeBall:: @ 8291136
	finditem ITEM_POKE_BALL
	end

RusturfTunnel_EventScript_ItemMachampite:: @ 8291143
	finditem ITEM_MACHAMPITE
	end

GraniteCave_1F_EventScript_ItemBlackBelt:: @ 8291150
	finditem ITEM_BLACK_BELT
	end

GraniteCave_B1F_EventScript_ItemPokeBall:: @ 829115D
	finditem ITEM_POKE_BALL
	end

GraniteCave_B2F_EventScript_ItemAggronite:: @ 829116A
	finditem ITEM_AGGRONITE
	end

GraniteCave_B2F_EventScript_ItemNugget:: @ 8291177
	finditem ITEM_NUGGET
	end

JaggedPass_EventScript_ItemAdrenalineOrb:: @ 8291184
	finditem ITEM_ADRENALINE_ORB
	end

FieryPath_EventScript_ItemMistySeed::
	finditem ITEM_MISTY_SEED
	end

FieryPath_EventScript_ItemTerrainExtender:: @ 829119E
	finditem ITEM_TERRAIN_EXTENDER
	end

MeteorFalls_1F_1R_EventScript_ItemZeraorite:: @ 82911AB
	finditem ITEM_ZERAORITE
	end

MeteorFalls_1F_1R_EventScript_ItemCometShard:: @ 82911B8
	finditem ITEM_COMET_SHARD
	end

MeteorFalls_1F_1R_EventScript_ItemFairyFeather:: @ 82911C5
	finditem ITEM_FAIRY_FEATHER
	end

MeteorFalls_1F_1R_EventScript_ItemPPUP:: @ 82911D2
	finditem ITEM_PP_UP
	end

MeteorFalls_B1F_2R_EventScript_ItemDrampanite:: @ 82911DF
	finditem ITEM_DRAMPANITE
	end

NewMauville_Inside_EventScript_ItemFastBall:: @ 82911EC
	finditem ITEM_FAST_BALL
	end

NewMauville_Inside_EventScript_ItemManectite:: @ 82911F9
	goto_if_unset FLAG_BADGE06_GET, EventScript_MegaStoneBadgeLocked
	finditem ITEM_MANECTITE
	end

NewMauville_Inside_EventScript_ItemAmpharosite:: @ 8291206
	finditem ITEM_AMPHAROSITE
	end

NewMauville_Inside_EventScript_ItemRaichuniteY:: @ 8291213
	finditem ITEM_RAICHUNITE_Y, 3
	end

NewMauville_Inside_EventScript_ItemElectirizer:: @ 8291220
	finditem ITEM_ELECTIRIZER
	end

AbandonedShip_Rooms_1F_EventScript_ItemLopunnite:: @ 829122D
	finditem ITEM_LOPUNNITE
	end

AbandonedShip_Rooms_B1F_EventScript_ItemBigNugget:: @ 829123A
	finditem ITEM_BIG_NUGGET
	end

AbandonedShip_Rooms2_B1F_EventScript_ItemLevelBall:: @ 8291247
	finditem ITEM_LEVEL_BALL
	end

AbandonedShip_Room_B1F_EventScript_ItemMaxRepel:: @ 8291254
	finditem ITEM_MAX_REPEL
	end

AbandonedShip_Rooms2_1F_EventScript_ItemLuxuryBall:: @ 8291261
	finditem ITEM_LUXURY_BALL
	end

AbandonedShip_CaptainsOffice_EventScript_ItemStorageKey:: @ 829126E
	finditem ITEM_STORAGE_KEY
	end

AbandonedShip_HiddenFloorRooms_EventScript_ItemTatsugirinite:: @ 829127B
	finditem ITEM_TATSUGIRINITE
	end

AbandonedShip_HiddenFloorRooms_EventScript_ItemScanner:: @ 8291288
	finditem ITEM_SCANNER
	end

AbandonedShip_HiddenFloorRooms_EventScript_ItemLucarionite:: @ 8291295
	finditem ITEM_LUCARIONITE
	end

AbandonedShip_HiddenFloorRooms_EventScript_ItemClefablite:: @ 82912A2
	finditem ITEM_CLEFABLITE
	end

ScorchedSlab_EventScript_ItemSteelixite:: @ 82912AF
	finditem ITEM_STEELIXITE
	end

SafariZone_Northwest_EventScript_ItemGolisopite:: @ 82912BC
	finditem ITEM_GOLISOPITE
	end

SafariZone_North_EventScript_ItemAbsolite:: @ 82912C9
	finditem ITEM_ABSOLITE
	end

SafariZone_Southwest_EventScript_ItemMaxRevive:: @ 82912D6
	finditem ITEM_MAX_REVIVE
	end

SafariZone_Northeast_EventScript_ItemBigPearl:: @ 82912E3
	finditem ITEM_BIG_PEARL
	end

SafariZone_Southeast_EventScript_ItemDelphoxite:: @ 82912F0
	finditem ITEM_DELPHOXITE
	end

MtPyre_2F_EventScript_ItemBanettite:: @ 82912FD
	finditem ITEM_BANETTITE
	end

MtPyre_3F_EventScript_ItemSablenite:: @ 829130A
	finditem ITEM_SABLENITE
	end

MtPyre_4F_EventScript_ItemSeaIncense:: @ 8291317
	finditem ITEM_SEA_INCENSE
	end

MtPyre_5F_EventScript_ItemAlakazite:: @ 8291324
	goto_if_unset FLAG_BADGE06_GET, EventScript_MegaStoneBadgeLocked
	finditem ITEM_ALAKAZITE
	end

MtPyre_6F_EventScript_ItemChimechite:: @ 8291331
	finditem ITEM_CHIMECHITE
	end

MtPyre_Exterior_EventScript_ItemGengarite:: @ 829133E
	finditem ITEM_GENGARITE
	end

MtPyre_Exterior_EventScript_ItemLucarioniteZ:: @ 829134B
	finditem ITEM_LUCARIONITE_Z
	end

AquaHideout_B1F_EventScript_ItemMasterBall:: @ 8291358
	finditem ITEM_MASTER_BALL
	end

AquaHideout_B1F_EventScript_ItemSharpedonite:: @ 8291365
	finditem ITEM_SHARPEDONITE
	end

AquaHideout_B1F_EventScript_ItemMaxElixir:: @ 8291372
	finditem ITEM_MAX_ELIXIR
	end

AquaHideout_B2F_EventScript_ItemNestBall:: @ 829137F
	finditem ITEM_NEST_BALL
	end

Route119_EventScript_ItemNugget:: @ 8291399
	finditem ITEM_NUGGET
	end

ShoalCave_LowTideEntranceRoom_EventScript_ItemBigPearl:: @ 82913C0
	finditem ITEM_BIG_PEARL
	end

ShoalCave_LowTideInnerRoom_EventScript_ItemNugget:: @ 82913CD
	finditem ITEM_NUGGET
	end

ShoalCave_LowTideStairsRoom_EventScript_ItemKangaskhanite::
	finditem ITEM_KANGASKHANITE
	end

ShoalCave_LowTideIceRoom_EventScript_ItemFroslassite:: @ 82913E7
	finditem ITEM_FROSLASSITE
	end

ShoalCave_LowTideIceRoom_EventScript_ItemNeverMeltIce:: @ 82913F4
	finditem ITEM_NEVER_MELT_ICE
	end

SeafloorCavern_Room9_EventScript_ItemDragoninite:: @ 8291401
	finditem ITEM_DRAGONINITE
	end

Route110_TrickHousePuzzle1_EventScript_ItemDawnStone::
	finditem ITEM_DAWN_STONE
	end

Route110_TrickHousePuzzle2_EventScript_ItemDuskStone:: @ 829141B
	finditem ITEM_DUSK_STONE
	end

Route110_TrickHousePuzzle2_EventScript_ItemHeavyBall:: @ 8291428
	finditem ITEM_HEAVY_BALL
	end

Route110_TrickHousePuzzle3_EventScript_ItemBigNugget:: @ 8291435
	finditem ITEM_BIG_NUGGET
	end

Route110_TrickHousePuzzle3_EventScript_ItemHoundoominite:: @ 8291442
	finditem ITEM_HOUNDOOMINITE
	end

Route110_TrickHousePuzzle4_EventScript_ItemScovillainite:: @ 829144F
	finditem ITEM_SCOVILLAINITE
	end

Route110_TrickHousePuzzle6_EventScript_ItemDestinyKnot:: @ 829145C
	finditem ITEM_DESTINY_KNOT
	end

Route110_TrickHousePuzzle7_EventScript_ItemDarkranite:: @ 8291469
	finditem ITEM_DARKRANITE
	end

Route110_TrickHousePuzzle8_EventScript_ItemMewtwoniteX:: @ 8291476
	finditem ITEM_MEWTWONITE_X
	end

VictoryRoad_1F_EventScript_ItemGarchompite:: @ 8291483
	finditem ITEM_GARCHOMPITE
	end

VictoryRoad_1F_EventScript_ItemMetagrossite:: @ 8291490
	finditem ITEM_METAGROSSITE
	end

VictoryRoad_B1F_EventScript_ItemGarchompiteZ:: @ 829149D
	finditem ITEM_GARCHOMPITE_Z
	end

VictoryRoad_B1F_EventScript_ItemCharizarditeY:: @ 82914AA
	finditem ITEM_CHARIZARDITE_Y
	end

VictoryRoad_B2F_EventScript_ItemTyranitarite:: @ 82914B7
	finditem ITEM_TYRANITARITE
	end

ArtisanCave_B1F_EventScript_ItemCometShard:: @ 82914C4
	finditem ITEM_COMET_SHARD
	end

ArtisanCave_1F_EventScript_ItemBigNugget:: @ 82914D1
	finditem ITEM_BIG_NUGGET
	end

MagmaHideout_1F_EventScript_ItemChesnaughtite:: @ 82914DE
	finditem ITEM_CHESNAUGHTITE
	end

MagmaHideout_2F_2R_EventScript_ItemMaxElixir:: @ 82914EB
	finditem ITEM_MAX_ELIXIR
	end

MagmaHideout_2F_2R_EventScript_ItemFullRestore:: @ 82914F8
	finditem ITEM_FULL_RESTORE
	end

MagmaHideout_3F_1R_EventScript_ItemCameruptite:: @ 8291505
	finditem ITEM_CAMERUPTITE
	end

MagmaHideout_3F_2R_EventScript_ItemHeatranite:: @ 8291512
	finditem ITEM_HEATRANITE
	end

MagmaHideout_4F_EventScript_ItemCharizarditeX:: @ 829151F
	goto_if_unset FLAG_BADGE06_GET, EventScript_MegaStoneBadgeLocked
	finditem ITEM_CHARIZARDITE_X
	end

MagmaHideout_3F_3R_EventScript_ItemPpMax:: @ 829152C
	finditem ITEM_PP_MAX
	end

PetalburgWoods_2_EventScript_ItemToxicOrb::
	finditem ITEM_TOXIC_ORB
	end

PetalburgWoods_2_Item_SunStone::
	finditem ITEM_SUN_STONE
	end

PetalburgWoods_3_EventScript_ItemNugget::
	finditem ITEM_NUGGET
	end

PetalburgWoods_3_EventScript_ItemVictreebelite::
	finditem ITEM_VICTREEBELITE
	end

PetalburgWoods_3_EventScript_ItemLuminousMoss::
	finditem ITEM_LUMINOUS_MOSS
	end

Seaspray_Cave_Lure_Ball::
	finditem ITEM_LURE_BALL
	end

Seaspray_Cave_EventScript_ItemSmoothRock::
	finditem ITEM_SMOOTH_ROCK
	end

Seaspray_Cave_EventScript_ItemCovertCloak::
	finditem ITEM_COVERT_CLOAK
	end

Seaspray_Cave_Blastoisinite::
	finditem ITEM_BLASTOISINITE
	end

Seaspray_Cave_EventScript_ItemBaxcalibrite::
	finditem ITEM_BAXCALIBRITE
	end

Seaspray_Cave_B1F_EventScript_ItemDampRock::
	finditem ITEM_DAMP_ROCK
	end

Seaspray_Cave_B1F_EventScript_ItemRockyHelmet::
	finditem ITEM_ROCKY_HELMET
	end

Seaspray_Cave_B1F_ItemRevive::
	finditem ITEM_REVIVE
	end

Seaspray_Cave_B1F_ItemKingsRock::
	finditem ITEM_KINGS_ROCK
	end

GraniteCave_B2F_EventScript_ItemGoldBottleCap::
	finditem ITEM_GOLD_BOTTLE_CAP
	end

EmberPath_EventScript_ItemClearAmulet::
	finditem ITEM_CLEAR_AMULET
	end

EmberPath_EventScript_ItemElectricSeed::
	finditem ITEM_ELECTRIC_SEED
	end

Ashen_Woods_ItemFlame_Orb::
	finditem ITEM_FLAME_ORB
	end

AshenWoods_EventScript_ItemIronBall::
	finditem ITEM_IRON_BALL
	end

AshenWoods_EventScript_ItemMirrorHerb::
	finditem ITEM_MIRROR_HERB
	end

SandstrewnRuins_EventScript_ItemUtilityUmbrella::
	finditem ITEM_UTILITY_UMBRELLA
	end

SandstrewnRuins_EventScript_ItemAbilityShield::
	finditem ITEM_ABILITY_SHIELD
	end

Sandstrewn_Ruins_ItemDomeFossil::
	finditem ITEM_DOME_FOSSIL
	end

Sandstrewn_Ruins_ItemHelixFossil::
	finditem ITEM_HELIX_FOSSIL
	end

Sandstrewn_Ruins_ItemOldAmber::
	finditem ITEM_OLD_AMBER
	end

Sandstrewn_Ruins_ItemSkullFossil::
	finditem ITEM_SKULL_FOSSIL
	end

Sandstrewn_Ruins_ItemArmorFossil::
	finditem ITEM_ARMOR_FOSSIL
	end

Sandstrewn_Ruins_ItemCoverFossil::
	finditem ITEM_COVER_FOSSIL
	end

Sandstrewn_Ruins_ItemPlumeFossil::
	finditem ITEM_PLUME_FOSSIL
	end

Sandstrewn_Ruins_ItemSailFossil::
	finditem ITEM_SAIL_FOSSIL
	end

Sandstrewn_Ruins_ItemJawFossil::
	finditem ITEM_JAW_FOSSIL
	end

SlateportCity_EventScript_ItemLightClay::
	finditem ITEM_LIGHT_CLAY
	end

NewMauville_Inside_EventScript_ItemEelektrossite::
	finditem ITEM_EELEKTROSSITE
	end

Route119_EventScript_ItemGreninjite::
	finditem ITEM_GRENINJITE
	end

MtPyre_Summit_EventScript_ItemStarPiece::
	finditem ITEM_STAR_PIECE
	end

Route111_RuinsExterior_EventScript_ItemStardust::
	finditem ITEM_STARDUST
	end

ShoalCave_LowTideIceRoom_EventScript_ItemGlalitite::
	finditem ITEM_GLALITITE
	end

Route124_EventScript_ItemExcadrite::
	finditem ITEM_EXCADRITE
	end

AquaHideout_B1F_EventScript_ItemBarbaracite::
	finditem ITEM_BARBARACITE
	end

SafariZone_South_EventScript_ItemMalamarite::
	finditem ITEM_MALAMARITE
	end

Route131_EventScript_SplashPlate::
	finditem ITEM_SPLASH_PLATE
	end

ScorchedSlab_EventScript_ItemDuskBall::
	finditem ITEM_DUSK_BALL, 5
	end

GraniteCave_B1F_EventScript_ItemGlimmoranite::
	finditem ITEM_GLIMMORANITE
	end

Route117_EventScript_ItemStardust::
	finditem ITEM_STARDUST
	end

Meteor_Falls1F_2R_EventScript_Salamencite::
	finditem ITEM_SALAMENCITE
	end

Mirage_Tower_4F_EventScript_ItemGhostGem::
	finditem ITEM_GHOST_GEM
	end

TrickHouse5_EventScript_TerrainExtender::
	finditem ITEM_TERRAIN_EXTENDER
	end

DewfordMeadow_EventScript_ItemRedShard::
	finditem ITEM_RED_SHARD
	end

DewfordMeadow_EventScript_RingTarget::
	finditem ITEM_RING_TARGET
	end

DewfordManor_EventScript_ItemNugget::
	finditem ITEM_NUGGET
	end

DewfordManor_EventScript_ItemAirBalloon::
	finditem ITEM_AIR_BALLOON
	end

VerdanturfMeadow_EventScript_ItemLumBerry::
	finditem ITEM_LUM_BERRY
	end

MagmaHideout_2F_2R_EventScript_ItemMagmaStone::
	finditem ITEM_MAGMA_STONE
	end

EverGrandeCity_EventScript_ItemMewtwoniteY::
	finditem ITEM_MEWTWONITE_Y
	end

DewfordMeadow_EventScript_ItemHeatRock::
	finditem ITEM_HEAT_ROCK
	end

DesertUnderpass_EventScript_Flygonite::
	finditem ITEM_FLYGONITE
	end

SeafloorCavern_Room6_EventScript_Kingdranite::
	finditem ITEM_KINGDRANITE
	end

Altering_Cave_EventScript_MasterBall1::
	finditem ITEM_MASTER_BALL
	end

Altering_Cave_EventScript_MasterBall2::
	finditem ITEM_MASTER_BALL
	end

Altering_Cave_EventScript_ItemZygardite::
	finditem ITEM_ZYGARDITE
	end

EventScript_MegaStoneBadgeLocked::
	msgbox Text_MegaStoneBadgeLocked, MSGBOX_NPC
	end

Text_MegaStoneBadgeLocked:
	.string "This stone is waiting for a Trainer\n"
	.string "with six Gym Badges.$"
