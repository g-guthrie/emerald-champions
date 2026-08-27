#!/usr/bin/env python3
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_doubles_conversion.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_underused_balance.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_encounter_upgrade.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_ordinary_availability.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_bespoke_wild_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_battle_set_presets.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verify_capture_ready_wilds.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verify_multi_battle_sets.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verify_competitive_references.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_marquee_design_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_marquee_collision_audit.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_marquee_report.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_custom_teams.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_team_polish.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_team_quality_audit.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_battle_guide.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_battle_sequence_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_physical_encounter_atlas.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_species_usage_ledger.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_chapter_review_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_bespoke_battle_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_evolution_stage_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_battle_context.py"), "--next", "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_ai_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_trainer_dialogue_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_gen9_curated.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verify_verdant_gen9_battle_mechanics.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_logical_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_item_economy_audit.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_reward_rewrite.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_availability_report.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_story_progression_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/emerald_champions_route_sign_audit.py")],
    cwd=ROOT,
    check=True,
)


def read(path: str) -> str:
    return (ROOT / path).read_text()


champions_megas = {
    "MEGANIUM_MEGA": ("meganium", "meganiumite"),
    "FERALIGATR_MEGA": ("feraligatr", "feraligite"),
    "EMBOAR_MEGA": ("emboar", "emboarite"),
    "RAICHU_MEGA_X": ("raichu_x", "raichunite_x"),
    "RAICHU_MEGA_Y": ("raichu_y", "raichunite_y"),
    "DRAGONITE_MEGA": ("dragonite", "dragoninite"),
    "EXCADRILL_MEGA": ("excadrill", "excadrite"),
    "MALAMAR_MEGA": ("malamar", "malamarite"),
    "CHANDELURE_MEGA": ("chandelure", "chandelurite"),
    "HAWLUCHA_MEGA": ("hawlucha", "hawluchanite"),
    "GRENINJA_MEGA": ("greninja", "greninjite"),
}

world_mega_stones = {
    "PetalburgWoods_3_TM86_GrassKnot": "ITEM_MEGANIUMITE",
    "SeafloorCavern_Room9_EventScript_ItemTM26": "ITEM_FERALIGITE",
    "FieryPath_EventScript_ItemTM06": "ITEM_EMBOARITE",
    "NewMauville_Inside_EventScript_ItemTM91FlashCannon": "ITEM_RAICHUNITE_X",
    "Route103_EventScript_ItemTM40AerialAce": "ITEM_RAICHUNITE_Y",
    "MeteorFalls_1F_1R_EventScript_ItemTM59DragonPulse": "ITEM_DRAGONINITE",
    "Sandstrewn_Ruins_ItemLeechLife": "ITEM_EXCADRITE",
    "DewfordMeadow_EventScript_TM95Snarl": "ITEM_MALAMARITE",
    "MtPyre_Summit_EventScript_ItemTM61_WillOWisp": "ITEM_CHANDELURITE",
    "Route119_EventScript_ItemTM62_Acrobatics": "ITEM_HAWLUCHANITE",
    "Seaspray_Cave_Water_Pulse": "ITEM_GRENINJITE",
}

trainer_mega_showcases = {
    "sParty_Rose3": ("SPECIES_MEGANIUM", "ITEM_MEGANIUMITE"),
    "sParty_MattMtPyre": ("SPECIES_FERALIGATR", "ITEM_FERALIGITE"),
    "sParty_Flannery1": ("SPECIES_EMBOAR", "ITEM_EMBOARITE"),
    "sParty_Isabel3": ("SPECIES_RAICHU", "ITEM_RAICHUNITE_X"),
    "sParty_Wattson1": ("SPECIES_RAICHU", "ITEM_RAICHUNITE_Y"),
    "sParty_Lydia3": ("SPECIES_DRAGONITE", "ITEM_DRAGONINITE"),
    "sParty_TabithaMagmaHideout": ("SPECIES_EXCADRILL", "ITEM_EXCADRITE"),
    "sParty_Archie1": ("SPECIES_MALAMAR", "ITEM_MALAMARITE"),
    "sParty_Isaac3": ("SPECIES_CHANDELURE", "ITEM_CHANDELURITE"),
    "sParty_Maria3": ("SPECIES_HAWLUCHA", "ITEM_HAWLUCHANITE"),
    "sParty_Lao3": ("SPECIES_GRENINJA", "ITEM_GRENINJITE"),
}

mega_family_bases = (
    "SPECIES_CHIKORITA",
    "SPECIES_TOTODILE",
    "SPECIES_TEPIG",
    "SPECIES_PIKACHU",
    "SPECIES_DRATINI",
    "SPECIES_DRILBUR",
    "SPECIES_INKAY",
    "SPECIES_LITWICK",
    "SPECIES_HAWLUCHA",
    "SPECIES_FROAKIE",
)

starter_battle_items = (
    "ITEM_LIFE_ORB",
    "ITEM_CHOICE_BAND",
    "ITEM_CHOICE_SPECS",
    "ITEM_CHOICE_SCARF",
    "ITEM_FOCUS_SASH",
    "ITEM_ASSAULT_VEST",
    "ITEM_EVIOLITE",
    "ITEM_LEFTOVERS",
    "ITEM_ROCKY_HELMET",
    "ITEM_HEAVY_DUTY_BOOTS",
)


def array_body(path: str, name: str) -> str:
    return read(path).split(f"{name}[] = {{", 1)[1].split("};", 1)[0]


wild_encounters = json.loads(read("src/data/wild_encounters.json"))["wild_encounter_groups"][0]
wild_slot_counts = {
    field["type"]: len(field["encounter_rates"])
    for field in wild_encounters["fields"]
}
wild_table_lengths_match = all(
    len(encounter[field_name]["mons"]) == slot_count
    for encounter in wild_encounters["encounters"]
    for field_name, slot_count in wild_slot_counts.items()
    if field_name in encounter
)
start_menu_source = read("src/start_menu.c")
pokedex_source = read("src/pokedex.c")
wild_encounter_source = read("src/wild_encounter.c")
normal_start_menu = start_menu_source.split("static void BuildNormalStartMenu(void)", 2)[2].split(
    "static void BuildSafariZoneStartMenu(void)", 1
)[0]
party_menu_source = read("src/party_menu.c")
item_source = read("src/item.c")
battle_item_unlocks = item_source.split("sBattleItemUnlocks[]", 1)[1].split("};", 1)[0]
general_mart_stock = read("data/scripts/general_mart.inc")
all_map_json = "\n".join(path.read_text() for path in (ROOT / "data" / "maps").rglob("map.json"))
save_source = read("src/save.c")
field_specials_source = read("src/field_specials.c")
pokemon_source = read("src/pokemon.c")
frontier_util_source = read("src/frontier_util.c")
factory_source = read("src/battle_factory.c")
dome_source = read("src/battle_dome.c")
arena_source = read("src/battle_arena.c")
battle_anim_script_source = read("data/battle_anim_scripts.s")
battle_anim_source = read("src/battle_anim.c")
battle_anim_new_source = read("src/battle_anim_new.c")
battle_script_command_source = read("src/battle_script_commands.c")
battle_util_source = read("src/battle_util.c")
pc_tutor_source = read("data/scripts/pokemon_center_move_tutor.inc")
pc_menu_block = field_specials_source.split("[SCROLL_MULTI_POKE_CENTER_TUTOR] =", 1)[1].split("},", 1)[0]
battle_set_runtime = read("src/verdant_battle_sets.c")
battle_set_generator_source = read("scripts/verdant_battle_set_presets.py")
base_stats_source = read("src/data/pokemon/base_stats.h")
form_reviews_100 = read("docs/battle_set_reviews/100_forms_899_1010.json")
form_reviews_110 = read("docs/battle_set_reviews/110_forms_1011_1120.json")
form_reviews_120 = read("docs/battle_set_reviews/120_forms_1121_1225.json")

ported_gen8_move_animations = (
    "Move_ZIPPY_ZAP",
    "Move_SPLISHY_SPLASH",
    "Move_FLOATY_FALL",
    "Move_PIKA_PAPOW",
    "Move_BOUNCY_BUBBLE",
    "Move_BUZZY_BUZZ",
    "Move_SIZZLY_SLIDE",
    "Move_GLITZY_GLOW",
    "Move_BADDY_BAD",
    "Move_SAPPY_SEED",
    "Move_FREEZY_FROST",
    "Move_SPARKLY_SWIRL",
    "Move_VEEVEE_VOLLEY",
    "Move_EXPANDING_FORCE",
    "Move_SCALE_SHOT",
    "Move_METEOR_BEAM",
    "Move_MISTY_EXPLOSION",
    "Move_GRASSY_GLIDE",
    "Move_RISING_VOLTAGE",
    "Move_SKITTER_SMACK",
    "Move_LASH_OUT",
    "Move_POLTERGEIST",
    "Move_CORROSIVE_GAS",
    "Move_COACHING",
    "Move_FLIP_TURN",
    "Move_TRIPLE_AXEL",
    "Move_DUAL_WINGBEAT",
    "Move_SCORCHING_SANDS",
    "Move_WICKED_BLOW",
    "Move_SURGING_STRIKES",
    "Move_THUNDER_CAGE",
    "Move_DRAGON_ENERGY",
    "Move_FREEZING_GLARE",
    "Move_FIERY_WRATH",
    "Move_THUNDEROUS_KICK",
    "Move_GLACIAL_LANCE",
    "Move_ASTRAL_BARRAGE",
    "Move_EERIE_SPELL",
)

verdant_world_item_migrations = (
    ("PETALBURG_WOODS_3_GRASS_KNOT", "ITEM_MEGANIUMITE", "FLAG_VERDANT_MIGRATED_MEGANIUMITE"),
    ("FLAG_ITEM_SEAFLOOR_CAVERN_ROOM_9_TM_26", "ITEM_FERALIGITE", "FLAG_VERDANT_MIGRATED_FERALIGITE"),
    ("FLAG_ITEM_FIERY_PATH_TM06", "ITEM_EMBOARITE", "FLAG_VERDANT_MIGRATED_EMBOARITE"),
    ("FLAG_ITEM_NEW_MAUVILLE_INSIDE_TM91", "ITEM_RAICHUNITE_X", "FLAG_VERDANT_MIGRATED_RAICHUNITE_X"),
    ("FLAG_ITEM_ROUTE_103_TM40_AERIAL_ACE", "ITEM_RAICHUNITE_Y", "FLAG_VERDANT_MIGRATED_RAICHUNITE_Y"),
    ("FLAG_ITEM_METEOR_FALLS_1F_1R_TM59_DRAGON_PULSE", "ITEM_DRAGONINITE", "FLAG_VERDANT_MIGRATED_DRAGONINITE"),
    ("FLAG_SANDSTREWN_RUINS_LEECH_LIFE", "ITEM_EXCADRITE", "FLAG_VERDANT_MIGRATED_EXCADRITE"),
    ("FLAG_ITEM_DEWFORD_MEADOW_TM95", "ITEM_MALAMARITE", "FLAG_VERDANT_MIGRATED_MALAMARITE"),
    ("FLAG_ITEM_MT_PYRE_SUMMIT_TM61_WILLOWISP", "ITEM_CHANDELURITE", "FLAG_VERDANT_MIGRATED_CHANDELURITE"),
    ("FLAG_ITEM_ROUTE_119_TM62_ACROBATICS", "ITEM_HAWLUCHANITE", "FLAG_VERDANT_MIGRATED_HAWLUCHANITE"),
    ("FLAG_SEASPRAY_CAVE_WATER_PULSE", "ITEM_GRENINJITE", "FLAG_VERDANT_MIGRATED_GRENINJITE"),
    ("FLAG_ITEM_ROUTE_109_RARE_CANDY", "ITEM_ADRENALINE_ORB", "FLAG_VERDANT_MIGRATED_ADRENALINE_ORB"),
    ("FLAG_ITEM_TRICK_HOUSE_PUZZLE_4_ASSAULT_VEST", "ITEM_BLUNDER_POLICY", "FLAG_VERDANT_MIGRATED_BLUNDER_POLICY"),
    ("FLAG_ITEM_SHOAL_CAVE_INNER_ROOM_RARE_CANDY", "ITEM_SHED_SHELL", "FLAG_VERDANT_MIGRATED_SHED_SHELL"),
)


def one_time_gift_is_retryable(path: str, item: str) -> bool:
    source = read(path)
    gift = f"giveitem {item}"
    if gift not in source:
        return False
    after_gift = source.split(gift, 1)[1][:180]
    return (
        "compare VAR_RESULT, FALSE" in after_gift
        and "goto_if_eq Common_EventScript_ShowBagIsFull" in after_gift
        and "setflag " in after_gift
        and after_gift.index("compare VAR_RESULT, FALSE")
        < after_gift.index("goto_if_eq Common_EventScript_ShowBagIsFull")
        < after_gift.index("setflag ")
    )


def battle_animation_body(label: str) -> str:
    match = re.search(
        rf"^{re.escape(label)}::\s*\n(.*?)(?=^Move_[A-Z0-9_]+:{{1,2}}|\Z)",
        battle_anim_script_source,
        re.MULTILINE | re.DOTALL,
    )
    return "" if match is None else match.group(1)


font1_widths = [
    int(value)
    for line in read("graphics/fonts/font1_latin_widths.inc").splitlines()
    for value in re.findall(r"\d+", line)
]
font1_charmap = {}
for line in read("charmap.txt").splitlines():
    match = re.match(r"'(.*)'\s*=\s*([0-9A-Fa-f]{2})\s*$", line)
    if match:
        char = match.group(1)
        if char == r"\'":
            char = "'"
        if len(char) == 1:
            font1_charmap[char] = int(match.group(2), 16)


def font1_text_width(text: str) -> int:
    return sum(font1_widths[font1_charmap[char]] for char in text if char in font1_charmap)


def render_static_placeholders(text: str) -> str:
    return text.replace("{POKEBLOCK}", "Pokéblock").replace("{PKMN}", "Pokémon")


item_names = re.findall(r'\.name\s*=\s*_\("([^"]*)"\)', read("src/data/items.h"))
pc_held_message_max_width = max(
    font1_text_width(f"{name} is held.")
    for name in item_names
    if "{" not in name
)
item_description_literal_widths = [
    font1_text_width(render_static_placeholders(line))
    for body in re.findall(
        r"static const u8\s+\w+Desc\[\]\s*=\s*_\((.*?)\);",
        read("src/data/text/item_descriptions.h"),
        re.DOTALL,
    )
    for line in "".join(re.findall(r'"([^"\\]*(?:\\.[^"\\]*)*)"', body)).replace(r"\n", "\n").splitlines()
    if "{" not in render_static_placeholders(line)
]
ported_animation_bodies = tuple(battle_animation_body(label) for label in ported_gen8_move_animations)
astral_barrage_body = battle_animation_body("Move_ASTRAL_BARRAGE")
fiery_wrath_body = battle_animation_body("Move_FIERY_WRATH")
fiery_wrath_geyser_body = fiery_wrath_body.split("FieryWrathGeyser:", 1)[1] if "FieryWrathGeyser:" in fiery_wrath_body else ""
moveend_target_visible = battle_script_command_source.split("case MOVEEND_TARGET_VISIBLE:", 1)[1].split(
    "case MOVEEND_ITEM_EFFECTS_TARGET:", 1
)[0]
moveend_item_order = battle_script_command_source.split("case MOVEEND_ITEM_EFFECTS_TARGET:", 1)[1].split(
    "case MOVEEND_ITEM_EFFECTS_ALL:", 1
)[0]


checks = {
    "party actions preserve all 11 choices in a native scrolling list": (
        "#define PARTY_MENU_MAX_ACTIONS               (MAX_MON_MOVES + 7)" in party_menu_source
        and "u8 actions[PARTY_MENU_MAX_ACTIONS];" in party_menu_source
        and "ListMenu_ProcessInput(sPartyMenuInternal->actionListTaskId)" in party_menu_source
        and "PARTY_MENU_MAX_VISIBLE_ACTIONS       8" in party_menu_source
        and "MENU_NATURE" not in read("src/data/party_menu.h")
    ),
    "party action scrolling owns and releases list state": (
        "DestroyPartyActionList();" in party_menu_source
        and "RemoveScrollIndicatorArrowPair" in party_menu_source
        and "DestroyListMenuTask" in party_menu_source
        and "selectedActionIndex" in party_menu_source
    ),
    "party field moves use the selected scrolling-list row": (
        "sPartyMenuInternal->selectedActionIndex = input;" in party_menu_source
        and "sPartyMenuInternal->selectedActionIndex = sPartyMenuInternal->numActions - 1;" in party_menu_source
        and "sPartyMenuInternal->actions[sPartyMenuInternal->selectedActionIndex] - MENU_FIELD_MOVES"
        in party_menu_source
        and "sPartyMenuInternal->actions[Menu_GetCursorPos()] - MENU_FIELD_MOVES" not in party_menu_source
    ),
    "bag item icons use the non-flickering double buffer": (
        "RemoveBagItemIconSprite(gBagMenu->itemIconSlot ^ 1);" in read("src/item_menu.c")
        and "DestroySpriteAndFreeResources(&gSprites[spriteId[id]]);" in read("src/item_menu_icons.c")
        and "spriteId[id ^ 1] != SPRITE_NONE" in read("src/item_menu_icons.c")
        and "void HideBagItemIconSprite(u8 id)" in read("src/item_menu_icons.c")
    ),
    "PC held-item messages fit their native 18-tile window": (
        'gText_ItemIsNowHeld[] = _("{DYNAMIC 0} is held.")' in read("src/strings.c")
        and 'gText_ChangedToNewItem[] = _("{DYNAMIC 0} is held.")' in read("src/strings.c")
        and pc_held_message_max_width <= 18 * 8
    ),
    "Pokemon Center exposes one all-legal move teacher": (
        "task->tNumItems = 7;" in read("src/field_specials.c")
        and "gText_LearnANewMove" not in pc_menu_block
        and "case 0, PKMN_Center_Move_Tutor_MoveTutorIntro" in pc_tutor_source
        and "case 5, PKMN_Center_BattleSet_ChooseMon" in pc_tutor_source
        and "case 6, PKMN_Center_Move_Tutor_General_Exit" in pc_tutor_source
    ),
    "Day Care UI does not promise disabled level gains": (
        "level = GetLevelFromBoxMonExp(&daycare->mons[i].mon);" in read("src/daycare.c")
        and "Pokémon don't gain levels here" in read("data/scripts/day_care.inc")
        and "Our fixed care fee" in read("data/scripts/day_care.inc")
    ),
    "stat service copy matches implemented EV and IV controls": (
        "or raised by giving them Vitamins" not in read("data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc")
        and "to whatever" not in read("data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc")
        and "IVs were adjusted" in read("data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc")
    ),
    "former held-item rewards now provide finite progression": (
        "giveitem ITEM_ALTARIANITE" in read("data/maps/FortreeCity_Gym/scripts.inc")
        and "finditem ITEM_FOSSILIZED_FISH" in read("data/scripts/item_ball_scripts.inc")
        and "giveitem ITEM_DRAGON_SCALE" in read("data/maps/Route114/scripts.inc")
        and "giveitem ITEM_MILOTICITE" in read("data/maps/SootopolisCity_Gym_1F/scripts.inc")
    ),
    "one-time held-item gifts survive a full Bag": all(
        one_time_gift_is_retryable(path, item)
        for path, item in (
            ("data/maps/FallarborTown_Mart/scripts.inc", "ITEM_SHINY_STONE"),
            ("data/maps/FortreeCity_House2/scripts.inc", "ITEM_ICE_STONE"),
            ("data/maps/LavaridgeTown_House/scripts.inc", "ITEM_KINGS_ROCK"),
            ("data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc", "ITEM_METAL_ALLOY"),
            ("data/maps/MauvilleCity/scripts.inc", "ITEM_DUBIOUS_DISC"),
            ("data/maps/MossdeepCity/scripts.inc", "ITEM_ICE_STONE"),
            ("data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc", "ITEM_UPGRADE"),
            ("data/maps/Route110/scripts.inc", "ITEM_RARE_CANDY"),
            ("data/maps/RustboroCity_Mart/scripts.inc", "ITEM_MOON_STONE"),
            ("data/maps/SlateportCity_PokemonFanClub/scripts.inc", "ITEM_DEEP_SEA_TOOTH"),
            ("data/maps/VerdanturfTown_Mart/scripts.inc", "ITEM_GIMMIGHOUL_COIN"),
        )
    ),
    "Mega kits are atomic retryable and migrated": (
        "TryAddVerdantItemBundle" in item_source
        and "if (!AddBagItem(itemIds[i], 1))" in item_source
        and "if (added[i])\n                    RemoveBagItem(itemIds[i], 1);" in item_source
        and "TryGiveVerdantStevenRewardBundle" in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
        and "TryGiveVerdantMegaKit" in read("data/maps/PetalburgCity_Gym/scripts.inc")
        and "FLAG_VERDANT_MIGRATED_MEGA_KIT" in save_source
        and "PlayerOwnsItemAnywhere(migration->item)" in save_source
    ),
    "persistent item ownership checks still include Bag PC party Day Care and storage boxes": (
        "CheckBagHasItem(itemId, 1) || CheckPCHasItem(itemId, 1)" in item_source
        and "gSaveBlock1Ptr->daycare.mons[i].mon" in item_source
        and "GetBoxMonDataAt(boxId, boxPosition, MON_DATA_HELD_ITEM)" in item_source
    ),
    "all replaced world pickups have old-save migrations": all(
        f"{{{pickup_flag}, {item}, {migration_flag}}}" in save_source
        for pickup_flag, item, migration_flag in verdant_world_item_migrations
    ),
    "Cycling Road best reward uses its own retry-safe flag": (
        "FLAG_VERDANT_CYCLING_REWARD_ELECTRIC_SEED" in read("data/maps/Route110/scripts.inc")
        and "goto_if_set FLAG_TM93_WILD_CHARGE" not in read("data/maps/Route110/scripts.inc")
    ),
    "Battle Frontier party counts cannot exceed selection storage": (
        party_menu_source.count("return min(gSpecialVar_0x8005, MAX_FRONTIER_PARTY_SIZE);") == 2
    ),
    "invalid movement directions cannot index collision callbacks": (
        "if (direction == DIR_NONE || direction > DIR_EAST)" in read("src/event_object_movement.c")
    ),
    "directly addressed buried trainers cannot overrun reveal callbacks": (
        "if (task->tFuncId < ARRAY_COUNT(sTrainerSeeFuncList2))" in read("src/trainer_see.c")
        and "else if (!FieldEffectActiveListContains(FLDEFF_ASH_PUFF))" in read("src/trainer_see.c")
        and read("src/trainer_see.c").split("sTrainerSeeFuncList2[]", 1)[1].split("};", 1)[0].count("WaitRevealBuriedTrainer") == 0
    ),
    "Frontier EV builders cannot divide by an empty spread": (
        factory_source.count("if (count != 0)") >= 2
        and factory_source.count("evs = MAX_TOTAL_EVS / count;") == 2
        and pokemon_source.count("if (statCount != 0)") >= 2
        and pokemon_source.count("evAmount = MAX_TOTAL_EVS / statCount;") == 2
    ),
    "Factory IV lookup clamps to its actual table": (
        "if (challengeNum >= ARRAY_COUNT(sFixedIVTable))" in factory_source
        and "a1 = ARRAY_COUNT(sFixedIVTable) - 1;" in factory_source
        and "if (arg0 > 8)" not in factory_source
    ),
    "Factory opponents use the Factory streak and level mode": (
        "factoryWinStreaks[battleMode][lvlMode] / 7" in read("src/battle_tower.c")
        and "towerWinStreaks[battleMode][0] / 7" not in read("src/battle_tower.c")
    ),
    "Frontier bans cover alternate forms of banned species": (
        "baseSpecies = GET_BASE_SPECIES_ID(species);" in frontier_util_source
        and "gFrontierBannedSpecies[i] != baseSpecies" in frontier_util_source
    ),
    "Arena Mind scoring bounds-checks modern moves": (
        "static s8 GetArenaMindRating(u16 move)" in arena_source
        and "if (move >= MOVES_COUNT)" in arena_source
        and "gBattleMoves[move].effect" in arena_source
        and "mindPoints[battler] += GetArenaMindRating(gCurrentMove);" in arena_source
        and "sMindRatings[gCurrentMove]" not in arena_source
    ),
    "Dome Levitate scoring returns the intended immunity value": (
        "if (defAbility == ABILITY_LEVITATE && moveType == TYPE_GROUND)" in dome_source
        and "typePower = TYPE_x0;" in dome_source.split(
            "if (defAbility == ABILITY_LEVITATE && moveType == TYPE_GROUND)", 1
        )[1].split("else", 1)[0]
        and "if (arg2 == 1)\n            return 8;" not in dome_source
        and "case TYPE_x0:\n            typePower = 8;" in dome_source
        and "case TYPE_x0:\n            typePower = -16;" in dome_source
        and "textPrinter.fontId = GetStringWidth(2, textPrinter.currentChar, 0) > 60 ? 7 : 2;" in dome_source
    ),
    "Pike status rooms honor modern immunity abilities": (
        "ability == ABILITY_COMATOSE || ability == ABILITY_PURIFYING_SALT" in read("src/battle_pike.c")
        and "ability == ABILITY_IMMUNITY || ability == ABILITY_PASTEL_VEIL" in read("src/battle_pike.c")
    ),
    "Pyramid Bag capacity uses its own stack limit": (
        "quantities[i] + count <= MAX_PYRAMID_BAG_CAPACITY" in item_source
        and "(quantities[i] + count) - MAX_PYRAMID_BAG_CAPACITY" in item_source
    ),
    "Frontier item icons release field-effect resources": (
        "FieldEffectFreeGraphicsResources(&gSprites[sScrollableMultichoice_ItemSpriteId]);"
        in field_specials_source
    ),
    "tutor lookups validate both menu and row": (
        "static bool8 TryGetTutorMoveByMenu(u16 menu, u16 moveIndex, u16 *move)" in field_specials_source
        and "if (moveIndex >= ARRAY_COUNT(list))" in field_specials_source
        and field_specials_source.count("TryGetTutorMoveByMenu(") >= 3
        and "gStringVar1[0] = EOS;" in field_specials_source
    ),
    "Dome entry yields one frame before scripted movement": (
        "goto_if_eq BattleFrontier_BattleDomePreBattleRoom_EventScript_ReturnFromBattle\n\tdelay 1"
        in read("data/maps/BattleFrontier_BattleDomePreBattleRoom/scripts.inc")
    ),
    "credits use the selected starter generation and bounded scans": (
        "GetStarterPokemonForGeneration(VarGet(VAR_STARTER_MON), VarGet(VAR_STARTER_GEN))"
        in read("src/credits.c")
        and "if (sCreditsData->numMonToShow == 0)" in read("src/credits.c")
        and "dexNum < NUM_MON_SLIDES && sCreditsData->monToShow[dexNum] != starter"
        in read("src/credits.c")
    ),
    "VBlank waiting avoids raw inline SWI state hazards": (
        "if (gWirelessCommType != 0)" in read("src/main.c")
        and "while (!(gMain.intrCheck & INTR_FLAG_VBLANK))" in read("src/main.c")
        and "VBlankIntrWait();" in read("src/main.c")
        and 'asm("swi 0x5")' not in read("src/main.c")
    ),
    "terrain replacement clears only terrain status bits": (
        "gFieldStatuses &= ~STATUS_FIELD_TERRAIN_ANY;" in battle_util_source
        and "STATUS_FIELD_GRASSY_TERRAIN | EFFECT_ELECTRIC_TERRAIN"
        not in battle_util_source
    ),
    "Sticky Barb residual damage ignores fainted holders": (
        "case HOLD_EFFECT_STICKY_BARB:   // Not an orb per se" in battle_util_source
        and "if (IsBattlerAlive(battlerId)"
        in battle_util_source.split(
            "case HOLD_EFFECT_STICKY_BARB:   // Not an orb per se", 1
        )[1].split("break;", 1)[0]
    ),
    "ability stat messages preserve the actual stage count": (
        "BattleScript_TargetAbilityStatRaiseOnMoveEnd::" in read("data/battle_scripts_1.s")
        and "printstring STRINGID_DEFENDERSSTATROSE"
        in read("data/battle_scripts_1.s").split(
            "BattleScript_TargetAbilityStatRaiseOnMoveEnd::", 1
        )[1].split("BattleScript_ScriptingAbilityStatRaise::", 1)[0]
        and "printstring STRINGID_DEFENDERSSTATROSE"
        in read("data/battle_scripts_1.s").split(
            "BattleScript_WeakArmorSpeedAnim:", 1
        )[1].split("BattleScript_WeakArmorActivatesEnd:", 1)[0]
    ),
    "wild loadout items restore before a captured Pokémon is transferred": (
        "u16 originalEnemyItems[PARTY_SIZE]" in read("include/battle.h")
        and "gBattleStruct->originalEnemyItems[i] = GetMonData(&gEnemyParty[i]"
        in read("src/battle_main.c")
        and "if (currentItem != originalItem)" in battle_script_command_source.split(
            "static void Cmd_givecaughtmon(void)", 2
        )[2].split("static void Cmd_trysetcaughtmondexflags", 1)[0]
        and "SetMonData(&gEnemyParty[partyIndex], MON_DATA_HELD_ITEM, &originalItem)"
        in battle_script_command_source
    ),
    "status immunity checks the actual Minior battler": (
        "|| IsShieldsDownProtected(battler)"
        in battle_script_command_source.split(
            "u32 IsAbilityStatusProtected", 1
        )[1].split("static void RecalcBattlerStats", 1)[0]
        and "IsShieldsDownProtected(battler\n"
        not in battle_script_command_source
    ),
    "Zoom Lens applies only when its holder moves later": (
        "GetBattlerTurnOrderNum(battlerAtk) > GetBattlerTurnOrderNum(battlerDef))\n    {"
        in battle_script_command_source
        and "GetBattlerTurnOrderNum(battlerAtk) > GetBattlerTurnOrderNum(battlerDef))\n    {"
        in read("src/battle_ai_util.c")
        and "GetBattlerTurnOrderNum(battlerAtk) > GetBattlerTurnOrderNum(battlerDef));"
        not in battle_script_command_source
        and "GetBattlerTurnOrderNum(battlerAtk) > GetBattlerTurnOrderNum(battlerDef));"
        not in read("src/battle_ai_util.c")
    ),
    "always-hit scripts still respect Protect and Detect": (
        "JumpIfMoveAffectedByProtect(gCurrentMove)"
        in battle_script_command_source.split(
            "if (move == NO_ACC_CALC_CHECK_LOCK_ON)", 1
        )[1].split("else if (gSpecialStatuses", 1)[0]
        and "JumpIfMoveAffectedByProtect(0)" not in battle_script_command_source
    ),
    "absorbing abilities resolve before type immunity": (
        "if (AbilityBattleEffects(ABILITYEFFECT_ABSORBING, gBattlerTarget, 0, 0, gCurrentMove))"
        in battle_script_command_source.split(
            "static void Cmd_typecalc(void)", 2
        )[2].split("static void Cmd_adjustdamage", 1)[0]
    ),
    "transformed battlers cannot persist copied HP forms": (
        "if (gBattleMons[battler].status2 & STATUS2_TRANSFORMED)"
        in battle_util_source.split(
            "static bool32 ShouldChangeFormHpBased", 1
        )[1].split("static u8 ForewarnChooseMove", 1)[0]
    ),
    "Sleep Talk records its own slot for Last Resort": (
        "gDisableStructs[gBattlerAttacker].usedMoves |= gBitTable[gCurrMovePos];"
        in battle_script_command_source.split(
            "static void Cmd_trychoosesleeptalkmove(void)", 2
        )[2].split("static void Cmd_setdestinybond", 1)[0]
    ),
    "absorbed Parental Bond moves cannot schedule a second strike": (
        "orhalfword gMoveResultFlags, MOVE_RESULT_DOESNT_AFFECT_FOE"
        in read("data/battle_scripts_1.s").split(
            "BattleScript_MoveStatDrain::", 1
        )[1].split("BattleScript_MonMadeMoveUseless_PPLoss::", 1)[0]
    ),
    "Mirror Move records immune executed attacks": (
        "&& !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)"
        not in battle_script_command_source.split(
            "case MOVEEND_MIRROR_MOVE:", 1
        )[1].split("case MOVEEND_NEXT_TARGET:", 1)[0]
    ),
    "Red Card replacements cannot inherit move-user effects": (
        "u8 redCardSwitched;" in read("include/battle.h")
        and "gBattleStruct->redCardSwitched |= gBitTable[gBattlerTarget];"
        in battle_script_command_source
        and "!(gBattleStruct->redCardSwitched & gBitTable[gBattlerAttacker])"
        in battle_script_command_source.split(
            "case MOVEEND_LIFEORB_SHELLBELL:", 1
        )[1].split("case MOVEEND_PICKPOCKET:", 1)[0]
        and "gBattleStruct->redCardSwitched &= ~gBitTable[gBattlerAttacker];"
        in battle_script_command_source
    ),
    "Ally Switch respects both sides of Tower Link Multi ownership": (
        "GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT"
        in battle_script_command_source.split(
            "case VARIOUS_JUMP_IF_NO_ALLY:", 1
        )[1].split("case VARIOUS_ALLY_SWITCH_SWAP:", 1)[0]
        and "BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_TOWER_LINK_MULTI"
        in battle_script_command_source.split(
            "case VARIOUS_JUMP_IF_NO_ALLY:", 1
        )[1].split("case VARIOUS_ALLY_SWITCH_SWAP:", 1)[0]
    ),
    "wild Wimp Out does not require a reserve party member": (
        "if (GetBattlerSide(battler) == B_SIDE_OPPONENT)\n        return TRUE;"
        in battle_util_source.split(
            "bool32 CanBattlerActivateEmergencyExit", 1
        )[1].split("bool32 DidBattlerCrossEmergencyExitThreshold", 1)[0]
        and "return CountUsablePartyMons(battler) > 0;"
        in battle_util_source.split(
            "bool32 CanBattlerActivateEmergencyExit", 1
        )[1].split("bool32 DidBattlerCrossEmergencyExitThreshold", 1)[0]
    ),
    "Sticky Web tracks setter identity and side independently": (
        "u8 stickyWebBattlerId;" in read("include/battle.h")
        and "u8 stickyWebBattlerSide;" in read("include/battle.h")
        and "gSideTimers[targetSide].stickyWebBattlerId = gBattlerAttacker"
        in battle_script_command_source
        and "gSideTimers[targetSide].stickyWebBattlerSide = GetBattlerSide(gBattlerAttacker)"
        in battle_script_command_source
        and "gBattleScripting.stickyWebStatDrop = TRUE;" in battle_script_command_source
        and "stickyWebBattlerSide != GetBattlerSide(gBattlerTarget)" in battle_util_source
        and "gBattleStruct->stickyWebUser" not in battle_script_command_source
        and "gBattleStruct->stickyWebUser" not in read("src/battle_main.c")
    ),
    "passive Solar Power and held-item KOs update battle outcome": (
        "atk24 BattleScript_SolarPowerActivatesEnd"
        in read("data/battle_scripts_1.s").split(
            "BattleScript_SolarPowerActivates::", 1
        )[1].split("BattleScript_HealerActivates::", 1)[0]
        and "atk24 BattleScript_ItemHurtEnd2End"
        in read("data/battle_scripts_1.s").split(
            "BattleScript_ItemHurtEnd2::", 1
        )[1].split("BattleScript_ItemHealHP_Ret::", 1)[0]
    ),
    "whiteout text reports only money actually removed": (
        "money = min(money, GetMoney(&gSaveBlock1Ptr->money));"
        in battle_script_command_source.split(
            "static void Cmd_getmoneyreward(void)", 2
        )[2].split("static void Cmd_unknown_5E", 1)[0]
    ),
    "Pancham's Pokédex method includes its level threshold": (
        'gText_EVO_LEVEL_DARK_TYPE_MON_IN_PARTY[]   = _("Lv. {STR_VAR_2} with Dark-type ally")'
        in read("src/strings.c")
        and read("src/pokedex.c").split(
            "case EVO_LEVEL_DARK_TYPE_MON_IN_PARTY:", 1
        )[1].split("case EVO_TRADE_SPECIFIC_MON:", 1)[0].count("ConvertIntToDecimalStringN") == 1
        and "EVO_LEVEL_DARK_TYPE_MON_IN_PARTY, 32, SPECIES_PANGORO"
        in read("src/data/pokemon/evolution.h")
    ),
    "rain evolution Pokédex text loads its own level": (
        read("src/pokedex.c").split(
            "case EVO_LEVEL_RAIN:", 1
        )[1].split("case EVO_SPECIFIC_MON_IN_PARTY:", 1)[0].count("ConvertIntToDecimalStringN") == 1
    ),
    "Petalburg's poison tutorial matches one-HP survival": (
        "Pokémon, it will lose HP down to 1."
        in read("data/maps/PetalburgCity_Mart/scripts.inc")
        and "until it faints" not in read("data/maps/PetalburgCity_Mart/scripts.inc")
    ),
    "Pickup recovers consumed battle items during end-turn processing": (
        "static bool32 TryPickupUsedItem(u8 battler)" in battle_util_source
        and "case ABILITY_PICKUP:" in battle_util_source.split(
            "case ABILITYEFFECT_ENDTURN:", 1
        )[1].split("case ABILITY_HARVEST:", 1)[0]
        and "BattleScriptPushCursorAndCallback(BattleScript_PickupActivates)"
        in battle_util_source
        and "BattleScript_PickupActivates::" in read("data/battle_scripts_1.s")
    ),
    "entry hazards inspect the entering battler's held item": (
        "u32 holdEffect = GetBattlerHoldEffect(battlerId, TRUE);"
        in battle_util_source.split(
            "bool32 IsBattlerAffectedByHazards", 1
        )[1].split("bool32 TestSheerForceFlag", 1)[0]
        and "GetBattlerHoldEffect(gActiveBattler, TRUE)"
        not in battle_util_source.split(
            "bool32 IsBattlerAffectedByHazards", 1
        )[1].split("bool32 TestSheerForceFlag", 1)[0]
    ),
    "Itemfinder naming matches its native dialogue": (
        '.name = _("Itemfinder")' in read("src/data/items.h").split(
            "[ITEM_ITEMFINDER]", 1
        )[1].split("[ITEM_OLD_ROD]", 1)[0]
        and 'Dowsing MCHN' not in read("src/data/items.h")
    ),
    "Rotom appliances remain reachable after every Johto starter choice": (
        sum(
            event.get("script") == "Rotom_Appliances_Main"
            and event.get("y") == 4
            and event.get("x") in (8, 9, 10)
            for event in json.loads(
                read("data/maps/LittlerootTown_ProfessorBirchsLab/map.json")
            )["bg_events"]
        ) == 3
    ),
    "literal Bag description lines fit the native window": (
        max(item_description_literal_widths) <= 104
    ),
    "guaranteed support moves display no accuracy check": (
        ".accuracy = 0" in read("src/data/battle_moves.h").split(
            "[MOVE_HELPING_HAND]", 1
        )[1].split("[MOVE_TRICK]", 1)[0]
        and ".accuracy = 0" in read("src/data/battle_moves.h").split(
            "[MOVE_WATER_SPORT]", 1
        )[1].split("[MOVE_CALM_MIND]", 1)[0]
    ),
    "all 38 imported move animations are real and no move is a blank TODO": (
        len(ported_gen8_move_animations) == 38
        and len(set(ported_gen8_move_animations)) == 38
        and all(
            body
            and len(
                [
                    line
                    for line in body.splitlines()
                    if line.strip()
                    and not line.lstrip().startswith("@")
                    and not line.rstrip().endswith(":")
                ]
            ) >= 2
            and re.search(r"end\s+@\s*to\s*do", body, re.IGNORECASE) is None
            for body in ported_animation_bodies
        )
        and re.search(
            r"^\s*end\s+@\s*to\s*do\s*:",
            battle_anim_script_source,
            re.MULTILINE | re.IGNORECASE,
        ) is None
    ),
    "Astral Barrage explicitly loads assets and addresses every live target": (
        all(
            f"loadspritegfx {tag}" in astral_barrage_body
            for tag in (
                "ANIM_TAG_PURPLE_FLAME",
                "ANIM_TAG_SHADOW_BALL",
                "ANIM_TAG_HANDS_AND_FEET",
                "ANIM_TAG_THIN_RING",
                "ANIM_TAG_ICE_CHUNK",
                "ANIM_TAG_EXPLOSION",
                "ANIM_TAG_GHOSTLY_SPIRIT",
                "ANIM_TAG_WISP_FIRE",
            )
        )
        and "createspriteontargets gCurseGhostSpriteTemplate, ANIM_TARGET, 3, 2"
        in astral_barrage_body
        and ".macro createvisualtaskontargets" in read("asm/macros/battle_anim_script.inc")
        and ".byte 0x30" in read("asm/macros/battle_anim_script.inc")
        and ".macro createspriteontargets" in read("asm/macros/battle_anim_script.inc")
        and ".byte 0x31" in read("asm/macros/battle_anim_script.inc")
        and "ScriptCmd_createvisualtaskontargets" in battle_anim_source
        and "ScriptCmd_createspriteontargets" in battle_anim_source
        and "GetBattleAnimMoveTargets" in battle_anim_source
    ),
    "Fiery Wrath uses a target-aware bounded affine emitter": (
        fiery_wrath_body.count("call FieryWrathGeyser") == 4
        and fiery_wrath_geyser_body.count("createsprite gSpriteTemplate_FieryWrathGeyser") == 16
        and fiery_wrath_geyser_body.count("delay 0") == 16
        and "u8 target = GetAnimBattlerId(gBattleAnimArgs[0]);" in battle_anim_new_source
        and "if (!IsBattlerSpriteVisible(target))\n        target = gBattleAnimTarget;"
        in battle_anim_new_source
        and "sprite->y -= 8;" in battle_anim_new_source
        and "if (sprite->y < -4)\n        DestroyAnimSprite(sprite);" in battle_anim_new_source
    ),
    "affine allocation failure cannot leak or strand animation accounting": (
        "bool8 InitSpriteAffineAnim(struct Sprite *sprite);" in read("gflib/sprite.h")
        and "&& !InitSpriteAffineAnim(sprite)" in read("gflib/sprite.c")
        and "DestroySprite(sprite);\n        return MAX_SPRITES;" in read("gflib/sprite.c")
        and "sprite->oam.affineMode = ST_OAM_AFFINE_OFF;" in read("gflib/sprite.c")
        and "return FALSE;" in read("gflib/sprite.c").split("bool8 InitSpriteAffineAnim", 1)[1].split("void SetOamMatrixRotationScaling", 1)[0]
        and len(
            re.findall(
                r"if\s*\(CreateSpriteAndAnimate\(.*?\)\s*!= MAX_SPRITES\)\s*"
                r"gAnimVisualTaskCount\+\+;",
                battle_anim_source,
                re.DOTALL,
            )
        ) >= 2
    ),
    "Knock Off resolves after target contact-item effects": (
        "gBattleStruct->moveEffect2 = gBattleScripting.moveEffect;" in battle_script_command_source
        and moveend_item_order.index("ItemBattleEffects(ITEMEFFECT_TARGET")
        < moveend_item_order.index("TryKnockOffBattleScript(gBattlerTarget)")
        and "MOVEEND_ITEM_EFFECTS_TARGET               12" in read("include/constants/battle_script_commands.h")
        and "MOVEEND_MOVE_EFFECTS2                     13" in read("include/constants/battle_script_commands.h")
    ),
    "spread and redirected moves keep their active target synchronized": (
        "gBattleStruct->moveTarget[gBattlerAttacker] = gBattlerTarget = battlerId;"
        in battle_script_command_source
        and "gBattleStruct->moveTarget[gBattlerAttacker] = gBattlerTarget = gSideTimers[side].followmeTarget;"
        in battle_util_source
        and "gBattleMons[i].hp != 0" in battle_anim_source.split("static u8 GetBattleAnimMoveTargets", 1)[1].split("static void ScriptCmd_createsprite", 1)[0]
        and "!(gAbsentBattlerFlags & gBitTable[i])" in battle_anim_source.split("static u8 GetBattleAnimMoveTargets", 1)[1].split("static void ScriptCmd_createsprite", 1)[0]
    ),
    "Destiny Bond command carries and follows its failure pointer": (
        ".macro trysetdestinybond failInstr:req" in read("asm/macros/battle_script.inc")
        and ".4byte \\failInstr" in read("asm/macros/battle_script.inc")
        and "trysetdestinybond BattleScript_ButItFailed" in read("data/battle_scripts_1.s")
        and "gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);"
        in battle_script_command_source.split("static void Cmd_setdestinybond", 2)[2].split("static void TrySetDestinyBondToHappen", 1)[0]
        and "gBattlescriptCurrInstr += 5;" in battle_script_command_source.split("static void Cmd_setdestinybond", 2)[2].split("static void TrySetDestinyBondToHappen", 1)[0]
    ),
    "Cursed Body uses its canonical thirty-percent chance": (
        "case ABILITY_CURSED_BODY:" in battle_util_source
        and "(Random() % 100) < 30" in battle_util_source.split("case ABILITY_CURSED_BODY:", 1)[1].split("case ABILITY_MUMMY:", 1)[0]
        and "(Random() % 3) == 0" not in battle_util_source.split("case ABILITY_CURSED_BODY:", 1)[1].split("case ABILITY_MUMMY:", 1)[0]
    ),
    "move-end visibility checks target bounds before target arrays": (
        moveend_target_visible.index("gBattlerTarget < gBattlersCount")
        < moveend_target_visible.index("gSpecialStatuses[gBattlerTarget]")
        and moveend_target_visible.index("gBattlerTarget < gBattlersCount")
        < moveend_target_visible.index("gStatuses3[gBattlerTarget]")
    ),
    "Wish Bagon remains a legal event-only teacher move": (
        "if (eggSpecies == SPECIES_BAGON)\n        AddMoveIfLegalAndNew(MOVE_WISH"
        in pokemon_source
        and "MOVE_WISH" not in read("src/data/pokemon/egg_moves.h").split("egg_moves(BAGON", 1)[1].split(")", 1)[0]
    ),
    "legal move data includes the repaired edge cases": (
        "if (eggSpecies == SPECIES_BAGON)\n        AddMoveIfLegalAndNew(MOVE_WISH" in pokemon_source
        and "[SPECIES_SLOWBRO]" in read("src/data/pokemon/tmhm_learnsets.h")
        and "TMHM2(TM76_STEALTH_ROCK)" in read("src/data/pokemon/tmhm_learnsets.h").split("[SPECIES_SLOWBRO]", 1)[1].split("[SPECIES_MAGNEMITE]", 1)[0]
        and "TMHM2(TM78_BULLDOZE)" in read("src/data/pokemon/tmhm_learnsets.h").split("[SPECIES_WORMADAM_SANDY_CLOAK]", 1)[1].split("[SPECIES_WORMADAM_TRASH_CLOAK]", 1)[0]
    ),
    "AI scores imminent Mega Evolutions as their transformed forms": (
        "TrySimulateMegaEvolutionForAI(&savedBattleMon)" in read("src/battle_controller_opponent.c")
        and "SetMonData(&simulatedMon, MON_DATA_SPECIES, &megaSpecies)" in read("src/battle_controller_opponent.c")
        and "CalculateMonStats(&simulatedMon)" in read("src/battle_controller_opponent.c")
        and "gBattleMons[gActiveBattler].ability = GetMonAbility(&simulatedMon)" in read("src/battle_controller_opponent.c")
        and "gBattleMons[gActiveBattler] = savedBattleMon" in read("src/battle_controller_opponent.c")
    ),
    "AI predicts move types with initialized dual-type matchups": (
        "bestTypeDmg = GetTypeMatchup" in read("src/battle_ai_switch_items.c")
        and "typeDmg1 *= GetTypeModifier" not in read("src/battle_ai_switch_items.c")
    ),
    "AI uses integrated switch-in ranking": (
        "GetBestMonForSwitch" in read("src/battle_ai_switch_items.c")
        and "GetBestMonDefensive" not in read("src/battle_ai_switch_items.c")
        and "GetBestMonOffensive" not in read("src/battle_ai_switch_items.c")
    ),
    "AI rejects lethal switch-in hazards": (
        "AI_CalcPartyMonHazardDamage" in read("src/battle_ai_switch_items.c")
        and "if (switchBattler >= PARTY_SIZE)" in read("src/battle_ai_util.c")
    ),
    "AI move scoring predicates repaired": (
        "!gBattleMons[battlerDef].status2 &" not in read("src/battle_ai_main.c")
        and "gLastMoves[battlerDef] != 0xFFFF" in read("src/battle_ai_main.c")
        and "AI_DATA->atkAbility == ABILITY_COMATOSE" in read("src/battle_ai_main.c")
    ),
    "AI forced tactical switches are deterministic": (
        "Random() % 3 < 2" not in read("src/battle_ai_switch_items.c")
        and "absorbingTypeAbilities[j] == monAbility && Random()" not in read("src/battle_ai_switch_items.c")
    ),
    "daycare nature parent guard": "if (parent < 0)" in read("src/daycare.c"),
    "daycare ability counter initialized": "u8 femaleCount = 0;" in read("src/daycare.c"),
    "egg moves no longer require shared egg groups": "DoMonsShareEggGroup" not in read("src/daycare.c"),
    "cap EV gains recalculate stats": read("src/battle_script_commands.c").count("CalculateMonStats(&gPlayerParty[gBattleStruct->expGetterMonId])") >= 2,
    "EV service charges actual gain": "gSpecialVar_0x8009 = actualIncrement * 100" in read("src/field_specials.c"),
    "rare candy preserves an intermediate-form tutor window": (
        "GetRareCandyTargetLevel" in read("src/party_menu.c")
        and "One Rare Candy stops after one evolution" in read("src/party_menu.c")
    ),
    "Battle Style selector removed": "MENUITEM_BATTLE_STYLE" not in read("src/option_menu.c"),
    "normal Birch intro has no hack-author interstitial": (
        "gText_Pie_" not in read("src/main_menu.c")
        and "Buffel Saft" not in read("data/text/birch_speech.inc")
        and "gen eight will be added soon" not in read("data/text/birch_speech.inc")
        and "gSaveBlock2Ptr->gameDifficulty = DIFFICULTY_CHALLENGE;" in read("src/main_menu.c")
        and "gSaveBlock2Ptr->levelCaps = LEVEL_CAPS_STRICT;" in read("src/main_menu.c")
    ),
    "Pokécenter teacher offers every legal move without badge gates": (
        "goto PKMN_Center_MoveReminder_EventScriptChooseMon" in read("data/scripts/pokemon_center_move_tutor.inc")
        and "special GiveAllTMs" not in read("data/scripts/pokemon_center_move_tutor.inc").split("PKMN_Center_Move_Tutor_MoveTutorIntro::", 1)[1].split("PKMN_Center_Move_Tutor_NoBadges::", 1)[0]
        and "AddAllLegalMovesForSpecies" in read("src/pokemon.c")
        and "GetEggMovesSpecies" in read("src/pokemon.c")
        and "NUM_TECHNICAL_MACHINES + NUM_HIDDEN_MACHINES" in read("src/pokemon.c")
        and "TUTOR_MOVE_COUNT" in read("src/pokemon.c")
        and all(
            token in read("src/pokemon.c").split("GetMoveRelearnerMoves", 1)[1].split("GetLevelUpMovesBySpecies", 1)[0]
            for token in (
                "SPECIES_ROTOM_HEAT", "MOVE_OVERHEAT",
                "SPECIES_ROTOM_WASH", "MOVE_HYDRO_PUMP",
                "SPECIES_ROTOM_FROST", "MOVE_FREEZE_DRY",
                "SPECIES_ROTOM_FAN", "MOVE_HURRICANE",
                "SPECIES_ROTOM_MOW", "MOVE_LEAF_STORM",
                "SPECIES_PIKACHU_ROCK_STAR", "MOVE_METEOR_MASH",
                "SPECIES_PIKACHU_BELLE", "MOVE_ICICLE_CRASH",
                "SPECIES_PIKACHU_POP_STAR", "MOVE_DRAINING_KISS",
                "SPECIES_PIKACHU_PH_D", "MOVE_ELECTRIC_TERRAIN",
                "SPECIES_PIKACHU_LIBRE", "MOVE_FLYING_PRESS",
                "SPECIES_NECROZMA_DUSK_MANE", "MOVE_SUNSTEEL_STRIKE",
                "SPECIES_NECROZMA_DAWN_WINGS", "MOVE_MOONGEIST_BEAM",
                "SPECIES_ZACIAN_CROWNED_SWORD", "MOVE_BEHEMOTH_BLADE",
                "SPECIES_ZAMAZENTA_CROWNED_SHIELD", "MOVE_BEHEMOTH_BASH",
            )
        )
        and "moveLevel <= level" not in read("src/pokemon.c").split("GetMoveRelearnerMoves", 1)[1].split("GetLevelUpMovesBySpecies", 1)[0]
    ),
    "Pokécenter battle-set builder is native and transactional": (
        'gText_BuildBattleSet[] = _("Learn a moveset")' in read("src/strings.c")
        and "task->tNumItems = 7;" in field_specials_source.split("case SCROLL_MULTI_POKE_CENTER_TUTOR:", 1)[1].split("break;", 1)[0]
        and re.search(r"gText_BuildBattleSet,\s*gText_Exit", pc_menu_block)
        and "special ChoosePartyMon" in pc_tutor_source.split("PKMN_Center_BattleSet_ChooseMon::", 1)[1].split("PKMN_Center_BattleSet_CantBuildForEgg::", 1)[0]
        and "compare VAR_0x8004, 255" in pc_tutor_source.split("PKMN_Center_BattleSet_ChooseMon::", 1)[1].split("PKMN_Center_BattleSet_CantBuildForEgg::", 1)[0]
        and "special IsSelectedMonEgg" in pc_tutor_source.split("PKMN_Center_BattleSet_ChooseMon::", 1)[1].split("PKMN_Center_BattleSet_CantBuildForEgg::", 1)[0]
        and "This replaces all four moves" in pc_tutor_source
        and "special ApplySelectedMonBattleSet" in pc_tutor_source
        and "ApplyVerdantBattleSetChoice(mon, gSpecialVar_0x8005)" in field_specials_source
        and "MON_DATA_SPECIES2" in battle_set_runtime
        and "MON_DATA_PP_BONUSES" in battle_set_runtime
        and "for (i = 0; i < MAX_MON_MOVES; i++)\n        SetMonMoveSlot" in battle_set_runtime
        and "MON_DATA_NATURE" in battle_set_runtime
        and "MON_DATA_ABILITY_NUM" in battle_set_runtime
        and "MON_DATA_HELD_ITEM" in battle_set_runtime
        and "BATTLE_SET_APPLY_SPECIAL_ITEM" in battle_set_runtime
        and "_EV" not in battle_set_runtime
        and battle_set_runtime.index("preset->nature >= NUM_NATURES") < battle_set_runtime.index("SetMonData(mon, MON_DATA_PP_BONUSES")
        and battle_set_runtime.count("CalculateMonStats(mon);") == 1
    ),
    "authored battle sets preserve form-exclusive mechanics": (
        all(
            species in battle_set_generator_source and move in battle_set_generator_source
            for species, move in (
                ("SPECIES_PIKACHU_ROCK_STAR", "MOVE_METEOR_MASH"),
                ("SPECIES_PIKACHU_BELLE", "MOVE_ICICLE_CRASH"),
                ("SPECIES_PIKACHU_POP_STAR", "MOVE_DRAINING_KISS"),
                ("SPECIES_PIKACHU_PH_D", "MOVE_ELECTRIC_TERRAIN"),
                ("SPECIES_PIKACHU_LIBRE", "MOVE_FLYING_PRESS"),
                ("SPECIES_NECROZMA_DUSK_MANE", "MOVE_SUNSTEEL_STRIKE"),
                ("SPECIES_NECROZMA_DAWN_WINGS", "MOVE_MOONGEIST_BEAM"),
                ("SPECIES_ZACIAN_CROWNED_SWORD", "MOVE_BEHEMOTH_BLADE"),
                ("SPECIES_ZAMAZENTA_CROWNED_SHIELD", "MOVE_BEHEMOTH_BASH"),
            )
        )
        and all(move in form_reviews_100 for move in (
            "MOVE_METEOR_MASH", "MOVE_ICICLE_CRASH", "MOVE_DRAINING_KISS",
            "MOVE_ELECTRIC_TERRAIN", "MOVE_FLYING_PRESS",
        ))
        and all(token in form_reviews_110 for token in (
            "ITEM_DOUSE_DRIVE", "ITEM_SHOCK_DRIVE", "ITEM_BURN_DRIVE", "ITEM_CHILL_DRIVE",
            "MOVE_TECHNO_BLAST", "MOVE_WATER_SHURIKEN",
        ))
        and '"alias_of": "SPECIES_GENESECT"' not in form_reviews_110
        and all(move in form_reviews_120 for move in (
            "MOVE_SUNSTEEL_STRIKE", "MOVE_MOONGEIST_BEAM",
            "MOVE_BEHEMOTH_BLADE", "MOVE_BEHEMOTH_BASH",
        ))
        and "[SPECIES_ZYGARDE_10_POWER_CONSTRUCT]" in base_stats_source
        and "ABILITY_POWER_CONSTRUCT" in base_stats_source.split(
            "[SPECIES_ZYGARDE_10_POWER_CONSTRUCT]", 1
        )[1].split("},", 1)[0]
        and '"species": "SPECIES_ZYGARDE_10_POWER_CONSTRUCT"' in form_reviews_120
        and '"ability": "ABILITY_POWER_CONSTRUCT"' in form_reviews_120.split(
            '"species": "SPECIES_ZYGARDE_10_POWER_CONSTRUCT"', 1
        )[1].split("},", 1)[0]
    ),
    "world TM pickups replaced": "finditem ITEM_TM" not in read("data/scripts/item_ball_scripts.inc"),
    "gifted TMs replaced": "giveitem ITEM_TM" not in "\n".join(p.read_text() for p in (ROOT / "data").rglob("*.inc")),
    "dead ability items removed from marts": "ITEM_ABILITY_CAPSULE" not in read("data/scripts/general_mart.inc") and "ITEM_ABILITY_PATCH" not in read("data/scripts/general_mart.inc"),
    "competitive held items are free and unlimited at Center vendors": (
        "gVerdantFreeBattleItems" in read("src/item.c")
        and "CreateFreePokemartMenu(sUnlockedBattleItemMart)" in field_specials_source
        and "sMartInfo.freeItems" in read("src/shop.c")
        and 'static const u8 sText_Free[] = _("FREE")' in read("src/shop.c")
        and "ITEM_WELLSPRING_MASK" not in read("src/data/pokemon/verdant_multi_battle_sets.h").split("gVerdantFreeBattleItems[]", 1)[1]
    ),
    "Rare Candy remains in medicine Marts while Center loadout items stay separate": (
        "BuildPokemartItemsWithCoreStock" not in read("src/shop.c")
        and general_mart_stock.count("ITEM_RARE_CANDY") == 9
        and all_map_json.count('"script": "PokemonCenter_BattleItemMart_Script"') == 16
        and all_map_json.count('"script": "General_Pokemart_Script"') == 0
        and '[ITEM_RARE_CANDY]' in read("src/data/items.h")
        and '.price = 1000' in read("src/data/items.h").split('[ITEM_RARE_CANDY]', 1)[1].split('},', 1)[0]
    ),
    "Poké Mart item names cannot overrun the Cancel row": (
        "u8 (*sItemNames)[ITEM_NAME_LENGTH]" in read("src/shop.c")
        and "u8 (*sItemNames)[16]" not in read("src/shop.c")
    ),
    "core battle items no longer duplicate campaign gifts or pickups": all(
        f"giveitem {item}" not in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("scripts.inc"))
        and f'"item": "{item}"' not in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("map.json"))
        and f"finditem {item}" not in read("data/scripts/item_ball_scripts.inc")
        for item in starter_battle_items
    ),
    "ordinary held items restore": "RestorePlayerHeldItemsAfterBattle" in read("src/battle_main.c"),
    "Champions Mega species registered": all(
        f"SPECIES_{name}" in read("include/constants/species.h")
        for name in champions_megas
    ),
    "Champions Mega Stones registered": all(
        f"ITEM_{item.upper()}" in read("include/constants/items.h")
        for _, item in champions_megas.values()
    ),
    "Champions Mega evolutions registered": all(
        f"SPECIES_{name}" in read("src/data/pokemon/evolution.h")
        for name in champions_megas
    ),
    "Champions Mega graphics complete": all(
        all(
            (ROOT / "graphics" / "pokemon" / f"mega_{asset}" / filename).is_file()
            for filename in ("front.png", "back.png", "icon.png", "normal.pal", "shiny.pal")
        )
        for asset, _ in champions_megas.values()
    ),
    "Champions Mega Stone graphics complete": all(
        (ROOT / "graphics" / "items" / "icons" / f"{item}.png").is_file()
        and (ROOT / "graphics" / "items" / "icon_palettes" / f"{item}.pal").is_file()
        for _, item in champions_megas.values()
    ),
    "Champions Mega abilities implemented": all(
        ability in read("src/battle_util.c") + read("src/battle_main.c")
        for ability in ("ABILITY_PIERCING_DRILL", "ABILITY_DRAGONIZE", "ABILITY_MEGA_SOL")
    ),
    "Mega Sol drives Weather Ball visuals": (
        "GetBattlerAbility(gBattleAnimAttacker) == ABILITY_MEGA_SOL"
        in read("src/battle_anim_effects_3.c")
    ),
    "Mega Bracelet introduced by Steven with Norman fallback": (
        "special TryGiveVerdantStevenRewardBundle" in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
        and "setflag FLAG_SYS_RECEIVED_KEYSTONE" in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
        and all(item in item_source for item in (
            "ITEM_MEGA_BRACELET", "ITEM_SCEPTILITE", "ITEM_BLAZIKENITE", "ITEM_SWAMPERTITE"
        ))
        and "goto_if_set FLAG_SYS_RECEIVED_KEYSTONE" in read("data/maps/PetalburgCity_Gym/scripts.inc")
        and "special TryGiveVerdantMegaKit" in read("data/maps/PetalburgCity_Gym/scripts.inc")
    ),
    "all new Mega Stones reward exploration": (
        "GrantChampionsMegaStones" not in read("src/field_specials.c")
        and "GrantChampionsMegaStones" not in read("data/specials.inc")
        and all(
            f"{script}::" in read("data/scripts/item_ball_scripts.inc")
            and stone in read("data/scripts/item_ball_scripts.inc").split(f"{script}::", 1)[1].split("end", 1)[0]
            for script, stone in world_mega_stones.items()
        )
    ),
    "all new Megas appear during campaign progression": all(
        species in array_body("src/data/trainer_parties.h", party)
        and stone in array_body("src/data/trainer_parties.h", party)
        for party, (species, stone) in trainer_mega_showcases.items()
    ) and all(
        sum(
            f"ITEM_{item.upper()}" in array_body("src/data/trainer_parties.h", party)
            for _, item in champions_megas.values()
        ) == 1
        for party in trainer_mega_showcases
    ),
    "all new Mega families remain obtainable": all(
        species in read("src/data/wild_encounters.json")
        or species in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("scripts.inc"))
        for species in mega_family_bases
    ),
    "wild encounter table lengths match configured slots": (
        wild_table_lengths_match
        and wild_slot_counts == {
            "land_mons": 12,
            "water_mons": 4,
            "rock_smash_mons": 4,
            "fishing_mons": 10,
            "honey_mons": 6,
        }
        and "#define WATER_WILD_COUNT    4" in read("include/wild_encounter.h")
        and "#define ROCK_WILD_COUNT     4" in read("include/wild_encounter.h")
    ),
    "disabled trainer entries leave no stray active commas": (
        re.search(r"^\s*},?\s+\*/,\s*$", read("src/data/trainer_parties.h"), re.M) is None
    ),
    "Area Dex UI is fully removed and native Pokedex behavior is restored": (
        not (ROOT / "include/area_dex.h").exists()
        and not (ROOT / "src/area_dex.c").exists()
        and "src/area_dex.o" not in read("ld_script.txt") + read("sym_ewram.txt")
        and all(token not in start_menu_source + pokedex_source for token in (
            "MENU_ACTION_AREA_DEX",
            "CB2_InitAreaDex",
            "PAGE_CURRENT_AREA",
            "Task_LoadCurrentAreaPage",
        ))
        and "case 0: //BACK TO LIST" in pokedex_source
        and "AddStartMenuAction(MENU_ACTION_EXIT);" in normal_start_menu
    ),
    "route signs derive species from every physical encounter method": (
        "BufferCurrentMapRouteSignSpecies" in wild_encounter_source
        and all(token in wild_encounter_source for token in (
            "header->landMonsInfo",
            "header->waterMonsInfo",
            "header->rockSmashMonsInfo",
            "header->fishingMonsInfo",
            "header->honeyMonsInfo",
            "SPECIES_FEEBAS",
            "GetStringWidth(1, name, 0)",
            "CollectRouteSignSpecies",
            "sText_RouteSignGrass",
            "sText_RouteSignSurf",
            "sText_RouteSignRockSmash",
            "sText_RouteSignOldRod",
            "sText_RouteSignGoodRod",
            "sText_RouteSignSuperRod",
            "sText_RouteSignHoney",
            "sText_RouteSignUnderBridge",
        ))
        and "sText_RouteSignSpeciesPercent" not in wild_encounter_source
        and "entries[i].chance" not in wild_encounter_source
        and wild_encounter_source.count("dest = StringCopy(gStringVar4, sText_RouteSignSpeciesHeader);") == 1
        and "dest = StringCopy(dest, sText_RouteSignSpeciesHeader)" not in wild_encounter_source
        and "Common_EventScript_ShowRouteSpecies::" in read("data/event_scripts.s")
        and sum(
            path.read_text().count("goto Common_EventScript_ShowRouteSpecies")
            for path in (ROOT / "data/maps").glob("Route*/scripts.inc")
        ) == 32
    ),
    "wild type-attraction logic respects each encounter table length": (
        "TryGetAbilityInfluencedWildMonIndex(wildMonInfo->wildPokemon, WATER_WILD_COUNT" in wild_encounter_source
        and "TryGetRandomWildMonIndexByType(wildMon, type, numMon, monIndex)" in wild_encounter_source
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
if failed:
    raise SystemExit(f"{len(failed)} Verdant regression check(s) failed")
print(f"All {len(checks)} Verdant regression checks passed")
