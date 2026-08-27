#!/usr/bin/env python3
"""Import and validate alternate doubles sets from the Champions handbook."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import unicodedata
from collections import Counter, defaultdict
from pathlib import Path

import verdant_battle_set_presets as presets


ROOT = Path(__file__).resolve().parents[1]
RAW = ROOT / "docs/pokemon_champions_handbook_sets.json"
MANIFEST = ROOT / "docs/verdant_multi_battle_sets.json"
HEADER = ROOT / "src/data/pokemon/verdant_multi_battle_sets.h"
PROTECTED_HEADER = ROOT / "src/data/pokemon/verdant_protected_set_items.h"
DEFAULTS = ROOT / "docs/verdant_battle_set_presets.json"
MAX_SETS = 3
MAX_NAME_CHARS = 23

MANUAL_DEFAULT_PROMOTIONS = [
    {
        "species": "SPECIES_AZUMARILL",
        "change": "MOVE_KNOCK_OFF -> MOVE_PLAY_ROUGH",
        "reason": "The handbook's M-B Belly Drum set restores powerful second STAB and prevents Water resists from blanking the default cleaner.",
    },
    {
        "species": "SPECIES_BLAZIKEN",
        "change": "MOVE_LOW_KICK -> MOVE_CLOSE_COMBAT",
        "reason": "The handbook's M-B attacker uses reliable high-power Fighting STAB instead of target-weight-dependent damage.",
    },
]


SPECIES_ALIASES = {
    "blacephalon": "SPECIES_BLACEPHALON",
    "flabebe": "SPECIES_FLABEBE",
    "hooh": "SPECIES_HO_OH",
    "jangmoo": "SPECIES_JANGMO_O",
    "hakamoo": "SPECIES_HAKAMO_O",
    "kommoo": "SPECIES_KOMMO_O",
    "mimetype": "SPECIES_MIME_JR",
    "mrmime": "SPECIES_MR_MIME",
    "nidoranf": "SPECIES_NIDORAN_F",
    "nidoranm": "SPECIES_NIDORAN_M",
    "porygonz": "SPECIES_PORYGON_Z",
    "typenull": "SPECIES_TYPE_NULL",
}


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def normalize(value: str) -> str:
    value = unicodedata.normalize("NFKD", value).encode("ascii", "ignore").decode()
    value = value.replace("♀", "f").replace("♂", "m").lower()
    return re.sub(r"[^a-z0-9]", "", value)


def constant_guess(name: str) -> str:
    ascii_name = unicodedata.normalize("NFKD", name).encode("ascii", "ignore").decode().upper()
    return "SPECIES_" + re.sub(r"[^A-Z0-9]+", "_", ascii_name).strip("_")


def compact_role(role: str, default: bool = False) -> str:
    role = role.replace("Doubles ", "").replace("Doubles", "Support")
    role = role.replace(" — ", " - ")
    lower = role.lower()
    mega = ""
    if "mega" in lower:
        if "mega-x" in lower or "mega x" in lower:
            mega = "Mega X "
        elif "mega-y" in lower or "mega y" in lower:
            mega = "Mega Y "
        else:
            mega = "Mega "
        role = role.split(" - ", 1)[0]
        role = re.sub(r"^Mega\s+", "", role, flags=re.I)
    replacements = (
        ("Special Attacker", "Special Attacker"),
        ("Physical Attacker", "Physical Attacker"),
        ("Setup Sweeper", "Setup Sweeper"),
        ("Special Wall", "Special Wall"),
        ("Physical Wall", "Physical Wall"),
        ("Tailwind Support", "Tailwind"),
        ("Trick Room Support", "Trick Room"),
        ("Fast Attacker", "Fast Attacker"),
        ("Bulky Attacker", "Bulky Attacker"),
        ("Bulky Setup", "Bulky Setup"),
        ("Wallbreaker", "Wallbreaker"),
        ("Choice Attacker", "Choice Attacker"),
        ("Offensive Protect", "Offensive"),
        ("Bulky Protect", "Bulky Setup"),
        ("Support", "Support"),
    )
    chosen = None
    for needle, label in replacements:
        if needle.lower() in role.lower():
            chosen = label
            break
    if chosen is None and default:
        keywords = (
            ("trick room", "Trick Room"), ("tailwind", "Tailwind"),
            ("redirection", "Redirection"), ("support", "Support"),
            ("special", "Special Attacker"), ("physical", "Physical Attacker"),
            ("wall", "Bulky Support"), ("setup", "Setup Sweeper"),
            ("spread", "Spread Attacker"), ("attacker", "Attacker"),
        )
        chosen = next((label for needle, label in keywords if needle in lower), "Recommended")
    elif chosen is None:
        chosen = role.split(" - ", 1)[0].strip()
    label = (mega + chosen).strip()
    if len(label) > MAX_NAME_CHARS:
        label = label.replace("Physical Attacker", "Physical").replace("Special Attacker", "Special")
        label = label.replace("Setup Sweeper", "Sweeper").replace("Bulky Attacker", "Bulky")
    if len(label) > MAX_NAME_CHARS:
        label = label[:MAX_NAME_CHARS].rsplit(" ", 1)[0].rstrip()
    return label or "Recommended"


def extract_docx(path: Path) -> dict:
    try:
        from docx import Document
    except ImportError as exc:
        raise SystemExit("python-docx is required only for --extract-docx") from exc
    document = Document(path)
    rows: list[dict] = []
    species: dict | None = None
    current: dict | None = None
    for paragraph in document.paragraphs:
        text = paragraph.text.strip()
        if paragraph.style.name == "Heading 2":
            match = re.match(r"#(\d+)\s+(.+)", text)
            if match:
                species = {"national_dex": int(match.group(1)), "species_name": match.group(2).strip()}
                current = None
        elif paragraph.style.name == "Heading 3" and species:
            match = re.match(r"Set (\d+):\s*(.*?)\s*\[([^]]+)\]$", text)
            if match:
                current = {
                    **species,
                    "set_number": int(match.group(1)),
                    "role": match.group(2).strip(),
                    "evidence": match.group(3).strip(),
                }
                rows.append(current)
        elif paragraph.style.name == "Set Details" and current:
            if text.startswith("Moves: "):
                current["moves"] = [value.strip() for value in text[7:].split("•")]
            elif text.startswith("Item / Nature / Ability: "):
                values = [value.strip() for value in text[len("Item / Nature / Ability: "):].split("•")]
                if len(values) == 3:
                    current["item"], current["nature"], current["ability"] = values
    if len(rows) != 1216 or any(not all(field in row for field in ("moves", "item", "nature", "ability")) for row in rows):
        raise ValueError(f"unexpected handbook structure: parsed {len(rows)} complete/partial sets")
    return {
        "schema_version": 1,
        "source_file": path.name,
        "source_sha256": sha256(path),
        "declared_species_count": 1025,
        "declared_set_count": 1216,
        "sets": rows,
    }


def species_mapper(dex: presets.LocalDex):
    by_name: dict[str, list[str]] = defaultdict(list)
    for species, name in dex.names.items():
        if species in dex.supported:
            by_name[normalize(name)].append(species)

    def resolve(name: str) -> str | None:
        key = normalize(name)
        alias = SPECIES_ALIASES.get(key)
        if alias in dex.supported:
            return alias
        guess = constant_guess(name)
        if guess in dex.supported:
            return guess
        candidates = by_name.get(key, [])
        if not candidates:
            return None
        return min(candidates, key=lambda species: (species.count("_"), len(species), dex.values[species]))

    return resolve


def item_mapper():
    constants_source = (ROOT / "include/constants/items.h").read_text()
    item_tokens = set(re.findall(r"^#define\s+(ITEM_[A-Z0-9_]+)\b", constants_source, re.M))
    source = (ROOT / "src/data/items.h").read_text()
    matches = list(re.finditer(r"^\s*\[(ITEM_[A-Z0-9_]+)\]\s*=", source, re.M))
    by_name: dict[str, str] = {}
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(source)
        body = source[match.end():end]
        name = re.search(r'\.name\s*=\s*_\("([^"]+)"\)', body)
        if name:
            by_name[normalize(name.group(1))] = match.group(1)
    by_name["none"] = "ITEM_NONE"

    def resolve(name: str) -> str | None:
        guess = "ITEM_" + re.sub(
            r"[^A-Z0-9]+",
            "_",
            unicodedata.normalize("NFKD", name).encode("ascii", "ignore").decode().upper(),
        ).strip("_")
        return guess if guess in item_tokens else by_name.get(normalize(name))

    return resolve


def build() -> dict:
    raw = json.loads(RAW.read_text())
    default_payload = json.loads(DEFAULTS.read_text())
    defaults = {row["species"]: row for row in default_payload["presets"]}
    dex = presets.LocalDex()
    resolve_species = species_mapper(dex)
    resolve_item = item_mapper()
    grouped_raw: dict[str, list[dict]] = defaultdict(list)
    skipped = Counter()

    for row in raw["sets"]:
        species = resolve_species(row["species_name"])
        if species is None or species not in defaults:
            skipped["species unavailable in Emerald Champions"] += 1
            continue
        grouped_raw[species].append(row)

    alternatives: list[dict] = []
    ranges: dict[str, dict] = {}
    default_names: dict[str, str] = {}

    for species in dex.supported:
        default = defaults[species]
        default_moves = [move for move in default["moves"] if move != "MOVE_NONE"]
        default_signature = (frozenset(default_moves), default["nature"], default["ability"], default["runtime_item"])
        legal = dex.legal_moves(species)
        selectable = legal - presets.UNSAFE_AUTOBUILD_MOVES
        desired_alternatives = min(MAX_SETS, len(grouped_raw.get(species, []))) - 1
        if desired_alternatives <= 0:
            continue
        candidates: list[dict] = []
        seen = {default_signature}

        for row in grouped_raw.get(species, []):
            published_moves = [dex.move_by_name.get(presets.compact(move)) for move in row["moves"]]
            moves = []
            for move in published_moves:
                if move is not None and move in selectable and move not in moves:
                    moves.append(move)
            if len(moves) < min(4, len(row["moves"])):
                skipped["published move slots adapted to exact local legality"] += 1
            moves = presets.fallback_moves(species, selectable or legal, dex, seed=moves)[:4]
            nature_name = presets.compact(row["nature"].removesuffix(" nature"))
            nature = "NATURE_" + nature_name.upper() if nature_name in presets.NATURES else default["nature"]
            ability_key = presets.compact(row["ability"])
            ability_lookup = {
                presets.compact(ability.removeprefix("ABILITY_")): ability
                for ability in dex.stats[species].abilities
                if ability != "ABILITY_NONE"
            }
            ability = ability_lookup.get(ability_key, default["ability"])
            ability_slot = dex.stats[species].abilities.index(ability)
            published_item = resolve_item(row["item"])
            if published_item is None:
                published_item = default["suggested_item"]
                skipped["published item adapted to a local item"] += 1
            runtime_item = presets.runtime_set_item(published_item, row["role"], moves)
            signature = (frozenset(moves), nature, ability, runtime_item)
            if signature in seen:
                skipped["duplicates the current or another retained set"] += 1
                continue
            seen.add(signature)
            candidates.append({
                "species": species,
                "name": compact_role(row["role"]),
                "moves": moves + ["MOVE_NONE"] * (4 - len(moves)),
                "nature": nature,
                "ability": ability,
                "ability_slot": ability_slot,
                "suggested_item": published_item,
                "runtime_item": runtime_item,
                "handbook": row,
                "ability_adapted": ability_lookup.get(ability_key) is None,
                "item_adapted": resolve_item(row["item"]) is None or published_item in presets.PROTECTED_SET_ITEMS,
                "move_difference": len(set(moves) ^ set(default_moves)),
            })

        evidence_rank = {"M-B ladder data": 0, "Smogon doubles pool": 1, "Projected": 2}
        candidates.sort(key=lambda row: (
            row["handbook"]["set_number"] == 1,
            row["handbook"]["set_number"],
            evidence_rank.get(row["handbook"]["evidence"], 3),
            -row["move_difference"],
            row["name"],
        ))
        chosen: list[dict] = []
        review = default.get("authored_review") or {}
        default_label = compact_role(review.get("role", "Recommended"), default=True)
        used_names: set[str] = {default_label}
        for candidate in candidates:
            base_name = candidate["name"]
            name = base_name
            for suffix in (" II", " III"):
                if name not in used_names:
                    break
                name = (base_name[: MAX_NAME_CHARS - len(suffix)] + suffix).strip()
            candidate["name"] = name
            used_names.add(name)
            chosen.append(candidate)
            if len(chosen) == desired_alternatives:
                break
        if not chosen:
            continue
        offset = len(alternatives)
        alternatives.extend(chosen)
        ranges[species] = {"offset": offset, "count": len(chosen)}
        default_names[species] = default_label

    result = {
        "schema_version": 1,
        "source": {
            "file": raw["source_file"],
            "sha256": raw["source_sha256"],
            "raw_sets": len(raw["sets"]),
        },
        "policy": {
            "default": "The existing individually authored Emerald Champions preset remains Set 1 and the ordinary-wild capture default.",
            "alternatives": "The handbook's one-to-three role count is authoritative for mapped species. Set 1 is the authored local default; remaining documented roles are legality-adapted into Sets 2 and 3.",
            "held_items": "Each runtime set equips one free ordinary competitive item. Protected form/progression recommendations are adapted to a role-appropriate free item and remain manually equipped progression rewards.",
            "promotion": "A handbook set may improve Set 1 only through a manual source-backed superiority decision recorded in default_promotions and the authored review batch.",
        },
        "set_count": len(defaults) + len(alternatives),
        "mapped_handbook_species": len(grouped_raw),
        "expected_alternative_count": sum(min(MAX_SETS, len(rows)) - 1 for rows in grouped_raw.values()),
        "species_with_choices": len(ranges),
        "alternative_count": len(alternatives),
        "default_promotions": MANUAL_DEFAULT_PROMOTIONS,
        "skipped": dict(sorted(skipped.items())),
        "ranges": ranges,
        "default_names": default_names,
        "alternatives": alternatives,
    }
    item_source = (ROOT / "include/constants/items.h").read_text()
    berry_block = item_source.split("// Berries", 1)[1].split("// Items", 1)[0]
    berries = set(re.findall(r"^#define\s+(ITEM_[A-Z0-9_]+_BERRY)\b", berry_block, re.M))
    unlock_block = (ROOT / "src/item.c").read_text().split("sBattleItemUnlocks[]", 1)[1].split("};", 1)[0]
    legacy_battle_items = set(re.findall(r"\{(ITEM_[A-Z0-9_]+),", unlock_block))
    runtime_items = {row["runtime_item"] for row in defaults.values()} | {row["runtime_item"] for row in alternatives}
    item_order = {
        item: index
        for index, item in enumerate(re.findall(r"^#define\s+(ITEM_[A-Z0-9_]+)\b", item_source, re.M))
    }
    free_items = sorted(
        (berries | legacy_battle_items | runtime_items) - presets.PROTECTED_SET_ITEMS - {"ITEM_NONE"},
        key=lambda item: (item_order.get(item, 1 << 30), item),
    )
    result["free_items"] = free_items
    result["free_item_count"] = len(free_items)
    return result


def c_escape(value: str) -> str:
    return value.replace("\\", "\\\\").replace('"', '\\"')


def render_header(payload: dict) -> str:
    lines = [
        "// Generated by scripts/verdant_handbook_battle_sets.py. Do not edit by hand.",
    ]
    for species, name in payload["default_names"].items():
        lines.append(f'static const u8 sVerdantDefaultSetName_{species.removeprefix("SPECIES_")}[] = _("{c_escape(name)}");')
    lines.extend([
        "",
        "const u8 *const gVerdantDefaultBattleSetNames[NUM_SPECIES] =",
        "{",
    ])
    for species in payload["default_names"]:
        lines.append(f"    [{species}] = sVerdantDefaultSetName_{species.removeprefix('SPECIES_')},")
    lines.extend(["};", "", "const struct VerdantBattleSetRange gVerdantBattleSetRanges[NUM_SPECIES] =", "{"])
    for species, row in payload["ranges"].items():
        lines.append(f"    [{species}] = {{{row['offset']}, {row['count']}}},")
    lines.extend(["};", "", "const struct VerdantBattleSetChoice gVerdantBattleSetAlternatives[] =", "{"])
    for row in payload["alternatives"]:
        lines.extend([
            "    {",
            f'        .name = _("{c_escape(row["name"])}"),',
            "        .preset =",
            "        {",
            "            .moves = {" + ", ".join(row["moves"]) + "},",
            f"            .nature = {row['nature']},",
            f"            .abilitySlot = {row['ability_slot']},",
            f"            .item = {row['runtime_item']},",
            "        },",
            "    },",
        ])
    lines.extend(["};", "", "const u16 gVerdantFreeBattleItems[] =", "{"])
    lines.extend(f"    {item}," for item in payload["free_items"])
    lines.extend(["    ITEM_NONE,", "};", ""])
    return "\n".join(lines)


def render_protected_header() -> str:
    mega_block = (ROOT / "include/constants/items.h").read_text().split("// Mega Stones", 1)[1].split("// Unused", 1)[0]
    mega_items = set(re.findall(r"^#define\s+(ITEM_[A-Z0-9_]+)\b", mega_block, re.M))
    tokens = sorted(presets.PROTECTED_SET_ITEMS - mega_items)
    return "\n".join([
        "// Generated by scripts/verdant_handbook_battle_sets.py. Do not edit by hand.",
        *(f"    case {item}:" for item in tokens),
        "",
    ])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--extract-docx", type=Path)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.extract_docx:
        RAW.write_text(json.dumps(extract_docx(args.extract_docx), indent=2, ensure_ascii=False) + "\n")
    payload = build()
    header = render_header(payload)
    protected_header = render_protected_header()
    manifest = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"
    if args.check:
        if not MANIFEST.exists() or MANIFEST.read_text() != manifest:
            raise SystemExit("multi-set manifest is stale")
        if not HEADER.exists() or HEADER.read_text() != header:
            raise SystemExit("multi-set runtime header is stale")
        if not PROTECTED_HEADER.exists() or PROTECTED_HEADER.read_text() != protected_header:
            raise SystemExit("protected progression-item header is stale")
        print(
            f"PASS: {payload['set_count']} total presets; "
            f"{payload['species_with_choices']} species/forms have native choices; "
            f"{payload['alternative_count']} validated handbook alternatives"
        )
    else:
        MANIFEST.write_text(manifest)
        HEADER.write_text(header)
        PROTECTED_HEADER.write_text(protected_header)
        print(MANIFEST.relative_to(ROOT))
        print(HEADER.relative_to(ROOT))
        print(PROTECTED_HEADER.relative_to(ROOT))


if __name__ == "__main__":
    main()
