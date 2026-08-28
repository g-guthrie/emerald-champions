#!/usr/bin/env python3
"""Build a source-backed atlas of physical trainer-battle encounters.

The battle guide counts trainer definitions.  This atlas counts script
invocations and then groups only relationships proven by source: the existing
canonical sequence, one trainer id in one script source, a REMATCH() table row,
or explicit switch branches.  A common wrapper is reachability evidence, not
grouping evidence: it may execute several battles in sequence.  Anything else
stays split and is reported instead of being guessed into campaign order.
"""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict, deque
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
JSON_PATH = ROOT / "docs/verdant_physical_encounter_atlas.json"
MARKDOWN_PATH = ROOT / "docs/verdant_physical_encounter_atlas.md"
SEQUENCE_PATH = ROOT / "docs/verdant_battle_sequence.json"
GUIDE_PATH = ROOT / "docs/verdant_battle_guide.json"

LABEL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*):{1,2}(?:\s*@.*)?$")
BATTLE_RE = re.compile(
    r"^\s*(trainerbattle(?:_[a-z_]+)?|multi_2_vs_2)\s+(.+?)\s*$"
)
TOKEN_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
REMATCH_RE = re.compile(
    r"\[(REMATCH_[A-Z0-9_]+)\]\s*=\s*REMATCH\("
    r"(TRAINER_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+),\s*"
    r"(TRAINER_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+),\s*([A-Z0-9_]+)\)"
)


@dataclass(frozen=True)
class Label:
    name: str
    path: Path
    map_name: str
    line: int
    body: tuple[str, ...]


class DisjointSet:
    def __init__(self, values: list[str], canonical: dict[str, str]):
        self.parent = {value: value for value in values}
        self.canonical = {value: ({canonical[value]} if value in canonical else set()) for value in values}

    def find(self, value: str) -> str:
        parent = self.parent[value]
        if parent != value:
            self.parent[value] = self.find(parent)
        return self.parent[value]

    def union(
        self,
        left: str,
        right: str,
        allow_unassigned_into_canonical: bool = False,
    ) -> tuple[bool, tuple[str, ...]]:
        a, b = self.find(left), self.find(right)
        if a == b:
            return True, ()
        if (
            bool(self.canonical[a]) != bool(self.canonical[b])
            and not allow_unassigned_into_canonical
        ):
            combined = self.canonical[a] | self.canonical[b]
            return False, tuple(sorted(combined))
        combined = self.canonical[a] | self.canonical[b]
        if len(combined) > 1:
            return False, tuple(sorted(combined))
        if b < a:
            a, b = b, a
        self.parent[b] = a
        self.canonical[a] |= self.canonical[b]
        return True, ()


def relative(path: Path) -> str:
    return str(path.relative_to(ROOT))


def json_strings(value, pointer: str = ""):
    if isinstance(value, dict):
        for key in sorted(value):
            escaped = key.replace("~", "~0").replace("/", "~1")
            yield from json_strings(value[key], f"{pointer}/{escaped}")
    elif isinstance(value, list):
        for index, item in enumerate(value):
            yield from json_strings(item, f"{pointer}/{index}")
    elif isinstance(value, str):
        yield pointer or "/", value


def parse_labels() -> dict[str, Label]:
    labels: dict[str, Label] = {}
    paths = sorted((ROOT / "data/maps").glob("*/scripts.inc"))
    paths += sorted((ROOT / "data/scripts").glob("*.inc"))
    for path in paths:
        lines = path.read_text(errors="ignore").splitlines()
        starts: list[tuple[int, str]] = []
        for index, line in enumerate(lines):
            match = LABEL_RE.match(line.strip())
            if match:
                starts.append((index, match.group(1)))
        for position, (start, name) in enumerate(starts):
            end = starts[position + 1][0] if position + 1 < len(starts) else len(lines)
            if name in labels:
                raise ValueError(f"duplicate global map-script label: {name}")
            labels[name] = Label(
                name=name,
                path=path,
                map_name=(
                    path.parent.name
                    if path.parent.parent.name == "maps"
                    else f"Global_{path.stem}"
                ),
                line=start + 1,
                body=tuple(lines[start + 1 : end]),
            )
    return labels


def graph_for(labels: dict[str, Label]) -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    edges = {name: set() for name in labels}
    reverse = {name: set() for name in labels}
    for name, label in labels.items():
        # Script comments routinely name other labels as documentation.  They
        # are not control-flow edges (for example the Mossdeep Rich Boy comment
        # mentioning the separate three-grunt gauntlet).
        code = "\n".join(line.split("@", 1)[0] for line in label.body)
        for token in TOKEN_RE.findall(code):
            if token in labels and token != name:
                edges[name].add(token)
                reverse[token].add(name)
    return edges, reverse


def root_records(labels: dict[str, Label]) -> dict[str, list[dict]]:
    records: dict[str, list[dict]] = defaultdict(list)

    for name, label in labels.items():
        if name.endswith("_MapScripts"):
            records[name].append({
                "type": "map-script-table",
                "map": label.map_name,
                "file": relative(label.path),
                "line": label.line,
                "pointer": None,
            })

    for path in sorted((ROOT / "data/maps").glob("*/map.json")):
        data = json.loads(path.read_text())
        map_name = data.get("name", path.parent.name)
        for pointer, value in json_strings(data):
            if value not in labels:
                continue
            kind = "map-json-reference"
            details: dict = {}
            match = re.match(r"/(object_events|coord_events|bg_events)/(\d+)/script$", pointer)
            if match:
                collection, raw_index = match.groups()
                index = int(raw_index)
                kind = collection.removesuffix("s").replace("_", "-")
                event = data[collection][index]
                details = {
                    "eventIndex": index,
                    "x": event.get("x"),
                    "y": event.get("y"),
                }
                if collection == "object_events":
                    details.update({
                        "trainerType": event.get("trainer_type"),
                        "graphicsId": event.get("graphics_id"),
                        "movementType": event.get("movement_type"),
                    })
            records[value].append({
                "type": kind,
                "map": map_name,
                "file": relative(path),
                "line": None,
                "pointer": pointer,
                **details,
            })

    # A small number of trainer-setup scripts are entered from C rather than a
    # map event (notably the Mossdeep two-versus-two setup).  A declaration is
    # not evidence; a non-extern C use is.
    label_names = set(labels)
    for path in sorted((ROOT / "src").glob("**/*.c")):
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            if re.search(r"\bextern\b", line):
                continue
            for token in sorted(set(TOKEN_RE.findall(line)) & label_names):
                records[token].append({
                    "type": "c-runtime-reference",
                    "map": labels[token].map_name,
                    "file": relative(path),
                    "line": line_number,
                    "pointer": None,
                })

    for name in records:
        unique = {json.dumps(record, sort_keys=True): record for record in records[name]}
        records[name] = [unique[key] for key in sorted(unique)]
    return records


def reachable_labels(edges: dict[str, set[str]], roots: set[str]) -> set[str]:
    seen: set[str] = set()
    pending = list(sorted(roots, reverse=True))
    while pending:
        label = pending.pop()
        if label in seen:
            continue
        seen.add(label)
        pending.extend(sorted(edges[label] - seen, reverse=True))
    return seen


def ancestor_roots(
    label: str,
    reverse: dict[str, set[str]],
    roots: set[str],
) -> list[str]:
    found: set[str] = set()
    seen: set[str] = set()
    pending = [label]
    while pending:
        current = pending.pop()
        if current in seen:
            continue
        seen.add(current)
        if current in roots:
            found.add(current)
        pending.extend(sorted(reverse[current] - seen, reverse=True))
    return sorted(found)


def trainer_aliases() -> tuple[dict[str, str], set[str]]:
    text = (ROOT / "include/constants/opponents.h").read_text()
    numeric: dict[str, int] = {}
    trainer_ids: set[str] = set()
    for symbol, raw_value in re.findall(r"^#define\s+([A-Z0-9_]+)\s+(\d+)\s*$", text, re.M):
        numeric[symbol] = int(raw_value)
        if symbol.startswith("TRAINER_") and not symbol.startswith("TRAINER_BATTLE_"):
            trainer_ids.add(symbol)
    canonical_by_number: dict[int, str] = {}
    for symbol in sorted(trainer_ids):
        canonical_by_number.setdefault(numeric[symbol], symbol)
    aliases = {
        symbol: canonical_by_number.get(value, symbol)
        for symbol, value in numeric.items()
    }
    return aliases, set(numeric)


def invocation_rows(
    labels: dict[str, Label],
    edges: dict[str, set[str]],
    reverse: dict[str, set[str]],
    roots: dict[str, list[dict]],
) -> list[dict]:
    aliases, known_trainer_symbols = trainer_aliases()
    root_names = set(roots)
    reached = reachable_labels(edges, root_names)
    rows: list[dict] = []
    for label_name in sorted(labels, key=lambda key: (relative(labels[key].path), labels[key].line, key)):
        label = labels[label_name]
        for offset, raw_line in enumerate(label.body, 1):
            code = re.sub(r"\s+@.*$", "", raw_line).rstrip()
            match = BATTLE_RE.match(code)
            if not match:
                continue
            opcode, raw_args = match.groups()
            args = [part.strip() for part in raw_args.split(",")]
            opponents: list[str] = []
            allies: list[str] = []
            battle_type = None
            if opcode == "multi_2_vs_2":
                for index in (0, 2):
                    if index < len(args):
                        resolved = aliases.get(args[index], args[index])
                        if resolved in known_trainer_symbols or args[index] in known_trainer_symbols:
                            opponents.append(resolved)
                if len(args) > 4:
                    resolved = aliases.get(args[4], args[4])
                    if resolved in known_trainer_symbols or args[4] in known_trainer_symbols:
                        allies.append(resolved)
            elif opcode == "trainerbattle":
                battle_type = args[0] if args else None
                if len(args) > 1:
                    resolved = aliases.get(args[1], args[1])
                    if resolved in known_trainer_symbols or args[1] in known_trainer_symbols:
                        opponents.append(resolved)
            elif args:
                resolved = aliases.get(args[0], args[0])
                if resolved in known_trainer_symbols or args[0] in known_trainer_symbols:
                    opponents.append(resolved)

            line_number = label.line + offset
            invocation_id = f"INV_{label.map_name.upper()}_{line_number:04d}"
            root_labels = ancestor_roots(label_name, reverse, root_names) if label_name in reached else []
            trigger_records = [
                {"label": root_label, **record}
                for root_label in root_labels
                for record in roots[root_label]
            ]
            trigger_records.sort(key=lambda row: (
                row["type"], row["file"], row.get("pointer") or "", row.get("line") or 0, row["label"]
            ))
            rows.append({
                "invocationId": invocation_id,
                "reachability": "proven" if label_name in reached else "unresolved",
                "source": {
                    "map": label.map_name,
                    "file": relative(label.path),
                    "line": line_number,
                    "label": label_name,
                    "opcode": opcode,
                    "battleType": battle_type,
                    "arguments": args,
                },
                "opponentTrainerIds": sorted(set(opponents)),
                "allyTrainerIds": sorted(set(allies)),
                "triggerRoots": trigger_records,
            })
    ids = [row["invocationId"] for row in rows]
    if len(ids) != len(set(ids)):
        raise ValueError("invocation ids are not unique")
    return rows


def rematch_rows() -> list[dict]:
    text = (ROOT / "src/battle_setup.c").read_text()
    return [
        {
            "rematchId": match.group(1),
            "trainerIds": list(match.group(2, 3, 4, 5)),
            "mapConstant": match.group(6),
        }
        for match in REMATCH_RE.finditer(text)
    ]


def guide_indexes() -> tuple[dict[str, dict], dict[str, list[dict]], dict[str, int]]:
    guide = json.loads(GUIDE_PATH.read_text())
    by_id: dict[str, list[dict]] = defaultdict(list)
    for entry in guide["entries"]:
        by_id[entry["trainerId"]].append(entry)
    preferred: dict[str, dict] = {}
    for trainer_id, entries in by_id.items():
        preferred[trainer_id] = min(
            entries,
            key=lambda row: (
                row.get("category") == "rematch",
                row.get("chapterRank", 999),
                row.get("order", 9999),
            ),
        )
    chapter_rank = {
        entry["chapter"]: entry.get("chapterRank", 999)
        for entry in guide["entries"]
    }
    return preferred, by_id, chapter_rank


def validated_sequence() -> tuple[dict, dict[str, int]]:
    sequence = json.loads(SEQUENCE_PATH.read_text())
    entries = sequence.get("entries", [])
    if not entries:
        raise ValueError("canonical battle sequence is empty")
    indices = [entry.get("index") for entry in entries]
    minimum, maximum = min(indices), max(indices)
    if indices != list(range(minimum, maximum + 1)):
        raise ValueError(f"canonical battle sequence is not contiguous: {indices}")
    encounter_ids = [entry.get("encounter_id") for entry in entries]
    if len(encounter_ids) != len(set(encounter_ids)):
        raise ValueError("canonical battle sequence has duplicate encounter ids")
    return sequence, {"minimumIndex": minimum, "maximumIndex": maximum, "count": len(entries)}


def merge_groups(
    invocations: list[dict],
    labels: dict[str, Label],
    edges: dict[str, set[str]],
    sequence: dict,
) -> tuple[list[dict], list[dict]]:
    sequence_entries = sequence["entries"]
    sequence_by_trainer: dict[str, dict] = {}
    for entry in sequence_entries:
        for trainer_id in entry["trainer_ids"]:
            if trainer_id in sequence_by_trainer:
                raise ValueError(f"sequence trainer assigned twice: {trainer_id}")
            sequence_by_trainer[trainer_id] = entry

    reachable = [row for row in invocations if row["reachability"] == "proven"]
    by_id = {row["invocationId"]: row for row in reachable}
    canonical: dict[str, str] = {}
    for row in reachable:
        matches = {
            sequence_by_trainer[trainer_id]["encounter_id"]
            for trainer_id in row["opponentTrainerIds"]
            if trainer_id in sequence_by_trainer
        }
        if len(matches) > 1:
            raise ValueError(f"one source invocation crosses canonical encounters: {row['invocationId']}")
        if matches:
            canonical[row["invocationId"]] = next(iter(matches))

    dsu = DisjointSet(list(by_id), canonical)
    proof_edges: list[dict] = []
    conflicts: list[dict] = []

    def join(
        ids: list[str],
        proof_type: str,
        detail: str,
        allow_unassigned_into_canonical: bool = False,
    ) -> None:
        ids = sorted(set(ids))
        if len(ids) < 2:
            return
        partitions: dict[tuple[str, ...], list[str]] = defaultdict(list)
        if allow_unassigned_into_canonical:
            partitions[()].extend(ids)
        else:
            for invocation_id in ids:
                root = dsu.find(invocation_id)
                partitions[tuple(sorted(dsu.canonical[root]))].append(invocation_id)

        representatives = []
        for canonical_key, partition in sorted(partitions.items()):
            anchor = partition[0]
            joined = [anchor]
            for other in partition[1:]:
                ok, canonical_ids = dsu.union(
                    anchor,
                    other,
                    allow_unassigned_into_canonical=allow_unassigned_into_canonical,
                )
                if not ok:
                    raise ValueError(
                        f"compatible grouping partition unexpectedly conflicted: {canonical_ids}"
                    )
                joined.append(other)
            representatives.append((canonical_key, anchor))
            if len(joined) > 1:
                proof_edges.append({
                    "proofType": proof_type,
                    "detail": detail,
                    "invocationIds": joined,
                })

        # Preserve every canonical sequence encounter as its own unit.  One
        # unassigned partition is also kept separate, so a later rematch or
        # alternate mode cannot silently become part of an indexed first fight.
        if len(representatives) > 1:
            anchor_key, anchor = representatives[0]
            for other_key, other in representatives[1:]:
                conflicts.append({
                    "proofType": proof_type,
                    "detail": detail,
                    "invocationIds": [anchor, other],
                    "canonicalEncounterIds": sorted(set(anchor_key) | set(other_key)),
                    "containsUnassignedPartition": not anchor_key or not other_key,
                })

    # The current hand-audited sequence is authoritative and is never edited or
    # re-ordered by this atlas.
    for entry in sequence_entries:
        ids = [
            row["invocationId"]
            for row in reachable
            if set(row["opponentTrainerIds"]) & set(entry["trainer_ids"])
        ]
        join(ids, "authoritative-sequence", entry["encounter_id"])

    # One trainer id used by multiple commands in one script source proves an
    # initial/rematch, twin-avatar, or conditional-branch relationship.  The
    # source restriction prevents unrelated appearances of a reused id from
    # being silently collapsed.  Merely sharing a reachable wrapper does not
    # merge commands because wrappers can execute battles sequentially.
    trainer_source_to_ids: dict[tuple[str, str], list[str]] = defaultdict(list)
    for row in reachable:
        for trainer_id in row["opponentTrainerIds"]:
            trainer_source_to_ids[(row["source"]["file"], trainer_id)].append(row["invocationId"])
    for (file_name, trainer_id), ids in sorted(trainer_source_to_ids.items()):
        join(ids, "shared-trainer-id-in-source", f"{trainer_id} in {file_name}")

    rematches = rematch_rows()
    for rematch in rematches:
        trainer_ids = set(rematch["trainerIds"])
        ids = [
            row["invocationId"]
            for row in reachable
            if trainer_ids & set(row["opponentTrainerIds"])
        ]
        join(
            ids,
            "rematch-table",
            f"{rematch['rematchId']} on {rematch['mapConstant']}",
            allow_unassigned_into_canonical=True,
        )

    # Direct case destinations containing battle commands are mutually
    # exclusive script branches.  No transitive inference is made here.
    label_invocations: dict[str, list[str]] = defaultdict(list)
    for row in reachable:
        label_invocations[row["source"]["label"]].append(row["invocationId"])
    for router, label in sorted(labels.items()):
        targets = []
        for line in label.body:
            match = re.match(r"\s*case\s+[^,]+,\s*([A-Za-z_][A-Za-z0-9_]*)", line)
            if match and match.group(1) in label_invocations:
                targets.extend(label_invocations[match.group(1)])
        join(targets, "explicit-switch-branches", router)

    # Gender routers are another exact mutually exclusive branch pattern.  A
    # target can itself switch on starter choice, so collect descendant battle
    # commands.  Canonical-vs-unassigned partitioning still prevents a
    # postgame rematch from being folded into an indexed first encounter.
    for router, label in sorted(labels.items()):
        code_lines = [line.split("@", 1)[0] for line in label.body]
        code = "\n".join(code_lines)
        if (
            "checkplayergender" not in code
            or not re.search(r"compare\s+VAR_RESULT,\s*MALE", code)
            or not re.search(r"compare\s+VAR_RESULT,\s*FEMALE", code)
        ):
            continue
        targets = [
            match.group(1)
            for line in code_lines
            if (match := re.match(r"\s*goto_if_eq\s+([A-Za-z_][A-Za-z0-9_]*)", line))
            and match.group(1) in labels
        ]
        descendant_ids: list[str] = []
        for target in targets:
            seen: set[str] = set()
            pending = [target]
            while pending:
                current = pending.pop()
                if current in seen:
                    continue
                seen.add(current)
                descendant_ids.extend(label_invocations.get(current, []))
                pending.extend(sorted(edges[current] - seen, reverse=True))
        join(descendant_ids, "explicit-gender-branches", router)

    grouped: dict[str, list[dict]] = defaultdict(list)
    for invocation_id, row in by_id.items():
        grouped[dsu.find(invocation_id)].append(row)

    preferred_guide, all_guide, chapter_ranks = guide_indexes()
    rematch_by_trainer = {
        trainer_id: rematch
        for rematch in rematches
        for trainer_id in rematch["trainerIds"]
    }
    sequence_by_id = {entry["encounter_id"]: entry for entry in sequence_entries}
    result: list[dict] = []

    for rows in grouped.values():
        rows.sort(key=lambda row: (row["source"]["file"], row["source"]["line"]))
        invocation_ids = {row["invocationId"] for row in rows}
        sequence_ids = sorted({canonical[row["invocationId"]] for row in rows if row["invocationId"] in canonical})
        if len(sequence_ids) > 1:
            raise ValueError(f"group crossed authoritative sequence entries: {sequence_ids}")
        sequence_entry = sequence_by_id[sequence_ids[0]] if sequence_ids else None

        source_opponents = sorted({trainer_id for row in rows for trainer_id in row["opponentTrainerIds"]})
        allies = sorted({trainer_id for row in rows for trainer_id in row["allyTrainerIds"]})
        expanded_opponents = set(source_opponents)
        rematch_ids: set[str] = set()
        for trainer_id in source_opponents:
            if trainer_id in rematch_by_trainer:
                rematch = rematch_by_trainer[trainer_id]
                expanded_opponents.update(rematch["trainerIds"])
                rematch_ids.add(rematch["rematchId"])

        guide_candidates = [preferred_guide[trainer_id] for trainer_id in expanded_opponents if trainer_id in preferred_guide]
        guide_entry = None
        if sequence_entry:
            sequence_trainers = set(sequence_entry["trainer_ids"])
            seq_candidates = [candidate for candidate in guide_candidates if candidate["trainerId"] in sequence_trainers]
            if seq_candidates:
                guide_entry = min(seq_candidates, key=lambda row: row.get("order", 9999))
        if guide_entry is None and guide_candidates:
            guide_entry = min(
                guide_candidates,
                key=lambda row: (row.get("category") == "rematch", row.get("chapterRank", 999), row.get("order", 9999)),
            )

        chapters = sorted(
            {
                entry["chapter"]
                for trainer_id in expanded_opponents
                for entry in all_guide.get(trainer_id, [])
            },
            key=lambda chapter: (chapter_ranks.get(chapter, 999), chapter),
        )
        primary_chapter = guide_entry["chapter"] if guide_entry else "Unresolved chapter"
        chapter_rank = guide_entry.get("chapterRank", 999) if guide_entry else 999
        level_cap = guide_entry.get("levelCap") if guide_entry else None
        if sequence_entry and sequence_entry.get("strict_cap") is not None:
            level_cap = sequence_entry["strict_cap"]

        if sequence_entry:
            category = "canonical-sequence"
            campaign_category = sequence_entry.get("category")
        elif any(
            row["source"].get("battleType") in {"TRAINER_BATTLE_PYRAMID", "TRAINER_BATTLE_HILL"}
            for row in rows
        ):
            category = "battle-facility-dynamic"
            campaign_category = guide_entry.get("category") if guide_entry else None
        elif rematch_ids:
            category = "rematch-family"
            campaign_category = guide_entry.get("category") if guide_entry else None
        elif any(row["source"]["opcode"] == "multi_2_vs_2" for row in rows):
            category = "scripted-multi-battle"
            campaign_category = guide_entry.get("category") if guide_entry else None
        elif len(rows) > 1 or len(source_opponents) > 1:
            category = "branch-or-shared-definition"
            campaign_category = guide_entry.get("category") if guide_entry else None
        else:
            category = "single-scripted-trainer"
            campaign_category = guide_entry.get("category") if guide_entry else None

        earliest = rows[0]["source"]
        group_id = sequence_entry["encounter_id"] if sequence_entry else f"PHYSICAL_{earliest['map'].upper()}_{earliest['line']:04d}"
        evidence = []
        if sequence_entry:
            evidence.append({"proofType": "authoritative-sequence", "detail": sequence_entry["encounter_id"]})
        for edge in proof_edges:
            if set(edge["invocationIds"]) & invocation_ids:
                evidence.append({"proofType": edge["proofType"], "detail": edge["detail"]})
        if not evidence:
            evidence.append({"proofType": "single-source-invocation", "detail": rows[0]["invocationId"]})
        evidence = [
            json.loads(value)
            for value in sorted({json.dumps(item, sort_keys=True) for item in evidence})
        ]
        trigger_sites = [
            trigger
            for row in rows
            for trigger in row["triggerRoots"]
            if trigger["type"] in {"object-event", "coord-event", "bg-event", "c-runtime-reference"}
        ]
        trigger_sites = [
            json.loads(value)
            for value in sorted({json.dumps(item, sort_keys=True) for item in trigger_sites})
        ]

        result.append({
            "groupId": group_id,
            "sequenceIndex": sequence_entry["index"] if sequence_entry else None,
            "sequenceStatus": sequence_entry.get("status") if sequence_entry else None,
            "category": category,
            "campaignCategory": campaign_category,
            "primaryChapter": primary_chapter,
            "chapterRank": chapter_rank,
            "levelCap": level_cap,
            "allGuideChapters": chapters,
            "sourceOpponentTrainerIds": source_opponents,
            "resolvedOpponentTrainerIds": sorted(expanded_opponents),
            "allyTrainerIds": allies,
            "rematchIds": sorted(rematch_ids),
            "invocationIds": [row["invocationId"] for row in rows],
            "sources": [row["source"] for row in rows],
            "triggerSites": trigger_sites,
            "groupingEvidence": evidence,
        })

    result.sort(key=lambda row: (
        row["sequenceIndex"] is None,
        row["sequenceIndex"] if row["sequenceIndex"] is not None else 9999,
        row["chapterRank"],
        row["sources"][0]["file"],
        row["sources"][0]["line"],
        row["groupId"],
    ))
    for ordinal, group in enumerate(result, 1):
        group["atlasOrdinal"] = ordinal

    covered_sequence = {group["sequenceIndex"] for group in result if group["sequenceIndex"] is not None}
    promised_sequence = {entry["index"] for entry in sequence_entries}
    if covered_sequence != promised_sequence:
        raise ValueError(f"canonical sequence coverage drift: missing={sorted(promised_sequence - covered_sequence)}")
    result_by_sequence = {
        group["sequenceIndex"]: group
        for group in result
        if group["sequenceIndex"] is not None
    }
    for entry in sequence_entries:
        group = result_by_sequence[entry["index"]]
        observed_source = set(group["sourceOpponentTrainerIds"])
        observed_resolved = set(group["resolvedOpponentTrainerIds"])
        expected = set(entry["trainer_ids"])
        # A canonical encounter may name only the physical source record or
        # explicitly own its whole rematch family.  Both are source-proven:
        # the former comes from the map invocation and the latter from the
        # REMATCH() expansion already stored on this atlas group.
        if expected not in (observed_source, observed_resolved):
            raise ValueError(
                f"Battle {entry['index']} membership drift: "
                f"expected={sorted(expected)}, source={sorted(observed_source)}, "
                f"resolved={sorted(observed_resolved)}"
            )
    return result, sorted(conflicts, key=lambda row: (row["proofType"], row["detail"], row["invocationIds"]))


def build() -> dict:
    labels = parse_labels()
    edges, reverse = graph_for(labels)
    roots = root_records(labels)
    invocations = invocation_rows(labels, edges, reverse, roots)
    sequence, boundary = validated_sequence()
    groups, conflicts = merge_groups(invocations, labels, edges, sequence)
    reachable = [row for row in invocations if row["reachability"] == "proven"]
    unresolved = [row for row in invocations if row["reachability"] == "unresolved"]

    by_chapter: dict[str, dict[str, int]] = {}
    for chapter in sorted(
        {group["primaryChapter"] for group in groups},
        key=lambda value: (min(group["chapterRank"] for group in groups if group["primaryChapter"] == value), value),
    ):
        chapter_groups = [group for group in groups if group["primaryChapter"] == chapter]
        by_chapter[chapter] = {
            "physicalGroups": len(chapter_groups),
            "scriptInvocations": sum(len(group["invocationIds"]) for group in chapter_groups),
        }
    by_category = {
        category: {
            "physicalGroups": sum(group["category"] == category for group in groups),
            "scriptInvocations": sum(
                len(group["invocationIds"])
                for group in groups
                if group["category"] == category
            ),
        }
        for category in sorted({group["category"] for group in groups})
    }
    by_opcode = dict(sorted(Counter(row["source"]["opcode"] for row in reachable).items()))
    by_root_type = dict(sorted(Counter(
        root_type
        for row in reachable
        for root_type in {record["type"] for record in row["triggerRoots"]}
    ).items()))

    cross_map = []
    for row in reachable:
        trigger_maps = sorted({
            record["map"]
            for record in row["triggerRoots"]
            if record["type"] in {"object-event", "coord-event", "bg-event"}
            and record.get("map") != row["source"]["map"]
        })
        if trigger_maps:
            cross_map.append({
                "invocationId": row["invocationId"],
                "definitionMap": row["source"]["map"],
                "triggerMaps": trigger_maps,
                "source": row["source"],
            })

    facility_reasons = {
        "TRAINER_BATTLE_PYRAMID": "Battle Pyramid selects runtime facility identity; the fixed trainer symbol is a setup placeholder, not one stable overworld opponent.",
        "TRAINER_BATTLE_HILL": "Trainer Hill selects runtime facility identity; the fixed trainer symbol is a setup placeholder, not one stable overworld opponent.",
    }
    runtime_identity = [
        {
            "invocationId": row["invocationId"],
            "source": row["source"],
            "reason": facility_reasons[row["source"]["battleType"]],
        }
        for row in reachable
        if row["source"].get("battleType") in facility_reasons
    ]

    groups_by_direct_trainer: dict[str, set[str]] = defaultdict(set)
    for group in groups:
        for trainer_id in group["sourceOpponentTrainerIds"]:
            groups_by_direct_trainer[trainer_id].add(group["groupId"])
    reused_across_groups = []
    for trainer_id, group_ids in sorted(groups_by_direct_trainer.items()):
        if len(group_ids) < 2:
            continue
        rows = [group for group in groups if group["groupId"] in group_ids]
        reused_across_groups.append({
            "trainerId": trainer_id,
            "groupIds": sorted(group_ids),
            "maps": sorted({source["map"] for group in rows for source in group["sources"]}),
            "resolution": "left separate because neither one trainer id in one script source nor a REMATCH() row proves one encounter",
        })

    group_by_invocation = {
        invocation_id: group["groupId"]
        for group in groups
        for invocation_id in group["invocationIds"]
    }
    common_wrapper_groups: dict[tuple, dict] = {}
    for row in reachable:
        for trigger in row["triggerRoots"]:
            if trigger["type"] not in {"object-event", "coord-event", "bg-event"}:
                continue
            key = (trigger["label"], trigger["file"], trigger.get("pointer"))
            record = common_wrapper_groups.setdefault(key, {
                "trigger": trigger,
                "groupIds": set(),
                "invocationIds": set(),
            })
            record["groupIds"].add(group_by_invocation[row["invocationId"]])
            record["invocationIds"].add(row["invocationId"])
    common_wrapper_ambiguities = [
        {
            "trigger": record["trigger"],
            "groupIds": sorted(record["groupIds"]),
            "invocationIds": sorted(record["invocationIds"]),
            "resolution": "left separate because a common reachable wrapper can contain sequential battles; only explicit branch or rematch evidence may merge them",
        }
        for _, record in sorted(common_wrapper_groups.items())
        if len(record["groupIds"]) > 1
    ]

    invocations_by_label: dict[tuple[str, str], list[dict]] = defaultdict(list)
    for row in reachable:
        source = row["source"]
        invocations_by_label[(source["file"], source["label"])].append(row)
    multi_command_labels = []
    for (file_name, label), rows in sorted(invocations_by_label.items()):
        group_ids = {group_by_invocation[row["invocationId"]] for row in rows}
        if len(rows) < 2 or len(group_ids) < 2:
            continue
        multi_command_labels.append({
            "file": file_name,
            "label": label,
            "invocationIds": sorted(row["invocationId"] for row in rows),
            "groupIds": sorted(group_ids),
            "resolution": "commands remain separate because lexical co-location does not prove alternatives; they may execute sequentially",
        })

    guide = json.loads(GUIDE_PATH.read_text())
    reachable_trainers = {
        trainer_id
        for group in groups
        for trainer_id in group["resolvedOpponentTrainerIds"] + group["allyTrainerIds"]
    }
    guide_without_invocation = sorted({
        entry["trainerId"]
        for entry in guide["entries"]
        if entry["trainerId"] not in reachable_trainers
    })

    unresolved_rows = [{
        "invocationId": row["invocationId"],
        "source": row["source"],
        "opponentTrainerIds": row["opponentTrainerIds"],
        "reason": "No map JSON event, map-script table, or non-extern C runtime reference reaches the enclosing label in the static source graph.",
    } for row in unresolved]

    gabby_ty = [
        row for row in reachable
        if row["source"]["file"] == "data/scripts/gabby_and_ty.inc"
    ]
    trainer_hill = [
        row for row in reachable
        if row["source"]["file"] == "data/scripts/trainer_hill.inc"
    ]
    mossdeep_gauntlet = [
        row for row in reachable
        if row["source"]["label"] == "MossdeepCity_SpaceCenter_2F_EventScript_BattleThreeMagmaGrunts"
    ]
    mossdeep_group_ids = sorted({
        group_by_invocation[row["invocationId"]]
        for row in mossdeep_gauntlet
    })
    rich_boy_false_edges = [
        row["invocationId"]
        for row in mossdeep_gauntlet
        if any(
            root["label"] == "MossdeepCity_SpaceCenter_2F_EventScript_RichBoy"
            for root in row["triggerRoots"]
        )
    ]
    if len(gabby_ty) != 12:
        raise ValueError(f"global Gabby/Ty invocation coverage drift: {len(gabby_ty)}")
    if len(trainer_hill) != 1:
        raise ValueError(f"global Trainer Hill invocation coverage drift: {len(trainer_hill)}")
    if len(mossdeep_gauntlet) != 3 or len(mossdeep_group_ids) != 3:
        raise ValueError(
            "Mossdeep three-grunt gauntlet must remain three sequential physical groups: "
            f"invocations={len(mossdeep_gauntlet)}, groups={mossdeep_group_ids}"
        )
    if rich_boy_false_edges:
        raise ValueError(f"Mossdeep Rich Boy comment created false graph edges: {rich_boy_false_edges}")
    source_assertions = {
        "gabbyAndTy": {
            "file": "data/scripts/gabby_and_ty.inc",
            "reachableInvocations": len(gabby_ty),
            "invocationIds": sorted(row["invocationId"] for row in gabby_ty),
            "status": "included",
        },
        "trainerHill": {
            "file": "data/scripts/trainer_hill.inc",
            "reachableInvocations": len(trainer_hill),
            "invocationIds": sorted(row["invocationId"] for row in trainer_hill),
            "status": "included via C runtime reference",
        },
        "mossdeepThreeGruntGauntlet": {
            "file": "data/maps/MossdeepCity_SpaceCenter_2F/scripts.inc",
            "label": "MossdeepCity_SpaceCenter_2F_EventScript_BattleThreeMagmaGrunts",
            "reachableInvocations": len(mossdeep_gauntlet),
            "separatePhysicalGroups": len(mossdeep_group_ids),
            "groupIds": mossdeep_group_ids,
            "richBoyCommentEdges": len(rich_boy_false_edges),
            "status": "three sequential commands remain separate; comment is not control flow",
        },
    }

    return {
        "meta": {
            "title": "Emerald Champions Physical Encounter Atlas",
            "version": 1,
            "scope": "All trainerbattle*, trainerbattle, and multi_2_vs_2 invocations declared in data/maps/*/scripts.inc and data/scripts/*.inc, with static reachability from map JSON, map callback tables, and C runtime entry points.",
            "groupingRule": "Merge only when proven by the canonical sequence, one trainer id in one script source, an explicit REMATCH() row, or direct switch cases. A common reachable wrapper is not grouping proof because it may execute battles sequentially. Never infer grouping from proximity, name, class, or theme.",
            "canonicalBoundary": {
                **boundary,
                "source": "docs/verdant_battle_sequence.json",
                "meaning": "Only these explicitly indexed groups have authored order. Every group without a sequence index is unordered and may belong before, between, or after indexed encounters.",
            },
        },
        "totals": {
            "scriptBattleDeclarations": len(invocations),
            "provenReachableInvocations": len(reachable),
            "unresolvedReachabilityDeclarations": len(unresolved),
            "physicalEncounterGroups": len(groups),
            "canonicalSequenceGroups": sum(group["sequenceIndex"] is not None for group in groups),
            "unorderedFoundationGroups": sum(group["sequenceIndex"] is None for group in groups),
            "directOpponentTrainerIds": len({trainer_id for row in reachable for trainer_id in row["opponentTrainerIds"]}),
            "resolvedOpponentTrainerIdsIncludingRematches": len({trainer_id for group in groups for trainer_id in group["resolvedOpponentTrainerIds"]}),
            "allyTrainerIds": len({trainer_id for row in reachable for trainer_id in row["allyTrainerIds"]}),
        },
        "countsByChapter": by_chapter,
        "countsByCategory": by_category,
        "countsByOpcode": by_opcode,
        "reachableInvocationCountsByRootType": by_root_type,
        "sourceCoverageAssertions": source_assertions,
        "physicalGroups": groups,
        "invocations": invocations,
        "gapAndAmbiguityInventory": {
            "unresolvedSourceInvocations": unresolved_rows,
            "authoritativeGroupingConflicts": conflicts,
            "crossMapTriggerInvocations": cross_map,
            "runtimeSelectedFacilityIdentity": runtime_identity,
            "commonWrapperGroupsLeftUnmerged": common_wrapper_ambiguities,
            "multiCommandLabelsLeftUnmerged": multi_command_labels,
            "trainerIdsReusedAcrossUnmergedGroups": reused_across_groups,
            "guideTrainerDefinitionsWithoutReachableScriptInvocation": guide_without_invocation,
            "unorderedChronology": {
                "groupCount": sum(group["sequenceIndex"] is None for group in groups),
                "resolution": "unresolved by design; these groups may occur before, between, or after indexed encounters and must be inserted by verified physical play order",
            },
            "engineDrivenFacilityBoundary": "Facility battles started wholly by specials/C without a trainerbattle-family map-script command are outside this invocation atlas and must be indexed by a separate facility runtime audit.",
        },
    }


def markdown(data: dict) -> str:
    totals = data["totals"]
    boundary = data["meta"]["canonicalBoundary"]
    minimum = boundary["minimumIndex"]
    maximum = boundary["maximumIndex"]
    lines = [
        "# Emerald Champions Physical Encounter Atlas",
        "",
        "Generated by `scripts/verdant_physical_encounter_atlas.py`. Do not hand-edit.",
        "",
        f"This is a physical-trigger atlas, not another trainer-definition guide. It preserves authored sequence indices {minimum}-{maximum}. Groups without an index are explicitly unordered: they may occur before, between, or after indexed encounters.",
        "",
        "## Measured scope",
        "",
        f"- {totals['scriptBattleDeclarations']} trainer-battle source declarations",
        f"- {totals['provenReachableInvocations']} statically reachable invocations",
        f"- {totals['unresolvedReachabilityDeclarations']} declarations with no proven runtime entry",
        f"- {totals['physicalEncounterGroups']} proven physical encounter groups",
        f"- {totals['canonicalSequenceGroups']} authoritative sequence groups (indices {minimum}-{maximum})",
        f"- {totals['unorderedFoundationGroups']} unsequenced groups awaiting verified physical placement",
        "",
        "Grouping proof is deliberately narrow: canonical sequence, one trainer id in one script source, a `REMATCH()` row, or direct `case` branches. A common wrapper is reachability evidence only because it may run multiple battles sequentially. Adjacency, repeated names, trainer class, and apparent theme are never proof.",
        "",
        "## Focused source assertions",
        "",
        f"- Gabby and Ty: {data['sourceCoverageAssertions']['gabbyAndTy']['reachableInvocations']} reachable commands from `data/scripts/gabby_and_ty.inc`.",
        f"- Trainer Hill: {data['sourceCoverageAssertions']['trainerHill']['reachableInvocations']} reachable command from `data/scripts/trainer_hill.inc`.",
        f"- Mossdeep gauntlet: {data['sourceCoverageAssertions']['mossdeepThreeGruntGauntlet']['reachableInvocations']} reachable commands remain {data['sourceCoverageAssertions']['mossdeepThreeGruntGauntlet']['separatePhysicalGroups']} separate physical groups; Rich Boy comment edges = {data['sourceCoverageAssertions']['mossdeepThreeGruntGauntlet']['richBoyCommentEdges']}.",
        "",
        "## Counts by chapter",
        "",
        "| Chapter | Physical groups | Script invocations |",
        "|---|---:|---:|",
    ]
    for chapter, counts in data["countsByChapter"].items():
        lines.append(f"| {chapter} | {counts['physicalGroups']} | {counts['scriptInvocations']} |")
    lines += [
        "",
        "## Counts by category",
        "",
        "| Category | Physical groups | Script invocations |",
        "|---|---:|---:|",
    ]
    for category, counts in data["countsByCategory"].items():
        lines.append(f"| {category} | {counts['physicalGroups']} | {counts['scriptInvocations']} |")
    lines += [
        "",
        "## Physical encounter groups",
        "",
        "`Seq` is blank when a group has no authored order. A blank does not mean the encounter occurs after the indexed range. `Calls` counts reachable source opcodes, not party definitions.",
        "",
        "| Atlas | Seq | Group | Chapter | Category | Trainers | Calls | First source | Proof |",
        "|---:|---:|---|---|---|---|---:|---|---|",
    ]
    for group in data["physicalGroups"]:
        sequence = group["sequenceIndex"] if group["sequenceIndex"] is not None else ""
        trainers = ", ".join(value.removeprefix("TRAINER_") for value in group["sourceOpponentTrainerIds"])
        source = group["sources"][0]
        proof = "; ".join(item["proofType"] for item in group["groupingEvidence"])
        lines.append(
            f"| {group['atlasOrdinal']} | {sequence} | `{group['groupId']}` | {group['primaryChapter']} | "
            f"{group['category']} | {trainers} | {len(group['invocationIds'])} | "
            f"`{source['file']}:{source['line']}` `{source['label']}` `{source['opcode']}` | {proof} |"
        )

    gaps = data["gapAndAmbiguityInventory"]
    lines += [
        "",
        "## Gap and ambiguity inventory",
        "",
        f"- **Unordered chronology:** {gaps['unorderedChronology']['groupCount']} groups intentionally have no canonical sequence index. {gaps['unorderedChronology']['resolution']}.",
        f"- **Authoritative merge conflicts:** {len(gaps['authoritativeGroupingConflicts'])}. A noncanonical inference is rejected whenever it would merge distinct indexed encounters in {minimum}-{maximum}.",
        f"- **Common wrappers left unmerged:** {len(gaps['commonWrapperGroupsLeftUnmerged'])}. Shared reachability is not mistaken for an alternative branch.",
        f"- **Multi-command labels left unmerged:** {len(gaps['multiCommandLabelsLeftUnmerged'])}. Sequential commands remain separate unless another proof joins them.",
        f"- **Cross-map trigger ownership:** {len(gaps['crossMapTriggerInvocations'])} reachable invocations are defined in a different map script file from at least one triggering event.",
        f"- **Reused trainer ids left split:** {len(gaps['trainerIdsReusedAcrossUnmergedGroups'])} ids appear in multiple groups without proof they are one encounter.",
        f"- **Guide definitions without reachable script invocation:** {len(gaps['guideTrainerDefinitionsWithoutReachableScriptInvocation'])}.",
        f"- **Facility boundary:** {gaps['engineDrivenFacilityBoundary']}",
        "",
        "### Unresolved source declarations",
        "",
        "These declarations remain visible instead of being silently counted as encounters.",
        "",
        "| Invocation | Trainers | Source label | Opcode |",
        "|---|---|---|---|",
    ]
    for row in gaps["unresolvedSourceInvocations"]:
        source = row["source"]
        trainers = ", ".join(row["opponentTrainerIds"]) or "unresolved trainer symbol"
        lines.append(
            f"| `{row['invocationId']}` | {trainers} | `{source['file']}:{source['line']}` `{source['label']}` | `{source['opcode']}` |"
        )
    lines += [
        "",
        "### Runtime-selected facility identity",
        "",
    ]
    if gaps["runtimeSelectedFacilityIdentity"]:
        for row in gaps["runtimeSelectedFacilityIdentity"]:
            lines.append(f"- `{row['invocationId']}`: {row['reason']}")
    else:
        lines.append("- None detected.")
    lines += [
        "",
        "### Trainer ids reused across unmerged groups",
        "",
    ]
    if gaps["trainerIdsReusedAcrossUnmergedGroups"]:
        for row in gaps["trainerIdsReusedAcrossUnmergedGroups"]:
            lines.append(
                f"- `{row['trainerId']}`: {', '.join(f'`{value}`' for value in row['groupIds'])} "
                f"({', '.join(row['maps'])}); {row['resolution']}."
            )
    else:
        lines.append("- None detected.")
    lines.append("")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true", help="write deterministic JSON and Markdown outputs")
    parser.add_argument("--check", action="store_true", help="fail if checked-in outputs differ")
    args = parser.parse_args()

    data = build()
    json_text = json.dumps(data, indent=2, sort_keys=False) + "\n"
    markdown_text = markdown(data)
    if args.write:
        JSON_PATH.write_text(json_text)
        MARKDOWN_PATH.write_text(markdown_text)
    if args.check:
        problems = []
        if not JSON_PATH.exists() or JSON_PATH.read_text() != json_text:
            problems.append(relative(JSON_PATH))
        if not MARKDOWN_PATH.exists() or MARKDOWN_PATH.read_text() != markdown_text:
            problems.append(relative(MARKDOWN_PATH))
        if problems:
            raise SystemExit("FAIL: physical encounter atlas drift: " + ", ".join(problems))

    totals = data["totals"]
    boundary = data["meta"]["canonicalBoundary"]
    print(
        "PASS: physical encounter atlas covers "
        f"{totals['provenReachableInvocations']}/{totals['scriptBattleDeclarations']} reachable script invocations "
        f"in {totals['physicalEncounterGroups']} proven physical groups"
    )
    print(
        f"PASS: canonical sequence indices {boundary['minimumIndex']}-{boundary['maximumIndex']} are preserved; "
        f"{totals['unorderedFoundationGroups']} unsequenced groups remain explicitly unordered"
    )
    print(
        f"INFO: {totals['unresolvedReachabilityDeclarations']} source declarations have unresolved reachability"
    )


if __name__ == "__main__":
    main()
