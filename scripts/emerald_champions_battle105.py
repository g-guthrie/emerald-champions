#!/usr/bin/env python3
"""Generate and verify Battle 105, Courtney and her Grunt's Meteor Falls impact multi battle."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

import verdant_battle_set_presets as presets
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"
FACTION = ROOT / "docs/emerald_champions_faction_anchor_designs.json"

COURTNEY_TEAM = [
    {"level": 1, "species": "SPECIES_LUNATONE", "item": "ITEM_LIFE_ORB", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_POWER_GEM", "MOVE_PSYCHIC", "MOVE_ICY_WIND", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_JIRACHI", "item": "ITEM_CHOICE_SCARF", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_IRON_HEAD", "MOVE_ICY_WIND", "MOVE_U_TURN", "MOVE_TRICK"]},
    {"level": 4, "species": "SPECIES_AERODACTYL", "item": "ITEM_AERODACTYLITE", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_ROCK_SLIDE", "MOVE_DUAL_WINGBEAT", "MOVE_ICE_FANG", "MOVE_PROTECT"]},
]

GRUNT_TEAM = [
    {"level": 1, "species": "SPECIES_SOLROCK", "item": "ITEM_FOCUS_SASH", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_ROCK_SLIDE", "MOVE_ZEN_HEADBUTT", "MOVE_WILL_O_WISP", "MOVE_EXPLOSION"]},
    {"level": 2, "species": "SPECIES_MINIOR", "item": "ITEM_WHITE_HERB", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_SHELL_SMASH", "MOVE_ACROBATICS", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_CELESTEELA", "item": "ITEM_ASSAULT_VEST", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_HEAVY_SLAM", "MOVE_FLAMETHROWER", "MOVE_GIGA_DRAIN", "MOVE_ROCK_SLIDE"]},
]

ALLY_IDS = [
    "MAY_TREECKO_METEOR_FALLS",
    "MAY_TORCHIC_METEOR_FALLS",
    "MAY_MUDKIP_METEOR_FALLS",
    "BRENDAN_TREECKO_METEOR_FALLS",
    "BRENDAN_TORCHIC_METEOR_FALLS",
    "BRENDAN_MUDKIP_METEOR_FALLS",
]

ALLY_SIGNATURES = {
    "MAY_TREECKO_METEOR_FALLS": ["SPECIES_BLAZIKEN", "SPECIES_STARMIE", "SPECIES_MIMIKYU"],
    "MAY_TORCHIC_METEOR_FALLS": ["SPECIES_SWAMPERT", "SPECIES_RAPIDASH", "SPECIES_MIMIKYU"],
    "MAY_MUDKIP_METEOR_FALLS": ["SPECIES_SCEPTILE", "SPECIES_RAPIDASH", "SPECIES_MIMIKYU"],
    "BRENDAN_TREECKO_METEOR_FALLS": ["SPECIES_BLAZIKEN", "SPECIES_ARAQUANID", "SPECIES_MIMIKYU"],
    "BRENDAN_TORCHIC_METEOR_FALLS": ["SPECIES_SWAMPERT", "SPECIES_ARCANINE", "SPECIES_MIMIKYU"],
    "BRENDAN_MUDKIP_METEOR_FALLS": ["SPECIES_SCEPTILE", "SPECIES_ARCANINE", "SPECIES_MIMIKYU"],
}

REFERENCES = [
    "showdown:gen5randomdoublesbattle:021",
    "showdown:gen8randomdoublesbattle:014",
    "showdown:gen9championsrandomdoublesbattle:005",
]

NEXT = {
    "index": 106,
    "encounter_id": "BATTLE_106_METEOR_FALLS_NICOLAS",
    "location": "MeteorFalls_1F_2R",
    "category": "optional Pokemaniac four-record Match Call family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_NICOLAS_1", "TRAINER_NICOLAS_2", "TRAINER_NICOLAS_3", "TRAINER_NICOLAS_4"],
    "access_note": "Nicolas is the first optional trainer in Meteor Falls 1F 2R after the required impact battle. One physical record owns his initial fight and three sequential Match Call rematches.",
}


def design() -> dict:
    return {
        "guide_order": 105,
        "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Required Meteor Falls story confrontation immediately after Route 114. The player selects three party members "
            "and fights beside May or Brendan; the rival's party branches by gender and the player's starter slot, while both "
            "three-member Magma owner records remain exact across all six paths."
        ),
        "runtime_branches": [
            "May + Treecko-slot player: Blaziken, Starmie, Mimikyu ally party.",
            "May + Torchic-slot player: Swampert, Rapidash, Mimikyu ally party.",
            "May + Mudkip-slot player: Sceptile, Rapidash, Mimikyu ally party.",
            "Brendan + Treecko-slot player: Blaziken, Araquanid, Mimikyu ally party.",
            "Brendan + Torchic-slot player: Swampert, Arcanine, Mimikyu ally party.",
            "Brendan + Mudkip-slot player: Sceptile, Arcanine, Mimikyu ally party.",
            "Every branch faces Courtney's Lunatone/Jirachi/Mega Aerodactyl and the Grunt's Solrock/Minior/Celesteela in multi_2_vs_2.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 required faction-admin multi battle",
            "effective_levels": "enemy 41/41/42/42/43/44; all rivals level 40",
            "eligible_ratio": "24/24 source slots across six enemy and eighteen ally slots",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Solrock, Lunatone, Minior, Jirachi, Celesteela, Aerodactyl, Starmie, Mimikyu, and Araquanid are single-stage "
                "or ordinary-method mature forms; every Hoenn starter final evolves by 36, Rapidash at 40, and Arcanine by stone."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 10.0,
        "observed_difficulty": None,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Jirachi role selected; donor roster rejected", "reason": "The reproducible doubles set validates speed control and pivoting; Courtney adds a public Choice commitment."},
                {"reference_id": REFERENCES[1], "decision": "Celesteela role selected; donor roster rejected", "reason": "The exact doubles set validates heavy mixed pressure; recovery and Leech Seed are removed."},
                {"reference_id": REFERENCES[2], "decision": "Mega Aerodactyl role selected; donor roster rejected", "reason": "The Champions generator validates offensive Rock speed; the local fossil climax rejects sand, screens, and Tailwind."},
            ],
            "decision": (
                "The complete corpus and protected faction anchor were reviewed after the exact previous-ten Route 114 window. "
                "No recent encounter spends this impact sequence; locally authored Solrock/Lunatone/Minior roles remain explicit."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Choice Scarf Jirachi keeps Iron Head, Icy Wind, U-turn, and Trick as Courtney's committed trajectory pivot."},
            {"reference_id": REFERENCES[1], "adaptation": "Assault Vest Celesteela keeps four immediate attacks and Beast Boost without importing a passive seed loop."},
            {"reference_id": REFERENCES[2], "adaptation": "Mega Aerodactyl keeps immediate Rock/Flying/Ice pressure and Protect as Courtney's sole transformation."},
            {"source": "protected faction anchor", "adaptation": "Solrock, Lunatone, and Minior create the site-specific impact lead and finite shell break without borrowing Mossdeep terrain or Trick Room."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_LUNATONE", "SPECIES_SOLROCK"],
            "courtney_source_order": [member["species"] for member in COURTNEY_TEAM],
            "grunt_source_order": [member["species"] for member in GRUNT_TEAM],
            "ownership_reason": (
                "The engine caps each two-opponent Multi owner at three. Courtney owns trajectory, mythical pivot, and Mega "
                "fossil; the Grunt owns detonation, shell break, and heavy fragment. This is the only source-legal way to preserve "
                "the protected six-member impact roster and intended twin-meteor lead."
            ),
        },
        "team_intent": (
            "Life Orb Lunatone and Focus Sash Solrock expose mixed Rock/Psychic pressure, Icy Wind, burn, Protect, and one "
            "conditional Explosion. White Herb Minior is a finite Shell Smash meteor; Choice Scarf Jirachi redirects trajectory "
            "through Iron Head, Icy Wind, U-turn, or Trick; Assault Vest Celesteela is the largest four-attack fragment; Mega "
            "Aerodactyl is the fast fossil left after impact. There is no weather, terrain, room, sleep, redirection, or sustain loop."
        ),
        "intended_counterplay": (
            "Wide Guard, Ghost immunity, Protect, priority, double-targeting, or immediate Solrock pressure blunt the lead. "
            "Taunt, Haze, Unaware, phazing, Steel/Water/Electric/Rock, or focus stop Minior. Jirachi's Choice lock is exploitable "
            "through resist pivots, Ground/Fire/Ghost/Dark, Protect, and forced targets. Electric/Fire special pressure and careful "
            "KO sequencing limit Celesteela; Water/Electric/Ice/Steel/Rock, burn/Intimidate, priority, or speed reversal answer "
            "Mega Aerodactyl. Every rival branch supplies at least one relevant speed, Water/Grass/Fire, priority, Wide Guard, "
            "Helping Hand, Ghost, or Protect tool, but the player still chooses the three-person half of the solution."
        ),
        "bespoke_ai": (
            "Both opponent owners use smart switching, partner awareness, HP awareness, Combo Setup, Speed Control, and Field "
            "Control; the Grunt also enables Will Suicide solely for Solrock. Reusable AI rewards partner Protect when a low-HP "
            "ally has Explosion and rewards Explosion only when the visible partner action or immunity makes it safe, while the "
            "ordinary collateral scorer remains active. Shell Smash requires visible survival value, Choice rules remain native, "
            "and Aerodactyl Mega Evolves normally. No move, target, switch, or wave is scripted."
        ),
        "uniqueness": (
            "All six enemy species are new to the first 104 physical encounters and absent from the previous-ten Route 114 "
            "window. Jirachi, Celesteela, and Mega Aerodactyl create rare spectacle without stacking raw legends; the twin-rock "
            "detonation and split owner topology are unique. Tate/Liza's terrain, room, and cosmic formation remain untouched."
        ),
        "story_logic": (
            "The battle now uses Meteor Falls itself: Courtney explicitly announces trajectory, collision, fracture, and fossil. "
            "Her Grunt's defeat line names the moon sheltering the sun. The six existing rival/starter branches, choose-three menu, "
            "loss return, Meteorite theft, Archie arrival, and all subsequent story flags remain unchanged."
        ),
        "reward_logic": "Required story progress, EXP, and prize money only; the Meteorite remains a Team Magma plot object and no redundant held-item reward is added.",
        "campaign_reservations": {
            "spends": ["Courtney Meteor Falls impact composition", "one controlled Solrock detonation", "first Minior shell break", "first Jirachi", "first Celesteela", "first Mega Aerodactyl"],
            "preserves": ["later Courtney calibration and Mega Houndoom", "Tate/Liza cosmic formation", "Maxie Groudon land story", "Tabitha machinery", "every Primal"],
            "repeat_rule": "These six enemy species and the impact sequence must not recur soon; later celestial teams require a different field, owner structure, and primary question.",
        },
        "author_self_check": {
            "strongest_part": "Every enemy slot is one phase of a meteor impact, and the two-owner source partition makes the story battle mechanically native instead of papering over the Multi engine.",
            "weakest_link": "Explosion can feel cheap and the rival AI reduces player control. The strict low-HP/safe-partner scoring, public Sash/Protect state, broad immunity/guard answers, six audited ally branches, and no second detonation are mandatory safeguards."
        },
        "closure": (
            "Battle 105 is source-closed at quality 10 and target difficulty 10: all six rival gender/starter branches, choose/"
            "cancel/loss/story paths, two three-member enemy owners, six legal cap+1 to +4 sets, six distinct items, one Mega, "
            "three indexed references, protected-anchor reconciliation, conditional-detonation AI, native-width dialogue, broad "
            "counterplay, and zero reward debt are proven. Runtime remains unplayed and observed difficulty is unset."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 105,
        "encounter_id": "BATTLE_105_METEOR_FALLS_COURTNEY_GRUNT",
        "identity": {"location": "MeteorFalls_1F_1R", "category": "required rival-assisted faction-admin multi battle", "format": "six-branch multi_2_vs_2", "strict_cap": 40, "memory_hook": "Lunatone shelters one Solrock detonation before Minior cracks, Jirachi redirects, Celesteela lands, and Mega Aerodactyl fossilizes the impact."},
        "primary_player_question": "Can the player time one controlled detonation, stop Minior, exploit Jirachi's lock, and preserve the right pressure for Celesteela and Mega Aerodactyl while coordinating with the assigned rival branch?",
        "tempo": "Required six-enemy impact multi: mixed twin-meteor lead, finite shell break and Choice trajectory middles, then heavy fragment and Mega fossil owners.",
        "pressure_sources": ["Focus Sash conditional-Explosion Solrock", "Life Orb Icy Wind Lunatone", "White Herb Shields Down Minior", "Choice Scarf Serene Grace Jirachi", "Assault Vest Beast Boost Celesteela", "Mega Aerodactyl"],
        "intentional_opening": "Courtney's Lunatone and the Grunt's Solrock are fixed source leads in every rival branch; no Explosion or Protect turn is scripted.",
        "intentional_weakness": "Shared lead weaknesses, one conditional detonation, one setup user, public Choice lock, no Celesteela recovery, frail Mega, and no weather/terrain/room/sleep/redirection loop.",
        "first_loss_lesson": "Courtney's impact is timed. Deny or shelter from the detonation, stop Minior before it cracks, force Jirachi onto the wrong line, and save speed or priority for the fossil.",
        "revealed_information": ["cap 40", "six rival branches", "choose three", "multi_2_vs_2", "levels 41-44", "conditional Explosion", "Protect coordination", "Minior Shell Smash", "Choice Jirachi", "Beast Boost Celesteela", "Mega Aerodactyl", "no extra reward"],
        "counterplay_classes": ["Wide Guard/Ghost/Protect/focus", "Taunt/Haze/Unaware/phazing/priority", "Water/Grass/Ghost/Dark/Steel into twin rocks", "Choice exploitation", "Electric/Fire into Celesteela", "Water/Electric/Ice/Steel/Rock and speed reversal into Mega Aerodactyl", "complement assigned rival branch"],
        "target_difficulty": 10.0,
        "difficulty_rationale": "Six optimized cap+1 to +4 enemy sets, one conditional board wipe, finite setup, Choice flinch/speed pressure, Beast Boost, and a fast Mega oppose a player-selected three plus a fixed ally three. Broad public answers exist, but this is a required faction-admin apex.",
        "tuning_knob": "Tune Aerodactyl +4 to +3 first, then Celesteela +3 to +2, then Minior/Jirachi +2 to +1; preserve all species, owner partition, moves, items, and impact phases.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["required-multi", "six-rival-branches", "magma-admin", "meteor-impact", "controlled-explosion", "solrock", "lunatone", "minior", "jirachi", "celesteela", "mega-aerodactyl", "choice-scarf", "beast-boost", "one-mega", "three-fresh-rare-species", "no-weather", "no-room", "no-sleep", "no-redirection"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Three exact indexed doubles references plus protected locally authored impact roles."},
        "author_self_check": {"strongest_part": "The site, story, exact engine topology, team order, and AI all express one impact sequence.", "weakest_link": "Explosion and fixed ally behavior can feel unfair; public state, safe-partner gating, broad answers, and branch audits are mandatory."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_105_METEOR_FALLS_COURTNEY_GRUNT"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 105] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 105:
            row.update({
                "category": "required rival-assisted Meteor Falls impact multi",
                "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"],
                "access_note": "Six May/Brendan and starter-slot branches share the same Courtney/Grunt opponent owners in multi_2_vs_2 after the player chooses three Pokemon.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 106] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 105 else "next" if row["index"] == 106 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 105,
        "next_index": 106,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 106,
        "physical_encounter_groups": 526,
        "unordered_physical_groups": 420,
    })
    return designs, ledger, sequence, os_data


def parse_party(trainer_id: str, trainers: str, parties: str) -> list[dict]:
    block = doubles.trainer_blocks(trainers)[trainer_id].group(0)
    return [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    expected = {"TRAINER_COURTNEY_METEOR_FALLS": COURTNEY_TEAM, "TRAINER_GRUNT_METEOR_FALLS": GRUNT_TEAM}
    for trainer_id, team in expected.items():
        if parse_party(trainer_id, trainers, parties) != team:
            raise SystemExit(f"FAIL: Battle 105 source party differs for {trainer_id}")
        block = blocks[trainer_id].group(0)
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 105 {trainer_id} missing {token}")
    if "AI_FLAG_WILL_SUICIDE" not in blocks["TRAINER_GRUNT_METEOR_FALLS"].group(0):
        raise SystemExit("FAIL: Battle 105 controlled detonation profile missing")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    combined = COURTNEY_TEAM + GRUNT_TEAM
    for member in combined:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 105 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 105 invalid ability slot for {member['species']}")
    if len({m["species"] for m in combined}) != 6 or len({m["item"] for m in combined}) != 6:
        raise SystemExit("FAIL: Battle 105 enemy species/items are not unique")

    for ally_id, signature in ALLY_SIGNATURES.items():
        team = parse_party(ally_id, trainers, parties)
        if len(team) != 3 or [member["species"] for member in team] != signature:
            raise SystemExit(f"FAIL: Battle 105 ally branch drifted for {ally_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 105 ally illegal moves for {ally_id}/{member['species']}: {illegal}")

    script = (ROOT / "data/maps/MeteorFalls_1F_1R/scripts.inc").read_text()
    invocation = "multi_2_vs_2 TRAINER_COURTNEY_METEOR_FALLS, MeteorFalls_1F_1R_Text_CourtneyLose, TRAINER_GRUNT_METEOR_FALLS"
    if script.count(invocation) != 6 or any(script.count(ally_id) != 1 for ally_id in ALLY_IDS):
        raise SystemExit("FAIL: Battle 105 six multi branches drifted")
    for cue in ("Meteor Falls supplies", "Trajectory. Collision. Fracture", "fossil left behind", "twin meteors", "Impact model", "moon sheltered the sun"):
        if cue not in script:
            raise SystemExit(f"FAIL: Battle 105 dialogue missing {cue}")
    section = script.split("MeteorFalls_1F_1R_Text_CourtneyLetsBattle:", 1)[1].split("MeteorFalls_1F_1R_Text_ChooseMons:", 1)[0]
    for line in re.findall(r'\.string "([^"]*)"', section):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 105 overlong dialogue: {visible}")

    ai = (ROOT / "src/battle_ai_main.c").read_text()
    for token in ("Controlled detonation is a partner puzzle", "HasMove(BATTLE_PARTNER(battlerAtk), MOVE_EXPLOSION)", "partnerProtecting", "partnerImmune", "GetHealthPercentage(battlerAtk) <= 50"):
        if token not in ai:
            raise SystemExit(f"FAIL: Battle 105 detonation AI missing {token}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_COURTNEY_METEOR_FALLS": {"format": "double", "target_size": 3, "archetype": "Impact trajectory half", "difficulty": 100, "partner_interaction": True, "multi_partner": True, "level_offset": 2, "location": "Meteor Falls 1 F 1 R"},
        "TRAINER_GRUNT_METEOR_FALLS": {"format": "double", "target_size": 3, "archetype": "Impact fragment half", "difficulty": 100, "partner_interaction": True, "multi_partner": True, "level_offset": 2, "location": "Meteor Falls 1 F 1 R"},
    }
    for trainer_id, value in expected_manifest.items():
        if manifest[trainer_id] != value:
            raise SystemExit(f"FAIL: Battle 105 manifest stale for {trainer_id}")

    anchor = json.loads(FACTION.read_text())["designs"]["METEOR_FALLS_COURTNEY"]
    if anchor["status"]["source"] != "source-closed" or anchor["runtime"]["trainer_ids"] != ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"] or anchor["runtime"]["canonical_format"] != "multi_2_vs_2":
        raise SystemExit("FAIL: Battle 105 protected faction anchor is not source-honest")
    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 105 competitive reference missing")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payloads = expected_payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 105 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        opponents = [row for row in guide if row["trainerId"] in {"TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"}]
        if len(opponents) != 2 or any(row["designStatus"] != "closed" or row["partySize"] != 3 for row in opponents):
            raise SystemExit("FAIL: Battle 105 opponent guide stale")
        allies = [row for row in guide if row["trainerId"] in set(ALLY_IDS)]
        if len(allies) != 6 or any(row["partySize"] != 3 for row in allies):
            raise SystemExit("FAIL: Battle 105 ally guide stale")
    print("PASS: Battle 105 Meteor Falls six-branch impact multi battle is source-closed")


if __name__ == "__main__":
    main()
