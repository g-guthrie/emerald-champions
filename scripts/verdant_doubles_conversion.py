#!/usr/bin/env python3
"""Import, apply, and verify Verdant's mostly-doubles trainer conversion.

The audit package is planning input. The checked-in manifest and current game
source are authoritative after import. Re-running --apply is idempotent.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRAINERS_PATH = ROOT / "src/data/trainers.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"
BASE_STATS_PATH = ROOT / "src/data/pokemon/base_stats.h"
CURATED_BASE_STATS_PATH = ROOT / "src/data/pokemon/verdant_gen9_base_stats.h"
MANIFEST_PATH = ROOT / "docs/verdant_doubles_manifest.json"

BASE_AI = (
    "AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | "
    "AI_FLAG_CHECK_VIABILITY"
)

MARQUEE_TRAINERS = {
    "Roxanne": "TRAINER_ROXANNE_1",
    "Brawly": "TRAINER_BRAWLY_1",
    "Wattson": "TRAINER_WATTSON_1",
    "Flannery": "TRAINER_FLANNERY_1",
    "Norman": "TRAINER_NORMAN_1",
    "Winona": "TRAINER_WINONA_1",
    "Tate & Liza": "TRAINER_TATE_AND_LIZA_1",
    "Juan": "TRAINER_JUAN_1",
    "Sidney": "TRAINER_SIDNEY",
    "Phoebe": "TRAINER_PHOEBE",
    "Glacia": "TRAINER_GLACIA",
    "Drake": "TRAINER_DRAKE",
    "Wallace": "TRAINER_WALLACE",
}

BOSS_LEVEL_OFFSETS = {
    "Roxanne": [-1, -1, -2, -2, 0, 1],
    "Brawly": [2, 3, 3, 3, 4, 5],
    "Wattson": [2, 2, 3, 3, 4, 5],
    "Flannery": [0, 0, 0, 0, 1, 2],
    "Norman": [0, 0, 0, 1, 1, 2],
    "Winona": [0, 0, 0, 0, 1, 2],
    "Tate & Liza": [0, 0, 0, 0, 1, 2],
    "Juan": [0, 0, 0, 0, 1, 2],
    "Sidney": [0, 0, 0, 0, 1, 2],
    "Phoebe": [0, 0, 0, 0, 1, 2],
    "Glacia": [0, 0, 0, 0, 1, 2],
    "Drake": [0, 0, 0, 0, 1, 2],
    "Wallace": [0, 0, 0, 0, 1, 2],
}

FORM_OVERRIDES = {
    ("Wattson", "Rotom"): "SPECIES_ROTOM_MOW",
    ("Brawly", "Urshifu"): "SPECIES_URSHIFU",
    ("Sidney", "Urshifu"): "SPECIES_URSHIFU",
    ("Winona", "Landorus"): "SPECIES_LANDORUS_THERIAN",
    ("Sidney", "Hoopa"): "SPECIES_HOOPA_UNBOUND",
    ("Tate & Liza", "Calyrex"): "SPECIES_CALYREX_ICE_RIDER",
    ("Phoebe", "Calyrex"): "SPECIES_CALYREX_SHADOW_RIDER",
}

# Verdant-specific improvements over the planning package. These keep the
# Champions Mega additions visible during the main campaign.
BOSS_SLOT_OVERRIDES = {
    ("Flannery", 3): {
        "species": "SPECIES_CAMERUPT",
        "item": "ITEM_CHARCOAL",
        "ability": "ABILITY_SOLID_ROCK",
        "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
        "moves": ["MOVE_ERUPTION", "MOVE_EARTH_POWER", "MOVE_HEAT_WAVE", "MOVE_PROTECT"],
        "role": "Trick Room special cannon",
    },
    ("Flannery", 5): {
        "species": "SPECIES_EMBOAR",
        "item": "ITEM_EMBOARITE",
        "ability": "ABILITY_RECKLESS",
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_HEAT_CRASH", "MOVE_CLOSE_COMBAT", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT"],
        "role": "Champions Mega physical closer",
    },
}

ABILITY_ALIASES = {
    "Electric Surge": "ElectroSurge",
    "Psychic Surge": "PsychicSurge",
    "Shadow Shield": "ShadowShield",
    "Chilling Neigh": "ChillngNeigh",
    "Power Construct": "PwrConstruct",
}

def select_rebalanced(text: str) -> str:
    output, stack, active = [], [], True
    for line in text.splitlines():
        match = re.match(r"^\s*#\s*(ifdef|ifndef)\s+REBALANCED_VERSION\s*$", line)
        if match:
            condition = match.group(1) == "ifdef"
            stack.append((active, condition))
            active = active and condition
            continue
        if re.match(r"^\s*#\s*else\s*$", line) and stack:
            parent, condition = stack[-1]
            stack[-1] = (parent, not condition)
            active = parent and not condition
            continue
        if re.match(r"^\s*#\s*endif\s*$", line) and stack:
            active = stack.pop()[0]
            continue
        if active:
            output.append(line)
    return "\n".join(output)


def base_ability_slots() -> dict[str, list[str]]:
    text = select_rebalanced(
        BASE_STATS_PATH.read_text() + "\n" + CURATED_BASE_STATS_PATH.read_text()
    )
    slots = {}
    pattern = re.compile(r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{(.*?)(?=^\s*\[SPECIES_|\Z)", re.M | re.S)
    for match in pattern.finditer(text):
        abilities = re.search(r"\.abilities\s*=\s*\{([^}]+)\}", match.group(2))
        if abilities:
            slots[match.group(1)] = [value.strip() for value in abilities.group(1).split(",")]
    return slots


def ability_slot(species: str, ability: str, slots: dict[str, list[str]]) -> int:
    values = slots.get(species, [])
    if ability not in values:
        raise ValueError(f"{species} does not expose {ability}; found {values}")
    return values.index(ability)


def choose_spread(battle: str, mon: dict, species: dict, moves: dict) -> str:
    physical = sum(moves[name]["category"] == "Physical" for name in mon["moves"])
    special = sum(moves[name]["category"] == "Special" for name in mon["moves"])
    slow_mode = battle in {"Roxanne", "Flannery", "Tate & Liza"} and species["stats"]["speed"] <= 60
    bulky = any(word in mon["role"] for word in ("bulky", "anchor", "guard", "setter", "control", "deterrent", "weather"))
    if physical > special:
        if slow_mode:
            return "SPREAD_31_IV_HP_ATK_BRAVE"
        return "SPREAD_31_IV_HP_ATK_ADAMANT" if bulky else "SPREAD_31_IV_ATK_SPEED_JOLLY"
    if special > physical:
        if slow_mode:
            return "SPREAD_31_IV_HP_SPATK_QUIET"
        return "SPREAD_31_IV_HP_SPATK_MODEST" if bulky else "SPREAD_31_IV_SPATK_SPEED_TIMID"
    return "SPREAD_31_IV_HP_DEF_SPDEF_SASSY"


def import_audit(audit_dir: Path) -> None:
    guide_path = audit_dir / "inputs/verdant-guide.json"
    ledger_path = audit_dir / "outputs/verdant_trainer_conversion_ledger.csv"
    design_path = audit_dir / "first_wave_redesign.json"
    guide = json.loads(guide_path.read_text())
    design = json.loads(design_path.read_text())
    ledger = list(csv.DictReader(ledger_path.open()))
    trainers_text = TRAINERS_PATH.read_text()
    source_ids = set(re.findall(r"^\s*\[((?:TRAINER|MAY|BRENDAN)_[A-Z0-9_]+)\]\s*=", trainers_text, re.M)) - {"TRAINER_NONE"}
    ledger_ids = {row["trainer_id"] for row in ledger}
    if source_ids != ledger_ids:
        raise ValueError(f"audit/source trainer mismatch: missing={sorted(source_ids-ledger_ids)}, extra={sorted(ledger_ids-source_ids)}")

    species_by_name: dict[str, list[dict]] = {}
    for species in guide["species"]:
        species_by_name.setdefault(species["name"], []).append(species)
    moves_by_name = {move["name"]: move for move in guide["moves"]}
    items_by_name = {item["name"]: item["id"] for item in guide["items"]}
    slots = base_ability_slots()

    resolved_bosses = []
    for battle in design["battles"]:
        team = []
        for mon in battle["team"]:
            ability_name = ABILITY_ALIASES.get(mon["ability"], mon["ability"])
            override = FORM_OVERRIDES.get((battle["battle"], mon["species"]))
            candidates = species_by_name[mon["species"]]
            if override:
                species = next(candidate for candidate in candidates if candidate["id"] == override)
            else:
                matching = [candidate for candidate in candidates if ability_name in {a["name"] for a in candidate.get("abilities", [])}]
                non_battle_forms = [candidate for candidate in matching if not any(tag in candidate["id"] for tag in ("_MEGA", "_PRIMAL"))]
                species = (non_battle_forms or matching or candidates)[0]
            ability_id = next(ability["id"] for ability in species["abilities"] if ability["name"] == ability_name)
            item_id = items_by_name[mon["item"]]
            move_ids = [moves_by_name[name]["id"] for name in mon["moves"]]
            team.append({
                "species": species["id"],
                "item": item_id,
                "ability": ability_id,
                "ability_slot": ability_slot(species["id"], ability_id, slots),
                "spread": choose_spread(battle["battle"], mon, species, moves_by_name),
                "moves": move_ids,
                "role": mon["role"],
            })
        for index in range(len(team)):
            override = BOSS_SLOT_OVERRIDES.get((battle["battle"], index))
            if override:
                team[index] = {
                    **override,
                    "ability_slot": ability_slot(override["species"], override["ability"], slots),
                }
        resolved_bosses.append({
            "battle": battle["battle"],
            "trainer_id": MARQUEE_TRAINERS[battle["battle"]],
            "format": battle["format"].lower(),
            "identity": battle["identity"],
            "team": team,
        })

    formats = {}
    for row in ledger:
        current_size = int(row["current_party_size"])
        target_size = max(current_size, int(row["target_party_size"]))
        requested_double = row["recommended_format"] in {"Convert to doubles", "Keep doubles"}
        # Norman and Drake are the two deliberately recurring singles duelists,
        # including their rematch variants. They provide format contrast.
        if row["trainer_id"].startswith(("TRAINER_NORMAN_", "TRAINER_DRAKE")):
            requested_double = False
        if requested_double and target_size % 2:
            target_size += 1
        formats[row["trainer_id"]] = {
            "format": "double" if requested_double else "single",
            "target_size": target_size,
            "archetype": row["recommended_archetype"],
            "difficulty": int(row["current_difficulty_score"]),
            "partner_interaction": row["partner_interaction"] == "True",
            "level_offset": round(float(row["median_level_offset"])),
            "location": row["location"],
        }

    payload = {
        "source": {
            "guide_sha256": hashlib.sha256(guide_path.read_bytes()).hexdigest(),
            "ledger_sha256": hashlib.sha256(ledger_path.read_bytes()).hexdigest(),
            "design_sha256": hashlib.sha256(design_path.read_bytes()).hexdigest(),
            "trainer_count": len(formats),
        },
        "formats": formats,
        "bosses": resolved_bosses,
    }
    MANIFEST_PATH.write_text(json.dumps(payload, indent=2) + "\n")
    print(f"wrote {MANIFEST_PATH} ({len(formats)} trainers, {len(resolved_bosses)} marquee teams)")


TRAINER_BLOCK = re.compile(r"^    \[((?:TRAINER|MAY|BRENDAN)_[A-Z0-9_]+)\] =\n    \{.*?^    \},\n", re.M | re.S)
PARTY_BLOCK_TEMPLATE = r"(^static const struct TrainerMonItemCustomMoves {name}\[\]\s*=\s*\{{[^\n]*\n)(.*?)(^\}};)"


def trainer_blocks(text: str) -> dict[str, re.Match]:
    return {match.group(1): match for match in TRAINER_BLOCK.finditer(text)}


def party_name(block: str) -> str:
    match = re.search(r"\.party\s*=\s*\{\.ItemCustomMoves\s*=\s*(\w+)\}", block)
    if not match:
        raise ValueError("trainer is not using ItemCustomMoves")
    return match.group(1)


def set_ai(block: str, flags: list[str]) -> str:
    match = re.search(r"^(\s*\.aiFlags\s*=\s*)(.*?)(,\s*)$", block, re.M)
    if not match:
        raise ValueError("trainer block has no aiFlags")
    existing = [part.strip() for part in match.group(2).split("|")]
    for flag in flags:
        if flag not in existing:
            existing.append(flag)
    return block[:match.start()] + match.group(1) + " | ".join(existing) + match.group(3) + block[match.end():]


def clear_ai(block: str, flags: set[str]) -> str:
    match = re.search(r"^(\s*\.aiFlags\s*=\s*)(.*?)(,\s*)$", block, re.M)
    if not match:
        raise ValueError("trainer block has no aiFlags")
    existing = [part.strip() for part in match.group(2).split("|")]
    kept = [part for part in existing if part not in flags]
    return block[:match.start()] + match.group(1) + " | ".join(kept) + match.group(3) + block[match.end():]


def rewrite_trainers(text: str, manifest: dict) -> str:
    marquee = {boss["trainer_id"]: boss for boss in manifest["bosses"]}
    pieces, cursor = [], 0
    for match in TRAINER_BLOCK.finditer(text):
        pieces.append(text[cursor:match.start()])
        trainer_id, block = match.group(1), match.group(0)
        rule = manifest["formats"].get(trainer_id)
        if rule:
            desired = "TRUE" if rule["format"] == "double" else "FALSE"
            block = re.sub(r"(\.doubleBattle\s*=\s*)(TRUE|FALSE)", rf"\g<1>{desired}", block)
            block = clear_ai(block, {"AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER"})
            flags = ["AI_FLAG_CHECK_FOE"]
            if rule.get("smart_switching", rule["target_size"] == 6 or rule["difficulty"] >= 70):
                flags.append("AI_FLAG_SMART_SWITCHING")
            if rule["format"] == "double" and (rule["partner_interaction"] or trainer_id in marquee):
                flags.append("AI_FLAG_HELP_PARTNER")
            block = set_ai(block, flags)
        pieces.append(block)
        cursor = match.end()
    pieces.append(text[cursor:])
    return "".join(pieces)


def party_match(text: str, name: str) -> re.Match:
    match = re.search(PARTY_BLOCK_TEMPLATE.format(name=re.escape(name)), text, re.M | re.S)
    if not match:
        raise ValueError(f"party array not found: {name}")
    return match


def active_code(body: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", "", body, flags=re.S)


def species_in_party(body: str) -> list[str]:
    return re.findall(r"\.species\s*=\s*(SPECIES_[A-Z0-9_]+)", active_code(body))


def levels_in_party(body: str) -> list[int]:
    return [int(value) for value in re.findall(r"\.lvl\s*=\s*(-?\d+)", active_code(body))]


def render_mon(mon: dict, level: int, note: str) -> str:
    moves = ", ".join(mon["moves"])
    return (
        "    {\n"
        f"    .lvl = {level},\n"
        f"    .species = {mon['species']},\n"
        f"    .heldItem = {mon['item']},\n"
        f"    .ability = {mon['ability_slot']},\n"
        f"    .spread = {mon['spread']},\n"
        f"    .moves = {moves}\n"
        f"    }} /* {note} */"
    )



def replace_party(text: str, name: str, rendered: list[str]) -> str:
    match = party_match(text, name)
    replacement = match.group(1) + ",\n".join(rendered) + "\n" + match.group(3)
    return text[:match.start()] + replacement + text[match.end():]


def rewrite_parties(text: str, trainers_text: str, manifest: dict) -> str:
    slots = base_ability_slots()
    blocks = trainer_blocks(trainers_text)
    boss_ids = {boss["trainer_id"] for boss in manifest["bosses"]}

    # Story bosses are fully hand-authored. Preserve their existing level offsets.
    for boss in manifest["bosses"]:
        name = party_name(blocks[boss["trainer_id"]].group(0))
        levels = BOSS_LEVEL_OFFSETS[boss["battle"]]
        rendered = [render_mon(mon, levels[index], f"Verdant doubles: {mon['role']}") for index, mon in enumerate(boss["team"])]
        text = replace_party(text, name, rendered)

    # Ordinary teams are authored explicitly by verdant_custom_teams.py. This
    # converter owns only format flags and the hand-authored story bosses; it
    # must never synthesize repeated filler cores again.
    for trainer_id, rule in manifest["formats"].items():
        if rule["format"] != "double" or trainer_id in boss_ids:
            continue
        name = party_name(blocks[trainer_id].group(0))
        match = party_match(text, name)
        body = match.group(2)
        existing = species_in_party(body)
        target = rule["target_size"]
        if len(existing) != target:
            raise ValueError(
                f"{trainer_id}/{name} has {len(existing)} mons, expected {target}; "
                "run scripts/verdant_custom_teams.py --apply"
            )
    return text


def apply_conversion() -> None:
    manifest = json.loads(MANIFEST_PATH.read_text())
    original_trainers = TRAINERS_PATH.read_text()
    original_parties = PARTIES_PATH.read_text()
    updated_trainers = rewrite_trainers(original_trainers, manifest)
    updated_parties = rewrite_parties(original_parties, updated_trainers, manifest)
    if updated_trainers != original_trainers:
        TRAINERS_PATH.write_text(updated_trainers)
    if updated_parties != original_parties:
        PARTIES_PATH.write_text(updated_parties)
    print(f"updated trainers={updated_trainers != original_trainers}, parties={updated_parties != original_parties}")


def verify_conversion() -> None:
    manifest = json.loads(MANIFEST_PATH.read_text())
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    blocks = trainer_blocks(trainers_text)
    problems = []
    doubles = 0
    for trainer_id, rule in manifest["formats"].items():
        block = blocks[trainer_id].group(0)
        actual_double = ".doubleBattle = TRUE" in block
        expected_double = rule["format"] == "double"
        doubles += actual_double
        if actual_double != expected_double:
            problems.append(f"{trainer_id}: format mismatch")
        if "AI_FLAG_CHECK_FOE" not in block:
            problems.append(f"{trainer_id}: missing foe-aware AI")
        name = party_name(block)
        size = len(species_in_party(party_match(parties_text, name).group(2)))
        if expected_double and size != rule["target_size"]:
            problems.append(f"{trainer_id}: doubles size {size}, expected {rule['target_size']}")
        if expected_double and not rule.get("multi_partner") and (size < 4 or size % 2):
            problems.append(f"{trainer_id}: unsafe doubles party size {size}")

    for boss in manifest["bosses"]:
        block = blocks[boss["trainer_id"]].group(0)
        name = party_name(block)
        body = party_match(parties_text, name).group(2)
        if species_in_party(body) != [mon["species"] for mon in boss["team"]]:
            problems.append(f"{boss['battle']}: story team mismatch")
        mega_count = sum(mon["item"].endswith(("ITE", "NITE")) and mon["item"] != "ITEM_EVIOLITE" for mon in boss["team"])
        if mega_count > 1:
            problems.append(f"{boss['battle']}: {mega_count} Mega items")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(f"PASS: {len(manifest['formats'])} trainers validated; {doubles} doubles and {len(manifest['formats']) - doubles} intentional singles")
    print(f"PASS: {len(manifest['bosses'])} marquee teams and every doubles party has an even four/six-mon wave")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--import-audit", type=Path)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not any((args.import_audit, args.apply, args.check)):
        parser.error("choose --import-audit, --apply, or --check")
    if args.import_audit:
        import_audit(args.import_audit.resolve())
    if args.apply:
        apply_conversion()
    if args.check:
        verify_conversion()


if __name__ == "__main__":
    main()
