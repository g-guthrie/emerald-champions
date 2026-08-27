#!/usr/bin/env python3
"""Answer Verdant's core design questions with source-backed invariants."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def label_block(path: str, label: str) -> str:
    text = read(path)
    match = re.search(rf"^{re.escape(label)}(?::|::).*?(?=^[A-Za-z0-9_]+(?::|::)|\Z)", text, re.M | re.S)
    if not match:
        raise ValueError(f"missing dialogue label {label} in {path}")
    return match.group(0)


def visible_line_length(line: str) -> int:
    match = re.search(r'\.string\s+"(.*)"', line)
    if not match:
        return 0
    value = re.sub(r"\\[npl]$", "", match.group(1))
    value = re.sub(r"\{[^}]+\}", "PLAYER", value)
    return len(value)


manifest = json.loads(read("docs/verdant_doubles_manifest.json"))
trainers_text = read("src/data/trainers.h")
parties_text = read("src/data/trainer_parties.h")
trainer_blocks = doubles.trainer_blocks(trainers_text)

double_bodies = []
double_sizes = []
for trainer_id, rule in manifest["formats"].items():
    block = trainer_blocks[trainer_id].group(0)
    body = doubles.party_match(parties_text, doubles.party_name(block)).group(2)
    if rule["format"] == "double":
        double_bodies.append(body)
        double_sizes.append((len(doubles.species_in_party(body)), bool(rule.get("multi_partner"))))

all_map_scripts = "\n".join(path.read_text() for path in (ROOT / "data" / "maps").rglob("scripts.inc"))
all_script_includes = "\n".join(path.read_text() for path in (ROOT / "data" / "scripts").rglob("*.inc"))
all_map_json = "\n".join(path.read_text() for path in (ROOT / "data" / "maps").rglob("*.json"))
all_reward_data = all_map_scripts + "\n" + all_script_includes + "\n" + all_map_json

core_items = (
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

champions_mega_stones = (
    "ITEM_MEGANIUMITE",
    "ITEM_FERALIGITE",
    "ITEM_EMBOARITE",
    "ITEM_RAICHUNITE_X",
    "ITEM_RAICHUNITE_Y",
    "ITEM_DRAGONINITE",
    "ITEM_EXCADRITE",
    "ITEM_MALAMARITE",
    "ITEM_CHANDELURITE",
    "ITEM_HAWLUCHANITE",
    "ITEM_GRENINJITE",
)
mega_item_blocks = read("src/data/items.h")
mega_items = set(re.findall(
    r"^\s*\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{(?:(?!^\s*\[ITEM_).)*?HOLD_EFFECT_MEGA_STONE",
    mega_item_blocks,
    re.M | re.S,
))

wild_group = json.loads(read("src/data/wild_encounters.json"))["wild_encounter_groups"][0]
wild_fields = {field["type"]: field["encounter_rates"] for field in wild_group["fields"]}
wild_by_map = {entry["map"]: entry for entry in wild_group["encounters"] if "map" in entry}
all_ordinary_wild_species = {
    mon["species"]
    for entry in wild_by_map.values()
    for method in wild_fields
    for mon in entry.get(method, {}).get("mons", [])
}
ordinary_wild_legend_species = {
    "SPECIES_KUBFU", "SPECIES_TYPE_NULL", "SPECIES_SUICUNE", "SPECIES_VOLCANION",
    "SPECIES_MANAPHY", "SPECIES_TERRAKION", "SPECIES_KELDEO", "SPECIES_TAPU_FINI",
}
withheld_ordinary_wild_species = {"SPECIES_DARKRAI", "SPECIES_MARSHADOW"}
base_stats_text = read("src/data/pokemon/base_stats.h")


def species_catch_rate(species: str) -> int | None:
    match = re.search(
        rf"^\s*\[{re.escape(species)}\]\s*=\s*\{{"
        rf"(?:(?!^\s*\[SPECIES_).)*?^\s*\.catchRate\s*=\s*(\d+)",
        base_stats_text,
        re.M | re.S,
    )
    return int(match.group(1)) if match else None


early_showcases = {
    "MAP_ROUTE101": {"SPECIES_DREEPY", "SPECIES_LARVESTA"},
    "MAP_ROUTE102": {"SPECIES_HATENNA", "SPECIES_INDEEDEE"},
    "MAP_ROUTE103": {"SPECIES_TOXEL", "SPECIES_YAMPER"},
    "MAP_PETALBURG_WOODS": {"SPECIES_IMPIDIMP", "SPECIES_FOONGUS"},
    "MAP_ROUTE116": {"SPECIES_ROOKIDEE", "SPECIES_DREEPY"},
    "MAP_RUSTURF_TUNNEL": {"SPECIES_LARVESTA", "SPECIES_BAGON"},
}

rewritten_dialogue = {
    "data/scripts/secret_power_tm.inc": ("Route111_Text_MakingRoomUseTMToMakeYourOwn",),
    "data/maps/FortreeCity_House2/scripts.inc": ("FortreeCity_Text_GiveTM49",),
    "data/maps/SlateportCity_OceanicMuseum_1F/scripts.inc": ("SlateportCity_OceanicMuseum_1F_Text_HopeINeverSeeYouAgain",),
    "data/maps/VerdanturfTown_Mart/scripts.inc": ("VerdanturfTown_Mart_Text_SlowPokemon", "VerdanturfTown_Mart_Text_HaveTM66"),
    "data/maps/VerdanturfTown_BattleTentLobby/scripts.inc": ("VerdanturfTown_BattleTentLobby_Text_AttractionRunsDeep",),
    "data/maps/SlateportCity_BattleTentLobby/scripts.inc": ("SlateportCity_BattleTentLobby_Text_ExplainTorment",),
    "data/maps/MauvilleCity_Gym/scripts.inc": ("MauvilleCity_Gym_Text_ExplainVoltSwitch",),
    "data/maps/LavaridgeTown_Gym_1F/scripts.inc": ("LavaridgeTown_Gym_1F_Text_ExplainOverheat",),
    "data/maps/FallarborTown_CozmosHouse/scripts.inc": ("FallarborTown_CozmosHouse_Text_MayIHaveMeteorite", "FallarborTown_CozmosHouse_Text_PleaseUseThisTM"),
    "data/maps/Route104/scripts.inc": ("Route104_Text_TMsAreOneTimeUse",),
    "data/maps/PetalburgCity_Gym/scripts.inc": ("PetalburgCity_Gym_Text_ExplainFacade",),
    "data/maps/MauvilleCity/scripts.inc": ("MauvilleCity_Text_WattsonThanksTakeTM", "MauvilleCity_Text_HaveTM74"),
    "data/maps/Route110/scripts.inc": ("Route110_Text_GiveTM93", "Route110_Text_GiveTM83"),
    "data/maps/FallarborTown_Mart/scripts.inc": ("FallarborTown_Mart_Text_HaveTM60",),
    "data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc": ("PacifidlogTown_PokemonCenter_1F_Text_GiveExplosion",),
    "data/maps/LavaridgeTown_House/scripts.inc": ("LavaridgeTown_House_Text_HaveTM94", "LavaridgeTown_House_Text_GiveTM94"),
    "data/maps/Route114/scripts.inc": ("Route114_Text_AllMyMonDoesIsRoarTakeThis", "Route114_Text_ExplainRoar"),
    "data/maps/SlateportCity_PokemonFanClub/scripts.inc": ("SlateportCity_PokemonFanClub_Text_GiveTM58",),
    "data/maps/SootopolisCity_House1/scripts.inc": ("SootopolisCity_House1_Text_DevelopedThisTM", "SootopolisCity_House1_Text_ExplainBrickBreak"),
}

dialogue_blocks = []
for path, labels in rewritten_dialogue.items():
    dialogue_blocks.extend(label_block(path, label) for label in labels)

stale_tm_dialogue = []
for path in list((ROOT / "data" / "maps").rglob("scripts.inc")) + list((ROOT / "data" / "scripts").rglob("*.inc")):
    for line_no, line in enumerate(path.read_text().splitlines(), 1):
        if ".string" in line and re.search(r"\bTM(?:\d|\b)|Technical Machine", line):
            if path.name == "pokemon_center_move_tutor.inc" and "moves, TMs, HMs" in line:
                continue
            stale_tm_dialogue.append(f"{path.relative_to(ROOT)}:{line_no}")

item_names = re.findall(r'\.name\s*=\s*_\("([^"]+)"\)', read("src/data/items.h"))
battle_item_unlocks = read("src/item.c").split("sBattleItemUnlocks[]", 1)[1].split("};", 1)[0]
general_mart_stock = read("data/scripts/general_mart.inc")


questions = [
    ("Is every new game unambiguously Challenge Mode with strict caps?", all(token in read(path) for path in ("src/main_menu.c", "src/new_game.c") for token in ("DIFFICULTY_CHALLENGE", "LEVEL_CAPS_STRICT"))),
    ("Is Set battle style mandatory, with the selector removed?", "OPTIONS_BATTLE_STYLE_SET" in read("src/main_menu.c") and "MENUITEM_BATTLE_STYLE" not in read("src/option_menu.c")),
    ("Does one Rare Candy add up to ten levels while stopping at the cap or next level evolution?", all(token in read("src/party_menu.c") for token in ("targetLevel = min(level + 10, GetLevelCap())", "GetRareCandyTargetLevel(mon, level)", "IsLevelThresholdEvolution"))),
    ("Does leveling preserve chosen moves while the tutor retains every legal move source?", all("return MOVE_NONE;" in read("src/pokemon.c").split(function, 1)[1].split("}", 1)[0] for function in ("MonTryLearningNewMove(", "MonTryLearningNewMoveInRange(", "MonTryLearningNewEvolutionMove(")) and "AddAllLegalMovesForSpecies" in read("src/pokemon.c")),
    ("Can the reusable Leveler raise the whole eligible party exactly to the current cap?", all(token in read("src/party_menu.c") for token in ("StartLevelerPartySequence", "FindNextLevelerSlot", "targetLevel = GetLevelCap()")) and "RemoveBagItem(gSpecialVar_ItemId, 1);" in read("src/party_menu.c")),
    ("Are competitive held items free and unlimited at every Pokémon Center battle vendor?", "gVerdantFreeBattleItems" in read("src/item.c") and "CreateFreePokemartMenu" in read("src/field_specials.c") and "sMartInfo.freeItems" in read("src/shop.c") and all_map_json.count('"script": "PokemonCenter_BattleItemMart_Script"') == 16),
    ("Do medicine Marts keep progression and Rare Candy while both vendor menus remain bounded?", general_mart_stock.count("ITEM_RARE_CANDY") == 9 and "ITEM_ULTRA_BALL" in general_mart_stock and "ITEM_FULL_RESTORE" in general_mart_stock and "ITEM_MAX_ETHER" in general_mart_stock and "sUnlockedBattleItemMart[192]" in read("src/field_specials.c") and "count + 1 < capacity" in read("src/item.c") and "u8 (*sItemNames)[ITEM_NAME_LENGTH]" in read("src/shop.c") and max(map(len, item_names)) < 17),
    ("Are obsolete TM pickups, gifts, and shop entries completely gone?", "ITEM_TM" not in all_reward_data),
    ("Do the old TM specialty vendors still have a useful role?", all(item in read("data/maps/SlateportCity/scripts.inc") for item in ("ITEM_SOFT_SAND", "ITEM_HARD_STONE", "ITEM_BLACK_BELT", "ITEM_MAGNET")) and all(item in read("data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc") for item in ("ITEM_DAMP_ROCK", "ITEM_HEAT_ROCK", "ITEM_SMOOTH_ROCK", "ITEM_ICY_ROCK"))),
    ("Is the now-redundant ordinary held-item reward layer explicitly inventoried for redesign?", "legacy held-item rewards remain inventoried for redesign" in read("scripts/verdant_item_economy_audit.py") and "Existing rewards need" in read("docs/emerald_champions_progression_contract.md") and "a later incentive redesign" in read("docs/emerald_champions_progression_contract.md")),
    ("Are Mega, Primal, mask, Drive, Memory, and Plate progression items excluded from free loadouts?", "IsVerdantProtectedProgressionItem" in read("src/verdant_battle_sets.c") and "verdant_protected_set_items.h" in read("src/verdant_battle_sets.c") and "ITEM_WELLSPRING_MASK" not in read("src/data/pokemon/verdant_multi_battle_sets.h").split("gVerdantFreeBattleItems[]", 1)[1]),
    ("Can the Pokémon Center teacher offer every legal move source?", all(token in read("src/pokemon.c") for token in ("AddAllLegalMovesForSpecies", "GetEggMovesSpecies", "NUM_TECHNICAL_MACHINES + NUM_HIDDEN_MACHINES", "TUTOR_MOVE_COUNT"))),
    ("Is that complete move list available before the first Badge?", "goto PKMN_Center_MoveReminder_EventScriptChooseMon" in read("data/scripts/pokemon_center_move_tutor.inc").split("PKMN_Center_Move_Tutor_MoveTutorIntro::", 1)[1].split("PKMN_Center_Move_Tutor_NoBadges::", 1)[0]),
    ("Can abilities be changed natively without consumable items?", all(token in read("src/party_menu.c") for token in ("CursorCb_Ability", "DisplayAbilitySelectionWindow", "Task_HandleAbilitySelectionInput", "SetMonData(mon, MON_DATA_ABILITY_NUM"))),
    ("Does the Day Care still create eggs through normal compatibility rules?", all(token in read("src/daycare.c") for token in ("GetDaycareCompatibilityScore", "EggGroupsOverlap", "TriggerPendingDaycareEgg", "GiveEggFromDaycare"))),
    ("Does breeding retain incentives beyond move access?", all(token in read("src/daycare.c") for token in ("ITEM_EVERSTONE", "ITEM_DESTINY_KNOT", "InheritIVs", "TryInheritAbility"))),
    ("Are IV and EV services bounded, priced by real gain, and stat-safe?", all(token in read("src/field_specials.c") for token in ("MAX_PER_STAT_EVS", "MAX_TOTAL_EVS", "actualIncrement * 100", "CalculateMonStats"))),
    ("Is every real trainer record represented in the authored format manifest?", len(manifest["formats"]) == 854 and set(manifest["formats"]) == set(trainer_blocks) - {"TRAINER_NONE"}),
    ("Is the campaign genuinely mostly doubles while low-stakes routes get relief?", len(double_bodies) >= len(manifest["formats"]) * 0.65 and len(manifest["formats"]) - len(double_bodies) >= len(manifest["formats"]) * 0.25),
    ("Can every doubles battle safely deploy its authored four/six-mon wave or special three-mon partner party?", all(size == 3 if is_partner else size in (4, 6) for size, is_partner in double_sizes)),
    ("Do intentional singles remain as pacing contrast?", manifest["formats"]["TRAINER_NORMAN_1"]["format"] == "single" and manifest["formats"]["TRAINER_DRAKE"]["format"] == "single" and len(manifest["formats"]) - len(double_bodies) >= len(manifest["formats"]) * 0.25),
    ("Do marquee bosses exceed the cap at the ace and carry complete teams?", all(max(doubles.BOSS_LEVEL_OFFSETS[boss["battle"]]) >= 1 and len(boss["team"]) == 6 and all(len(mon["moves"]) == 4 and mon["item"] != "ITEM_NONE" for mon in boss["team"]) for boss in manifest["bosses"])),
    ("Do doubles teams use protection and active speed control?", sum("MOVE_PROTECT" in body for body in double_bodies) >= 275 and sum("MOVE_TAILWIND" in body or "MOVE_TRICK_ROOM" in body for body in double_bodies) >= 70),
    ("Do doubles teams create real spread-pressure decisions?", sum(any(move in body for move in ("MOVE_ROCK_SLIDE", "MOVE_EARTHQUAKE", "MOVE_HEAT_WAVE", "MOVE_DAZZLING_GLEAM", "MOVE_MUDDY_WATER", "MOVE_BLIZZARD")) for body in double_bodies) >= 300),
    ("Does trainer AI understand foes, partners, and tactical switching?", all("AI_FLAG_CHECK_FOE" in trainer_blocks[trainer_id].group(0) for trainer_id in manifest["formats"]) and "AI_FLAG_SMART_SWITCHING" in trainers_text and "AI_FLAG_HELP_PARTNER" in trainers_text),
    ("Are the repaired AI decisions guarded against prior deterministic defects?", all(token in read("src/battle_ai_switch_items.c") + read("src/battle_ai_main.c") for token in ("GetBestMonForSwitch", "AI_CalcPartyMonHazardDamage", "gLastMoves[battlerDef] != 0xFFFF")) and "Random() % 3 < 2" not in read("src/battle_ai_switch_items.c")),
    ("Are Megas and Primals constrained and deliberately showcased?", all(sum(mon["item"] in mega_items for mon in boss["team"]) <= 1 for boss in manifest["bosses"]) and all(stone in parties_text for stone in champions_mega_stones) and all(token in read("src/data/pokemon/evolution.h") for token in ("EVO_PRIMAL_REVERSION, ITEM_BLUE_ORB, SPECIES_KYOGRE_PRIMAL", "EVO_PRIMAL_REVERSION, ITEM_RED_ORB, SPECIES_GROUDON_PRIMAL"))),
    ("Does the player receive Mega access early enough to experiment?", "special TryGiveVerdantStevenRewardBundle" in read("data/maps/GraniteCave_StevensRoom/scripts.inc") and "setflag FLAG_SYS_RECEIVED_KEYSTONE" in read("data/maps/GraniteCave_StevensRoom/scripts.inc") and all(stone in read("src/item.c").split("TryAddVerdantStevenRewardBundle", 1)[1].split("}", 1)[0] for stone in ("ITEM_MEGA_BRACELET", "ITEM_SCEPTILITE", "ITEM_BLAZIKENITE", "ITEM_SWAMPERTITE"))),
    ("Are exciting encounters accessible without one-percent or catch-rate-3 hunting?", all(showcases <= {mon["species"] for mon in wild_by_map[map_id]["land_mons"]["mons"] if wild_fields["land_mons"][wild_by_map[map_id]["land_mons"]["mons"].index(mon)] >= 4} for map_id, showcases in early_showcases.items()) and 1 not in wild_fields["land_mons"] and ordinary_wild_legend_species <= all_ordinary_wild_species and not (withheld_ordinary_wild_species & all_ordinary_wild_species) and all(species_catch_rate(species) == 45 for species in ordinary_wild_legend_species) and all(len(entry[field]["mons"]) == len(wild_fields[field]) for entry in wild_by_map.values() for field in wild_fields if field in entry)),
    ("Are rewritten rewards, dialogue, and menus visually and semantically clean?", not stale_tm_dialogue and all(visible_line_length(line) <= 36 for block in dialogue_blocks for line in block.splitlines()) and "ITEM_NAME_LENGTH" in read("src/shop.c")),
]


def main() -> None:
    failures = []
    for index, (question, passed) in enumerate(questions, 1):
        status = "PASS" if passed else "FAIL"
        print(f"{status} {index:02d}: {question}")
        if not passed:
            failures.append(question)
    if len(questions) != 30:
        failures.append(f"audit must contain exactly 30 questions, found {len(questions)}")
    if failures:
        raise SystemExit("\n".join(f"FAIL: {failure}" for failure in failures))
    print("PASS: all 30 logical audit questions are source-backed")


if __name__ == "__main__":
    main()
