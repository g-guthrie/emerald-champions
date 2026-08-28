#!/usr/bin/env python3
"""Generate and verify Emerald Champions rival/superboss marquee anchors."""

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


OUTPUT_JSON = ROOT / "docs/emerald_champions_superboss_anchor_designs.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_superboss_anchor_designs.md"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
LEAGUE_PATH = ROOT / "docs/verdant_marquee_battle_designs.json"
GYMS_PATH = ROOT / "docs/emerald_champions_gym_anchor_designs.json"
FACTIONS_PATH = ROOT / "docs/emerald_champions_faction_anchor_designs.json"
META_PATH = ROOT / "docs/competitive_team_index.meta.json"

EXPECTED_ORDER = [
    "STEVEN_METEOR_FALLS_SUPERBOSS",
    "CYNTHIA_MOSSDEEP_SUPERBOSS",
    "LEAF_ALTERING_CAVE_SUPERBOSS",
    "WALLY_VICTORY_ROAD",
    "LILYCOVE_RIVAL",
    "ROUTE_119_RIVAL",
    "STEVEN_MOSSDEEP_ALLY",
]

ALLOWED_PROTECTED_REUSES = {
    ("CYNTHIA_MOSSDEEP_SUPERBOSS", "SPECIES_GARCHOMP"): "Drake uses base Garchomp inside a Dragon League formation; Cynthia's iconic Mega Garchomp is her public opening ace and asks a distinct redirection-Earthquake question.",
    ("CYNTHIA_MOSSDEEP_SUPERBOSS", "SPECIES_MILOTIC"): "Cynthia's iconic base Competitive Milotic and Wallace's later Mega Milotic have different ownership, transformations, and battle jobs; the recognizable champion roster earns this single reprise.",
}

ALLOWED_INTERNAL_REUSES = {
    ("ROUTE_119_RIVAL", "SPECIES_BUTTERFREE"): "Butterfree is the rival's visible late-campaign signature; it returns as the Lilycove Mega after appearing in ordinary form on Route 119.",
    ("ROUTE_119_RIVAL", "SPECIES_BLAZIKEN"): "Blaziken is only the Fire counter-starter placeholder in the three source templates; runtime replaces it with the correct fully evolved counter from any of the 21 Gen 1-7 starters.",
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


def steven_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen9randomdoublesbattle:010": (
            "selected-role",
            "The generated Diancie roster validates Diamond Storm, body-defense pressure, and doubles speed-state relevance. Steven uses Helping Hand and no second speed mode so Diancie remains a visible mineral conductor.",
        ),
        "showdown:gen9randomdoublesbattle:005": (
            "selected-set",
            "The generated Magearna roster validates Soul-Heart and Trick Room in modern doubles. Steven makes it the sole conditional reversal and rejects Fake Out, hazards, and unrelated teammates.",
        ),
        "showdown:gen7randomdoublesbattle:010": (
            "selected-set",
            "The generated Kartana roster validates fast Beast Boost offense. Steven uses a public Choice Scarf and four direct attacks without redirection or setup.",
        ),
        "elite:wolfe:milwaukee-2025": (
            "selected-role",
            "Wolfe Glick's Milwaukee team validates Gholdengo as elite special pressure. Steven imports Make It Rain and immediate coverage but not Illusion, Tera, Fake Tears, or the complete champion shell.",
        ),
        "elite:ray-rizzo:worlds-2012": (
            "selected-history",
            "Ray Rizzo's World Champion roster validates Metagross as the ace of a highest-status doubles team. Steven keeps the iconic Mega but rejects Swagger, sand, and the full historic composition.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team repeats Courtney's Aerodactyl, Flannery's Heatran, and several passive hazards/recovery lines. Steven's level-100 superboss should be the ultimate rare mineral formation: one conditional reversal, mixed Steel pressure, finite stat drops, and iconic Mega Metagross.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_DIANCIE",
            "level_offset": 0,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_CLEAR_BODY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_DIAMOND_STORM", "MOVE_BODY_PRESS", "MOVE_HELPING_HAND", "MOVE_PROTECT"],
            "role": "Mineral conductor: Diamond Storm can harden it, Body Press converts that public state, and Helping Hand amplifies the correct partner without permanent support.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_MAGEARNA",
            "level_offset": 0,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_SOUL_HEART",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_TRICK_ROOM", "MOVE_FLEUR_CANNON", "MOVE_FLASH_CANNON", "MOVE_PROTECT"],
            "role": "Sole speed reversal and Soul-Heart engine; Fleur Cannon's drop and Trick Room's board test remain public and finite.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_MELMETAL",
            "level_offset": 0,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_IRON_FIST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_DOUBLE_IRON_BASH", "MOVE_HIGH_HORSEPOWER", "MOVE_ICE_PUNCH", "MOVE_THUNDER_PUNCH"],
            "role": "Slow physical ingot that rewards a correct Trick Room but remains four-attack pressure without it.",
            "lead_group": "slow-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_KARTANA",
            "level_offset": 0,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_BEAST_BOOST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_LEAF_BLADE", "MOVE_AERIAL_ACE", "MOVE_SACRED_SWORD", "MOVE_KNOCK_OFF"],
            "role": "Fast cutting tool whose Choice lock and Beast Boost can close quickly but can also be trapped in the wrong attack.",
            "lead_group": "fast-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_GHOLDENGO",
            "level_offset": 0,
            "item": "ITEM_WHITE_HERB",
            "ability": "ABILITY_GOOD_AS_GOLD",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_MAKE_IT_RAIN", "MOVE_SHADOW_BALL", "MOVE_THUNDERBOLT", "MOVE_PROTECT"],
            "role": "Special gold pressure with one finite White Herb reset; Make It Rain's spread damage and Special Attack cost prevent mindless repetition.",
            "lead_group": "special-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_METAGROSS",
            "level_offset": 0,
            "item": "ITEM_METAGROSSITE",
            "ability": "ABILITY_CLEAR_BODY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_METEOR_MASH", "MOVE_ZEN_HEADBUTT", "MOVE_ICE_PUNCH", "MOVE_PROTECT"],
            "role": "Steven's sole Mega and iconic final alloy: direct mixed coverage, no setup, no Earthquake partner hazard, and no recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "STEVEN_METEOR_FALLS_SUPERBOSS",
        "planning_tier": "cap_100_superboss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Optional postgame Steven superboss in his Meteor Falls chamber",
            "location": "MeteorFalls_StevensCave",
            "strict_cap": 100,
            "player_tools": [
                "Game clear, full regional travel, the complete ordinary item and move toolkit, Leveler access, and all campaign Mega Stones",
                "Every legal catch, legendary side quest, Frontier reward, and postgame acquisition earned before choosing this challenge",
                "Unlimited team reconstruction through PC, teacher, abilities, natures, items, and level 100",
                "No in-battle items under the campaign's boss rules",
                "Live Hard level 100, Medium level 98, or Easy level 96 trainer settings",
            ],
            "mega_access": "Steven uses exactly one Mega Metagross and no Primal or other battle gimmick.",
            "evolution_phase": "Postgame ceiling: any fully evolved, legendary, mythical, Ultra Beast, Gen 9, or Mega species is appropriate.",
            "preparation_access": "Full preparation is available immediately before the optional challenge; there is no preceding attrition lock.",
            "gauntlet_position": "The ultimate mineral and Steel formation. It must be target 10 without recycling League, Gym, Courtney, or faction species.",
            "mechanics_baseline_id": "postgame_superboss",
            "live_difficulty": "Hard clamps all six authored levels to 100; Medium and Easy apply the global -2 and -4 trainer-level reductions only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_STEVEN"],
            "canonical_format": "double",
            "party_size": 6,
            "required": False,
            "variants": [
                {"variant_id": "meteor_falls_superboss", "trainer_ids": ["TRAINER_STEVEN"], "format": "double", "scope": "designed-here", "reachability": "optional postgame"},
                {"variant_id": "mossdeep_ally", "trainer_ids": ["TRAINER_STEVEN_MOSSDEEP"], "format": "ally-multi", "scope": "separate-backward-anchor", "reachability": "required campaign ally"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Steven",
                "src/data/trainers.h:TRAINER_STEVEN",
                "data/maps/MeteorFalls_StevensCave/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "This is an optional postgame superboss with no single mandatory previous-ten order; the final postgame atlas must select and document its intended access context.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["CHAMPION_WALLACE", "CYNTHIA_MOSSDEEP_SUPERBOSS", "LEAF_ALTERING_CAVE_SUPERBOSS", "STEVEN_MOSSDEEP_ALLY"],
            "required_preimplementation_review": "Refresh the postgame access sequence and last ten intended superbosses. Preserve one conditional Trick Room, six mineral/Steel rare threats, and Mega Metagross unless those exact interactions cluster in the chosen order.",
        },
        "identity": {
            "memory_hook": "Steven shows six forms of treasure: Diancie's living gem, Magearna's crafted soul, Melmetal's ingot, Kartana's blade, Gholdengo's coin, and Mega Metagross's perfect alloy.",
            "story_fit": "The former Champion's cave collection becomes a battle taxonomy of rare material rather than a generic Steel wall.",
            "primary_player_question": "Can the player decide whether to deny or reverse Magearna's Trick Room while preventing Diancie, Soul-Heart, Beast Boost, Make It Rain, and Mega Metagross from converting each knockout into the next material advantage?",
            "primary_mode": "Diancie and Magearna expose defense conversion, Helping Hand, one conditional Trick Room, Fleur Cannon cost, and Soul-Heart from turn one.",
            "secondary_mode": "Melmetal and Kartana create opposite speed physical pressure, Gholdengo creates finite special spread pressure, and Mega Metagross closes directly.",
            "preview_pressure": "Every slot is rare and mineral-coded, but the single Trick Room setter and public Choice/White Herb states keep the board interpretable.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard is six level-100 rare threats with conditional Trick Room, mixed speed, defense conversion, Helping Hand, Soul-Heart, two Beast Boost-style snowballs, Choice pressure, Make It Rain, and Mega Metagross. Shared Fire/Ground/Fighting pressure, one setter, public locks, and finite stat resets preserve counterplay.",
            "pressure_sources": [
                "Diancie Diamond Storm defense and Body Press conversion",
                "Mental Herb Magearna Trick Room, Soul-Heart, and Fleur Cannon",
                "Assault Vest Melmetal slow Iron Fist coverage",
                "Choice Scarf Kartana fast Beast Boost pressure",
                "White Herb Gholdengo Make It Rain spread pressure",
                "Mega Metagross direct Steel/Psychic/Ice finale",
            ],
            "resource_tax": "The fight taxes Fire/Ground/Fighting positioning, Trick Room control, Choice exploitation, spread defense, stat clearing, mixed bulk, and enough priority or speed control to stop multiple public snowball abilities.",
            "tuning_order": [
                "Preserve six-material identity, one conditional reversal, and iconic Mega Metagross",
                "Validate Trick Room, Soul-Heart, Beast Boost, White Herb, and Choice logic before changing sets",
                "Tune AI predicates and ordering before species or moves because Hard levels cannot rise above 100",
                "If testing is excessive, use team/set adjustments before weakening the identity",
                "Use Medium/Easy global levels only as player-selected relief, not the Hard balance target",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_DIANCIE", "SPECIES_MAGEARNA"],
            "mandatory_order_reason": "The lead exposes the speed question and material-conversion thesis. Fast, slow, special, and Mega reserves are board-state choices.",
            "reserve_sequence": [
                "Use Melmetal when Trick Room or raw physical bulk makes the slow ingot correct.",
                "Use Kartana when fast Choice pressure and coverage are superior; respect its selected move.",
                "Use Gholdengo when special spread or status immunity creates the best line and track its stat drop and White Herb.",
                "Preserve Mega Metagross as final alloy when practical, but deploy it earlier if its coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Set Trick Room only when the opposing effective board is faster and Steven's active or reserve slow materials gain meaningful order; never reverse a winning fast Kartana or Mega Metagross state.",
                "Use Helping Hand only for meaningful same-turn damage and let Diancie attack or Protect otherwise.",
                "Track Fleur Cannon and Make It Rain stat drops, Soul-Heart and Beast Boost triggers, and White Herb consumption exactly.",
                "Respect Kartana's public Choice lock and select Melmetal, Kartana, or Gholdengo from visible speed and damage needs.",
                "Mega Evolve Metagross normally and use direct coverage without hidden matchup knowledge.",
            ],
            "forbidden_behaviors": [
                "Do not set Trick Room automatically, stack support over a knockout, or fabricate stat resets.",
                "Do not violate Choice lock or trigger Soul-Heart/Beast Boost without a real faint.",
                "Do not add sleep, redirection, passive recovery loops, hidden information, a second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A exposes Diancie-Magearna conversion and conditional reversal. State B selects slow Melmetal or fast Kartana. State C deploys Gholdengo as finite special spread pressure. State D exposes Mega Metagross as final alloy. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, Encore, opposing Trick Room, Imprison where legal, priority, double-targeting, or speed-neutral teams can deny or reverse Magearna's single speed mode.",
                "Use Fire/Ground/Fighting spread and single-target pressure, Wide Guard, Protect, and resist pivots against the shared mineral typing.",
                "Exploit Kartana's Choice lock, Melmetal's speed, Gholdengo's Make It Rain drops, and Magearna's Fleur Cannon cost.",
                "Use Haze, Clear Smog, Unaware, phazing, status, or immediate focus to prevent Soul-Heart, Beast Boost, and Diamond Storm from compounding.",
                "Preserve Fire/Ground/Ghost/Dark, physical Intimidate or burn, special pressure, priority, or speed control for Mega Metagross.",
            ],
            "intentional_weakness": "Five members share Steel and/or Rock pressure; Magearna is the only speed setter; Kartana is Choice-locked; Gholdengo and Magearna lower themselves; Melmetal is slow; Mega Metagross has no recovery. There is no redirection, healing loop, or second transformation.",
            "first_loss_lesson": "Steven's treasure compounds only if you let each material hand value to the next. Decide the speed mode, exploit the public costs and locks, clear snowball stats, and preserve the correct Fire/Ground/Fighting line for Metagross.",
            "revealed_information": [
                "Trick Room, Diamond Storm boosts, Body Press scaling, Soul-Heart, Beast Boost, Choice lock, stat drops, White Herb, and Mega evolution are public state.",
                "Every snowball ability requires ordinary battle events.",
                "There is one Trick Room setter and one Mega.",
                "No hidden postgame rule changes are proposed.",
            ],
            "unacceptable_failure_modes": [
                "Trick Room reverses Steven's winning fast board",
                "Stat drops, White Herb, Soul-Heart, or Beast Boost resolve incorrectly",
                "Kartana violates Choice lock",
                "The team gains passive stall or a second gimmick",
                "Steven repeats protected Gym, League, or faction species",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Diancie random doubles", "Magearna Trick Room doubles", "Kartana doubles", "Wolfe Gholdengo", "World Champion Metagross"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Swagger, sand, Illusion, Fake Tears, Tera, redirection, passive hazards/recovery, and complete historic teams are not imported.",
                "Courtney's Aerodactyl, Flannery's Heatran, and protected League species are removed from current Steven.",
                "No second Mega, Primal, Z-Move, Dynamax, or Gigantamax appears.",
            ],
            "imported_elements": [
                "Generated Diancie and Magearna doubles roles",
                "Generated Kartana fast Beast Boost pressure",
                "Wolfe-validated Gholdengo special offense",
                "World Champion Metagross legitimacy as iconic ace",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Steven's six-material rare collection",
                "The postgame's definitive mineral conditional Trick Room",
                "Diancie and Magearna conversion lead",
                "Gholdengo Make It Rain and Kartana/Melmetal speed contrast",
                "Mega Metagross as Steven's uncontested signature",
            ],
            "preserves": [
                "Cynthia's iconic Sinnoh balance and Leaf's Kanto all-stars",
                "Tate and Liza's cosmic three-formation reversal",
                "Courtney's Glimmora-Steelix safe zone",
                "Other Steel and mineral teams only when they do not repeat this full material taxonomy",
            ],
            "releases": [
                "Skarmory, Aerodactyl, Cradily, and Heatran leave Steven's superboss roster",
                "Other rare Steel and Rock species remain available outside this exact conversion sequence",
            ],
            "collision_notes": [
                "No species overlaps the protected League, Gym, or faction anchor boards.",
                "Metagross was deliberately removed from Courtney so Mega Metagross belongs only to Steven.",
                "Conditional Trick Room overlaps other battles only as a common tool; the rare-material snowball question is unique.",
            ],
        },
        "presentation": {
            "intro_concept": "Steven invites the player to face the six treasures that taught him what strength looks like when pressure changes matter.",
            "defeat_concept": "He says the player did not crack the collection; they understood each material's limits and worked them apart.",
            "post_battle_concept": "The optional postgame reward flow remains native and must be audited separately from this team design.",
            "hint_concept": "The cave hint says the doll can reverse time, the gem hardens, the blade commits, the gold weakens after it rains, and the final alloy never needs setup.",
            "native_width_status": "concept-only; exact challenge, defeat, reward, and hint text require native font-width validation at implementation",
            "guide_summary": "Document level 100, Diancie-Magearna conversion lead, conditional Trick Room, Melmetal/Kartana speed contrast, Gholdengo finite Make It Rain, Mega Metagross finale, exact snowball AI, and live difficulty levels.",
        },
        "author_self_check": {
            "strongest_part": "The team is unmistakably Steven without defaulting to passive Steel walls: every rare material changes battle state in a different, visible, attack-driven way.",
            "weakest_link": "Soul-Heart plus Beast Boost plus level 100 can snowball brutally. Their event correctness and broad Haze/Unaware/Fire/Ground/Fighting counterplay need real-ROM testing before observed difficulty can be claimed.",
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
                "The source guide places optional Steven at cap 100 in a six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Metagrossite maps Metagross to Mega Metagross and no other transformation item appears.",
                "All five selected references exist and include Wolfe, World Champion, and generated full-set evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Steven with the exact six level-100 sets.",
                "Add partner, HP, speed, field, and combo flags and implement material reserve and conditional Trick Room scoring.",
                "Regression-test Diamond Storm, Body Press, Helping Hand, Trick Room, Soul-Heart, Fleur Cannon, Beast Boost, Choice lock, Make It Rain, White Herb, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and audit the existing optional reward.",
                "Run level-100 Fire/Ground/Fighting, anti-Trick-Room, Haze/Unaware, Choice exploitation, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def cynthia_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:sejun-park:worlds-2014": (
            "selected-architecture",
            "Sejun Park's World Champion roster validates redirection beside Garchomp Earthquake and alternate speed control. Cynthia imports that interaction through Togekiss without Shadow Tag or the complete team.",
        ),
        "showdown:gen7randomdoublesbattle:010": (
            "selected-role",
            "The generated Togekiss roster validates Follow Me, Tailwind, and direct Serene Grace pressure. Cynthia removes setup and recovery for a four-action lead.",
        ),
        "showdown:gen7randomdoublesbattle:014": (
            "adapted-set",
            "The generated Roserade roster validates fast special pressure. Cynthia removes sleep and uses Leaf Storm, Sludge Bomb, Shadow Ball, and Protect.",
        ),
        "showdown:gen9championsrandomdoublesbattle:007": (
            "adapted-set",
            "The Champions generator validates Milotic as competitive doubles control. Cynthia uses base Competitive Milotic as her iconic anti-Intimidate answer, distinct from Wallace's Mega ace.",
        ),
        "elite:ray-rizzo:worlds-2012": (
            "selected-history",
            "Ray Rizzo's World Champion roster validates Garchomp at the highest doubles stakes. Cynthia keeps her iconic Mega Garchomp but rejects sand, Swagger, Trick Room, and the historic full roster.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "Cynthia's species identity is worth preserving, but the current team relies on Sleep Powder, Substitute/Recover, multiple passive recovery loops, and six equal level advantages. The redesign keeps the iconic roster while making every turn doubles-active and Hard level 100.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_TOGEKISS",
            "level_offset": 0,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_SERENE_GRACE",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_FOLLOW_ME", "MOVE_TAILWIND", "MOVE_AIR_SLASH", "MOVE_DAZZLING_GLEAM"],
            "role": "Iconic redirection and speed lead that still attacks; Follow Me can protect Garchomp, but no recovery or Helping Hand creates a permanent support loop.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_GARCHOMP",
            "level_offset": 0,
            "item": "ITEM_GARCHOMPITE",
            "ability": "ABILITY_ROUGH_SKIN",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_EARTHQUAKE", "MOVE_DRAGON_CLAW", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Cynthia's sole Mega and immediately visible ace; Earthquake is ally-safe beside Togekiss and must be partner-aware otherwise.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": True,
        },
        {
            "order": 3,
            "species": "SPECIES_ROSERADE",
            "level_offset": 0,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_TECHNICIAN",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_LEAF_STORM", "MOVE_SLUDGE_BOMB", "MOVE_SHADOW_BALL", "MOVE_PROTECT"],
            "role": "Sleep-free fast special scalpel whose Leaf Storm cost and Sash are finite public resources.",
            "lead_group": "coverage-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_LUCARIO",
            "level_offset": 0,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_INNER_FOCUS",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_CLOSE_COMBAT", "MOVE_METEOR_MASH", "MOVE_EXTREME_SPEED", "MOVE_PROTECT"],
            "role": "Immediate Fighting/Steel breaker with priority and no Choice lock, setup, or recovery.",
            "lead_group": "coverage-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_SPIRITOMB",
            "level_offset": 0,
            "item": "ITEM_LEFTOVERS",
            "ability": "ABILITY_PRESSURE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_SNARL", "MOVE_WILL_O_WISP", "MOVE_SUCKER_PUNCH", "MOVE_PROTECT"],
            "role": "Finite disruption pivot: reduces special damage, burns physical answers, threatens priority, and has no recovery move or Substitute.",
            "lead_group": "control-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_MILOTIC",
            "level_offset": 0,
            "item": "ITEM_ADRENALINE_ORB",
            "ability": "ABILITY_COMPETITIVE",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_RECOVER", "MOVE_PROTECT"],
            "role": "Iconic base-form anti-Intimidate closer; one recovery move is retained but no coil, hypnosis, or passive second sustain layer exists.",
            "lead_group": "closer-reserve",
            "mega_candidate": False,
        },
    ]
    return {
        "anchor_id": "CYNTHIA_MOSSDEEP_SUPERBOSS",
        "planning_tier": "cap_100_superboss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Optional postgame Cynthia superboss in Mossdeep",
            "location": "MossdeepCity_House1",
            "strict_cap": 100,
            "player_tools": [
                "Game clear, full travel, complete team-building services, and level 100 access",
                "All campaign catches, side quests, Mega Stones, and ordinary held items earned before selecting the challenge",
                "Unlimited PC reconstruction and no grinding requirement",
                "No in-battle items under the campaign's boss rules",
                "Live Hard level 100, Medium level 98, or Easy level 96 trainer settings",
            ],
            "mega_access": "Cynthia uses exactly one Mega Garchomp and no other battle gimmick.",
            "evolution_phase": "Postgame ceiling: the iconic fully evolved champion roster is appropriate.",
            "preparation_access": "Full preparation is available immediately before the optional house challenge.",
            "gauntlet_position": "The recognizable iconic-team superboss. It must prove familiar species can still be a target-10 doubles puzzle without sleep or passive stall.",
            "mechanics_baseline_id": "postgame_superboss",
            "live_difficulty": "Hard clamps all six authored levels to 100; Medium and Easy apply the global -2 and -4 trainer-level reductions only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_CYNTHIA_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": False,
            "variants": [
                {"variant_id": "mossdeep_superboss", "trainer_ids": ["TRAINER_CYNTHIA_1"], "format": "double", "scope": "designed-here", "reachability": "optional postgame"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Cynthia1",
                "src/data/trainers.h:TRAINER_CYNTHIA_1",
                "data/maps/MossdeepCity_House1/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "This optional postgame challenge has no single mandatory previous-ten order; the final postgame atlas must define its intended placement.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["STEVEN_METEOR_FALLS_SUPERBOSS", "LEAF_ALTERING_CAVE_SUPERBOSS", "CHAMPION_WALLACE", "WALLY_VICTORY_ROAD"],
            "required_preimplementation_review": "Refresh the intended postgame sequence. Preserve the iconic six, sleep-free active sets, redirection-Earthquake opening, and base Competitive Milotic unless these interactions cluster nearby.",
        },
        "identity": {
            "memory_hook": "It is unmistakably Cynthia, but every old icon now acts in doubles: Togekiss clears Garchomp's quake, Roserade and Lucario cut opposite defenses, Spiritomb controls tempo, and Milotic punishes intimidation.",
            "story_fit": "The crossover Champion should feel recognizable first and optimized second; the surprise is that her classic team is fully alive in Emerald Champions's battle language.",
            "primary_player_question": "Can the player break Togekiss's redirection and Tailwind without feeding Mega Garchomp free Earthquakes, then navigate mixed immediate coverage, Spiritomb's finite control, and Competitive Milotic without relying on Intimidate autopilot?",
            "primary_mode": "Togekiss plus Mega Garchomp is the fixed iconic opening: Follow Me, Tailwind, Air Slash, Dazzling Gleam, and ally-safe Earthquake are all visible.",
            "secondary_mode": "Roserade and Lucario split special and physical coverage, Spiritomb controls both damage categories, and Milotic closes against stat-drop plans.",
            "preview_pressure": "The exact iconic species are a promise, not a trick. Difficulty comes from doubles coordination and clean sets rather than surprise replacements.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard is six level-100 iconic threats with redirection, Tailwind, ally-safe Mega Earthquake, mixed immediate coverage, priority, Snarl, burn, and Competitive anti-Intimidate. Sleep, Substitute, Calm Mind, multiple recovery loops, and hidden gimmicks are removed.",
            "pressure_sources": [
                "Togekiss Follow Me, Tailwind, Serene Grace Air Slash, and Dazzling Gleam",
                "Mega Garchomp Earthquake, Dragon, Rock, and Protect",
                "Focus Sash Roserade three-type special burst",
                "Life Orb Inner Focus Lucario physical coverage and priority",
                "Spiritomb Snarl, burn, Sucker Punch, and Protect",
                "Competitive Adrenaline Orb Milotic Water/Ice and one Recover",
            ],
            "resource_tax": "The fight taxes redirection control, Wide Guard and Flying/Levitate positioning, speed control, mixed bulk, priority awareness, stat-drop discipline, Fairy/Ice/Dragon offense, and enough burst to prevent Milotic recovery.",
            "tuning_order": [
                "Preserve iconic roster, coordinated opening, and sleep-free active design",
                "Validate Follow Me, partner-safe Earthquake, Tailwind, Competitive, and priority before changing sets",
                "Tune AI and ordering before species because Hard levels cannot exceed 100",
                "If testing is excessive, reduce one control interaction before breaking roster identity",
                "Use Medium/Easy only as player-selected relief",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_TOGEKISS", "SPECIES_GARCHOMP"],
            "mandatory_order_reason": "The iconic duo and Earthquake-redirection lesson must be public immediately. Remaining icons are matchup-selected reserves.",
            "reserve_sequence": [
                "Use Roserade for fast special Grass/Poison/Ghost coverage and accept Leaf Storm's public drop.",
                "Use Lucario for physical Fighting/Steel priority when redirection or speed allows.",
                "Use Spiritomb to reduce the correct damage category through Snarl or burn, not to stall.",
                "Preserve Milotic as anti-Intimidate closer when practical, but deploy it earlier if Competitive or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Togekiss and Garchomp jointly: Follow Me requires real survival or attack value, Tailwind requires speed value, and Garchomp Earthquake must account for its partner.",
                "Use Air Slash and Dazzling Gleam when support is redundant rather than locking Togekiss into passive turns.",
                "Track Leaf Storm and Close Combat drops and select Roserade or Lucario from visible target defense and coverage.",
                "Use Spiritomb Snarl or Will-O-Wisp against the correct visible category and attack otherwise.",
                "Recognize Competitive and Adrenaline Orb from actual stat-drop events and use Recover only when survival value exceeds damage.",
            ],
            "forbidden_behaviors": [
                "Do not spam Follow Me, Tailwind, Snarl, burn, or Recover without visible value.",
                "Do not Earthquake a vulnerable partner by default or infer hidden Intimidate/stat drops.",
                "Do not add sleep, Substitute loops, second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes Togekiss-Mega Garchomp iconic coordination. State B selects Roserade or Lucario for the needed damage category. State C uses Spiritomb as finite control. State D uses Milotic as anti-stat-drop closer. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, spread moves, Feint, double-targeting, priority, opposing Tailwind, Trick Room, or immediate Togekiss pressure can break redirection and speed support.",
                "Use Wide Guard, Flying/Levitate, Ice/Fairy/Dragon, Intimidate only with Milotic awareness, and burn against Mega Garchomp.",
                "Break Roserade's Sash with spread or priority and exploit Leaf Storm drops; pressure Lucario with Ground/Fire/Fighting/Psychic and its Life Orb cost.",
                "Use Taunt, status immunity, special/physical category changes, Fairy, or concentrated damage so Spiritomb cannot control the correct axis every turn.",
                "Avoid unnecessary stat drops, use Electric/Grass, Toxic/Taunt, special burst, or double-targeting to prevent Milotic's one Recover from stabilizing.",
            ],
            "intentional_weakness": "Togekiss has no Protect or recovery; Earthquake needs safe positioning; Roserade is Sash-dependent; Lucario is frail; Spiritomb controls only one line per turn; Milotic has one recovery move and no passive second engine. No sleep or alternate speed mode exists.",
            "first_loss_lesson": "You already know Cynthia's species; the puzzle is their coordination. Break the opening lane, switch damage categories around Spiritomb, and do not hand Competitive Milotic the exact stat drop it wants.",
            "revealed_information": [
                "Follow Me, Tailwind, partner immunity, stat drops, Focus Sash, Life Orb, Snarl, burn, Competitive, Adrenaline Orb, recovery, and Mega evolution are public state.",
                "Milotic is an explicit iconic protected reprise distinct from Wallace's Mega form.",
                "There is one speed setter and one Mega.",
                "No sleep move appears on the roster.",
            ],
            "unacceptable_failure_modes": [
                "Togekiss support loops without board value",
                "Garchomp damages vulnerable partners unnecessarily",
                "Spiritomb or Milotic turns the fight into passive stall",
                "Competitive triggers without a real stat drop",
                "Roster identity is diluted to avoid one justified Milotic reprise",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["World Champion Garchomp redirection", "Togekiss random doubles", "Roserade doubles", "Milotic Champions doubles", "World Champion Garchomp"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Sleep Powder, Substitute, Calm Mind, multiple recovery loops, Swagger, sand, and complete historic teams are removed.",
                "No second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax appears.",
                "Wallace's Mega Milotic dual-speed rain remains distinct from Cynthia's base anti-Intimidate closer.",
            ],
            "imported_elements": [
                "World Champion proof for redirection beside Garchomp",
                "Generated Togekiss active doubles support",
                "Generated sleep-free Roserade pressure",
                "Champions Milotic anti-stat-drop legitimacy",
                "World Champion Garchomp legitimacy as iconic Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Cynthia's recognizable six-species iconic roster",
                "The postgame's definitive Togekiss-Mega Garchomp opening",
                "Sleep-free Roserade and immediate Lucario",
                "Finite Spiritomb control",
                "Base Competitive Milotic as iconic closer",
            ],
            "preserves": [
                "Wallace's Mega Milotic dual-speed rain and Champion identity",
                "Steven's rare-material taxonomy and Leaf's Kanto all-stars",
                "Other Garchomp and Togekiss appearances only when they do not repeat this iconic opening",
                "Sleep and stall archetypes for encounters designed explicitly around them",
            ],
            "releases": [
                "No iconic species is released; the redesign changes sets and AI rather than erasing Cynthia",
                "Other Sinnoh species remain available outside this exact champion composition",
            ],
            "collision_notes": [
                "Milotic is the single intentional protected reprise: base Competitive Cynthia versus Mega Wallace.",
                "No other species overlaps the protected League, Gym, or faction anchors.",
                "Mega Garchomp is unique to Cynthia in the current marquee board.",
            ],
        },
        "presentation": {
            "intro_concept": "Cynthia says the player already knows every member of her team; knowing their names is not the same as understanding how they fight together.",
            "defeat_concept": "She recognizes that the player respected the familiar team enough to solve it anew rather than relying on old expectations.",
            "post_battle_concept": "The optional postgame reward remains native and requires a separate incentive audit.",
            "hint_concept": "The house hint says the fairy clears the dragon's quake, the flower no longer sleeps, the spirit controls one damage axis, and the serpent wants you to lower a stat.",
            "native_width_status": "concept-only; exact challenge, defeat, reward, and hint text require native font-width validation at implementation",
            "guide_summary": "Document level 100, iconic Togekiss-Mega Garchomp opening, sleep-free Roserade, immediate Lucario, finite Spiritomb, base Competitive Milotic, partner-aware AI, and live difficulty levels.",
        },
        "author_self_check": {
            "strongest_part": "The recognizable roster is preserved while every passive or sleep-heavy set becomes a real doubles action, so nostalgia and difficulty reinforce each other.",
            "weakest_link": "Milotic repeats a protected Champion species. The iconic roster and base Competitive role justify it, but the guide must explicitly contrast Cynthia's closer with Wallace's Mega rain ace.",
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
                "The source guide places Cynthia at cap 100 in an optional six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Garchompite maps Garchomp to Mega Garchomp and no other transformation item appears.",
                "All five selected references exist and include World Champion, generated, and Champions evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Cynthia1 with the exact six level-100 sets.",
                "Add partner, HP, speed, field, and combo flags and implement opening and reserve scoring.",
                "Regression-test Follow Me, Tailwind, Earthquake partner safety, Serene Grace, stat drops, Sash, priority, Snarl, burn, Competitive, Adrenaline Orb, Recover, Mega timing, and replacements.",
                "Write and font-measure exact dialogue and audit the optional reward.",
                "Run level-100 redirection denial, Wide Guard/Flying, Ice/Fairy/Dragon, anti-stat-drop, mixed-category, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def leaf_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen4randomdoublesbattle:003": (
            "selected-role",
            "The generated roster validates Mew and Snorlax as legitimate doubles pieces. Leaf turns Mew into speed and physical support and Snorlax into one finite Belly Drum threat.",
        ),
        "showdown:gen9randomdoublesbattle:014": (
            "selected-set",
            "The generated Mewtwo roster validates immediate Psystrike coverage at modern doubles stakes. Leaf uses Life Orb and four direct actions without Nasty Plot or recovery.",
        ),
        "showdown:gen9randomdoublesbattle:012": (
            "adapted-role",
            "The generated Moltres roster validates active doubles offense and burn pressure. Leaf removes Tailwind duplication and uses four direct/support actions.",
        ),
        "showdown:gen9championsrandomdoublesbattle:010": (
            "adapted-set",
            "The Champions generator validates Blastoise as an active doubles attacker. Leaf reserves custom Mega Blastoise as the final launcher with HP-sensitive Water Spout.",
        ),
        "vgc:korean-nationals-2018": (
            "selected-history",
            "The Korean National Champion roster validates Snorlax at major doubles stakes. Leaf imports the threat of a bulky Kanto setup piece but not the full terrain and Mega structure.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team mixes Mega Kangaskhan with non-Mega Shell Smash Blastoise and passive Tangrowth/Ninetales. Leaf should be a Kanto all-star superboss with one support legend, one setup icon, two rare attackers, one physical enforcer, and Mega Blastoise.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_MEW",
            "level_offset": 0,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_SYNCHRONIZE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_HELPING_HAND", "MOVE_COACHING", "MOVE_PSYCHIC"],
            "role": "Original mythical team engine: chooses speed, special amplification, physical coaching, or direct Psychic rather than using Transform or recovery.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_SNORLAX",
            "level_offset": 0,
            "item": "ITEM_FIGY_BERRY",
            "ability": "ABILITY_GLUTTONY",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_BELLY_DRUM", "MOVE_BODY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT"],
            "role": "Kanto setup icon with one finite Belly Drum and berry; no Recycle, redirection, or recovery loop guarantees the turn.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_MEWTWO",
            "level_offset": 0,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_UNNERVE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PSYSTRIKE", "MOVE_ICE_BEAM", "MOVE_AURA_SPHERE", "MOVE_PROTECT"],
            "role": "Immediate ultimate special attacker with three exact coverage types and no setup, recovery, or alternate form.",
            "lead_group": "legend-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_TAUROS",
            "level_offset": 0,
            "item": "ITEM_CHOICE_BAND",
            "ability": "ABILITY_INTIMIDATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DOUBLE_EDGE", "MOVE_CLOSE_COMBAT", "MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE"],
            "role": "Classic physical enforcer whose public Choice commitment and recoil are both extreme pressure and clear counterplay.",
            "lead_group": "physical-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_MOLTRES",
            "level_offset": 0,
            "item": "ITEM_SAFETY_GOGGLES",
            "ability": "ABILITY_FLAME_BODY",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_AIR_SLASH", "MOVE_WILL_O_WISP", "MOVE_ROOST"],
            "role": "Rare mixed control attacker: spread Fire, Flying pressure, burn, and one recovery move without weather or Tailwind duplication.",
            "lead_group": "legend-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_BLASTOISE",
            "level_offset": 0,
            "item": "ITEM_BLASTOISINITE",
            "ability": "ABILITY_TORRENT",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_WATER_SPOUT", "MOVE_DARK_PULSE", "MOVE_AURA_SPHERE", "MOVE_PROTECT"],
            "role": "Leaf's sole Mega and starter finale: HP-sensitive spread Water plus Mega Launcher coverage, with no Shell Smash or recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "LEAF_ALTERING_CAVE_SUPERBOSS",
        "planning_tier": "cap_100_superboss",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Optional postgame Leaf/Green superboss in Altering Cave",
            "location": "AlteringCave_B1F",
            "strict_cap": 100,
            "player_tools": [
                "Game clear, full travel, complete level-100 team-building services, and all campaign progression items",
                "Every earned legendary side quest, Mega Stone, and ordinary battle item",
                "Unlimited PC reconstruction and no grinding requirement",
                "No in-battle items under boss rules",
                "Live Hard level 100, Medium level 98, or Easy level 96 trainer settings",
            ],
            "mega_access": "Leaf uses exactly one Mega Blastoise and no alternate Mega, Primal, or other battle gimmick.",
            "evolution_phase": "Postgame ceiling: Kanto legends, fully evolved icons, and one Mega starter are appropriate.",
            "preparation_access": "Full preparation is available immediately before the optional cave challenge.",
            "gauntlet_position": "The Kanto all-star superboss. It must feel legendary and nostalgic without repeating protected marquee species or relying on multiple Megas.",
            "mechanics_baseline_id": "postgame_superboss",
            "live_difficulty": "Hard clamps all six authored levels to 100; Medium and Easy apply the global -2 and -4 trainer-level reductions only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_LEAF_ALTERING_CAVE"],
            "canonical_format": "double",
            "party_size": 6,
            "required": False,
            "variants": [
                {"variant_id": "altering_cave_superboss", "trainer_ids": ["TRAINER_LEAF_ALTERING_CAVE"], "format": "double", "scope": "designed-here", "reachability": "optional postgame"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_LeafAlteringCave",
                "src/data/trainers.h:TRAINER_LEAF_ALTERING_CAVE",
                "data/maps/AlteringCave_B1F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "This optional postgame challenge has no single mandatory previous-ten order; the final postgame atlas must define its intended placement.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["STEVEN_METEOR_FALLS_SUPERBOSS", "CYNTHIA_MOSSDEEP_SUPERBOSS", "BATTLE_FRONTIER_BRAINS", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Refresh the intended postgame sequence. Preserve Kanto all-stars, one Mew-Snorlax setup question, two distinct legends, public Choice Tauros, and Mega Blastoise unless these exact interactions cluster nearby.",
        },
        "identity": {
            "memory_hook": "Leaf brings Kanto's whole mythology: Mew teaches, Snorlax drums, Mewtwo overwhelms, Tauros charges, Moltres controls the air, and Mega Blastoise finishes as the chosen starter.",
            "story_fit": "The original heroine's optional cave battle should feel like a complete Kanto adventure compressed into one elite doubles team.",
            "primary_player_question": "Can the player prevent Mew from creating the exact speed or coaching line Snorlax needs, then survive immediate Mewtwo/Tauros/Moltres pressure while reducing Mega Blastoise's HP before Water Spout?",
            "primary_mode": "Mew plus Snorlax exposes a contestable support-and-Belly-Drum opening with no redirection, Fake Out, or Recycle to force it through.",
            "secondary_mode": "Mewtwo and Tauros apply opposite-category immediate pressure, Moltres supplies finite control, and Mega Blastoise closes through HP-sensitive Water and pulse coverage.",
            "preview_pressure": "Two mythicals/legendaries, one classic setup icon, one classic physical enforcer, one legendary bird, and one Mega starter make the Kanto fantasy explicit.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard is six level-100 Kanto all-stars with adaptive Mew support, Belly Drum, Life Orb Mewtwo, Choice Band Tauros, burn/control Moltres, and HP-sensitive Mega Blastoise. The setup lacks redirection and Fake Out, locks and recoil are public, and only two members have recovery/protection loops.",
            "pressure_sources": [
                "Mental Herb Mew Tailwind, Helping Hand, Coaching, and Psychic",
                "Gluttony Figy Berry Snorlax one-time Belly Drum",
                "Life Orb Mewtwo Psystrike and exact coverage",
                "Choice Band Intimidate Tauros recoil and four direct attacks",
                "Safety Goggles Moltres spread Fire, burn, and Roost",
                "Mega Blastoise current-HP Water Spout and Mega Launcher coverage",
            ],
            "resource_tax": "The fight taxes setup denial, speed control, mixed physical/special bulk, Choice exploitation, Intimidate and burn management, Electric/Grass pressure, priority, and immediate HP damage against Blastoise.",
            "tuning_order": [
                "Preserve Kanto all-stars, adaptive Mew, finite Snorlax setup, and Mega starter finale",
                "Validate joint support, Belly Drum, berry, Choice, recoil, Flame Body, and Water Spout HP before changing sets",
                "Tune AI and ordering before species because Hard levels cannot exceed 100",
                "If testing is excessive, weaken one support predicate before diluting the Kanto roster",
                "Use Medium/Easy only as player-selected relief",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_MEW", "SPECIES_SNORLAX"],
            "mandatory_order_reason": "The lead makes the one setup puzzle public. Legendary, physical, aerial-control, and Mega reserves are selected by board state.",
            "reserve_sequence": [
                "Use Mewtwo when immediate special coverage and Psystrike create the best line.",
                "Use Tauros when Intimidate and a public Choice attack create immediate physical pressure.",
                "Use Moltres when spread Fire, Flying, or burn control is matchup-correct and avoid passive Roost loops.",
                "Preserve Mega Blastoise as starter finale when practical, but deploy it earlier while Water Spout HP and coverage are uniquely valuable.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Mew and Snorlax jointly: Tailwind, Helping Hand, and Coaching require visible value; Belly Drum requires survival, berry state, and a real next-turn line.",
                "Stop supporting when Snorlax setup is denied or spent and let Mew attack.",
                "Respect Tauros's Choice lock, Double-Edge recoil, Mewtwo Life Orb, and Moltres Roost/burn from public state.",
                "Evaluate Water Spout from Blastoise's current HP and prefer pulse coverage or Protect when stronger.",
                "Mega Evolve Blastoise normally and do not Shell Smash or add an alternate Mega.",
            ],
            "forbidden_behaviors": [
                "Do not force Belly Drum into a visible knockout or fabricate berry healing.",
                "Do not stack Mew support when direct damage wins, violate Choice lock, or spam Roost.",
                "Do not spam low-HP Water Spout or use hidden information.",
                "Do not add second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A attempts the Mew-Snorlax support/setup line only when legal. State B selects Mewtwo or Tauros for the needed damage category. State C uses Moltres as finite aerial control. State D exposes Mega Blastoise as starter finale. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, Encore, Haze, Unaware, phazing, priority, double-targeting, or immediate Snorlax damage can deny the one setup line.",
                "Exploit Mew's lack of Protect and Snorlax's limited coverage, then switch damage categories between Mewtwo and Tauros.",
                "Use Ghost/resist pivots and Protect to exploit Tauros's Choice lock and recoil; use special bulk, Dark/Ghost/Bug, and priority against Mewtwo.",
                "Use Rock/Electric/Water, Taunt, special burst, status immunity, or concentrated attacks so Moltres cannot control and Roost repeatedly.",
                "Damage Mega Blastoise immediately, use Water immunity, Wide Guard, Electric/Grass, special bulk, or priority so Water Spout loses force.",
            ],
            "intentional_weakness": "Mew has no Protect; Snorlax has one setup and no Recycle; Mewtwo pays Life Orb; Tauros is Choice-locked and takes recoil; Moltres has one recovery move; Blastoise's strongest attack is HP-sensitive. There is no redirection, Fake Out, sleep, or alternate speed mode.",
            "first_loss_lesson": "Kanto's icons are dangerous for different reasons. Break the one setup, exploit public costs and locks, change your defensive axis between legends, and hit Blastoise before its HP becomes your problem.",
            "revealed_information": [
                "Tailwind, Helping Hand, Coaching, Belly Drum, berry consumption, Life Orb, Choice lock, recoil, Intimidate, burn, current HP, and Mega evolution are public state.",
                "There is one Mega and no alternate starter transformation.",
                "Water Spout uses ordinary current-HP scaling.",
                "No hidden Kanto-only rule exists.",
            ],
            "unacceptable_failure_modes": [
                "Mew-Snorlax setup is scripted through obvious denial",
                "Choice, recoil, berry, or Belly Drum state resolves incorrectly",
                "Moltres or Snorlax creates passive stall",
                "Blastoise spams low-HP Water Spout",
                "Leaf repeats protected marquee species or uses multiple Megas",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Mew Snorlax doubles", "Mewtwo random doubles", "Moltres doubles", "Mega Blastoise Champions doubles", "Snorlax champion doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Mega Kangaskhan, Shell Smash Blastoise, passive Tangrowth/Ninetales, Transform, Recycle, redirection, and multiple setup engines are removed.",
                "No second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax appears.",
                "Protected League, Gym, and faction species are not reused.",
            ],
            "imported_elements": [
                "Generated Mew and Snorlax doubles legitimacy",
                "Generated immediate Mewtwo pressure",
                "Generated active Moltres control",
                "Champions Blastoise offense adapted into Leaf's sole Mega",
                "Tournament Snorlax legitimacy as a Kanto setup icon",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Leaf's Kanto all-star composition",
                "The postgame's definitive Mew-Snorlax finite setup",
                "Mewtwo immediate special apex and Tauros physical enforcer",
                "Moltres as legendary aerial control",
                "Mega Blastoise as Leaf's starter finale",
            ],
            "preserves": [
                "Steven's rare materials and Cynthia's iconic Sinnoh balance",
                "Other Kanto legends and starters for encounters that do not repeat the all-star sequence",
                "Mega Kangaskhan for Norman and other already protected signature Megas",
                "Battle Frontier random-team identities",
            ],
            "releases": [
                "Ninetales, Kangaskhan, Gengar, Tangrowth, and the old non-Mega Blastoise set leave Leaf's current roster",
                "Other Kanto species remain broadly available outside this exact all-star team",
            ],
            "collision_notes": [
                "No species overlaps the protected League, Gym, or faction anchor boards.",
                "Mega Blastoise is unique here; Norman retains Mega Kangaskhan.",
                "The Kanto theme is roster identity, while the actual puzzle is finite setup plus alternating damage axes.",
            ],
        },
        "presentation": {
            "intro_concept": "Leaf says every Kanto journey tells a different story; this is hers, and every old partner now knows how Champions battle.",
            "defeat_concept": "She says the player respected the classics without treating any of them like museum pieces.",
            "post_battle_concept": "The optional Altering Cave reward remains native and requires separate incentive verification.",
            "hint_concept": "The cave hint says the first mythical teaches, the giant has one drum, the clone and bull attack opposite defenses, the bird controls, and the starter's strongest water depends on its health.",
            "native_width_status": "concept-only; exact challenge, defeat, reward, and hint text require native font-width validation at implementation",
            "guide_summary": "Document level 100, Mew-Snorlax finite setup, immediate Mewtwo, Choice Band Tauros, active Moltres, Mega Blastoise HP-sensitive finale, joint AI, and live difficulty levels.",
        },
        "author_self_check": {
            "strongest_part": "The team feels like a Kanto legend without stacking six box legends: every icon has a distinct competitive job and the Mega starter is the emotional finish.",
            "weakest_link": "Mew plus Belly Drum can become binary. It must remain a contestable option rather than a scripted opening, and Leaf must be fully dangerous when the setup is denied.",
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
                "The source guide places Leaf at cap 100 in an optional six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Blastoisinite maps Blastoise to Mega Blastoise and no other transformation item appears.",
                "All five selected references exist and include tournament, generated, and Champions evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_LeafAlteringCave with the exact six level-100 sets.",
                "Add partner, HP, speed, field, and combo flags and implement support/setup and reserve scoring.",
                "Regression-test Tailwind, Helping Hand, Coaching, Belly Drum, berry, Life Orb, Choice lock, recoil, Intimidate, Flame Body, burn, Roost, Water Spout HP, Mega Launcher, Mega timing, and replacements.",
                "Write and font-measure exact dialogue and audit the optional reward.",
                "Run level-100 setup denial, mixed-category, Choice exploitation, Electric/Grass/Rock/Dark/Ghost, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def wally_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:wolfe:milwaukee-2025": (
            "selected-role",
            "Wolfe Glick's Milwaukee team validates Whimsicott Tailwind and active support at elite stakes. Wally imports only speed and Encore-style tempo, not Illusion, Tera, or the complete team.",
        ),
        "elite:francesco-pio-pero:naic-2026": (
            "selected-history",
            "The 2026 North American International Champion roster validates Sylveon in a current Mega-era team. Wally uses a self-contained Pixilate Throat Spray set without the event's dual-Mega preview.",
        ),
        "showdown:gen4randomdoublesbattle:008": (
            "adapted-set",
            "The generated Breloom roster validates Technician priority and multi-hit pressure. Wally deliberately removes Spore so the Sash attacker is interactive rather than variance-driven.",
        ),
        "vgc:korean-winter-league-2018": (
            "selected-history",
            "The winning Korean Winter League roster validates Azumarill as serious doubles offense. Wally uses Assault Vest Huge Power and four attacks without Belly Drum duplication.",
        ),
        "showdown:gen9randomdoublesbattle:021": (
            "selected-set",
            "The generated Haxorus roster validates immediate Mold Breaker doubles pressure. Wally makes its Choice Scarf commitment public and gives it four direct attacks.",
        ),
        "showdown:gen5randomdoublesbattle:022": (
            "adapted-set",
            "The generated Gallade roster validates physical Fighting/Psychic pressure and priority-era positioning. Wally reserves custom Mega Gallade as the final Wide Guard ace.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team is strong but repeats Cynthia's Togekiss/Roserade/Garchomp and Tabitha's Magnezone. Wally should prove growth without legends: six ordinary species developed into an elite priority, speed, spread, and Mega formation.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_WHIMSICOTT",
            "level_offset": 1,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_PRANKSTER",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_ENCORE", "MOVE_HELPING_HAND", "MOVE_MOONBLAST"],
            "role": "Wally's contestable confidence: chooses speed, punishment, amplification, or direct Fairy damage without redirection or recovery.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_SYLVEON",
            "level_offset": 1,
            "item": "ITEM_THROAT_SPRAY",
            "ability": "ABILITY_PIXILATE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HYPER_VOICE", "MOVE_QUICK_ATTACK", "MOVE_MYSTICAL_FIRE", "MOVE_PROTECT"],
            "role": "Finite spread centerpiece: one Pixilate sound attack can consume Throat Spray, while priority and Fire coverage prevent one-note play.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_BRELOOM",
            "level_offset": 2,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_TECHNICIAN",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_MACH_PUNCH", "MOVE_BULLET_SEED", "MOVE_ROCK_TOMB", "MOVE_PROTECT"],
            "role": "Sleep-free technician: multi-hit Grass, priority Fighting, and speed-changing Rock pressure with one visible Sash.",
            "lead_group": "priority-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_AZUMARILL",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_HUGE_POWER",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_LIQUIDATION", "MOVE_PLAY_ROUGH", "MOVE_AQUA_JET", "MOVE_KNOCK_OFF"],
            "role": "Bulky Huge Power attacker with Water/Fairy priority and item removal; no Belly Drum or Protect creates a second setup puzzle.",
            "lead_group": "priority-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_HAXORUS",
            "level_offset": 3,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_MOLD_BREAKER",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DRAGON_CLAW", "MOVE_STOMPING_TANTRUM", "MOVE_POISON_JAB", "MOVE_ROCK_SLIDE"],
            "role": "Fast public commitment that breaks ability assumptions and supplies Dragon/Ground/Poison/Rock coverage without setup.",
            "lead_group": "breaker-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_GALLADE",
            "level_offset": 4,
            "item": "ITEM_GALLADITE",
            "ability": "ABILITY_JUSTIFIED",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_PSYCHO_CUT", "MOVE_CLOSE_COMBAT", "MOVE_WIDE_GUARD", "MOVE_PROTECT"],
            "role": "Wally's sole Mega and emotional ace: direct Psychic/Fighting pressure plus finite spread defense, with no setup or recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "WALLY_VICTORY_ROAD",
        "planning_tier": "rival_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Required Victory Road Wally battle immediately before the League approach",
            "location": "VictoryRoad_1F",
            "strict_cap": 80,
            "player_tools": [
                "Eight Badges and the complete pre-League catch, move, ability, item, Mega, and leveling toolkit",
                "Full PC and preparation access before Victory Road's final rival chamber",
                "All campaign Mega Stones and legendary side quests available before the League gate",
                "No in-battle items under the game's boss rules",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Wally uses exactly one Mega Gallade and no legendary, Primal, or other gimmick.",
            "evolution_phase": "Pre-League climax: fully evolved competitive threats and a signature Mega are appropriate.",
            "preparation_access": "The player can prepare before the chamber; verify exact native healing and rematch flow during source implementation.",
            "gauntlet_position": "The emotional rival finale and proof that ordinary species can reach elite strength. It should contrast the surrounding legendary-heavy bosses.",
            "mechanics_baseline_id": "rival_finale",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4 above cap 80; Medium and Easy subtract two and four levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_WALLY_VR_1"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "victory_road_story", "trainer_ids": ["TRAINER_WALLY_VR_1"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "victory_road_optional", "trainer_ids": ["TRAINER_WALLY_VR_2"], "format": "double", "scope": "deferred-rematch", "reachability": "optional"},
                {"variant_id": "match_call_rematches", "trainer_ids": ["TRAINER_WALLY_VR_3", "TRAINER_WALLY_VR_4", "TRAINER_WALLY_VR_5"], "format": "double", "scope": "deferred-rematch", "reachability": "postgame/rematch"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_WallyVR1",
                "src/data/trainers.h:TRAINER_WALLY_VR_1",
                "data/maps/VictoryRoad_1F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological ledger has not reached Victory Road, so an exact previous-ten context would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["ELITE_FOUR_SIDNEY", "CHAMPION_WALLACE", "STEVEN_METEOR_FALLS_SUPERBOSS", "MAUVILLE_WALLY"],
            "required_preimplementation_review": "Refresh the last ten Victory Road encounters. Preserve no-legendaries growth, Whimsicott-Sylveon opening, sleep-free priority core, Choice Haxorus, and Mega Gallade unless those exact interactions cluster nearby.",
        },
        "identity": {
            "memory_hook": "Wally wins with growth, not trophies: Whimsicott gives him courage, Sylveon finds its voice, Breloom and Azumarill fight above their weight, Haxorus breaks limits, and Mega Gallade stands beside him.",
            "story_fit": "The formerly fragile rival reaches Victory Road with no legendary shortcut. His team is proof that training and adaptation can match the game's rarest opponents.",
            "primary_player_question": "Can the player interrupt Whimsicott's flexible support and Sylveon's finite spread boost, then survive layered priority and a Choice Mold Breaker without spending every answer before Mega Gallade's Wide Guard endgame?",
            "primary_mode": "Whimsicott plus Sylveon creates contestable Tailwind/Encore/Helping Hand and one Throat Spray Hyper Voice without redirection or sleep.",
            "secondary_mode": "Breloom and Azumarill form distinct fast and bulky priority, Haxorus is a public Choice breaker, and Mega Gallade closes through direct pressure and Wide Guard.",
            "preview_pressure": "No species is legendary, but every item, ability, priority bracket, speed control, and coverage choice is fully competitive.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 81 through 84 against cap 80 with adaptive Prankster support, Pixilate spread plus finite boost, two distinct priority attackers, Choice Mold Breaker, Wide Guard, and Mega Gallade. No sleep, redirection, recovery loop, or legendary stat inflation is needed.",
            "pressure_sources": [
                "Mental Herb Whimsicott Tailwind, Encore, Helping Hand, and Moonblast",
                "Pixilate Throat Spray Sylveon spread, priority, and Fire coverage",
                "Focus Sash Technician Breloom multi-hit and priority",
                "Assault Vest Huge Power Azumarill bulky priority and Knock Off",
                "Choice Scarf Mold Breaker Haxorus four-type coverage",
                "Mega Gallade Psychic/Fighting and Wide Guard finale",
            ],
            "resource_tax": "The fight taxes speed control, Encore awareness, Wide Guard and spread/single variation, priority resistance, item preservation, physical Intimidate or burn, Fairy/Psychic/Flying answers, and Choice-lock exploitation.",
            "tuning_order": [
                "Preserve no-legendaries thesis, finite sound boost, priority layers, Choice breaker, and Mega Gallade",
                "Validate support predicates, Throat Spray, multi-hit, priority, Choice, Mold Breaker, and Wide Guard before changing sets",
                "Adjust offsets within +1 to +4, beginning with Gallade, Haxorus, and Azumarill",
                "Then adjust Whimsicott or Sylveon survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_WHIMSICOTT", "SPECIES_SYLVEON"],
            "mandatory_order_reason": "The lead makes Wally's adaptive support and found voice public. Priority, breaker, and Mega roles are board-state reserves.",
            "reserve_sequence": [
                "Use Breloom when Technician priority, multi-hit, or Rock Tomb speed value is correct; no Spore fallback exists.",
                "Use Azumarill when bulky Huge Power, Aqua Jet, Fairy coverage, or Knock Off is the better priority line.",
                "Use Haxorus when Mold Breaker and a public Choice attack create the strongest immediate break.",
                "Preserve Mega Gallade as emotional ace when practical, but deploy it earlier when Wide Guard or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Whimsicott and Sylveon jointly: Tailwind, Encore, and Helping Hand require visible value; Moonblast is preferred when support is redundant.",
                "Track Throat Spray consumption and Pixilate targeting exactly; do not farm Quick Attack or Protect turns.",
                "Select Breloom versus Azumarill from actual target bulk, priority need, multi-hit value, and item-removal value.",
                "Respect Haxorus's Choice lock and Mold Breaker only against abilities it actually bypasses.",
                "Use Wide Guard only against disclosed spread pressure and Mega Evolve Gallade normally.",
            ],
            "forbidden_behaviors": [
                "Do not spam support, Encore hidden moves, or Wide Guard without visible value.",
                "Do not add Spore, sleep, redirection, recovery loops, or legendary species.",
                "Do not violate Choice lock, priority, multi-hit, or Mold Breaker rules.",
                "Do not add second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes Whimsicott-Sylveon adaptive voice. State B selects Breloom or Azumarill for the needed priority profile. State C commits Haxorus to a breaker move. State D exposes Mega Gallade as Wide Guard ace. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt after Mental Herb, priority, opposing Tailwind/Trick Room, double-targeting, or immediate Whimsicott pressure can break adaptive support.",
                "Use Wide Guard, sound resistance, Steel/Poison, special bulk, Snarl, or concentrated damage before Sylveon's finite boost compounds.",
                "Break Breloom's Sash with spread or multi-hit and use Flying/Fire/Psychic/Fairy; pressure Azumarill specially or with Electric/Grass/Poison while respecting Aqua Jet.",
                "Exploit Haxorus's public Choice lock through Fairy/Steel, immunity, Protect, Intimidate/burn, and forced target changes.",
                "Vary spread and single-target attacks around Wide Guard and use Fairy/Ghost/Flying, burn, Intimidate, priority, or speed control against Mega Gallade.",
            ],
            "intentional_weakness": "Whimsicott has no Protect; Sylveon's boost is one-time; Breloom is Sash-dependent; Azumarill lacks Protect; Haxorus is Choice-locked; Mega Gallade has no recovery. There is no sleep, redirection, alternate speed mode, or legendary bulk.",
            "first_loss_lesson": "Wally's strength is efficient ordinary Pokémon. Break the first support decision, identify which priority profile is entering, trap Haxorus in the wrong move, and vary targeting so Gallade cannot shield everything.",
            "revealed_information": [
                "Tailwind, Encore resolution, Helping Hand, Pixilate, Throat Spray, Focus Sash, multi-hit, priority, Huge Power, Choice lock, Mold Breaker, Wide Guard, and Mega evolution are public state.",
                "No legendary or sleep move exists in the team.",
                "There is one speed setter and one Mega.",
                "All level offsets and live difficulty reductions use the normal global rules.",
            ],
            "unacceptable_failure_modes": [
                "Whimsicott support is scripted or reads hidden moves",
                "Throat Spray, Choice, Mold Breaker, multi-hit, or priority resolves incorrectly",
                "Wide Guard loops blindly",
                "The roster gains a legendary shortcut or Spore",
                "Wally repeats Cynthia/Tabitha species instead of owning his growth team",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Wolfe Whimsicott", "champion Sylveon", "sleep-free Breloom doubles", "Azumarill tournament", "Haxorus random doubles", "Mega Gallade doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Illusion, Tera, dual-Mega preview, Spore, Belly Drum, redirection, and full tournament shells are not imported.",
                "Current Togekiss, Garchomp, Magnezone, and Roserade collisions are removed.",
                "No second Mega, Primal, Z-Move, Dynamax, or Gigantamax appears.",
            ],
            "imported_elements": [
                "Wolfe-validated Whimsicott active support",
                "Current Mega-era champion Sylveon legitimacy",
                "Generated sleep-free Breloom pressure",
                "Tournament Azumarill and generated Haxorus roles",
                "Generated Gallade adapted into Wally's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Wally's no-legendaries Victory Road thesis",
                "Whimsicott-Sylveon adaptive voice opening",
                "Sleep-free Breloom and bulky Azumarill priority contrast",
                "Choice Scarf Haxorus breaker",
                "Mega Gallade as Wally's signature ace",
            ],
            "preserves": [
                "Cynthia's iconic Togekiss/Garchomp/Roserade and Tabitha's Magnezone",
                "Legendary-heavy boss identities around the League and postgame",
                "Other ordinary-species champion teams if they do not repeat this priority structure",
                "Wally rematches for later progression rather than duplicate story team",
            ],
            "releases": [
                "Togekiss, Garchomp, Magnezone, and Roserade leave Victory Road Wally",
                "Other ordinary species remain broadly available outside this exact growth composition",
            ],
            "collision_notes": [
                "No species overlaps the protected League, Gym, faction, or completed superboss anchors.",
                "Mega Gallade is unique to Wally in the current marquee board.",
                "Difficulty comes from coordination and offsets, not legendary inflation.",
            ],
        },
        "presentation": {
            "intro_concept": "Wally says he once needed help crossing a route; now every partner on this team grew strong by learning exactly when the others need them.",
            "defeat_concept": "He is proud rather than crushed: the battle proved his team belongs at the League even though the player solved it.",
            "post_battle_concept": "Native Victory Road and League progression remain unchanged; rematch dialogue must acknowledge future refinement rather than replay the same lesson.",
            "hint_concept": "The chamber hint says the cotton chooses the pace, the fairy finds its voice once, the mushroom never sleeps foes, the dragon commits, and Gallade blocks spread attacks.",
            "native_width_status": "concept-only; exact intro, defeat, rematch, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 80, Whimsicott-Sylveon opening, sleep-free Breloom/Azumarill priority, Choice Haxorus, Mega Gallade Wide Guard finale, no-legendaries thesis, AI predicates, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "In a game full of rare monsters, Wally's hardest statement is that six accessible species can be a League-level team through perfect roles and trust.",
            "weakest_link": "Whimsicott plus Sylveon is a recognizable competitive module. The sleep-free priority contrast, no-legend narrative, and Haxorus/Gallade finish must keep the whole fight personal rather than borrowed.",
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
                "The source guide places required Victory Road Wally at strict cap 80 in a six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Galladite maps Gallade to Mega Gallade and no other transformation item appears.",
                "All six selected references exist across Wolfe, champions, tournaments, and generated full-set evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_WallyVR1 with the exact six sets and offsets.",
                "Add partner, HP, speed, field, and combo flags and implement adaptive support and reserve scoring.",
                "Regression-test Tailwind, Encore, Helping Hand, Pixilate, Throat Spray, Technician multi-hit, priority, Huge Power, Choice lock, Mold Breaker, Wide Guard, Mega timing, and replacements.",
                "Write and font-measure exact dialogue and separate story/rematch identities.",
                "Run cap-80 speed denial, Wide Guard/sound, priority resistance, Choice exploitation, Fairy/Ghost/Flying, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def lilycove_rival_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen8randomdoublesbattle:011": (
            "selected-role",
            "The generated Butterfree roster validates redirection and speed support at doubles stakes. The rival upgrades that visible Route 119 signature into custom Mega Butterfree without importing sleep.",
        ),
        "showdown:gen7randomdoublesbattle:016": (
            "selected-set",
            "The generated Mienshao roster validates Fake Out, pivoting, and immediate Fighting pressure. Lilycove uses Eject Button and no setup or recovery.",
        ),
        "showdown:gen4randomdoublesbattle:029": (
            "selected-set",
            "The generated Espeon roster validates fast special doubles pressure. The rival uses Magic Bounce and four immediate actions with Life Orb rather than setup or recovery.",
        ),
        "vgc:ocic-2020": (
            "adapted-role",
            "The 2020 Oceania International Champion roster validates Dracovish as a Mega-era doubles breaker. The rival imports public Choice Scarf Fishious Rend pressure, not the full champion team.",
        ),
        "showdown:gen7randomdoublesbattle:013": (
            "adapted-set",
            "The generated Palossand roster validates it as a doubles defensive attacker. The rival uses Water Compaction and one Shore Up without sand, recovery loops, or setup partners.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current branch teams diverge beyond the starter, repeat protected species, and only support Hoenn starters. The final rival should have one five-member adaptive core plus the correct fully evolved counter-starter from all 21 Gen 1-7 choices.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_BUTTERFREE",
            "level_offset": 1,
            "item": "ITEM_BUTTERFRENITE",
            "ability": "ABILITY_COMPOUND_EYES",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_RAGE_POWDER", "MOVE_BUG_BUZZ", "MOVE_HURRICANE"],
            "role": "The rival's sole Mega and matured Route 119 signature: chooses speed, one-turn redirection, or two direct attacks without sleep or recovery.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": True,
        },
        {
            "order": 2,
            "species": "SPECIES_MIENSHAO",
            "level_offset": 1,
            "item": "ITEM_EJECT_BUTTON",
            "ability": "ABILITY_INNER_FOCUS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_FEINT", "MOVE_CLOSE_COMBAT", "MOVE_U_TURN"],
            "role": "Tactical lead that can buy Tailwind, break Protect, attack, or pivot; Eject Button makes its handoff public and finite.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ESPEON",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_MAGIC_BOUNCE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PSYCHIC", "MOVE_DAZZLING_GLEAM", "MOVE_SHADOW_BALL", "MOVE_PROTECT"],
            "role": "Immediate special coverage and Magic Bounce module with no setup; Life Orb cost and ordinary frailty remain visible.",
            "lead_group": "coverage-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_DRACOVISH",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_STRONG_JAW",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FISHIOUS_REND", "MOVE_DRAGON_RUSH", "MOVE_CRUNCH", "MOVE_ROCK_SLIDE"],
            "role": "Public speed commitment whose Fishious Rend reward depends on real move order and whose Choice lock can be exploited.",
            "lead_group": "breaker-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_PALOSSAND",
            "level_offset": 3,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_WATER_COMPACTION",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_EARTH_POWER", "MOVE_SHADOW_BALL", "MOVE_SHORE_UP", "MOVE_PROTECT"],
            "role": "Bulky field anchor that punishes careless Water contact and owns one recovery move, but no sand or second sustain engine.",
            "lead_group": "anchor-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_BLAZIKEN",
            "level_offset": 4,
            "item": "ITEM_EXPERT_BELT",
            "ability": "ABILITY_BLAZE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_NAIVE",
            "moves": ["MOVE_FIRE_PLEDGE", "MOVE_HEAT_WAVE", "MOVE_FLARE_BLITZ", "MOVE_PROTECT"],
            "role": "Fire counter-starter source placeholder. Runtime replaces species with the correct fully evolved Gen 1-7 counter; Grass and Water templates use their exact type-legal four-move tables.",
            "lead_group": "starter-reserve",
            "mega_candidate": False,
        },
    ]
    return {
        "anchor_id": "LILYCOVE_RIVAL",
        "planning_tier": "rival_milestone",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Final May/Brendan campaign battle in Lilycove after six Badges",
            "location": "LilycoveCity",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and the complete catch, move, ability, ordinary item, leveling, and Mega toolkit available before Lilycove",
                "All 21 Gen 1-7 player starter choices, with the rival carrying the same-trio type counter",
                "Full PC and Center preparation before the department-store confrontation",
                "No in-battle items under boss rules",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "The rival uses exactly one fixed Mega Butterfree in every branch. The dynamic counter-starter remains ordinary so all 21 starter choices are legal and visually consistent.",
            "evolution_phase": "Late campaign: every dynamic starter is fully evolved, and the common core is fully evolved with one Mega.",
            "preparation_access": "The player may prepare immediately before the Lilycove trigger; confirm all May/Brendan approach branches during source implementation.",
            "gauntlet_position": "The rival's final campaign lesson: adaptation across a stable toolkit and one player-dependent starter, not a narrow answer to any one of 21 choices.",
            "mechanics_baseline_id": "rival_milestone",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4 above cap 60; Medium and Easy subtract two and four levels only.",
        },
        "runtime": {
            "trainer_ids": [
                "TRAINER_MAY_LILYCOVE_TREECKO", "TRAINER_MAY_LILYCOVE_TORCHIC", "TRAINER_MAY_LILYCOVE_MUDKIP",
                "TRAINER_BRENDAN_LILYCOVE_TREECKO", "TRAINER_BRENDAN_LILYCOVE_TORCHIC", "TRAINER_BRENDAN_LILYCOVE_MUDKIP",
            ],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "fire_counter_template", "trainer_ids": ["TRAINER_MAY_LILYCOVE_TREECKO", "TRAINER_BRENDAN_LILYCOVE_TREECKO"], "format": "double", "scope": "designed-here", "reachability": "player chose Grass slot in any Gen 1-7 trio"},
                {"variant_id": "water_counter_template", "trainer_ids": ["TRAINER_MAY_LILYCOVE_TORCHIC", "TRAINER_BRENDAN_LILYCOVE_TORCHIC"], "format": "double", "scope": "designed-here", "reachability": "player chose Fire slot in any Gen 1-7 trio"},
                {"variant_id": "grass_counter_template", "trainer_ids": ["TRAINER_MAY_LILYCOVE_MUDKIP", "TRAINER_BRENDAN_LILYCOVE_MUDKIP"], "format": "double", "scope": "designed-here", "reachability": "player chose Water slot in any Gen 1-7 trio"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MayLilycove* and sParty_BrendanLilycove*",
                "src/data/trainers.h:TRAINER_MAY_LILYCOVE_* and TRAINER_BRENDAN_LILYCOVE_*",
                "data/maps/LilycoveCity/scripts.inc",
                "src/battle_main.c:dynamic rival starter substitution",
                "src/starter_choose.c:starter evolution mappings",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological ledger has not reached Lilycove, so an exact previous-ten context would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["ROUTE_119_RIVAL", "AQUA_HIDEOUT_MATT", "MOSSDEEP_GYM_TATE_AND_LIZA", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Refresh the last ten Lilycove-area battles. Preserve Mega Butterfree maturation, tactical lead, immediate special and Choice modules, Palossand, and dynamic starter neutrality unless those exact interactions cluster nearby.",
        },
        "identity": {
            "memory_hook": "The rival arrives with a finished notebook: Butterfree controls the board, Mienshao creates the opening, Espeon reflects disruption, Dracovish tests speed, Palossand stabilizes, and the original counter-starter proves their shared journey.",
            "story_fit": "May/Brendan should feel like the player-facing systems expert: the team is adaptable, legible, and genuinely different in one emotionally important slot for every starter choice.",
            "primary_player_question": "Can the player disrupt Mega Butterfree and Mienshao's flexible opening, exploit Dracovish's Choice/move-order commitment, and solve the common core without the answer depending narrowly on which of 21 counter-starters occupies the final slot?",
            "primary_mode": "Mega Butterfree and Mienshao expose Tailwind, Rage Powder, Fake Out, Feint, Eject Button, and immediate attacks without sleep.",
            "secondary_mode": "Espeon covers specially and reflects status, Dracovish commits physically, Palossand anchors, and the dynamic fully evolved starter supplies branch flavor rather than the whole puzzle.",
            "preview_pressure": "The common five are identical across May/Brendan and all starter generations. Only the final species and its type-legal set change.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 61 through 64 against cap 60 with Mega redirection/speed, tactical Fake Out/Feint, Magic Bounce special coverage, Choice move-order pressure, Water Compaction, and a fully evolved counter-starter. Sleep, recovery loops, branch-specific hard counters, and a second Mega are absent.",
            "pressure_sources": [
                "Mega Butterfree Tailwind, Rage Powder, Hurricane, and Bug Buzz",
                "Eject Button Mienshao Fake Out, Feint, Close Combat, and U-turn",
                "Life Orb Magic Bounce Espeon immediate coverage",
                "Choice Scarf Strong Jaw Dracovish move-order pressure",
                "Water Compaction Palossand Ground/Ghost anchor",
                "Dynamic fully evolved Gen 1-7 counter-starter at cap +4",
            ],
            "resource_tax": "The fight taxes redirection denial, speed control, Protect/Feint variation, Choice exploitation, mixed bulk, Water discipline around Palossand, and broad Grass/Fire/Water starter coverage.",
            "tuning_order": [
                "Preserve branch neutrality, fixed Mega, adaptive core, and starter emotional payoff",
                "Validate all 21 species substitutions and three type templates before changing common sets",
                "Adjust offsets within +1 to +4, beginning with starter, Palossand, and Dracovish",
                "Then adjust Butterfree or Mienshao survivability",
                "Change common species only after every branch receives Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_BUTTERFREE", "SPECIES_MIENSHAO"],
            "mandatory_order_reason": "The fixed lead makes the rival's adaptive board control public. Coverage, breaker, anchor, and starter are matchup-selected reserves.",
            "reserve_sequence": [
                "Use Espeon for immediate special coverage or visible Magic Bounce value without setup.",
                "Use Dracovish when its Choice move and real move order produce the strongest physical break.",
                "Use Palossand when Water Compaction, Ground/Ghost coverage, or one recovery turn is matchup-correct.",
                "Use the dynamic starter according to its actual species, moves, ability, and visible type; do not hardcode Hoenn assumptions.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Butterfree and Mienshao jointly: Tailwind, Rage Powder, Fake Out, and Feint require visible value; both attack when support is redundant.",
                "Resolve Eject Button and reserve selection normally and keep May/Brendan branch behavior identical.",
                "Evaluate Fishious Rend from actual move order and respect Dracovish's Choice lock.",
                "Recognize Water Compaction only from real Water damage and prevent Palossand recovery loops.",
                "Load and evaluate the actual dynamic starter species and its type-specific move table rather than the Hoenn placeholder.",
            ],
            "forbidden_behaviors": [
                "Do not use Sleep Powder, hidden starter knowledge beyond public preview, or branch-specific hardcoded counters.",
                "Do not treat Fishious Rend as doubled without moving first, violate Choice lock, or fabricate Water Compaction.",
                "Do not give the dynamic starter a Mega Stone or species-incompatible move.",
                "Do not add second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes Mega Butterfree-Mienshao adaptive control. State B selects Espeon or Dracovish by damage axis. State C uses Palossand as anchor. State D deploys the actual dynamic counter-starter. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, spread moves, Feint, priority, double-targeting, opposing speed control, Rock/Fire/Electric/Flying, or immediate Butterfree pressure can break the lead.",
                "Use Ghost/Inner Focus/Protect sequencing around Fake Out and Feint and account for Mienshao's Eject Button handoff.",
                "Exploit Espeon's Life Orb and low physical bulk without donating reflected status; exploit Dracovish's Choice lock and deny move order with priority, paralysis, Trick Room, or faster control.",
                "Avoid weak Water contact into Palossand, use Grass/Ice/Ghost/Dark, Taunt, Toxic, item disruption, or concentrated special damage.",
                "Prepare broad neutral answers for the public starter branch rather than one narrow species counter.",
            ],
            "intentional_weakness": "Butterfree, Mienshao, and Espeon are frail; Espeon pays Life Orb; Dracovish is Choice-locked; Palossand owns only one recovery move; the starter has an ordinary item and slot-0 ability. There is no sleep, second speed mode, or branch-specific hidden trick.",
            "first_loss_lesson": "The starter changes the flavor, not the solution. Break the common opening, exploit the public commitment modules, and adapt one reserve slot after preview instead of rebuilding for a single scripted counter.",
            "revealed_information": [
                "Tailwind, Rage Powder, Fake Out, Feint, Eject Button, Life Orb, Choice lock, move order, Water Compaction, starter species, and Mega evolution are public state.",
                "The player-selected starter determines the rival's same-trio fully evolved counter across all 21 Gen 1-7 choices.",
                "The dynamic starter is not a Mega and uses one of three audited type templates.",
                "May and Brendan have exact party and AI parity.",
            ],
            "unacceptable_failure_modes": [
                "Only Hoenn starters substitute dynamically",
                "A counter-starter receives illegal moves or a wrong ability/item",
                "The lead support reads hidden actions or uses sleep",
                "Fishious Rend, Choice, Water Compaction, or Eject Button resolves incorrectly",
                "One starter branch is materially easier because the common core diverges",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Butterfree doubles", "Mienshao doubles", "Espeon random doubles", "Dracovish champion doubles", "Palossand doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Sleep, branch-divergent common teams, starter-specific Megas, setup spam, and complete tournament rosters are not imported.",
                "Current Vikavolt, Swellow, Starmie, Tsareena, Mimikyu, Rapidash, and three forced starter Megas are removed.",
                "No second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax appears.",
            ],
            "imported_elements": [
                "Generated Butterfree and Mienshao active doubles support",
                "Generated Espeon immediate offense",
                "Tournament-validated Dracovish breaker",
                "Generated Palossand doubles anchoring",
                "Repository-native 21-starter counter relationship extended to final evolutions",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Mega Butterfree as the rival's late signature maturation",
                "Lilycove adaptive tactical lead",
                "Espeon special and Dracovish physical commitment contrast",
                "Palossand Water Compaction anchor",
                "Fully evolved 21-starter counter payoff",
            ],
            "preserves": [
                "Wally's no-legendaries growth team and postgame superboss identities",
                "Starter species in other battles only when not framed as the player's dynamic counterpart",
                "Route 119's earlier ordinary Butterfree and Sneasel/Rotom progression",
                "Other Mega Butterfree appearances are excluded from marquee fights",
            ],
            "releases": [
                "Vikavolt, Swellow, Starmie, Tsareena, Mimikyu, and Rapidash leave Lilycove rival",
                "The Hoenn-only Mega-starter branch design is retired",
            ],
            "collision_notes": [
                "Butterfree and the dynamic starter placeholder intentionally recur from Route 119 as same-rival progression.",
                "No fixed common species overlaps the protected League, Gym, faction, Wally, or superboss anchors.",
                "Dynamic starter overlap with other campaigns is player-branch identity, not roster filler.",
            ],
        },
        "presentation": {
            "intro_concept": "The rival says they stopped trying to guess the player's exact team; they built five partners that can adapt, then trusted the starter that has answered the player's since day one.",
            "defeat_concept": "They recognize that the player solved the shared core and then adapted to the starter branch instead of being hard-countered by it.",
            "post_battle_concept": "Native Lilycove progression remains unchanged; dialogue must name no specific starter species.",
            "hint_concept": "The department-store hint says the butterfly has matured, the fighter opens shelter, the fossil cares about moving first, the sand castle dislikes Water only after it survives, and the last partner always counters the player's first choice.",
            "native_width_status": "concept-only; exact May/Brendan intro, defeat, and branch-neutral text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, Mega Butterfree-Mienshao lead, Espeon/Dracovish damage contrast, Palossand anchor, all 21 fully evolved counter-starters, three legal move templates, May/Brendan parity, AI, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "The final rival is genuinely personalized for every starter choice without becoming 21 narrow puzzles; the branch changes one meaningful emotional slot inside one excellent common team.",
            "weakest_link": "Universal type templates cannot be each starter's perfect bespoke set. That is intentional restraint: branch fairness and legality matter more than pretending 21 different optimized puzzles occupy one encounter.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass-for-three-source-templates",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The source guide places all six Lilycove May/Brendan records at strict cap 60 in six-Pokemon doubles.",
                "Every fixed move, item, spread, species, and ability slot exists; all three Hoenn source templates are locally legal.",
                "Butterfrenite maps Butterfree to Mega Butterfree and the starter carries no transformation item.",
                "All five selected references exist across tournament and generated full-set evidence.",
                "The current engine dynamically substitutes only Route 103, Rustboro, and Route 110, so final-evolution hooks are correctly listed as blockers.",
            ],
            "source_blockers": [
                "Add GetFinalEvolutionForStarter mappings for all 21 Gen 1-7 base starters and dynamic slot-5 hooks for Route 119 and Lilycove rival records.",
                "Replace all 12 May/Brendan Route 119 and Lilycove parties with exact common cores plus three type templates, maintaining gender parity.",
                "Prove Grass Pledge/Energy Ball/Seed Bomb/Protect, Fire Pledge/Heat Wave/Flare Blitz/Protect, and Water Pledge/Ice Beam/Waterfall/Protect are legal for every corresponding final evolution; adjust only with an explicit exception table if required.",
                "Regression-test all 21 starter choices, all six source record branches, party preview, ability slot, item, AI, Hard/Medium/Easy, and save compatibility.",
                "Write and font-measure branch-neutral dialogue and regenerate guide, evolution audit, and rival closure proofs.",
            ],
        },
        "mechanics_proposal": {
            "status": "required-before-source-closure",
            "hook": "Replace party slot index 5 for IsRoute119RivalTrainer and IsLilycoveRivalTrainer with GetFinalEvolutionForStarter(GetStarterPokemonForGeneration(counter_slot, VAR_STARTER_GEN)).",
            "source_templates": {
                "grass_counter": ["MOVE_GRASS_PLEDGE", "MOVE_ENERGY_BALL", "MOVE_SEED_BOMB", "MOVE_PROTECT"],
                "fire_counter": ["MOVE_FIRE_PLEDGE", "MOVE_HEAT_WAVE", "MOVE_FLARE_BLITZ", "MOVE_PROTECT"],
                "water_counter": ["MOVE_WATER_PLEDGE", "MOVE_ICE_BEAM", "MOVE_WATERFALL", "MOVE_PROTECT"],
            },
            "invariants": ["21 final mappings", "May/Brendan parity", "ordinary Expert Belt", "slot-0 starter ability", "no dynamic Mega", "all moves legal"],
        },
    }


def route119_rival_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen8randomdoublesbattle:011": (
            "selected-role",
            "The generated Butterfree roster validates its ordinary-form doubles support. Route 119 introduces the signature before its Lilycove Mega maturation and removes sleep.",
        ),
        "showdown:gen7randomdoublesbattle:016": (
            "selected-core",
            "The generated roster contains Weavile and Mienshao and validates Fake Out plus fast pivot pressure. Route 119 uses Weavile as the evolved Sneasel signature from the earlier rival team.",
        ),
        "showdown:gen7randomdoublesbattle:006": (
            "selected-set",
            "The generated Rotom-Mow roster validates Leaf Storm and pivot pressure. The rival evolves the earlier base Rotom into a route-appropriate appliance form.",
        ),
        "showdown:gen4randomdoublesbattle:030": (
            "selected-set",
            "The generated Starmie roster validates fast four-coverage doubles offense. Route 119 uses an Expert Belt and no setup or recovery.",
        ),
        "showdown:gen9championsrandomdoublesbattle:008": (
            "selected-set",
            "The Champions generator validates Passimian as physical pivot pressure. The rival uses Assault Vest Defiant and four direct attacks without Receiver scripting.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current Route 119 branch teams diverge, repeat protected species, and only substitute Hoenn starters. The redesign evolves the rival's earlier Sneasel and Rotom signatures, introduces Butterfree, and uses one neutral dynamic starter slot across all 21 choices.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_BUTTERFREE",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_COMPOUND_EYES",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_RAGE_POWDER", "MOVE_BUG_BUZZ", "MOVE_HURRICANE"],
            "role": "Ordinary-form late-rival signature: contestable speed/redirection and direct attacks with no sleep; later becomes the Lilycove Mega.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_WEAVILE",
            "level_offset": 1,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_PRESSURE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_ICICLE_CRASH", "MOVE_KNOCK_OFF", "MOVE_FEINT"],
            "role": "The earlier rival Sneasel fully evolved: tactical opening, Protect punishment, Ice pressure, and item removal with no setup.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ROTOM_MOW",
            "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_LEAF_STORM", "MOVE_THUNDERBOLT", "MOVE_VOLT_SWITCH", "MOVE_WILL_O_WISP"],
            "role": "The earlier base Rotom now route-adapted: Grass/Electric pressure, pivot, and burn with a public Leaf Storm cost.",
            "lead_group": "pivot-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_STARMIE",
            "level_offset": 2,
            "item": "ITEM_EXPERT_BELT",
            "ability": "ABILITY_ANALYTIC",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HYDRO_PUMP", "MOVE_PSYSHOCK", "MOVE_ICE_BEAM", "MOVE_PROTECT"],
            "role": "Fast special coverage check that attacks different defenses and owns no setup, recovery, or weather dependence.",
            "lead_group": "coverage-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_PASSIMIAN",
            "level_offset": 3,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_DEFIANT",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF", "MOVE_ROCK_SLIDE", "MOVE_U_TURN"],
            "role": "Bulky physical pivot and anti-Intimidate lesson with four immediate attacks and no Receiver dependency.",
            "lead_group": "physical-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_BLAZIKEN",
            "level_offset": 4,
            "item": "ITEM_LEFTOVERS",
            "ability": "ABILITY_BLAZE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_NAIVE",
            "moves": ["MOVE_FIRE_PLEDGE", "MOVE_HEAT_WAVE", "MOVE_FLARE_BLITZ", "MOVE_PROTECT"],
            "role": "Fire counter-starter source placeholder. Runtime replaces species with the correct fully evolved Gen 1-7 counter and uses the audited Grass, Fire, or Water template.",
            "lead_group": "starter-reserve",
            "mega_candidate": False,
        },
    ]
    return {
        "anchor_id": "ROUTE_119_RIVAL",
        "planning_tier": "rival_milestone",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "May/Brendan Route 119 battle after the Weather Institute and before Fortree",
            "location": "Route119",
            "strict_cap": 55,
            "player_tools": [
                "Five Badges and all catch, move, ability, ordinary item, leveling, and Mega tools through Route 119",
                "All 21 Gen 1-7 starter choices with same-trio type-counter rival logic",
                "Full preparation after the Weather Institute before the route trigger where native access permits",
                "No in-battle items under boss rules",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "The rival deliberately uses no Mega here; ordinary Butterfree matures into Mega Butterfree at Lilycove. The dynamic starter also remains ordinary.",
            "evolution_phase": "Mid-late campaign: every dynamic starter is fully evolved and the common core shows explicit evolution from earlier rival signatures.",
            "preparation_access": "Confirm exact Route 119 trigger and Weather Institute healing access during implementation.",
            "gauntlet_position": "The penultimate rival milestone and progression bridge: earlier Sneasel/Rotom mature, Butterfree debuts, and the all-generation starter logic reaches final forms.",
            "mechanics_baseline_id": "rival_milestone",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4 above cap 55; Medium and Easy subtract two and four levels only.",
        },
        "runtime": {
            "trainer_ids": [
                "TRAINER_MAY_ROUTE_119_TREECKO", "TRAINER_MAY_ROUTE_119_TORCHIC", "TRAINER_MAY_ROUTE_119_MUDKIP",
                "TRAINER_BRENDAN_ROUTE_119_TREECKO", "TRAINER_BRENDAN_ROUTE_119_TORCHIC", "TRAINER_BRENDAN_ROUTE_119_MUDKIP",
            ],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "fire_counter_template", "trainer_ids": ["TRAINER_MAY_ROUTE_119_TREECKO", "TRAINER_BRENDAN_ROUTE_119_TREECKO"], "format": "double", "scope": "designed-here", "reachability": "player chose Grass slot in any Gen 1-7 trio"},
                {"variant_id": "water_counter_template", "trainer_ids": ["TRAINER_MAY_ROUTE_119_TORCHIC", "TRAINER_BRENDAN_ROUTE_119_TORCHIC"], "format": "double", "scope": "designed-here", "reachability": "player chose Fire slot in any Gen 1-7 trio"},
                {"variant_id": "grass_counter_template", "trainer_ids": ["TRAINER_MAY_ROUTE_119_MUDKIP", "TRAINER_BRENDAN_ROUTE_119_MUDKIP"], "format": "double", "scope": "designed-here", "reachability": "player chose Water slot in any Gen 1-7 trio"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MayRoute119* and sParty_BrendanRoute119*",
                "src/data/trainers.h:TRAINER_MAY_ROUTE_119_* and TRAINER_BRENDAN_ROUTE_119_*",
                "data/maps/Route119/scripts.inc",
                "src/battle_main.c:dynamic rival starter substitution",
                "src/starter_choose.c:starter evolution mappings",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological ledger has not reached Route 119, so an exact previous-ten context would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["WEATHER_INSTITUTE_SHELLY", "FORTREE_GYM_WINONA", "LILYCOVE_RIVAL", "MT_PYRE_MATT"],
            "required_preimplementation_review": "Refresh the last ten Route 119 encounters. Preserve evolved Sneasel/Rotom continuity, ordinary Butterfree setup, mixed fixed core, and dynamic starter neutrality unless those interactions cluster nearby.",
        },
        "identity": {
            "memory_hook": "The rival's notebook is taking shape: Sneasel became Weavile, Rotom chose a mower for the route, Butterfree learned to direct doubles, Starmie and Passimian cover both defenses, and the starter reached its final form.",
            "story_fit": "This encounter should visibly continue the rival's actual partners rather than replace the whole roster with six unrelated competitive imports.",
            "primary_player_question": "Can the player break Butterfree-Weavile's sleep-free tactical opening, then navigate Rotom's pivot, Starmie's coverage, Passimian's Defiant pressure, and one public fully evolved counter-starter without any branch becoming a hard-counter script?",
            "primary_mode": "Ordinary Butterfree plus evolved Weavile exposes Tailwind, Rage Powder, Fake Out, Feint, item removal, and two direct attacks without sleep.",
            "secondary_mode": "Rotom-Mow pivots, Starmie pressures specially, Passimian pressures physically, and the dynamic starter supplies branch flavor.",
            "preview_pressure": "The common five are identical across gender and starter generation. The final public starter slot is the only branch difference.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard uses levels 56 through 59 against cap 55 with contestable Tailwind/redirection, Fake Out/Feint, item removal, Rotom pivot/burn, Analytic coverage, Defiant anti-Intimidate, and a fully evolved counter-starter. No sleep, Mega, recovery loop, or branch-specific hidden trick exists.",
            "pressure_sources": [
                "Focus Sash Butterfree Tailwind, Rage Powder, Hurricane, and Bug Buzz",
                "Life Orb Weavile Fake Out, Feint, Ice, and Knock Off",
                "Rotom-Mow Leaf Storm, Electric pressure, pivot, and burn",
                "Expert Belt Analytic Starmie mixed-defense coverage",
                "Assault Vest Defiant Passimian physical pivot pressure",
                "Dynamic fully evolved Gen 1-7 counter-starter at cap +4",
            ],
            "resource_tax": "The fight taxes support denial, speed control, Protect/Feint variation, item preservation, Intimidate discipline, mixed bulk, pivot tracking, and broad starter coverage.",
            "tuning_order": [
                "Preserve continuity, no-sleep lead, fixed common core, and dynamic final starter",
                "Validate all 21 final substitutions and three type templates before common-team changes",
                "Adjust offsets within +1 to +4, beginning with starter, Passimian, and Starmie",
                "Then adjust Butterfree or Weavile survivability",
                "Test every branch on Hard/Medium/Easy before source closure",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_BUTTERFREE", "SPECIES_WEAVILE"],
            "mandatory_order_reason": "The fixed lead demonstrates partner growth. Pivot, special, physical, and starter roles are matchup-selected reserves.",
            "reserve_sequence": [
                "Use Rotom-Mow when Grass/Electric, burn, Levitate, or Volt Switch improves the visible board.",
                "Use Starmie for the required special coverage axis and account for Hydro Pump accuracy.",
                "Use Passimian when physical pressure, Knock Off, Defiant, or U-turn is correct.",
                "Use the actual dynamic starter species and type template without Hoenn-only assumptions.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Butterfree-Weavile actions jointly: Tailwind, Rage Powder, Fake Out, and Feint require visible value; both attack when support is redundant.",
                "Track Leaf Storm drops and use Volt Switch/U-turn only when the reserve improves the board.",
                "Recognize Defiant from actual stat drops and never infer Intimidate before it occurs.",
                "Evaluate Starmie's real accuracy and target defense without hidden information.",
                "Load and evaluate the actual final counter-starter and its type template, with May/Brendan parity.",
            ],
            "forbidden_behaviors": [
                "Do not use sleep, hidden branch counters, or support loops.",
                "Do not use Feint based on hidden Protect, fabricate Defiant, or ignore Leaf Storm drops.",
                "Do not give the starter illegal moves, the wrong final evolution, or a Mega Stone.",
                "Do not add Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax to this milestone.",
            ],
            "state_machine": "State A establishes Butterfree-Weavile tactical growth. State B selects Rotom-Mow as pivot. State C chooses Starmie or Passimian by damage axis. State D deploys the actual dynamic starter. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, spread moves, Ghost/Inner Focus, priority, double-targeting, opposing speed control, Rock/Fire/Electric/Flying, or immediate Butterfree pressure can break the lead.",
                "Use Steel/Fighting/Fairy/Fire and item-independent plans against Weavile; exploit its Life Orb and low bulk after Fake Out.",
                "Exploit Rotom's Leaf Storm drop, Starmie's ordinary accuracy and physical bulk, and Passimian's special vulnerability.",
                "Avoid gratuitous Intimidate into Defiant, or use Haze, Unaware, burn, Fairy/Psychic/Flying, and concentrated special damage.",
                "Use public preview to prepare a broad answer for the dynamic starter rather than assuming Hoenn species.",
            ],
            "intentional_weakness": "Butterfree is Sash-dependent; Weavile is frail; Rotom pays Leaf Storm drops; Starmie has no item sustain; Passimian lacks Protect; the starter uses ordinary item/ability and a universal legal template. There is no sleep, Mega, second speed mode, or recovery loop.",
            "first_loss_lesson": "This rival has grown with recognizable partners. Break the common tactical opening, exploit each public cost, and adjust one slot after preview for the actual starter instead of expecting a Hoenn-only script.",
            "revealed_information": [
                "Tailwind, Rage Powder, Focus Sash, Fake Out, Feint, item removal, Leaf Storm drops, pivots, Defiant, starter species, and all level offsets are public state.",
                "All 21 Gen 1-7 counter-starters reach final evolution here.",
                "No Mega appears in this milestone; Butterfree's Mega is reserved for Lilycove.",
                "May and Brendan have exact party and AI parity.",
            ],
            "unacceptable_failure_modes": [
                "Only Hoenn starter branches work",
                "The rival uses an incorrect middle or final evolution",
                "A starter receives illegal type-template moves",
                "Support reads hidden actions or uses sleep",
                "One gender or branch has a divergent common team",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Butterfree random doubles", "Weavile Mienshao doubles", "Rotom Mow doubles", "Starmie random doubles", "Passimian Champions doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Sleep, Mega starter branches, branch-divergent common cores, recovery loops, and hidden hard counters are not imported.",
                "Current Swellow, Tsareena, Mimikyu, Vikavolt, Rapidash, and Hoenn-only starter logic are removed.",
                "No Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax appears in this milestone.",
            ],
            "imported_elements": [
                "Generated ordinary Butterfree support",
                "Generated Weavile tactical pressure",
                "Generated Rotom-Mow and Starmie coverage",
                "Champions Passimian physical pivoting",
                "Repository-native all-generation starter logic extended to final evolutions",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Route 119 evolved Sneasel and Rotom continuity",
                "Ordinary Butterfree as future Mega signature",
                "Starmie special and Passimian physical contrast",
                "First all-21 final counter-starter milestone",
                "Sleep-free rival tactical lead",
            ],
            "preserves": [
                "Lilycove Mega Butterfree and entirely refreshed four-member common reserve core",
                "Wally's ordinary-species finale and Shelly's weather research",
                "Other Rotom forms and fast support battles outside this exact rival progression",
                "Starter branches as player identity rather than global allocation",
            ],
            "releases": [
                "Swellow, Tsareena, Mimikyu, Vikavolt, Rapidash, and the old Route 119 branch teams",
                "Hoenn-only late rival substitution is retired",
            ],
            "collision_notes": [
                "Butterfree and the dynamic starter placeholder intentionally recur at Lilycove as same-rival progression.",
                "No other fixed species overlaps the protected marquee board.",
                "Earlier Sneasel and Rotom continuity is character progression, not a cross-trainer repeat.",
            ],
        },
        "presentation": {
            "intro_concept": "The rival points out that Sneasel evolved, Rotom found the right tool for the route, and their starter kept pace with the player's across every region's possible beginning.",
            "defeat_concept": "They recognize that the player read how the old partners had grown and still adapted to the starter branch.",
            "post_battle_concept": "Native Route 119 progression remains unchanged and dialogue names no specific starter species.",
            "hint_concept": "The route hint says the butterfly directs without sleep, the old Sneasel now breaks shelter, Rotom chose a mower, the monkey hates stat drops, and the last partner is the player's starter's final counter.",
            "native_width_status": "concept-only; exact May/Brendan and branch-neutral text require native font-width validation at implementation",
            "guide_summary": "Document cap 55, ordinary Butterfree-Weavile lead, Rotom-Mow pivot, Starmie/Passimian damage contrast, all 21 final counter-starters, three legal templates, May/Brendan parity, AI, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "This feels like a continuing rival team: two early partners visibly matured, one late signature begins, and the player's starter choice remains emotionally real across all seven generations.",
            "weakest_link": "The common core has several fast utility attackers and could feel toolbox-like. Continuity, exact role separation, and the public starter branch must keep it a character team rather than five random good sets.",
        },
        "verification": {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass-for-three-source-templates",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
            "evidence": [
                "The source guide places all six Route 119 May/Brendan records at strict cap 55 in six-Pokemon doubles.",
                "Every fixed move, item, spread, species, and ability slot exists; all three Hoenn source templates are locally legal.",
                "No transformation item appears in the proposed team.",
                "All five selected references exist across generated and Champions full-set evidence.",
                "The current engine lacks final-evolution hooks for Route 119 and Lilycove, correctly preventing source-closure claims.",
            ],
            "source_blockers": [
                "Add the shared GetFinalEvolutionForStarter mapping and dynamic party slot-5 hooks used by both late rival milestones.",
                "Replace all six Route 119 May/Brendan parties with the exact common core and three type templates, maintaining parity.",
                "Prove every Grass/Fire/Water universal move template across all seven final starters or author a minimal explicit exception table.",
                "Regression-test all 21 choices, all six source records, no-Mega behavior, party preview, AI, saves, and Hard/Medium/Easy.",
                "Write and font-measure branch-neutral dialogue and regenerate guide, evolution audit, and closure proofs.",
            ],
        },
        "mechanics_proposal": {
            "status": "required-before-source-closure",
            "hook": "Shared final-evolution counter-starter substitution in trainer party slot index 5 for Route 119 and Lilycove.",
            "source_templates": {
                "grass_counter": ["MOVE_GRASS_PLEDGE", "MOVE_ENERGY_BALL", "MOVE_SEED_BOMB", "MOVE_PROTECT"],
                "fire_counter": ["MOVE_FIRE_PLEDGE", "MOVE_HEAT_WAVE", "MOVE_FLARE_BLITZ", "MOVE_PROTECT"],
                "water_counter": ["MOVE_WATER_PLEDGE", "MOVE_ICE_BEAM", "MOVE_WATERFALL", "MOVE_PROTECT"],
            },
            "invariants": ["21 final mappings", "May/Brendan parity", "ordinary Leftovers", "slot-0 starter ability", "no Mega", "all moves legal"],
        },
    }


def steven_mossdeep_ally_design(meta: dict, records: dict[str, dict], source: dict, maxie_source: dict, courtney_source: dict) -> dict:
    selected = {
        "showdown:gen7randomdoublesbattle:017": (
            "selected-set",
            "The generated Reshiram roster validates immediate Fire/Dragon doubles offense. Mossdeep Maxie uses Turboblaze and four direct moves without weather or setup.",
        ),
        "vgc:regional-collinsville-il-2019": (
            "selected-history",
            "The winning Collinsville roster validates Volcarona at high-level doubles stakes. Maxie's space-center version uses Rage Powder and direct offense without Quiver Dance or recovery.",
        ),
        "showdown:gen4randomdoublesbattle:019": (
            "selected-set",
            "The generated Deoxys-Attack roster validates extreme fast special pressure. Courtney uses Focus Sash and four direct actions without hazards or setup.",
        ),
        "vgc:euic-2019": (
            "selected-history",
            "The 2019 Europe International Champion roster validates Nihilego in a major restricted format. Courtney uses Expert Belt coverage without importing the complete team.",
        ),
        "showdown:gen7randomdoublesbattle:011": (
            "selected-set",
            "The generated Blacephalon roster validates Beast Boost special offense. Courtney uses a public Choice Scarf and Trick rather than setup or recovery.",
        ),
        "showdown:gen4randomdoublesbattle:030": (
            "selected-role",
            "The generated Cradily roster validates Storm Drain and active Grass/Rock coverage. Steven uses Assault Vest and Acid Spray to help the player's chosen attackers.",
        ),
        "showdown:gen4randomdoublesbattle:011": (
            "selected-role",
            "The generated Claydol roster validates dual screens and Ground/Psychic coverage. Steven uses Light Clay but no Trick Room so the ally does not choose the player's speed mode.",
        ),
        "showdown:gen5randomdoublesbattle:015": (
            "adapted-set",
            "The generated Aggron roster validates heavy physical doubles pressure. Steven upgrades Mega Aggron as the durable allied ace without revealing Mega Metagross.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    ally_team = [
        {
            "order": 1, "species": "SPECIES_CRADILY", "level_offset": 1,
            "item": "ITEM_ASSAULT_VEST", "ability": "ABILITY_STORM_DRAIN", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
            "moves": ["MOVE_GIGA_DRAIN", "MOVE_POWER_GEM", "MOVE_EARTH_POWER", "MOVE_ACID_SPRAY"],
            "role": "Active allied Water shield and special-defense enabler; four attacks prevent recovery stalling.",
            "lead_group": "ally-slot-1", "mega_candidate": False,
        },
        {
            "order": 2, "species": "SPECIES_CLAYDOL", "level_offset": 1,
            "item": "ITEM_LIGHT_CLAY", "ability": "ABILITY_LEVITATE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_PSYCHIC", "MOVE_EARTH_POWER"],
            "role": "Allied defensive controller that chooses one relevant screen and otherwise attacks; it never imposes Trick Room on the player.",
            "lead_group": "ally-slot-2", "mega_candidate": False,
        },
        {
            "order": 3, "species": "SPECIES_AGGRON", "level_offset": 2,
            "item": "ITEM_AGGRONITE", "ability": "ABILITY_STURDY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HEAVY_SLAM", "MOVE_BODY_PRESS", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Steven's sole ally Mega: durable direct Steel/Rock/Fighting pressure that does not steal his later Metagross reveal.",
            "lead_group": "ally-slot-3", "mega_candidate": True,
        },
    ]
    maxie_team = [
        {
            "order": 1, "species": "SPECIES_RESHIRAM", "level_offset": 1,
            "item": "ITEM_LIFE_ORB", "ability": "ABILITY_TURBOBLAZE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_DRAGON_PULSE", "MOVE_EARTH_POWER", "MOVE_PROTECT"],
            "role": "Maxie's space-fire lead and rare direct special pressure.", "lead_group": "maxie-slot-1", "mega_candidate": False,
        },
        {
            "order": 2, "species": "SPECIES_VOLCARONA", "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY", "ability": "ABILITY_FLAME_BODY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_RAGE_POWDER", "MOVE_HEAT_WAVE", "MOVE_BUG_BUZZ", "MOVE_PROTECT"],
            "role": "One-turn solar redirection and mixed spread pressure without setup.", "lead_group": "maxie-slot-2", "mega_candidate": False,
        },
        {
            "order": 3, "species": "SPECIES_TURTONATOR", "level_offset": 3,
            "item": "ITEM_LEFTOVERS", "ability": "ABILITY_SHELL_ARMOR", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_WIDE_GUARD", "MOVE_HEAT_WAVE", "MOVE_DRAGON_PULSE", "MOVE_PROTECT"],
            "role": "Maxie's final launch shield with finite Wide Guard and no Shell Smash or recovery move.", "lead_group": "maxie-slot-3", "mega_candidate": False,
        },
    ]
    courtney_team = [
        {
            "order": 1, "species": "SPECIES_DEOXYS_ATTACK", "level_offset": 1,
            "item": "ITEM_FOCUS_SASH", "ability": "ABILITY_PRESSURE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PSYCHO_BOOST", "MOVE_ICE_BEAM", "MOVE_SHADOW_BALL", "MOVE_PROTECT"],
            "role": "Courtney's orbital strike: extreme immediate coverage with public Psycho Boost cost.", "lead_group": "courtney-slot-1", "mega_candidate": False,
        },
        {
            "order": 2, "species": "SPECIES_NIHILEGO", "level_offset": 2,
            "item": "ITEM_EXPERT_BELT", "ability": "ABILITY_BEAST_BOOST", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_POWER_GEM", "MOVE_SLUDGE_BOMB", "MOVE_THUNDERBOLT", "MOVE_PROTECT"],
            "role": "Ultra-space Rock/Poison/Electric pressure and public Beast Boost snowball.", "lead_group": "courtney-slot-2", "mega_candidate": False,
        },
        {
            "order": 3, "species": "SPECIES_BLACEPHALON", "level_offset": 3,
            "item": "ITEM_CHOICE_SCARF", "ability": "ABILITY_BEAST_BOOST", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_SHADOW_BALL", "MOVE_PSYCHIC", "MOVE_TRICK"],
            "role": "Courtney's final ultra-space commitment: fast spread/single pressure with an exploitable Choice lock.", "lead_group": "courtney-slot-3", "mega_candidate": False,
        },
    ]
    current = {
        "ally_party": [mon["species"] for mon in source["mons"]],
        "maxie_party": [mon["species"] for mon in maxie_source["mons"]],
        "courtney_party": [mon["species"] for mon in courtney_source["mons"]],
        "source_party_sizes": {"steven": source["party_size"], "maxie": maxie_source["party_size"], "courtney": courtney_source["party_size"]},
        "reason_for_replacement": "The physical multi battle deploys three Pokemon per trainer, while current source over-authors 4/6/6 and repeats protected species. The redesign specifies the actual nine deployable slots and one ally Mega.",
    }
    return {
        "anchor_id": "STEVEN_MOSSDEEP_ALLY",
        "planning_tier": "required_multi_climax",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Required Mossdeep Space Center 2-vs-2-trainers climax after seven Badges",
            "location": "MossdeepCity_SpaceCenter_2F",
            "strict_cap": 70,
            "player_tools": [
                "Seven Badges, full pre-crisis catch and team-building access, and the choose-three multi-battle party selector",
                "All ordinary competitive items, legal moves, abilities, natures, levels, and campaign Mega Stones",
                "Steven contributes exactly three allied Pokemon; the player chooses exactly three",
                "No in-battle items under boss rules",
                "Live difficulty reduces enemy trainers only; Steven's allied levels remain authored",
            ],
            "mega_access": "Steven uses one Mega Aggron. Neither enemy trainer transforms; Mega Metagross, Mega Camerupt, and Mega Houndoom remain protected elsewhere.",
            "evolution_phase": "Late campaign multi climax: fully evolved, legendary, mythical, Ultra Beast, and one ally Mega are appropriate.",
            "preparation_access": "The script explicitly prompts the player to choose three Pokemon and may be declined for further preparation before starting.",
            "gauntlet_position": "The campaign's bespoke cooperative battle. Difficulty must come from enemy coordination while Steven remains helpful, predictable, and non-destructive to the player's chosen trio.",
            "mechanics_baseline_id": "required_multi_climax",
            "live_difficulty": "Hard enemies are levels 71-73; Medium/Easy lower only enemy parties by two/four. Steven remains levels 71,71,72 on every setting.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_MAXIE_MOSSDEEP", "TRAINER_COURTNEY_MOSSDEEP", "TRAINER_STEVEN_MOSSDEEP"],
            "canonical_format": "multi_2_vs_2",
            "party_size": 3,
            "required": True,
            "variants": [
                {"variant_id": "space_center_multi", "trainer_ids": ["TRAINER_MAXIE_MOSSDEEP", "TRAINER_COURTNEY_MOSSDEEP", "TRAINER_STEVEN_MOSSDEEP"], "format": "player-three-plus-Steven-three versus Maxie-three-plus-Courtney-three", "scope": "designed-here", "reachability": "required main story"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MaxieMossdeep, sParty_CourtneyMossdeep, sParty_StevenMossdeep",
                "src/data/trainers.h:three Mossdeep trainer records",
                "data/maps/MossdeepCity_SpaceCenter_2F/scripts.inc:multi_2_vs_2",
                "src/battle_main.c and multi battle party-size logic",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological ledger has not reached Mossdeep Space Center, so an exact previous-ten context would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MOSSDEEP_GYM_TATE_AND_LIZA", "SEAFLOOR_CAVERN_FINAL_ARCHIE", "STEVEN_METEOR_FALLS_SUPERBOSS", "MAGMA_HIDEOUT_FINAL_MAXIE"],
            "required_preimplementation_review": "Refresh the last ten Mossdeep battles and the exact multi deployment engine. Preserve three-per-trainer truth, space-fire versus ultra-space enemy identities, and a safe three-member Steven ally roster.",
        },
        "identity": {
            "memory_hook": "Maxie launches a sunless starship of Reshiram, Volcarona, and Turtonator; Courtney opens ultra-space with Deoxys, Nihilego, and Blacephalon; Steven answers with Cradily, Claydol, and Mega Aggron beside the player's chosen three.",
            "story_fit": "The Space Center climax should look and play like an invasion of space science, while Steven's fossil/mineral allies make him recognizably himself without revealing his superboss collection.",
            "primary_player_question": "Can the player choose three partners that complement Steven's Water shield, screens, Acid Spray, and Mega Aggron while surviving two coordinated enemy trainers with redirection/Wide Guard, Psycho Boost, two Beast Boost paths, and a final Choice commitment?",
            "primary_mode": "Enemy slot one opens Reshiram plus Deoxys-Attack for immediate special pressure; Steven opens Cradily beside the player's first chosen Pokemon.",
            "secondary_mode": "Volcarona/Turtonator and Nihilego/Blacephalon supply finite support and snowballs, while Claydol and Mega Aggron provide screens and physical stabilization for the player.",
            "preview_pressure": "The battle truthfully shows three Pokemon per trainer. No unreachable fourth/sixth slots or protected later Megas appear.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard fields six enemy Pokemon at levels 71-73 across two trainers with immediate legendary/mythical pressure, Rage Powder, Wide Guard, two Beast Boost paths, Choice speed, and mixed spread/single coverage. Steven's three-member ally is useful but cannot solve target selection for the player.",
            "pressure_sources": [
                "Reshiram Heat Wave plus Deoxys-Attack Psycho Boost opening",
                "Volcarona Rage Powder and Turtonator Wide Guard finite protection",
                "Nihilego and Blacephalon Beast Boost snowballs",
                "Choice Scarf Blacephalon spread pressure or Trick",
                "Steven Cradily Storm Drain and Acid Spray support",
                "Steven screens and Mega Aggron physical stabilization",
            ],
            "resource_tax": "The encounter taxes three-Pokemon selection, special bulk, spread/single variation, redirection and Wide Guard denial, stat-drop and Beast Boost control, Choice exploitation, and coordination with a fixed ally.",
            "tuning_order": [
                "Preserve exact 3+3 versus 3+3 trainer structure and distinct space identities",
                "Validate party deployment, allied difficulty immunity, partner AI, and cross-trainer replacement before set changes",
                "Adjust enemy offsets +1 to +3 before weakening species; never weaken Steven on Medium/Easy",
                "Then tune Rage Powder/Wide Guard and Beast Boost predicates",
                "Change species only after player-trio diversity testing",
            ],
        },
        "team": ally_team,
        "opponent_teams": {"maxie": maxie_team, "courtney": courtney_team},
        "ordering": {
            "intended_lead": ["SPECIES_CRADILY"],
            "mandatory_order_reason": "Steven's three ally slots deploy in authored order; Maxie and Courtney each deploy their own three slots in authored order under multi rules.",
            "reserve_sequence": [
                "Claydol follows Cradily and chooses one relevant screen before attacking.",
                "Mega Aggron is Steven's final durable ally and should not replace a healthy correct partner prematurely.",
                "Maxie advances Reshiram to Volcarona to Turtonator; Courtney advances Deoxys-Attack to Nihilego to Blacephalon, subject only to legal multi replacement behavior.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Steven must target enemies only, choose Reflect versus Light Screen from visible enemy categories, avoid redundant screens, and use Acid Spray when the player's visible special attacker benefits.",
                "Enemy AI must coordinate Rage Powder and Wide Guard only with real partner value and cannot read the player's hidden selected move.",
                "Track Psycho Boost and Choice lock, Beast Boost triggers, and cross-trainer target/replacement state exactly.",
                "Apply live difficulty only to gEnemyParty, never Steven's gMultiPartnerParty; add a regression proving ally levels are invariant.",
                "Mega Evolve Aggron normally and never expose Mega Metagross or an enemy Mega in this encounter.",
            ],
            "forbidden_behaviors": [
                "Do not load fourth-through-sixth trainer slots into a three-per-trainer multi battle.",
                "Do not let Steven damage, status, or speed-drop the player's side or impose Trick Room.",
                "Do not spam screens, Rage Powder, Wide Guard, or Protect without visible value.",
                "Do not add sleep, hidden information, second Mega, enemy Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "Enemy state A opens Reshiram plus Deoxys-Attack; state B introduces Volcarona plus Nihilego; state C closes with Turtonator plus Blacephalon, with legal desynchronization after faints. Steven state A opens Cradily, state B screens through Claydol, state C Mega Evolves Aggron. All actors have direct-action fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Choose a three-member player squad that supplies special bulk, speed control, and at least one way around Rage Powder/Wide Guard while complementing Steven's mainly defensive aid.",
                "Use Rock/Water/Ground/Dragon/Fairy against Maxie's Fire/Dragon trio and Psychic/Ground/Steel/Dark/Ghost against Courtney's ultra-space trio.",
                "Use spread and single-target variation, Taunt, Feint, double-targeting, or direct support removal to break Rage Powder and Wide Guard.",
                "Exploit Deoxys's Sash/frailty, Blacephalon's Choice lock, Psycho Boost drops, and Beast Boost dependence on real knockouts.",
                "Avoid duplicating Steven's screens/Storm Drain at team select; bring offense and speed that Acid Spray and protection can enable.",
            ],
            "intentional_weakness": "Enemy support is split across separate trainers and only three slots each; Deoxys and Blacephalon are frail; Psycho Boost drops and Choice lock are public; Steven contributes no speed mode and modest damage until Aggron. There is no weather, sleep, recovery loop, or enemy transformation.",
            "first_loss_lesson": "This battle begins in party selection. Bring what Steven lacks, remove the enemy support that protects the current attacker, and treat the two enemy trainers as desynchronizable three-member lines rather than one six-slot switch engine.",
            "revealed_information": [
                "Chosen player trio, three-per-trainer party sizes, screens, Storm Drain, Acid Spray, Rage Powder, Wide Guard, Psycho Boost drops, Choice lock, Beast Boost, and Mega Aggron are public state.",
                "Medium/Easy modify enemies only; Steven's levels are invariant.",
                "Each trainer owns its own replacement sequence.",
                "Mega Aggron is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Unreachable fourth/sixth trainer slots enter battle",
                "Difficulty lowers Steven or ally AI harms the player",
                "Enemy trainers share illegal switches or hidden information",
                "Rage Powder/Wide Guard loops blindly",
                "Protected later Steven/Maxie/Courtney Megas appear",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Reshiram doubles", "Volcarona tournament", "Deoxys Attack doubles", "Nihilego champion", "Blacephalon doubles", "Cradily Claydol Aggron doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Current sleep, hazards, recovery loops, duplicate Heatran, protected Solgaleo/Cresselia/Aerodactyl/Metagross, and overlong source parties are removed.",
                "Trick Room, weather, enemy Megas, and later signature Megas are not imported.",
                "No second Mega, Primal, Tera, Z-Move, Dynamax, or Gigantamax appears.",
            ],
            "imported_elements": [
                "Generated Reshiram and Deoxys-Attack immediate pressure",
                "Tournament Volcarona and Nihilego legitimacy",
                "Generated Blacephalon Choice snowball",
                "Generated Cradily, Claydol, and Aggron ally roles",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "The campaign's definitive 3+3-trainer cooperative battle",
                "Maxie's Reshiram-Volcarona-Turtonator space-fire line",
                "Courtney's Deoxys-Nihilego-Blacephalon ultra-space line",
                "Steven's Cradily-Claydol-Mega Aggron allied support",
                "Enemy-only live-difficulty invariant for multi battles",
            ],
            "preserves": [
                "Steven's Diancie/Magearna/Melmetal/Kartana/Gholdengo/Mega Metagross superboss",
                "Final Maxie Primal Groudon/Mega Camerupt and final Courtney Mega Houndoom",
                "Tate and Liza's Solgaleo/Lunala cosmic formations",
                "Other multi battles only if they do not repeat this space invasion structure",
            ],
            "releases": [
                "Victreebel, Crobat, Camerupt, Solgaleo, Heatran, Ninetales, Krookodile, Houndoom, Cresselia, Malamar, Aerodactyl, Metagross, and Deoxys-normal leave current Mossdeep source",
                "Unreachable extra slots are deleted rather than documented as usable reserves",
            ],
            "collision_notes": [
                "All nine deployable species are unique against the protected marquee boards and current tranche.",
                "Mega Aggron is Steven's campaign ally Mega; Mega Metagross remains exclusive to his superboss.",
                "The space visual theme differs from Courtney's meteor impact and Tate/Liza's Psychic cosmic formations through multi-trainer coordination and no speed mode.",
            ],
        },
        "presentation": {
            "intro_concept": "Steven asks the player to choose three partners that can attack through the openings his fossils and armor will create.",
            "defeat_concept": "Maxie and Courtney recognize that the player and Steven broke their two launch lines apart rather than fighting one combined army.",
            "post_battle_concept": "Native Mossdeep progression remains unchanged; dialogue must acknowledge the player's chosen trio and Steven's support without naming unavailable slots.",
            "hint_concept": "The selection prompt warns that Steven brings Water protection and screens but no speed control; Maxie guards spread attacks, Courtney snowballs from knockouts, and each enemy trainer has exactly three.",
            "native_width_status": "concept-only; exact selection, intro, defeat, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 70, exact 3+3 versus 3+3 multi structure, all nine sets, enemy lines, Steven ally behavior, choose-three advice, enemy-only difficulty levels, AI coordination, and replacement rules.",
        },
        "author_self_check": {
            "strongest_part": "The dossier finally treats the encounter as the physical multi battle it is: nine exact deployable Pokemon, two distinct enemy trainers, a useful ally, and party selection as part of the puzzle.",
            "weakest_link": "Fixed three-slot ordering can make the enemy waves feel scripted. Cross-trainer faints naturally desynchronize them, and each slot has independent action fallbacks, but real multi-battle testing is essential.",
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
                "The source script uses multi_2_vs_2 and choose_mons; engine MULTI_PARTY_SIZE is three.",
                "Current source incorrectly declares 6/6 enemy slots and 4 ally slots, while the guide admits Steven deploys only the first three.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Aggronite maps Aggron to Mega Aggron and no other transformation item appears.",
                "ApplyLiveTrainerLevelDifficulty currently iterates gEnemyParty only, which matches the ally-invariance requirement and needs a regression test.",
            ],
            "source_blockers": [
                "Replace and truncate sParty_MaxieMossdeep, sParty_CourtneyMossdeep, and sParty_StevenMossdeep to three exact deployable sets each.",
                "Add partner, HP, speed, field, and combo flags to all three records and implement ally-safe/player-helpful and cross-trainer enemy scoring.",
                "Regression-test choose_mons, all player slot orders, three-per-trainer deployment, independent replacements, screens, Storm Drain, Acid Spray, Rage Powder, Wide Guard, Psycho Boost, Choice lock, Beast Boost, Mega Aggron, and simultaneous faints.",
                "Prove Hard/Medium/Easy lower enemy levels only and never Steven, and prove no fourth-through-sixth source slot is referenced.",
                "Write and font-measure exact selection/dialogue and regenerate the physical atlas and guide as one physical encounter.",
            ],
        },
        "mechanics_proposal": {
            "status": "required-before-source-closure",
            "party_truth": "Exactly three Pokemon per trainer. Remove unreachable extras instead of treating them as reserves.",
            "difficulty_invariant": "Apply -2/-4 only to gEnemyParty; never gMultiPartnerParty.",
            "ally_invariant": "Steven may help the player but may never target, damage, status, speed-drop, or force a speed mode on the player side.",
        },
    }


def protected_species() -> set[str]:
    protected = set()
    for path in (LEAGUE_PATH, GYMS_PATH, FACTIONS_PATH):
        payload = json.loads(path.read_text())
        for dossier in payload["designs"].values():
            protected.update(mon["species"] for mon in dossier["team"])
    return protected


def build() -> dict:
    meta = json.loads(META_PATH.read_text())
    records = {record["reference_id"]: record for record in competitive.load_records()}
    source_teams = {team["trainer_id"]: team for team in quality.audit()["teams"]}
    designs = {
        "STEVEN_METEOR_FALLS_SUPERBOSS": steven_design(meta, records, source_teams["TRAINER_STEVEN"]),
        "CYNTHIA_MOSSDEEP_SUPERBOSS": cynthia_design(meta, records, source_teams["TRAINER_CYNTHIA_1"]),
        "LEAF_ALTERING_CAVE_SUPERBOSS": leaf_design(meta, records, source_teams["TRAINER_LEAF_ALTERING_CAVE"]),
        "WALLY_VICTORY_ROAD": wally_design(meta, records, source_teams["TRAINER_WALLY_VR_1"]),
        "LILYCOVE_RIVAL": lilycove_rival_design(meta, records, source_teams["TRAINER_MAY_LILYCOVE_TREECKO"]),
        "ROUTE_119_RIVAL": route119_rival_design(meta, records, source_teams["TRAINER_MAY_ROUTE_119_TREECKO"]),
        "STEVEN_MOSSDEEP_ALLY": steven_mossdeep_ally_design(
            meta,
            records,
            source_teams["TRAINER_STEVEN_MOSSDEEP"],
            source_teams["TRAINER_MAXIE_MOSSDEEP"],
            source_teams["TRAINER_COURTNEY_MOSSDEEP"],
        ),
    }
    uses: dict[str, list[str]] = {}
    for anchor_id, dossier in designs.items():
        all_mons = list(dossier["team"])
        for opponent_team in dossier.get("opponent_teams", {}).values():
            all_mons.extend(opponent_team)
        for mon in all_mons:
            uses.setdefault(mon["species"], []).append(anchor_id)
    protected = protected_species()
    protected_rows = [
        {"anchor_id": anchor_id, "species": mon["species"]}
        for anchor_id, dossier in designs.items()
        for mon in list(dossier["team"]) + [mon for team in dossier.get("opponent_teams", {}).values() for mon in team]
        if mon["species"] in protected
    ]
    unwaived = [row for row in protected_rows if (row["anchor_id"], row["species"]) not in ALLOWED_PROTECTED_REUSES]
    internal_collisions = {species: anchors for species, anchors in uses.items() if len(anchors) > 1}
    unwaived_internal = {
        species: anchors
        for species, anchors in internal_collisions.items()
        if any((anchor_id, species) not in ALLOWED_INTERNAL_REUSES for anchor_id in anchors[1:])
    }
    return {
        "version": 1,
        "title": "Emerald Champions rival and superboss anchor designs",
        "phase": "superbosses_then_rivals_backward",
        "expected_order": EXPECTED_ORDER,
        "designed_count": len(designs),
        "designs": designs,
        "anchor_review": {
            "status": "pass",
            "slot_count": sum(len(dossier["team"]) + sum(len(team) for team in dossier.get("opponent_teams", {}).values()) for dossier in designs.values()),
            "distinct_species_count": len(uses),
            "internal_species_collisions": internal_collisions,
            "allowed_internal_reuses": [
                {"anchor_id": anchor_id, "species": species, "reason": reason}
                for (anchor_id, species), reason in ALLOWED_INTERNAL_REUSES.items()
                if anchor_id in designs and species in uses
            ],
            "unwaived_internal_species_collisions": unwaived_internal,
            "protected_collisions": protected_rows,
            "allowed_protected_reuses": [
                {"anchor_id": anchor_id, "species": species, "reason": reason}
                for (anchor_id, species), reason in ALLOWED_PROTECTED_REUSES.items()
                if anchor_id in designs
            ],
            "unwaived_protected_collisions": unwaived,
            "unique_mega_signatures": [
                {"anchor_id": anchor_id, "species": mon["species"], "item": mon["item"]}
                for anchor_id, dossier in designs.items()
                for mon in list(dossier["team"]) + [mon for team in dossier.get("opponent_teams", {}).values() for mon in team]
                if mon["mega_candidate"]
            ],
            "all_primary_questions_distinct": len({d["identity"]["primary_player_question"] for d in designs.values()}) == len(designs),
            "judgment": "Steven tests material-state compounding, Cynthia tests iconic doubles coordination, and Leaf tests finite setup across Kanto damage axes. The three are mechanically and culturally distinct.",
        },
    }


def validate(payload: dict) -> None:
    contract = json.loads(OS_PATH.read_text())["dossier_contract"]
    if list(payload["designs"]) != EXPECTED_ORDER[: payload["designed_count"]]:
        raise AssertionError("Superboss/rival anchors are not advancing in requested backward order")
    dex = presets.LocalDex()
    abilities = doubles.base_ability_slots()
    items = set(re.findall(r"#define\s+(ITEM_[A-Z0-9_]+)", (ROOT / "include/constants/items.h").read_text()))
    spreads = set(re.findall(r"#define\s+(SPREAD_[A-Z0-9_]+)", (ROOT / "include/constants/spreads.h").read_text()))
    refs = {record["reference_id"] for record in competitive.load_records()}
    mega_source = (ROOT / "src/data/pokemon/evolution.h").read_text() + (ROOT / "src/data/pokemon/verdant_gen9_evolutions.h").read_text()
    protected = protected_species()
    seen: dict[str, str] = {}

    for anchor_id, dossier in payload["designs"].items():
        for field in contract["required_top_level"]:
            if field not in dossier:
                raise AssertionError(f"{anchor_id} missing {field}")
        for section, required_key in (
            ("campaign_state", "campaign_state_required"), ("runtime", "runtime_required"),
            ("rolling_context", "rolling_context_required"), ("identity", "identity_required"),
            ("difficulty", "difficulty_required"), ("ordering", "ordering_required"),
            ("ai", "ai_required"), ("counterplay", "counterplay_required"),
            ("competitive_research", "competitive_research_required"),
            ("campaign_reservations", "reservations_required"), ("presentation", "presentation_required"),
            ("verification", "verification_required"), ("author_self_check", "author_self_check_required"),
        ):
            missing = set(contract[required_key]) - set(dossier[section])
            if missing:
                raise AssertionError(f"{anchor_id}.{section} missing {sorted(missing)}")
        if dossier["status"] != {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"}:
            raise AssertionError(f"{anchor_id} status drifted")
        if dossier["difficulty"]["target"] != 10 or dossier["difficulty"]["observed"] is not None:
            raise AssertionError(f"{anchor_id} difficulty status is dishonest")
        if anchor_id == "STEVEN_MOSSDEEP_ALLY":
            if len(dossier["team"]) != 3 or set(dossier.get("opponent_teams", {})) != {"maxie", "courtney"} or any(len(team) != 3 for team in dossier["opponent_teams"].values()):
                raise AssertionError("Mossdeep multi must define Steven three plus Maxie three plus Courtney three")
        elif len(dossier["team"]) != 6:
            raise AssertionError(f"{anchor_id} requires six Pokemon")
        all_mons = list(dossier["team"])
        for opponent_team in dossier.get("opponent_teams", {}).values():
            all_mons.extend(opponent_team)
        expected_mega_count = 0 if anchor_id == "ROUTE_119_RIVAL" else 1
        if sum(mon["mega_candidate"] for mon in all_mons) != expected_mega_count:
            raise AssertionError(f"{anchor_id} requires {expected_mega_count} Mega")
        for mon in all_mons:
            if set(contract["mon_required"]) - set(mon):
                raise AssertionError(f"{anchor_id} {mon.get('species')} lacks required Pokemon fields")
            if mon["species"] in protected and (anchor_id, mon["species"]) not in ALLOWED_PROTECTED_REUSES:
                raise AssertionError(f"{anchor_id} collides on protected species {mon['species']}")
            if mon["species"] in seen and (anchor_id, mon["species"]) not in ALLOWED_INTERNAL_REUSES:
                raise AssertionError(f"{anchor_id} repeats tranche species {mon['species']} from {seen[mon['species']]}")
            seen.setdefault(mon["species"], anchor_id)
            illegal = [move for move in mon["moves"] if move not in dex.legal_moves(mon["species"])]
            if illegal:
                raise AssertionError(f"{anchor_id} {mon['species']} illegal moves {illegal}")
            slots = abilities.get(mon["species"], [])
            if mon["ability_slot"] >= len(slots) or slots[mon["ability_slot"]] != mon["ability"]:
                raise AssertionError(f"{anchor_id} {mon['species']} ability slot mismatch")
            if mon["item"] not in items or mon["spread"] not in spreads:
                raise AssertionError(f"{anchor_id} {mon['species']} missing item or spread token")
            if len(mon["moves"]) != 4 or len(set(mon["moves"])) != 4:
                raise AssertionError(f"{anchor_id} {mon['species']} needs four distinct moves")
            if mon["mega_candidate"] and not re.search(rf"\[{mon['species']}\].*?EVO_MEGA_EVOLUTION,\s*{mon['item']}", mega_source, re.S):
                raise AssertionError(f"{anchor_id} Mega pairing is not source-legal")
        selected_refs = dossier["competitive_research"]["selected_reference_ids"]
        if not selected_refs or not set(selected_refs) <= refs:
            raise AssertionError(f"{anchor_id} references are missing")
        if len(dossier["counterplay"]["classes"]) < 3:
            raise AssertionError(f"{anchor_id} lacks broad counterplay")

    review = payload["anchor_review"]
    expected_distinct = review["slot_count"] - len(review["allowed_internal_reuses"])
    if review["status"] != "pass" or review["distinct_species_count"] != expected_distinct:
        raise AssertionError("Superboss tranche review is incomplete")
    if review["unwaived_internal_species_collisions"] or review["unwaived_protected_collisions"] or not review["all_primary_questions_distinct"]:
        raise AssertionError("Superboss tranche collision review failed")
    expected_megas = payload["designed_count"] - (1 if "ROUTE_119_RIVAL" in payload["designs"] else 0)
    if len(review["unique_mega_signatures"]) != expected_megas or len({(row["species"], row["item"]) for row in review["unique_mega_signatures"]}) != expected_megas:
        raise AssertionError("Superboss Mega review failed")


def markdown(payload: dict) -> str:
    lines = [
        "# Emerald Champions rival and superboss anchor designs",
        "",
        f"Progress: {payload['designed_count']}/{len(EXPECTED_ORDER)} primary anchors are design-complete; source remains untouched.",
        "",
    ]
    for anchor_id, dossier in payload["designs"].items():
        lines.extend([
            f"## {anchor_id}", "",
            f"- Status: design `{dossier['status']['design']}`, source `{dossier['status']['source']}`, runtime `{dossier['status']['runtime']}`.",
            f"- Format/cap: {dossier['runtime']['canonical_format']}, cap {dossier['campaign_state']['strict_cap']}.",
            f"- Primary question: {dossier['identity']['primary_player_question']}",
            f"- Strongest part: {dossier['author_self_check']['strongest_part']}",
            f"- Weakest link: {dossier['author_self_check']['weakest_link']}",
            f"- First-loss lesson: {dossier['counterplay']['first_loss_lesson']}",
            f"- References: {', '.join(f'`{ref}`' for ref in dossier['competitive_research']['selected_reference_ids'])}",
            "- Team:",
        ])
        for mon in dossier["team"]:
            mega = "; Mega" if mon["mega_candidate"] else ""
            lines.append(f"  - `{mon['species']}` — `{mon['item']}`, `{mon['ability']}`{mega}; " + ", ".join(f"`{move}`" for move in mon["moves"]))
        for label, opponent_team in dossier.get("opponent_teams", {}).items():
            lines.append(f"- {label.title()} opponent team:")
            for mon in opponent_team:
                lines.append(f"  - `{mon['species']}` — `{mon['item']}`, `{mon['ability']}`; " + ", ".join(f"`{move}`" for move in mon["moves"]))
        lines.extend(["", f"AI must execute: {' '.join(dossier['ai']['custom_requirements'])}", ""])
    review = payload["anchor_review"]
    lines.extend([
        "## Tranche review", "",
        f"- Distinct species: {review['distinct_species_count']} across {review['slot_count']} slots; unwaived internal collisions: {len(review['unwaived_internal_species_collisions'])}; unwaived protected collisions: {len(review['unwaived_protected_collisions'])}.",
        f"- Unique Mega signatures: {len(review['unique_mega_signatures'])}/{payload['designed_count']}.",
        f"- Judgment: {review['judgment']}", "",
    ])
    if payload["designed_count"] < len(EXPECTED_ORDER):
        lines.extend(["## Next anchor", "", f"`{EXPECTED_ORDER[payload['designed_count']]}`", ""])
    else:
        lines.extend(["## Primary anchor board complete", "", "Next: Frontier brains and rematch anchors, followed by campaign-wide collision review.", ""])
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
            raise SystemExit("FAIL: superboss anchor JSON is missing or stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: superboss anchor Markdown is missing or stale")
    print(f"PASS: {payload['designed_count']}/{len(EXPECTED_ORDER)} rival/superboss anchors are design-complete and source-honest")
    print(f"PASS: {payload['anchor_review']['distinct_species_count']} distinct species, {len(payload['anchor_review']['unique_mega_signatures'])} unique Megas, and zero unwaived collisions")
    if payload["designed_count"] < len(EXPECTED_ORDER):
        print(f"NEXT: {EXPECTED_ORDER[payload['designed_count']]}")
    else:
        print("NEXT: Frontier brains and rematch anchors")


if __name__ == "__main__":
    main()
