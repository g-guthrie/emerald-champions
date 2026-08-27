# Emerald Champions item-economy ledger

This report is generated from current ROM source and the generated battle guide by
`python3 scripts/verdant_item_economy_audit.py --write`.
Ordinary non-Berry competitive held items are free loadout infrastructure. Berries and evolution/form catalysts remain progression, and no fixed reward duplicates the free stock.

## Fixed boss rewards

| Boss | Current item | Earlier fixed source? | Status |
|---|---|---|---|
| Roxanne | ITEM_PROTECTOR | None | Clean |
| Brawly | ITEM_RAZOR_CLAW | None | Clean |
| Wattson | ITEM_ELECTIRIZER | None | Clean |
| Flannery | ITEM_MAGMARIZER | None | Clean |
| Norman | ITEM_LOPUNNITE | None | Clean |
| Winona | ITEM_ALTARIANITE | None | Clean |
| Tate & Liza | ITEM_METAGROSSITE | None | Clean |
| Juan | ITEM_MILOTICITE | None | Clean |

## Tested canonical direction

| Boss | Proposed item | Earlier collision | Required action |
|---|---|---|---|
| Brawly | ITEM_RAZOR_CLAW | None | implemented |
| Norman | ITEM_LOPUNNITE | None | implemented |
| Tate & Liza | ITEM_METAGROSSITE | None | implemented |
| Flannery | ITEM_MAGMARIZER | None | implemented |
| Winona | ITEM_ALTARIANITE | None | implemented |
| Juan | ITEM_MILOTICITE | None | implemented |

## Exploration summary

- Visible Rare Candy pickups: **11**.
- Former technical-reward Rare Candy pickups: **11**.
- Hidden Rare Candy pickups: **12**.
- The eleven visible sources are intentionally retained at early campaign gates; mid and late former-technical filler has been replaced with finite rewards.

| Retained label | Area | Guide order | Badges |
|---|---|---:|---:|
| Route111_EventScript_ItemTM37 | Route111 | 131 | 3 |
| Route116_EventScript_TM77StruggleBug | Route116 | 31 | 1 |
| RustboroCity_EventScript_ItemAbilityCapsule | RustboroCity | 27 | 0 |
| PetalburgWoods_2_Item_TM80Venoshock | PetalburgWoods_2 | 24 | 0 |
| PetalburgWoods_3_TM34_SludgeWave | PetalburgWoods_3 | 24 | 0 |
| Seaspray_Cave_Stealth_Rock | Seaspray_Cave | 53 | 1 |
| Seaspray_Cave_ItemStoneEdge | Seaspray_Cave | 53 | 1 |
| Seaspray_Cave_B1F_ItemFreezeDry | Seaspray_Cave_B1F | 53 | 1 |
| Granite_Cave_B2F_TM31_Brick_Break | GraniteCave_B2F | 53 | 1 |
| GraniteCave_B1F_EventScript_ItemTM65ShadowClaw | GraniteCave_B1F | 53 | 1 |
| DewfordManor_EventScript_TM100Curse | DewfordManor_1F | 53 | 1 |

## First fixed acquisition

| Item | Legacy gate | First fixed source |
|---|---:|---|
| ITEM_ABOMASITE | not_in_discovery_shop | Seaspray_Cave_B1F / visible_pickup |
| ITEM_ABSOLITE | not_in_discovery_shop | SafariZone_North / visible_pickup |
| ITEM_ABSORB_BULB | 1 | src/item.c / badge_shop |
| ITEM_ADRENALINE_ORB | discovery | None |
| ITEM_AERODACTYLITE | not_in_discovery_shop | MirageTower_4F / visible_pickup |
| ITEM_AGGRONITE | not_in_discovery_shop | Route116 / fixed_gift |
| ITEM_AIR_BALLOON | 4 | src/item.c / badge_shop |
| ITEM_ALAKAZITE | not_in_discovery_shop | SlateportCity / visible_pickup |
| ITEM_ALTARIANITE | not_in_discovery_shop | FortreeCity_Gym / fixed_gift |
| ITEM_AMPHAROSITE | not_in_discovery_shop | NewMauville_Inside / visible_pickup |
| ITEM_ASSAULT_VEST | 0 | src/item.c / badge_shop |
| ITEM_AUDINITE | not_in_discovery_shop | Route104 / visible_pickup |
| ITEM_BANETTITE | not_in_discovery_shop | DewfordManor_1F / visible_pickup |
| ITEM_BEEDRILLITE | not_in_discovery_shop | PetalburgWoods_3 / visible_pickup |
| ITEM_BIG_ROOT | not_in_discovery_shop | PetalburgWoods_3 / hidden_item |
| ITEM_BINDING_BAND | not_in_discovery_shop | Route116 / visible_pickup |
| ITEM_BLACK_BELT | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_BLACK_GLASSES | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_BLACK_SLUDGE | discovery | None |
| ITEM_BLASTOISINITE | not_in_discovery_shop | Seaspray_Cave / visible_pickup |
| ITEM_BLAZIKENITE | not_in_discovery_shop | GraniteCave_StevensRoom / fixed_bundle |
| ITEM_BLUNDER_POLICY | discovery | None |
| ITEM_BRIGHT_POWDER | discovery | None |
| ITEM_BUG_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_BUTTERFRENITE | not_in_discovery_shop | DewfordMeadow / visible_pickup |
| ITEM_CAMERUPTITE | not_in_discovery_shop | MagmaHideout_3F_1R / visible_pickup |
| ITEM_CELL_BATTERY | 0 | src/item.c / badge_shop |
| ITEM_CHANDELURITE | not_in_discovery_shop | MtPyre_Summit / visible_pickup |
| ITEM_CHARCOAL | not_in_discovery_shop | None |
| ITEM_CHARIZARDITE_X | not_in_discovery_shop | FieryPath / visible_pickup |
| ITEM_CHARIZARDITE_Y | not_in_discovery_shop | EmberPath / visible_pickup |
| ITEM_CHOICE_BAND | 0 | src/item.c / badge_shop |
| ITEM_CHOICE_SCARF | 0 | src/item.c / badge_shop |
| ITEM_CHOICE_SPECS | 0 | src/item.c / badge_shop |
| ITEM_DAMP_ROCK | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_DARK_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_DESTINY_KNOT | not_in_discovery_shop | Route111_WinstrateFamilysHouse / fixed_gift |
| ITEM_DIANCITE | not_in_discovery_shop | PacifidlogTown_House2 / fixed_gift |
| ITEM_DIRE_HIT | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_DRAGONINITE | not_in_discovery_shop | MeteorFalls_1F_1R / visible_pickup |
| ITEM_DRAGON_FANG | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_DRAGON_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_EJECT_BUTTON | 1 | src/item.c / badge_shop |
| ITEM_EJECT_PACK | discovery | None |
| ITEM_ELECTRIC_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_ELECTRIC_SEED | 5 | src/item.c / badge_shop |
| ITEM_EMBOARITE | not_in_discovery_shop | FieryPath / visible_pickup |
| ITEM_EVIOLITE | 0 | src/item.c / badge_shop |
| ITEM_EXCADRITE | not_in_discovery_shop | SandstrewnRuins / visible_pickup |
| ITEM_EXPERT_BELT | discovery | None |
| ITEM_FAIRY_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_FERALIGITE | not_in_discovery_shop | SeafloorCavern_Room9 / visible_pickup |
| ITEM_FIGHTING_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_FIRE_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_FLAME_ORB | discovery | None |
| ITEM_FLOAT_STONE | not_in_discovery_shop | RustboroCity_Flat2_2F / fixed_gift |
| ITEM_FLUFFY_TAIL | not_in_discovery_shop | LilycoveCity_DepartmentStore_2F / specialty_shop |
| ITEM_FLYGONITE | not_in_discovery_shop | DesertUnderpass / visible_pickup |
| ITEM_FLYING_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_FOCUS_BAND | discovery | None |
| ITEM_FOCUS_SASH | 0 | src/item.c / badge_shop |
| ITEM_GALLADITE | not_in_discovery_shop | FallarborTown_CozmosHouse / fixed_gift |
| ITEM_GARCHOMPITE | not_in_discovery_shop | SandstrewnRuins / visible_pickup |
| ITEM_GARDEVOIRITE | not_in_discovery_shop | VerdanturfTown_WandasHouse / fixed_gift |
| ITEM_GENGARITE | not_in_discovery_shop | MtPyre_Exterior / visible_pickup |
| ITEM_GHOST_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_GLALITITE | not_in_discovery_shop | ShoalCave_LowTideIceRoom / visible_pickup |
| ITEM_GLIMMORANITE | not_in_discovery_shop | Route119 / visible_pickup |
| ITEM_GRASSY_SEED | 5 | src/item.c / badge_shop |
| ITEM_GRASS_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_GRENINJITE | not_in_discovery_shop | Seaspray_Cave / visible_pickup |
| ITEM_GRIP_CLAW | not_in_discovery_shop | None |
| ITEM_GROUND_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_GUARD_SPEC | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_GYARADOSITE | not_in_discovery_shop | Route118 / fixed_gift |
| ITEM_HARD_STONE | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_HAWLUCHANITE | not_in_discovery_shop | Route119 / visible_pickup |
| ITEM_HEAT_ROCK | not_in_discovery_shop | Route119_WeatherInstitute_1F / fixed_bundle |
| ITEM_HEAVY_DUTY_BOOTS | 0 | src/item.c / badge_shop |
| ITEM_HERACRONITE | not_in_discovery_shop | Route115 / visible_pickup |
| ITEM_HOUNDOOMINITE | not_in_discovery_shop | Route112 / visible_pickup |
| ITEM_ICE_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_ICY_ROCK | not_in_discovery_shop | Route119_WeatherInstitute_1F / fixed_bundle |
| ITEM_IRON_BALL | not_in_discovery_shop | None |
| ITEM_KANGASKHANITE | not_in_discovery_shop | SafariZone_Northeast / visible_pickup |
| ITEM_KINGDRANITE | not_in_discovery_shop | SeafloorCavern_Room6 / visible_pickup |
| ITEM_KINGLERITE | not_in_discovery_shop | Route106 / visible_pickup |
| ITEM_KINGS_ROCK | discovery | Seaspray_Cave_B1F / visible_pickup |
| ITEM_LAGGING_TAIL | not_in_discovery_shop | None |
| ITEM_LAPRASITE | not_in_discovery_shop | Route127 / visible_pickup |
| ITEM_LATIASITE | not_in_discovery_shop | SouthernIsland_Interior / fixed_bundle |
| ITEM_LATIOSITE | not_in_discovery_shop | SouthernIsland_Interior / fixed_bundle |
| ITEM_LEEK | not_in_discovery_shop | MauvilleCity_Mart / specialty_shop |
| ITEM_LEFTOVERS | 0 | src/item.c / badge_shop |
| ITEM_LIFE_ORB | 0 | src/item.c / badge_shop |
| ITEM_LIGHT_BALL | not_in_discovery_shop | MauvilleCity_Mart / specialty_shop |
| ITEM_LIGHT_CLAY | discovery | None |
| ITEM_LOPUNNITE | not_in_discovery_shop | PetalburgCity_Gym / fixed_gift |
| ITEM_LUCARIONITE | not_in_discovery_shop | AbandonedShip_HiddenFloorRooms / visible_pickup |
| ITEM_LUCKY_PUNCH | not_in_discovery_shop | None |
| ITEM_LUMINOUS_MOSS | 3 | src/item.c / badge_shop |
| ITEM_MACHAMPITE | not_in_discovery_shop | Route121 / visible_pickup |
| ITEM_MAGNET | not_in_discovery_shop | Route110_TrickHouseEnd / fixed_gift |
| ITEM_MALAMARITE | not_in_discovery_shop | DewfordMeadow / visible_pickup |
| ITEM_MANECTITE | not_in_discovery_shop | Route110 / visible_pickup |
| ITEM_MAWILITE | not_in_discovery_shop | Route117 / visible_pickup |
| ITEM_MEDICHAMITE | not_in_discovery_shop | Route111_RuinsExterior / visible_pickup |
| ITEM_MEGANIUMITE | not_in_discovery_shop | PetalburgWoods_3 / visible_pickup |
| ITEM_MENTAL_HERB | 0 | src/item.c / badge_shop |
| ITEM_METAGROSSITE | not_in_discovery_shop | MossdeepCity_Gym / fixed_gift |
| ITEM_METAL_COAT | not_in_discovery_shop | GraniteCave_StevensRoom / fixed_bundle |
| ITEM_METAL_POWDER | not_in_discovery_shop | MauvilleCity_Mart / specialty_shop |
| ITEM_METRONOME | discovery | None |
| ITEM_MEWTWONITE_X | not_in_discovery_shop | Route110_TrickHousePuzzle6 / visible_pickup |
| ITEM_MEWTWONITE_Y | not_in_discovery_shop | EverGrandeCity / visible_pickup |
| ITEM_MILOTICITE | not_in_discovery_shop | SootopolisCity_Gym_1F / fixed_gift |
| ITEM_MIRACLE_SEED | not_in_discovery_shop | Route104 / fixed_gift |
| ITEM_MISTY_SEED | 5 | src/item.c / badge_shop |
| ITEM_MUSCLE_BAND | discovery | None |
| ITEM_MYSTIC_WATER | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_NEVER_MELT_ICE | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_NORMAL_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_PIDGEOTITE | not_in_discovery_shop | Route120 / visible_pickup |
| ITEM_PINSIRITE | not_in_discovery_shop | AshenWoods / visible_pickup |
| ITEM_POISON_BARB | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_POISON_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_POWER_ANKLET | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_POWER_BAND | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_POWER_BELT | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_POWER_BRACER | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_POWER_HERB | 4 | src/item.c / badge_shop |
| ITEM_POWER_LENS | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_POWER_WEIGHT | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_PROTECTIVE_PADS | discovery | None |
| ITEM_PSYCHIC_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_PSYCHIC_SEED | 5 | src/item.c / badge_shop |
| ITEM_QUICK_CLAW | discovery | None |
| ITEM_QUICK_POWDER | not_in_discovery_shop | None |
| ITEM_RAICHUNITE_X | not_in_discovery_shop | NewMauville_Inside / visible_pickup |
| ITEM_RAICHUNITE_Y | not_in_discovery_shop | Route103 / visible_pickup |
| ITEM_RAZOR_CLAW | not_in_discovery_shop | DewfordTown_Gym / fixed_gift |
| ITEM_RAZOR_FANG | discovery | SlateportCity_OceanicMuseum_1F / fixed_gift |
| ITEM_RED_CARD | 0 | src/item.c / badge_shop |
| ITEM_RING_TARGET | not_in_discovery_shop | DewfordMeadow / visible_pickup |
| ITEM_ROCKY_HELMET | 0 | src/item.c / badge_shop |
| ITEM_ROCK_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_ROOM_SERVICE | discovery | None |
| ITEM_SABLENITE | not_in_discovery_shop | RusturfTunnel / visible_pickup |
| ITEM_SAFETY_GOGGLES | discovery | None |
| ITEM_SALAMENCITE | not_in_discovery_shop | MeteorFalls_1F_2R / visible_pickup |
| ITEM_SCEPTILITE | not_in_discovery_shop | GraniteCave_StevensRoom / fixed_bundle |
| ITEM_SCIZORITE | not_in_discovery_shop | Route119 / visible_pickup |
| ITEM_SCOPE_LENS | discovery | None |
| ITEM_SHARPEDONITE | not_in_discovery_shop | AquaHideout_B1F / visible_pickup |
| ITEM_SHARP_BEAK | not_in_discovery_shop | None |
| ITEM_SHED_SHELL | discovery | None |
| ITEM_SHELL_BELL | not_in_discovery_shop | ShoalCave_LowTideEntranceRoom / fixed_gift |
| ITEM_SILK_SCARF | not_in_discovery_shop | None |
| ITEM_SILVER_POWDER | not_in_discovery_shop | VerdanturfTown_PokemonCenter_1F / fixed_gift |
| ITEM_SLOWBRONITE | not_in_discovery_shop | ShoalCave_LowTideEntranceRoom / fixed_gift |
| ITEM_SMOOTH_ROCK | not_in_discovery_shop | Route119_WeatherInstitute_1F / fixed_bundle |
| ITEM_SNOWBALL | 2 | src/item.c / badge_shop |
| ITEM_SOFT_SAND | not_in_discovery_shop | Route109 / fixed_gift |
| ITEM_SPELL_TAG | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_STEELIXITE | not_in_discovery_shop | GraniteCave_B2F / visible_pickup |
| ITEM_STEEL_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_STICKY_BARB | not_in_discovery_shop | None |
| ITEM_SWAMPERTITE | not_in_discovery_shop | GraniteCave_StevensRoom / fixed_bundle |
| ITEM_TATSUGIRINITE | not_in_discovery_shop | Route132 / visible_pickup |
| ITEM_TERRAIN_EXTENDER | discovery | None |
| ITEM_THICK_CLUB | not_in_discovery_shop | MauvilleCity_Mart / specialty_shop |
| ITEM_THROAT_SPRAY | discovery | None |
| ITEM_TOXIC_ORB | discovery | None |
| ITEM_TRAINING_BAND | not_in_discovery_shop | SlateportCity / specialty_shop |
| ITEM_TWISTED_SPOON | not_in_discovery_shop | LilycoveCity_DepartmentStore_4F / specialty_shop |
| ITEM_TYRANITARITE | not_in_discovery_shop | ScorchedSlab_B2F / visible_pickup |
| ITEM_UTILITY_UMBRELLA | discovery | None |
| ITEM_VENUSAURITE | not_in_discovery_shop | PetalburgCity / visible_pickup |
| ITEM_WATER_GEM | not_in_discovery_shop | LavaridgeTown_Mart / specialty_shop |
| ITEM_WEAKNESS_POLICY | 6 | src/item.c / badge_shop |
| ITEM_WHITE_HERB | 2 | src/item.c / badge_shop |
| ITEM_WIDE_LENS | discovery | None |
| ITEM_WISE_GLASSES | discovery | None |
| ITEM_X_ACCURACY | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_X_ATTACK | not_in_discovery_shop | PetalburgWoods / visible_pickup |
| ITEM_X_DEFENSE | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_X_SPEED | not_in_discovery_shop | MauvilleCity / visible_pickup |
| ITEM_X_SP_ATK | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_X_SP_DEF | not_in_discovery_shop | VerdanturfTown_Mart / specialty_shop |
| ITEM_ZOOM_LENS | discovery | None |
