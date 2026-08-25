#!/usr/bin/env python3
"""Apply and verify Verdant's explicit trainer-by-trainer team designs."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]
TRAINERS_PATH = ROOT / "src/data/trainers.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"
FORMAT_MANIFEST_PATH = ROOT / "docs/verdant_doubles_manifest.json"
TEAM_MANIFEST_PATH = ROOT / "docs/verdant_custom_team_plans.json"


LEGENDARY_FAMILIES = (
    "ARTICUNO", "ZAPDOS", "MOLTRES", "MEWTWO", "MEW",
    "RAIKOU", "ENTEI", "SUICUNE", "LUGIA", "HO_OH", "CELEBI",
    "REGIROCK", "REGICE", "REGISTEEL", "LATIAS", "LATIOS", "KYOGRE", "GROUDON", "RAYQUAZA", "JIRACHI", "DEOXYS",
    "UXIE", "MESPRIT", "AZELF", "DIALGA", "PALKIA", "HEATRAN", "REGIGIGAS", "GIRATINA", "CRESSELIA", "PHIONE", "MANAPHY", "DARKRAI", "SHAYMIN", "ARCEUS",
    "VICTINI", "COBALION", "TERRAKION", "VIRIZION", "TORNADUS", "THUNDURUS", "RESHIRAM", "ZEKROM", "LANDORUS", "KYUREM", "KELDEO", "MELOETTA", "GENESECT",
    "XERNEAS", "YVELTAL", "ZYGARDE", "DIANCIE", "HOOPA", "VOLCANION",
    "TYPE_NULL", "SILVALLY", "TAPU_KOKO", "TAPU_LELE", "TAPU_BULU", "TAPU_FINI", "COSMOG", "COSMOEM", "SOLGALEO", "LUNALA",
    "NIHILEGO", "BUZZWOLE", "PHEROMOSA", "XURKITREE", "CELESTEELA", "KARTANA", "GUZZLORD", "NECROZMA", "MAGEARNA", "MARSHADOW", "POIPOLE", "NAGANADEL", "STAKATAKA", "BLACEPHALON", "ZERAORA", "MELTAN", "MELMETAL",
    "ZACIAN", "ZAMAZENTA", "ETERNATUS", "KUBFU", "URSHIFU", "ZARUDE", "REGIELEKI", "REGIDRAGO", "GLASTRIER", "SPECTRIER", "CALYREX",
)

# These are deliberate same-species gimmick fights from Inclement's authored
# roster (Magikarp showcase, mono-Golem hiker, twins, and escalating rematches),
# not accidental custom-plan repetition.
INTENTIONAL_DUPLICATE_PARTIES = {
    "TRAINER_DARRIN", "TRAINER_NOB_5", "TRAINER_BERNIE_5", "TRAINER_JEFFREY_5",
    "TRAINER_COLTON", "TRAINER_TIMOTHY_5", "TRAINER_RONALD", "TRAINER_NICOLAS_5",
    "TRAINER_CYNDY_5", "TRAINER_MAGIKARP_GUY", "TRAINER_TRENT_1", "TRAINER_TRENT_2",
    "TRAINER_TRENT_3", "TRAINER_TRENT_4", "TRAINER_TORI_AND_TIA", "TRAINER_ANDRES_5",
    "TRAINER_PABLO_5", "TRAINER_KOJI_5", "TRAINER_FERNANDO_5", "TRAINER_SAWYER_5",
    "TRAINER_THALIA_4",
}

AI_PROFILES = {
    "AI_FLAG_SETUP_FIRST_TURN": {
        "TRAINER_DARIAN",
        "TRAINER_GRUNT_PETALBURG_WOODS",
        "TRAINER_HALEY_1",
        "TRAINER_RICK",
        "TRAINER_TIANA",
    },
    "AI_FLAG_HP_AWARE": {
        "TRAINER_DARIAN",
        "TRAINER_HALEY_1",
        "TRAINER_LYLE",
    },
    "AI_FLAG_WILL_SUICIDE": {
        "TRAINER_LYLE",
    },
    "AI_FLAG_HELP_PARTNER": {
        "MAY_TREECKO_METEOR_FALLS", "MAY_TORCHIC_METEOR_FALLS", "MAY_MUDKIP_METEOR_FALLS",
        "BRENDAN_TREECKO_METEOR_FALLS", "BRENDAN_TORCHIC_METEOR_FALLS", "BRENDAN_MUDKIP_METEOR_FALLS",
    },
    "AI_FLAG_PERISH_TRAP": {
        "TRAINER_JAMES_1",
        "TRAINER_VALERIE_4",
    },
    "AI_FLAG_COMBO_SETUP": {
        "TRAINER_YUJI", "TRAINER_DANIELLE", "TRAINER_KATE_AND_JOY",
        "TRAINER_TABITHA_MAGMA_HIDEOUT", "TRAINER_ANNA_AND_MEG_1",
    },
    "AI_FLAG_SPEED_CONTROL": {
        "TRAINER_ALLEN",
        "TRAINER_CALVIN_1",
        "TRAINER_CINDY_1",
        "TRAINER_DARIAN",
        "TRAINER_GRUNT_PETALBURG_WOODS",
        "TRAINER_WINSTON_1",
        "TRAINER_ROXANNE_1", "TRAINER_BRAWLY_1", "TRAINER_FLANNERY_1",
        "TRAINER_WINONA_1", "TRAINER_TATE_AND_LIZA_1", "TRAINER_JUAN_1",
        "TRAINER_SIDNEY", "TRAINER_PHOEBE", "TRAINER_GLACIA", "TRAINER_WALLACE",
        "TRAINER_SYLVIA", "TRAINER_VIRGIL", "TRAINER_NICHOLAS", "TRAINER_MAURA",
        "TRAINER_YUJI", "TRAINER_BETH", "TRAINER_VALERIE_4",
        "MAY_TREECKO_METEOR_FALLS", "MAY_TORCHIC_METEOR_FALLS", "MAY_MUDKIP_METEOR_FALLS",
        "BRENDAN_TREECKO_METEOR_FALLS", "BRENDAN_TORCHIC_METEOR_FALLS", "BRENDAN_MUDKIP_METEOR_FALLS",
    },
    "AI_FLAG_FIELD_CONTROL": {
        "TRAINER_LYLE",
        "TRAINER_TIANA",
        "TRAINER_ROXANNE_1", "TRAINER_WATTSON_1", "TRAINER_FLANNERY_1",
        "TRAINER_TATE_AND_LIZA_1", "TRAINER_JUAN_1",
        "TRAINER_GLACIA", "TRAINER_WALLACE", "TRAINER_SAWYER_1", "TRAINER_BETH",
        "TRAINER_SHAYLA", "TRAINER_CLIFFORD", "TRAINER_NICHOLAS", "TRAINER_LENNY",
        "TRAINER_LINDA", "TRAINER_TABITHA_MAGMA_HIDEOUT", "TRAINER_MAXIE_MT_CHIMNEY",
        "TRAINER_MAXIE_MOSSDEEP", "TRAINER_ARCHIE_SLATEPORT",
    },
}


def party_entries(body: str) -> list[str]:
    entries: list[str] = []
    depth = 0
    start = None
    state = "code"
    i = 0
    while i < len(body):
        if state == "code" and body.startswith("/*", i):
            state = "block"
            i += 2
            continue
        if state == "code" and body.startswith("//", i):
            state = "line"
            i += 2
            continue
        if state == "block":
            if body.startswith("*/", i):
                state = "code"
                i += 2
                continue
            i += 1
            continue
        if state == "line":
            if body[i] == "\n":
                state = "code"
            i += 1
            continue
        if body[i] == "{":
            if depth == 0:
                start = i
            depth += 1
        elif body[i] == "}":
            depth -= 1
            if depth == 0 and start is not None:
                end = i + 1
                while end < len(body) and body[end] in " \t\r\n,":
                    end += 1
                if body.startswith("/*", end):
                    comment_end = body.find("*/", end + 2)
                    if comment_end >= 0:
                        end = comment_end + 2
                entries.append(body[start:end])
                start = None
        i += 1
    return entries


def parse_species(entry: str) -> str | None:
    match = re.search(r"\.species\s*=\s*(SPECIES_[A-Z0-9_]+)", entry)
    return match.group(1) if match else None


def clean_entry(entry: str) -> str:
    entry = re.sub(r"\s*/\* Verdant (?:doubles|custom):.*?\*/\s*", "", entry, flags=re.S)
    return entry.strip().rstrip(",").rstrip()


def normalize_disabled_entry_commas(text: str) -> str:
    """Remove active separators left after a whole array entry is commented."""
    return re.sub(r"^(\s*},?\s+\*/),\s*$", r"\1", text, flags=re.M)


def level_for_addition(rule: dict) -> int:
    if rule["difficulty"] >= 65:
        return 1
    if rule["difficulty"] >= 55:
        return 0
    return int(rule["level_offset"])


def render_build(build: dict, level: int, note: str) -> str:
    moves = ", ".join(build["moves"])
    return (
        "    {\n"
        f"    .lvl = {level},\n"
        f"    .species = {build['species']},\n"
        f"    .heldItem = {build['item']},\n"
        f"    .ability = {build['ability_slot']},\n"
        f"    .spread = {build['spread']},\n"
        f"    .moves = {moves}\n"
        f"    }} /* Verdant custom: {note} */"
    )


def replace_party_body(parties_text: str, party_name: str, entries: list[str]) -> str:
    match = doubles.party_match(parties_text, party_name)
    new_body = "\n" + ",\n".join(entries) + "\n"
    return parties_text[:match.start(2)] + new_body + parties_text[match.end(2):]


def apply_ai_profiles(trainers_text: str) -> str:
    # Profile-only flags are authoritative.  Clear stale assignments before
    # adding the current sets so removing a trainer from a profile is just as
    # reproducible as adding one. HELP_PARTNER is excluded because the format
    # manifest also assigns it to bosses and story-partner battles.
    profile_only_flags = set(AI_PROFILES) - {"AI_FLAG_HELP_PARTNER"}

    def clear_stale_profiles(match: re.Match) -> str:
        parts = [part.strip() for part in match.group(2).split("|")]
        kept = [part for part in parts if part not in profile_only_flags]
        return match.group(1) + " | ".join(kept)

    trainers_text = re.sub(
        r"(\.aiFlags\s*=\s*)([^,\n]+)",
        clear_stale_profiles,
        trainers_text,
    )
    blocks = doubles.trainer_blocks(trainers_text)
    flags_by_trainer: dict[str, list[str]] = defaultdict(list)
    for flag, trainer_ids in AI_PROFILES.items():
        for trainer_id in trainer_ids:
            flags_by_trainer[trainer_id].append(flag)
    for trainer_id, profile_flags in flags_by_trainer.items():
        match = blocks[trainer_id]
        block = match.group(0)
        ai_match = re.search(r"(\.aiFlags\s*=\s*)([^,\n]+)", block)
        if not ai_match:
            raise ValueError(f"missing aiFlags for {trainer_id}")
        expression = ai_match.group(2).strip()
        for flag in profile_flags:
            if flag not in expression:
                expression += f" | {flag}"
        updated = block[:ai_match.start(2)] + expression + block[ai_match.end(2):]
        trainers_text = trainers_text[:match.start()] + updated + trainers_text[match.end():]
        blocks = doubles.trainer_blocks(trainers_text)
    return trainers_text


def apply() -> None:
    formats = json.loads(FORMAT_MANIFEST_PATH.read_text())
    custom = json.loads(TEAM_MANIFEST_PATH.read_text())
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()

    # The format manifest is authoritative for doubles/singles and AI flags.
    trainers_text = doubles.rewrite_trainers(trainers_text, formats)
    trainers_text = apply_ai_profiles(trainers_text)
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    boss_ids = {boss["trainer_id"] for boss in formats["bosses"]}
    touched = set(custom["plans"]) | set(custom.get("replacements", {})) | set(custom["route_single_trainers"])

    for trainer_id in sorted(touched):
        if trainer_id in boss_ids:
            raise ValueError(f"custom ordinary-team manifest must not replace story boss {trainer_id}")
        block = trainer_blocks[trainer_id].group(0)
        party_name = doubles.party_name(block)
        body = doubles.party_match(parties_text, party_name).group(2)
        # The explicit polish manifest is the final party layer. This keeps
        # --apply idempotent after polish while still refreshing trainer flags.
        if "Verdant polish:" in body:
            continue
        baseline = [
            entry
            for entry in party_entries(body)
            if "Verdant doubles:" not in entry
            and not ("Verdant custom:" in entry and "individual diversity replacement" not in entry)
        ]
        baseline = [clean_entry(entry) for entry in baseline]

        for replacement in custom.get("replacements", {}).get(trainer_id, []):
            index = replacement["index"]
            if index >= len(baseline):
                raise ValueError(f"replacement index out of range: {trainer_id}[{index}]")
            actual = parse_species(baseline[index])
            replacement_species = replacement["build"]["species"]
            if actual not in (replacement["from"], replacement_species):
                raise ValueError(f"replacement source drift: {trainer_id}[{index}] {actual} != {replacement['from']}")
            baseline[index] = render_build(
                replacement["build"],
                int(replacement["build"]["level"]),
                f"individual diversity replacement for {trainer_id}",
            )

        rule = formats["formats"][trainer_id]
        additions = custom.get("plans", {}).get(trainer_id, {}).get("additions", [])
        rendered_additions = [
            render_build(
                build,
                level_for_addition(rule),
                f"{rule['location']} — {build.get('reference', rule['archetype'])}",
            )
            for build in additions
        ]
        parties_text = replace_party_body(parties_text, party_name, baseline + rendered_additions)

    parties_text = normalize_disabled_entry_commas(parties_text)
    TRAINERS_PATH.write_text(trainers_text)
    PARTIES_PATH.write_text(parties_text)
    print(f"applied {len(custom['plans'])} explicit team plans, {len(custom.get('replacements', {}))} diversity replacements, and {len(custom['route_single_trainers'])} route singles")


def trainer_family(trainer_id: str) -> str:
    match = re.fullmatch(r"(MAY|BRENDAN)_(TREECKO|TORCHIC|MUDKIP)_METEOR_FALLS", trainer_id)
    if match:
        return f"{match.group(1)}_METEOR_FALLS_PARTNER"
    trainer_id = re.sub(r"_(MUDKIP|TREECKO|TORCHIC)$", "", trainer_id)
    return re.sub(r"_[1-5]$", "", trainer_id)


def active_species_for_trainer(parties_text: str, trainer_block: str) -> list[str]:
    body = doubles.party_match(parties_text, doubles.party_name(trainer_block)).group(2)
    return [species for entry in party_entries(body) if (species := parse_species(entry))]


def mega_pairs() -> list[tuple[str, str]]:
    text = (ROOT / "src/data/pokemon/evolution.h").read_text()
    pairs = []
    for species, body in re.findall(
        r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{(.*?)(?=^\s*\[SPECIES_|\Z)",
        text,
        re.M | re.S,
    ):
        for item in re.findall(r"\{EVO_MEGA_EVOLUTION,\s*(ITEM_[A-Z0-9_]+),", body):
            pairs.append((species, item))
    return pairs


def check() -> None:
    formats = json.loads(FORMAT_MANIFEST_PATH.read_text())
    custom = json.loads(TEAM_MANIFEST_PATH.read_text())
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    problems = []
    if re.search(r"^\s*},?\s+\*/,\s*$", parties_text, re.M):
        problems.append("disabled trainer party entries retain active separator commas")

    route_singles = set(custom["route_single_trainers"])
    if len(route_singles) != 64:
        problems.append(f"route singles: expected 64, found {len(route_singles)}")
    if len(custom["plans"]) != 374:
        problems.append(f"customized trainer count drifted: {len(custom['plans'])}")

    for flag, trainer_ids in AI_PROFILES.items():
        for trainer_id in trainer_ids:
            if flag not in blocks[trainer_id].group(0):
                problems.append(f"{trainer_id}: missing AI profile {flag}")

    all_species = Counter()
    families_by_species: dict[str, set[str]] = defaultdict(set)
    party_bodies = {}
    mega_items = {item for _, item in mega_pairs()}
    for trainer_id, rule in formats["formats"].items():
        block = blocks[trainer_id].group(0)
        actual_double = ".doubleBattle = TRUE" in block
        expected_double = rule["format"] == "double"
        if actual_double != expected_double:
            problems.append(f"{trainer_id}: format mismatch")
        species = active_species_for_trainer(parties_text, block)
        party_bodies[trainer_id] = doubles.party_match(parties_text, doubles.party_name(block)).group(2)
        party_mega_items = [item for item in mega_items if re.search(rf"\.heldItem\s*=\s*{re.escape(item)}\b", party_bodies[trainer_id])]
        if len(party_mega_items) > 1:
            problems.append(f"{trainer_id}: multiple Mega items {party_mega_items}")
        if expected_double and len(species) != rule["target_size"]:
            problems.append(f"{trainer_id}: doubles size {len(species)} != {rule['target_size']}")
        if expected_double and not rule.get("multi_partner") and (len(species) < 4 or len(species) % 2):
            problems.append(f"{trainer_id}: unsafe doubles size {len(species)}")
        if len(species) != len(set(species)) and trainer_id not in INTENTIONAL_DUPLICATE_PARTIES:
            problems.append(f"{trainer_id}: duplicate species inside party")
        for value in species:
            all_species[value] += 1
            families_by_species[value].add(trainer_family(trainer_id))

    nonboss_ids = set(formats["formats"]) - {boss["trainer_id"] for boss in formats["bosses"]}
    for trainer_id in nonboss_ids:
        if "Verdant doubles:" in party_bodies[trainer_id]:
            problems.append(f"{trainer_id}: legacy module filler remains")

    added = Counter(
        build["species"]
        for plan in custom["plans"].values()
        for build in plan["additions"]
    )
    if sum(added.values()) != 606 or len(added) != 589 or max(added.values()) > 3:
        problems.append(f"custom addition diversity drifted: slots={sum(added.values())}, unique={len(added)}, max={max(added.values())}")
    constants = "\n".join(
        (ROOT / path).read_text()
        for path in (
            "include/constants/species.h", "include/constants/items.h", "include/constants/moves.h",
            "include/constants/spreads.h",
        )
    )
    ability_slots = doubles.base_ability_slots()
    for trainer_id, plan in custom["plans"].items():
        for build in plan["additions"]:
            for constant in (build["species"], build["item"], build["spread"], *build["moves"]):
                if constant not in constants and constant != "ITEM_NONE":
                    problems.append(f"{trainer_id}: unknown custom-team constant {constant}")
            if build["ability_slot"] >= len(ability_slots.get(build["species"], [])):
                problems.append(f"{trainer_id}: invalid ability slot for {build['species']}")
    pairs = Counter(
        tuple(sorted((plan["additions"][0]["species"], plan["additions"][1]["species"])))
        for plan in custom["plans"].values()
        if len(plan["additions"]) >= 2
    )
    if pairs and max(pairs.values()) > 1:
        problems.append("a custom two-Pokémon addition pair repeats")

    overused = {species: len(families) for species, families in families_by_species.items() if len(families) > 12}
    if overused:
        problems.append(f"species exceed twelve unrelated trainer families: {overused}")

    for family in LEGENDARY_FAMILIES:
        prefix = f"SPECIES_{family}"
        if not any(species == prefix or species.startswith(prefix + "_") for species in all_species):
            problems.append(f"legendary family never appears: {family}")

    for species, item in mega_pairs():
        if not any(
            re.search(rf"\.species\s*=\s*{re.escape(species)}\b", body)
            and re.search(rf"\.heldItem\s*=\s*{re.escape(item)}\b", body)
            for body in party_bodies.values()
        ):
            problems.append(f"Mega never appears on a trainer: {species} + {item}")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    doubles_count = sum(rule["format"] == "double" for rule in formats["formats"].values())
    print(f"PASS: {len(formats['formats'])} trainers; {doubles_count} doubles and {len(formats['formats']) - doubles_count} intentional singles")
    print("PASS: 374 explicit custom plans, 606 slots, 589 added species, no repeated pair")
    print("PASS: every legendary family and every Mega evolution appears on an opponent")
    print("PASS: no species spans more than twelve unrelated trainer families")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.apply and not args.check:
        parser.error("choose --apply or --check")
    if args.apply:
        apply()
    if args.check:
        check()


if __name__ == "__main__":
    main()
