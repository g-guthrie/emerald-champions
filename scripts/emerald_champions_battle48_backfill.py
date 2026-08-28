#!/usr/bin/env python3
"""Generate/check the story-correct Slateport Archie Battle 48 closure records."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish

ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
RESERVATIONS = ROOT / "docs/verdant_historic_team_reservations.json"

TEAM = [
    {"level": 1, "species": "SPECIES_LIEPARD", "item": "ITEM_DAMP_ROCK", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPEED_TIMID", "moves": ["MOVE_RAIN_DANCE", "MOVE_FAKE_OUT", "MOVE_ENCORE", "MOVE_FOUL_PLAY"]},
    {"level": 1, "species": "SPECIES_HITMONTOP", "item": "ITEM_EJECT_BUTTON", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_FAKE_OUT", "MOVE_WIDE_GUARD", "MOVE_FEINT", "MOVE_CLOSE_COMBAT"]},
    {"level": 2, "species": "SPECIES_MANAPHY", "item": "ITEM_LIFE_ORB", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_ENERGY_BALL", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_KINGDRA", "item": "ITEM_MYSTIC_WATER", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_SCALD", "MOVE_DRACO_METEOR", "MOVE_ICY_WIND", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_QWILFISH", "item": "ITEM_BLACK_SLUDGE", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_WATERFALL", "MOVE_POISON_JAB", "MOVE_THUNDER_WAVE", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_MALAMAR", "item": "ITEM_MALAMARITE", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_SUPERPOWER", "MOVE_PSYCHO_CUT", "MOVE_KNOCK_OFF", "MOVE_PROTECT"]},
]

REFERENCES = [
    ("showdown:gen6randomdoublesbattle:018", "Prankster Liepard is adapted into manual Rain Dance, Fake Out, Encore, and Foul Play heist control."),
    ("elite:wolfe:worlds-2016", "World Champion Hitmontop supplies Eject Button, Fake Out, Wide Guard, Feint, and Close Combat support without importing Kyogre."),
    ("showdown:gen6randomdoublesbattle:009", "Manaphy supplies a rare immediate special attacker with no Tail Glow setup."),
    ("elite:wolfe:toronto-2024", "Kingdra supplies primary rain offense while Perish, Shadow Tag, redirection, sleep, and Trick Room stay rejected."),
    ("showdown:gen9championsrandomdoublesbattle:011", "Qwilfish supplies Intimidate, paralysis, and physical pursuit control without hazards."),
    ("showdown:gen9championsrandomdoublesbattle:002", "Mega Malamar retains Contrary Superpower, Psycho Cut, Knock Off, and Protect without a dual-speed shell."),
]


def design():
    refs = [{"reference_id": reference_id, "adaptation": adaptation} for reference_id, adaptation in REFERENCES]
    return {
        "guide_order": 48,
        "trainer_ids": ["TRAINER_ARCHIE_SLATEPORT"],
        "status": "closed",
        "campaign_point": "First required Archie boss immediately after both Museum Grunts; the native script fully restores HP, PP, status, and fainted party members before the battle and again afterward",
        "strict_cap": 30,
        "evolution_stage_fit": {
            "campaign_phase": "first healed Aqua leader-grade tactical Mega boss",
            "effective_levels": "31-34",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Liepard, Hitmontop, Kingdra, Qwilfish, and Malamar are naturally obtainable fully evolved or single-stage threats by these levels under boss rules; Manaphy is a deliberate rare mythical. Mega Malamar is allowed after the Granite Cave bracelet. Kyogre and Blue Orb are absent because the story has not awakened Kyogre.",
        },
        "manual_quality": 10,
        "manual_difficulty": 10,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [{"reference_id": reference_id, "decision": "selected role; full donor rejected", "reason": adaptation} for reference_id, adaptation in REFERENCES],
            "decision": "Six independent competitive records support one early tactical heist crew. Manual rain is vulnerable and branch-neutral; support is layered but public; Manaphy is the rare prize; Kingdra and Qwilfish control the escape; Mega Malamar is the only transformation.",
        },
        "competitive_references": refs,
        "ordering": {
            "intended_lead": ["SPECIES_LIEPARD", "SPECIES_HITMONTOP"],
            "source_order": [entry["species"] for entry in TEAM],
            "reason": "Liepard and Hitmontop expose manual coordination immediately. Manaphy and Kingdra are special rain reserves, Qwilfish controls pursuit, and Mega Malamar is ordered last as the mastermind. Generic visible-board switching may vary reserves without reading hidden actions.",
        },
        "team_intent": "Prankster Liepard and Eject Button Hitmontop attempt one manual rain handoff through Rain Dance, Fake Out, Wide Guard, Feint, Encore, or direct attacks. Manaphy supplies immediate three-type special coverage; Kingdra converts actual rain into Swift Swim spread pressure with a public Draco Meteor cost; Qwilfish adds a second Intimidate and Thunder Wave; level-34 Mega Malamar closes through Contrary Superpower, Psycho Cut, Knock Off, or Protect.",
        "intended_counterplay": "Inner Focus, Ghost typing, Protect, priority, Taunt, double-targeting, weather replacement, or immediate Liepard pressure can deny rain. Single-target attacks play around Wide Guard; non-Protect lines play around Feint; Eject Button is public. Electric/Grass and special bulk answer Manaphy/Kingdra, while Draco Meteor drops and finite rain are exploitable. Ground/Psychic/Electric pressure Qwilfish. Mega Malamar remains 4x weak to Bug and weak to Fairy; burn, Haze, Clear Smog, Unaware, phazing, careful stat control, and focused special damage stop Contrary growth.",
        "bespoke_ai": "Archie uses foe-aware, smart-switching, partner-aware, HP-aware, speed-control, field-control, and combo scoring. Liepard-Hitmontop support is scored jointly from visible value, Rain Dance is never refreshed while active, Eject Button resolves normally, Swift Swim and weather expire normally, Intimidate and paralysis require real events, and Mega Malamar uses native transformed-form simulation. No hidden read, Kyogre, Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax exists.",
        "uniqueness": "This is the campaign's first manual-rain faction heist, first Manaphy opponent, and first Mega Malamar. Hitmontop intentionally returns from Brawly as the exact World Champion tactical support piece. Qwilfish returns after a long gap in a new leader-grade Intimidate and paralysis pursuit role. Kyogre, Raichu, Ludicolo, and Crobat are removed so their protected owners and later Aqua doctrines remain distinct.",
        "story_logic": "Archie has not awakened or captured Kyogre. He enters after both Grunts with a crew that brings its own storm, and the script heals the player before and after the fight. Dialogue now describes the heist handoffs and Mega Malamar without claiming premature Primal ownership.",
        "reward_logic": "Archie gives EXP and prize money, then story progression: Stern receives the Devon Goods, Route 110 opens, and the player exits fully healed. No held-item or shop unlock is inserted into the locked scene, and Malamarite remains trainer equipment.",
        "campaign_reservations": {
            "spends": ["Slateport tactical heist", "manual rain plus layered Hitmontop support", "Manaphy rare museum prize", "Mega Malamar mastermind"],
            "preserves": ["Archie's first Kyogre and only Primal for Seafloor Cavern", "Palafin, Archaludon, Urshifu, Tsareena, and Mega Sharpedo for the final current", "Wattson's Raichu and other leaders' signatures"],
            "repeat_rule": "Later Archie may reuse none of this common five automatically; only the character and Aqua tactical growth recur. Kyogre debuts exclusively at Seafloor Cavern.",
        },
        "author_self_check": {
            "strongest_part": "Removing premature Primal Kyogre makes Archie more credible: he is already target-10 dangerous through tactical coordination, and the later legendary awakening becomes a genuine escalation.",
            "weakest_link": "Manual rain plus two Fake Out users can feel support-dense. Joint scoring, no hidden reads, direct-attack fallbacks, and the frailty of Liepard/Hitmontop keep the opening contestable.",
        },
        "closure": "Battle 48 is source-closed at target 10/10 under the story-correct backfill: six legal levels 31-34, six distinct items and roles, one Mega and no Primal, six competitive references, full pre/post heal contract, exact ordering and AI requirements, branch-neutral native-width dialogue, explicit later-Archie reservations, and no unsupported gimmick. Runtime playtesting remains required before difficulty 10 is observed.",
    }


def reservation():
    return {
        "campaign_order": 50,
        "anchor": "SLATEPORT_ARCHIE_INTERCEPTION",
        "trainer_ids": ["TRAINER_ARCHIE_SLATEPORT"],
        "planning_tier": "faction_leader",
        "design_commitment": "spent",
        "target_difficulty": 10,
        "protected_identity": "The first proof that Aqua leadership understands tactical rain positioning without already possessing the legendary it seeks.",
        "signature_reveal": "A vulnerable manual-rain heist crew plus Mega Malamar; Kyogre and Blue Orb remain fully reserved for Seafloor Cavern.",
        "primary_candidate_modes": ["manual-rain tactical offense", "Fake Out/Wide Guard/Feint positioning"],
        "secondary_candidate_modes": ["Manaphy/Kingdra special pressure", "Qwilfish pursuit control", "Contrary Mega finish"],
        "mega_legendary_posture": {
            "mega": "Exactly one Mega Malamar; Brawly's first-Mega reveal has already occurred.",
            "legendary": "Manaphy is the one rare mythical prize. Kyogre and every Primal mechanic are forbidden here and reserved for the story-correct Seafloor awakening.",
        },
        "candidate_reference_ids": [reference_id for reference_id, _ in REFERENCES],
        "adaptation_constraints": ["No Kyogre/Blue Orb/Primal before awakening.", "Manual rain must be vulnerable and finite.", "No Tera, Z-Move, Dynamax, or Gigantamax."],
        "chronological_closure_note": "Escalates Aqua from grunts to competitive command through coordination, then leaves Route 110, Wattson, and later Archie to broaden the game's power curve.",
    }


def expected_payloads():
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_048_SLATEPORT_MUSEUM_ARCHIE"] = design()
    reservations = json.loads(RESERVATIONS.read_text())
    entries = reservations["marquee_blueprints"]["entries"]
    index = next(i for i, entry in enumerate(entries) if entry.get("anchor") == "SLATEPORT_ARCHIE_INTERCEPTION")
    entries[index] = reservation()
    return designs, reservations


def verify_source():
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_ARCHIE_SLATEPORT"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 48 source party differs from backfill")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 48 trainer missing {token}")
    party_block = parties.split("static const struct TrainerMonItemCustomMoves sParty_Archie1[]", 1)[1].split("};", 1)[0]
    for forbidden in ("SPECIES_KYOGRE", "ITEM_BLUE_ORB", "SPECIES_RAICHU", "SPECIES_LUDICOLO", "SPECIES_CROBAT"):
        if forbidden in party_block:
            raise SystemExit(f"FAIL: Battle 48 retains forbidden premature source token {forbidden}")
    museum = (ROOT / "data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc").read_text()
    for cue in ("survived both waves", "brings its own storm", "Mega Malamar", "first heist", "complete plan"):
        if cue not in museum:
            raise SystemExit(f"FAIL: Battle 48 dialogue missing {cue}")
    if any(forbidden in museum for forbidden in ("primal rain", "Kyogre calls", "first tide", "simps")):
        raise SystemExit("FAIL: Battle 48 dialogue retains superseded or non-native text")
    for label in (
        "SlateportCity_OceanicMuseum_2F_Text_CameToSeeWhatsTakingSoLong",
        "SlateportCity_OceanicMuseum_2F_Text_ArchieWarning",
        "SlateportCity_OceanicMuseum_2F_Text_ArchieDefeat",
        "SlateportCity_OceanicMuseum_2F_Text_ArchieDefeated",
    ):
        text_block = museum.split(label + ":", 1)[1].split("\n\n", 1)[0]
        for line in re.findall(r'\.string "([^"]*)"', text_block):
            visible = line.replace("\\n", "").replace("\\p", "").replace("\\l", "").replace("$", "")
            if len(visible) > 36:
                raise SystemExit(f"FAIL: Battle 48 map has overlong dialogue line: {visible}")
    guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
    guide_entry = next(entry for entry in guide["entries"] if entry["trainerId"] == "TRAINER_ARCHIE_SLATEPORT")
    expected_species = [entry["species"] for entry in TEAM]
    if guide_entry["levelCap"] != 30 or guide_entry["format"] != "double" or guide_entry["designStatus"] != "closed":
        raise SystemExit("FAIL: Battle 48 guide metadata is stale")
    if [entry["speciesId"] for entry in guide_entry["party"]] != expected_species:
        raise SystemExit("FAIL: Battle 48 guide party is stale")


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    designs, reservations = expected_payloads()
    expected_designs = json.dumps(designs, indent=2, ensure_ascii=False) + "\n"
    expected_reservations = json.dumps(reservations, indent=2, ensure_ascii=False) + "\n"
    if args.write:
        DESIGNS.write_text(expected_designs); RESERVATIONS.write_text(expected_reservations)
    if args.check:
        if DESIGNS.read_text() != expected_designs: raise SystemExit("FAIL: Battle 48 design closure stale")
        if RESERVATIONS.read_text() != expected_reservations: raise SystemExit("FAIL: Slateport reservation stale")
        verify_source()
    print("PASS: Battle 48 design and historic reservation describe the story-correct manual-rain heist")


if __name__ == "__main__": main()
