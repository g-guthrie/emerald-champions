#!/usr/bin/env python3
"""Build, validate, and retry native battles from real campaign checkpoints."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parent))
import run_emerald_champions_campaign as campaign
import render_emerald_champions_ui as ui
from item_catalog import free_vendor_items


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = ROOT / "tests/campaign/playthrough.json"
DEFAULT_RECIPES = ROOT / "tests/campaign/battle_checkpoint_recipes.json"
DEFAULT_OUT = ROOT / "work/battle-checkpoint-lab"
BUTTONS = ("A", "B", "START", "SELECT", "UP", "DOWN", "LEFT", "RIGHT", "L", "R")
READ_RE = re.compile(r"^READ width=4 address=([0-9a-f]+) value=([0-9a-f]+)$", re.M)
SUBSTRUCT_ORDERS = (
    "GAEM", "GAME", "GEAM", "GEMA", "GMAE", "GMEA",
    "AGEM", "AGME", "AEGM", "AEMG", "AMGE", "AMEG",
    "EGAM", "EGMA", "EAGM", "EAMG", "EMGA", "EMAG",
    "MGAE", "MGEA", "MAGE", "MAEG", "MEGA", "MEAG",
)
POCKET_NAMES = ("items", "medicine", "battle", "tm_hm", "berries", "poke_balls", "key_items", "mega_stones")
SERVICE_ITEMS = {
    "leveler": "ITEM_LEVELER",
    "move_tutor": "ITEM_MOVE_RELEARNER",
    "ability_editor": "ITEM_ABILITY_CAPSULE",
    "flight_beacon": "ITEM_FLIGHT_BEACON",
    "portable_healing": "ITEM_POKE_VIAL",
}


def source_digest(paths: list[Path]) -> dict[str, str]:
    return {path.relative_to(ROOT).as_posix(): sha256(path) for path in paths}


def reachable_maps(manifest: dict[str, Any], segment_id: str) -> list[str]:
    by_id = {row["id"]: row for row in manifest["segments"]}
    maps: set[str] = set()
    for ancestor in campaign.segment_ancestry(segment_id, by_id):
        maps.update(by_id[ancestor].get("coverage", {}).get("maps", []))
    return sorted(maps)


def world_reachable_maps(manifest: dict[str, Any], segment_id: str) -> list[str]:
    """Add optional interiors without crossing the first-pass story frontier."""
    frontier = set(reachable_maps(manifest, segment_id))
    by_id: dict[str, dict[str, Any]] = {}
    for path in (ROOT / "data/maps").glob("*/map.json"):
        data = load_json(path)
        by_id[str(data.get("id"))] = data
    result = set(frontier)
    queue = list(frontier)
    while queue:
        current = queue.pop()
        data = by_id.get(current, {})
        targets = [row.get("map") for row in (data.get("connections") or []) if row.get("map") in frontier]
        targets += [row.get("dest_map") for row in (data.get("warp_events") or [])]
        for target in targets:
            if not isinstance(target, str) or target not in by_id or target in result:
                continue
            if target in frontier or any(target.startswith(root + "_") for root in frontier):
                result.add(target)
                queue.append(target)
    return sorted(result)


def wild_arsenal(map_names: list[str], method_access: dict[str, dict[str, Any]] | None = None) -> tuple[list[dict[str, Any]], Path]:
    path = ROOT / "src/data/wild_encounters.json"
    data = load_json(path)
    method_access = method_access or {"land_mons": {"available": True, "evidence": "ordinary walking"}}
    fishing_groups = next(field["groups"] for field in data["wild_encounter_groups"][0]["fields"]
                          if field["type"] == "fishing_mons")
    rows: dict[str, dict[str, Any]] = {}
    for group in data.get("wild_encounter_groups", []):
        for encounter in group.get("encounters", []):
            map_name = encounter.get("map")
            if map_name not in map_names:
                continue
            for method, table in encounter.items():
                if not method.endswith("_mons") or not isinstance(table, dict):
                    continue
                for index, mon in enumerate(table.get("mons", [])):
                    required_method = method
                    if method == "fishing_mons":
                        required_method = next((rod for rod, indexes in fishing_groups.items() if index in indexes), "unknown_fishing")
                    access = method_access.get(required_method, {"available": False, "evidence": "method not unlocked"})
                    if not access["available"]:
                        continue
                    species = mon.get("species")
                    if isinstance(species, str):
                        row = rows.setdefault(species, {"species": species, "sources": []})
                        row["sources"].append({"kind": "wild", "map": map_name, "method": method,
                                               "required_method": required_method, "unlock_evidence": access["evidence"],
                                               "min_level": mon.get("min_level"), "max_level": mon.get("max_level")})
    return [rows[key] for key in sorted(rows)], path


def center_services(map_names: list[str]) -> tuple[list[dict[str, str]], list[Path]]:
    rows: dict[str, dict[str, str]] = {}
    sources: list[Path] = []
    for map_json in sorted((ROOT / "data/maps").glob("*PokemonCenter_1F/map.json")):
        data = load_json(map_json)
        center_id = str(data.get("id", ""))
        if not any(center_id.startswith(map_id + "_") for map_id in map_names):
            continue
        sources.append(map_json)
        rows["heal"] = {"service": "heal", "source": map_json.relative_to(ROOT).as_posix()}
        rows["level_to_cap"] = {"service": "level_to_cap", "source": "data/scripts/pkmn_center_nurse.inc"}
        for obj in data.get("object_events", []):
            script = str(obj.get("script", ""))
            if "BattleVendor" in script:
                for service in ("preset", "nature", "ability", "held_item", "stat_points"):
                    rows[service] = {"service": service, "source": map_json.relative_to(ROOT).as_posix(), "script": script}
            if "MoveTutor" in script:
                rows["moves"] = {"service": "moves", "source": map_json.relative_to(ROOT).as_posix(), "script": script}
    return [rows[key] for key in sorted(rows)], sources


def opponent_dossier(trainer_ids: list[str]) -> tuple[list[dict[str, Any]], Path]:
    # This is deliberately read from the executable trainer source, never from
    # a benchmark-authored solution or recommendation.
    path = ROOT / "src/data/trainers.party"
    text = path.read_text()
    dossiers = []
    for trainer in trainer_ids:
        match = re.search(rf"^=== {re.escape(trainer)} ===\n(?P<body>.*?)(?=^=== |\Z)", text, re.M | re.S)
        if not match:
            raise LabError(f"trainer dossier source is missing {trainer}")
        body = match.group("body")
        header, *blocks = re.split(r"\n(?=SPECIES_[A-Z0-9_]+ @ ITEM_)", body.strip())
        mons = []
        for block in blocks:
            first, *lines = block.splitlines()
            species, item = re.match(r"(SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+)", first).groups()
            fields = {key: value for key, value in (line.split(": ", 1) for line in lines if ": " in line)}
            mons.append({"species": species, "item": item, "level": int(fields["Level"]),
                         "ability": fields["Ability"], "nature": fields["Nature"],
                         "stat_points": fields.get("EVs"), "moves": [line[2:] for line in lines if line.startswith("- ")]})
        headers = {key: value for key, value in (line.split(": ", 1) for line in header.splitlines() if ": " in line)}
        dossiers.append({"trainer_id": trainer, "name": headers.get("Name"), "class": headers.get("Class"),
                         "format": "doubles" if headers.get("Double Battle") == "Yes" else "singles",
                         "ai": headers.get("AI", "").split(" / "), "party": mons,
                         "field_conditions": []})
    return dossiers, path


def explicit_enum_values(path: Path, prefix: str) -> dict[int, str]:
    return {int(value): name for name, value in re.findall(rf"\b({prefix}[A-Z0-9_]+)\s*=\s*(\d+)", path.read_text())}


def source_array(text: str, name: str) -> list[str]:
    match = re.search(rf"static const u16 {name}\[\]\s*=\s*\{{(?P<body>.*?)\n\}};", text, re.S)
    if not match:
        raise LabError(f"source array is missing: {name}")
    return list(dict.fromkeys(re.findall(r"ITEM_[A-Z0-9_]+", match.group("body"))))


def materialize_legal_arsenal(map_names: list[str], cap: int, party: list[dict[str, Any]],
                              center_rows: list[dict[str, str]], inventory: list[dict[str, Any]],
                              method_access: dict[str, dict[str, Any]] | None = None) -> tuple[dict[str, Any], list[Path]]:
    wild_rows, wild_path = wild_arsenal(map_names, method_access)
    species_by_id = explicit_enum_values(ROOT / "include/constants/species.h", "SPECIES_")
    direct: dict[str, dict[str, Any]] = {row["species"]: row for row in wild_rows}
    for mon in party:
        species = species_by_id.get(mon["species_id"], f"SPECIES_ID_{mon['species_id']}")
        direct.setdefault(species, {"species": species, "sources": []})["sources"].append(
            {"kind": "owned_party", "slot": mon["slot"]})

    map_sources: list[Path] = []
    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        data = load_json(map_json)
        if data.get("id") not in map_names:
            continue
        script = map_json.with_name("scripts.inc")
        if not script.is_file():
            continue
        map_sources += [map_json, script]
        for species in re.findall(r"\b(?:givemon|giveegg|setwildbattle)\s+(SPECIES_[A-Z0-9_]+)", script.read_text()):
            direct.setdefault(species, {"species": species, "sources": []})["sources"].append(
                {"kind": "gift_or_static", "map": data["id"], "source": script.relative_to(ROOT).as_posix()})

    field_specials = ROOT / "src/field_specials.c"
    held_items = sorted(free_vendor_items(ROOT)) \
        if any(row["service"] == "held_item" for row in center_rows) else []
    item_by_id = explicit_enum_values(ROOT / "include/constants/items.h", "ITEM_")
    owned_items = {item_by_id.get(int(row["item_id"]), f"ITEM_ID_{row['item_id']}") for row in inventory}
    mega_access = "ITEM_MEGA_RING" in owned_items
    mega_items = sorted(set(re.findall(r"ITEM_[A-Z0-9_]+", (ROOT / "src/data/emerald_champions_mega_stones.h").read_text()))) if mega_access else []
    evolution_items = sorted(set(re.findall(r"ITEM_[A-Z0-9_]+", (ROOT / "src/data/emerald_champions_evolution_items.h").read_text()))) if mega_access else []
    available_evolution_items = owned_items | set(evolution_items)

    family_paths = sorted((ROOT / "src/data/pokemon/species_info").glob("*families.h"))
    evolution_edges: list[dict[str, Any]] = []
    species_blocks: dict[str, str] = {}
    for path in family_paths:
        text = path.read_text()
        starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, match in enumerate(starts):
            species = match.group(1)
            block = text[match.start():starts[index + 1].start() if index + 1 < len(starts) else len(text)]
            species_blocks[species] = block
            evo = re.search(r"\.evolutions\s*=\s*EVOLUTION\((.*?)\),\s*\n", block, re.S)
            if not evo:
                continue
            for method, parameter, target in re.findall(r"\{(EVO_[A-Z0-9_]+),\s*([A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", evo.group(1)):
                allowed = False
                reason = "method unavailable"
                if method in {"EVO_LEVEL", "EVO_LEVEL_BATTLE_ONLY"} and parameter.isdigit():
                    allowed, reason = int(parameter) <= cap, f"level {parameter} <= cap {cap}"
                elif method == "EVO_ITEM":
                    allowed, reason = parameter in available_evolution_items, f"requires {parameter}"
                evolution_edges.append({"from": species, "to": target, "method": method, "parameter": parameter,
                                        "available": allowed, "reason": reason, "source": path.relative_to(ROOT).as_posix()})

    available = set(direct)
    changed = True
    while changed:
        changed = False
        for edge in evolution_edges:
            if edge["available"] and edge["from"] in available and edge["to"] not in available:
                available.add(edge["to"])
                direct[edge["to"]] = {"species": edge["to"], "sources": [{"kind": "evolution", **edge}]}
                changed = True

    sets_path = ROOT / "data/emerald_champions/emerald_champions_battle_sets.json"
    sets = load_json(sets_path)
    all_presets = sets["defaults"] + sets["alternatives"] + sets["singles_defaults"] + sets["singles_alternatives"]
    presets: dict[str, list[dict[str, Any]]] = {}
    accessible_items = set(held_items) | set(mega_items) | owned_items | {"ITEM_NONE"}
    for preset in all_presets:
        if preset["species"] in available and preset["item"] in accessible_items and preset["required_item"] in accessible_items:
            presets.setdefault(preset["species"], []).append(preset)

    learnsets_path = ROOT / "data/emerald_champions/showdown_champions_learnsets.json"
    learnsets = load_json(learnsets_path)
    nature_path = ROOT / "include/constants/pokemon.h"
    natures = sorted(set(re.findall(r"\bNATURE_[A-Z0-9_]+", nature_path.read_text())))
    result_species = []
    for species in sorted(available):
        block = species_blocks.get(species, "")
        ability_match = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", block)
        abilities = [value for value in dict.fromkeys(re.findall(r"ABILITY_[A-Z0-9_]+", ability_match.group(1)))
                     if value != "ABILITY_NONE"] if ability_match else []
        key = species.removeprefix("SPECIES_").replace("_", "").lower()
        legal_moves = learnsets["learnsets"].get(key, [])
        result_species.append({**direct[species], "abilities": abilities, "legal_moves": legal_moves,
                               "natures": natures, "stat_points": {"total": 66, "per_stat_max": 32},
                               "presets": presets.get(species, []),
                               "legality_provenance": [learnsets_path.relative_to(ROOT).as_posix(), sets_path.relative_to(ROOT).as_posix()]})
    sources = [wild_path, field_specials, sets_path, learnsets_path, nature_path,
               ROOT / "include/constants/species.h", ROOT / "include/constants/items.h",
               ROOT / "src/data/emerald_champions_mega_stones.h", ROOT / "src/data/emerald_champions_evolution_items.h",
               *family_paths, *map_sources]
    return {"reachable_maps": map_names, "encounter_method_access": method_access or {},
            "chronology": "first_pass_campaign_order_no_future_resources",
            "pokemon": result_species, "held_items": held_items,
            "mega_access": mega_access, "mega_stones": mega_items, "evolution_items": evolution_items,
            "evolution_edges": evolution_edges, "natures": natures,
            "stat_point_rule": {"total": 66, "per_stat_max": 32}}, sources


class LabError(RuntimeError):
    pass


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def load_json(path: Path) -> dict[str, Any]:
    value = json.loads(path.read_text())
    if not isinstance(value, dict):
        raise LabError(f"expected JSON object: {path}")
    return value


def nm_symbols(elf: Path) -> dict[str, int]:
    candidates = (shutil.which("arm-none-eabi-nm"), ROOT / "tools/binutils/bin/arm-none-eabi-nm", Path("/opt/homebrew/bin/arm-none-eabi-nm"))
    nm = next((str(path) for path in candidates if path and Path(path).is_file()), None)
    if nm is None:
        raise LabError("arm-none-eabi-nm is required")
    result = subprocess.run([nm, "-S", str(elf)], text=True, capture_output=True)
    if result.returncode:
        raise LabError(result.stderr.strip())
    found: dict[str, int] = {}
    for line in result.stdout.splitlines():
        fields = line.split()
        if len(fields) >= 4:
            try:
                found[fields[-1]] = int(fields[0], 16)
            except ValueError:
                pass
    return found


def runner_reads(runner: Path, rom: Path, state: Path, addresses: list[int]) -> dict[int, int]:
    command = [str(runner), "--rom", str(rom), "--frames", "1", "--state-in", str(state)]
    for address in addresses:
        command += ["--read", f"4:0x{address:x}"]
    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode:
        raise LabError(f"memory read failed:\n{result.stdout}{result.stderr}")
    return {int(a, 16): int(v, 16) for a, v in READ_RE.findall(result.stdout)}


def words_to_bytes(values: dict[int, int], address: int, length: int) -> bytes:
    return b"".join(values[pos].to_bytes(4, "little") for pos in range(address, address + length, 4))[:length]


def read_block(runner: Path, rom: Path, state: Path, address: int, length: int) -> bytes:
    addresses = list(range(address, address + length, 4))
    values = runner_reads(runner, rom, state, addresses)
    return words_to_bytes(values, address, length)


def parse_party(raw: bytes) -> list[dict[str, Any]]:
    party = []
    for slot in range(6):
        mon = raw[slot * 100:(slot + 1) * 100]
        personality = int.from_bytes(mon[0:4], "little")
        ot_id = int.from_bytes(mon[4:8], "little")
        has_species = bool(mon[19] & 2)
        if not has_species:
            continue
        key = personality ^ ot_id
        secure = b"".join((int.from_bytes(mon[pos:pos + 4], "little") ^ key).to_bytes(4, "little") for pos in range(32, 80, 4))
        order = SUBSTRUCT_ORDERS[personality % 24]
        growth = secure[order.index("G") * 12:][:12]
        attacks = secure[order.index("A") * 12:][:12]
        species_word = int.from_bytes(growth[0:2], "little")
        move_words = [int.from_bytes(attacks[pos:pos + 2], "little") for pos in range(0, 8, 2)]
        party.append({
            "slot": slot,
            "species_id": species_word & 0x7FF,
            "level": mon[84],
            "hp": int.from_bytes(mon[86:88], "little"),
            "max_hp": int.from_bytes(mon[88:90], "little"),
            "held_item_id": (int.from_bytes(growth[0:4], "little") >> 16) & 0x3FF,
            "move_ids": [word & 0x7FF for word in move_words if (word & 0x7FF)],
        })
    return party


def parse_inventory(runner: Path, rom: Path, state: Path, symbols: dict[str, int], encryption_key: int) -> list[dict[str, int | str]]:
    descriptors = read_block(runner, rom, state, symbols["gBagPockets"], 96)
    rows: list[dict[str, int | str]] = []
    for index, pocket in enumerate(POCKET_NAMES):
        descriptor = descriptors[index * 12:(index + 1) * 12]
        primary_ptr = int.from_bytes(descriptor[0:4], "little")
        overflow_ptr = int.from_bytes(descriptor[4:8], "little")
        capacity_and_id = int.from_bytes(descriptor[8:10], "little")
        capacity = capacity_and_id & 0x3FF
        primary_capacity = int.from_bytes(descriptor[10:12], "little")
        portions = [(primary_ptr, min(capacity, primary_capacity))]
        if capacity > primary_capacity and overflow_ptr:
            portions.append((overflow_ptr, capacity - primary_capacity))
        position = 0
        for pointer, count in portions:
            raw = read_block(runner, rom, state, pointer, count * 4)
            for offset in range(count):
                item_id = int.from_bytes(raw[offset * 4:offset * 4 + 2], "little")
                quantity = int.from_bytes(raw[offset * 4 + 2:offset * 4 + 4], "little") ^ (encryption_key & 0xFFFF)
                if item_id and quantity:
                    rows.append({"pocket": pocket, "slot": position, "item_id": item_id, "quantity": quantity})
                position += 1
    return rows


def numeric_constants() -> dict[str, int]:
    compiler = shutil.which("cc") or shutil.which("clang")
    if not compiler:
        raise LabError("host C preprocessor unavailable")
    command = [compiler, "-dM", "-E", "-Iinclude", "-Isrc", "-I.", "-include", "constants/items.h", "-x", "c", "/dev/null"]
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True)
    values = {}
    for name, raw in re.findall(r"^#define\s+([A-Z][A-Z0-9_]+)\s+([0-9]+)\s*$", result.stdout, re.M):
        values[name] = int(raw)
    return values


def trainer_segments(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    result = []
    for segment in manifest["segments"]:
        coverage = segment.get("coverage", {})
        legacy = [value for value in coverage.get("battles", []) if value.startswith("TRAINER_")]
        canonical = list(coverage.get("trainers", []))
        if legacy and canonical and legacy != canonical:
            raise LabError(f"{segment['id']}: coverage.battles trainers disagree with coverage.trainers")
        trainers = canonical or legacy
        if trainers:
            branches = list(coverage.get("branches", []))
            if canonical and not branches:
                # A second segment may complete post-battle dialogue for the
                # same trainer without invoking another physical battle.
                continue
            result.append({"segment": segment["id"], "parent": segment.get("parent"),
                           "trainer_ids": trainers, "branches": branches})
    return result


def validate_recipes(manifest: dict[str, Any], recipes: dict[str, Any]) -> None:
    if recipes.get("schema_version") != 1 or not isinstance(recipes.get("encounters"), list):
        raise LabError("recipe schema_version must be 1 with an encounters list")
    segments = {row["id"]: row for row in manifest["segments"]}
    seen: set[str] = set()
    for recipe in recipes["encounters"]:
        encounter_id = recipe.get("id")
        if not isinstance(encounter_id, str) or encounter_id in seen:
            raise LabError("recipe encounter IDs must be unique strings")
        seen.add(encounter_id)
        segment_id = recipe.get("campaign_segment")
        if segment_id not in segments:
            raise LabError(f"{encounter_id}: unknown campaign segment {segment_id}")
        segment = segments[segment_id]
        count = recipe.get("pre_battle_action_count")
        if not isinstance(count, int) or not 0 <= count <= len(segment.get("semantic_actions", [])):
            raise LabError(f"{encounter_id}: invalid pre_battle_action_count")
        if not recipe.get("engage_actions"):
            raise LabError(f"{encounter_id}: engage_actions are required")


def checkpoint_index(manifest: dict[str, Any], recipes: dict[str, Any], out: Path) -> dict[str, Any]:
    by_segment = {row["campaign_segment"]: row for row in recipes["encounters"]}
    rows = []
    for encounter in trainer_segments(manifest):
        recipe = by_segment.get(encounter["segment"])
        checkpoint = out / str(recipe["id"]) / "checkpoint.json" if recipe else None
        rows.append({
            **encounter,
            "recipe": recipe["id"] if recipe else None,
            "status": "validated" if checkpoint and checkpoint.is_file() else "recipe_ready" if recipe else "recipe_required",
            "checkpoint": str(checkpoint) if checkpoint and checkpoint.is_file() else None,
        })
    master_path = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
    master_text = master_path.read_text()
    payload = {"schema_version": 1, "campaign_trainer_encounter_count": len(rows),
               "campaign_trainer_invocation_count": sum(len(row["trainer_ids"]) for row in rows),
               "authored_scope": {"physical_encounters": len(re.findall(r"^=== ENCOUNTER ", master_text, re.M)),
                                  "trainer_branches": len(re.findall(r"^--- BRANCH ", master_text, re.M)),
                                  "source": master_path.relative_to(ROOT).as_posix(), "source_sha256": sha256(master_path)},
               "encounters": rows}
    out.mkdir(parents=True, exist_ok=True)
    (out / "index.json").write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    return payload


def query_flags(state: Path, runner: Path, rom: Path, addresses: dict[str, int], constants: dict[str, int], names: list[str]) -> dict[str, bool]:
    result = {}
    for name in names:
        value, _ = campaign.query_campaign_value(kind=1, identifier=constants[name], runner=runner, rom=rom, state=state, addresses=addresses)
        result[name] = bool(value)
    return result


def build_checkpoint(args: argparse.Namespace, manifest: dict[str, Any], recipes: dict[str, Any]) -> Path:
    recipe = next((row for row in recipes["encounters"] if row["id"] == args.encounter), None)
    if recipe is None:
        raise LabError(f"unknown or unprepared encounter: {args.encounter}")
    segments = {row["id"]: row for row in manifest["segments"]}
    segment = segments[recipe["campaign_segment"]]
    campaign_run = args.campaign_run.resolve()
    parent_state = campaign_run / "checkpoints" / f"{segment['parent']}.ss1"
    parent_meta = Path(str(parent_state) + ".json")
    if not parent_state.is_file() or not parent_meta.is_file():
        raise LabError(f"campaign parent checkpoint is missing: {parent_state}")
    parent = load_json(parent_meta)
    rom = Path(parent["rom"])
    artifact = parent.get("artifact_evidence", {})
    elf = Path(artifact.get("elf", {}).get("snapshot", args.elf))
    runner = ui.build_runner()
    symbols = nm_symbols(elf)
    optional_telemetry = {"gEcHeadlessCampaignLastResolution", "gEcHeadlessCampaignQueryObjectActive",
                          "gEcHeadlessCampaignQueryObjectX", "gEcHeadlessCampaignQueryObjectY"}
    required = [name for name in campaign.TELEMETRY_SYMBOLS if name not in optional_telemetry] + ["gEcHeadlessFixtureActiveScenario", "gPlayerPartyPtr", "gBagPockets", "gSaveBlock2Ptr", "gSaveBlock1Ptr"]
    missing = [name for name in required if name not in symbols]
    if missing:
        raise LabError(f"ELF lacks battle-lab symbols: {missing}")
    addresses = {name: symbols.get(name, symbols["gEcHeadlessCampaignQueryValue"])
                 for name in campaign.TELEMETRY_SYMBOLS}
    directory = args.out / recipe["id"]
    directory.mkdir(parents=True, exist_ok=True)
    state = directory / "state.ss1"
    shutil.copyfile(parent_state, state)
    initial, _ = campaign.run_state_chunk(runner=runner, rom=rom, state=state, addresses=addresses, frames=1)
    prefix = dict(segment)
    prefix["semantic_actions"] = segment.get("semantic_actions", [])[:recipe["pre_battle_action_count"]]
    screenshot_dir = directory / "construction"
    screenshot_dir.mkdir(parents=True, exist_ok=True)
    telemetry, trace = campaign.apply_semantic_actions(prefix, runner=runner, rom=rom, state=state, addresses=addresses, initial=initial, screenshot_dir=screenshot_dir)
    if not campaign.is_stable_overworld(telemetry):
        raise LabError("constructed checkpoint is not a stable pre-battle overworld state")
    shot = directory / "pre-battle.png"
    campaign.run_state_chunk(runner=runner, rom=rom, state=state, addresses=addresses, frames=1, screenshot=shot)
    constants = campaign.parse_numeric_constants()
    badge_names = [f"FLAG_BADGE{i:02d}_GET" for i in range(1, 9)]
    cap_names = badge_names + ["FLAG_IS_CHAMPION"]
    flag_values = query_flags(state, runner, rom, addresses, constants, cap_names)
    cap_steps = list(zip(cap_names, (14, 20, 30, 40, 45, 55, 60, 70, 80)))
    level_cap = next((cap for name, cap in cap_steps if not flag_values[name]), 100)
    pointers = runner_reads(runner, rom, state, [symbols["gPlayerPartyPtr"], symbols["gSaveBlock2Ptr"]])
    party_pointer = pointers[symbols["gPlayerPartyPtr"]]
    save2_pointer = pointers[symbols["gSaveBlock2Ptr"]]
    encryption_key = runner_reads(runner, rom, state, [save2_pointer + 0xAC])[save2_pointer + 0xAC]
    party = parse_party(read_block(runner, rom, state, party_pointer, 600))
    inventory = parse_inventory(runner, rom, state, symbols, encryption_key)
    item_constants = numeric_constants()
    inventory_ids = {int(row["item_id"]) for row in inventory}
    services = {name: item_constants.get(item_name) in inventory_ids for name, item_name in SERVICE_ITEMS.items()}
    money_ptr = parent.get("telemetry", {}).get("money")
    # SaveBlock1 money is encrypted and its source offset is stable in Emerald.
    save1_ptr = runner_reads(runner, rom, state, [symbols["gSaveBlock1Ptr"]])[symbols["gSaveBlock1Ptr"]]
    encrypted_money = runner_reads(runner, rom, state, [save1_ptr + 0x490])[save1_ptr + 0x490]
    money = encrypted_money ^ encryption_key
    maps = world_reachable_maps(manifest, segment["id"])
    available_center_services, center_sources = center_services(maps)
    item_by_id = explicit_enum_values(ROOT / "include/constants/items.h", "ITEM_")
    owned_item_names = {item_by_id.get(int(row["item_id"]), f"ITEM_ID_{row['item_id']}") for row in inventory}
    field_flags = query_flags(state, runner, rom, addresses, constants,
                              ["FLAG_RECEIVED_HM_SURF", "FLAG_RECEIVED_HM_ROCK_SMASH"])
    method_access = {
        "land_mons": {"available": True, "evidence": "ordinary walking encounter"},
        "hidden_mons": {"available": False, "evidence": "include/config/dexnav.h DEXNAV_ENABLED is FALSE"},
        "water_mons": {"available": flag_values["FLAG_BADGE05_GET"] and field_flags["FLAG_RECEIVED_HM_SURF"],
                       "evidence": "Stone through Balance Badge plus FLAG_RECEIVED_HM_SURF required by src/field_move.c"},
        "rock_smash_mons": {"available": flag_values["FLAG_BADGE03_GET"] and field_flags["FLAG_RECEIVED_HM_ROCK_SMASH"],
                            "evidence": "Dynamo Badge plus FLAG_RECEIVED_HM_ROCK_SMASH required by src/field_move.c"},
        "old_rod": {"available": "ITEM_OLD_ROD" in owned_item_names, "evidence": "ITEM_OLD_ROD in checkpoint bag"},
        "good_rod": {"available": "ITEM_GOOD_ROD" in owned_item_names, "evidence": "ITEM_GOOD_ROD in checkpoint bag"},
        "super_rod": {"available": "ITEM_SUPER_ROD" in owned_item_names, "evidence": "ITEM_SUPER_ROD in checkpoint bag"},
    }
    legal_arsenal, arsenal_sources = materialize_legal_arsenal(
        maps, level_cap, party, available_center_services, inventory, method_access)
    dossier, trainer_path = opponent_dossier(recipe["trainer_ids"])
    hashed_sources = list(dict.fromkeys(path for path in arsenal_sources if path.is_file())) + center_sources
    payload = {
        "schema_version": 1,
        "encounter": {"id": recipe["id"], "trainer_ids": recipe["trainer_ids"], "map_id": telemetry["gEcHeadlessCampaignMapId"], "level_cap": level_cap,
                      "opponent_dossier": dossier},
        "campaign": {"segment": segment["id"], "parent_segment": segment["parent"], "parent_state": str(parent_state), "parent_state_sha256": sha256(parent_state), "pre_battle_action_count": recipe["pre_battle_action_count"], "construction_trace": trace},
        "identity": {"rom": str(rom), "rom_sha256": sha256(rom), "elf": str(elf), "elf_sha256": sha256(elf), "runner": str(runner), "runner_sha256": sha256(runner)},
        "checkpoint": {"state": str(state), "state_sha256": sha256(state), "screenshot": str(shot), "screenshot_sha256": sha256(shot)},
        "player": {"money": money, "badges": [name for name in badge_names if flag_values[name]], "party": party, "inventory": inventory},
        "services": {"portable_items": services, "available_center_services": available_center_services},
        "legal_arsenal": legal_arsenal,
        "dependency_graph": {"inputs": source_digest(hashed_sources + [trainer_path, args.manifest.resolve(), args.recipes.resolve()]),
                             "invalidation": "any input hash change requires regeneration"},
        "preparation_adapter": {"schema_version": 1, "request_log": "preparation.jsonl",
                                "execution_boundary": "native Center UI or canonical game functions",
                                "allowed_changes": ["party", "preset", "moves", "nature", "ability", "held_item", "stat_points", "level"],
                                "result_states": ["requested", "applied", "rejected"],
                                "reject_if": ["species_not_in_legal_arsenal", "item_not_owned", "move_not_legal", "service_unavailable", "cap_exceeded"]},
        "launch": {"campaign_autowin_disabled": True, "disabled_symbol": "gEcHeadlessFixtureActiveScenario", "disabled_value": 0, "reset_source": str(state), "allowed_controls": list(BUTTONS), "observation": "screenshots", "engage_actions": recipe["engage_actions"]},
    }
    metadata = directory / "checkpoint.json"
    metadata.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    validate_checkpoint(metadata)
    checkpoint_index(manifest, recipes, args.out)
    return metadata


def validate_checkpoint(path: Path) -> dict[str, Any]:
    value = load_json(path)
    required = {"schema_version", "encounter", "campaign", "identity", "checkpoint", "player", "services",
                "legal_arsenal", "dependency_graph", "preparation_adapter", "launch"}
    if value.get("schema_version") != 1 or not required <= value.keys():
        raise LabError(f"checkpoint has invalid top-level schema: {path}")
    state = Path(value["checkpoint"]["state"])
    shot = Path(value["checkpoint"]["screenshot"])
    if sha256(state) != value["checkpoint"]["state_sha256"] or sha256(shot) != value["checkpoint"]["screenshot_sha256"]:
        raise LabError("checkpoint artifact hash mismatch")
    if not value["launch"].get("campaign_autowin_disabled"):
        raise LabError("checkpoint launch does not disable campaign auto-win")
    if not isinstance(value["player"].get("party"), list) or not isinstance(value["player"].get("inventory"), list):
        raise LabError("checkpoint player resources are incomplete")
    if not value["legal_arsenal"].get("pokemon") or not value["dependency_graph"].get("inputs"):
        raise LabError("checkpoint legal arsenal is not materialized")
    drifted = []
    for relative, expected in value["dependency_graph"]["inputs"].items():
        source = ROOT / relative
        if not source.is_file() or sha256(source) != expected:
            drifted.append(relative)
    if drifted:
        raise LabError("checkpoint dependencies changed; regenerate: " + ", ".join(drifted))
    value["validation_status"] = "validated"
    return value


def retry_checkpoint(metadata_path: Path, attempt_dir: Path, buttons: list[str]) -> Path:
    meta = validate_checkpoint(metadata_path)
    state = Path(meta["checkpoint"]["state"])
    rom = Path(meta["identity"]["rom"])
    runner = Path(meta["identity"]["runner"])
    elf = Path(meta["identity"]["elf"])
    if sha256(rom) != meta["identity"]["rom_sha256"] or sha256(elf) != meta["identity"]["elf_sha256"] or sha256(runner) != meta["identity"]["runner_sha256"]:
        raise LabError("launch identity hash mismatch")
    symbols = nm_symbols(elf)
    attempt_dir.mkdir(parents=True, exist_ok=False)
    current = attempt_dir / "current.ss1"
    shutil.copyfile(state, current)
    next_state = attempt_dir / "next.ss1"
    shot = attempt_dir / "observation-000000.png"
    command = [str(runner), "--rom", str(rom), "--frames", "1", "--state-in", str(current), "--state-out", str(next_state), "--screenshot", str(shot), "--write", f"0:4:0x{symbols['gEcHeadlessFixtureActiveScenario']:x}:0"]
    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode:
        raise LabError(result.stdout + result.stderr)
    os.replace(next_state, current)
    events = [{"index": 0, "button": None, "screenshot": str(shot), "screenshot_sha256": sha256(shot)}]
    for index, button in enumerate(buttons, 1):
        button = button.upper()
        if button not in BUTTONS:
            raise LabError(f"unsupported control: {button}")
        shot = attempt_dir / f"observation-{index:06d}.png"
        next_state = attempt_dir / "next.ss1"
        command = [str(runner), "--rom", str(rom), "--frames", "16", "--state-in", str(current), "--state-out", str(next_state), "--screenshot", str(shot), "--key", f"0:1:{button}"]
        result = subprocess.run(command, text=True, capture_output=True)
        if result.returncode:
            raise LabError(result.stdout + result.stderr)
        os.replace(next_state, current)
        events.append({"index": index, "button": button, "screenshot": str(shot), "screenshot_sha256": sha256(shot)})
    (attempt_dir / "events.jsonl").write_text("".join(json.dumps(row, sort_keys=True) + "\n" for row in events))
    return shot


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--recipes", type=Path, default=DEFAULT_RECIPES)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("index")
    build = sub.add_parser("build")
    build.add_argument("--encounter", required=True)
    build.add_argument("--campaign-run", type=Path, required=True)
    build.add_argument("--elf", type=Path, default=ROOT / "pokeemerald-playthrough.elf")
    validate = sub.add_parser("validate")
    validate.add_argument("checkpoint", type=Path)
    retry = sub.add_parser("retry")
    retry.add_argument("checkpoint", type=Path)
    retry.add_argument("attempt_dir", type=Path)
    retry.add_argument("buttons", nargs="*")
    args = parser.parse_args()
    try:
        manifest = campaign.load_manifest(args.manifest)
        recipes = load_json(args.recipes)
        validate_recipes(manifest, recipes)
        if args.command == "index":
            print(json.dumps(checkpoint_index(manifest, recipes, args.out), indent=2, sort_keys=True))
        elif args.command == "build":
            print(build_checkpoint(args, manifest, recipes))
        elif args.command == "validate":
            print(json.dumps(validate_checkpoint(args.checkpoint), indent=2, sort_keys=True))
        else:
            print(retry_checkpoint(args.checkpoint, args.attempt_dir, args.buttons))
    except (LabError, RuntimeError, OSError, ValueError, KeyError, json.JSONDecodeError) as error:
        print(f"battle-checkpoint-lab: FAIL: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
