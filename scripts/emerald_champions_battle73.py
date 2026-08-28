#!/usr/bin/env python3
"""Generate/check the exact Battle 73 Derek closure."""

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
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"

TEAM = [
    {"level": 1, "species": "SPECIES_WORMADAM", "item": "ITEM_OCCA_BERRY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_GIGA_DRAIN", "MOVE_BUG_BUZZ", "MOVE_QUIVER_DANCE", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_WORMADAM_SANDY_CLOAK", "item": "ITEM_ASSAULT_VEST", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_EARTHQUAKE", "MOVE_ROCK_BLAST", "MOVE_SUCKER_PUNCH", "MOVE_BUG_BITE"]},
    {"level": 3, "species": "SPECIES_WORMADAM_TRASH_CLOAK", "item": "ITEM_ROCKY_HELMET", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_DEF_BOLD", "moves": ["MOVE_FLASH_CANNON", "MOVE_BUG_BUZZ", "MOVE_IRON_DEFENSE", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_MOTHIM", "item": "ITEM_LIFE_ORB", "ability_slot": 2, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_QUIVER_DANCE", "MOVE_BUG_BUZZ", "MOVE_AIR_SLASH", "MOVE_PSYCHIC"]},
]
REFERENCES = ["showdown:gen4randombattle:017", "showdown:gen5randombattle:007", "showdown:gen6randombattle:016"]


def design():
    return {
        "guide_order": 73,
        "trainer_ids": ["TRAINER_DEREK"],
        "status": "closed",
        "campaign_point": "Optional fixed Route 117 Bug Maniac after Maria; lower-lane order is canonical but open meadow geometry permits reversal with the upper cluster",
        "strict_cap": 40,
        "evolution_stage_fit": {
            "campaign_phase": "post-Wattson Route 117 mature-family single",
            "effective_levels": "41-44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": "All three Wormadam cloaks and Mothim evolve from Burmy at level 20, so every member is naturally mature well before cap 40. Derek deliberately uses no Mega or legendary in this change-of-pace route single.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.6,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": "showdown:gen4randombattle:017", "decision": "Wormadam setup/coverage role selected; full team rejected", "reason": "The generated single validates Wormadam as more than filler."},
                {"reference_id": "showdown:gen5randombattle:007", "decision": "Wormadam offense and setup legitimacy selected", "reason": "A second generated generation supports the family as a serious singles threat."},
                {"reference_id": "showdown:gen6randombattle:016", "decision": "Mothim Quiver Dance closer selected", "reason": "The generated Mothim provides the exact late setup-attacker language."},
            ],
            "decision": "No donor supplies the exact four-member Burmy family because the family theme is map/trainer-authored. Three reproducible singles records validate Wormadam and Mothim roles, while exact cloak differentiation is locally hand-authored.",
        },
        "competitive_references": [{"reference_id": reference_id, "adaptation": "Generated singles role adapted to the exact locally legal cloak or Mothim set."} for reference_id in REFERENCES],
        "ordering": {
            "intended_lead": ["SPECIES_WORMADAM"],
            "source_order": [entry["species"] for entry in TEAM],
            "reason": "Plant Cloak introduces setup and recovery pressure, Sandy changes damage category and coverage, Trash changes defense and contact incentives, and source-last Mothim is the Tinted Lens Quiver Dance finish. Singles switching remains matchup-aware rather than a forced wave.",
        },
        "team_intent": "Derek turns one raised Burmy swarm into four mature answers. Occa Plant Cloak uses Giga Drain, Bug Buzz, one Quiver Dance, or Protect. Assault Vest Sandy Cloak is the physical Earthquake/Rock Blast/Sucker Punch/Bug Bite breaker. Rocky Helmet Trash Cloak uses Steel/Bug special pressure and one Iron Defense. Level-44 Life Orb Tinted Lens Mothim closes with Quiver Dance, Bug Buzz, Air Slash, or Psychic.",
        "intended_counterplay": "Fire, Flying, Rock, and strong special pressure answer Plant Cloak through Occa; Water, Ice, Flying, and special attacks answer Sandy while Ground immunity blanks Earthquake; Fire, Fighting, Ground, Taunt, Haze, and special attacks answer Trash; Stealth Rock, priority, paralysis, Taunt, Haze, Unaware, phazing, Rock, Electric, Ice, and Life Orb recoil answer Mothim. No one field or hidden mechanic must be decoded.",
        "bespoke_ai": "Derek remains a native smart singles trainer with bad-move, faint, viability, foe, switching, and HP-aware scoring. Setup moves require visible survival/value; no first-turn setup is forced. Assault Vest, Occa, Rocky Helmet, Tinted Lens, Life Orb, Quiver Dance, Iron Defense, multi-hit Rock Blast, Sucker Punch, and Protect use existing mechanics. No custom selector or hidden read is added.",
        "uniqueness": "All four exact species are new to the first 72 closed encounters. This is the campaign's only complete Burmy-family opponent roster and the first deliberate all-cloak progression puzzle. It is also the second single in the last eleven encounters, providing a needed format change without becoming easy filler.",
        "story_logic": "Derek's existing dialogue already describes raising one Burmy swarm into Plant, Sandy, Trash, and Mothim outcomes. Badge gating truthfully keeps the mature level-41-44 family behind Wattson. He has no callback, item, or rematch.",
        "reward_logic": "Derek grants only ordinary EXP and prize money. No item, shop, legendary, Mega Stone, or progression reward is attached to this optional sight trainer.",
        "campaign_reservations": {
            "spends": ["complete Burmy evolution-family showcase", "three Wormadam cloak-role contrast", "Tinted Lens Mothim route closer"],
            "preserves": ["weather, terrain, room, Tailwind, screens, hazards, redirection, self-activation, legends, and Megas for other encounters"],
            "repeat_rule": "Later Wormadam or Mothim use requires a materially different trainer or role; no global replacement is forced.",
        },
        "author_self_check": {
            "strongest_part": "The family theme is instantly understandable, visually native, and mechanically real: each cloak changes the answer before Tinted Lens Mothim asks for setup denial.",
            "weakest_link": "Shared Fire/Flying/Rock pressure can compress the family after preview. The +1 to +4 levels, distinct damage axes, Occa, Assault Vest, Rocky Helmet, and Tinted Lens keep that broad answer from becoming a free sweep.",
        },
        "closure": "Battle 73 is source-closed at quality 10 and target difficulty 8.6: four fresh legal mature species at levels 41-44, four distinct items and roles, exact singles AI, three current corpus references, native-width family dialogue, broad typed/setup counterplay, no reward debt, and no unsupported gimmick. Runtime playtesting remains required before difficulty is observed.",
    }


def ledger_entry():
    return {
        "index": 73,
        "encounter_id": "BATTLE_073_ROUTE_117_DEREK",
        "identity": {"location": "Route117", "category": "optional fixed Bug Maniac single", "format": "single", "strict_cap": 40, "memory_hook": "One Burmy swarm became Plant, Sandy, and Trash cloaks before Tinted Lens Mothim emerges as the Quiver Dance finish."},
        "primary_player_question": "Can the player change damage axis and type answer across three cloaks, then preserve setup denial for Tinted Lens Mothim rather than trying to sweep the whole family with one move?",
        "tempo": "Four-stage singles family ladder—special sustain/setup, physical coverage, defensive contact tax, then fragile Tinted Lens setup—with no persistent field or custom subsystem.",
        "pressure_sources": ["level-41 Occa Overcoat Plant Cloak", "Giga Drain, Bug Buzz, Quiver Dance, Protect", "level-42 Assault Vest Sandy Cloak", "Earthquake, Rock Blast, Sucker Punch, Bug Bite", "level-43 Rocky Helmet Trash Cloak", "Flash Cannon, Bug Buzz, Iron Defense, Protect", "level-44 Life Orb Tinted Lens Mothim", "Quiver Dance, Bug Buzz, Air Slash, Psychic"],
        "intentional_opening": "Plant Cloak is the fixed first family lesson. Native singles AI may switch by visible matchup; no setup or reserve order beyond source-first/source-last is forced.",
        "intentional_weakness": "The family shares Fire/Flying/Rock pressure, has no speed field or recovery loop beyond Giga Drain, and relies on finite setup/items. Sandy and Trash expose special seams; Mothim is frail and pays Life Orb.",
        "first_loss_lesson": "The cloak changed the answer. Use the right category and typing for each form, then stop Mothim's one setup instead of preserving a single Bug resistance and hoping Tinted Lens respects it.",
        "revealed_information": ["Badge 3 is required", "Derek remains a single", "all four are mature Burmy outcomes", "Plant has Occa", "Sandy has Assault Vest", "Trash has Rocky Helmet", "Mothim has Tinted Lens and Life Orb", "Derek has no reward or rematch"],
        "counterplay_classes": ["Fire/Flying/Rock and strong neutral pressure", "Water/Ice/Ground immunity into Sandy", "Fire/Fighting/Ground and special pressure into Trash", "Taunt/Haze/Unaware/phazing/status against setup", "hazards/priority/Rock/Electric/Ice into Mothim", "item removal and Protect timing"],
        "target_difficulty": 8.6,
        "difficulty_rationale": "Four optimized legal levels 41-44, full distinct items, two setup threats, mixed damage axes, contact tax, multi-hit coverage, and a Tinted Lens Life Orb closer make a serious route single. Shared typed seams and no persistent field keep it below bosses and dense doubles.",
        "tuning_knob": "Tune Mothim from +4 to +3 first, then Trash Cloak from +3 to +2; preserve all four family roles and items.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-single", "bug-maniac", "burmy-family", "plant-cloak", "sandy-cloak", "trash-cloak", "mothim", "tinted-lens", "quiver-dance", "mixed-damage", "contact-tax", "no-mega", "no-legendary", "no-weather", "no-terrain", "no-room", "no-tailwind"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Three reproducible generated singles records support Wormadam/Mothim viability; the exact family ladder is transparently hand-authored from local forms."},
        "author_self_check": {"strongest_part": "Every evolution outcome gets one distinct job and Mothim makes the visual family reveal end in a real competitive threat.", "weakest_link": "Broad Fire/Flying/Rock coverage compresses the family; level and item pressure is the deliberate compensating difficulty knob."},
    }


def expected_payloads():
    designs = json.loads(DESIGNS.read_text()); designs["designs"]["BATTLE_073_ROUTE_117_DEREK"] = design()
    ledger = json.loads(LEDGER.read_text()); ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 73] + [ledger_entry()]; ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for entry in sequence["entries"]:
        if entry["index"] == 73: entry["status"] = "closed"
        elif entry["index"] == 74: entry["status"] = "next"
        elif entry["index"] > 74: entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({"closed_encounters": 73, "next_index": 74, "next_encounter_id": "BATTLE_074_ROUTE_117_AISHA_MELINA_BRANDI", "queued_sequence_entries": 10})
    return designs, ledger, sequence, operating_system


def verify_source():
    trainers = (ROOT / "src/data/trainers.h").read_text(); parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_DEREK"].group(0); body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM: raise SystemExit("FAIL: Battle 73 source party differs from closure")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in block: raise SystemExit(f"FAIL: Battle 73 trainer missing {token}")
    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route117_Text_DerekIntro:", 1)[1].split("Route117_Text_AnnaIntro:", 1)[0]
    for cue in ("One Burmy swarm", "Plant, Sandy, Trash, and Mothim", "Tinted Lens", "Dynamo Badge"):
        if cue not in dialogue: raise SystemExit(f"FAIL: Battle 73 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36: raise SystemExit(f"FAIL: Battle 73 overlong dialogue: {visible}")


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    payloads = expected_payloads(); paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    expected = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, expected): path.write_text(text)
    if args.check:
        for path, text in zip(paths, expected):
            if path.read_text() != text: raise SystemExit(f"FAIL: Battle 73 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text()); entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_DEREK")
        if entry["designStatus"] != "closed" or entry["levelCap"] != 40 or [mon["speciesId"] for mon in entry["party"]] != [mon["species"] for mon in TEAM]: raise SystemExit("FAIL: Battle 73 guide stale")
    print("PASS: Battle 73 Derek is source-closed at quality 10 and target difficulty 8.6")


if __name__ == "__main__": main()
