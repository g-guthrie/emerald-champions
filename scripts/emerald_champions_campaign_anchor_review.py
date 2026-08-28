#!/usr/bin/env python3
"""Review every backward-designed Emerald Champions campaign anchor as one board."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT_JSON = ROOT / "docs/emerald_champions_campaign_anchor_review.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_campaign_anchor_review.md"

BOARD_PATHS = [
    ("league", ROOT / "docs/verdant_marquee_battle_designs.json"),
    ("gyms", ROOT / "docs/emerald_champions_gym_anchor_designs.json"),
    ("factions", ROOT / "docs/emerald_champions_faction_anchor_designs.json"),
    ("rivals_superbosses", ROOT / "docs/emerald_champions_superboss_anchor_designs.json"),
    ("frontier_brains", ROOT / "docs/emerald_champions_frontier_brain_designs.json"),
]
REMATCH_PATH = ROOT / "docs/emerald_champions_rematch_family_audit.json"
SEQUENCE_PATH = ROOT / "docs/verdant_battle_sequence.json"
BESPOKE_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"

KEYWORDS = {
    "tailwind": "Tailwind", "trick_room": "Trick Room", "redirection": "Rage Powder|Follow Me|redirection",
    "weather": "sun|rain|snow|sand|weather|Drought|Drizzle", "setup": "setup|Shell Smash|Dragon Dance|Belly Drum|Shift Gear|Coil",
    "priority": "priority|Fake Out|Aqua Jet|Sucker Punch|Extreme Speed", "choice": "Choice", "wide_guard": "Wide Guard",
    "self_activation": "self-activation|activation", "trap_clock": "Perish|trap|clock", "manual_field": "Gravity|terrain|screen",
}


def load(path):
    return json.loads(path.read_text())


def all_mons(dossier):
    result = list(dossier["team"])
    for team in dossier.get("opponent_teams", {}).values():
        result.extend(team)
    return result


def build():
    boards = []
    designs = []
    allowed = set()
    for board_name, path in BOARD_PATHS:
        payload = load(path)
        boards.append({"board": board_name, "path": str(path.relative_to(ROOT)), "anchor_count": len(payload["designs"])})
        for anchor_id, dossier in payload["designs"].items():
            designs.append((board_name, anchor_id, dossier))
        review = payload.get("pair_review") or payload.get("anchor_review") or {}
        for key in ("allowed_protected_reuses", "allowed_internal_reuses"):
            for row in review.get(key, []):
                allowed.add((row["anchor_id"], row["species"]))

    species_uses = defaultdict(list)
    mega_uses = defaultdict(list)
    primal_uses = []
    question_uses = defaultdict(list)
    reference_uses = defaultdict(list)
    status_counts = Counter()
    strategy_counts = Counter()
    backfills = []
    slot_count = 0
    for board_name, anchor_id, dossier in designs:
        mons = all_mons(dossier)
        slot_count += len(mons)
        for entry in mons:
            species_uses[entry["species"]].append({"board": board_name, "anchor_id": anchor_id})
            if entry.get("mega_candidate"):
                mega_uses[(entry["species"], entry["item"])].append(anchor_id)
            if entry["item"] in {"ITEM_RED_ORB", "ITEM_BLUE_ORB"}:
                primal_uses.append({"anchor_id": anchor_id, "species": entry["species"], "item": entry["item"]})
        question_uses[dossier["identity"]["primary_player_question"]].append(anchor_id)
        for reference_id in dossier["competitive_research"]["selected_reference_ids"]:
            reference_uses[reference_id].append(anchor_id)
        status_counts[(dossier["status"]["design"], dossier["status"]["source"], dossier["status"]["runtime"])] += 1
        active_text = json.dumps({
            "identity": dossier["identity"], "difficulty": dossier["difficulty"],
            "ordering": dossier["ordering"], "ai": dossier["ai"], "counterplay": dossier["counterplay"],
        })
        for label, pattern in KEYWORDS.items():
            if any(token.lower() in active_text.lower() for token in pattern.split("|")):
                strategy_counts[label] += 1
        if dossier["status"]["source"] in {"revision-required"} or dossier.get("mechanics_proposal"):
            backfills.append({
                "anchor_id": anchor_id,
                "source_status": dossier["status"]["source"],
                "mechanics_proposal": dossier.get("mechanics_proposal"),
                "source_blockers": dossier["verification"]["source_blockers"],
            })

    collisions = {species: uses for species, uses in species_uses.items() if len(uses) > 1}
    unwaived = {}
    for species, uses in collisions.items():
        # The first authored use owns the reveal. Every later use must be explicitly waived.
        later = uses[1:]
        if any((row["anchor_id"], species) not in allowed for row in later):
            unwaived[species] = uses

    rematches = load(REMATCH_PATH)
    sequence = load(SEQUENCE_PATH)
    bespoke = load(BESPOKE_PATH)
    repeated_refs = {reference_id: anchors for reference_id, anchors in reference_uses.items() if len(anchors) > 1}
    duplicate_questions = {question: anchors for question, anchors in question_uses.items() if len(anchors) > 1}
    mega_collisions = {f"{species}|{item}": anchors for (species, item), anchors in mega_uses.items() if len(anchors) > 1}

    return {
        "version": 1,
        "title": "Emerald Champions campaign-wide anchor review",
        "scope": "Backward-designed primary anchors plus the separately source-reviewed Gym/Wally rematch layer. Ordinary physical encounters remain governed by forward chronological closure.",
        "boards": boards,
        "totals": {
            "primary_anchors": len(designs), "primary_slots": slot_count,
            "distinct_primary_species": len(species_uses), "species_collisions": len(collisions),
            "unwaived_species_collisions": len(unwaived), "mega_signatures": len(mega_uses),
            "mega_signature_collisions": len(mega_collisions), "primal_uses": len(primal_uses),
            "distinct_primary_questions": len(question_uses), "duplicate_primary_questions": len(duplicate_questions),
            "selected_reference_ids": len(reference_uses), "reused_reference_ids": len(repeated_refs),
            "required_backfills": len(backfills),
            "rematch_families": rematches["totals"]["families"], "reachable_rematch_records": rematches["totals"]["reachable_records"],
            "rematch_pokemon_sets": rematches["totals"]["pokemon_sets"],
            "chronological_closed": sum(entry["status"] == "closed" for entry in sequence["entries"]),
            "chronological_next": next(entry["index"] for entry in sequence["entries"] if entry["status"] == "next"),
            "bespoke_closed_designs": sum(design.get("status") == "closed" for design in bespoke["designs"].values()),
        },
        "species_collisions": collisions,
        "allowed_reuses": [{"anchor_id": anchor, "species": species} for anchor, species in sorted(allowed)],
        "unwaived_species_collisions": unwaived,
        "mega_signature_collisions": mega_collisions,
        "primal_uses": primal_uses,
        "duplicate_primary_questions": duplicate_questions,
        "strategy_presence": dict(sorted(strategy_counts.items())),
        "reused_references": repeated_refs,
        "status_counts": [{"design": key[0], "source": key[1], "runtime": key[2], "count": count} for key, count in sorted(status_counts.items())],
        "required_backfills": backfills,
        "rematch_layer": {
            "status": "source-reviewed-static-pass",
            "families": rematches["totals"]["families"], "records": rematches["totals"]["reachable_records"],
            "sets": rematches["totals"]["pokemon_sets"], "quality_floor": rematches["totals"]["minimum_quality_score"],
            "runtime_observed": None, "dedupe_policy": rematches["policy"]["reason"],
        },
        "judgment": {
            "collision_result": "Every repeated primary species is a documented character, iconic-team, or form/progression exception; no unwaived collision remains.",
            "gimmick_result": "The only battle transformations are Mega Evolution plus exactly one Red Orb Maxie and one Blue Orb Archie. Slateport Archie contains no premature Kyogre.",
            "variety_result": "No global quota is used. Strategy counts are a drift monitor only; exact primary questions remain unique and teams span speed, field, priority, setup, commitment, self-activation, traps, multi battles, and random generation.",
            "status_result": f"Design-complete is not source-complete. Most backward anchors remain unimplemented/unplayed; rematches are source-reviewed; chronological closure is {sum(entry['status'] == 'closed' for entry in sequence['entries'])} with Battle {next(entry['index'] for entry in sequence['entries'] if entry['status'] == 'next')} next.",
        },
    }


def validate(payload):
    totals = payload["totals"]
    if totals["primary_anchors"] != 37 or totals["primary_slots"] != 225:
        raise AssertionError("primary anchor scope drifted")
    if totals["unwaived_species_collisions"] or totals["mega_signature_collisions"] or totals["duplicate_primary_questions"]:
        raise AssertionError("campaign anchor collision gate failed")
    expected_primals = {
        ("MAGMA_HIDEOUT_FINAL_MAXIE", "SPECIES_GROUDON", "ITEM_RED_ORB"),
        ("SEAFLOOR_CAVERN_FINAL_ARCHIE", "SPECIES_KYOGRE", "ITEM_BLUE_ORB"),
    }
    actual_primals = {(row["anchor_id"], row["species"], row["item"]) for row in payload["primal_uses"]}
    if actual_primals != expected_primals:
        raise AssertionError(f"Primal reveal contract drifted: {actual_primals}")
    if totals["rematch_families"] != 9 or totals["reachable_rematch_records"] != 35 or totals["rematch_pokemon_sets"] != 210:
        raise AssertionError("rematch layer drifted")
    if totals["chronological_closed"] != totals["bespoke_closed_designs"] or totals["chronological_next"] != totals["chronological_closed"] + 1:
        raise AssertionError("forward closure frontier drifted")
    required = {row["anchor_id"] for row in payload["required_backfills"]}
    for anchor in ("LILYCOVE_RIVAL", "ROUTE_119_RIVAL", "STEVEN_MOSSDEEP_ALLY", "NOLAND"):
        if anchor not in required:
            raise AssertionError(f"required backfill missing {anchor}")
    if payload["rematch_layer"]["runtime_observed"] is not None:
        raise AssertionError("rematches claim runtime difficulty without playtest")


def markdown(payload):
    t = payload["totals"]
    lines = [
        "# Emerald Champions campaign-wide anchor review", "",
        f"PASS: {t['primary_anchors']} primary anchors, {t['primary_slots']} authored deployable slots, {t['distinct_primary_species']} distinct species.",
        f"PASS: {t['unwaived_species_collisions']} unwaived species collisions, {t['mega_signature_collisions']} Mega-signature collisions, {t['duplicate_primary_questions']} duplicate primary questions.",
        f"PASS: exactly two Primals in story-correct locations; {t['mega_signatures']} distinct Mega signatures.",
        f"Rematches: {t['rematch_families']} families, {t['reachable_rematch_records']} records, {t['rematch_pokemon_sets']} exact sets; runtime still unplayed.",
        f"Forward source closure remains Battle {t['chronological_closed']} closed; Battle {t['chronological_next']} next.", "",
        "## Required source backfills", "",
    ]
    for row in payload["required_backfills"]:
        lines.append(f"- `{row['anchor_id']}` — source `{row['source_status']}`; {len(row['source_blockers'])} explicit blockers.")
    lines += ["", "## Strategy drift monitor", ""]
    for key, count in payload["strategy_presence"].items():
        lines.append(f"- {key}: {count} anchors")
    lines += ["", "## Judgment", "", payload["judgment"]["collision_result"], "", payload["judgment"]["variety_result"], "", payload["judgment"]["status_result"], ""]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    payload = build(); validate(payload)
    expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"; expected_md = markdown(payload)
    if args.write: OUTPUT_JSON.write_text(expected_json); OUTPUT_MD.write_text(expected_md)
    if args.check:
        if not OUTPUT_JSON.exists() or OUTPUT_JSON.read_text() != expected_json: raise SystemExit("FAIL: campaign anchor review JSON stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md: raise SystemExit("FAIL: campaign anchor review Markdown stale")
    print("PASS: campaign-wide anchor board has zero unwaived species/Mega/question collisions")
    print("PASS: story-correct Primals, documented strategy variety, honest source/runtime status, and rematch layer")
    print(f"NEXT: implement required anchor backfills when reached; continue forward closure at Battle {payload['totals']['chronological_next']}")


if __name__ == "__main__": main()
