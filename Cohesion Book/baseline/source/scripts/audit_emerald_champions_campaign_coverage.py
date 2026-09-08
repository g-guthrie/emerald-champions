#!/usr/bin/env python3
"""Build and validate measurable Emerald Champions campaign coverage.

The source inventory answers what is compiled.  ``coverage_scope.json`` records
which compiled content belongs to the campaign test surface.  The playthrough
manifest records what a run declares it exercises.  These are deliberately
separate so a green subset cannot silently become a claim of full coverage.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from collections import Counter
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SCOPE = ROOT / "tests/campaign/coverage_scope.json"
DEFAULT_MANIFEST = ROOT / "tests/campaign/playthrough.json"

KINDS = (
    "maps",
    "warps",
    "scripts",
    "object_scripts",
    "coord_scripts",
    "bg_scripts",
    "map_scripts",
    "trainers",
    "flags",
    "vars",
    "items",
    "modules",
    "branches",
)
ANNOTATION_KEYS = {"labels", "tutorials"}
TRAINER_RE = re.compile(r"^\s*(trainerbattle(?:_[a-z0-9_]+)?)\s+(TRAINER_[A-Z0-9_]+)", re.M)
MAP_SCRIPT_RES = (
    re.compile(r"^\s*map_script\s+[^,\n]+,\s*([A-Za-z_][A-Za-z0-9_]*)", re.M),
    re.compile(r"^\s*map_script_2\s+[^,\n]+,\s*[^,\n]+,\s*([A-Za-z_][A-Za-z0-9_]*)", re.M),
)
SCRIPT_LABEL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)(?:::|:)\s*$", re.M)
TOKEN_RES = {
    "flags": re.compile(r"\bFLAG_[A-Z0-9_]+\b"),
    "vars": re.compile(r"\bVAR_[A-Z0-9_]+\b"),
    "items": re.compile(r"\bITEM_[A-Z0-9_]+\b"),
}


class AuditError(RuntimeError):
    pass


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text())
    except FileNotFoundError as exc:
        raise AuditError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise AuditError(f"invalid JSON in {path}: {exc}") from exc


def canonical_sha(value: Any) -> str:
    payload = json.dumps(value, sort_keys=True, separators=(",", ":")).encode()
    return hashlib.sha256(payload).hexdigest()


def map_constant(name: str) -> str:
    # map.json is authoritative when present; this is only an error fallback.
    return "MAP_" + re.sub(r"(?<!^)(?=[A-Z])", "_", name).upper()


def classify_map(name: str, map_id: str, group: str, scope: dict[str, Any]) -> str:
    if map_id in set(scope.get("main_story_maps", [])):
        return "main_story"
    if name in set(scope.get("generated_maps", [])) or any(
        name.startswith(prefix) for prefix in scope.get("generated_map_prefixes", [])
    ):
        return "generated"
    if "Unused" in name or group in set(scope.get("system_groups", [])) or name in set(scope.get("system_maps", [])) or any(
        name.startswith(prefix) for prefix in scope.get("system_map_prefixes", [])
    ):
        return "system"
    if any(group.endswith(suffix) for suffix in scope.get("excluded_group_suffixes", [])):
        return "system"
    if group in set(scope.get("optional_groups", [])):
        return "optional"
    return "campaign_unclassified"


def add_entity(
    entities: dict[str, list[dict[str, Any]]],
    kind: str,
    entity_id: str,
    classification: str,
    source: str,
    **facts: Any,
) -> None:
    entity = {
        "id": entity_id,
        "classification": classification,
        "source": source,
    }
    entity.update(facts)
    entities[kind].append(entity)


def stable_object_id(map_id: str, event: dict[str, Any]) -> str:
    script = event.get("script", "0x0")
    local = event.get("local_id")
    anchor = str(local) if local else f"{event.get('x','?')},{event.get('y','?')},{event.get('elevation','?')}"
    return f"object:{map_id}:{script}@{anchor}"


def build_inventory(root: Path, scope: dict[str, Any]) -> dict[str, Any]:
    groups_path = root / "data/maps/map_groups.json"
    groups = load_json(groups_path)
    entities: dict[str, list[dict[str, Any]]] = {kind: [] for kind in KINDS}
    map_seen: set[str] = set()

    for group in groups.get("group_order", []):
        if group not in groups:
            raise AuditError(f"map group {group} is ordered but undefined")
        for name in groups[group]:
            map_dir = root / "data/maps" / name
            map_path = map_dir / "map.json"
            if not map_path.is_file():
                raise AuditError(f"compiled map {group}/{name} has no map.json")
            data = load_json(map_path)
            map_id = data.get("id") or map_constant(name)
            if map_id in map_seen:
                raise AuditError(f"duplicate compiled map id: {map_id}")
            map_seen.add(map_id)
            classification = classify_map(name, map_id, group, scope)
            rel_map = str(map_path.relative_to(root))
            add_entity(
                entities, "maps", map_id, classification, rel_map,
                name=name, group=group, map_type=data.get("map_type"),
            )
            # Event JSON owns hidden-item, pickup, object-hide, and coordinate
            # state that may never be named again in the map's script source.
            serialized_map = json.dumps(data, sort_keys=True)
            for kind, token_re in TOKEN_RES.items():
                for token in sorted(set(token_re.findall(serialized_map))):
                    add_entity(
                        entities, kind, token, classification, rel_map,
                        map=map_id,
                    )

            for index, warp in enumerate(data.get("warp_events", [])):
                dest = warp.get("dest_map", "?")
                entity_id = (
                    f"warp:{map_id}:{warp.get('x','?')},{warp.get('y','?')},"
                    f"{warp.get('elevation','?')}->{dest}:{warp.get('dest_warp_id','?')}"
                )
                add_entity(entities, "warps", entity_id, classification, rel_map, map=map_id, index=index)

            event_groups = (
                ("object_scripts", "object_events"),
                ("coord_scripts", "coord_events"),
                ("bg_scripts", "bg_events"),
            )
            for kind, field in event_groups:
                for index, event in enumerate(data.get(field, [])):
                    script = event.get("script")
                    if not script or script in ("0", "0x0", "NULL"):
                        continue
                    if kind == "object_scripts":
                        entity_id = stable_object_id(map_id, event)
                    else:
                        entity_id = (
                            f"{kind[:-1]}:{map_id}:{script}@{event.get('x','?')},"
                            f"{event.get('y','?')},{event.get('elevation','?')}"
                        )
                    add_entity(
                        entities, kind, entity_id, classification, rel_map,
                        map=map_id, script=script, index=index,
                    )

            scripts_path = map_dir / "scripts.inc"
            if not scripts_path.is_file():
                continue
            script_text = scripts_path.read_text(errors="replace")
            rel_script = str(scripts_path.relative_to(root))
            for label in sorted(set(SCRIPT_LABEL_RE.findall(script_text))):
                add_entity(
                    entities, "scripts", label, classification, rel_script,
                    map=map_id,
                )
            map_script_labels = []
            for pattern in MAP_SCRIPT_RES:
                map_script_labels.extend(pattern.findall(script_text))
            for index, label in enumerate(map_script_labels):
                add_entity(
                    entities, "map_scripts", f"map_script:{map_id}:{label}:{index}",
                    classification, rel_script, map=map_id, script=label, index=index,
                )
            trainer_counts: Counter[tuple[str, str]] = Counter()
            for match in TRAINER_RE.finditer(script_text):
                macro, trainer = match.groups()
                occurrence = trainer_counts[(macro, trainer)]
                trainer_counts[(macro, trainer)] += 1
                add_entity(
                    entities, "trainers", f"trainer:{map_id}:{trainer}:{macro}:{occurrence}",
                    classification, rel_script, map=map_id, trainer=trainer,
                    macro=macro, occurrence=occurrence,
                )
            for kind, token_re in TOKEN_RES.items():
                for token in sorted(set(token_re.findall(script_text))):
                    add_entity(
                        entities, kind, token, classification, rel_script,
                        map=map_id,
                    )

    # Shared story scripts are compiled alongside map scripts but have no single
    # owning map.  Keep them explicit rather than attributing them arbitrarily.
    for scripts_path in sorted((root / "data/scripts").glob("*.inc")):
        script_text = scripts_path.read_text(errors="replace")
        rel_script = str(scripts_path.relative_to(root))
        classification = "campaign_unclassified"
        for label in sorted(set(SCRIPT_LABEL_RE.findall(script_text))):
            add_entity(entities, "scripts", label, classification, rel_script, map=None)
        trainer_counts: Counter[tuple[str, str]] = Counter()
        for match in TRAINER_RE.finditer(script_text):
            macro, trainer = match.groups()
            occurrence = trainer_counts[(macro, trainer)]
            trainer_counts[(macro, trainer)] += 1
            add_entity(
                entities, "trainers", f"trainer:GLOBAL:{trainer}:{macro}:{occurrence}",
                classification, rel_script, map=None, trainer=trainer,
                macro=macro, occurrence=occurrence,
            )
        for kind, token_re in TOKEN_RES.items():
            for token in sorted(set(token_re.findall(script_text))):
                add_entity(
                    entities, kind, token, classification, rel_script, map=None,
                )

    # Collapse global state tokens while retaining every source/map use.
    for kind in ("flags", "vars", "items"):
        merged: dict[str, dict[str, Any]] = {}
        for entity in entities[kind]:
            current = merged.setdefault(entity["id"], {
                "id": entity["id"],
                "classification": entity["classification"],
                "sources": [],
                "maps": [],
            })
            current["sources"].append(entity["source"])
            if entity.get("map"):
                current["maps"].append(entity["map"])
            # A token used by main story is main-story state even if also optional.
            order = {"main_story": 0, "campaign_unclassified": 1, "optional": 2, "generated": 3, "system": 4}
            if order[entity["classification"]] < order[current["classification"]]:
                current["classification"] = entity["classification"]
        for current in merged.values():
            current["sources"] = sorted(set(current["sources"]))
            current["maps"] = sorted(set(current["maps"]))
        entities[kind] = list(merged.values())

    for kind in ("modules", "branches"):
        for declared in scope.get(kind, []):
            evidence_sources = []
            for evidence in declared.get("evidence", []):
                evidence_path = root / evidence["path"]
                if not evidence_path.is_file():
                    raise AuditError(f"{kind} {declared['id']} evidence file is missing: {evidence['path']}")
                text = evidence_path.read_text(errors="replace")
                if evidence["contains"] not in text:
                    raise AuditError(
                        f"{kind} {declared['id']} lost source evidence: "
                        f"{evidence['contains']!r} not in {evidence['path']}"
                    )
                evidence_sources.append(evidence["path"])
            add_entity(
                entities, kind, declared["id"], declared["classification"],
                evidence_sources[0] if evidence_sources else str(DEFAULT_SCOPE.relative_to(root)),
                evidence=evidence_sources,
            )

    # A global script can be included from multiple compilation units.  Merge
    # labels by identity and retain all proven definitions.
    merged_scripts: dict[str, dict[str, Any]] = {}
    for entity in entities["scripts"]:
        current = merged_scripts.setdefault(entity["id"], {
            "id": entity["id"],
            "classification": "reference_only",
            "sources": [],
            "maps": [],
        })
        current["sources"].append(entity["source"])
        if entity.get("map"):
            current["maps"].append(entity["map"])
    for current in merged_scripts.values():
        current["sources"] = sorted(set(current["sources"]))
        current["maps"] = sorted(set(current["maps"]))
    entities["scripts"] = list(merged_scripts.values())

    for kind in KINDS:
        entities[kind].sort(key=lambda entity: entity["id"])
        ids = [entity["id"] for entity in entities[kind]]
        if len(ids) != len(set(ids)):
            duplicate = next(entity_id for entity_id, count in Counter(ids).items() if count > 1)
            raise AuditError(f"duplicate {kind} inventory id: {duplicate}")

    inventory = {
        "schema_version": 1,
        "source": "data/maps/map_groups.json plus each compiled map.json/scripts.inc",
        "scope_file": str(DEFAULT_SCOPE.relative_to(root)),
        "entities": entities,
    }
    inventory["inventory_sha256"] = canonical_sha(inventory)
    return inventory


def declared_coverage(manifest: dict[str, Any]) -> tuple[dict[str, set[str]], list[str], list[dict[str, Any]]]:
    covered = {kind: set() for kind in KINDS}
    errors: list[str] = []
    segment_ids: set[str] = set()
    segment_claims: list[dict[str, Any]] = []
    for segment in manifest.get("segments", []):
        segment_id = segment.get("id")
        if not segment_id or segment_id in segment_ids:
            errors.append(f"segment id is missing or duplicated: {segment_id!r}")
            continue
        segment_ids.add(segment_id)
        expected_map = segment.get("expected", {}).get("map")
        if expected_map:
            covered["maps"].add(expected_map)
        coverage = segment.get("coverage", {})
        if coverage and not isinstance(coverage, dict):
            errors.append(f"segment {segment_id} coverage must be an object")
            continue
        segment_maps = set(coverage.get("maps", []))
        if expected_map:
            segment_maps.add(expected_map)
        claim = {
            "id": segment_id,
            "maps": segment_maps,
            "scripts": set(),
            "trainers": set(),
            "battle_scenarios": set(),
            "event_aliases": {kind: set() for kind in ("object_scripts", "coord_scripts", "bg_scripts", "map_scripts")},
            "warp_aliases": set(),
        }
        for key, values in coverage.items():
            if key not in KINDS and key not in ("states", "battles", "interactive_flows") and key not in ANNOTATION_KEYS:
                errors.append(f"segment {segment_id} has unknown coverage kind: {key}")
                continue
            if not isinstance(values, list) or not all(isinstance(value, str) for value in values):
                errors.append(f"segment {segment_id} coverage.{key} must be a string array")
                continue
            if key in ANNOTATION_KEYS:
                # Human-readable review tags never count as source coverage.
                pass
            elif key == "states":
                for value in values:
                    if value.startswith("FLAG_"):
                        covered["flags"].add(value)
                    elif value.startswith("VAR_"):
                        covered["vars"].add(value)
                    else:
                        errors.append(f"segment {segment_id} has invalid state id: {value}")
            elif key == "battles":
                for value in values:
                    if value.startswith("TRAINER_"):
                        claim["trainers"].add(value)
                    elif value.startswith("BATTLE_"):
                        claim["battle_scenarios"].add(value)
                    else:
                        errors.append(f"segment {segment_id} has invalid battle id: {value}")
            elif key == "interactive_flows":
                # Native tutorial/puzzle/minigame coverage has its own source-backed
                # contract and validator. Accept the declaration here without
                # pretending it is a map/script inventory entity.
                pass
            elif key == "trainers":
                claim["trainers"].update(values)
            elif key in claim["event_aliases"]:
                claim["event_aliases"][key].update(values)
            elif key == "warps":
                claim["warp_aliases"].update(values)
            elif key == "branches":
                for value in values:
                    if value.startswith("BATTLE_"):
                        claim["battle_scenarios"].add(value)
                    else:
                        covered["branches"].add(value)
            else:
                covered[key].update(values)
                if key == "scripts":
                    claim["scripts"].update(values)
        segment_claims.append(claim)
    return covered, errors, segment_claims


def audit(inventory: dict[str, Any], scope: dict[str, Any], manifest: dict[str, Any]) -> dict[str, Any]:
    covered, errors, segment_claims = declared_coverage(manifest)
    required_classes = set(scope.get("required_classifications", []))
    inventory_by_kind = {
        kind: {entry["id"]: entry for entry in inventory["entities"][kind]}
        for kind in KINDS
    }

    # Semantic script labels cover their concrete event bindings.  This keeps
    # authoring readable while coverage remains per invocation in the report.
    for claim in segment_claims:
        for kind in ("object_scripts", "coord_scripts", "bg_scripts", "map_scripts"):
            for entry in inventory["entities"][kind]:
                if entry.get("script") in claim["scripts"] and entry.get("map") in claim["maps"]:
                    covered[kind].add(entry["id"])
        for trainer in claim["trainers"]:
            matches = [
                entry for entry in inventory["entities"]["trainers"]
                if entry.get("trainer") == trainer and entry.get("map") in claim["maps"]
            ]
            if not matches:
                errors.append(
                    f"segment {claim['id']} trainer {trainer} has no invocation on its declared maps"
                )
            for entry in matches:
                covered["trainers"].add(entry["id"])
        for kind, aliases in claim["event_aliases"].items():
            known_ids = {entry["id"] for entry in inventory["entities"][kind]}
            for alias in aliases:
                if alias in known_ids:
                    covered[kind].add(alias)
                    continue
                matches = [
                    entry for entry in inventory["entities"][kind]
                    if entry.get("script") == alias and entry.get("map") in claim["maps"]
                ]
                if not matches:
                    errors.append(
                        f"segment {claim['id']} {kind} alias {alias} has no binding on its declared maps"
                    )
                for entry in matches:
                    covered[kind].add(entry["id"])
        map_name_ids = {
            entry.get("name"): entry["id"] for entry in inventory["entities"]["maps"]
        }
        known_warps = {entry["id"] for entry in inventory["entities"]["warps"]}
        for alias in claim["warp_aliases"]:
            if alias in known_warps:
                covered["warps"].add(alias)
                continue
            match = re.fullmatch(r"([A-Za-z0-9_]+):(\d+)", alias)
            map_id = map_name_ids.get(match.group(1)) if match else None
            candidates = [
                entry for entry in inventory["entities"]["warps"]
                if map_id is not None
                and entry.get("map") == map_id
                and entry.get("index") == int(match.group(2))
            ]
            if len(candidates) != 1:
                errors.append(f"segment {claim['id']} warp alias {alias} does not resolve uniquely")
            for entry in candidates:
                covered["warps"].add(entry["id"])

    scenario_ids = [scenario for claim in segment_claims for scenario in claim["battle_scenarios"]]
    if len(scenario_ids) != len(set(scenario_ids)):
        errors.append("battle scenario ids must be globally unique")
    result: dict[str, Any] = {
        "schema_version": 1,
        "inventory_sha256": inventory["inventory_sha256"],
        "complete": False,
        "errors": errors,
        "declared_battle_scenarios": sorted(scenario_ids),
        "kinds": {},
    }
    for kind in KINDS:
        entries = inventory["entities"][kind]
        known = set(inventory_by_kind[kind])
        required = {entry["id"] for entry in entries if entry["classification"] in required_classes}
        unknown = sorted(covered[kind] - known)
        missing = sorted(required - covered[kind])
        result["kinds"][kind] = {
            "inventory": len(known),
            "required": len(required),
            "declared": len(covered[kind]),
            "covered": len(required & covered[kind]),
            "missing_count": len(missing),
            "unknown_count": len(unknown),
            "missing": missing,
            "unknown": unknown,
            "classification_counts": dict(sorted(Counter(entry["classification"] for entry in entries).items())),
        }
        if unknown:
            errors.append(f"{kind}: {len(unknown)} declared ids do not exist in current source")
        if missing:
            errors.append(f"{kind}: {len(missing)} required ids lack declared coverage")
    result["complete"] = not errors
    return result


def print_summary(result: dict[str, Any]) -> None:
    print(f"inventory sha256: {result['inventory_sha256']}")
    print("kind                 inventory  required  declared  covered  missing  unknown")
    for kind in KINDS:
        row = result["kinds"][kind]
        print(
            f"{kind:20} {row['inventory']:9d} {row['required']:9d} "
            f"{row['declared']:9d} {row['covered']:8d} {row['missing_count']:8d} {row['unknown_count']:8d}"
        )
    print("PASS: complete declared campaign coverage" if result["complete"] else "INCOMPLETE: campaign coverage gaps remain")
    for error in result["errors"][:20]:
        print(f"- {error}")
    if len(result["errors"]) > 20:
        print(f"- ... {len(result['errors']) - 20} more errors (see JSON report)")


def self_test() -> None:
    fake_inventory = {
        "inventory_sha256": "test",
        "entities": {kind: [] for kind in KINDS},
    }
    fake_inventory["entities"]["maps"] = [
        {"id": "MAP_A", "classification": "main_story"},
        {"id": "MAP_SYSTEM", "classification": "system"},
    ]
    fake_inventory["entities"]["branches"] = [
        {"id": "choice.a", "classification": "main_story"},
    ]
    fake_inventory["entities"]["scripts"] = [
        {"id": "MapA_EventScript_Npc", "classification": "reference_only"},
    ]
    fake_inventory["entities"]["object_scripts"] = [
        {
            "id": "object:MAP_A:MapA_EventScript_Npc@1,1,0",
            "classification": "main_story",
            "map": "MAP_A",
            "script": "MapA_EventScript_Npc",
        },
    ]
    fake_inventory["entities"]["trainers"] = [
        {
            "id": "trainer:MAP_A:TRAINER_A:trainerbattle_single:0",
            "classification": "main_story",
            "map": "MAP_A",
            "trainer": "TRAINER_A",
        },
    ]
    scope = {"required_classifications": ["main_story"]}
    complete = audit(fake_inventory, scope, {"segments": [{
        "id": "a",
        "expected": {"map": "MAP_A"},
        "coverage": {
            "branches": ["choice.a"],
            "scripts": ["MapA_EventScript_Npc"],
            "battles": ["BATTLE_A", "TRAINER_A"]
        },
    }]})
    assert complete["complete"]
    missing = audit(fake_inventory, scope, {"segments": [{"id": "a", "expected": {"map": "MAP_A"}}]})
    assert not missing["complete"] and missing["kinds"]["branches"]["missing"] == ["choice.a"]
    assert missing["kinds"]["object_scripts"]["missing_count"] == 1
    assert missing["kinds"]["trainers"]["missing_count"] == 1
    stale = audit(fake_inventory, scope, {"segments": [{
        "id": "a", "expected": {"map": "MAP_REMOVED"}, "coverage": {"branches": ["choice.a"]},
    }]})
    assert not stale["complete"] and stale["kinds"]["maps"]["unknown"] == ["MAP_REMOVED"]
    print("PASS: coverage auditor self-test")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--scope", type=Path, default=DEFAULT_SCOPE)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--inventory-out", type=Path)
    parser.add_argument("--report-out", type=Path)
    parser.add_argument("--inventory-only", action="store_true")
    parser.add_argument("--allow-incomplete", action="store_true")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return 0
    try:
        scope = load_json(args.scope)
        inventory = build_inventory(args.root.resolve(), scope)
        if args.inventory_out:
            args.inventory_out.parent.mkdir(parents=True, exist_ok=True)
            args.inventory_out.write_text(json.dumps(inventory, indent=2) + "\n")
        if args.inventory_only:
            counts = {kind: len(inventory["entities"][kind]) for kind in KINDS}
            print(json.dumps({"inventory_sha256": inventory["inventory_sha256"], "counts": counts}, indent=2))
            return 0
        result = audit(inventory, scope, load_json(args.manifest))
        if args.report_out:
            args.report_out.parent.mkdir(parents=True, exist_ok=True)
            args.report_out.write_text(json.dumps(result, indent=2) + "\n")
        print_summary(result)
        return 0 if result["complete"] or args.allow_incomplete else 1
    except AuditError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
