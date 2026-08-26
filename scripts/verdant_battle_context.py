#!/usr/bin/env python3
"""Build the advisory rolling context for Verdant's next bespoke battle.

This tool deliberately does not grade battles.  It validates the durable v2
ledger, reads exact party facts from source, and reports reason-coded patterns
from the preceding encounters so a designer can make a fresh local decision.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
SEQUENCE_PATH = ROOT / "docs/verdant_battle_sequence.json"
DESIGNS_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER_PATH = ROOT / "docs/verdant_battle_experience_ledger.json"
RESERVATIONS_PATH = ROOT / "docs/verdant_historic_team_reservations.json"
FORMATS_PATH = ROOT / "docs/verdant_doubles_manifest.json"
REPORT_JSON_PATH = ROOT / "docs/verdant_battle_context.json"
REPORT_MD_PATH = ROOT / "docs/verdant_battle_context.md"
COMPETITIVE_INDEX_META_PATH = ROOT / "docs/competitive_team_index.meta.json"

DIFFICULTY_FLOOR = 7.5

REQUIRED_LEDGER_FIELDS: dict[str, type | tuple[type, ...]] = {
    "identity": (str, dict),
    "primary_player_question": str,
    "tempo": str,
    "pressure_sources": list,
    "intentional_opening": str,
    "intentional_weakness": str,
    "first_loss_lesson": str,
    "revealed_information": (str, list),
    "counterplay_classes": list,
    "target_difficulty": (int, float),
    "difficulty_rationale": str,
    "tuning_knob": str,
    "playtest_status": str,
    "novelty_tags": list,
    "historic_reference_ids": list,
    "corpus_search": (str, list, dict),
}

PREMIUM_ITEMS = {
    "ITEM_ASSAULT_VEST",
    "ITEM_CHOICE_BAND",
    "ITEM_CHOICE_SCARF",
    "ITEM_CHOICE_SPECS",
    "ITEM_EVIOLITE",
    "ITEM_EXPERT_BELT",
    "ITEM_FOCUS_SASH",
    "ITEM_LIFE_ORB",
    "ITEM_LEFTOVERS",
    "ITEM_MENTAL_HERB",
    "ITEM_ROCKY_HELMET",
    "ITEM_SITRUS_BERRY",
    "ITEM_WEAKNESS_POLICY",
}

COMMON_MOVE_EXCLUSIONS = quality.PROTECT_MOVES | {
    "MOVE_FAKE_OUT",
    "MOVE_HELPING_HAND",
    "MOVE_KNOCK_OFF",
    "MOVE_NONE",
    "MOVE_PROTECT",
    "MOVE_RETURN",
}

PROSE_TAGS: tuple[tuple[str, tuple[str, ...]], ...] = (
    ("perish-clock", ("perish",)),
    ("trick-room", ("trick room",)),
    ("wonder-room", ("wonder room",)),
    ("gravity", ("gravity",)),
    ("commander", ("commander", "dondozo", "tatsugiri")),
    ("dancer-relay", ("dancer", "recital")),
    ("instruct", ("instruct",)),
    ("anger-point", ("anger point",)),
    ("guard-split", ("guard split",)),
    ("illusion", ("illusion", "false mew")),
    ("imposter", ("imposter", "ditto")),
    ("schooling", ("schooling",)),
    ("weather-snow", ("snow", "aurora veil", "slush rush")),
    ("weather-sand", ("sand stream", "sandstorm", "sand clock")),
    ("weather-rain", ("rain", "drizzle")),
    ("weather-sun", ("sun", "drought", "solar beam")),
    ("terrain", ("terrain",)),
    ("screens", ("reflect", "light screen", "aurora veil", "screens")),
    ("hazards", ("sticky web", "stealth rock", "spikes", "toxic spikes")),
    ("redirection", ("follow me", "rage powder", "redirection")),
    ("setup", ("swords dance", "nasty plot", "shell smash", "belly drum", "setup")),
    ("speed-control", ("icy wind", "electroweb", "tailwind", "speed control", "quash")),
    ("choice-lock", ("choice band", "choice scarf", "choice specs", "choice lock")),
    ("pivoting", ("u-turn", "volt switch", "pivot")),
    ("residual-control", ("toxic", "infestation", "yawn", "residual")),
)


def load_json(path: Path) -> Any:
    return json.loads(path.read_text())


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def text_list(value: Any) -> list[str]:
    if not isinstance(value, list):
        return []
    return [item.strip() for item in value if isinstance(item, str) and item.strip()]


def first_sentence(value: Any) -> str:
    if not isinstance(value, str) or not value.strip():
        return ""
    text = " ".join(value.split())
    match = re.match(r"(.+?[.!?])(?:\s|$)", text)
    return match.group(1) if match else text


def slug_label(token: str) -> str:
    return token.removeprefix("SPECIES_").removeprefix("ITEM_").removeprefix("MOVE_").replace("_", " ").title()


def prose_contains(prose: str, needle: str) -> bool:
    if re.fullmatch(r"[a-z0-9]+", needle):
        return bool(re.search(rf"\b{re.escape(needle)}\b", prose))
    return needle in prose


def canonical_source_tag(value: str) -> list[str]:
    tag = value.lower()
    aliases = {
        "active speed control": ["speed-control"],
        "dancer physical relay": ["dancer-relay"],
        "dancer recital": ["dancer-relay"],
        "dancer special relay": ["dancer-relay"],
        "fake out tempo": ["fake-out"],
        "frost breath + anger point": ["anger-point"],
        "guard split transfer": ["guard-split"],
        "instruct repetition": ["instruct"],
        "mixed-speed trick room": ["trick-room"],
        "perish trap": ["perish-clock"],
        "redirection setup": ["redirection", "setup"],
        "residual control": ["residual-control"],
        "screens setup": ["screens", "setup"],
        "trick room": ["trick-room"],
    }
    if tag in aliases:
        return aliases[tag]
    return [re.sub(r"[^a-z0-9]+", "-", tag).strip("-")]


def normalize_ledger(payload: Any) -> dict[str, dict]:
    """Accept a list, an encounter mapping, or a conventional entries wrapper."""
    rows: Any = payload
    if isinstance(payload, dict):
        for key in ("entries", "battles", "ledger"):
            if key in payload:
                rows = payload[key]
                break

    result: dict[str, dict] = {}
    if isinstance(rows, dict):
        for encounter_id, value in rows.items():
            if not isinstance(value, dict):
                continue
            row = dict(value)
            row.setdefault("encounter_id", encounter_id)
            result[str(row["encounter_id"])] = row
        return result

    if isinstance(rows, list):
        for value in rows:
            if not isinstance(value, dict):
                continue
            encounter_id = value.get("encounter_id") or value.get("id")
            if isinstance(encounter_id, str) and encounter_id:
                result[encounter_id] = dict(value)
    return result


def infer_tags(design: dict, source: dict) -> list[str]:
    # The team intent and AI paragraph describe what the encounter actually
    # does. Counterplay/uniqueness prose often names mechanics specifically to
    # contrast them, so scanning those fields creates false repetition.
    prose = " ".join(str(design.get(key, "")) for key in ("team_intent", "bespoke_ai")).lower()
    tags = {
        tag
        for source_tag in source.get("synergy_tags", [])
        for tag in canonical_source_tag(source_tag)
    }
    for tag, needles in PROSE_TAGS:
        if any(prose_contains(prose, needle) for needle in needles):
            tags.add(tag)
    return sorted(tags)


def infer_question(tags: list[str], design: dict) -> str:
    questions = (
        ("perish-clock", "Can the player break, reverse, or out-position the Perish clock?"),
        ("commander", "Can the player disrupt the Commander chain before its boosted closer takes over?"),
        ("trick-room", "Can the player prevent, reverse, or survive the altered turn order?"),
        ("wonder-room", "Can the player adapt attacks and targets while defensive stats are exchanged?"),
        ("gravity", "Can the player exploit or withstand the accuracy and Ground-pressure field?"),
        ("dancer-relay", "Can the player interrupt the shared dance before both active slots snowball?"),
        ("instruct", "Can the player deny a high-value move from being repeated by Instruct?"),
        ("anger-point", "Can the player prevent the ally-activation turn from creating a physical sweep?"),
        ("guard-split", "Can the player identify and punish the Guard Split recipient?"),
        ("illusion", "Can the player identify the concealed threat without losing the opening exchange?"),
        ("weather-snow", "Can the player deny or play through the snow-enabled defensive and speed advantage?"),
        ("weather-sand", "Can the player manage the sand clock and its enabled attackers?"),
        ("terrain", "Can the player contest the field state before its recipients convert it into tempo?"),
        ("hazards", "Can the player deny, remove, or race the entry-hazard plan?"),
        ("redirection", "Can the player route pressure around support that redirects key attacks?"),
        ("setup", "Can the player stop the protected setup threat without surrendering board position?"),
        ("speed-control", "Can the player regain move-order control before the opponent converts it?"),
    )
    for tag, question in questions:
        if tag in tags:
            return question
    intent = first_sentence(design.get("team_intent"))
    return f"Can the player solve this encounter's defining opening: {intent}" if intent else "What adaptation does this encounter demand?"


def infer_tempo(tags: list[str], source: dict) -> str:
    if "trick-room" in tags or any("Trick Room" in tag for tag in tags):
        return "slow-board-control"
    if "perish-clock" in tags or "residual-control" in tags:
        return "attrition-clock"
    if any(tag.startswith("weather-") or tag.endswith(" engine") for tag in tags):
        return "field-enabled-offense"
    if "setup" in tags or any("setup" in tag.lower() for tag in tags):
        return "setup-offense"
    if "speed-control" in tags or "Fake Out tempo" in tags or "pivoting" in tags:
        return "active-tempo-control"
    if source.get("format") == "single":
        return "singles-pressure"
    return "mixed-board-pressure"


def fallback_ledger_row(encounter: dict, design: dict, source: dict) -> dict:
    tags = infer_tags(design, source)
    counterplay = design.get("intended_counterplay", "")
    references = design.get("competitive_references", [])
    reference_ids = [
        str(item.get("reference_id"))
        for item in references
        if isinstance(item, dict) and item.get("reference_id")
    ]
    return {
        "encounter_id": encounter["encounter_id"],
        "index": encounter["index"],
        "identity": first_sentence(design.get("uniqueness")) or first_sentence(design.get("team_intent")),
        "primary_player_question": infer_question(tags, design),
        "tempo": infer_tempo(tags, source),
        "pressure_sources": tags or source.get("synergy_tags", []) or ["coverage and level pressure"],
        "intentional_opening": first_sentence(design.get("team_intent")),
        "intentional_weakness": first_sentence(counterplay),
        "first_loss_lesson": first_sentence(counterplay),
        "revealed_information": first_sentence(design.get("closure")),
        "counterplay_classes": [counterplay] if counterplay else [],
        "target_difficulty": design.get("manual_difficulty"),
        "difficulty_rationale": first_sentence(design.get("closure")),
        "tuning_knob": "Opponent levels relative to the strict cap, after preserving the strategy.",
        "playtest_status": "not-recorded",
        "novelty_tags": tags,
        "historic_reference_ids": reference_ids,
        "corpus_search": design.get("corpus_review", {}),
        "record_source": "legacy-design-inference",
    }


def physical_format(encounter: dict, source_formats: set[str]) -> str:
    explicit = encounter.get("battle_format") or encounter.get("format")
    if isinstance(explicit, str) and explicit:
        return explicit
    category = str(encounter.get("category", "")).lower()
    if "native pair" in category:
        return "native-pair double"
    if "double" in category:
        return "double"
    if "single" in category:
        return "single"
    if len(source_formats) == 1:
        return next(iter(source_formats))
    return "mixed" if source_formats else "unknown"


def parse_source_facts(entries: list[dict]) -> tuple[dict[str, dict], list[dict]]:
    """Read only parties used by the indexed encounters; ignore the old quality score."""
    hard_errors: list[dict] = []
    formats = load_json(FORMATS_PATH).get("formats", {})
    trainers_text = quality.TRAINERS_PATH.read_text()
    parties_text = quality.PARTIES_PATH.read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    move_data = quality.parse_moves()
    species_data = quality.parse_species()
    facts: dict[str, dict] = {}

    for encounter in entries:
        variants: dict[tuple, dict] = {}
        formats_seen: set[str] = set()
        for trainer_id in encounter.get("trainer_ids", []):
            block_match = blocks.get(trainer_id)
            if block_match is None:
                hard_errors.append({
                    "code": "SOURCE_TRAINER_MISSING",
                    "encounter_id": encounter["encounter_id"],
                    "message": f"Indexed trainer {trainer_id} is absent from src/data/trainers.h.",
                })
                continue
            block = block_match.group(0)
            party_name = doubles.party_name(block)
            party_match = doubles.party_match(parties_text, party_name)
            if party_match is None:
                hard_errors.append({
                    "code": "SOURCE_PARTY_MISSING",
                    "encounter_id": encounter["encounter_id"],
                    "message": f"Trainer {trainer_id} points to missing party {party_name}.",
                })
                continue
            mons = [
                quality.parse_mon(raw, move_data, species_data)
                for raw in custom.party_entries(party_match.group(2))
            ]
            signature = tuple(
                (mon["species"], mon["item"], mon["ability"], tuple(mon["moves"]), mon["level_offset"])
                for mon in mons
            )
            if signature not in variants:
                variants[signature] = {
                    "party_name": party_name,
                    "trainer_ids": [],
                    "party_size": len(mons),
                    "species": [mon["species"] for mon in mons],
                    "items": [mon["item"] for mon in mons],
                    "moves": sorted({move for mon in mons for move in mon["moves"] if move != "MOVE_NONE"}),
                    "synergy_tags": quality.synergy_tags(mons, move_data),
                    "protect_slots": sum(bool(set(mon["moves"]) & quality.PROTECT_MOVES) for mon in mons),
                    "fake_out_slots": sum("MOVE_FAKE_OUT" in mon["moves"] for mon in mons),
                    "setup_slots": sum(bool(set(mon["moves"]) & quality.SETUP_MOVES) for mon in mons),
                    "speed_control_slots": sum(
                        bool(set(mon["moves"]) & quality.SPEED_MOVES)
                        or any(move_data.get(move, {}).get("priority", 0) > 0 for move in mon["moves"])
                        for mon in mons
                    ),
                    "premium_item_slots": sum(mon["item"] in PREMIUM_ITEMS for mon in mons),
                    "level_offsets": [mon["level_offset"] for mon in mons],
                }
            variants[signature]["trainer_ids"].append(trainer_id)
            if trainer_id in formats:
                formats_seen.add(formats[trainer_id].get("format", "unknown"))

        unique_variants = list(variants.values())
        divisor = max(1, len(unique_variants))
        party_slots = sum(row["party_size"] for row in unique_variants)
        level_offsets = [offset for row in unique_variants for offset in row["level_offsets"]]
        facts[encounter["encounter_id"]] = {
            "format": physical_format(encounter, formats_seen),
            "source_record_formats": sorted(formats_seen),
            "source_variant_count": len(unique_variants),
            "representative_party_size": max((row["party_size"] for row in unique_variants), default=0),
            "average_party_size": round(party_slots / divisor, 2),
            "species": sorted({species for row in unique_variants for species in row["species"]}),
            "items": sorted({item for row in unique_variants for item in row["items"] if item != "ITEM_NONE"}),
            "moves": sorted({move for row in unique_variants for move in row["moves"]}),
            "synergy_tags": sorted({tag for row in unique_variants for tag in row["synergy_tags"]}),
            "level_offsets": sorted(set(level_offsets)),
            "average_level_offset": round(sum(level_offsets) / max(1, len(level_offsets)), 2),
            "max_level_offset": max(level_offsets, default=0),
            "densities": {
                "protect": round(sum(row["protect_slots"] for row in unique_variants) / max(1, party_slots), 3),
                "fake_out": round(sum(row["fake_out_slots"] for row in unique_variants) / max(1, party_slots), 3),
                "setup": round(sum(row["setup_slots"] for row in unique_variants) / max(1, party_slots), 3),
                "speed_control_or_priority": round(sum(row["speed_control_slots"] for row in unique_variants) / max(1, party_slots), 3),
                "premium_item": round(sum(row["premium_item_slots"] for row in unique_variants) / max(1, party_slots), 3),
            },
        }
    return facts, hard_errors


def validate_sequence(entries: list[dict]) -> list[dict]:
    errors: list[dict] = []
    expected = list(range(1, len(entries) + 1))
    actual = [entry.get("index") for entry in entries]
    if actual != expected:
        errors.append({
            "code": "SEQUENCE_NOT_CONTIGUOUS",
            "encounter_id": None,
            "message": "Canonical battle indices are not contiguous from one.",
        })
    encounter_ids = [entry.get("encounter_id") for entry in entries]
    if len(encounter_ids) != len(set(encounter_ids)):
        errors.append({
            "code": "SEQUENCE_DUPLICATE_ID",
            "encounter_id": None,
            "message": "Canonical sequence contains duplicate encounter IDs.",
        })
    next_rows = [entry for entry in entries if entry.get("status") == "next"]
    if len(next_rows) != 1:
        errors.append({
            "code": "SEQUENCE_NEXT_COUNT",
            "encounter_id": None,
            "message": f"Expected exactly one next encounter; found {len(next_rows)}.",
        })
    return errors


def validate_ledger_row(encounter_id: str, row: dict) -> list[dict]:
    errors: list[dict] = []
    for field, expected_type in REQUIRED_LEDGER_FIELDS.items():
        value = row.get(field)
        if value is None:
            errors.append({
                "code": "LEDGER_FIELD_MISSING",
                "encounter_id": encounter_id,
                "message": f"Ledger field {field} is required.",
            })
            continue
        if not isinstance(value, expected_type) or isinstance(value, bool):
            names = expected_type.__name__ if isinstance(expected_type, type) else "/".join(kind.__name__ for kind in expected_type)
            errors.append({
                "code": "LEDGER_FIELD_TYPE",
                "encounter_id": encounter_id,
                "message": f"Ledger field {field} must be {names}.",
            })
            continue
        if isinstance(value, str) and not value.strip():
            errors.append({
                "code": "LEDGER_FIELD_EMPTY",
                "encounter_id": encounter_id,
                "message": f"Ledger field {field} cannot be empty.",
            })
        if isinstance(value, list) and field not in {"historic_reference_ids"} and not value:
            errors.append({
                "code": "LEDGER_FIELD_EMPTY",
                "encounter_id": encounter_id,
                "message": f"Ledger field {field} cannot be an empty list.",
            })
        if isinstance(value, list) and field in {
            "pressure_sources", "counterplay_classes", "novelty_tags", "historic_reference_ids"
        } and len(text_list(value)) != len(value):
            errors.append({
                "code": "LEDGER_LIST_VALUE_TYPE",
                "encounter_id": encounter_id,
                "message": f"Every value in ledger field {field} must be a non-empty string.",
            })

    identity = row.get("identity")
    if isinstance(identity, dict):
        memory_hook = identity.get("memory_hook")
        if not isinstance(memory_hook, str) or not memory_hook.strip():
            errors.append({
                "code": "LEDGER_IDENTITY_MEMORY_HOOK",
                "encounter_id": encounter_id,
                "message": "Structured identity must include a non-empty memory_hook.",
            })

    difficulty = row.get("target_difficulty")
    if isinstance(difficulty, (int, float)) and not isinstance(difficulty, bool) and difficulty < DIFFICULTY_FLOOR:
        errors.append({
            "code": "DIFFICULTY_BELOW_FLOOR",
            "encounter_id": encounter_id,
            "message": f"Target difficulty {difficulty:g} is below Verdant's {DIFFICULTY_FLOOR:g} floor.",
        })
    assessed = row.get("current_assessed_difficulty")
    if assessed is not None:
        if not isinstance(assessed, (int, float)) or isinstance(assessed, bool):
            errors.append({
                "code": "ASSESSED_DIFFICULTY_TYPE",
                "encounter_id": encounter_id,
                "message": "current_assessed_difficulty must be numeric when recorded.",
            })
        elif assessed < DIFFICULTY_FLOOR:
            errors.append({
                "code": "ASSESSED_DIFFICULTY_BELOW_FLOOR",
                "encounter_id": encounter_id,
                "message": f"Current assessed difficulty {assessed:g} is below Verdant's {DIFFICULTY_FLOOR:g} floor.",
            })
    branch_difficulty = row.get("branch_difficulty")
    if branch_difficulty is not None:
        if not isinstance(branch_difficulty, dict):
            errors.append({
                "code": "BRANCH_DIFFICULTY_TYPE",
                "encounter_id": encounter_id,
                "message": "branch_difficulty must be an object when recorded.",
            })
        else:
            for branch, value in branch_difficulty.items():
                if not isinstance(value, (int, float)) or isinstance(value, bool):
                    errors.append({
                        "code": "BRANCH_DIFFICULTY_VALUE_TYPE",
                        "encounter_id": encounter_id,
                        "message": f"Branch {branch} difficulty must be numeric.",
                    })
                elif value < DIFFICULTY_FLOOR:
                    errors.append({
                        "code": "BRANCH_DIFFICULTY_BELOW_FLOOR",
                        "encounter_id": encounter_id,
                        "message": f"Branch {branch} difficulty {value:g} is below Verdant's {DIFFICULTY_FLOOR:g} floor.",
                    })
    counterplay = row.get("counterplay_classes")
    if isinstance(counterplay, list) and len(text_list(counterplay)) < 3:
        errors.append({
            "code": "COUNTERPLAY_TOO_NARROW",
            "encounter_id": encounter_id,
            "message": "At least three broad counterplay classes must be recorded.",
        })

    overrides = override_reasons(row)
    for code, reason in overrides.items():
        if not reason.strip():
            errors.append({
                "code": "OVERRIDE_REASON_EMPTY",
                "encounter_id": encounter_id,
                "message": f"Advisory override {code} must include a reason.",
            })
    return errors


def override_reasons(row: dict) -> dict[str, str]:
    value = row.get("override_reasons", row.get("advisory_overrides", {}))
    if isinstance(value, dict):
        return {str(code): str(reason) for code, reason in value.items()}
    if isinstance(value, list):
        result = {}
        for item in value:
            if isinstance(item, dict) and item.get("code"):
                result[str(item["code"])] = str(item.get("reason", ""))
        return result
    return {}


def warning(code: str, message: str, encounters: list[str], evidence: Any) -> dict:
    return {
        "code": code,
        "scope": "rolling-window",
        "encounter_ids": encounters,
        "message": message,
        "evidence": evidence,
    }


def encounter_counts(rows: list[dict], field: str) -> dict[str, list[str]]:
    result: dict[str, list[str]] = defaultdict(list)
    for row in rows:
        values = row.get(field, [])
        if isinstance(values, str):
            values = [values]
        if not isinstance(values, list):
            continue
        for value in set(item for item in values if isinstance(item, str) and item):
            result[value].append(row["encounter_id"])
    return dict(result)


def build_advisories(window_rows: list[dict], current: dict, reservations: list[dict], fallback_ids: list[str]) -> list[dict]:
    warnings: list[dict] = []
    if fallback_ids:
        warnings.append(warning(
            "LEDGER_FALLBACK",
            "Some rolling records were inferred from legacy design prose; author the v2 ledger before treating semantic comparisons as exact.",
            fallback_ids,
            {"inferred_count": len(fallback_ids)},
        ))
    if not window_rows:
        return warnings

    def scalar_groups(field: str) -> dict[str, list[str]]:
        grouped: dict[str, list[str]] = defaultdict(list)
        for row in window_rows:
            value = row.get(field)
            if isinstance(value, str) and value.strip():
                grouped[" ".join(value.lower().split())].append(row["encounter_id"])
        return grouped

    for value, encounters in scalar_groups("primary_player_question").items():
        if len(encounters) >= 2:
            warnings.append(warning(
                "PRIMARY_QUESTION_REPEAT",
                "The same primary player question appears more than once in the rolling window.",
                encounters,
                {"question": value, "count": len(encounters)},
            ))

    for tempo, encounters in scalar_groups("tempo").items():
        if len(encounters) >= 4:
            warnings.append(warning(
                "TEMPO_CLUSTER",
                "One tempo occupies at least four of the previous encounters; inspect whether the next battle should change pace.",
                encounters,
                {"tempo": tempo, "count": len(encounters)},
            ))

    formats = [row.get("source_facts", {}).get("format") for row in window_rows]
    if len(formats) >= 4 and len(set(formats[-4:])) == 1:
        warnings.append(warning(
            "FORMAT_STREAK",
            "The last four encounters share one battle format.",
            [row["encounter_id"] for row in window_rows[-4:]],
            {"format": formats[-1], "streak": 4},
        ))

    tags = encounter_counts(window_rows, "novelty_tags")
    repeated_tags = {tag: ids for tag, ids in tags.items() if len(ids) >= 3}
    if repeated_tags:
        warnings.append(warning(
            "NOVELTY_TAG_REPEAT",
            "One or more mechanics recur in at least three rolling encounters.",
            sorted({encounter for ids in repeated_tags.values() for encounter in ids}),
            {tag: len(ids) for tag, ids in sorted(repeated_tags.items())},
        ))

    setter_recipient = [
        row["encounter_id"]
        for row in window_rows
        if any(token in set(row.get("novelty_tags", [])) for token in ("commander", "anger-point", "dancer-relay", "guard-split", "instruct", "terrain"))
        or any(tag.endswith("-engine") for tag in row.get("novelty_tags", []))
        or {"redirection", "setup"} <= set(row.get("novelty_tags", []))
        or {"screens", "setup"} <= set(row.get("novelty_tags", []))
        or re.search(r"\b(?:lead|open|setter)s?\b.*\b(?:enable|support|recipient|ace|closer)\b", row.get("intentional_opening", "").lower())
    ]
    if len(setter_recipient) >= 4:
        warnings.append(warning(
            "SETTER_RECIPIENT_PATTERN",
            "At least four rolling encounters use a visible enabler-plus-recipient shape.",
            setter_recipient,
            {"count": len(setter_recipient)},
        ))

    all_slots = sum(row.get("source_facts", {}).get("average_party_size", 0) for row in window_rows)
    density_sums = Counter()
    for row in window_rows:
        source = row.get("source_facts", {})
        size = source.get("average_party_size", 0)
        for key, density in source.get("densities", {}).items():
            density_sums[key] += size * density
    densities = {key: round(value / max(1, all_slots), 3) for key, value in density_sums.items()}
    density_thresholds = {
        "protect": (0.45, "PROTECT_DENSITY"),
        "fake_out": (0.20, "FAKE_OUT_DENSITY"),
        "setup": (0.30, "SETUP_DENSITY"),
        "speed_control_or_priority": (0.55, "SPEED_CONTROL_DENSITY"),
        "premium_item": (0.80, "PREMIUM_ITEM_DENSITY"),
    }
    for key, (threshold, code) in density_thresholds.items():
        if len(window_rows) >= 4 and densities.get(key, 0) >= threshold:
            warnings.append(warning(
                code,
                f"Rolling {key.replace('_', ' ')} density is high enough to merit an editorial look, not an automatic change.",
                [row["encounter_id"] for row in window_rows],
                {"density": densities[key], "threshold": threshold},
            ))

    species_uses: dict[str, list[str]] = defaultdict(list)
    item_uses: dict[str, list[str]] = defaultdict(list)
    move_uses: dict[str, list[str]] = defaultdict(list)
    for row in window_rows:
        source = row.get("source_facts", {})
        for species in set(source.get("species", [])):
            species_uses[species].append(row["encounter_id"])
        for item in set(source.get("items", [])) & PREMIUM_ITEMS:
            item_uses[item].append(row["encounter_id"])
        for move in set(source.get("moves", [])) - COMMON_MOVE_EXCLUSIONS:
            move_uses[move].append(row["encounter_id"])

    repeated_species = {name: ids for name, ids in species_uses.items() if len(ids) >= 2}
    if repeated_species:
        warnings.append(warning(
            "SPECIES_REPEAT",
            "A species appears in multiple rolling encounters; confirm that each appearance earns a distinct role.",
            sorted({encounter for ids in repeated_species.values() for encounter in ids}),
            {slug_label(name): len(ids) for name, ids in sorted(repeated_species.items())},
        ))
    repeated_items = {name: ids for name, ids in item_uses.items() if len(ids) >= 4}
    if repeated_items:
        warnings.append(warning(
            "PREMIUM_ITEM_REPEAT",
            "A premium item appears across at least four rolling encounters.",
            sorted({encounter for ids in repeated_items.values() for encounter in ids}),
            {slug_label(name): len(ids) for name, ids in sorted(repeated_items.items())},
        ))
    repeated_moves = {name: ids for name, ids in move_uses.items() if len(ids) >= 4}
    if repeated_moves:
        warnings.append(warning(
            "SIGNATURE_MOVE_REPEAT",
            "A non-generic move appears across at least four rolling encounters.",
            sorted({encounter for ids in repeated_moves.values() for encounter in ids}),
            {slug_label(name): len(ids) for name, ids in sorted(repeated_moves.items())},
        ))

    used_refs = encounter_counts(window_rows, "historic_reference_ids")
    future_reserved: dict[str, str] = {}
    for reservation in reservations:
        if reservation.get("status") == "reserved":
            for ref in text_list(reservation.get("reference_ids", [])):
                future_reserved[ref] = str(reservation.get("encounter", "unknown"))
    collisions = {
        ref: {"used_by": ids, "reserved_for": future_reserved[ref]}
        for ref, ids in used_refs.items()
        if ref in future_reserved and future_reserved[ref] != current["encounter_id"]
    }
    if collisions:
        warnings.append(warning(
            "RESERVED_REFERENCE_COLLISION",
            "A historic reference reserved for a future marquee battle already appears in the rolling ledger.",
            sorted({encounter for value in collisions.values() for encounter in value["used_by"]}),
            collisions,
        ))

    current_reservation = next(
        (row for row in reservations if row.get("encounter") in {current["encounter_id"], *current.get("trainer_ids", [])}),
        None,
    )
    if current_reservation and current_reservation.get("showcase"):
        showcase_tokens = {
            word.lower() for word in re.findall(r"[A-Za-z0-9]+", str(current_reservation["showcase"]))
            if len(word) >= 4 and word.lower() not in {"mega", "major", "rare"}
        }
        seen = {
            species.lower(): ids
            for species, ids in species_uses.items()
            if any(token in species.lower() for token in showcase_tokens)
        }
        if seen:
            warnings.append(warning(
                "RESERVED_SHOWCASE_ECHO",
                "The current marquee showcase has already appeared in the rolling window.",
                sorted({encounter for ids in seen.values() for encounter in ids}),
                {slug_label(species): len(ids) for species, ids in seen.items()},
            ))
    return warnings


def parse_cli_overrides(values: list[str]) -> dict[str, str]:
    result = {}
    for value in values:
        code, separator, reason = value.partition("=")
        if not separator or not code.strip() or not reason.strip():
            raise ValueError(f"override must be CODE=reason, got {value!r}")
        result[code.strip()] = reason.strip()
    return result


def apply_overrides(warnings: list[dict], reasons: dict[str, str]) -> list[dict]:
    used: set[str] = set()
    for row in warnings:
        reason = reasons.get(row["code"])
        row["overridden"] = bool(reason)
        row["override_reason"] = reason
        if reason:
            used.add(row["code"])
    for code in sorted(set(reasons) - used):
        warnings.append({
            "code": "UNUSED_OVERRIDE",
            "scope": "current-encounter",
            "encounter_ids": [],
            "message": f"Override {code} does not match a current advisory.",
            "evidence": {"requested_code": code},
            "overridden": False,
            "override_reason": None,
        })
    return warnings


def select_encounter(entries: list[dict], selector: str | None) -> dict:
    if selector is None:
        candidates = [entry for entry in entries if entry.get("status") == "next"]
        if len(candidates) != 1:
            raise ValueError(f"cannot select --next: found {len(candidates)} next encounters")
        return candidates[0]
    if selector.isdigit():
        index = int(selector)
        for entry in entries:
            if entry.get("index") == index:
                return entry
    for entry in entries:
        if entry.get("encounter_id") == selector:
            return entry
    raise ValueError(f"unknown encounter selector: {selector}")


def compact_reservation(current: dict, reservations: list[dict]) -> dict | None:
    ids = {current["encounter_id"], *current.get("trainer_ids", [])}
    match = next((row for row in reservations if row.get("encounter") in ids), None)
    return dict(match) if match else None


def compact_blueprint(current: dict, blueprints: list[dict]) -> dict | None:
    current_ids = {current["encounter_id"], *current.get("trainer_ids", [])}
    match = next(
        (
            row for row in blueprints
            if current_ids & {row.get("anchor"), *row.get("trainer_ids", [])}
        ),
        None,
    )
    return dict(match) if match else None


def build_report(selector: str | None, window_size: int, cli_overrides: dict[str, str]) -> dict:
    sequence_payload = load_json(SEQUENCE_PATH)
    entries = sequence_payload["entries"]
    designs = load_json(DESIGNS_PATH).get("designs", {})
    reservation_payload = load_json(RESERVATIONS_PATH) if RESERVATIONS_PATH.exists() else {}
    reservations = reservation_payload.get("reservations", [])
    blueprint_payload = reservation_payload.get("marquee_blueprints", {})
    blueprints = blueprint_payload.get("entries", []) if isinstance(blueprint_payload, dict) else []
    ledger_present = LEDGER_PATH.exists()
    ledger_payload = load_json(LEDGER_PATH) if ledger_present else {}
    ledger = normalize_ledger(ledger_payload) if ledger_present else {}
    current = select_encounter(entries, selector)
    source_facts, source_errors = parse_source_facts(entries)
    hard_errors = validate_sequence(entries) + source_errors
    if ledger_present and COMPETITIVE_INDEX_META_PATH.exists():
        current_index = load_json(COMPETITIVE_INDEX_META_PATH)
        recorded_index = ledger_payload.get("competitive_index") if isinstance(ledger_payload, dict) else None
        expected = {key: current_index.get(key) for key in ("version", "record_count", "sha256")}
        actual = {key: (recorded_index or {}).get(key) for key in expected}
        if actual != expected:
            hard_errors.append({
                "code": "LEDGER_COMPETITIVE_INDEX_STALE",
                "encounter_id": None,
                "message": f"Ledger competitive-index identity is stale: expected {expected}, found {actual}.",
            })

    sequence_ids = {entry["encounter_id"] for entry in entries}
    for encounter_id in ledger:
        if encounter_id not in sequence_ids:
            hard_errors.append({
                "code": "LEDGER_UNKNOWN_ENCOUNTER",
                "encounter_id": encounter_id,
                "message": "Ledger row does not map to the canonical physical-encounter sequence.",
            })
        else:
            hard_errors.extend(validate_ledger_row(encounter_id, ledger[encounter_id]))

    closed = [entry for entry in entries if entry.get("status") == "closed"]
    for encounter in closed:
        if encounter["encounter_id"] not in designs:
            hard_errors.append({
                "code": "CLOSED_DESIGN_MISSING",
                "encounter_id": encounter["encounter_id"],
                "message": "Closed encounter has no bespoke design record.",
            })
    if ledger_present:
        for encounter in closed:
            if encounter["encounter_id"] not in ledger:
                hard_errors.append({
                    "code": "LEDGER_COVERAGE_GAP",
                    "encounter_id": encounter["encounter_id"],
                    "message": "Closed encounter is missing from the v2 experience ledger.",
                })
    else:
        for encounter in closed:
            design = designs.get(encounter["encounter_id"], {})
            difficulty = design.get("manual_difficulty")
            if not isinstance(difficulty, (int, float)) or isinstance(difficulty, bool):
                hard_errors.append({
                    "code": "DIFFICULTY_NOT_RECORDED",
                    "encounter_id": encounter["encounter_id"],
                    "message": "Closed legacy design has no numeric manual difficulty.",
                })
            elif difficulty < DIFFICULTY_FLOOR:
                hard_errors.append({
                    "code": "DIFFICULTY_BELOW_FLOOR",
                    "encounter_id": encounter["encounter_id"],
                    "message": f"Legacy target difficulty {difficulty:g} is below Verdant's {DIFFICULTY_FLOOR:g} floor.",
                })

    previous = [entry for entry in entries if entry["index"] < current["index"]][-window_size:]
    fallback_ids: list[str] = []
    rolling_rows: list[dict] = []
    for encounter in previous:
        encounter_id = encounter["encounter_id"]
        if encounter_id in ledger:
            row = dict(ledger[encounter_id])
            row.setdefault("record_source", "v2-ledger")
        else:
            row = fallback_ledger_row(encounter, designs.get(encounter_id, {}), source_facts.get(encounter_id, {}))
            fallback_ids.append(encounter_id)
        row["encounter_id"] = encounter_id
        row["index"] = encounter["index"]
        row["location"] = encounter.get("location")
        row["category"] = encounter.get("category")
        row["strict_cap"] = designs.get(encounter_id, {}).get("strict_cap")
        row["source_facts"] = source_facts.get(encounter_id, {})
        rolling_rows.append(row)

    active_overrides = {}
    current_ledger = ledger.get(current["encounter_id"], {})
    active_overrides.update(override_reasons(current_ledger))
    active_overrides.update(cli_overrides)
    advisories = build_advisories(rolling_rows, current, reservations, fallback_ids)
    advisories = apply_overrides(advisories, active_overrides)

    current_design = designs.get(current["encounter_id"], {})
    nearest_cap = next(
        (
            designs.get(entry["encounter_id"], {}).get("strict_cap")
            for entry in reversed(entries[: current["index"]])
            if designs.get(entry["encounter_id"], {}).get("strict_cap") is not None
        ),
        None,
    )
    strict_cap = current_design.get("strict_cap", current.get("strict_cap", nearest_cap))
    if current_design.get("strict_cap") is not None:
        strict_cap_source = "current-design"
    elif current.get("strict_cap") is not None:
        strict_cap_source = "canonical-sequence-stage"
    else:
        strict_cap_source = "nearest-prior-closed-design"
    current_summary = {
        "index": current["index"],
        "encounter_id": current["encounter_id"],
        "location": current.get("location"),
        "category": current.get("category"),
        "status": current.get("status"),
        "trainer_ids": current.get("trainer_ids", []),
        "access_note": current.get("access_note"),
        "strict_cap": strict_cap,
        "strict_cap_source": strict_cap_source,
        "campaign_point": current_design.get("campaign_point"),
        "soft_reservation": compact_reservation(current, reservations),
        "marquee_blueprint": compact_blueprint(current, blueprints),
        "source_facts": source_facts.get(current["encounter_id"], {}),
    }

    tag_counts = Counter(tag for row in rolling_rows for tag in row.get("novelty_tags", []))
    tempo_counts = Counter(row.get("tempo") for row in rolling_rows if row.get("tempo"))
    format_counts = Counter(row.get("source_facts", {}).get("format") for row in rolling_rows)
    return {
        "version": 1,
        "policy": {
            "difficulty_floor": DIFFICULTY_FLOOR,
            "default_window": 10,
            "advisories_are_scores": False,
            "advisories_are_automatic_bans": False,
            "tuning_preference": "Preserve sound strategy; tune opponent levels relative to the fixed cap first.",
        },
        "selection": current_summary,
        "ledger": {
            "path": display_path(LEDGER_PATH),
            "present": ledger_present,
            "records": len(ledger),
            "rolling_fallback_records": fallback_ids,
        },
        "rolling_window": {
            "requested_size": window_size,
            "actual_size": len(rolling_rows),
            "encounter_range": [rolling_rows[0]["index"], rolling_rows[-1]["index"]] if rolling_rows else [],
            "tempo_counts": dict(sorted(tempo_counts.items())),
            "format_counts": dict(sorted(format_counts.items())),
            "novelty_tag_counts": dict(sorted(tag_counts.items(), key=lambda item: (-item[1], item[0]))),
            "entries": rolling_rows,
        },
        "hard_errors": hard_errors,
        "advisory_warnings": advisories,
        "design_questions": [
            "What does this trainer and location naturally suggest?",
            "Which primary player question is fresh relative to the rolling window?",
            "What should a fair first loss teach the player to change?",
            "Which three or more broad counterplay families remain viable?",
            "What intentional weakness is real and not silently erased?",
            "Does a reserved boss mechanic or historic team need to remain untouched?",
            "Which complete competitive references are worth reading before authoring this roster?",
            "If the sound strategy is overtuned, which level relative to the fixed cap should change first?",
        ],
    }


def markdown(report: dict) -> str:
    selected = report["selection"]
    rolling = report["rolling_window"]
    ledger = report["ledger"]
    lines = [
        "# Verdant rolling battle context",
        "",
        f"## Next design: Battle {selected['index']} — `{selected['encounter_id']}`",
        "",
        f"- Location: {selected.get('location') or 'not recorded'}",
        f"- Category: {selected.get('category') or 'not recorded'}",
        f"- Strict cap: {selected.get('strict_cap') if selected.get('strict_cap') is not None else 'not yet recorded'} "
        f"({selected.get('strict_cap_source', 'unknown source')})",
        f"- Source format: {selected.get('source_facts', {}).get('format', 'unknown')}",
        f"- Rolling window: {rolling['actual_size']} encounter(s)"
        + (f" (Battles {rolling['encounter_range'][0]}–{rolling['encounter_range'][1]})" if rolling["encounter_range"] else ""),
        f"- Ledger: {'v2 ledger loaded' if ledger['present'] else 'legacy-design fallback; v2 ledger not present'}",
        "",
    ]
    reservation = selected.get("soft_reservation")
    if reservation:
        lines.extend([
            "## Soft marquee reservation",
            "",
            f"- Status: {reservation.get('status', 'unknown')}",
            f"- Identity: {reservation.get('identity') or 'unassigned'}",
            f"- Showcase: {reservation.get('showcase') or 'unassigned'}",
            f"- References: {', '.join(f'`{value}`' for value in reservation.get('reference_ids', [])) or 'none'}",
            "",
        ])
    blueprint = selected.get("marquee_blueprint")
    if blueprint:
        lines.extend([
            "## Marquee blueprint",
            "",
            f"- Commitment: {blueprint.get('design_commitment', 'soft')}",
            f"- Target difficulty: {blueprint.get('target_difficulty', 'unassigned')}",
            f"- Protected identity: {blueprint.get('protected_identity') or 'unassigned'}",
            f"- Signature reveal: {blueprint.get('signature_reveal') or 'unassigned'}",
            f"- Candidate references: {', '.join(f'`{value}`' for value in blueprint.get('candidate_reference_ids', [])) or 'none'}",
            "",
        ])

    lines.extend(["## Hard errors", ""])
    if report["hard_errors"]:
        for error in report["hard_errors"]:
            where = f" `{error['encounter_id']}`" if error.get("encounter_id") else ""
            lines.append(f"- **{error['code']}**{where}: {error['message']}")
    else:
        lines.append("- None.")

    lines.extend(["", "## Advisory warnings", ""])
    if report["advisory_warnings"]:
        for item in report["advisory_warnings"]:
            suffix = f" Override: {item['override_reason']}" if item.get("overridden") else ""
            lines.append(f"- **{item['code']}**: {item['message']}{suffix}")
            evidence = json.dumps(item.get("evidence"), sort_keys=True)
            lines.append(f"  Evidence: `{evidence}`")
    else:
        lines.append("- None. Similarity remains an editorial judgment even when no threshold fires.")

    lines.extend([
        "",
        "## Rolling experience ledger",
        "",
        "| # | Encounter | Format | Difficulty | Tempo | Primary question | Novelty tags |",
        "| ---: | --- | --- | ---: | --- | --- | --- |",
    ])
    for row in rolling["entries"]:
        difficulty = row.get("target_difficulty")
        difficulty_text = f"{difficulty:g}" if isinstance(difficulty, (int, float)) else "?"
        question = str(row.get("primary_player_question", "")).replace("|", "/")
        tags = ", ".join(row.get("novelty_tags", [])) or "none"
        lines.append(
            f"| {row['index']} | `{row['encounter_id']}` | {row.get('source_facts', {}).get('format', 'unknown')} | "
            f"{difficulty_text} | {row.get('tempo', 'unknown')} | {question} | {tags} |"
        )

    lines.extend(["", "## Design questions", ""])
    lines.extend(f"- {question}" for question in report["design_questions"])
    lines.extend([
        "",
        "Warnings are prompts, not scores, quotas, bans, or automatic rewrite instructions.",
        "",
    ])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--next", action="store_true", help="select the unique sequence entry marked next (default)")
    selection.add_argument("--encounter", help="select an encounter ID or numeric battle index")
    parser.add_argument("--window", type=int, default=10, help="number of preceding physical encounters to inspect")
    parser.add_argument("--override", action="append", default=[], metavar="CODE=REASON", help="document a reason for accepting one advisory")
    parser.add_argument("--write", action="store_true", help="write the canonical JSON and Markdown reports")
    parser.add_argument("--check", action="store_true", help="fail if any binary invariant is broken")
    args = parser.parse_args()

    if args.window < 1:
        parser.error("--window must be at least one")
    try:
        cli_overrides = parse_cli_overrides(args.override)
        report = build_report(args.encounter, args.window, cli_overrides)
    except (ValueError, KeyError, json.JSONDecodeError) as error:
        raise SystemExit(f"FAIL: {error}") from error

    if args.write:
        REPORT_JSON_PATH.write_text(json.dumps(report, indent=2) + "\n")
        REPORT_MD_PATH.write_text(markdown(report))

    selected = report["selection"]
    print(
        f"Battle {selected['index']} {selected['encounter_id']}: "
        f"{report['rolling_window']['actual_size']} prior encounters, "
        f"{len(report['hard_errors'])} hard error(s), "
        f"{len(report['advisory_warnings'])} advisory warning(s)"
    )
    for error in report["hard_errors"]:
        where = f" {error['encounter_id']}" if error.get("encounter_id") else ""
        print(f"HARD {error['code']}{where}: {error['message']}")
    for item in report["advisory_warnings"]:
        status = "OVERRIDDEN" if item.get("overridden") else "ADVISORY"
        print(f"{status} {item['code']}: {item['message']}")

    if args.check:
        if report["hard_errors"]:
            raise SystemExit(f"FAIL: {len(report['hard_errors'])} hard battle-context error(s)")
        print("PASS: battle-context schema and binary invariants are valid")


if __name__ == "__main__":
    main()
