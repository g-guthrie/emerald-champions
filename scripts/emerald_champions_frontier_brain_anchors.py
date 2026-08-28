#!/usr/bin/env python3
"""Generate and verify the seven Emerald Champions Frontier Brain anchors."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import search_competitive_teams as competitive  # noqa: E402
import verdant_battle_set_presets as presets  # noqa: E402
import verdant_doubles_conversion as doubles  # noqa: E402
import verdant_team_quality_audit as quality  # noqa: E402

OUTPUT_JSON = ROOT / "docs/emerald_champions_frontier_brain_designs.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_frontier_brain_designs.md"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
PROTECTED_PATHS = [
    ROOT / "docs/verdant_marquee_battle_designs.json",
    ROOT / "docs/emerald_champions_gym_anchor_designs.json",
    ROOT / "docs/emerald_champions_faction_anchor_designs.json",
    ROOT / "docs/emerald_champions_superboss_anchor_designs.json",
]
META_PATH = ROOT / "docs/competitive_team_index.meta.json"

EXPECTED_ORDER = ["ANABEL", "BRANDON", "GRETA", "LUCY", "NOLAND", "SPENSER", "TUCKER"]
ALLOWED_PROTECTED_REUSES = {
    ("BRANDON", "SPECIES_REGIGIGAS"): "Brandon's all-Regi identity requires Regigigas; Norman used it as one Normal-type singles drawback test, while Brandon makes it the final seal in the complete legendary family.",
    ("LUCY", "SPECIES_STEELIX"): "Courtney uses ordinary Air Balloon Steelix as a Sludge Wave safe tile; Lucy's iconic Mega Steelix is the final serpent and asks a trapping/status/coil-family question.",
}


def ref(record: dict, decision: str, reason: str) -> dict:
    return {
        "reference_id": record["reference_id"], "source_kind": record.get("source_kind"),
        "battle_style": record.get("battle_style"), "format": record.get("format"),
        "player": record.get("player"), "event": record.get("event"), "year": record.get("year"),
        "completeness": record.get("completeness"), "confidence": record.get("confidence"),
        "roster": record.get("roster", []), "sets": record.get("sets", []),
        "tags": record.get("tags", []), "urls": record.get("urls", []),
        "decision": decision, "reason": reason,
    }


def mon(order, species, item, ability, ability_slot, spread, moves, role, lead_group, mega=False):
    return {
        "order": order, "species": species, "level_offset": 0, "item": item,
        "ability": ability, "ability_slot": ability_slot, "spread": spread,
        "moves": moves, "role": role, "lead_group": lead_group, "mega_candidate": mega,
    }


def make_dossier(*, anchor_id, trainer_id, name, source, team, selected, records, meta,
                 hook, story_fit, question, primary_mode, secondary_mode, preview,
                 pressure, ai_requirements, forbidden, counterplay, weakness, lesson,
                 strongest, weakest, spends, preserves, mechanics=None, mega_text="exactly one Mega"):
    candidates = [ref(records[reference_id], decision, reason) for reference_id, (decision, reason) in selected.items()]
    current = {
        "party": [entry["species"] for entry in source["mons"]],
        "items": [entry["item"] for entry in source["mons"]],
        "quality_score": source["quality_score"],
        "reason_for_replacement": f"Current {name} is a useful draft, but the coordinated Frontier board now gives this Brain one protected facility identity and removes campaign collisions, passive loops, and repeated modules.",
    }
    offsets = [entry["level_offset"] for entry in team]
    return {
        "anchor_id": anchor_id,
        "planning_tier": "frontier_brain",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": f"Postgame Frontier Brain challenge: {name}",
            "location": "BattleFrontier",
            "strict_cap": 100,
            "player_tools": [
                "Game clear, full level-100 team-building services, all earned ordinary held items, moves, abilities, natures, and campaign Mega Stones",
                "Regular Frontier/Circuit battles may use the Champions random generator; this Brain remains a fixed marquee puzzle",
                "No grinding and full facility-side team preparation before the Brain challenge",
                "No in-battle items under Frontier boss rules",
                "Live Hard level 100, Medium level 98, or Easy level 96 trainer settings",
            ],
            "mega_access": f"{name} uses {mega_text}; no Primal or retired gimmick appears.",
            "evolution_phase": "Postgame ceiling: any fully evolved, legendary, mythical, Ultra Beast, or locally supported Mega is appropriate.",
            "preparation_access": "Full preparation is available before the Brain challenge; exact streak entry and reward gates require source verification.",
            "gauntlet_position": f"{name}'s fixed Brain puzzle inside the broader random-team Frontier ecosystem.",
            "mechanics_baseline_id": "frontier_brain",
            "live_difficulty": "Hard clamps every authored level to 100; Medium/Easy apply the global enemy-only -2/-4 reductions.",
        },
        "runtime": {
            "trainer_ids": [trainer_id], "canonical_format": "double", "party_size": 6, "required": False,
            "variants": [{"variant_id": anchor_id.lower(), "trainer_ids": [trainer_id], "format": "double", "scope": "designed-here", "reachability": "postgame Frontier Brain"}],
            "current_source_baseline": current,
            "source_paths": [f"src/data/trainer_parties.h:{source['party_name']}", f"src/data/trainers.h:{trainer_id}", "Battle Frontier scripts and facility dispatch"],
        },
        "rolling_context": {
            "available": False,
            "reason": "Frontier Brain order depends on facility/streak selection, so no single previous-ten sequence may be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": [other for other in EXPECTED_ORDER if other != anchor_id],
            "required_preimplementation_review": f"Refresh {name}'s exact facility rules, streak gate, prior generated teams, and reward before source implementation. Preserve {hook} unless runtime rules prove incompatible.",
        },
        "identity": {
            "memory_hook": hook, "story_fit": story_fit, "primary_player_question": question,
            "primary_mode": primary_mode, "secondary_mode": secondary_mode, "preview_pressure": preview,
        },
        "difficulty": {
            "target": 10, "observed": None,
            "rationale": f"Hard is six level-100 threats organized around {hook}. Difficulty comes from exact facility mechanics and coordinated roles, while multiple broad counterplay classes and public state remain intact.",
            "pressure_sources": pressure,
            "resource_tax": "The battle taxes correct team selection, speed and field control, mixed bulk, item/state awareness, and facility-specific adaptation rather than hidden information or grinding.",
            "tuning_order": [
                "Preserve the Brain's facility identity and roster uniqueness",
                "Validate facility rules and custom AI before set changes",
                "Tune ordering, support predicates, and public commitments before species",
                "Use set changes before diluting the identity because Hard cannot exceed level 100",
                "Use Medium/Easy only as player-selected relief",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": [team[0]["species"], team[1]["species"]],
            "mandatory_order_reason": f"The lead exposes {primary_mode} Remaining members are selected by visible board state.",
            "reserve_sequence": [entry["role"] for entry in team[2:]],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": ai_requirements,
            "forbidden_behaviors": forbidden,
            "state_machine": f"Open through {primary_mode} Transition through {secondary_mode} Every role retains a direct-action and missing-partner fallback.",
        },
        "counterplay": {
            "classes": counterplay, "intentional_weakness": weakness, "first_loss_lesson": lesson,
            "revealed_information": ["All weather, terrain, speed, ability, item, stat, form, Choice, and Mega state uses ordinary public battle information.", "No Brain receives hidden player move or switch knowledge.", "Hard/Medium/Easy affect enemy levels only.", "Facility-specific exceptions are documented in mechanics_proposal when required."],
            "unacceptable_failure_modes": ["AI reads hidden player actions", "A support or setup loop ignores board value", "Items, abilities, forms, or Choice state resolve incorrectly", "The Brain repeats another marquee puzzle", "A retired gimmick or second illegal Mega appears"],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": [name, hook, "Frontier Brain doubles", "Champions random doubles"],
            "candidates": candidates, "selected_reference_ids": list(selected),
            "rejected_gimmicks": ["Tera, Z-Move, Dynamax, Gigantamax, hidden-information reads, passive loops, and complete donor-team copying are rejected."],
            "imported_elements": [entry[1] for entry in selected.values()],
        },
        "campaign_reservations": {
            "spends": spends, "preserves": preserves,
            "releases": [f"Current {name} species not retained here return to the general postgame pool unless protected elsewhere."],
            "collision_notes": ["The validator rejects every unwaived species collision against League, Gym, faction, rival, superboss, and other Brain anchors.", f"{name}'s facility identity is {hook}"],
        },
        "presentation": {
            "intro_concept": f"{name} introduces the facility rule as the reason this battle cannot be solved like another campaign boss.",
            "defeat_concept": f"{name} recognizes that the player mastered the facility decision, not merely the level-100 stat line.",
            "post_battle_concept": "The native Frontier symbol/reward flow remains, but its incentive and repeatability require exact source audit.",
            "hint_concept": f"The facility hint names the public first-loss lesson: {lesson}",
            "native_width_status": "concept-only; exact Brain, attendant, reward, and hint text require native font-width validation",
            "guide_summary": f"Document level 100, exact six, facility identity, AI, broad counterplay, reward/streak gate, and live difficulty for {name}.",
        },
        "author_self_check": {"strongest_part": strongest, "weakest_link": weakest},
        "verification": {
            "design_schema": "pass", "species_items_moves_abilities": "pass", "source_implementation": "not-started",
            "script_and_format": "not-started", "dialogue_width": "concept-only", "guide": "concept-only",
            "runtime": "unplayed", "observed_difficulty": None,
            "evidence": [f"Current guide identifies {trainer_id} as a six-Pokemon postgame double.", "All proposed fixed tokens pass local static validation.", "Selected competitive references exist in the current corpus.", "No source party, dialogue, reward, or guide has been changed.", f"Authored offsets are {offsets}; Hard clamps to level 100."],
            "source_blockers": [f"Replace {source['party_name']} with the exact authored runtime or fallback sets.", "Implement facility-specific AI/mechanics and prove all fallback paths.", "Regression-test every item, ability, field, speed, form, Mega, replacement, and simultaneous-faint interaction.", "Audit and font-measure exact dialogue and reward flow.", "Run Hard/Medium/Easy real-ROM tests before observed difficulty is recorded."],
        },
        "mechanics_proposal": mechanics,
    }


def protected_species():
    result = set()
    for path in PROTECTED_PATHS:
        for dossier in json.loads(path.read_text())["designs"].values():
            result.update(entry["species"] for entry in dossier["team"])
            for team in dossier.get("opponent_teams", {}).values():
                result.update(entry["species"] for entry in team)
    return result


def build():
    meta = json.loads(META_PATH.read_text())
    records = {record["reference_id"]: record for record in competitive.load_records()}
    sources = {team["trainer_id"]: team for team in quality.audit()["teams"]}

    anabel_team = [
        mon(1,"SPECIES_UXIE","ITEM_LIGHT_CLAY","ABILITY_LEVITATE",0,"SPREAD_31_IV_HP_DEF_BOLD",["MOVE_REFLECT","MOVE_LIGHT_SCREEN","MOVE_THUNDER_WAVE","MOVE_PSYCHIC"],"Knowledge screen and paralysis controller without recovery.","lead"),
        mon(2,"SPECIES_AZELF","ITEM_LIFE_ORB","ABILITY_LEVITATE",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_PSYCHIC","MOVE_FLAMETHROWER","MOVE_DAZZLING_GLEAM","MOVE_PROTECT"],"Will as immediate fast special offense.","lead"),
        mon(3,"SPECIES_MESPRIT","ITEM_ASSAULT_VEST","ABILITY_LEVITATE",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_PSYCHIC","MOVE_ICE_BEAM","MOVE_THUNDERBOLT","MOVE_U_TURN"],"Emotion as bulky mixed coverage and pivot.","reserve"),
        mon(4,"SPECIES_INDEEDEE_FEMALE","ITEM_FOCUS_SASH","ABILITY_PSYCHIC_SURGE",2,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_FOLLOW_ME","MOVE_HELPING_HAND","MOVE_PSYCHIC","MOVE_PROTECT"],"Psychic Terrain and finite redirection for the slow mode.","reserve"),
        mon(5,"SPECIES_HATTERENE","ITEM_MENTAL_HERB","ABILITY_MAGIC_BOUNCE",2,"SPREAD_31_IV_HP_SPATK_QUIET",["MOVE_TRICK_ROOM","MOVE_DAZZLING_GLEAM","MOVE_PSYCHIC","MOVE_PROTECT"],"Sole conditional Trick Room and Magic Bounce threat.","reserve"),
        mon(6,"SPECIES_LATIAS","ITEM_LATIASITE","ABILITY_LEVITATE",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_DRAGON_PULSE","MOVE_MIST_BALL","MOVE_THUNDERBOLT","MOVE_PROTECT"],"Anabel's sole Mega and balanced final mind.","ace",True),
    ]
    brandon_team = [
        mon(1,"SPECIES_REGIELEKI","ITEM_FOCUS_SASH","ABILITY_TRANSISTOR",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_ELECTROWEB","MOVE_RISING_VOLTAGE","MOVE_VOLT_SWITCH","MOVE_PROTECT"],"Fast new seal and active speed control.","lead"),
        mon(2,"SPECIES_REGIDRAGO","ITEM_CHOICE_SPECS","ABILITY_DRAGONS_MAW",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_DRAGON_ENERGY","MOVE_DRACO_METEOR","MOVE_DRAGON_PULSE","MOVE_ANCIENT_POWER"],"HP-sensitive new seal with public Choice commitment.","lead"),
        mon(3,"SPECIES_REGIROCK","ITEM_WEAKNESS_POLICY","ABILITY_STURDY",2,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_ROCK_SLIDE","MOVE_BODY_PRESS","MOVE_STOMPING_TANTRUM","MOVE_PROTECT"],"Physical classic seal and finite policy.","reserve"),
        mon(4,"SPECIES_REGICE","ITEM_ASSAULT_VEST","ABILITY_CLEAR_BODY",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_ICE_BEAM","MOVE_THUNDERBOLT","MOVE_FOCUS_BLAST","MOVE_ICY_WIND"],"Special classic seal and second speed line.","reserve"),
        mon(5,"SPECIES_REGISTEEL","ITEM_LEFTOVERS","ABILITY_CLEAR_BODY",0,"SPREAD_31_IV_HP_DEF_SPDEF_SASSY",["MOVE_IRON_DEFENSE","MOVE_BODY_PRESS","MOVE_THUNDER_WAVE","MOVE_PROTECT"],"Single defensive conversion seal without Rest loop.","reserve"),
        mon(6,"SPECIES_REGIGIGAS","ITEM_LUM_BERRY","ABILITY_SLOW_START",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_CRUSH_GRIP","MOVE_HIGH_HORSEPOWER","MOVE_DRAIN_PUNCH","MOVE_WIDE_GUARD"],"Final king that must survive ordinary Slow Start.","ace"),
    ]
    greta_team = [
        mon(1,"SPECIES_AMBIPOM","ITEM_FOCUS_SASH","ABILITY_TECHNICIAN",0,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_BEAT_UP","MOVE_FAKE_OUT","MOVE_DOUBLE_HIT","MOVE_U_TURN"],"Visible Beat Up ignition and tactical lead.","lead"),
        mon(2,"SPECIES_TERRAKION","ITEM_LIFE_ORB","ABILITY_JUSTIFIED",0,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_CLOSE_COMBAT","MOVE_ROCK_SLIDE","MOVE_QUICK_GUARD","MOVE_PROTECT"],"Primary Justified recipient with finite activation.","lead"),
        mon(3,"SPECIES_COBALION","ITEM_ASSAULT_VEST","ABILITY_JUSTIFIED",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_IRON_HEAD","MOVE_CLOSE_COMBAT","MOVE_STONE_EDGE","MOVE_VOLT_SWITCH"],"Bulky Steel/Fighting pivot and alternate recipient.","reserve"),
        mon(4,"SPECIES_VIRIZION","ITEM_SCOPE_LENS","ABILITY_JUSTIFIED",0,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_LEAF_BLADE","MOVE_SACRED_SWORD","MOVE_STONE_EDGE","MOVE_PROTECT"],"Fast Grass/Fighting critical-pressure sword.","reserve"),
        mon(5,"SPECIES_KELDEO","ITEM_CHOICE_SPECS","ABILITY_JUSTIFIED",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_HYDRO_PUMP","MOVE_SCALD","MOVE_SECRET_SWORD","MOVE_ICY_WIND"],"Special sword with public Choice and speed control.","reserve"),
        mon(6,"SPECIES_HAWLUCHA","ITEM_HAWLUCHANITE","ABILITY_UNBURDEN",1,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_CLOSE_COMBAT","MOVE_ACROBATICS","MOVE_ROCK_SLIDE","MOVE_PROTECT"],"Greta's sole Mega and arena aerial finish.","ace",True),
    ]
    lucy_team = [
        mon(1,"SPECIES_SEVIPER","ITEM_EXPERT_BELT","ABILITY_INFILTRATOR",2,"SPREAD_31_IV_ATK_SPEED_NAIVE",["MOVE_GUNK_SHOT","MOVE_FLAMETHROWER","MOVE_GIGA_DRAIN","MOVE_PROTECT"],"Signature mixed serpent with no setup.","lead"),
        mon(2,"SPECIES_ARBOK","ITEM_BLACK_SLUDGE","ABILITY_INTIMIDATE",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_GUNK_SHOT","MOVE_CRUNCH","MOVE_GLARE","MOVE_PROTECT"],"Intimidate and paralysis coil support.","lead"),
        mon(3,"SPECIES_SERPERIOR","ITEM_LIFE_ORB","ABILITY_CONTRARY",2,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_LEAF_STORM","MOVE_DRAGON_PULSE","MOVE_GLARE","MOVE_PROTECT"],"Contrary special serpent and second finite paralysis line.","reserve"),
        mon(4,"SPECIES_SANDACONDA","ITEM_SITRUS_BERRY","ABILITY_SAND_SPIT",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_HIGH_HORSEPOWER","MOVE_ROCK_SLIDE","MOVE_COIL","MOVE_PROTECT"],"One Coil and Ground/Rock constriction without sand team dependency.","reserve"),
        mon(5,"SPECIES_ZYGARDE","ITEM_LEFTOVERS","ABILITY_AURA_BREAK",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_LANDS_WRATH","MOVE_DRAGON_DANCE","MOVE_CRUNCH","MOVE_PROTECT"],"Legendary serpent with one dance and ordinary base form.","reserve"),
        mon(6,"SPECIES_STEELIX","ITEM_STEELIXITE","ABILITY_STURDY",1,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_HEAVY_SLAM","MOVE_HIGH_HORSEPOWER","MOVE_ROCK_SLIDE","MOVE_PROTECT"],"Lucy's sole Mega and armored final serpent.","ace",True),
    ]
    noland_team = [
        mon(1,"SPECIES_GENESECT","ITEM_CHOICE_SCARF","ABILITY_DOWNLOAD",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_U_TURN","MOVE_ICE_BEAM","MOVE_THUNDERBOLT","MOVE_FLAMETHROWER"],"Fallback fast Download pivot.","lead"),
        mon(2,"SPECIES_PORYGON2","ITEM_EVIOLITE","ABILITY_DOWNLOAD",1,"SPREAD_31_IV_HP_DEF_SPDEF_SASSY",["MOVE_TRI_ATTACK","MOVE_ICE_BEAM","MOVE_TRICK_ROOM","MOVE_RECOVER"],"Fallback conditional reversal and bulky glue.","lead"),
        mon(3,"SPECIES_ROTOM","ITEM_SITRUS_BERRY","ABILITY_LEVITATE",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_ELECTROWEB","MOVE_THUNDERBOLT","MOVE_SHADOW_BALL","MOVE_PROTECT"],"Fallback active speed/pivot control.","reserve"),
        mon(4,"SPECIES_SILVALLY","ITEM_LIFE_ORB","ABILITY_RKS_SYSTEM",0,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_MULTI_ATTACK","MOVE_CRUNCH","MOVE_ICE_FANG","MOVE_PROTECT"],"Fallback adaptive physical attacker.","reserve"),
        mon(5,"SPECIES_KECLEON","ITEM_ASSAULT_VEST","ABILITY_PROTEAN",2,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_FAKE_OUT","MOVE_KNOCK_OFF","MOVE_DRAIN_PUNCH","MOVE_SHADOW_SNEAK"],"Fallback type-changing utility attacker.","reserve"),
        mon(6,"SPECIES_BANETTE","ITEM_BANETTITE","ABILITY_INSOMNIA",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_SHADOW_CLAW","MOVE_SUCKER_PUNCH","MOVE_WILL_O_WISP","MOVE_PROTECT"],"Fallback sole Mega when generation fails.","ace",True),
    ]
    spenser_team = [
        mon(1,"SPECIES_WEEZING_GALARIAN","ITEM_BLACK_SLUDGE","ABILITY_NEUTRALIZING_GAS",1,"SPREAD_31_IV_HP_DEF_BOLD",["MOVE_STRANGE_STEAM","MOVE_SLUDGE_BOMB","MOVE_TAUNT","MOVE_PROTECT"],"Ability-suppression lead and active Poison/Fairy control.","lead"),
        mon(2,"SPECIES_SLAKING","ITEM_CHOICE_BAND","ABILITY_TRUANT",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_DOUBLE_EDGE","MOVE_HIGH_HORSEPOWER","MOVE_ICE_PUNCH","MOVE_ROCK_SLIDE"],"Primary drawback beneficiary with public Choice.","lead"),
        mon(3,"SPECIES_ARCHEOPS","ITEM_LIFE_ORB","ABILITY_DEFEATIST",0,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_ROCK_SLIDE","MOVE_DUAL_WINGBEAT","MOVE_KNOCK_OFF","MOVE_PROTECT"],"Fast drawback attacker whose HP threshold returns when Gas leaves.","reserve"),
        mon(4,"SPECIES_GOLISOPOD","ITEM_ASSAULT_VEST","ABILITY_EMERGENCY_EXIT",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_FIRST_IMPRESSION","MOVE_LIQUIDATION","MOVE_LEECH_LIFE","MOVE_CLOSE_COMBAT"],"Priority and forced-exit attacker with no Protect.","reserve"),
        mon(5,"SPECIES_WISHIWASHI","ITEM_SITRUS_BERRY","ABILITY_SCHOOLING",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_MUDDY_WATER","MOVE_ICE_BEAM","MOVE_U_TURN","MOVE_PROTECT"],"Schooling threshold attacker and pivot.","reserve"),
        mon(6,"SPECIES_SABLEYE","ITEM_SABLENITE","ABILITY_PRANKSTER",2,"SPREAD_31_IV_HP_DEF_SPDEF_SASSY",["MOVE_KNOCK_OFF","MOVE_WILL_O_WISP","MOVE_RECOVER","MOVE_PROTECT"],"Spenser's sole Mega and final instinct-control mirror.","ace",True),
    ]
    tucker_team = [
        mon(1,"SPECIES_CHARIZARD","ITEM_CHARIZARDITE_Y","ABILITY_BLAZE",0,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_HEAT_WAVE","MOVE_SOLAR_BEAM","MOVE_AIR_SLASH","MOVE_PROTECT"],"Tucker's sole Mega and first-act sun star.","lead",True),
        mon(2,"SPECIES_VENUSAUR","ITEM_LIFE_ORB","ABILITY_CHLOROPHYLL",2,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_SLUDGE_BOMB","MOVE_GIGA_DRAIN","MOVE_EARTH_POWER","MOVE_PROTECT"],"First-act Chlorophyll partner with no sleep.","lead"),
        mon(3,"SPECIES_CLEFAIRY","ITEM_EVIOLITE","ABILITY_FRIEND_GUARD",2,"SPREAD_31_IV_HP_DEF_SPDEF_SASSY",["MOVE_FOLLOW_ME","MOVE_HELPING_HAND","MOVE_ICY_WIND","MOVE_PROTECT"],"Finite show support shared across acts.","reserve"),
        mon(4,"SPECIES_CORVIKNIGHT","ITEM_SAFETY_GOGGLES","ABILITY_MIRROR_ARMOR",2,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_TAILWIND","MOVE_TAUNT","MOVE_BRAVE_BIRD","MOVE_ROOST"],"Speed and anti-control stagehand with one recovery move.","reserve"),
        mon(5,"SPECIES_TYRANITAR","ITEM_SMOOTH_ROCK","ABILITY_SAND_STREAM",0,"SPREAD_31_IV_HP_ATK_ADAMANT",["MOVE_ROCK_SLIDE","MOVE_CRUNCH","MOVE_HIGH_HORSEPOWER","MOVE_PROTECT"],"Second-act weather change without a second Mega.","reserve"),
        mon(6,"SPECIES_DRACOZOLT","ITEM_CHOICE_SCARF","ABILITY_SAND_RUSH",2,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_BOLT_BEAK","MOVE_DRAGON_CLAW","MOVE_HIGH_HORSEPOWER","MOVE_ROCK_SLIDE"],"Second-act sand speed and public move-order commitment.","ace"),
    ]

    designs = {}
    configs = [
        ("ANABEL","TRAINER_ANABEL","Anabel",anabel_team,{"showdown:gen4randomdoublesbattle:002":("selected-set","Azelf fast offense."),"showdown:gen8randomdoublesbattle:018":("selected-set","Hatterene Trick Room and Hawlucha-era doubles roles."),"showdown:gen9randomdoublesbattle:002":("selected-set","Latias doubles legitimacy."),"vgc:regional-malmo-2020":("selected-history","Indeedee-Hatterene tournament structure.")},"knowledge, emotion, and will shifting between fast screens and one slow room","The Salon Maiden turns mind states into public speed and support choices.","Can the player read which mind state is active and stop the lake trio, Psychic Terrain, Trick Room, and Mega Latias from handing control forward?","Uxie-Azelf fast screen and offense.","Indeedee-Hatterene conditional terrain/Trick Room into Mesprit and Mega Latias.",["Screens plus fast Azelf","Psychic Terrain redirection","Conditional Trick Room","Mega Latias balance"],["Use screens and paralysis only when relevant.","Set Trick Room only when the slow board benefits.","Keep redirection finite and attack otherwise.","Mega Evolve Latias normally."],["No automatic Trick Room.","No passive screen/redirection loops.","No sleep or hidden information."],["Taunt/screen removal/speed reversal","Dark/Ghost/Bug pressure","Spread attacks around Follow Me","Choice and setup denial"],"Uxie is the only screen setter and Hatterene the only Trick Room setter; several members share Dark/Ghost/Bug weaknesses.","Identify whether Anabel is playing fast knowledge or slow intuition, then remove the one controller.","The lake-trio narrative makes six Psychic legends feel like distinct mind states, not a mono-type pile.","Psychic Terrain plus Trick Room can resemble Tate/Liza; the lake-trio screen-to-room progression must stay distinct.",["Anabel's complete lake-trio mind-state team","Mega Latias"],["Tate/Liza cosmic formations","Other Psychic teams"]),
        ("BRANDON","TRAINER_BRANDON","Brandon",brandon_team,{"showdown:gen6randomdoublesbattle:020":("selected-set","Regigigas and Wide Guard legitimacy."),"vgc:london-open-2023":("selected-history","Regieleki tournament speed pressure."),"vgc:regional-seattle-wa-2026":("selected-history","Regidrago modern tournament pressure.")},"six seals opening from Regieleki and Regidrago through the classic trio to Regigigas","The Pyramid King presents the complete Regi family as sequential elemental locks.","Can the player survive the new Regis' fast spread opening, solve each classic defensive seal, and reach Regigigas before Slow Start expires?","Regieleki-Regidrago fast HP-sensitive opening.","Regirock/Regice/Registeel defensive locks into final Regigigas.",["Electroweb and Dragon Energy","Weakness Policy Regirock","Assault Vest Regice","Iron Defense Registeel","Slow Start Regigigas"],["Evaluate Dragon Energy and Crush Grip from current HP.","Respect Choice Specs and Weakness Policy.","Use Iron Defense only with survival value.","Wide Guard only against real spread pressure."],["No ability suppression for Regigigas.","No Rest loops.","No hidden lock ordering."],["Priority and speed control","Fighting/Ground/Fire/Water coverage","Haze/Unaware/phazing","Choice and HP-pressure exploitation"],"No Mega, Regigigas suffers real Slow Start, Regidrago is Choice-locked, and each classic Regi exposes ordinary elemental weaknesses.","Treat each Regi as one public lock; preserve the tool needed for Regigigas rather than spending it on the first seal.","All six Regis finally appear together without cheating Slow Start or adding a generic support mon.","The all-Regi roster is rigid by design; set and order variety must keep it from feeling like six stat checks.",["All six Regis","Complete Pyramid seal sequence"],["Norman's separate Regigigas drawback cameo","Other legendary families"]),
        ("GRETA","TRAINER_GRETA","Greta",greta_team,{"showdown:gen6randomdoublesbattle:025":("selected-set","Ambipom Fake Out and fast utility."),"showdown:gen5randomdoublesbattle:002":("selected-set","Terrakion and Cobalion doubles legitimacy."),"showdown:gen8randomdoublesbattle:018":("selected-set","Hawlucha doubles role."),"vgc:ecc-2015-vg-regionals":("selected-history","Terrakion tournament legitimacy.")},"one visible Beat Up ignition followed by the four Swords of Justice and Mega Hawlucha","The Arena Tycoon turns martial discipline into one legal activation and five distinct fighting styles.","Can the player deny or absorb Ambipom's Beat Up into Terrakion, then adapt across Cobalion, Virizion, Keldeo, and Mega Hawlucha without one Fighting answer solving all?","Ambipom-Terrakion Fake Out/Beat Up activation.","Three other Swords split physical/special roles before Mega Hawlucha.",["Beat Up Justified","Fake Out and Quick Guard","Four Swords coverage","Mega Hawlucha speed"],["Beat Up only a healthy Justified ally with real payoff.","Stop activation after boosts are spent.","Respect Keldeo Choice Specs.","Mega Evolve Hawlucha normally."],["No repeated Beat Up farming.","No sleep or Substitute loops.","No hidden priority reads."],["Ghost/Fairy/Psychic/Flying pressure","Haze/Unaware/burn/Intimidate","Fake Out and activation denial","Choice exploitation"],"Ambipom and Hawlucha are frail, Keldeo is Choice-locked, and only one Beat Up activation exists.","Break the opening ignition, then answer each Sword's distinct type and damage category instead of treating them as one Fighting team.","The complete Swords of Justice appear as four genuinely different martial roles.","Beat Up Terrakion is famous; the full Swords progression and one-use predicate must make Greta more than a copied ladder core.",["Definitive Beat Up-Justified Brain","All four Swords of Justice","Mega Hawlucha"],["Other Fighting teams","Wally's Mega Gallade"]),
        ("LUCY","TRAINER_LUCY","Lucy",lucy_team,{"showdown:gen8randomdoublesbattle:030":("selected-set","Zygarde setup legitimacy."),"vgc:regional-malmo-2018":("selected-history","Serperior tournament legitimacy."),"elite:wolfe:indianapolis-2026":("adapted-role","Mega Steelix top-level durability reference.")},"six serpents using Intimidate, Glare, Contrary, Coil, legendary setup, and Mega Steelix","The Pike Queen's team finally looks and behaves like a procession through the Seviper-shaped facility.","Can the player manage two paralysis sources and two different setup serpents while preserving special pressure for Mega Steelix?","Seviper-Arbok mixed Poison pressure and Glare.","Serperior/Sandaconda/Zygarde setup progression into Mega Steelix.",["Intimidate and Glare","Contrary Leaf Storm","Coil and Dragon Dance","Mega Steelix bulk"],["Use Glare only on valuable unstatused targets.","Setup only with visible survival and payoff.","Track Contrary and Sand Spit exactly.","Mega Evolve Steelix normally."],["No evasion or Toxic stall.","No duplicate paralysis on statused targets.","No hidden setup reads."],["Taunt/Haze/Unaware/phazing","Ground/Psychic/Ice/Fairy coverage","Special Water/Fire/Fighting against Steelix","Status immunity and speed reversal"],"Several serpents are setup-dependent, paralysis is finite, and Mega Steelix remains specially vulnerable.","Do not let the Pike coil one stage at a time: deny setup, use status immunity, and save special burst for Steelix.","Every species is visibly serpentine and every coil mechanic is different.","Steelix repeats Courtney's base species; the Mega form and complete serpent identity must justify the exception.",["Definitive serpent Brain","Zygarde reveal","Mega Steelix"],["Courtney's ordinary safe-tile Steelix","Other Poison teams"]),
        ("NOLAND","TRAINER_NOLAND","Noland",noland_team,{"showdown:gen9championsrandomdoublesbattle:008":("selected-generator","Champions random-team roles and Banette fallback evidence."),"showdown:gen9randomdoublesbattle:019":("selected-generator","Modern random doubles legality and role balance."),"elite:shohei-kimura:worlds-2023":("selected-architecture","Positioning difficulty without formulaic speed mode.")},"a runtime-generated six-set Champions team with one verified fixed fallback","The Factory Head should embody the game's team generator rather than defend one arbitrary fixed roster.","Can the player identify a new generated team's roles from preview and public turns faster than Noland's AI can exploit its coherent random composition?","Generator selects six legal distinct species/items and at most one Mega.","Fixed Genesect/Porygon2/Rotom/Silvally/Kecleon/Mega Banette fallback only on generation failure.",["1309-set legal pool","Role-balanced generation","Species/item/type dedupe","Verified fixed fallback"],["Generate before preview from a saved deterministic run seed.","Use only locally legal preset moves/items/abilities.","Enforce one Mega and no Primal/signature progression item.","Fall back atomically on any invalid team."],["No TypeScript runtime or seeded thousands of teams.","No hidden post-preview regeneration.","No duplicate species/items or multiple Megas."],["Read preview roles","Exploit public Choice/setup","Broad balance rather than one hard counter","Replay adaptation across seeds"],"Random generation can produce variance, but legality, role, item, species, and fallback gates bound it; the fallback is fully public.","Noland's puzzle is learning quickly: identify speed, support, breaker, and Mega roles before committing your limited counterplan.","This is the clean native realization of the user-requested endless Champions random-team Frontier.","Generator variance can create outlier difficulty; deterministic seeds, validation, replay logging, and fallback telemetry are mandatory.",["Champions generator as Noland identity","One-Mega random Factory Brain","Atomic fallback"],["Fixed bespoke teams for every other Brain","Campaign progression items"],{"status":"required-before-source-closure","generator":"Adapt the existing C Champions Circuit selector and 1309 legal preset pool.","constraints":["six distinct species","distinct ordinary items","at most one Mega","no Primal/orbs/masks/drives/progression stones","role and damage balance","full legality"],"seed":"Persist one seed per Frontier challenge before party preview.","fallback":"Use the six authored fallback sets atomically if any invariant fails."}),
        ("SPENSER","TRAINER_SPENSER","Spenser",spenser_team,{"showdown:gen9randomdoublesbattle:011":("selected-set","Slaking doubles drawback pressure."),"showdown:gen7randomdoublesbattle:014":("selected-set","Wishiwashi doubles threshold pressure."),"showdown:gen9championsrandomdoublesbattle:007":("selected-set","Sableye active support and Mega-era role." )},"Neutralizing Gas selectively releasing five different drawback or threshold instincts before Mega Sableye","The Palace Maven turns autonomous instinct into abilities that change when the suppressor enters or leaves.","Can the player exploit which drawback is currently suppressed, force Weezing out, and then manage Truant, Defeatist, Emergency Exit, Schooling, and Mega Sableye as their normal rules return?","Galarian Weezing-Slaking suppression opening.","Archeops/Golisopod/Wishiwashi threshold sequence into Mega Sableye.",["Neutralizing Gas plus Slaking","Defeatist HP threshold","Emergency Exit","Schooling","Mega Sableye control"],["Track every suppressed/restored ability on entry and exit.","Do not pair Gas with a beneficiary when suppression is harmful.","Respect Choice and forced exits.","Mega Evolve Sableye normally and avoid Recover loops."],["No permanent ability deletion.","No AI bypass of Truant/thresholds after Gas leaves.","No passive stall."],["Remove/switch Weezing","Ability-agnostic damage","Taunt and status immunity","Threshold and Choice exploitation"],"The team is intentionally ability-dependent; Weezing is one suppressor, several thresholds can be forced, and Sableye has modest direct damage.","Watch the Gas, not just the species: force the suppressor off at the moment the active drawback matters most.","Five notoriously awkward Pokémon become one coherent Palace lesson without rewriting their abilities.","Neutralizing Gas interactions are engine-sensitive and require the deepest runtime regression of any fixed Brain.",["Definitive drawback-suppression Brain","Mega Sableye"],["Other Regigigas/Slaking appearances","Noland generator"]),
        ("TUCKER","TRAINER_TUCKER","Tucker",tucker_team,{"elite:wolfe:naic-2026":("selected-role","Wolfe Charizard-Y offensive/stall hybrid legitimacy."),"elite:wolfe:indianapolis-2026":("adapted-architecture","Tyranitar staged weather and showmanship reference."),"showdown:gen9championsrandomdoublesbattle:005":("selected-set","Tyranitar Champions doubles set."),"showdown:gen4randomdoublesbattle:009":("adapted-set","Venusaur-Charizard doubles relationship.")},"a two-act Dome show: Mega Charizard Y plus Venusaur, then Tyranitar plus Dracozolt sand","The Dome Ace changes the arena mid-performance without using two Megas.","Can the player interrupt the sun act, survive shared support, then retain the correct speed and Ground answers when Tyranitar changes the show to sand for Dracozolt?","Mega Charizard Y-Venusaur sun opening.","Clefairy/Corviknight bridge into non-Mega Tyranitar-Dracozolt sand finale.",["Mega sun and Chlorophyll","Friend Guard redirection","Tailwind and Taunt","Sand Stream and Sand Rush","Choice move-order Bolt Beak"],["Mega Evolve Charizard immediately only in the authored sun act.","Use support only with visible value.","Switch to Tyranitar act through ordinary replacement, not scripted weather cheating.","Evaluate Bolt Beak from real move order and Choice lock."],["No second Mega or Mega Tyranitar.","No sleep or hidden act selection.","No weather benefits outside active weather."],["Weather replacement/Cloud Nine","Rock/Water/Electric/Ground pressure","Taunt/redirection denial","Choice and move-order exploitation"],"Charizard and Venusaur rely on weather, Clefairy is passive, Corviknight has limited offense, Tyranitar changes weather for both sides, and Dracozolt is Choice-locked.","Treat it as two acts: break the sun stage without spending the tools needed for the sand-stage Bolt Beak finish.","Tucker gets a real theatrical change of set while obeying the one-Mega rule.","Sun-to-sand can resemble two modules; Clefairy/Corviknight continuity and ordinary replacement must make it one evolving show.",["Definitive two-act Dome battle","Mega Charizard Y","Sand Rush Dracozolt"],["Other weather bosses","Any second Mega"]),
    ]
    for anchor_id, trainer_id, name, team, selected, hook, story, question, primary, secondary, pressure, ai_req, forbidden, counters, weakness, lesson, strongest, weakest, spends, preserves, *mechanics in configs:
        designs[anchor_id] = make_dossier(
            anchor_id=anchor_id, trainer_id=trainer_id, name=name, source=sources[trainer_id], team=team,
            selected=selected, records=records, meta=meta, hook=hook, story_fit=story, question=question,
            primary_mode=primary, secondary_mode=secondary, preview=f"Preview clearly advertises {hook}.",
            pressure=pressure, ai_requirements=ai_req, forbidden=forbidden, counterplay=counters,
            weakness=weakness, lesson=lesson, strongest=strongest, weakest=weakest,
            spends=spends, preserves=preserves, mechanics=mechanics[0] if mechanics else None,
            mega_text="no Mega" if anchor_id == "BRANDON" else "exactly one Mega",
        )
    return {"version":1,"title":"Emerald Champions Frontier Brain anchor designs","expected_order":EXPECTED_ORDER,"designs":designs}


def validate(payload):
    contract = json.loads(OS_PATH.read_text())["dossier_contract"]
    if list(payload["designs"]) != EXPECTED_ORDER:
        raise AssertionError("Frontier Brain order drifted")
    dex = presets.LocalDex(); abilities = doubles.base_ability_slots()
    items = set(re.findall(r"#define\s+(ITEM_[A-Z0-9_]+)",(ROOT/"include/constants/items.h").read_text()))
    spreads = set(re.findall(r"#define\s+(SPREAD_[A-Z0-9_]+)",(ROOT/"include/constants/spreads.h").read_text()))
    refs = {record["reference_id"] for record in competitive.load_records()}
    mega_source = (ROOT/"src/data/pokemon/evolution.h").read_text()+(ROOT/"src/data/pokemon/verdant_gen9_evolutions.h").read_text()
    protected = protected_species(); seen = {}
    for anchor_id,dossier in payload["designs"].items():
        for field in contract["required_top_level"]:
            if field not in dossier: raise AssertionError(f"{anchor_id} missing {field}")
        if dossier["status"] != {"design":"design-complete","source":"unimplemented","static":"design-validated","runtime":"unplayed"}: raise AssertionError(f"{anchor_id} status")
        if dossier["difficulty"]["target"] != 10 or dossier["difficulty"]["observed"] is not None: raise AssertionError(f"{anchor_id} difficulty")
        if len(dossier["team"]) != 6: raise AssertionError(f"{anchor_id} party size")
        expected_megas = 0 if anchor_id == "BRANDON" else 1
        if sum(entry["mega_candidate"] for entry in dossier["team"]) != expected_megas: raise AssertionError(f"{anchor_id} Mega count")
        for entry in dossier["team"]:
            if set(contract["mon_required"])-set(entry): raise AssertionError(f"{anchor_id} mon schema")
            if entry["species"] in protected and (anchor_id,entry["species"]) not in ALLOWED_PROTECTED_REUSES: raise AssertionError(f"{anchor_id} protected {entry['species']}")
            if entry["species"] in seen: raise AssertionError(f"{anchor_id} repeats {entry['species']} from {seen[entry['species']]}")
            seen[entry["species"]]=anchor_id
            illegal=[move for move in entry["moves"] if move not in dex.legal_moves(entry["species"])]
            if illegal: raise AssertionError(f"{anchor_id} {entry['species']} illegal {illegal}")
            slots=abilities.get(entry["species"],[])
            if entry["ability_slot"]>=len(slots) or slots[entry["ability_slot"]]!=entry["ability"]: raise AssertionError(f"{anchor_id} ability {entry['species']}")
            if entry["item"] not in items or entry["spread"] not in spreads: raise AssertionError(f"{anchor_id} tokens {entry['species']}")
            if entry["mega_candidate"] and not re.search(rf"\[{entry['species']}\].*?EVO_MEGA_EVOLUTION,\s*{entry['item']}",mega_source,re.S): raise AssertionError(f"{anchor_id} Mega mapping")
        if not set(dossier["competitive_research"]["selected_reference_ids"])<=refs: raise AssertionError(f"{anchor_id} refs")
    return {"distinct_species":len(seen),"mega_count":sum(entry["mega_candidate"] for dossier in payload["designs"].values() for entry in dossier["team"])}


def markdown(payload,review):
    lines=["# Emerald Champions Frontier Brain anchor designs","",f"Seven Brains complete; {review['distinct_species']} distinct species and {review['mega_count']} unique Mega slots.",""]
    for anchor_id,dossier in payload["designs"].items():
        lines += [f"## {anchor_id}","",f"- Question: {dossier['identity']['primary_player_question']}",f"- First-loss lesson: {dossier['counterplay']['first_loss_lesson']}",f"- Strongest: {dossier['author_self_check']['strongest_part']}","- Team:"]
        for entry in dossier["team"]:
            lines.append(f"  - `{entry['species']}` — `{entry['item']}`; "+", ".join(f"`{move}`" for move in entry["moves"]))
        lines.append("")
    return "\n".join(lines)


def main():
    parser=argparse.ArgumentParser(); parser.add_argument("--write",action="store_true"); parser.add_argument("--check",action="store_true"); args=parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    payload=build(); review=validate(payload); payload["anchor_review"]={"status":"pass",**review,"allowed_protected_reuses":[{"anchor_id":a,"species":s,"reason":r} for (a,s),r in ALLOWED_PROTECTED_REUSES.items()]}
    expected_json=json.dumps(payload,indent=2,ensure_ascii=False)+"\n"; expected_md=markdown(payload,review)
    if args.write: OUTPUT_JSON.write_text(expected_json); OUTPUT_MD.write_text(expected_md)
    if args.check:
        if not OUTPUT_JSON.exists() or OUTPUT_JSON.read_text()!=expected_json: raise SystemExit("FAIL: Frontier Brain JSON stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text()!=expected_md: raise SystemExit("FAIL: Frontier Brain Markdown stale")
    print("PASS: all seven Frontier Brains are design-complete and source-honest")
    print(f"PASS: {review['distinct_species']} distinct species, {review['mega_count']} Mega slots, zero unwaived collisions")
    print("NEXT: Gym/rival rematch anchor families")


if __name__=="__main__": main()
