#!/usr/bin/env python3
"""Generate and verify backward-designed Emerald Champions Gym anchors."""

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


OUTPUT_JSON = ROOT / "docs/emerald_champions_gym_anchor_designs.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_gym_anchor_designs.md"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
MARQUEE_PATH = ROOT / "docs/verdant_marquee_battle_designs.json"
META_PATH = ROOT / "docs/competitive_team_index.meta.json"

EXPECTED_ORDER = [
    "SOOTOPOLIS_GYM_JUAN",
    "MOSSDEEP_GYM_TATE_AND_LIZA",
    "FORTREE_GYM_WINONA",
    "PETALBURG_GYM_NORMAN",
    "LAVARIDGE_GYM_FLANNERY",
    "MAUVILLE_GYM_WATTSON",
]

SPECIALTY_TYPES = {
    "SOOTOPOLIS_GYM_JUAN": "TYPE_WATER",
    "MOSSDEEP_GYM_TATE_AND_LIZA": "TYPE_PSYCHIC",
    "FORTREE_GYM_WINONA": "TYPE_FLYING",
    "PETALBURG_GYM_NORMAN": "TYPE_NORMAL",
    "LAVARIDGE_GYM_FLANNERY": "TYPE_FIRE",
    "MAUVILLE_GYM_WATTSON": "TYPE_ELECTRIC",
}


def reference_digest(record: dict, decision: str, reason: str) -> dict:
    return {
        "reference_id": record["reference_id"],
        "source_kind": record.get("source_kind"),
        "battle_style": record.get("battle_style"),
        "format": record.get("format"),
        "player": record.get("player"),
        "event": record.get("event"),
        "year": record.get("year"),
        "completeness": record.get("completeness"),
        "confidence": record.get("confidence"),
        "roster": record.get("roster", []),
        "sets": record.get("sets", []),
        "tags": record.get("tags", []),
        "urls": record.get("urls", []),
        "decision": decision,
        "reason": reason,
    }


def juan_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen6randomdoublesbattle:003": (
            "selected-core",
            "Empoleon Surf beside a Water Absorb partner is the direct source for Juan's ally-safe current mechanic; unrelated roster pieces are rejected.",
        ),
        "showdown:gen8randomdoublesbattle:020": (
            "selected-roles",
            "Suicune Tailwind and Snarl plus physical Water Bubble Araquanid establish distinct support and physical-pressure jobs without importing the full random team.",
        ),
        "showdown:gen9championsrandomdoublesbattle:011": (
            "selected-set",
            "Competitive Empoleon with Icy Wind and Flash Cannon supplies a modern doubles control set; Juan replaces Yawn and Hydro Pump with Protect and the authored Surf relay.",
        ),
        "vgc:worlds-2009": (
            "selected-history",
            "The 2009 World Champion roster proves Empoleon belongs in a highest-status doubles team; no exact team or unsupported historical mechanic is copied.",
        ),
        "vgc:regional-hong-kong-2017": (
            "selected-history",
            "The winning 2017 balance roster supports Araquanid as a legitimate tournament physical anchor rather than novelty filler.",
        ),
        "vgc:regional-merida-2025": (
            "adapted-species-only",
            "The winner validates Tatsugiri at elite stakes. Its Dondozo Commander core is explicitly rejected because Battle 27 already spent that lesson; Juan uses Mega Tatsugiri's Storm Drain instead.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current Politoed, Gothitelle, Kyogre, Palkia, rain, Perish, Shadow Tag, and Mega Kingdra shell collides directly with Archie, Phoebe, and Wallace reservations.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_SUICUNE",
            "level_offset": 1,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_INNER_FOCUS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_TAILWIND", "MOVE_SURF", "MOVE_SNARL", "MOVE_PROTECT"],
            "role": "Bulky conductor: establishes contestable speed, reduces special pressure, and later supplies ally-safe Surf activation.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_EMPOLEON",
            "level_offset": 1,
            "item": "ITEM_SHUCA_BERRY",
            "ability": "ABILITY_COMPETITIVE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SURF", "MOVE_FLASH_CANNON", "MOVE_ICY_WIND", "MOVE_PROTECT"],
            "role": "Competitive lead and current relay: punishes stat drops, changes speed, and can feed Juan's absorbent reserves.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_VOLCANION",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_WATER_ABSORB",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_STEAM_ERUPTION", "MOVE_HEAT_WAVE", "MOVE_EARTH_POWER", "MOVE_SLUDGE_BOMB"],
            "role": "Surf-safe anti-Grass and anti-Steel pivot whose recovery converts Juan's spread current into positioning value.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_ARAQUANID",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_WATER_BUBBLE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_LIQUIDATION", "MOVE_LEECH_LIFE", "MOVE_WIDE_GUARD", "MOVE_PROTECT"],
            "role": "Slow physical breaker and finite spread shield that punishes teams built only for Juan's special current.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_TAPU_FINI",
            "level_offset": 3,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_MISTY_SURGE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_MUDDY_WATER", "MOVE_MOONBLAST", "MOVE_NATURES_MADNESS", "MOVE_TRICK"],
            "role": "Fast disruption bridge: terrain blocks easy status plans while Scarf Trick or Nature's Madness opens the ace.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_TATSUGIRI_DROOPY",
            "level_offset": 4,
            "item": "ITEM_TATSUGIRINITE",
            "ability": "ABILITY_STORM_DRAIN",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_MUDDY_WATER", "MOVE_DRAGON_PULSE", "MOVE_ICY_WIND", "MOVE_PROTECT"],
            "role": "Juan's sole Mega and final conductor: an ally Surf becomes a visible Special Attack boost instead of collateral.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "SOOTOPOLIS_GYM_JUAN",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Eighth Gym Leader after the Cave of Origin crisis and before Victory Road",
            "location": "SootopolisCity_Gym_1F",
            "strict_cap": 70,
            "player_tools": [
                "Seven Badges and the full pre-League catch pool reachable through Surf, Dive, Waterfall, and late Hoenn routes",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items plus every campaign-earned progression item",
                "Mega Bracelet access since Granite Cave and all Mega Stones found before Sootopolis",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is the authored target",
            ],
            "mega_access": "The player has long-standing Mega access. Juan uses one Mega Tatsugiri; no Primal or second Mega appears.",
            "evolution_phase": "Late campaign: fully evolved, single-stage, legendary, mythical, curated Gen 9, and Champions Mega Pokemon are appropriate.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling access is available before entering the Gym. There is no multi-room party lock or preceding attrition requirement.",
            "gauntlet_position": "Final Badge boss. It must be target 10 on Hard but must preserve Archie and Wallace's later rain identities.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from each final level only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_JUAN_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_juan", "trainer_ids": ["TRAINER_JUAN_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_JUAN_5"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "current game-clear branch"},
                {"variant_id": "optional_rematch_modes", "trainer_ids": ["TRAINER_JUAN_2", "TRAINER_JUAN_3", "TRAINER_JUAN_4", "TRAINER_JUAN_5"], "format": "mixed", "scope": "deferred-to-rematch-phase", "reachability": "declared format and legendary choices require separate postgame audit"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Juan1",
                "src/data/trainers.h:TRAINER_JUAN_1",
                "data/maps/SootopolisCity_Gym_1F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "Juan is a future anchor; the chronological physical ledger has not reached Sootopolis and an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["SEAFLOOR_CAVERN_SHELLY", "SEAFLOOR_CAVERN_ARCHIE", "ELITE_FOUR_SIDNEY", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Refresh the final ten Sootopolis and Seafloor encounters. Preserve the Surf-relay question unless those battles spend ally Water activation, Mega Tatsugiri, Suicune control, or Araquanid Wide Guard immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Juan conducts the same Surf as pressure, healing, or a Storm Drain boost, then lets Mega Tatsugiri take the baton as the final current.",
            "story_fit": "Juan's elegance becomes literal choreography: every Water partner changes what the shared current means, so defensive turns and replacement order feel conducted rather than weather-driven.",
            "primary_player_question": "Can the player disrupt Juan's Surf-safe pairings and changing speed before ally Water Absorb or Mega Tatsugiri's Storm Drain converts spread pressure into recovery or a sweep?",
            "primary_mode": "Suicune and Competitive Empoleon establish contestable Tailwind, Icy Wind, Snarl, Steel pressure, and two potential Surf users without automatic weather.",
            "secondary_mode": "Board-state reserves alternate Assault Vest Volcanion recovery, physical Water Bubble Araquanid plus Wide Guard, Choice Scarf Tapu Fini disruption, and one final Surf-fed Mega Tatsugiri.",
            "preview_pressure": "All six Pokemon are visibly Water-aligned, but the player must distinguish which partner wants Surf, which attacks physically, which carries a Choice lock, and when the small Tatsugiri becomes the only Mega.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 71 through 74 against cap 70, six complete sets, two speed controls, mixed offense, Wide Guard, Choice disruption, and ally Water activation. It remains learnable because there is no automatic weather, redirection, trapping, sleep, evasion, or hidden required catch, and the absorbent pairings are visible.",
            "pressure_sources": [
                "Tailwind plus Icy Wind creates a two-sided but interactable speed contest",
                "Competitive Empoleon punishes careless Intimidate or stat drops",
                "Surf beside Water Absorb Volcanion can heal while maintaining spread pressure",
                "Water Bubble Araquanid forces a physical answer and can deny spread attacks with Wide Guard",
                "Choice Scarf Tapu Fini can Trick a wall or use Nature's Madness to open the ace",
                "Mega Tatsugiri can turn an allied Surf into a Storm Drain Special Attack boost before attacking",
            ],
            "resource_tax": "Juan is a standalone Badge boss, so his target 10 taxes formation changes, speed-control PP, physical and special answers, Wide Guard awareness, and denial rather than carried attrition from a prior fight.",
            "tuning_order": [
                "Preserve the visible Surf-relay identity, mixed categories, and sole Mega Tatsugiri climax",
                "Test joint action scoring and board-state pair selection before changing sets",
                "Adjust offsets within +1 to +4, beginning with Tatsugiri and Tapu Fini",
                "Then adjust Suicune or Empoleon bulk",
                "Change a move or item only after level-only Hard/Medium/Easy testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_SUICUNE", "SPECIES_EMPOLEON"],
            "mandatory_order_reason": "The lead exposes Juan's speed and Surf language without granting a free absorb boost. Later replacements are selected by visible board state; source order is not described as a forced wave.",
            "reserve_sequence": [
                "Prefer Volcanion beside a healthy Surf user when visible Fire, Grass, Steel, or special pressure makes Water Absorb recovery valuable.",
                "Prefer Araquanid when the opponent leans special bulk, spread damage, or Grass-neutral targets and Juan needs physical pressure or Wide Guard.",
                "Use Tapu Fini to disrupt a visible wall, status plan, or speed tier with Choice Scarf, Trick, Misty Terrain, or Nature's Madness.",
                "Hold Tatsugiri as the sole Mega climax when possible; pair it with surviving Suicune or Empoleon only when an ally Surf boost is safe and valuable. Use ordinary board-state fallbacks if that pair is unavailable.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"],
            "required_flags": ["AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Extend ally Surf scoring to Storm Drain as well as Water Absorb, while rejecting collateral into non-absorbent partners unless the visible knockout and survival value is superior.",
                "Score Suicune and Empoleon lead actions jointly so they do not redundantly set speed or use Surf into each other without a compelling visible result.",
                "Use a Juan-specific board-state reserve selector that values absorbent Surf partners, Araquanid's physical and Wide Guard role, Tapu Fini's visible Trick target, and the final Mega without forcing scripted pairs.",
                "Mega Evolve Tatsugiri when active unless a source-legal form or state prevents it; value an ally Surf boost only when Tatsugiri survives the predicted opposing actions.",
                "Use Tapu Fini Trick only against a visible high-value lock target and do not repeatedly choose Icy Wind or Tailwind when the current speed state is already favorable.",
            ],
            "forbidden_behaviors": [
                "Do not inspect unrevealed player moves, abilities, items, or switch choices to manufacture the ideal Surf pair.",
                "Do not use Surf into Araquanid or Tapu Fini merely because they resist it when direct moves create a better board.",
                "Do not reserve every replacement rigidly or keep Tatsugiri off the field when it is the only healthy legal response.",
                "Do not turn the fight back into rain, Perish Song, Shadow Tag, or passive recovery stall.",
            ],
            "state_machine": "Mode A is the Suicune-Empoleon speed and information lead. Mode B forms only when visible board state rewards Volcanion recovery or Araquanid physical/Wide Guard pressure. Mode C uses Tapu Fini to damage or lock a revealed defensive answer. Mode D exposes the sole Mega Tatsugiri; a surviving Surf user may conduct one boost, but both partners must retain independent legal attacks and fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, immediate pressure, Fake Out, priority, or opposing speed control can break the Suicune-Empoleon opening before it stabilizes.",
                "Electric, Grass, Ground, Freeze-Dry, strong physical damage, and targeted double-ups can exploit the team's shared structure while respecting its resistances.",
                "Wide Guard, Water immunity, spread reduction, weather replacement, and careful Protect use can deny Surf and Muddy Water value without one required species.",
                "Trick Room, paralysis, priority, or preserving a fast attacker after Tailwind can reverse Juan's changing speed tiers.",
                "Knocking out or phazing the visible absorbent partner prevents Surf from becoming recovery or a Storm Drain boost.",
            ],
            "intentional_weakness": "Juan has no rain setter, redirection, Fake Out, trapping, sleep, or recovery loop. His Surf relay is visible and partner-dependent; Volcanion and Tatsugiri can be removed, Araquanid is slow, Tapu Fini is Choice-locked, and Mega Tatsugiri has only base 92 Speed without Swift Swim.",
            "first_loss_lesson": "Do not treat six Water Pokemon as one matchup. Break the lead's speed control, identify whether Surf will damage, heal, or boost the partner, preserve a physical answer for Araquanid, and deny the surviving Surf user before Mega Tatsugiri takes the baton.",
            "revealed_information": [
                "The guaranteed lead reveals Tailwind, Icy Wind, Snarl, Competitive, and the possibility of Surf without an absorbent partner.",
                "Water Absorb and Storm Drain announce themselves when activated; Choice Scarf becomes inferable from turn order and Trick.",
                "Tatsugirinite is visible at Mega activation, and the player already has campaign access to the same stone family.",
            ],
            "unacceptable_failure_modes": [
                "Surf is selected into a non-absorbent ally when a safe direct line exists",
                "The reserve selector reads hidden player information or forces a dead pair",
                "The battle is solved only by one obscure immunity or catch",
                "Mega Tatsugiri never receives a playable board or becomes a free scripted sweep",
                "The team drifts back into Wallace rain, Archie aggression, or Phoebe Perish trapping",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Empoleon Surf Water Absorb ally activation", "Suicune Tailwind Snarl Araquanid Water Bubble", "Competitive Empoleon Icy Wind", "Tatsugiri tournament winner without reusing Commander"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Dondozo plus Commander is not imported because Battle 27 already owns that visible lesson.",
                "Rain, Kyogre, Palkia, Perish Song, Shadow Tag, and Mega Kingdra are removed to preserve Archie, Phoebe, and Wallace.",
                "No Tera, Z-Move, Dynamax, or Gigantamax dependency is imported.",
                "No full historic roster is copied merely because it contains one selected Water Pokemon.",
            ],
            "imported_elements": [
                "Empoleon Surf plus an absorbent partner",
                "Suicune Tailwind and Snarl role compression",
                "Araquanid as credible tournament physical pressure",
                "Competitive Empoleon with Icy Wind",
                "Tatsugiri as an elite-stakes reveal while rejecting its already-spent Commander core",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Juan's rainless Surf-relay identity",
                "Mega Tatsugiri as Juan's signature and only Mega",
                "Suicune plus Araquanid as elegant special-control and physical-pressure contrasts",
            ],
            "preserves": [
                "Archie's definitive aggressive rain and positioning doctrine",
                "Wallace's Kyogre-Tornadus rain, denyable Trick Room reversal, and Mega Milotic finale",
                "Phoebe's definitive Perish Song and Shadow Tag clock",
                "Tate and Liza's rare cosmic pair and fast-slow twin formation",
                "Dondozo plus Commander as Battle 27's already-spent lesson rather than repeating it here",
            ],
            "releases": [
                "Politoed, Gothitelle, Gastrodon, Palkia, Kyogre, and Mega Kingdra are released from Juan",
                "Mega Kingdra may appear elsewhere only if it does not duplicate Archie or Wallace rain",
            ],
            "collision_notes": [
                "No species overlaps the five main-story League teams.",
                "All six party members retain Water as a visible specialty type.",
                "Surf activation is a partner-state question, not weather, trapping, or Commander repetition.",
            ],
        },
        "presentation": {
            "intro_concept": "Juan keeps his elegant teacher persona but explains that mastery is not summoning rain—it is conducting one current so every partner gives it a different meaning.",
            "defeat_concept": "Juan recognizes that the player read every change in the current and interrupted the conductor rather than merely overpowering Water types.",
            "post_battle_concept": "The Rain Badge and Miloticite rewards remain native. Juan points toward Victory Road and Wallace without claiming his own strategy was rain.",
            "hint_concept": "The Gym guide warns that Juan's shared Water move may harm, heal, or strengthen its partner and that the smallest Pokemon holds the final baton.",
            "native_width_status": "concept-only; exact rewritten lines must be measured against native font widths at implementation",
            "guide_summary": "Document the cap-70 Suicune and Empoleon lead, rainless Surf relay, Water Absorb Volcanion, physical Wide Guard Araquanid, Choice Scarf Tapu Fini, Mega Tatsugiri Storm Drain climax, board-state pair selection, broad counterplay, Hard/Medium/Easy offsets, and the explicit separation from Archie, Phoebe, and Wallace.",
        },
        "author_self_check": {
            "strongest_part": "The same visible Surf can be ordinary spread pressure, heal Volcanion, or boost Mega Tatsugiri, making Juan feel like a conductor without stealing rain or Perish from later bosses.",
            "weakest_link": "The identity depends on joint action and reserve scoring that the current generic AI does not fully provide; if runtime pairing is poor, six individually excellent Water sets could look less coordinated than the dossier promises.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Juan as a required six-Pokemon double at badge count seven and strict cap 70.",
                "All six proposed species, items, moves, spreads, and selected ability slots exist and pass the local legality validator.",
                "Tatsugirinite maps Tatsugiri Droopy to Mega Tatsugiri Droopy, whose checked-in ability is Storm Drain and whose graphics are complete.",
                "Current AI already rewards ally Surf beside Water Absorb but does not yet include Storm Drain in that positive ally-combo branch.",
                "The selected competitive records are present in the 1005-record source-backed index.",
                "No game source, exact dialogue, or guide party has been changed for this design, and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Juan1 with the exact six authored sets and offsets.",
                "Add AI_FLAG_COMBO_SETUP and implement safe ally Surf plus Storm Drain scoring.",
                "Implement Juan's joint lead scoring and board-state reserve selector without hidden player information.",
                "Regression-test Surf with Water Absorb, Storm Drain, non-absorbent allies, Wide Guard, spread reduction, and simultaneous replacements.",
                "Regression-test Mega Tatsugiri activation, survival checks, and every missing-partner fallback.",
                "Write and font-measure exact intro, defeat, guide hint, and post-battle text; update the source-derived guide.",
                "Run representative cap-70 Hard, Medium, and Easy tests before recording observed difficulty.",
            ],
        },
        "mechanics_proposal": None,
    }


def tate_liza_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:wolfe:worlds-2025": (
            "selected-mode",
            "Wolfe's recent Lunala team demonstrates visible fast pressure transitioning into Lunala Trick Room and a slow attacker. Tate and Liza keep that dual-speed discipline but replace the entire sun, redirection, and Guts roster.",
        ),
        "elite:federico-camporesi:naic-2025": (
            "selected-mode",
            "The NAIC-winning Tailwind and Lunala Trick Room split validates a true two-speed preview. The twins use Psychic Terrain fast mode and Cresselia slow mode rather than copying species.",
        ),
        "elite:shoma-honami:worlds-2015": (
            "selected-role",
            "World Champion Cresselia balance supplies the durable Trick Room and Helping Hand role. Swagger variance, Fake Out, Intimidate, and the CHALK roster are rejected.",
        ),
        "elite:ray-rizzo:worlds-2012": (
            "selected-principle",
            "The World Champion mixed-speed structure supports one conventional fast axis plus a dangerous Cresselia slow axis; its sand and self-Swagger dependencies are not imported.",
        ),
        "showdown:gen7randomdoublesbattle:026": (
            "selected-set",
            "Mega Alakazam is validated as a fast doubles Mega. Tate and Liza replace Calm Mind and inaccurate coverage with Psychic Terrain Expanding Force, Dazzling Gleam, Encore, and Protect.",
        ),
        "showdown:gen9randomdoublesbattle:016": (
            "selected-set",
            "Full Metal Body Solgaleo with Sunsteel Strike and Psychic Fangs supplies the physical half of the final cosmic pair; the unrelated offense is rejected.",
        ),
        "vgc:naic-2018": (
            "selected-history",
            "The NAIC-winning Tapu Lele plus Mega Psychic attacker structure validates Psychic Terrain as a champion-level lead mode without requiring that exact Metagross roster.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "The current team already has excellent Psychic Terrain and Trick Room bones. Mega Slowbro and Victini are replaced so the twins gain a true fast Mega lead and a final Solgaleo-Lunala pair rather than several unrelated premium closers.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_TAPU_LELE",
            "level_offset": 1,
            "item": "ITEM_TERRAIN_EXTENDER",
            "ability": "ABILITY_PSYCHIC_SURGE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PSYCHIC", "MOVE_MOONBLAST", "MOVE_DAZZLING_GLEAM", "MOVE_PROTECT"],
            "role": "Fast terrain conductor that strengthens the opening and blocks priority without hiding behind a Choice lock.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_ALAKAZAM",
            "level_offset": 1,
            "item": "ITEM_ALAKAZITE",
            "ability": "ABILITY_MAGIC_GUARD",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_EXPANDING_FORCE", "MOVE_DAZZLING_GLEAM", "MOVE_ENCORE", "MOVE_PROTECT"],
            "role": "The twins' sole Mega and fast-mode glass cannon; Encore punishes passive attempts to wait out Terrain.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": True,
        },
        {
            "order": 3,
            "species": "SPECIES_CRESSELIA",
            "level_offset": 2,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_TRICK_ROOM", "MOVE_HELPING_HAND", "MOVE_ICE_BEAM", "MOVE_MOONLIGHT"],
            "role": "Denyable slow-mode hinge that reverses order only after the fast formation loses the speed contest or slow reserves dominate.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_CALYREX_ICE_RIDER",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_AS_ONE_ICE_RIDER",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_GLACIAL_LANCE", "MOVE_STOMPING_TANTRUM", "MOVE_ZEN_HEADBUTT", "MOVE_PROTECT"],
            "role": "Slow physical restricted attacker that converts a successful Trick Room into immediate spread pressure without another setup turn.",
            "lead_group": "slow-mode-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_SOLGALEO",
            "level_offset": 3,
            "item": "ITEM_WEAKNESS_POLICY",
            "ability": "ABILITY_FULL_METAL_BODY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_SUNSTEEL_STRIKE", "MOVE_PSYCHIC_FANGS", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Physical sun twin and screen breaker whose policy is threatening but never activated by scripted ally damage.",
            "lead_group": "cosmic-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_LUNALA",
            "level_offset": 4,
            "item": "ITEM_POWER_HERB",
            "ability": "ABILITY_SHADOW_SHIELD",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_MOONGEIST_BEAM", "MOVE_METEOR_BEAM", "MOVE_DAZZLING_GLEAM", "MOVE_WIDE_GUARD"],
            "role": "Special moon twin, one-use Meteor Beam threat, and finite Wide Guard answer to spread retaliation.",
            "lead_group": "cosmic-reserve",
            "mega_candidate": False,
        },
    ]
    return {
        "anchor_id": "MOSSDEEP_GYM_TATE_AND_LIZA",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Seventh Gym Leader battle after the Mossdeep Space Center approach and before Dive progression",
            "location": "MossdeepCity_Gym",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and the complete Fortree, Safari, Mt. Pyre, Lilycove, ocean-route, and Mossdeep catch pools",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items and every progression item found before Mossdeep",
                "Mega Bracelet and all reachable pre-Mossdeep Mega Stones",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is the authored target",
            ],
            "mega_access": "Reciprocal Mega access is mature. The twins use exactly one Mega Alakazam; Solgaleo and Lunala remain ordinary restricted forms.",
            "evolution_phase": "Late campaign: fully evolved, single-stage, legendary, mythical, curated Gen 9, and Champions Mega Pokemon are appropriate.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling access is available before the Gym. The Gym puzzle creates no battle attrition contract before the Leaders.",
            "gauntlet_position": "Seventh Badge boss and the campaign's definitive native twin formation. It must remain distinct from later Wallace dual-speed rain and League clocks.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_TATE_AND_LIZA_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_tate_liza", "trainer_ids": ["TRAINER_TATE_AND_LIZA_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_TATE_AND_LIZA_4"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "current rematch path"},
                {"variant_id": "declared_unused_rematches", "trainer_ids": ["TRAINER_TATE_AND_LIZA_2", "TRAINER_TATE_AND_LIZA_3", "TRAINER_TATE_AND_LIZA_5"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "declared records require source reachability audit"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_TateAndLiza1",
                "src/data/trainers.h:TRAINER_TATE_AND_LIZA_1",
                "data/maps/MossdeepCity_Gym/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Mossdeep, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MOSSDEEP_SPACE_CENTER_TABITHA", "MOSSDEEP_SPACE_CENTER_MULTI_CLIMAX", "SOOTOPOLIS_GYM_JUAN", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Refresh the final ten Mossdeep and ocean-route encounters. Preserve the fast Psychic Terrain, denyable Cresselia reversal, and Solgaleo-Lunala climax unless a nearby fight spends those exact decisions or species.",
        },
        "identity": {
            "memory_hook": "The twins attack first as one bright thought, reverse time behind Cresselia and Calyrex, then stand together as Solgaleo and Lunala for the final eclipse.",
            "story_fit": "Mossdeep's space center, rotating Gym, and inseparable twins become three formations—fast mind, reversed time, and sun-moon finale—rather than six unrelated Psychic legends.",
            "primary_player_question": "Can the player break Psychic Terrain offense, deny the one justified Trick Room reversal, and still preserve Dark or Ghost pressure to crack the final Solgaleo-Lunala eclipse?",
            "primary_mode": "Tapu Lele and Mega Alakazam form a fast Psychic Terrain lead with Expanding Force, Fairy coverage, Encore, and priority denial.",
            "secondary_mode": "Cresselia may reverse speed for Calyrex Ice Rider only when visible speed state warrants it; final Solgaleo and Lunala split physical and special pressure while sharing exploitable Dark and Ghost weaknesses.",
            "preview_pressure": "The six-Psychic preview visibly threatens fast Terrain, slow Trick Room, and a sun-moon pair, but no formation has redirection, trapping, Fake Out, sleep, or an invisible answer to every shared weakness.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 61 through 64 against cap 60, a fast Mega Terrain lead, one slow restricted attacker, two complementary cosmic closers, mixed categories, Wide Guard, and six full sets. The primary counterplay remains unusually broad because all six share Psychic identity and the final pair shares severe Dark or Ghost pressure.",
            "pressure_sources": [
                "Terrain-boosted Expanding Force plus Moonblast and Dazzling Gleam",
                "Encore punishing passive attempts to wait out Terrain or Protect repeatedly",
                "Mental Herb Cresselia creating one denyable Trick Room reversal",
                "Life Orb Glacial Lance and Stomping Tantrum under the slow mode",
                "Weakness Policy Solgaleo splitting physical pressure and breaking screens",
                "Power Herb Lunala applying one immediate Meteor Beam threat and finite Wide Guard",
            ],
            "resource_tax": "This standalone Gym boss taxes Terrain replacement, Dark and Ghost preservation, Wide Guard or spread mitigation, speed-mode denial, and mixed bulk rather than carried healing from prior trainers.",
            "tuning_order": [
                "Preserve all three visible formations and the shared final weakness",
                "Test the Terrain lead, Trick Room predicate, and cosmic pair fallbacks before changing sets",
                "Adjust offsets within +1 to +4, beginning with Lunala, Solgaleo, and Calyrex",
                "Then adjust Cresselia durability or Mega Alakazam damage",
                "Change species or mechanics only after Hard/Medium/Easy level testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_TAPU_LELE", "SPECIES_ALAKAZAM"],
            "mandatory_order_reason": "The fast Psychic Terrain lead establishes the twins' first formation. Later partners are selected from visible speed, HP, field, and matchup state rather than treated as an automatic scripted wave.",
            "reserve_sequence": [
                "Prefer Cresselia when the opponent has overtaken the fast mode or the remaining slow reserves materially benefit from Trick Room; never reverse a winning Terrain speed state automatically.",
                "Prefer Calyrex beside established Trick Room or when immediate physical spread pressure is the best visible response; use independent attacks when Cresselia is unavailable.",
                "Preserve Solgaleo and Lunala as the thematic final pair when practical, but deploy either independently if it is the only healthy or matchup-correct reserve.",
                "Use Lunala Wide Guard only against visible spread value and Solgaleo Protect or screen breaking according to the actual board, not to manufacture a fixed cinematic sequence.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"],
            "required_flags": ["AI_FLAG_COMBO_SETUP", "AI_FLAG_SCREENER"],
            "custom_requirements": [
                "Score Tapu Lele and Mega Alakazam actions jointly so Terrain Expanding Force, Fairy coverage, Encore, and Protect attack distinct visible plans instead of redundantly targeting one slot without knockout value.",
                "Use Trick Room only when the opposing effective board is faster or Calyrex and the cosmic reserves materially dominate the slow order; do not reverse the twins' own winning fast mode.",
                "Use a twin-specific reserve selector that recognizes the fast pair, slow pair, and cosmic pair but always permits healthy board-state fallbacks and simultaneous-faint replacements.",
                "Value Lunala Wide Guard only against revealed spread pressure and preserve Shadow Shield when a safer direct line exists.",
                "Never self-activate Solgaleo's Weakness Policy through scripted ally damage or hidden player information.",
            ],
            "forbidden_behaviors": [
                "Do not set Trick Room merely because Cresselia is active.",
                "Do not force Solgaleo and Lunala to wait while no legal healthy alternative exists.",
                "Do not inspect unrevealed Dark, Ghost, priority, spread, item, or switch information.",
                "Do not add redirection, trapping, sleep, a second Mega, or another field mode to erase the shared weaknesses.",
            ],
            "state_machine": "Mode A is Tapu Lele plus Mega Alakazam fast Psychic Terrain. Mode B becomes eligible only when visible speed math or slow reserves justify Cresselia Trick Room beside Calyrex. Mode C prefers Solgaleo plus Lunala as the final eclipse, with independent fallbacks when either twin is damaged, unavailable, or required earlier. Every mode remains board-state selected rather than globally allocated.",
        },
        "counterplay": {
            "classes": [
                "Replace Psychic Terrain, use Dark immunity, apply Steel resistance, or deny Mega Alakazam with priority after Terrain ends, faster pressure, Taunt, or spread mitigation.",
                "Taunt, Encore, reverse, stall, or immediately punish Cresselia so Trick Room never becomes a free Calyrex turn.",
                "Wide Guard, Protect timing, Intimidate, burns, physical walls, and speed reversal reduce Glacial Lance, Rock Slide, and Dazzling Gleam pressure.",
                "Preserve Dark or Ghost attacks, Knock Off, multi-hit damage, hazards, or focused double-targeting to break Shadow Shield and exploit the final cosmic pair's shared weaknesses.",
                "Use mixed offense: Mega Alakazam and Lunala lean special, while Calyrex and Solgaleo create the physical axis.",
            ],
            "intentional_weakness": "All six retain Psychic identity. The lead has no Fake Out or redirection, Cresselia is the only Trick Room setter and deals limited damage, Calyrex is slow outside Room, and Solgaleo plus Lunala share severe Dark and Ghost pressure. The team never adds trapping or priority denial beyond the interactable Terrain.",
            "first_loss_lesson": "Do not spend the Dark or Ghost answer merely surviving the Terrain lead. Remove or outlast Mega Alakazam, deny Cresselia's one reversal, then break Shadow Shield and attack the final sun-moon pair through their shared weakness before their mixed pressure separates your defenses.",
            "revealed_information": [
                "Psychic Terrain visibly announces priority denial and Expanding Force amplification.",
                "Cresselia's appearance identifies the only Trick Room setter; its Mental Herb is revealed only when consumed.",
                "Power Herb and Weakness Policy are one-use visible events, while Shadow Shield visibly changes Lunala's first damage exchange.",
                "Mega Alakazam is the sole Mega and transforms in the opening formation.",
            ],
            "unacceptable_failure_modes": [
                "Trick Room reverses the twins' own winning fast mode",
                "The cosmic pair is held illegally for a cinematic wave",
                "Hidden player information selects coverage, Wide Guard, or reserve order",
                "Dark and Ghost counterplay is silently erased by a new off-type support or trap",
                "The battle requires one obscure catch rather than broad Terrain, speed, spread, or shared-weakness answers",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Psychic Terrain Mega attacker", "Cresselia mixed speed Trick Room", "Lunala dual speed Wolfe", "Solgaleo physical restricted", "cosmic twins fast slow mode"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Tera dependencies from 2025 teams are removed completely.",
                "Swagger and self-Swagger variance from older Cresselia teams are rejected.",
                "No sun, sand, Fake Out, Intimidate, redirection, sleep, or full CHALK roster is imported.",
                "No second Mega or Primal appears beside Mega Alakazam.",
            ],
            "imported_elements": [
                "Recent Wolfe and NAIC Lunala dual-speed discipline",
                "World Champion Cresselia role compression and mixed-speed control",
                "Mega Alakazam as a credible fast Psychic attacker",
                "Solgaleo physical restricted pressure and screen breaking",
                "Tapu Lele plus Mega Psychic lead legitimacy from NAIC 2018",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "The campaign's definitive Psychic Terrain Gym lead",
                "Mega Alakazam as Tate and Liza's signature and only Mega",
                "Cresselia plus Calyrex Ice Rider as the twins' denyable slow formation",
                "Solgaleo plus Lunala as the rare cosmic final pair",
            ],
            "preserves": [
                "Wallace's rain-to-Trick-Room transition, which differs through weather and redirection",
                "Phoebe's trapping and Perish clock",
                "Drake's singles Latios handoff and special Mega Dragonite",
                "Magma and Aqua faction weather doctrines",
                "Steven's Mega Metagross and mineral-cosmic allied identity",
            ],
            "releases": [
                "Mega Slowbro and Victini are released from the main-story twins",
                "Other Psychic legendaries remain available if they do not duplicate the exact three-formation arc",
            ],
            "collision_notes": [
                "No species overlaps Juan or the five main-story League teams.",
                "All six species visibly retain Psychic typing.",
                "Trick Room overlaps Wallace only as a mechanic; the twins use one exposed Cresselia hinge without rain, redirection, or a Water endgame.",
            ],
        },
        "presentation": {
            "intro_concept": "The twins speak in alternating halves about thought moving faster than light, time folding backward, and the sun and moon meeting only when both minds agree.",
            "defeat_concept": "They recognize that the player did not chase one twin or one speed order, but kept the same plan coherent through all three formations.",
            "post_battle_concept": "The Mind Badge and native story progression remain unchanged. Their speech foreshadows the Space Center without claiming its later Magma formation is another Psychic field team.",
            "hint_concept": "The Gym guide warns that priority fails under the first formation, one partner can reverse the order, and the final sun and moon share the same darkness.",
            "native_width_status": "concept-only; alternating exact twin dialogue and guide lines require font-width verification at implementation",
            "guide_summary": "Document cap 60, the Tapu Lele and Mega Alakazam Terrain lead, Cresselia and Calyrex slow reversal, Solgaleo and Lunala final pair, exact board-state selection, common Dark/Ghost weakness, Hard/Medium/Easy offsets, historic dual-speed references, and the absence of hidden waves or a second gimmick.",
        },
        "author_self_check": {
            "strongest_part": "The battle tells one escalating twin story—fast shared thought, reversed time, then a sun-moon eclipse—while the final pair's common Dark and Ghost weakness rewards preservation rather than a secret answer.",
            "weakest_link": "Three formations can feel over-authored if reserve selection forces them regardless of board state; the design succeeds only if every pair is preferred but never illegally scripted and Cresselia declines Trick Room when fast mode is winning.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Tate and Liza as a required six-Pokemon double at badge count six and strict cap 60.",
                "All six proposed species retain Psychic type, and every item, move, spread, and selected ability slot exists and passes local legality.",
                "Alakazite maps Alakazam to Mega Alakazam, and no second Mega candidate is present.",
                "Current AI exposes speed, field, partner, foe, and smart-switching flags but needs a twin-specific pair and Trick Room selector.",
                "All selected competitive references exist in the current 1005-record index, including recent Wolfe and 2025 NAIC dual-speed evidence.",
                "No game source, exact dialogue, or guide party has been changed, and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_TateAndLiza1 with the exact six authored sets and offsets.",
                "Add the required combo and screen-aware flags and implement fast, slow, and cosmic board-state pair selection.",
                "Implement the conditional Trick Room predicate and prevent reversal of a winning fast state.",
                "Regression-test simultaneous faints, missing pair members, Terrain replacement, priority denial, Trick Room reversal, Wide Guard, Weakness Policy, Shadow Shield, and every independent fallback.",
                "Write alternating exact twin dialogue and font-measure all lines; update the source-derived guide.",
                "Run representative cap-60 Dark, Ghost, Steel, spread-control, priority, fast, slow, mixed-bulk, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def winona_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:wolfe:indianapolis-2026": (
            "selected-role",
            "Wolfe's current Talonflame validates Gale Wings positioning at elite level. Winona rejects the conditional dual-Mega, sand, setup, and redirection roster and uses Talonflame as a Feint/Tailwind air-traffic lead.",
        ),
        "showdown:gen9randomdoublesbattle:011": (
            "selected-set",
            "Intimidate Landorus-Therian with Rock Slide and board utility supports the partner-safe Ground spread axis; Winona uses Earthquake and Assault Vest instead of setup or hazards.",
        ),
        "showdown:gen9randomdoublesbattle:009": (
            "adapted-ability",
            "Volt Absorb Thundurus-Therian validates the Zapdos Discharge partner interaction. Nasty Plot and Tera Blast are explicitly removed for immediate coverage and Electroweb.",
        ),
        "showdown:gen8randomdoublesbattle:015": (
            "selected-role",
            "Sitrus Zapdos establishes durable Flying special pressure and recovery. Winona exchanges redundant Tailwind for Discharge, Hurricane, and Heat Wave.",
        ),
        "showdown:gen8randomdoublesbattle:013": (
            "selected-role",
            "Water Absorb Mantine proves a bulky Flying support can compress healing and field control. Winona adapts it to finite Wide Guard rather than another setup or screen mode.",
        ),
        "showdown:gen5randomdoublesbattle:019": (
            "adapted-species",
            "The reproducible Pidgeot set supports recovery and Heat Wave. Winona upgrades it to one No Guard Mega climax with Hurricane and no setup turn.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team is powerful but spends Tornadus, Mega Altaria, Rayquaza, and a generic Tailwind shell. The revision gives every pair an explicit altitude interaction and reserves Tornadus and Dragon Megas for Wallace and Drake.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_TALONFLAME",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_GALE_WINGS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_TAILWIND", "MOVE_FEINT", "MOVE_BRAVE_BIRD", "MOVE_QUICK_GUARD"],
            "role": "Air-traffic lead: establishes speed, breaks Protect for its partner, blocks priority, and threatens immediate Flying damage.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_LANDORUS_THERIAN",
            "level_offset": 1,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_INTIMIDATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_EARTHQUAKE", "MOVE_ROCK_SLIDE", "MOVE_U_TURN", "MOVE_KNOCK_OFF"],
            "role": "Partner-safe physical spread attacker and pivot; every ally is Flying and ignores its Earthquake.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ZAPDOS",
            "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_STATIC",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_DISCHARGE", "MOVE_HURRICANE", "MOVE_HEAT_WAVE", "MOVE_ROOST"],
            "role": "Durable special spread attacker whose Discharge becomes ally-safe only beside Volt Absorb Thundurus.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_THUNDURUS_THERIAN",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_VOLT_ABSORB",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDERBOLT", "MOVE_ELECTROWEB", "MOVE_GRASS_KNOT", "MOVE_PROTECT"],
            "role": "Discharge-safe special partner and secondary speed controller with immediate coverage instead of setup.",
            "lead_group": "discharge-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_MANTINE",
            "level_offset": 3,
            "item": "ITEM_LEFTOVERS",
            "ability": "ABILITY_WATER_ABSORB",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
            "moves": ["MOVE_WIDE_GUARD", "MOVE_AIR_SLASH", "MOVE_SCALD", "MOVE_ROOST"],
            "role": "Bulky altitude bridge and finite Wide Guard answer that slows the pace without adding screens or a second field mode.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_PIDGEOT",
            "level_offset": 4,
            "item": "ITEM_PIDGEOTITE",
            "ability": "ABILITY_NO_GUARD",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HURRICANE", "MOVE_HEAT_WAVE", "MOVE_TAILWIND", "MOVE_PROTECT"],
            "role": "Winona's sole Mega and accurate special climax: No Guard turns the final altitude exchange into certainty without setup.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "FORTREE_GYM_WINONA",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Sixth Gym Leader after the Weather Institute and Route 119 rival battle",
            "location": "FortreeCity_Gym",
            "strict_cap": 55,
            "player_tools": [
                "Five Badges and the western, volcanic, Petalburg-return, Weather Institute, and Route 119 catch pools",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items and all pre-Fortree progression items",
                "Mega Bracelet and all reachable pre-Fortree stones",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Reciprocal Mega access is established. Winona uses exactly one Mega Pidgeot and no Dragon Mega, Primal, or second Mega.",
            "evolution_phase": "Mature campaign: fully evolved, single-stage, legendary, mythical, curated Gen 9, and Mega Pokemon are appropriate, while rare middle stages may still appear on ordinary trainers.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling preparation is available before the Gym; no forced attrition precedes Winona.",
            "gauntlet_position": "Sixth Badge boss and the definitive airborne-positioning exam. It must not pre-spend Tate/Liza's cosmic formation or the later Dragon and rain Megas.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_WINONA_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_winona", "trainer_ids": ["TRAINER_WINONA_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_WINONA_5"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "current rematch branch"},
                {"variant_id": "declared_rematch_modes", "trainer_ids": ["TRAINER_WINONA_2", "TRAINER_WINONA_3", "TRAINER_WINONA_4", "TRAINER_WINONA_5"], "format": "mixed", "scope": "deferred-to-rematch-phase", "reachability": "all records require postgame reachability and format audit"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Winona1",
                "src/data/trainers.h:TRAINER_WINONA_1",
                "data/maps/FortreeCity_Gym/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Fortree, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["WEATHER_INSTITUTE_SHELLY", "RIVAL_ROUTE_119", "MOSSDEEP_GYM_TATE_AND_LIZA", "ELITE_FOUR_DRAKE"],
            "required_preimplementation_review": "Refresh the final ten Weather Institute, Route 119, and Fortree encounters. Preserve Feint plus partner-safe Earthquake, Discharge plus Volt Absorb, and Mega Pidgeot unless those exact interactions or species cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Winona opens a Tailwind lane with Feint beside an ally-safe Earthquake, shifts to a Discharge lane that only Thundurus can occupy, then closes the sky with No Guard Mega Pidgeot.",
            "story_fit": "Fortree's bridges, changing elevation, and rotating Gym become air-traffic control: each spread attack has a safe flight lane, every lane can be disrupted, and the final ace no longer misses.",
            "primary_player_question": "Can the player disrupt Winona's partner-safe spread lanes and changing speed before Feint removes Protect or No Guard Mega Pidgeot turns the final special exchange into guaranteed hits?",
            "primary_mode": "Gale Wings Talonflame and Assault Vest Landorus-Therian establish Tailwind, Feint, Quick Guard, U-turn, and Earthquake that every teammate naturally avoids.",
            "secondary_mode": "Zapdos may use Discharge beside Volt Absorb Thundurus-Therian, Mantine supplies finite Wide Guard, and Mega Pidgeot closes with accurate Hurricane and Heat Wave rather than setup.",
            "preview_pressure": "All six are visibly Flying, but the player must identify the Ground-safe lane, the one Electric-safe partner, the slow Wide Guard bridge, and the only Mega.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 56 through 59 against cap 55, immediate Tailwind and Feint pressure, partner-safe Earthquake, a conditional Discharge lane, mixed categories, Wide Guard, Intimidate, pivoting, and an accurate Mega climax. Shared Rock and Ice pressure, Stealth Rock, and the absence of redirection, sleep, trapping, or Trick Room keep the battle broadly solvable.",
            "pressure_sources": [
                "Gale Wings Tailwind, Feint, Brave Bird, and Quick Guard beside Landorus spread pressure",
                "Intimidate, Assault Vest, Earthquake, Rock Slide, U-turn, and Knock Off role compression",
                "Zapdos Discharge, Hurricane, Heat Wave, Static, and Roost durability",
                "Volt Absorb Thundurus Electroweb and immediate Life Orb special coverage",
                "Mantine Wide Guard and special bulk interrupting obvious spread retaliation",
                "No Guard Mega Pidgeot accurate Hurricane and Heat Wave plus an emergency second Tailwind",
            ],
            "resource_tax": "This standalone Gym boss taxes Protect timing, speed-control PP, Rock/Ice coverage, hazards, spread mitigation, mixed bulk, and recognition of safe partner lanes rather than carried healing.",
            "tuning_order": [
                "Preserve Feint plus Earthquake, Discharge plus Volt Absorb, and the sole No Guard Mega climax",
                "Test joint lead actions and safe spread pairing before changing sets",
                "Adjust offsets within +1 to +4, beginning with Mega Pidgeot and Mantine",
                "Then adjust Landorus or Zapdos bulk",
                "Change moves or species only after Hard/Medium/Easy level testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_TALONFLAME", "SPECIES_LANDORUS_THERIAN"],
            "mandatory_order_reason": "The lead immediately demonstrates partner-safe Ground spread and anti-Protect airspace control. Later pairings remain board-state preferences, not scripted reserve waves.",
            "reserve_sequence": [
                "Prefer Zapdos beside healthy Thundurus only when Discharge creates superior visible value; both retain independent attacks if the pair is broken.",
                "Use Mantine when visible spread pressure, special damage, Water coverage, or a required pace break makes Wide Guard and Roost valuable.",
                "Preserve Mega Pidgeot as the sole accurate special climax when practical, but deploy it earlier when it is the only healthy or matchup-correct reserve.",
                "Use U-turn and ordinary replacement scoring to reform safe lanes without reading hidden player actions.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"],
            "required_flags": ["AI_FLAG_COMBO_SETUP", "AI_FLAG_HP_AWARE"],
            "custom_requirements": [
                "Score Talonflame and Landorus actions jointly so Feint is chosen only when Landorus or the board can exploit the broken protection, and Tailwind or Quick Guard is not selected redundantly.",
                "Treat Earthquake as partner-safe for every Flying ally while still comparing direct attacks, Wide Guard, immunities, and visible knockout value.",
                "Prefer Zapdos Discharge beside Volt Absorb Thundurus only when both opponents and partner survival make it superior; never discharge freely beside another ally.",
                "Use a Winona reserve selector that recognizes the Ground-safe, Electric-safe, Wide Guard, and Mega roles without forcing unavailable pairs.",
                "Mega Evolve Pidgeot when active and use No Guard accuracy in damage estimates; do not use Tailwind again while a favorable Tailwind is active.",
            ],
            "forbidden_behaviors": [
                "Do not Feint a non-Protecting target without immediate visible payoff.",
                "Do not Discharge into Talonflame, Landorus, Mantine, or Pidgeot merely because the foes are vulnerable.",
                "Do not infer unrevealed Protect, Wide Guard, Rock, Ice, Electric, item, or switch choices.",
                "Do not add rain, Trick Room, redirection, sleep, trapping, a Dragon Mega, or a second Mega.",
            ],
            "state_machine": "Mode A is Talonflame plus Landorus anti-Protect Tailwind and Ground spread. Mode B becomes available when Zapdos and Thundurus can form a visible Discharge-safe lane; either attacks independently otherwise. Mode C uses Mantine as a finite Wide Guard and recovery bridge. Mode D exposes Mega Pidgeot as the sole accurate special climax, with all missing-partner and simultaneous-faint fallbacks resolved by visible board state.",
        },
        "counterplay": {
            "classes": [
                "Stealth Rock, Rock Slide, Ice coverage, strong Electric attacks, Gravity, Smack Down, or Thousand Arrows exploit the shared Flying structure and repeated switching.",
                "Deny or reverse Tailwind with Taunt, Trick Room, paralysis, Icy Wind, Electroweb, priority after Quick Guard, or simple survival until it expires.",
                "Wide Guard, Protect timing, Levitate or Flying immunities, Ground immunity, Volt Absorb, Lightning Rod, and targeted pressure can deny Earthquake, Rock Slide, Heat Wave, and Discharge value.",
                "Double-target Talonflame through Sash, remove Thundurus before Zapdos, or isolate Mega Pidgeot so the safe lanes never form.",
                "Use mixed Rock/Ice pressure: Landorus and Talonflame lean physical while Zapdos, Thundurus, Mantine, and Pidgeot lean special.",
            ],
            "intentional_weakness": "Every member is Flying and vulnerable to hazard or anti-air planning. The team has no redirection, Fake Out, sleep, trapping, Trick Room, weather, or setup sweeper. Talonflame loses Gale Wings after damage, Discharge is unsafe without Thundurus, Mantine is passive, and Mega Pidgeot has no boosting move or recovery.",
            "first_loss_lesson": "Read the flight lanes. Break Talonflame before Feint and Tailwind coordinate with Earthquake, prevent Zapdos and Thundurus from sharing the field, exploit the switches with Rock or Ice pressure, and keep a specially sturdy answer for accurate Mega Pidgeot.",
            "revealed_information": [
                "Talonflame's Gale Wings priority changes visibly after HP loss, and Feint reveals itself only when selected.",
                "Landorus Intimidate, Zapdos Static, Thundurus Volt Absorb, and Mantine Water Absorb are source-visible or activate publicly.",
                "Discharge and Earthquake targets are ordinary visible spread mechanics; No Guard is announced by Mega Pidgeot's ability behavior.",
                "Tailwind and Electroweb change speed through public field and stat state.",
            ],
            "unacceptable_failure_modes": [
                "AI damages its own non-absorbent partner with Discharge without decisive value",
                "Feint is scripted from hidden Protect knowledge",
                "Reserve pairs wait illegally for a cinematic formation",
                "Shared Rock and Ice counterplay is erased by new screens, redirection, or weather",
                "Mega Pidgeot becomes a setup sweep or a second Dragon-themed ace",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Wolfe Talonflame positioning", "Flying partner safe Earthquake", "Zapdos Discharge Volt Absorb", "Mantine Wide Guard", "Mega Pidgeot No Guard"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Wolfe's dual-Mega preview, sand, setup, redirection, and Trick Room are not imported.",
                "Thundurus Tera Blast and Nasty Plot are removed; Winona uses immediate local coverage and no unsupported gimmick.",
                "No rain, Dragon Mega, Rayquaza climax, or Tornadus repeats Wallace and Drake reservations.",
                "No full random or tournament roster is copied from a one-Pokemon fit.",
            ],
            "imported_elements": [
                "Recent Wolfe Talonflame positioning credibility",
                "Landorus physical spread and pivot compression",
                "Volt Absorb Thundurus as a Discharge-safe partner",
                "Durable Zapdos and Mantine support roles",
                "Mega Pidgeot as an accurate special Flying climax",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Winona's definitive partner-safe Flying spread lanes",
                "Talonflame Feint and Tailwind beside Landorus Earthquake",
                "Zapdos Discharge beside Volt Absorb Thundurus",
                "Mega Pidgeot as Winona's signature and only Mega",
            ],
            "preserves": [
                "Tate and Liza's Psychic Terrain, Trick Room, and cosmic final pair",
                "Drake's Dragon singles sequence and Mega Dragonite",
                "Wallace's Tornadus rain lead and Mega Milotic",
                "Magma and Aqua weather doctrines",
                "Mega Hawlucha as Brawly's already-spent Fighting/Flying reveal",
            ],
            "releases": [
                "Tornadus, Celesteela, Mega Altaria, and Rayquaza are released from main-story Winona",
                "Other Flying legends remain available if their roles do not duplicate the exact spread lanes",
            ],
            "collision_notes": [
                "No species overlaps Juan, Tate and Liza, or the five main-story League teams.",
                "All six species visibly retain Flying typing.",
                "Tailwind overlaps other anchors only as common speed control; Winona uniquely couples it to Feint and partner-safe Ground spread.",
            ],
        },
        "presentation": {
            "intro_concept": "Winona explains that mastery of the sky is not raw speed—it is choosing a lane where one partner's attack can never strike the other and closing every safe shelter the opponent sees.",
            "defeat_concept": "She recognizes that the player controlled the airspace, broke each safe lane, and still found the one place her final No Guard ace could be answered.",
            "post_battle_concept": "The Feather Badge and native reward flow remain unchanged. Winona's dialogue points east without foreshadowing the twins' cosmic or Wallace's rain identities.",
            "hint_concept": "The Gym guide warns that Protect can be broken, Ground cannot touch the flock, one thunderbird welcomes Discharge, and the final bird never misses.",
            "native_width_status": "concept-only; exact intro, defeat, guide, and hint lines require font-width verification at implementation",
            "guide_summary": "Document cap 55, Talonflame and Landorus lead, Feint plus partner-safe Earthquake, Zapdos and Volt Absorb Thundurus Discharge lane, Mantine Wide Guard, No Guard Mega Pidgeot, board-state fallbacks, shared Rock/Ice counterplay, historic references, and Hard/Medium/Easy offsets.",
        },
        "author_self_check": {
            "strongest_part": "Feint beside Earthquake and Discharge beside Volt Absorb turn Flying immunity into two different readable partner lanes, while No Guard Mega Pidgeot provides a clean final payoff instead of another setup ace.",
            "weakest_link": "Three legendary genies or birds can look like raw-stat inflation, and the design depends on safe spread scoring; the common Rock/Ice weakness, no setup, no redirection, modest offsets, and strict partner checks must remain intact.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Winona as a required six-Pokemon double at badge count five and strict cap 55.",
                "All six proposed species retain Flying type, and every item, move, spread, and selected ability slot exists and passes local legality.",
                "Pidgeotite maps Pidgeot to Mega Pidgeot, and no second Mega candidate appears.",
                "Current AI supports ally-aware scoring but requires exact Feint, Earthquake, and Discharge pair regression tests and a Winona reserve selector.",
                "All selected competitive references exist in the current 1005-record index, including Wolfe's 2026 Talonflame team.",
                "No game source, exact dialogue, or guide party has been changed, and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Winona1 with the exact six authored sets and offsets.",
                "Add required combo and HP-aware flags and implement joint Feint plus Earthquake scoring.",
                "Implement safe Discharge plus Volt Absorb evaluation and forbid collateral into other partners without superior visible value.",
                "Implement Winona's board-state reserve selector and all missing-pair and simultaneous-faint fallbacks.",
                "Regression-test Gale Wings HP thresholds, Feint, Quick Guard, Earthquake immunity, Wide Guard, Discharge, Volt Absorb, No Guard accuracy, Tailwind state, and U-turn replacements.",
                "Write and font-measure exact dialogue; update the source-derived guide.",
                "Run cap-55 Rock, Ice, Electric, hazard, spread-control, fast, slow, mixed-bulk, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def norman_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen4randombattle:013": (
            "selected-set",
            "Guts Swellow with Facade, Brave Bird, U-turn, and a protection turn supplies the exact fast physical commitment; Norman uses Quick Attack instead of Protect so Flame Orb timing remains visible and aggressive.",
        ),
        "smogon:gen4uu:003": (
            "selected-set",
            "The Smogon UU sample validates Guts Swellow's Facade, Brave Bird, Quick Attack, and U-turn as a complete competitive singles set.",
        ),
        "showdown:gen8randombattle:011": (
            "selected-role",
            "Fluffy Bewear is validated as a serious physical breaker. Norman trades Swords Dance and contact-only coverage for Assault Vest immediate pressure and explicit non-contact counterplay.",
        ),
        "smogon:gen5uu:002": (
            "selected-role",
            "The Smogon sample validates Meloetta as strong Normal/Psychic special pressure. Norman uses Choice Scarf and U-turn to create a readable speed commitment rather than Specs duplication.",
        ),
        "showdown:gen6randombattle:020": (
            "selected-role",
            "Regigigas Slow Start and Leftovers establish a visible survival clock. Confusion variance and Substitute stalling are rejected for direct attacks, Knock Off, and Thunder Wave.",
        ),
        "smogon:gen6nu:001": (
            "selected-set",
            "Scrappy Kangaskhan with Fake Out, Double-Edge, and Sucker Punch provides the singles ace chassis; Mega evolution adds one carefully constrained Power-Up Punch endgame.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "The current singles chassis is strong, but Zoroark repeats the campaign's early Illusion lesson and Slaking creates a second drawback giant. Bewear and Meloetta create visible category and item questions without hidden identity or redundant Truant timing.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_PORYGON_Z",
            "level_offset": 1,
            "item": "ITEM_CHOICE_SPECS",
            "ability": "ABILITY_ADAPTABILITY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_TRI_ATTACK", "MOVE_DARK_PULSE", "MOVE_THUNDERBOLT", "MOVE_ICE_BEAM"],
            "role": "Immediate special exam and first visible Choice commitment; its lock creates real switching counterplay.",
            "lead_group": "opening-candidate",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_SWELLOW",
            "level_offset": 1,
            "item": "ITEM_FLAME_ORB",
            "ability": "ABILITY_GUTS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FACADE", "MOVE_BRAVE_BIRD", "MOVE_U_TURN", "MOVE_QUICK_ATTACK"],
            "role": "Fast physical commitment whose visible burn changes both damage and the wisdom of using status.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_BEWEAR",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_FLUFFY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_DOUBLE_EDGE", "MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_SHADOW_CLAW"],
            "role": "Bulky physical category test: Fluffy rewards non-contact or Fire planning while Assault Vest prevents setup or healing loops.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_MELOETTA",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_SERENE_GRACE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HYPER_VOICE", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST", "MOVE_U_TURN"],
            "role": "Second readable Choice user and fast special pivot; Normal/Psychic coverage punishes one-dimensional Fighting answers.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_REGIGIGAS",
            "level_offset": 3,
            "item": "ITEM_LEFTOVERS",
            "ability": "ABILITY_SLOW_START",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_CRUSH_GRIP", "MOVE_DRAIN_PUNCH", "MOVE_KNOCK_OFF", "MOVE_THUNDER_WAVE"],
            "role": "Visible five-turn preservation clock and legendary Normal showcase with no ability suppression shortcut.",
            "lead_group": "endgame-candidate",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_KANGASKHAN",
            "level_offset": 4,
            "item": "ITEM_KANGASKHANITE",
            "ability": "ABILITY_SCRAPPY",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_DOUBLE_EDGE", "MOVE_SUCKER_PUNCH", "MOVE_POWER_UP_PUNCH"],
            "role": "Norman's sole Mega, father-child signature, priority closer, and only setup endgame.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "PETALBURG_GYM_NORMAN",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Fifth Gym Leader after four Badges and the Lavaridge return to Petalburg",
            "location": "PetalburgCity_Gym",
            "strict_cap": 45,
            "player_tools": [
                "Four Badges and the complete western, volcanic, desert, Lavaridge, and Petalburg-return catch pools",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items and all progression items earned before Petalburg",
                "Mega Bracelet and pre-Petalburg Mega Stones",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "The player has reciprocal Mega access. Norman uses exactly one Mega Kangaskhan and no other battle transformation.",
            "evolution_phase": "Mid-late campaign: boss teams may be fully evolved and use single-stage legends; ordinary nearby trainers may still preserve selected middle stages.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling preparation is available before entering Norman's room. Gym rooms do not impose a no-heal gauntlet.",
            "gauntlet_position": "Fifth Badge boss and deliberate singles change of pace after a doubles-heavy campaign. The battle must be lethal without importing doubles machinery.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opposing levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_NORMAN_1"],
            "canonical_format": "single",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_norman", "trainer_ids": ["TRAINER_NORMAN_1"], "format": "single", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_NORMAN_5"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "current rematch branch"},
                {"variant_id": "declared_rematch_modes", "trainer_ids": ["TRAINER_NORMAN_2", "TRAINER_NORMAN_3", "TRAINER_NORMAN_4", "TRAINER_NORMAN_5"], "format": "mixed", "scope": "deferred-to-rematch-phase", "reachability": "requires separate postgame audit"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Norman1",
                "src/data/trainers.h:TRAINER_NORMAN_1",
                "data/maps/PetalburgCity_Gym_LeaderRoom/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Norman, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["LAVARIDGE_GYM_FLANNERY", "RIVAL_ROUTE_119", "FORTREE_GYM_WINONA", "POSTGAME_GYM_REMATCH_CYCLE"],
            "required_preimplementation_review": "Refresh the final ten Lavaridge-return and Petalburg Gym encounters. Preserve singles format and visible information tests unless Choice locks, Guts Swellow, Fluffy Bewear, Slow Start, or Mega Kangaskhan cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Norman tests one fact at a time—special lock, visible burn, contact resistance, speed lock, Slow Start clock—then joins his own child in one final Mega parent-and-child attack.",
            "story_fit": "The father battle becomes a lesson in disciplined observation. Norman does not hide the rules; he asks whether the player can read each partner correctly and preserve composure for Kangaskhan's family climax.",
            "primary_player_question": "Can the player identify each Normal Pokemon's visible category, item commitment, and ability constraint quickly enough to exploit it before Mega Kangaskhan converts one mistake into the sole setup endgame?",
            "primary_mode": "Porygon-Z and Swellow alternate immediate special and physical commitments, each revealing a Choice lock or Flame Orb timing that can be exploited by disciplined switching.",
            "secondary_mode": "Fluffy Bewear tests contact selection, Scarf Meloetta tests speed and Fighting assumptions, Slow Start Regigigas creates a visible preservation clock, and Mega Kangaskhan is the only setup closer.",
            "preview_pressure": "All six visibly share Normal typing, but they threaten different categories, immunities, speed tiers, contact rules, and endgame clocks without Illusion or hidden form changes.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 46 through 49 against cap 45, two readable Choice commitments, immediate Guts Facade, Fluffy and Assault Vest bulk, a legendary Slow Start clock, mixed coverage, priority, and one Mega setup endgame. Fighting remains strong but never universal because Swellow and Meloetta punish careless use, while every item and ability can be inferred through public battle state.",
            "pressure_sources": [
                "Choice Specs Adaptability Tri Attack and coverage",
                "Flame Orb Guts Facade, Brave Bird, U-turn, and Quick Attack",
                "Fluffy Assault Vest Bewear forcing contact and category discipline",
                "Choice Scarf Meloetta pivoting through Hyper Voice, Psychic, Focus Blast, and U-turn",
                "Slow Start Regigigas surviving toward full power while spreading Thunder Wave and removing items",
                "Mega Kangaskhan Fake Out, priority, Double-Edge, and the team's sole Power-Up Punch setup",
            ],
            "resource_tax": "This standalone singles boss taxes safe pivots, physical and special walls, Fighting-answer preservation, non-contact coverage, speed control, phazing or Haze, and recognition of visible commitments rather than prior-battle attrition.",
            "tuning_order": [
                "Preserve singles format, visible commitments, Slow Start, and sole Mega Kangaskhan setup",
                "Test total switch scoring and each revealed item or ability before changing sets",
                "Adjust offsets within +1 to +4, beginning with Kangaskhan and Regigigas",
                "Then adjust Bewear or Meloetta bulk and speed",
                "Change a move or species only after Hard/Medium/Easy level testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_PORYGON_Z"],
            "mandatory_order_reason": "Porygon-Z opens with a visible special Choice test. Native singles switching remains board-state driven; later members are plausible endgames rather than a fixed scripted procession.",
            "reserve_sequence": [
                "Use Swellow when its fast physical Facade, Brave Bird, priority, or U-turn is the best visible response; do not waste its Guts state into an immunity or certain knockout.",
                "Use Bewear against visible contact-heavy physical plans and Meloetta against Fighting-heavy, slower, or specially vulnerable boards.",
                "Deploy Regigigas when its bulk can survive Slow Start turns or Thunder Wave and Knock Off create immediate value; do not cycle it merely to reset the clock.",
                "Prefer Mega Kangaskhan as the final ace when practical, but send it earlier if it is the only healthy or matchup-correct response. Power-Up Punch requires a visible survivable turn and future attack value.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"],
            "required_flags": ["AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_CHECK_FOE"],
            "custom_requirements": [
                "Use singles switch scoring that understands each revealed Choice lock, immunity, known speed order, current HP, status, and hazard cost without reading unrevealed player information.",
                "Treat Swellow's activated Guts and Bewear's Fluffy as public state when comparing attacks; distinguish contact from non-contact damage.",
                "Track Regigigas Slow Start turns accurately, avoid unnecessary clock resets, and value Thunder Wave or Knock Off only when immediate board gain exceeds direct damage.",
                "Use Fake Out only on Kangaskhan's legal first active turn and Power-Up Punch only when Kangaskhan survives the visible response and can exploit the boost.",
                "Respect both Choice users' locks and use U-turn only when a legal healthier or matchup-correct reserve exists.",
            ],
            "forbidden_behaviors": [
                "Do not inspect unrevealed moves, items, abilities, switch choices, or damage rolls to choose the perfect reserve.",
                "Do not switch Regigigas repeatedly and reset Slow Start without a concrete survival or matchup reason.",
                "Do not status Guts Swellow as though the Flame Orb and activation were hidden from Norman's own AI.",
                "Do not add Illusion, doubles support, trapping, weather, a second setup sweeper, or a second Mega.",
            ],
            "state_machine": "This is not a scripted wave. Singles replacement scoring classifies the active test—special Choice, Guts speed, contact bulk, Scarf pivot, Slow Start clock, or Mega endgame—and chooses the healthiest visible answer. Kangaskhan is preferred as the climax but never illegally withheld, and no state reads hidden player data.",
        },
        "counterplay": {
            "classes": [
                "Exploit the Specs and Scarf locks with immunities, resistances, Protect scouting, or safe switches, then punish U-turn destinations and hazard costs.",
                "Use Fighting pressure selectively while preserving answers to Swellow's Flying STAB, Meloetta's Psychic STAB, and Kangaskhan's priority.",
                "Attack Bewear with non-contact physical moves, special attacks, Fire, burn only when appropriate, or defense drops that bypass its Assault Vest role.",
                "Stall, phaze, debuff, disable, or pressure Regigigas during Slow Start; remove Leftovers and avoid donating free Thunder Wave turns.",
                "Use Intimidate, burn, Haze, phazing, Unaware, priority denial, or immediate offense to prevent Mega Kangaskhan from converting Power-Up Punch into a sweep.",
            ],
            "intentional_weakness": "Every member retains Normal typing and a real Fighting weakness or Fighting-answer tax. Two Pokemon are Choice-locked, Swellow pays recoil and burn damage, Bewear is vulnerable to non-contact and special pressure, Regigigas advertises Slow Start, and Kangaskhan owns the only setup move. There is no recovery loop, hazard setter, screen, weather, trap, sleep, or hidden identity.",
            "first_loss_lesson": "Read before attacking. Identify each lock and category, avoid contact into Fluffy, do not give Guts Swellow a better status plan, exploit Slow Start rather than rushing Regigigas, and keep Intimidate, burn, Haze, phazing, or immediate Fighting pressure for Mega Kangaskhan.",
            "revealed_information": [
                "Choice locks become inferable after the first selected move and turn order; U-turn publicly exits.",
                "Flame Orb and Guts, Fluffy contact reduction, and Slow Start activation are visible battle events.",
                "Fake Out legality follows Kangaskhan's first active turn, and Mega evolution publicly identifies the sole ace.",
                "No Illusion, hidden form, random evasion, or unrevealed trainer item is part of the design.",
            ],
            "unacceptable_failure_modes": [
                "AI switches through hidden player information rather than visible singles state",
                "Regigigas is cycled pointlessly and never receives a coherent clock",
                "Mega Kangaskhan uses Power-Up Punch into a guaranteed knockout or cannot exploit the boost",
                "One generic Fighting attacker defeats every member without facing Swellow, Meloetta, priority, or speed consequences",
                "The battle reintroduces Illusion or doubles machinery already spent elsewhere",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Normal singles Choice discipline", "Guts Swellow Facade", "Fluffy Bewear non contact", "Meloetta Choice pivot", "Regigigas Slow Start", "Kangaskhan priority singles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Illusion is removed because Battle 2 already owns the campaign's false-ace lesson.",
                "Slaking and Truant are released so Regigigas is the only drawback clock.",
                "Swagger, confusion, Substitute stall, sleep, hazards, and multiple setup sweepers are not imported.",
                "No doubles mode, unsupported transformation, or second Mega appears.",
            ],
            "imported_elements": [
                "Smogon and Showdown Guts Swellow exact singles pressure",
                "Fluffy Bewear as contact-sensitive physical bulk",
                "Meloetta as a credible Normal/Psychic Choice attacker",
                "Regigigas Slow Start as a visible clock rather than a joke",
                "Scrappy Kangaskhan priority chassis upgraded to one Mega setup climax",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "The campaign's definitive Normal-type singles information exam",
                "Mega Kangaskhan as Norman's signature and only Mega",
                "Regigigas as the visible Slow Start legendary clock",
                "Guts Swellow, Fluffy Bewear, and two distinct Choice commitments",
            ],
            "preserves": [
                "Winona's Flying doubles spread lanes",
                "Flannery's heat positioning and Magma's sun doctrines",
                "Rival preview-information puzzles, which must not reuse Illusion",
                "League and faction state machines",
                "Slaking and other Normal legends for distinct later encounters",
            ],
            "releases": [
                "Zoroark and Slaking are released from main-story Norman",
                "Porygon2 and other bulky Normal families remain available elsewhere",
            ],
            "collision_notes": [
                "No species overlaps Juan, Tate and Liza, Winona, or the five main-story League teams.",
                "All six species visibly retain Normal typing.",
                "Choice and priority are common mechanics, but Norman uniquely makes sequential identification of public commitments the whole singles question.",
            ],
        },
        "presentation": {
            "intro_concept": "Norman says strength is not guessing a secret—it is seeing exactly what a partner has committed to and remaining disciplined when the obvious attack is wrong.",
            "defeat_concept": "He recognizes that the player read each partner honestly, waited through raw power, and still kept composure when parent and child attacked together.",
            "post_battle_concept": "The Balance Badge, Surf progression, and family story remain native. Norman does not claim a doubles or weather lesson he never used.",
            "hint_concept": "The Gym guide warns that each partner reveals one rule: a locked move, a visible burn, a soft coat, a slow awakening, and one parent-and-child Mega.",
            "native_width_status": "concept-only; exact father, defeat, guide, and post-battle lines require font-width verification at implementation",
            "guide_summary": "Document cap 45, intentional singles, Specs Porygon-Z, Guts Swellow, Fluffy Assault Vest Bewear, Scarf Meloetta, Slow Start Regigigas, Mega Kangaskhan, visible locks and abilities, broad counterplay, source-honest AI needs, and Hard/Medium/Easy offsets.",
        },
        "author_self_check": {
            "strongest_part": "Every Pokemon asks a different visible singles question, and Regigigas into Mega Kangaskhan gives Norman two plausible endgames without hiding identity or importing doubles machinery.",
            "weakest_link": "Choice switching and six distinct information checks can feel like a sequence of modules unless Norman's replacement AI and dialogue unify them as one discipline test; runtime pacing must prove the team feels cohesive rather than encyclopedic.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Norman as a required six-Pokemon single at badge count four and strict cap 45.",
                "All six proposed species retain Normal type, and every item, move, spread, and selected ability slot exists and passes local legality.",
                "Kangaskhanite maps Kangaskhan to Mega Kangaskhan, and no second Mega candidate appears.",
                "The selected Showdown and Smogon singles references exist in the current 1005-record index.",
                "Current source already uses the intended singles format and many exact roles but still contains the rejected Illusion and duplicate-drawback slots.",
                "No game source, exact dialogue, or guide party has been changed, and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Norman1 with the exact six authored sets and offsets while preserving singles routing.",
                "Implement total visible-state singles switching for both Choice users, Guts, Fluffy contact, Slow Start, and Mega endgame value.",
                "Regression-test item locks, U-turn, Flame Orb, Guts, Fluffy contact classification, Assault Vest, Slow Start turns and resets, Fake Out legality, Power-Up Punch restraint, priority, phazing, and simultaneous effects.",
                "Remove any dialogue that implies Illusion, Truant, or doubles strategy and write/font-measure the exact discipline theme.",
                "Update the source-derived guide and reservations.",
                "Run cap-45 Fighting, Ghost immunity, physical, special, non-contact, stall, phazing, priority, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def flannery_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "vgc:regional-vancouver-bc-2023": (
            "selected-core",
            "The winning Lilligant-Torkoal roster validates After You sun pressure at elite level. Flannery imports only that lead language, not the unrelated Paradox, Dragon, Ground, and Kingambit roster.",
        ),
        "vgc:laic-2017": (
            "selected-history",
            "The LAIC-winning Torkoal-Lilligant mode establishes historic tournament legitimacy and a slow secondary roster. Flannery replaces sleep, Snorlax, and Mimikyu with a Fire-majority local slow mode.",
        ),
        "showdown:gen9randomdoublesbattle:019": (
            "adapted-role",
            "The generated doubles Delphox validates it as an aggressive Fire/Psychic board piece. Flannery trades its generic setup for the one exposed Trick Room hinge, using local Pyromancy to keep the slot distinctly thermal.",
        ),
        "smogon:gen9ou:004": (
            "adapted-role",
            "The published Skeledirge balance sample validates Unaware, Torch Song, and burn pressure as a durable Fire role. Flannery removes recovery and turns it into a finite Trick Room snowball so Camerupt remains available for Maxie's signature Mega.",
        ),
        "showdown:gen6randomdoublesbattle:010": (
            "adapted-role",
            "Air Balloon Flash Fire Heatran establishes defensive Fire positioning. Flannery replaces Substitute and burn loops with Magma Storm, Flash Cannon, Earth Power, and Protect.",
        ),
        "showdown:gen9championsrandomdoublesbattle:027": (
            "selected-set",
            "The local Champions generator validates Reckless Mega Emboar as immediate physical pressure. Flannery removes redundant priority and Protect for Ground and Water counter-coverage.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "sets": [
            {key: mon[key] for key in ("species", "item", "ability", "spread", "moves", "level_offset")}
            for mon in source["mons"]
        ],
        "reason_for_revision": "Source now matches the protected thermal-timing anchor: Cresselia and Volcanion are gone, Delphox and Heatran make the slow mode and bridge Flannery's own, and Mega Emboar remains the physical climax.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_TORKOAL",
            "level_offset": 1,
            "item": "ITEM_EJECT_BUTTON",
            "ability": "ABILITY_DROUGHT",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_ERUPTION", "MOVE_BODY_PRESS", "MOVE_YAWN", "MOVE_PROTECT"],
            "role": "Visible sun and temperature anchor whose HP-sensitive Eruption can be pulled forward or disrupted before it fires.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_LILLIGANT",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_CHLOROPHYLL",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_AFTER_YOU", "MOVE_HELPING_HAND", "MOVE_SOLAR_BEAM", "MOVE_PROTECT"],
            "role": "The one earned off-type support: changes Torkoal's move order, amplifies a partner, and threatens Water answers without sleep variance.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_DELPHOX",
            "level_offset": 2,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_PYROMANCY",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_TRICK_ROOM", "MOVE_HEAT_WAVE", "MOVE_SHADOW_BALL", "MOVE_WILL_O_WISP"],
            "role": "Denyable slow-mode hinge whose Pyromancy makes Heat Wave a burn threat; it reverses order only when visible speed state earns it.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_SKELEDIRGE",
            "level_offset": 2,
            "item": "ITEM_THROAT_SPRAY",
            "ability": "ABILITY_UNAWARE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_TORCH_SONG", "MOVE_SHADOW_BALL", "MOVE_HYPER_VOICE", "MOVE_PROTECT"],
            "role": "Slow Unaware snowball whose first sound attack can consume Throat Spray; it makes successful Trick Room materially different without borrowing Maxie's Camerupt.",
            "lead_group": "slow-mode-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_HEATRAN",
            "level_offset": 3,
            "item": "ITEM_AIR_BALLOON",
            "ability": "ABILITY_FLASH_FIRE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_MAGMA_STORM", "MOVE_FLASH_CANNON", "MOVE_EARTH_POWER", "MOVE_PROTECT"],
            "role": "Defensive heat bridge: visible Balloon and Magma Storm punish obvious Water, Rock, and Ground sequencing without a passive stall loop.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_EMBOAR",
            "level_offset": 4,
            "item": "ITEM_EMBOARITE",
            "ability": "ABILITY_RECKLESS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HEAT_CRASH", "MOVE_CLOSE_COMBAT", "MOVE_HIGH_HORSEPOWER", "MOVE_WILD_CHARGE"],
            "role": "Flannery's sole Mega and physical finale, with no setup or recovery turn and explicit anti-Water coverage.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "LAVARIDGE_GYM_FLANNERY",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "source-closed", "static": "source-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Fourth Gym Leader after Mt. Chimney and Jagged Pass",
            "location": "LavaridgeTown_Gym_1F",
            "strict_cap": 40,
            "player_tools": [
                "Three Badges and the Slateport, Mauville, ash-route, Meteor Falls, volcanic, desert-edge, and Jagged Pass catch pools",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items and all progression items earned before Lavaridge",
                "Mega Bracelet and pre-Lavaridge Mega Stones",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Reciprocal Mega access is established. Flannery uses exactly one Mega Emboar and no Primal or second Mega.",
            "evolution_phase": "Midgame boss phase: fully evolved and single-stage threats are appropriate, while selected middle stages remain normal outside marquee battles.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling access is available before entering the Gym. The steam-room trainers do not create a mandatory no-heal sequence.",
            "gauntlet_position": "Fourth Badge boss and the non-faction sun exam. It must distinguish thermal timing and anti-rain play from Maxie's positioning doctrine.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_FLANNERY_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_flannery", "trainer_ids": ["TRAINER_FLANNERY_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_FLANNERY_5"], "format": "double", "scope": "source-verified-rematch-family", "reachability": "current game-clear branch"},
                {"variant_id": "declared_rematch_modes", "trainer_ids": ["TRAINER_FLANNERY_2", "TRAINER_FLANNERY_3", "TRAINER_FLANNERY_4", "TRAINER_FLANNERY_5"], "format": "mixed", "scope": "source-verified-rematch-family", "reachability": "native singles/doubles and legendary-choice branches"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Flannery1",
                "src/data/trainers.h:TRAINER_FLANNERY_1",
                "data/maps/LavaridgeTown_Gym_1F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": True,
            "reason": "The chronological physical ledger has reached Flannery; Battles 113-122 are the exact previous-ten window.",
            "previous_encounters": [
                {
                    "index": row["index"],
                    "encounter_id": row["encounter_id"],
                    "format": row["identity"]["format"],
                    "primary_player_question": row["primary_player_question"],
                    "tempo": row["tempo"],
                    "novelty_tags": row["novelty_tags"],
                }
                for row in [
                    entry
                    for entry in json.loads((ROOT / "docs/verdant_battle_experience_ledger.json").read_text())["entries"]
                    if entry["index"] < 123
                ][-10:]
            ],
            "protected_neighbor_anchors": ["MT_CHIMNEY_TABITHA", "MT_CHIMNEY_MAXIE", "PETALBURG_GYM_NORMAN", "MAGMA_HIDEOUT_MAXIE"],
            "required_preimplementation_review": "Complete: the previous ten contain no After You, Fire-native Trick Room, Magma Storm, or Mega Emboar collision. Recent weatherless Fire drills make Flannery's first weather and full fast-slow boss formation an escalation rather than repetition.",
        },
        "identity": {
            "memory_hook": "Flannery can pull the slowest flame to the front, turn the order backward for Skeledirge, seal one exit with Heatran, then end the heat lesson physically through Mega Emboar.",
            "story_fit": "The hot-spring Gym becomes temperature timing rather than generic sun: steam can surge immediately, settle into a slow furnace, trap a cooling answer, and erupt as physical heat at the finish.",
            "primary_player_question": "Can the player damage or disrupt Flannery's HP-sensitive Eruption engines while navigating After You and one justified Trick Room reversal, then preserve Water or Ground pressure for Air Balloon Heatran and Mega Emboar?",
            "primary_mode": "Drought Torkoal and Chlorophyll Lilligant create visible After You or Helping Hand Eruption pressure without Sleep Powder, Fake Out, or redirection.",
            "secondary_mode": "Pyromancy Delphox may set Trick Room for Skeledirge only when speed state warrants it; Heatran bridges through Magma Storm before Mega Emboar's physical finale.",
            "preview_pressure": "Five Fire types and one visible Grass support advertise sun and anti-Water coverage, but the player must distinguish fast After You heat, slow Trick Room heat, a Balloon trap, and one physical Mega.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 41 through 44 against cap 40, immediate sun-amplified Eruption, two visible speed modes, Water and Ground counter-coverage, Flash Fire pivots, a finite Magma Storm trap, mixed categories, and one aggressive Mega. Water, Ground, Rock, weather replacement, HP pressure, Taunt, Wide Guard, and speed denial all remain broad answers.",
            "pressure_sources": [
                "Drought plus After You or Helping Hand enabling high-HP Torkoal Eruption",
                "Lilligant Solar Beam threatening obvious Water responses without sleep variance",
                "Mental Herb Pyromancy Delphox providing one conditional Trick Room and burn threat",
                "Unaware Skeledirge using Torch Song and one finite Throat Spray under the slow mode",
                "Air Balloon Heatran Magma Storm, Steel coverage, and Fire immunity",
                "Mega Emboar physical Heat Crash, Fighting, Ground, and Electric coverage",
            ],
            "resource_tax": "This standalone Gym boss taxes weather control, immediate HP pressure, Wide Guard, Taunt, speed reversal, Water/Ground preservation, Balloon removal, and mixed bulk rather than prior-battle attrition.",
            "tuning_order": [
                "Preserve After You Eruption, conditional Fire-native Trick Room, finite Heatran trap, and sole physical Mega climax",
                "Test joint lead scoring, Eruption HP awareness, and Trick Room predicates before changing sets",
                "Adjust offsets within +1 to +4, beginning with Emboar, Heatran, and Skeledirge",
                "Then adjust Torkoal or Delphox bulk",
                "Change moves or species only after Hard/Medium/Easy level testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_TORKOAL", "SPECIES_LILLIGANT"],
            "mandatory_order_reason": "The lead visibly establishes sun and move-order manipulation. Later slow and trap formations are board-state preferences, not scripted reserve waves.",
            "reserve_sequence": [
                "Prefer Delphox when opposing speed or remaining Skeledirge/Emboar value justifies Trick Room; use independent attacks or burn when reversal is wrong.",
                "Prefer Skeledirge beside established Trick Room or when Unaware and its finite sound-move snowball create the strongest visible line.",
                "Use Heatran to absorb visible Fire, punish Steel or Grass answers, or trap a disclosed defensive pivot; account for Balloon publicly.",
                "Preserve Mega Emboar as the sole physical climax when practical, but deploy it earlier when it is the only healthy matchup-correct reserve.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"],
            "required_flags": ["AI_FLAG_COMBO_SETUP", "AI_FLAG_HP_AWARE"],
            "custom_requirements": [
                "Score Torkoal and Lilligant actions jointly: After You requires a slower healthy partner, meaningful same-turn attack value, and survivable visible board; Helping Hand requires superior damage value without redundant support.",
                "Use Eruption damage from current HP and avoid it when Heat Wave, Earth Power, Body Press, or direct coverage creates the stronger visible line.",
                "Set Trick Room only when the opposing effective board is faster or slow Fire reserves dominate; never reverse a winning Chlorophyll or After You state.",
                "Use a Flannery reserve selector that recognizes fast heat, slow heat, Heatran trap, and Mega Emboar physical roles without forcing missing pairs.",
                "Value Magma Storm through real accuracy and trap payoff, and Mega Evolve Emboar when active without manufacturing a safe sweep.",
            ],
            "forbidden_behaviors": [
                "Do not use Sleep Powder or rely on low-accuracy status to reach target difficulty.",
                "Do not set Trick Room merely because Delphox is active or while the fast sun mode is winning.",
                "Do not spam low-HP Eruption or Magma Storm without visible expected value.",
                "Do not inspect hidden Water, Ground, Rock, weather, item, or switch choices.",
                "Do not add Primal Groudon, another Mega, redirection, or Magma's complete positioning shell.",
            ],
            "state_machine": "Mode A is Torkoal-Lilligant sun plus After You or Helping Hand. Mode B becomes eligible only when visible speed state justifies Delphox Trick Room beside Skeledirge or another slow reserve. Mode C uses Heatran as a finite trap and immunity bridge. Mode D exposes Mega Emboar as the sole physical climax. Every mode allows independent attacks and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Replace or suppress sun, use Cloud Nine, weather abilities, Rain Dance, Sand, or Hail, and pressure Torkoal's HP so Eruption loses force.",
                "Taunt, Fake Out, priority, spread damage, double-targeting, Encore, or speed reversal can break Lilligant and Delphox before their order manipulation succeeds.",
                "Wide Guard, Protect, Flash Fire, Water Absorb, Thick Fat, Rock, Water, Ground, and specially bulky answers can trade into Heat Wave or Eruption while targeting the partner.",
                "Break Heatran's visible Air Balloon before Ground pressure, pivot out of Magma Storm when legal, or use immediate Water/Fighting damage.",
                "Preserve physical Intimidate, burn, bulky Water/Ground, priority, or speed control for Mega Emboar's no-Protect finale.",
            ],
            "intentional_weakness": "Five members retain Fire typing and real Water, Ground, or Rock pressure. Lilligant is frail and the only off-type; Delphox is the only Trick Room setter; Heatran's Ground immunity is a visible one-use Balloon; Mega Emboar has no Protect, setup, recovery, or priority. There is no redirection, Fake Out, sleep, screen, or permanent trap loop.",
            "first_loss_lesson": "Damage Torkoal before After You converts Eruption, decide whether to deny Delphox or reverse its Trick Room, break Heatran's Balloon before committing Ground pressure, and keep a physical Water/Ground or Intimidate answer for Mega Emboar instead of spending everything on the sun lead.",
            "revealed_information": [
                "Drought, Chlorophyll speed, current HP, Eruption damage, Trick Room, Air Balloon, Flash Fire, and Mega evolution are all public battle state.",
                "After You and Helping Hand reveal their selected support target through ordinary move execution.",
                "Magma Storm accuracy and trapping remain ordinary visible mechanics; no guaranteed custom hit is proposed.",
                "Mega Emboar is the sole Mega and carries no hidden setup phase.",
            ],
            "unacceptable_failure_modes": [
                "AI uses low-HP Eruption instead of a stronger visible move",
                "After You targets an incapacitated, faster, or non-attacking partner",
                "Trick Room reverses Flannery's own winning fast mode",
                "Magma Storm is treated as guaranteed or creates a passive stall loop",
                "The team becomes indistinguishable from Maxie through Primal, redirection, or a complete faction sun core",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Torkoal Lilligant After You tournament", "Skeledirge Unaware Torch Song", "Heatran positioning trap", "Mega Emboar Champions doubles", "anti rain sun balance"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Sleep Powder variance is removed from Lilligant.",
                "Cresselia, Volcanion, Primal Groudon, redirection, Fake Out, and full faction sun shells are not imported.",
                "Substitute stall, confusion, unrelated sand or snow, and multiple setup recipients are rejected.",
                "No Tera, Z-Move, Dynamax, Gigantamax, second Mega, or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Tournament-proven Torkoal-Lilligant After You sun",
                "Fire-native Trick Room through Delphox and Skeledirge",
                "Air Balloon Flash Fire Heatran positioning",
                "Reckless Mega Emboar as immediate physical pressure",
                "Dual-speed preview without importing a second complete mode roster",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Flannery's definitive After You Eruption Gym lead",
                "Fire-native Delphox-Skeledirge slow formation",
                "Heatran Magma Storm thermal bridge",
                "Mega Emboar as Flannery's signature and only Mega",
            ],
            "preserves": [
                "Maxie's elite sun positioning, Primal, and alternate faction doctrine",
                "Tate and Liza's Cresselia cosmic reversal",
                "Juan's Volcanion Surf absorption",
                "Norman's singles discipline",
                "Wallace and Archie rain identities",
            ],
            "releases": [
                "Cresselia and Volcanion are released from main-story Flannery",
                "Other Fire legends and sun teams remain available if they do not duplicate After You plus Fire-native Trick Room",
            ],
            "collision_notes": [
                "No species overlaps Norman, Winona, Tate and Liza, Juan, or the five main-story League teams.",
                "Five of six members visibly retain Fire typing; Lilligant's After You and Solar Beam job earns the only exception.",
                "Sun overlaps Magma only as an environmental resource. Flannery's question is HP and move-order timing, not faction positioning or Primal pressure.",
            ],
        },
        "presentation": {
            "intro_concept": "Flannery admits she once thought heat meant attacking harder; now she asks whether the player can read when a flame will surge first, settle backward, seal an exit, or crash down physically.",
            "defeat_concept": "She recognizes that the player controlled temperature and timing rather than merely bringing Water types.",
            "post_battle_concept": "The Heat Badge and native reward flow remain unchanged. Her speech distinguishes her hot-spring discipline from Team Magma's ambitions.",
            "hint_concept": "The Gym guide warns that a flower can pull the slowest flame forward, a ghost can reverse the order, a floating furnace must be grounded, and the final boar carries no shield.",
            "native_width_status": "pass; exact intro, defeat, post-battle, and Gym-guide text are source-authored and checked against the native line budget",
            "guide_summary": "Document cap 40, Torkoal-Lilligant After You Eruption without sleep, Delphox-Skeledirge conditional Trick Room, Air Balloon Heatran Magma Storm, Mega Emboar physical finale, HP-aware AI, broad weather and type counterplay, historic references, and Hard/Medium/Easy offsets.",
        },
        "author_self_check": {
            "strongest_part": "The same slow Fire archetype can move first through After You or last through Trick Room, making temperature timing the real puzzle before a completely physical Mega Emboar finish.",
            "weakest_link": "After You plus Eruption is a famous core and could feel borrowed rather than bespoke; removing sleep, tying every later formation to Lavaridge's thermal story, and preserving Magma's complete positioning doctrine are necessary distinctions.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "pass",
            "script_and_format": "pass",
            "dialogue_width": "pass",
            "guide": "pass",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Flannery as a required six-Pokemon double at badge count three and strict cap 40.",
                "Five of six proposed species retain Fire type, and every item, move, spread, and selected ability slot exists and passes local legality.",
                "Emboarite maps Emboar to Mega Emboar, and no second Mega candidate appears.",
                "AI contains joint After You scoring, attacker-HP Eruption scoring, conditional Trick Room control, and a Flannery-only reserve formation selector.",
                "All selected competitive references exist in the current 1005-record index, including two tournament-winning Torkoal-Lilligant teams.",
                "The required source party, AI flags, formation selector, dialogue, Gym-guide hint, and source-derived guide match the protected anchor. No real-ROM battle has been run.",
            ],
            "source_blockers": [],
        },
        "mechanics_proposal": None,
    }


def wattson_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:luca-ceribelli:worlds-2024": (
            "selected-architecture",
            "The 2024 World Champion roster proves that Electric Terrain, Iron Hands, Farigiraf, Tailwind, and Trick Room can form one coherent fast-slow structure. Wattson translates the architecture into a five-Electric Hoenn circuit and imports none of its restricted or Tera assumptions.",
        ),
        "elite:wolfe:worlds-2016": (
            "adapted-positioning",
            "Wolfe Glick's World-winning Raichu positioning validates Lightning Rod, move-order control, and a late electric pivot. Wattson preserves readable protection and speed ideas but uses a single No Guard Mega Raichu Y instead of Primal rain or multiple Mega candidates.",
        ),
        "showdown:gen9championsrandomdoublesbattle:009": (
            "adapted-role",
            "The Champions generator validates Emolga as a real doubles support attacker rather than mascot filler. Wattson substitutes Tailwind, Encore, Helping Hand, and Lightning Rod for a route-specific circuit-control role.",
        ),
        "showdown:gen4randombattle:003": (
            "adapted-set",
            "The generated Ampharos set supplies Focus Blast and pivot-capable special pressure. Wattson turns Ampharos into a slow Choice Specs circuit breaker with locally legal Dragon and Rock coverage.",
        ),
        "vgc:ocic-2017": (
            "selected-history",
            "The 2017 Oceania International Champion roster validates Tapu Koko beside a slower Electric/Steel attacker. Wattson takes only the fast-terrain-versus-slow-voltage contrast, not the complete winning roster.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "The current design is already excellent, but two Normal/Psychic utility slots make Wattson read less clearly as an Electric specialist, Porygon2 duplicates a generic competitive glue role, and offsets +2 through +5 are needlessly opaque now that live difficulty exists. Farigiraf absorbs conditional Trick Room while Ampharos becomes the fifth Electric member.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_TAPU_KOKO",
            "level_offset": 1,
            "item": "ITEM_TERRAIN_EXTENDER",
            "ability": "ABILITY_ELECTRIC_SURGE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_WILD_CHARGE", "MOVE_NATURES_MADNESS", "MOVE_U_TURN", "MOVE_TAUNT"],
            "role": "Visible power-grid lead: establishes Electric Terrain, denies setup, cuts bulky targets, and can hand the live circuit to a reserve.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_EMOLGA",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_LIGHTNING_ROD",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_ENCORE", "MOVE_HELPING_HAND", "MOVE_THUNDERBOLT"],
            "role": "Frail aerial switchboard that protects its partner from Electric attacks and chooses among Tailwind, Encore, amplification, or direct voltage.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_IRON_HANDS",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_QUARK_DRIVE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_FAKE_OUT", "MOVE_CLOSE_COMBAT", "MOVE_WILD_CHARGE", "MOVE_ICE_PUNCH"],
            "role": "Terrain-charged physical relay whose Fake Out buys one visible timing turn and whose coverage punishes passive Ground assumptions.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_FARIGIRAF",
            "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_ARMOR_TAIL",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_TRICK_ROOM", "MOVE_TWIN_BEAM", "MOVE_LIGHT_SCREEN", "MOVE_PROTECT"],
            "role": "The one earned off-type circuit controller: blocks priority, can reverse speed for the heavy relays, and screens only when reversal is wrong.",
            "lead_group": "board-state-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_AMPHAROS",
            "level_offset": 3,
            "item": "ITEM_CHOICE_SPECS",
            "ability": "ABILITY_MOLD_BREAKER",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_THUNDERBOLT", "MOVE_DRAGON_PULSE", "MOVE_FOCUS_BLAST", "MOVE_POWER_GEM"],
            "role": "Slow high-voltage breaker for Farigiraf's reversed circuit; Mold Breaker and four attacks prevent a passive Trick Room loop.",
            "lead_group": "slow-mode-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_RAICHU",
            "level_offset": 4,
            "item": "ITEM_RAICHUNITE_Y",
            "ability": "ABILITY_STATIC",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDER", "MOVE_FOCUS_BLAST", "MOVE_GRASS_KNOT", "MOVE_PROTECT"],
            "role": "Wattson's sole Mega and final overvoltage: No Guard converts Thunder and Focus Blast from gambles into exact endgame threats.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MAUVILLE_GYM_WATTSON",
        "planning_tier": "badge_boss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Third Gym Leader after Dewford, Granite Cave, Slateport, Route 110, and the rival battle",
            "location": "MauvilleCity_Gym",
            "strict_cap": 30,
            "player_tools": [
                "Two Badges and all catch pools through Granite Cave, Slateport, Route 110, and Mauville",
                "The reusable Leveler, every legal move source, and on-demand legal ability switching",
                "Free ordinary competitive held items plus all progression items earned before Mauville",
                "Mega Bracelet access established in Granite Cave and any Mega Stones obtainable before Wattson",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Reciprocal Mega access is established. Wattson uses exactly one Mega Raichu Y and no other transformation.",
            "evolution_phase": "Early-mid campaign transition: natural level-30 evolutions are fair. Ampharos evolves at 30 and Farigiraf appears at 32, while single-stage rare threats remain appropriate for a target-10 Gym boss.",
            "preparation_access": "Full PC, Center teacher, ability, item, and leveling access is available immediately before the Gym. Switch puzzles do not create party attrition.",
            "gauntlet_position": "Third Badge boss and first mature reciprocal-Mega systems exam. It teaches field state, fast and slow order, priority denial, and exact Mega payoff without reserving later cosmic or faction versions of those ideas.",
            "mechanics_baseline_id": "gym_main_story",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_WATTSON_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "main_story_wattson", "trainer_ids": ["TRAINER_WATTSON_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "postgame_daily_rematch", "trainer_ids": ["TRAINER_WATTSON_5"], "format": "double", "scope": "deferred-to-rematch-phase", "reachability": "current rematch branch"},
                {"variant_id": "declared_rematch_modes", "trainer_ids": ["TRAINER_WATTSON_2", "TRAINER_WATTSON_3", "TRAINER_WATTSON_4", "TRAINER_WATTSON_5"], "format": "mixed", "scope": "deferred-to-rematch-phase", "reachability": "requires separate postgame audit"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Wattson1",
                "src/data/trainers.h:TRAINER_WATTSON_1",
                "data/maps/MauvilleCity_Gym/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Mauville, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["ROUTE_110_RIVAL", "NEW_MAUVILLE_WATTSON", "LAVARIDGE_GYM_FLANNERY", "MOSSDEEP_GYM_TATE_AND_LIZA"],
            "required_preimplementation_review": "Refresh the final ten Route 110 and Mauville encounters. Preserve Wattson's five-Electric circuit, conditional dual speed, priority denial, and No Guard Mega Raichu unless those exact lessons cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Wattson turns on the grid with Koko, routes current through airborne Emolga, flips the circuit through Farigiraf, powers a slow Ampharos transformer, then deliberately overloads Mega Raichu Y.",
            "story_fit": "The switch Gym becomes an electrical-engineering exam: generation, routing, phase reversal, insulation, and controlled overvoltage each have a visible battle analogue.",
            "primary_player_question": "Can the player identify whether Wattson's live circuit is fast or reversed, deny the correct controller rather than merely attacking Electric weaknesses, and preserve a Ground or special-bulk answer for No Guard Mega Raichu Y?",
            "primary_mode": "Tapu Koko plus Emolga establishes visible Electric Terrain and contestable Tailwind, with Taunt, Encore, Lightning Rod, and U-turn making the opening about routing rather than raw spread damage.",
            "secondary_mode": "Farigiraf can block priority and conditionally reverse speed for Iron Hands and Ampharos; Mega Raichu Y then returns the finale to fast exact-accuracy offense.",
            "preview_pressure": "Five Electric types make the specialty honest, but Ground is not an automatic answer: Emolga is immune, Iron Hands has Ice Punch, Ampharos is Dragon-typed locally, and Mega Raichu carries Grass Knot.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 31 through 34 against cap 30, one rare terrain lead, two contestable speed states, priority denial, physical and special pressure, and one 160 Special Attack No Guard Mega. The team still exposes broad Ground, spread, Taunt, screen-breaking, weather/terrain replacement, and speed-counterplay routes.",
            "pressure_sources": [
                "Extended Electric Terrain with immediate Taunt, Nature's Madness, and pivot pressure",
                "Focus Sash Emolga choosing among Tailwind, Encore, Helping Hand, and direct damage",
                "Terrain-activated Assault Vest Iron Hands with Fake Out and anti-Ground Ice coverage",
                "Armor Tail Farigiraf providing one conditional Trick Room and Light Screen",
                "Choice Specs Mold Breaker Ampharos applying slow-mode special pressure",
                "No Guard Mega Raichu Y making Thunder and Focus Blast exact threats",
            ],
            "resource_tax": "This standalone boss taxes terrain control, target priority, speed-mode recognition, Ground positioning, mixed bulk, and ace preservation rather than prior-route healing resources.",
            "tuning_order": [
                "Preserve the five-Electric circuit, conditional two-speed structure, and No Guard Mega Raichu payoff",
                "Test joint lead support scoring, Trick Room predicates, and Mega accuracy before changing sets",
                "Adjust offsets within +1 to +4, beginning with Raichu, Ampharos, and Iron Hands",
                "Then adjust Farigiraf or Emolga bulk",
                "Change moves or species only after Hard/Medium/Easy level testing",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_TAPU_KOKO", "SPECIES_EMOLGA"],
            "mandatory_order_reason": "The fixed lead visibly establishes generation and routing. Farigiraf, the heavy relays, and the Mega are selected by board state rather than forced into scripted waves.",
            "reserve_sequence": [
                "Use Iron Hands when Fake Out, physical pressure, or terrain activation creates immediate value; do not preserve it solely for a planned pair.",
                "Prefer Farigiraf when opposing priority matters or the visible speed relation makes reversal favorable; use Twin Beam or Light Screen when Trick Room is wrong.",
                "Prefer Ampharos under favorable reversal or when Mold Breaker coverage is the strongest disclosed line; account for its Choice lock.",
                "Preserve Mega Raichu Y as the overvoltage climax when practical, but deploy it earlier if its exact coverage is the only matchup-correct reserve.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "required_flags": [],
            "custom_requirements": [
                "Score Koko and Emolga jointly: Tailwind, Taunt, Encore, Helping Hand, and U-turn require visible board value and may not stack redundant support when direct damage is superior.",
                "Set Trick Room only when the opposing effective board is faster and Iron Hands or Ampharos gains meaningful action order; never reverse a winning Tailwind or fast Mega state.",
                "Recognize Armor Tail and Lightning Rod when valuing priority and Electric targets, including ally-protection rather than hidden player intent.",
                "Use a Wattson reserve selector for fast grid, heavy relay, reversed transformer, and Mega overvoltage roles with independent missing-partner fallbacks.",
                "Mega Evolve Raichu when active and evaluate Thunder and Focus Blast with No Guard only after the transformation is actually selected.",
            ],
            "forbidden_behaviors": [
                "Do not Tailwind and Trick Room blindly or layer support when a visible knockout is available.",
                "Do not use Encore, Taunt, or Light Screen based on hidden player choices.",
                "Do not treat Raichu's inaccurate attacks as guaranteed before Mega evolution.",
                "Do not add Miraidon, Tera, a second Mega, redirection, or Tate and Liza's complete cosmic reversal structure.",
            ],
            "state_machine": "Mode A is Koko-Emolga fast terrain routing. Mode B brings Iron Hands for physical tempo. Mode C becomes eligible only when Farigiraf's priority denial or Trick Room improves the visible board and can feed Ampharos. Mode D Mega Evolves Raichu Y into exact-accuracy overvoltage. Each mode retains independent attacks and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Replace Electric Terrain, use Ground types or Ground spread pressure, or exploit Koko's recoil and Emolga's Sash dependence before the grid stabilizes.",
                "Taunt, Encore, Fake Out where Armor Tail is absent, priority, double-targeting, or opposing Tailwind and Trick Room can contest each visible order controller.",
                "Use Wide Guard where relevant, Lightning Rod or Volt Absorb, specially bulky Grass/Dragon/Ground answers, screen removal, Snarl, or careful Protect sequencing against the special circuit.",
                "Exploit Ampharos's Choice lock and Farigiraf's single reversal; force the wrong speed state rather than solving both modes with one fragile sweeper.",
                "Preserve Ground, special bulk, priority outside Armor Tail, weather/terrain replacement, or a faster controlled knockout for Mega Raichu Y after No Guard is revealed.",
            ],
            "intentional_weakness": "Five members share Electric typing and meaningful Ground exposure. Emolga is frail; Farigiraf is the only Trick Room setter and sole off-type; Ampharos is Choice-locked; Iron Hands lacks Protect; Mega Raichu is physically frail. There is no redirection, recovery loop, sleep, permanent trap, or second transformation.",
            "first_loss_lesson": "Do not treat the team as six Electric targets. Break the Koko-Emolga routing, decide whether Farigiraf should be denied or allowed to reverse into your own slow answer, exploit Ampharos's lock, and keep the correct Ground or special-bulk piece for Mega Raichu's exact Thunder and Focus Blast.",
            "revealed_information": [
                "Electric Terrain, Tailwind, Trick Room, Armor Tail priority denial, Choice lock, and Mega evolution are public battle state.",
                "Lightning Rod and Ground immunity reveal through ordinary targeting rules; the AI receives no advance knowledge of intended targets.",
                "No Guard applies only after Mega Raichu Y appears, so pre-Mega accuracy remains ordinary.",
                "There is exactly one Mega and one Trick Room setter.",
            ],
            "unacceptable_failure_modes": [
                "AI stacks Tailwind and Trick Room without visible justification",
                "Emolga spends every turn supporting when Thunderbolt can secure a knockout",
                "Farigiraf chooses Trick Room while the fast grid is winning",
                "Raichu's Thunder or Focus Blast receives No Guard accuracy before Mega evolution",
                "The team reads as Luca Ceribelli's roster with names swapped rather than Wattson's electrical Gym",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Electric Terrain dual speed world champion", "Tapu Koko doubles", "Raichu world champion positioning", "Emolga Champions random doubles", "Ampharos competitive set"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Miraidon, Tera, Primal weather, multiple Mega candidates, redirection, and restricted-pair assumptions are not imported.",
                "Porygon2 is removed so Farigiraf owns reversal and five members visibly keep Wattson's type.",
                "Nuzzle spam, sleep, evasion, paralysis dependence, and passive recovery loops are rejected.",
                "No Z-Move, Dynamax, Gigantamax, second Mega, or unsupported transformation appears.",
            ],
            "imported_elements": [
                "World-champion electric-terrain fast/slow architecture",
                "World-winning Raichu positioning and priority-aware control",
                "Champions-generator Emolga legitimacy as a doubles utility attacker",
                "Generated Ampharos special coverage adapted into a local slow transformer",
                "Historic Tapu Koko fast pressure beside a slower Electric attacker",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Wattson's definitive Electric Terrain and airborne routing lead",
                "Farigiraf as the early priority-denial and conditional reversal lesson",
                "Choice Specs Ampharos as the slow transformer",
                "Mega Raichu Y No Guard overvoltage as Wattson's signature climax",
            ],
            "preserves": [
                "Tate and Liza's three-formation cosmic reversal and Calyrex-Ice payoff",
                "Later faction terrain, weather, and restricted-legend positioning",
                "Wolfe's complete 2016 championship structure for a later historic homage",
                "Mega Raichu X and physical Electric Surge for another encounter",
            ],
            "releases": [
                "Porygon2 is released from main-story Wattson for later Eviolite or Trick Room teams",
                "Magnezone, Rotom forms, Electivire, and the wider Electric roster remain available for route trainers, New Mauville, rematches, and postgame",
            ],
            "collision_notes": [
                "No species overlaps Flannery, Norman, Winona, Tate and Liza, Juan, or the five main-story League teams.",
                "Five of six members visibly retain Electric typing; Farigiraf's Armor Tail and reversal job earns the only exception.",
                "Wattson introduces dual speed at small scale. Tate and Liza retain the later cosmic three-formation version, not a duplicate of this electrical routing exam.",
            ],
        },
        "presentation": {
            "intro_concept": "Wattson welcomes the player to a live circuit: power means nothing unless it is generated, routed, reversed, insulated, and released at the right moment.",
            "defeat_concept": "He laughs that the player did not merely ground the voltage; they read the whole circuit and interrupted the correct switch.",
            "post_battle_concept": "The Dynamo Badge and native reward flow remain unchanged. Wattson frames Mega Raichu's overload as controlled engineering rather than a hidden trick.",
            "hint_concept": "The Gym guide warns that the bird routes lightning, the long-necked switch blocks priority and can reverse order, the slow sheep hits through safeguards, and Raichu becomes perfectly accurate only after Mega evolution.",
            "native_width_status": "concept-only; exact intro, defeat, guide, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 30, Koko-Emolga terrain and Tailwind routing, Iron Hands physical relay, Farigiraf conditional priority denial and Trick Room, Choice Specs Ampharos transformer, No Guard Mega Raichu Y climax, and Hard/Medium/Easy offsets.",
        },
        "author_self_check": {
            "strongest_part": "Every member reads as a different part of an electrical circuit, and Mega Raichu Y's No Guard changes Thunder and Focus Blast from risky moves into a visible mechanical climax.",
            "weakest_link": "Fast terrain plus slow reversal comes from a famous modern championship architecture; the five-Electric typing, Emolga routing, Ampharos transformer, single earned Farigiraf exception, and exact Mega payoff must remain visible so the battle belongs to Wattson.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The current guide identifies Wattson as a required six-Pokemon double at badge count two and strict cap 30.",
                "Five of six proposed species retain Electric type; Ampharos and Farigiraf reach their local evolution conditions at the authored opponent levels.",
                "Every item, move, spread, and selected ability slot exists and passes local legality; Raichunite Y maps Raichu to No Guard Mega Raichu Y.",
                "Current AI already has the broad combo, HP, speed, partner, and field flags but needs joint lead, Armor Tail, conditional Trick Room, and post-Mega accuracy regression coverage.",
                "All selected competitive references exist in the current 1005-record index and include two documented World Champion structures.",
                "No game source, exact dialogue, or guide party has been changed, and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Wattson1 with the exact six authored sets and offsets.",
                "Implement joint Koko-Emolga action scoring and Wattson's board-state reserve selector.",
                "Implement conditional Farigiraf Trick Room plus all fast, slow, and missing-partner fallbacks.",
                "Regression-test Electric Terrain, Tailwind, Encore, Lightning Rod, U-turn, Quark Drive, Fake Out, Armor Tail, Trick Room, Choice lock, Mega Raichu Y, No Guard timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue; update the source-derived guide and campaign collision notes.",
                "Run cap-30 Ground, Lightning Rod, Volt Absorb, terrain replacement, fast, slow, mixed-bulk, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def gym_board_review(designs: dict[str, dict]) -> dict:
    league_species = {
        mon["species"]
        for dossier in json.loads(MARQUEE_PATH.read_text())["designs"].values()
        for mon in dossier["team"]
    }
    types = species_types()
    species_uses: dict[str, list[str]] = {}
    reference_uses: dict[str, list[str]] = {}
    mega_signatures = []
    specialty_counts = {}
    questions = {}
    for anchor_id, dossier in designs.items():
        questions[anchor_id] = dossier["identity"]["primary_player_question"]
        specialty = SPECIALTY_TYPES[anchor_id]
        specialty_counts[anchor_id] = sum(specialty in types.get(mon["species"], set()) for mon in dossier["team"])
        for mon in dossier["team"]:
            species_uses.setdefault(mon["species"], []).append(anchor_id)
            if mon["mega_candidate"]:
                mega_signatures.append({"anchor_id": anchor_id, "species": mon["species"], "item": mon["item"]})
        for reference_id in dossier["competitive_research"]["selected_reference_ids"]:
            reference_uses.setdefault(reference_id, []).append(anchor_id)
    return {
        "status": "pass",
        "scope": "Six backward-designed main-story Gym anchors against each other and the five-member League arc; source implementation and runtime difficulty remain separate gates.",
        "gym_anchor_count": len(designs),
        "unique_species_count": len(species_uses),
        "gym_species_collisions": {species: anchors for species, anchors in species_uses.items() if len(anchors) > 1},
        "league_species_collisions": {species: anchors for species, anchors in species_uses.items() if species in league_species},
        "mega_signatures": mega_signatures,
        "specialty_type_member_counts": specialty_counts,
        "primary_questions": questions,
        "reused_reference_ids": {reference_id: anchors for reference_id, anchors in reference_uses.items() if len(anchors) > 1},
        "judgment": "The board spends six different species rosters, six different Megas, and six different primary questions. Shared competitive references are evidence, not allocations; any reuse remains acceptable only when the imported interaction and battle lesson differ.",
    }


def build() -> dict:
    meta = json.loads(META_PATH.read_text())
    records = {record["reference_id"]: record for record in competitive.load_records()}
    source_teams = {team["trainer_id"]: team for team in quality.audit()["teams"]}
    designs = {
        "SOOTOPOLIS_GYM_JUAN": juan_design(meta, records, source_teams["TRAINER_JUAN_1"]),
        "MOSSDEEP_GYM_TATE_AND_LIZA": tate_liza_design(meta, records, source_teams["TRAINER_TATE_AND_LIZA_1"]),
        "FORTREE_GYM_WINONA": winona_design(meta, records, source_teams["TRAINER_WINONA_1"]),
        "PETALBURG_GYM_NORMAN": norman_design(meta, records, source_teams["TRAINER_NORMAN_1"]),
        "LAVARIDGE_GYM_FLANNERY": flannery_design(meta, records, source_teams["TRAINER_FLANNERY_1"]),
        "MAUVILLE_GYM_WATTSON": wattson_design(meta, records, source_teams["TRAINER_WATTSON_1"]),
    }
    return {
        "version": 1,
        "title": "Emerald Champions backward Gym anchor designs",
        "phase": "gyms_backward_juan_to_wattson",
        "expected_order": EXPECTED_ORDER,
        "designed_count": len(designs),
        "remaining_count": len(EXPECTED_ORDER) - len(designs),
        "designs": designs,
        "gym_board_review": gym_board_review(designs),
    }


def species_types() -> dict[str, set[str]]:
    source = doubles.select_rebalanced(
        (ROOT / "src/data/pokemon/base_stats.h").read_text()
        + "\n"
        + (ROOT / "src/data/pokemon/verdant_gen9_base_stats.h").read_text()
    )
    result = {}
    pattern = re.compile(r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{(.*?)(?=^\s*\[SPECIES_|\Z)", re.M | re.S)
    for match in pattern.finditer(source):
        types = set(re.findall(r"\.type[12]\s*=\s*(TYPE_[A-Z0-9_]+)", match.group(2)))
        result[match.group(1)] = types
    return result


def validate(payload: dict) -> None:
    operating_system = json.loads(OS_PATH.read_text())
    contract = operating_system["dossier_contract"]
    expected_prefix = EXPECTED_ORDER[: payload["designed_count"]]
    if list(payload["designs"]) != expected_prefix:
        raise AssertionError("Gym anchors are not being designed backward in canonical requested order")
    league_species = {
        mon["species"]
        for dossier in json.loads(MARQUEE_PATH.read_text())["designs"].values()
        for mon in dossier["team"]
    }
    dex = presets.LocalDex()
    abilities = doubles.base_ability_slots()
    types = species_types()
    item_tokens = set(re.findall(r"#define\s+(ITEM_[A-Z0-9_]+)", (ROOT / "include/constants/items.h").read_text()))
    spread_tokens = set(re.findall(r"#define\s+(SPREAD_[A-Z0-9_]+)", (ROOT / "include/constants/spreads.h").read_text()))
    refs = {record["reference_id"] for record in competitive.load_records()}
    mega_source = (ROOT / "src/data/pokemon/evolution.h").read_text() + (ROOT / "src/data/pokemon/verdant_gen9_evolutions.h").read_text()
    gym_species: dict[str, str] = {}
    mega_signatures: set[tuple[str, str]] = set()
    primary_questions: set[str] = set()

    for anchor_id, dossier in payload["designs"].items():
        for field in contract["required_top_level"]:
            if field not in dossier:
                raise AssertionError(f"{anchor_id} missing {field}")
        for section, required_key in (
            ("campaign_state", "campaign_state_required"),
            ("runtime", "runtime_required"),
            ("rolling_context", "rolling_context_required"),
            ("identity", "identity_required"),
            ("difficulty", "difficulty_required"),
            ("ordering", "ordering_required"),
            ("ai", "ai_required"),
            ("counterplay", "counterplay_required"),
            ("competitive_research", "competitive_research_required"),
            ("campaign_reservations", "reservations_required"),
            ("presentation", "presentation_required"),
            ("verification", "verification_required"),
            ("author_self_check", "author_self_check_required"),
        ):
            missing = set(contract[required_key]) - set(dossier[section])
            if missing:
                raise AssertionError(f"{anchor_id}.{section} missing {sorted(missing)}")
        if dossier["difficulty"]["target"] != 10 or dossier["difficulty"]["observed"] is not None:
            raise AssertionError(f"{anchor_id} difficulty status is dishonest")
        expected_status = (
            {"design": "design-complete", "source": "source-closed", "static": "source-validated", "runtime": "unplayed"}
            if anchor_id == "LAVARIDGE_GYM_FLANNERY"
            else {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"}
        )
        if dossier["status"] != expected_status:
            raise AssertionError(f"{anchor_id} status drifted")
        if anchor_id == "LAVARIDGE_GYM_FLANNERY":
            expected_sets = [
                {key: mon[key] for key in ("species", "item", "ability", "spread", "moves", "level_offset")}
                for mon in dossier["team"]
            ]
            if dossier["runtime"]["current_source_baseline"]["sets"] != expected_sets:
                raise AssertionError("Flannery source no longer matches the protected anchor")
        if len(dossier["team"]) != 6 or sum(mon["mega_candidate"] for mon in dossier["team"]) != 1:
            raise AssertionError(f"{anchor_id} must have six Pokemon and one Mega")
        question = dossier["identity"]["primary_player_question"]
        if question in primary_questions:
            raise AssertionError(f"{anchor_id} repeats another Gym's primary question verbatim")
        primary_questions.add(question)
        specialty = SPECIALTY_TYPES[anchor_id]
        if sum(specialty in types.get(mon["species"], set()) for mon in dossier["team"]) < 4:
            raise AssertionError(f"{anchor_id} no longer has a {specialty} majority")
        for mon in dossier["team"]:
            missing = set(contract["mon_required"]) - set(mon)
            if missing:
                raise AssertionError(f"{anchor_id} {mon.get('species')} missing {sorted(missing)}")
            if mon["species"] in league_species:
                raise AssertionError(f"{anchor_id} prematurely repeats League species {mon['species']}")
            if mon["species"] in gym_species:
                raise AssertionError(f"{anchor_id} repeats {mon['species']} from {gym_species[mon['species']]}")
            gym_species[mon["species"]] = anchor_id
            legal = dex.legal_moves(mon["species"])
            illegal = [move for move in mon["moves"] if move not in legal]
            if illegal:
                raise AssertionError(f"{anchor_id} {mon['species']} illegal moves {illegal}")
            slots = abilities.get(mon["species"], [])
            if mon["ability_slot"] >= len(slots) or slots[mon["ability_slot"]] != mon["ability"]:
                raise AssertionError(f"{anchor_id} {mon['species']} ability slot mismatch")
            if mon["item"] not in item_tokens or mon["spread"] not in spread_tokens:
                raise AssertionError(f"{anchor_id} {mon['species']} item or spread token missing")
            if len(mon["moves"]) != 4 or len(set(mon["moves"])) != 4:
                raise AssertionError(f"{anchor_id} {mon['species']} needs four distinct moves")
            if mon["mega_candidate"] and not re.search(
                rf"\[{mon['species']}\].*?EVO_MEGA_EVOLUTION,\s*{mon['item']}", mega_source, re.S
            ):
                raise AssertionError(f"{anchor_id} Mega pairing is not source-legal")
            if mon["mega_candidate"]:
                signature = (mon["species"], mon["item"])
                if signature in mega_signatures:
                    raise AssertionError(f"{anchor_id} repeats Mega signature {signature}")
                mega_signatures.add(signature)
        selected = dossier["competitive_research"]["selected_reference_ids"]
        if not selected or not set(selected) <= refs:
            raise AssertionError(f"{anchor_id} competitive references are missing")
        if len(dossier["counterplay"]["classes"]) < 3:
            raise AssertionError(f"{anchor_id} lacks broad counterplay")
        active_design = json.dumps({
            "identity": dossier["identity"],
            "team": dossier["team"],
            "ordering": dossier["ordering"],
            "ai": dossier["ai"],
        }).lower()
        if any(word in active_design for word in ("terastallization", "z-move", "dynamax", "gigantamax")):
            raise AssertionError(f"{anchor_id} imports an unsupported battle gimmick")

    review = payload["gym_board_review"]
    if review["status"] != "pass" or review["gym_anchor_count"] != len(EXPECTED_ORDER):
        raise AssertionError("Gym anchor-board review is incomplete")
    if review["unique_species_count"] != 36 or review["gym_species_collisions"] or review["league_species_collisions"]:
        raise AssertionError("Gym anchor-board species collision review failed")
    if len(review["mega_signatures"]) != 6 or len({(row["species"], row["item"]) for row in review["mega_signatures"]}) != 6:
        raise AssertionError("Gym anchor-board Mega review failed")
    if any(count < 4 for count in review["specialty_type_member_counts"].values()):
        raise AssertionError("Gym anchor-board specialty review failed")


def markdown(payload: dict) -> str:
    lines = [
        "# Emerald Champions backward Gym anchor designs",
        "",
        f"Progress: {payload['designed_count']}/{len(payload['expected_order'])} design-complete; Flannery source-closed and five later Gym anchors protected.",
        "",
    ]
    for anchor_id, dossier in payload["designs"].items():
        lines.extend([
            f"## {anchor_id}",
            "",
            f"- Status: design `{dossier['status']['design']}`, source `{dossier['status']['source']}`, runtime `{dossier['status']['runtime']}`.",
            f"- Format/cap: {dossier['runtime']['canonical_format']}, cap {dossier['campaign_state']['strict_cap']}, offsets {[mon['level_offset'] for mon in dossier['team']]}.",
            f"- Primary question: {dossier['identity']['primary_player_question']}",
            f"- Strongest part: {dossier['author_self_check']['strongest_part']}",
            f"- Weakest link: {dossier['author_self_check']['weakest_link']}",
            f"- First-loss lesson: {dossier['counterplay']['first_loss_lesson']}",
            f"- References: {', '.join(f'`{ref}`' for ref in dossier['competitive_research']['selected_reference_ids'])}",
            "- Team:",
        ])
        for mon in dossier["team"]:
            mega = "; Mega" if mon["mega_candidate"] else ""
            lines.append(
                f"  - `{mon['species']}` +{mon['level_offset']} — `{mon['item']}`, `{mon['ability']}`{mega}; "
                + ", ".join(f"`{move}`" for move in mon["moves"])
            )
        lines.extend(["", f"AI must execute: {' '.join(dossier['ai']['custom_requirements'])}", ""])
    if payload["remaining_count"]:
        lines.extend(["## Next backward anchor", "", f"`{payload['expected_order'][payload['designed_count']]}`", ""])
    else:
        review = payload["gym_board_review"]
        lines.extend([
            "## Six-Gym anchor-board review",
            "",
            f"- Unique species: {review['unique_species_count']}/36; Gym collisions: {len(review['gym_species_collisions'])}; League collisions: {len(review['league_species_collisions'])}.",
            f"- Unique Mega signatures: {len(review['mega_signatures'])}/6.",
            "- Specialty members: " + ", ".join(f"`{anchor}` {count}/6" for anchor, count in review["specialty_type_member_counts"].items()) + ".",
            f"- Judgment: {review['judgment']}",
            "",
        ])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payload = build()
    validate(payload)
    expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"
    expected_md = markdown(payload)
    if args.write:
        OUTPUT_JSON.write_text(expected_json)
        OUTPUT_MD.write_text(expected_md)
    if args.check:
        if not OUTPUT_JSON.exists() or OUTPUT_JSON.read_text() != expected_json:
            raise SystemExit("FAIL: Gym anchor JSON is missing or stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: Gym anchor Markdown is missing or stale")
    print(f"PASS: {payload['designed_count']}/{len(payload['expected_order'])} backward Gym anchors are design-complete and source-honest")
    print(f"NEXT: {payload['expected_order'][payload['designed_count']] if payload['remaining_count'] else 'campaign anchor review'}")


if __name__ == "__main__":
    main()
