#!/usr/bin/env python3
"""Per-Mega-Stone register: enabled form(s), world acquisition source, every
trainer holder (with class and whether mega_slots actually authorizes battle
evolution), a signature owner, secondary owners, and the earliest cap window
in which the player can obtain the matching base family.

This is a design-review index, not a reachability or showcase-acceptance
claim. In particular:
  - "player family access" only traces ordinary wild encounter tables,
    givemon/giveegg script grants and legendary-sign wild/visible sources.
    Trades, Pickup/lottery odds and other indirect routes are not modeled;
    a species with none of those recorded sources is printed "unmapped",
    not "unavailable".
  - "mega_slots permits evolution" reports whether the holder's authored
    mega_slots bitmask covers that party position, not whether the AI will
    actually trigger the Mega Evolution in a given battle.
  - Signature owner is the design tier used across this file, not a claim
    about difficulty or story weight:
      elite/leader > gym > admin/boss > rival > ace > brain > regular/casual/grunt
    Ties break on first-access order: the lower docs/trainer-review-index.json
    review_index (falling back to encounter number when a trainer id is not
    indexed) wins.
"""
from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path

import emerald_champions_teams as teams
import export_trainer_catalogue as trainers
from generate_emerald_champions_mega_archive import stones
from verify_mega_stone_rewards import world_reward_sources
from verify_trainer_ability_legality import preprocess_species_info

ROOT = Path(__file__).resolve().parents[1]
FORM_TABLES = ROOT / "src/data/pokemon/form_change_tables.h"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
REVIEW_INDEX = ROOT / "docs/trainer-review-index.json"
WILD = ROOT / "src/data/wild_encounters.json"
LEGENDARY_SIGNS = ROOT / "src/data/pokemon/legendary_signs.h"
MAPS_DIR = ROOT / "data/maps"

# Design tier used only to pick a "signature" holder among several; not a
# difficulty or story-importance ranking. "boss" is not named in the user's
# order and is grouped with admin as the nearest equivalent multi-owner class.
TIER = {
    "elite": 6, "leader": 6,
    "gym": 5,
    "admin": 4, "boss": 4,
    "rival": 3,
    "ace": 2,
    "brain": 1,
    "regular": 0, "casual": 0, "grunt": 0,
}

SPECIAL_FORM_NOTES = {
    "SPECIES_RAYQUAZA_MEGA": "Mega Rayquaza -- move-triggered (Dragon Ascent), not one of the 99 item-based stones; listed separately.",
    "SPECIES_MEOWSTIC_F_MEGA": "female Meowstic form of Meowsticite (shares the stone with SPECIES_MEOWSTIC_M_MEGA).",
    "SPECIES_MAGEARNA_ORIGINAL_MEGA": "original-color Magearna form of Magearnite (shares the stone with SPECIES_MAGEARNA_MEGA).",
    "SPECIES_TATSUGIRI_CURLY_MEGA": "Tatsugiri Curly form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_DROOPY_MEGA": "Tatsugiri Droopy form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_STRETCHY_MEGA": "Tatsugiri Stretchy form of Tatsugirinite.",
}


def pretty(token: str) -> str:
    return re.sub(r"^(SPECIES|ITEM|MOVE)_", "", token).replace("_", " ").title()


def item_forms() -> dict[str, list[str]]:
    """ITEM_x -> every SPECIES_x_MEGA[...] it triggers (some items cover more
    than one form: Meowsticite, Magearnite, Tatsugirinite)."""
    text = FORM_TABLES.read_text()
    forms: dict[str, list[str]] = defaultdict(list)
    for species, item in re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)", text
    ):
        forms[item].append(species)
    return dict(forms)


def move_triggered_forms() -> dict[str, str]:
    """SPECIES_x_MEGA -> MOVE_x for the non-item Mega triggers (Rayquaza)."""
    text = FORM_TABLES.read_text()
    return dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE,\s*(SPECIES_[A-Z0-9_]+),\s*(MOVE_[A-Z0-9_]+)", text
    ))


def base_species(mega_species: str) -> str:
    """Strip a Mega/Mega-X/Y/Z suffix back to the base species token."""
    return re.sub(r"_MEGA(_[XYZ])?$", "", mega_species)


def evolution_edges() -> dict[str, set[str]]:
    """Undirected adjacency over configured evolution relationships, used to
    find every family member (pre-evolutions and evolutions) of a species."""
    text = preprocess_species_info()
    text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    adjacency: dict[str, set[str]] = defaultdict(set)
    for i, m in enumerate(marks):
        body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
        evo = re.search(r"\.evolutions\s*=\s*(.*?)(?:\n\s*\.\w+\s*=|\n\s*\},?\s*\Z)", body, re.S)
        if not evo:
            continue
        for target in re.findall(r"SPECIES_\w+", evo[1]):
            adjacency[m[1]].add(target)
            adjacency[target].add(m[1])
    return adjacency


def family_of(species: str, adjacency: dict[str, set[str]]) -> set[str]:
    if species not in adjacency:
        return {species}
    seen = {species}
    stack = [species]
    while stack:
        cur = stack.pop()
        for nxt in adjacency[cur]:
            if nxt not in seen:
                seen.add(nxt)
                stack.append(nxt)
    return seen


def map_dir_to_id() -> dict[str, str]:
    result = {}
    for path in sorted(MAPS_DIR.glob("*/map.json")):
        try:
            result[path.parent.name] = json.loads(path.read_text())["id"]
        except (OSError, ValueError, KeyError):
            continue
    return result


def map_cap_index() -> dict[str, int]:
    """MAP_x -> earliest strict_cap of any authored encounter whose
    'location' field is that map's directory. This is the encounter index
    described in the task: it only covers maps that host an authored trainer
    battle; other maps fall through to 'unmapped'."""
    dir_to_id = map_dir_to_id()
    _, blocks = teams.split_encounters(MASTER.read_text())
    cap_by_map: dict[str, int] = {}
    for _number, block in blocks:
        meta = trainers.fields(block)
        loc = meta.get("location", "").strip()
        cap = meta.get("strict_cap", "").strip()
        mapid = dir_to_id.get(loc)
        if not mapid or not cap.isdigit():
            continue
        cap = int(cap)
        if mapid not in cap_by_map or cap < cap_by_map[mapid]:
            cap_by_map[mapid] = cap
    return cap_by_map


def species_wild_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where it appears in the wild table."""
    data = json.loads(WILD.read_text())
    result: dict[str, set[str]] = defaultdict(set)
    for group in data["wild_encounter_groups"]:
        if not group.get("for_maps"):
            continue
        for entry in group.get("encounters", []):
            mapid = entry.get("map")
            if not mapid:
                continue
            for field in entry.values():
                if isinstance(field, dict) and "mons" in field:
                    for mon in field["mons"]:
                        result[mon["species"]].add(mapid)
    return result


def species_gift_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where a givemon/giveegg script grants it
    (Hoenn map-local scripts only; FRLG maps are out of scope elsewhere in
    this tree). No Hoenn-side fossil-revival script pattern distinct from
    givemon was found under data/maps or data/scripts."""
    dir_to_id = map_dir_to_id()
    result: dict[str, set[str]] = defaultdict(set)
    for path in sorted(MAPS_DIR.glob("*/scripts.inc")):
        mapid = dir_to_id.get(path.parent.name)
        if not mapid:
            continue
        text = path.read_text()
        for species in re.findall(r"\b(?:givemon|giveegg)\s+(SPECIES_\w+)", text):
            result[species].add(mapid)
    return result


def species_legendary_sign_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids from wild/visible legendary sign macros
    (OTHER_SIGN entries use a non-map source constant and are skipped)."""
    text = LEGENDARY_SIGNS.read_text()
    result: dict[str, set[str]] = defaultdict(set)
    for kind, species, maparg in re.findall(
        r"\b(RARE_WILD_SIGN|NATIVE_WILD_SIGN|VISIBLE_SIGN|ORDINARY_WILD_SIGN)\(\s*\w+,\s*(\w+),\s*(\w+)", text
    ):
        result["SPECIES_" + species].add("MAP_" + maparg)
    return result


def review_index_order() -> dict[str, int]:
    """TRAINER_x -> review_index, from docs/trainer-review-index.json."""
    if not REVIEW_INDEX.exists():
        return {}
    data = json.loads(REVIEW_INDEX.read_text())
    order: dict[str, int] = {}
    for encounter in data["encounters"]:
        for trainer_id in encounter["trainer_ids"]:
            order[trainer_id] = encounter["review_index"]
    return order


def build_register() -> list[dict]:
    all_stones = stones()
    forms = item_forms()
    rewards = world_reward_sources()
    branches = teams.read_teams()
    order = review_index_order()
    adjacency = evolution_edges()
    cap_by_map = map_cap_index()
    wild_maps = species_wild_maps()
    gift_maps = species_gift_maps()
    sign_maps = species_legendary_sign_maps()

    rows = []
    for item in all_stones:
        holders = []
        for branch in branches:
            for index, mon in enumerate(branch.mons):
                if "ITEM_" + mon.item != item:
                    continue
                can_evolve = bool(branch.mega_slots is not None and (branch.mega_slots >> index) & 1)
                holders.append(dict(
                    trainer=branch.trainer, encounter=f"E{branch.encounter:04d}",
                    cls=branch.cls, can_evolve=can_evolve,
                    review_index=order.get(branch.trainer),
                ))
        if holders:
            def sort_key(h):
                tier = TIER.get(h["cls"], 0)
                access = h["review_index"] if h["review_index"] is not None else 10 ** 9
                return (-tier, access)
            holders.sort(key=sort_key)
            signature = holders[0]
            secondary = holders[1:]
        else:
            signature = None
            secondary = []

        species_forms = forms.get(item, [])
        family_caps = []
        unmapped_species = []
        for form_species in species_forms:
            root = base_species(form_species)
            family = family_of(root, adjacency)
            maps = set()
            for sp in family:
                maps |= wild_maps.get(sp, set())
                maps |= gift_maps.get(sp, set())
                maps |= sign_maps.get(sp, set())
            caps = [cap_by_map[m] for m in maps if m in cap_by_map]
            if caps:
                family_caps.append(min(caps))
            else:
                unmapped_species.append(root)

        rows.append(dict(
            stone=item,
            forms=species_forms,
            world_source=rewards.get(item, []),
            holders=holders,
            signature_owner=signature,
            secondary_owners=secondary,
            player_family_access=(min(family_caps) if family_caps else None),
            player_family_access_unmapped=sorted(set(unmapped_species)) if not family_caps else [],
            no_trainer_holder=not holders,
        ))
    return rows


def special_form_rows() -> list[dict]:
    """Rows for forms the book calls out that fall outside the 99 item-based
    stones (Mega Rayquaza) or that share one of those stones with a sibling
    form already covered by build_register (Meowstic/Magearna/Tatsugiri --
    reprinted here for a single reference list, per the task's request)."""
    branches = teams.read_teams()
    moves = move_triggered_forms()
    rows = []
    for species, move in moves.items():
        root = base_species(species)
        owners = []
        for branch in branches:
            for index, mon in enumerate(branch.mons):
                if mon.species != root[len("SPECIES_"):]:
                    continue
                has_move = move[len("MOVE_"):] in mon.moves
                owners.append(dict(trainer=branch.trainer, cls=branch.cls, encounter=f"E{branch.encounter:04d}", has_trigger_move=has_move))
        rows.append(dict(kind="move-triggered", species=species, trigger=move,
                          note=SPECIAL_FORM_NOTES.get(species, ""), owners=owners))
    for species, note in SPECIAL_FORM_NOTES.items():
        if species in moves:
            continue
        rows.append(dict(kind="shared-stone-form", species=species, trigger=None, note=note, owners=[]))
    return rows


def render_lines() -> tuple[list[str], list[dict], list[dict]]:
    rows = build_register()
    specials = special_form_rows()
    no_holder = [r["stone"] for r in rows if r["no_trainer_holder"]]
    lines = [
        "\nMEGA REGISTER",
        f"{len(rows)} item-based Mega Stones. Signature owner = highest design tier "
        "(elite/leader>gym>admin/boss>rival>ace>brain>regular/casual/grunt), ties broken "
        "by earlier docs/trainer-review-index.json review_index. mega_slots=yes means the "
        "holder's authored Mega permission bitmask covers that party position, not that the "
        "AI will trigger it in a given battle. player_family_access is the earliest strict_cap "
        "of any authored trainer encounter located on a map where the base family is available "
        "in the wild table, a givemon/giveegg script grant, or a legendary sign; 'unmapped' means "
        "no such source was found under this index's rules (trades and Pickup/lottery odds are "
        "not modeled).",
        f"NO TRAINER HOLDER rows: {len(no_holder)}" + (f" -- {', '.join(no_holder)}" if no_holder else " (none)"),
    ]
    for row in rows:
        forms_str = ", ".join(pretty(f) for f in row["forms"]) or "(no form found in form_change_tables.h)"
        source_str = "; ".join(row["world_source"]) or "NO WORLD SOURCE FOUND"
        lines.append(f"\n{pretty(row['stone'])} ({row['stone']}) -> {forms_str}")
        lines.append(f"  World source: {source_str}")
        if row["no_trainer_holder"]:
            lines.append("  NO TRAINER HOLDER")
        else:
            sig = row["signature_owner"]
            lines.append(f"  Signature owner: {sig['trainer']} ({sig['encounter']}, class={sig['cls']}, "
                          f"mega_slots evolves this slot={'yes' if sig['can_evolve'] else 'no'})")
            if row["secondary_owners"]:
                lines.append("  Secondary owners: " + "; ".join(
                    f"{h['trainer']} ({h['encounter']}, class={h['cls']}, "
                    f"mega_slots evolves this slot={'yes' if h['can_evolve'] else 'no'})"
                    for h in row["secondary_owners"]))
        if row["player_family_access"] is not None:
            lines.append(f"  Player family access: cap {row['player_family_access']}")
        else:
            lines.append("  Player family access: unmapped" + (
                f" ({', '.join(pretty(s) for s in row['player_family_access_unmapped'])})"
                if row["player_family_access_unmapped"] else ""))
    lines.append("\nSPECIAL FORMS CALLED OUT BY THE BOOK")
    for row in specials:
        if row["kind"] == "move-triggered":
            owners = row["owners"]
            if owners:
                status = "; ".join(f"{o['trainer']} ({o['encounter']}, class={o['cls']}, "
                                    f"has {pretty(row['trigger'])}={'yes' if o['has_trigger_move'] else 'no'})" for o in owners)
            else:
                status = "NO TRAINER HOLDER of the base species"
            lines.append(f"{pretty(row['species'])} -- move-triggered by {pretty(row['trigger'])}: {status}")
        else:
            lines.append(f"{pretty(row['species'])} -- {row['note']}")
    return lines, rows, specials


def generate(root: Path = ROOT) -> tuple[list[str], dict, set[Path]]:
    lines, rows, specials = render_lines()
    catalog = dict(mega_register=rows, special_forms=specials)
    paths = {
        Path(__file__), FORM_TABLES, MASTER, REVIEW_INDEX, WILD, LEGENDARY_SIGNS,
        root / "src/data/emerald_champions_mega_stones.h",
        teams.TEAMS,
    }
    return lines, catalog, paths


def main() -> None:
    lines, catalog, _paths = generate()
    print("\n".join(lines))
    out = ROOT / "work/mega-register.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(catalog, indent=2) + "\n")
    no_holder = [r["stone"] for r in catalog["mega_register"] if r["no_trainer_holder"]]
    print(f"\n{'PASS' if not no_holder else 'FAIL'}: {len(catalog['mega_register'])} stones; "
          f"{len(no_holder)} with NO TRAINER HOLDER")


if __name__ == "__main__":
    main()
