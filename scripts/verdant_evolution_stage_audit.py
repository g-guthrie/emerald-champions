#!/usr/bin/env python3
"""Gate early bespoke battles against Verdant's real evolution progression.

This audit intentionally reads the game sources instead of maintaining a
second species-stage list.  Trainer levels are offsets from the active strict
cap, so a party member with ``.lvl = 1`` in the opening chapter is level 15,
not level 1.

The gate is expected to fail while an early encounter still uses a roster that
belongs later in the campaign.  A non-zero exit is therefore useful design
feedback, not a reason to weaken the policy.
"""

from __future__ import annotations

import json
import math
import re
from dataclasses import dataclass
from pathlib import Path

import verdant_battle_guide as guide
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]
DESIGNS_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"
EVOLUTIONS_PATH = ROOT / "src/data/pokemon/evolution.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"
TRAINERS_PATH = ROOT / "src/data/trainers.h"
POLICY_PATH = ROOT / "docs/verdant_evolution_stage_policy.md"
WILD_ENCOUNTERS_PATH = ROOT / "src/data/wild_encounters.json"

# These change a Pokemon during battle and are not family-stage progression.
BATTLE_TRANSFORM_METHODS = {
    "EVO_MEGA_EVOLUTION",
    "EVO_MOVE_MEGA_EVOLUTION",
    "EVO_PRIMAL_REVERSION",
}
MEGA_METHODS = {"EVO_MEGA_EVOLUTION", "EVO_MOVE_MEGA_EVOLUTION"}
SHOWCASE_KINDS = {"rare-single-stage", "legendary-single-stage"}


@dataclass(frozen=True)
class Evolution:
    source: str
    method: str
    parameter: str
    target: str

    @property
    def is_battle_transform(self) -> bool:
        return self.method in BATTLE_TRANSFORM_METHODS

    @property
    def level_requirement(self) -> int:
        if not self.method.startswith("EVO_LEVEL"):
            return 1
        if not self.parameter.isdigit():
            raise ValueError(
                f"{self.source} -> {self.target}: {self.method} has "
                f"non-numeric level parameter {self.parameter!r}"
            )
        return int(self.parameter)


def read(path: Path) -> str:
    return path.read_text()


def parse_evolutions() -> list[Evolution]:
    """Parse every edge in gEvolutionTable, including non-level methods."""
    source = re.sub(
        r"/\*.*?\*/|//[^\n]*",
        "",
        read(EVOLUTIONS_PATH)
        + "\n"
        + read(ROOT / "src/data/pokemon/verdant_gen9_evolutions.h"),
        flags=re.S,
    )
    entries = list(
        re.finditer(
            r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(.*?)"
            r"(?=^\s*\[SPECIES_[A-Z0-9_]+\]\s*=|^};)",
            source,
            re.M | re.S,
        )
    )
    if not entries:
        raise ValueError("could not parse any entries from gEvolutionTable")

    result: list[Evolution] = []
    for entry in entries:
        species = entry.group(1)
        body = entry.group(2)
        triples = re.findall(
            r"\{\s*(EVO_[A-Z0-9_]+)\s*,\s*([^,{}]+?)\s*,\s*"
            r"(SPECIES_[A-Z0-9_]+)\s*\}",
            body,
        )
        for method, parameter, target in triples:
            result.append(Evolution(species, method, parameter.strip(), target))
    if not result:
        raise ValueError("gEvolutionTable entries contained no evolution edges")
    return result


def parse_strict_caps() -> list[int]:
    source = read(ROOT / "src/pokemon.c")
    match = re.search(
        r"static const u8 levelCapsStrict\[\]\s*=\s*\{([^}]+)\}", source
    )
    if not match:
        raise ValueError("could not find levelCapsStrict in src/pokemon.c")
    caps = [int(value) for value in re.findall(r"\d+", match.group(1))]
    if len(caps) < 9:
        raise ValueError(f"strict cap table is unexpectedly short: {caps}")
    return caps


def stage_graph(
    evolutions: list[Evolution],
) -> tuple[dict[str, list[Evolution]], dict[str, list[Evolution]]]:
    incoming: dict[str, list[Evolution]] = {}
    outgoing: dict[str, list[Evolution]] = {}
    for edge in evolutions:
        if edge.is_battle_transform:
            continue
        incoming.setdefault(edge.target, []).append(edge)
        outgoing.setdefault(edge.source, []).append(edge)
    return incoming, outgoing


def minimum_legal_levels(
    evolutions: list[Evolution],
) -> tuple[dict[str, int], dict[str, list[Evolution]]]:
    """Return minimum attainable level and its evolution path for each species.

    Non-level methods (stone, trade, friendship, move, location, and so on)
    still make their target an evolved species, but add no artificial level
    threshold.  Earlier level requirements in the same family are propagated;
    for example Mamoswine cannot precede Piloswine's real level requirement.
    """
    incoming, outgoing = stage_graph(evolutions)
    species = set(incoming) | set(outgoing)
    for edges in incoming.values():
        species.update(edge.source for edge in edges)
    for edges in outgoing.values():
        species.update(edge.target for edge in edges)

    levels = {mon: 1 for mon in species if mon not in incoming}
    paths: dict[str, list[Evolution]] = {mon: [] for mon in levels}

    # Evolution families are acyclic in the source, but a bounded relaxation
    # also gives a safe, deterministic failure mode if that ever changes.
    for _ in range(len(species) + 1):
        changed = False
        for edge in (edge for edge in evolutions if not edge.is_battle_transform):
            if edge.source not in levels:
                continue
            candidate = max(levels[edge.source], edge.level_requirement)
            if edge.target not in levels or candidate < levels[edge.target]:
                levels[edge.target] = candidate
                paths[edge.target] = paths[edge.source] + [edge]
                changed = True
        if not changed:
            break

    unresolved = sorted(species - levels.keys())
    if unresolved:
        raise ValueError(
            "evolution graph has no root-reachable path for: " + ", ".join(unresolved)
        )
    return levels, paths


def species_stage(
    species: str,
    incoming: dict[str, list[Evolution]],
    outgoing: dict[str, list[Evolution]],
) -> str:
    if species in incoming:
        return "evolved"
    if species in outgoing:
        return "first-stage"
    return "single-stage"


def parse_showcase_annotations() -> dict[tuple[str, str], tuple[str, str]]:
    """Read explicit early rare/legendary showcase notes from the policy table."""
    source = read(POLICY_PATH)
    annotations: dict[tuple[str, str], tuple[str, str]] = {}
    pattern = re.compile(
        r"^\|\s*`?(BATTLE_[A-Z0-9_]+)`?\s*"
        r"\|\s*`?(SPECIES_[A-Z0-9_]+)`?\s*"
        r"\|\s*`?([a-z-]+)`?\s*\|\s*([^|]+?)\s*\|\s*$",
        re.M,
    )
    for encounter, species, kind, rationale in pattern.findall(source):
        key = (encounter, species)
        if key in annotations:
            raise ValueError(f"duplicate showcase annotation for {encounter}/{species}")
        annotations[key] = (kind, rationale.strip())
    return annotations


def parse_bracelet_access() -> list[dict]:
    """Find actual campaign scripts that grant the player's Mega Bracelet."""
    grants = []
    grant_pattern = re.compile(
        r"(?:\bgiveitem\s+ITEM_MEGA_BRACELET\b|"
        r"\bspecial\s+TryGiveVerdant(?:StevenRewardBundle|MegaKit)\b)"
    )
    for path in sorted((ROOT / "data/maps").glob("*/scripts.inc")):
        lines = path.read_text(errors="ignore").splitlines()
        for index, line in enumerate(lines, 1):
            if grant_pattern.search(line):
                map_name = path.parent.name
                chapter = guide.chapter_for(map_name, "")
                grants.append(
                    {
                        "map": map_name,
                        "badge": chapter["badge"],
                        "cap": chapter["cap"],
                        "rank": guide.map_rank(map_name),
                        "line": index,
                        "file": str(path.relative_to(ROOT)),
                    }
                )
    if not grants:
        raise ValueError("no reachable script grants ITEM_MEGA_BRACELET")
    return sorted(grants, key=lambda row: (row["badge"], row["rank"], row["line"]))


def parse_starter_species() -> list[str]:
    source = read(ROOT / "src/starter_choose.c")
    arrays = re.findall(
        r"sStarterMon[A-Za-z0-9_]+\[STARTER_MON_COUNT\]\s*=\s*\{(.*?)\};",
        source,
        re.S,
    )
    starters = []
    for body in arrays:
        starters.extend(re.findall(r"SPECIES_[A-Z0-9_]+", body))
    result = list(dict.fromkeys(starters))
    if len(result) != 21:
        raise ValueError(f"expected 21 selectable starters, parsed {len(result)}")
    return result


def validate_first_mega_path(
    evolutions: list[Evolution], first_bracelet: dict, strict_caps: list[int]
) -> tuple[list[str], list[str]]:
    """Verify that the first Bracelet arrives with a native, usable Mega path."""
    problems: list[str] = []
    evidence: list[str] = []

    if (
        first_bracelet["map"] != "GraniteCave_StevensRoom"
        or first_bracelet["badge"] != 1
        or strict_caps[first_bracelet["badge"]] != 20
    ):
        problems.append(
            "first Mega Bracelet must remain Steven's post-Stone Granite Cave "
            "reward at the cap-20 milestone"
        )
    else:
        evidence.append("Steven grants the first Bracelet after Stone Badge at cap 20")

    encounters = json.loads(read(WILD_ENCOUNTERS_PATH))
    by_map = {
        encounter["map"]: encounter
        for group in encounters["wild_encounter_groups"]
        for encounter in group["encounters"]
        if "map" in encounter
    }
    woods2 = by_map.get("MAP_PETALBURG_WOODS_2", {})
    honey_species = [
        mon["species"]
        for mon in woods2.get("honey_mons", {}).get("mons", [])
    ]
    if "SPECIES_WEEDLE" not in honey_species:
        problems.append("Petalburg Woods 2 Honey pool does not contain Weedle")
    if {"SPECIES_BEEDRILL", "SPECIES_BUTTERFREE"} & set(honey_species):
        problems.append(
            "Petalburg Woods 2 Honey pool skips the intended young-family progression"
        )
    if not problems or "SPECIES_WEEDLE" in honey_species:
        evidence.append("Stone-era Honey pool exposes Weedle in unevolved form")

    woods2_map = json.loads(read(ROOT / "data/maps/PetalburgWoods_2/map.json"))
    woods3_map = json.loads(read(ROOT / "data/maps/PetalburgWoods_3/map.json"))
    main_woods_map = json.loads(read(ROOT / "data/maps/PetalburgWoods/map.json"))
    if not any(
        warp.get("dest_map") == "MAP_PETALBURG_WOODS_2"
        for warp in main_woods_map.get("warp_events", [])
    ):
        problems.append("main Petalburg Woods has no warp into Petalburg Woods 2")
    if not any(
        event.get("graphics_id") == "OBJ_EVENT_GFX_CUTTABLE_TREE"
        for event in main_woods_map.get("object_events", [])
    ):
        problems.append("main Petalburg Woods no longer gates the Mega route with Cut")
    if not any(
        warp.get("dest_map") == "MAP_PETALBURG_WOODS_3"
        for warp in woods2_map.get("warp_events", [])
    ):
        problems.append("Petalburg Woods 2 has no warp into Petalburg Woods 3")

    stone_event = next(
        (
            event
            for event in woods3_map.get("object_events", [])
            if event.get("script") == "PetalburgWoods_3_Beedrillite"
        ),
        None,
    )
    if stone_event is None:
        problems.append("Petalburg Woods 3 does not place the Beedrillite object")
    elif stone_event.get("graphics_id") != "OBJ_EVENT_GFX_MEGA_STONE":
        problems.append("Petalburg Woods 3 Beedrillite lacks native Mega Stone graphics")

    item_scripts = read(ROOT / "data/scripts/item_ball_scripts.inc")
    if not re.search(
        r"PetalburgWoods_3_Beedrillite::\s*\n\s*finditem\s+ITEM_BEEDRILLITE\b",
        item_scripts,
    ):
        problems.append("Petalburg Woods 3 stone script does not grant Beedrillite")

    mart = read(ROOT / "data/scripts/general_mart.inc")
    stone_mart = re.search(
        r"PokeMart_Poke_Center_Stone_Badge::(.*?)(?=^\S.*?::$|\Z)",
        mart,
        re.M | re.S,
    )
    if not stone_mart or "ITEM_HONEY" not in stone_mart.group(1):
        problems.append("Stone Badge Pokémon Center shop does not sell Honey")

    rustboro_mart = read(ROOT / "data/maps/RustboroCity_Mart/map.json")
    if "General_Mart_Script" not in rustboro_mart:
        problems.append("Rustboro Poké Mart no longer exposes the badge-tier medicine shop")

    roxanne = read(ROOT / "data/maps/RustboroCity_Gym/scripts.inc")
    if "setflag FLAG_BADGE01_GET" not in roxanne:
        problems.append("Roxanne's reward no longer unlocks the Stone Badge shop tier")

    cutter = read(ROOT / "data/maps/RustboroCity_CuttersHouse/scripts.inc")
    if not re.search(r"\bgiveitem\s+ITEM_HM01_CUT\b", cutter):
        problems.append("Rustboro Cutter does not grant HM01 Cut")

    expected_edges = {
        ("SPECIES_WEEDLE", "EVO_LEVEL", "7", "SPECIES_KAKUNA"),
        ("SPECIES_KAKUNA", "EVO_LEVEL", "10", "SPECIES_BEEDRILL"),
        (
            "SPECIES_BEEDRILL",
            "EVO_MEGA_EVOLUTION",
            "ITEM_BEEDRILLITE",
            "SPECIES_BEEDRILL_MEGA",
        ),
    }
    actual_edges = {
        (edge.source, edge.method, edge.parameter, edge.target)
        for edge in evolutions
    }
    missing_edges = sorted(expected_edges - actual_edges)
    if missing_edges:
        problems.append(f"Beedrill Mega family edges are incomplete: {missing_edges}")

    if not any(
        event.get("graphics_id") == "OBJ_EVENT_GFX_CUTTABLE_TREE"
        for event in woods3_map.get("object_events", [])
    ):
        problems.append("Petalburg Woods 3 no longer gates its Mega alcove with Cut")

    if not problems:
        evidence.extend(
            [
                "Rustboro supplies Cut and its Poké Mart badge-tier medicine shop supplies Honey",
                "the main Woods Cut route reaches Woods 2 and the Beedrillite map",
                "Weedle evolves at 7/10 and Beedrill Mega Evolves with Beedrillite",
            ]
        )
    return problems, evidence


def trainer_party(
    trainer_id: str,
    trainer_blocks: dict,
    parties_text: str,
) -> list[dict]:
    if trainer_id not in trainer_blocks:
        raise ValueError(f"unknown trainer id {trainer_id}")
    block = trainer_blocks[trainer_id].group(0)
    required_flags = ("F_TRAINER_PARTY_HELD_ITEM", "F_TRAINER_PARTY_CUSTOM_MOVESET")
    if not all(flag in block for flag in required_flags):
        raise ValueError(
            f"{trainer_id}: early bespoke gate requires cap-relative ItemCustomMoves data"
        )
    party_name = doubles.party_name(block)
    match = doubles.party_match(parties_text, party_name)
    return [polish.parse_entry(entry) for entry in custom.party_entries(match.group(2))]


def party_variants(
    encounter_id: str,
    trainer_ids: list[str],
    trainer_blocks: dict,
    parties_text: str,
) -> list[list[dict]]:
    variants = [
        trainer_party(trainer_id, trainer_blocks, parties_text)
        for trainer_id in trainer_ids
    ]

    if encounter_id == "BATTLE_001_ROUTE_103_RIVAL":
        battle_main = read(ROOT / "src/battle_main.c")
        required = (
            "IsRoute103RivalTrainer(trainerNum)",
            "GetStarterPokemonForGeneration((VarGet(VAR_STARTER_MON) + 1) % 3",
        )
        if not all(token in battle_main for token in required):
            raise ValueError("Route 103 dynamic 21-starter counterpart hook is missing")
        template = variants[0]
        if len(template) != 1:
            raise ValueError("Route 103 rival template must contain exactly one Pokemon")
        variants = []
        for species in parse_starter_species():
            build = dict(template[0])
            build["species"] = species
            variants.append([build])

    unique: dict[tuple, list[dict]] = {}
    for party in variants:
        signature = tuple(
            (mon["level"], mon["species"], mon["item"], tuple(mon["moves"]))
            for mon in party
        )
        unique.setdefault(signature, party)
    return list(unique.values())


def encounter_campaign_state(
    trainer_ids: list[str],
    references: dict[str, list[dict]],
    strict_caps: list[int],
    expected_cap: int | None = None,
) -> dict:
    sources = []
    for trainer_id in trainer_ids:
        trainer_sources = [
            row for row in references.get(trainer_id, []) if row["role"] == "opponent"
        ]
        if not trainer_sources:
            raise ValueError(f"{trainer_id}: no reachable campaign trainerbattle reference")
        sources.extend(trainer_sources)

    states = []
    for source in sources:
        chapter = guide.chapter_for(source["map"], trainer_ids[0])
        badge = chapter["badge"]
        if badge >= len(strict_caps):
            raise ValueError(f"{source['map']}: badge index {badge} exceeds strict caps")
        states.append(
            {
                **source,
                "badge": badge,
                "cap": strict_caps[badge],
                "rank": guide.map_rank(source["map"]),
            }
        )
    earliest = min(states, key=lambda row: (row["badge"], row["rank"], row["line"]))
    if expected_cap is not None:
        if expected_cap not in strict_caps:
            raise ValueError(f"documented strict cap {expected_cap} is not in the source cap table")
        earliest = {
            **earliest,
            "source_badge": earliest["badge"],
            "source_cap": earliest["cap"],
            "badge": strict_caps.index(expected_cap),
            "cap": expected_cap,
            "campaign_state_source": "closed design strict_cap",
        }
    return {"earliest": earliest, "sources": states}


def describe_path(path: list[Evolution]) -> str:
    if not path:
        return "base species"
    pieces = []
    for edge in path:
        requirement = (
            f"Lv{edge.level_requirement}"
            if edge.method.startswith("EVO_LEVEL")
            else edge.method.removeprefix("EVO_").lower().replace("_", " ")
        )
        pieces.append(
            f"{edge.source.removeprefix('SPECIES_')} --{requirement}--> "
            f"{edge.target.removeprefix('SPECIES_')}"
        )
    return "; ".join(pieces)


def main() -> None:
    designs = json.loads(read(DESIGNS_PATH))["designs"]
    trainers_text = read(TRAINERS_PATH)
    parties_text = read(PARTIES_PATH)
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    references = guide.script_references()
    strict_caps = parse_strict_caps()
    evolutions = parse_evolutions()
    incoming, outgoing = stage_graph(evolutions)
    minimum_levels, minimum_paths = minimum_legal_levels(evolutions)
    mega_forms = {
        edge.target for edge in evolutions if edge.method in MEGA_METHODS
    }
    mega_items = {
        edge.parameter
        for edge in evolutions
        if edge.method == "EVO_MEGA_EVOLUTION" and edge.parameter.startswith("ITEM_")
    }
    annotations = parse_showcase_annotations()
    bracelet_grants = parse_bracelet_access()
    first_bracelet = bracelet_grants[0]
    mega_path_problems, mega_path_evidence = validate_first_mega_path(
        evolutions, first_bracelet, strict_caps
    )

    configuration_problems = list(mega_path_problems)
    for (encounter_id, species), (kind, _) in annotations.items():
        if encounter_id not in designs:
            configuration_problems.append(
                f"showcase annotation names unknown encounter {encounter_id}"
            )
        if kind not in SHOWCASE_KINDS:
            configuration_problems.append(
                f"{encounter_id}/{species}: unsupported showcase kind {kind}"
            )

    audited = []
    for encounter_id, design in designs.items():
        if design.get("status") != "closed":
            continue
        state = encounter_campaign_state(
            design.get("trainer_ids", []), references, strict_caps, design.get("strict_cap")
        )
        # The strict young-stage gate applies until the first reachable Mega
        # Bracelet grant, including the cap-20 Route 116/Dewford chapter.
        documented_mega_access = design.get("evolution_stage_fit", {}).get("mega_access")
        before_bracelet = (
            not documented_mega_access
            if isinstance(documented_mega_access, bool)
            else (
                state["earliest"]["badge"], state["earliest"]["rank"]
            ) < (first_bracelet["badge"], first_bracelet["rank"])
        )
        if not before_bracelet:
            continue
        audited.append((encounter_id, design, state))

    if not audited:
        raise ValueError("no closed pre-Mega-access bespoke encounters were reachable")

    results = []
    for encounter_id, design, state in audited:
        cap = state["earliest"]["cap"]
        variants = party_variants(
            encounter_id,
            design["trainer_ids"],
            trainer_blocks,
            parties_text,
        )
        problems = []
        variant_summaries = []
        encountered_species = set()

        for variant_number, party in enumerate(variants, 1):
            eligible = []
            evolved = []
            for mon in party:
                species = mon["species"]
                encountered_species.add(species)
                stage = species_stage(species, incoming, outgoing)
                effective_level = max(1, min(100, cap + mon["level"]))
                legal_minimum = minimum_levels.get(species, 1)
                if stage in {"first-stage", "single-stage"}:
                    eligible.append(species)
                else:
                    evolved.append(species)

                if mon["item"] in mega_items:
                    problems.append(
                        f"variant {variant_number}: {species} holds pre-access Mega item "
                        f"{mon['item']}"
                    )
                if species in mega_forms:
                    problems.append(
                        f"variant {variant_number}: pre-access Mega form {species} is used directly"
                    )
                if stage == "evolved" and effective_level < legal_minimum:
                    problems.append(
                        f"variant {variant_number}: {species} is effectively Lv{effective_level} "
                        f"but its earliest legal family path requires Lv{legal_minimum} "
                        f"({describe_path(minimum_paths.get(species, []))})"
                    )

            if state["earliest"]["badge"] == 0:
                required = math.ceil(len(party) * 0.75)
                if len(eligible) < required:
                    problems.append(
                        f"variant {variant_number}: only {len(eligible)}/{len(party)} are "
                        f"first-stage or truly single-stage; requires at least "
                        f"{required}/{len(party)} (evolved: {', '.join(evolved)})"
                    )
            variant_summaries.append(
                f"{len(eligible)}/{len(party)} stage-eligible at "
                f"Lv{min(max(1, cap + mon['level']) for mon in party)}-"
                f"{max(min(100, cap + mon['level']) for mon in party)}"
            )

        for (annotated_encounter, species), (kind, _) in annotations.items():
            if annotated_encounter != encounter_id:
                continue
            if species not in encountered_species:
                configuration_problems.append(
                    f"{encounter_id}: showcase annotation species {species} is not in the party"
                )
            elif species_stage(species, incoming, outgoing) != "single-stage":
                configuration_problems.append(
                    f"{encounter_id}: showcase {species} is not truly single-stage in evolution.h"
                )

        results.append(
            {
                "encounter": encounter_id,
                "map": state["earliest"]["map"],
                "cap": cap,
                "variants": len(variants),
                "summary": sorted(set(variant_summaries)),
                "problems": list(dict.fromkeys(problems)),
            }
        )

    print("Verdant evolution-stage progression gate")
    print(
        f"Source facts: {len(evolutions)} evolution edges; strict caps {strict_caps}; "
        f"{len(mega_items)} Mega items; {len(mega_forms)} Mega forms"
    )
    print(
        "Earliest player Mega access: "
        f"{first_bracelet['map']} (badge index {first_bracelet['badge']}, "
        f"cap {strict_caps[first_bracelet['badge']]}) at "
        f"{first_bracelet['file']}:{first_bracelet['line']}"
    )
    mega_path_status = "FAIL" if mega_path_problems else "PASS"
    print(f"{mega_path_status} first usable Mega path:")
    for fact in mega_path_evidence:
        print(f"  - {fact}")
    for problem in mega_path_problems:
        print(f"  - {problem}")
    print(f"Audited {len(results)} closed, reachable pre-Mega-access bespoke encounters.\n")

    for result in results:
        status = "FAIL" if result["problems"] else "PASS"
        summaries = "; ".join(result["summary"])
        print(
            f"{status} {result['encounter']} — {result['map']}, cap {result['cap']}, "
            f"{result['variants']} source-valid variant(s): {summaries}"
        )
        for problem in result["problems"]:
            print(f"  - {problem}")

    if annotations:
        print("\nDocumented early rare/single-stage showcases:")
        for (encounter, species), (kind, rationale) in sorted(annotations.items()):
            print(f"  - {encounter}: {species} [{kind}] — {rationale}")

    all_problems = configuration_problems + [
        problem for result in results for problem in result["problems"]
    ]
    print()
    if all_problems:
        if configuration_problems:
            print("Policy configuration problems:")
            for problem in configuration_problems:
                print(f"  - {problem}")
        failed = [result["encounter"] for result in results if result["problems"]]
        print(
            f"FAIL: {len(failed)}/{len(results)} encounters violate the early-stage gate: "
            + ", ".join(failed)
        )
        raise SystemExit(1)

    print(f"PASS: all {len(results)} early bespoke encounters satisfy evolution progression")


if __name__ == "__main__":
    main()
