#!/usr/bin/env python3
"""Generate and verify paired Magma/Aqua marquee battle dossiers."""

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


OUTPUT_JSON = ROOT / "docs/emerald_champions_faction_anchor_designs.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_faction_anchor_designs.md"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
LEAGUE_PATH = ROOT / "docs/verdant_marquee_battle_designs.json"
GYMS_PATH = ROOT / "docs/emerald_champions_gym_anchor_designs.json"
META_PATH = ROOT / "docs/competitive_team_index.meta.json"

EXPECTED_PAIR_ORDER = [
    ["MAGMA_HIDEOUT_FINAL_MAXIE", "SEAFLOOR_CAVERN_FINAL_ARCHIE"],
    ["MAGMA_HIDEOUT_COURTNEY", "SEAFLOOR_CAVERN_SHELLY"],
    ["MAGMA_HIDEOUT_TABITHA", "AQUA_HIDEOUT_MATT"],
    ["MT_CHIMNEY_MAXIE", "MT_PYRE_MATT"],
    ["MT_CHIMNEY_TABITHA", "WEATHER_INSTITUTE_SHELLY"],
    ["METEOR_FALLS_COURTNEY", "SLATEPORT_ARCHIE"],
]

ALLOWED_PROTECTED_REUSES = {
    ("SEAFLOOR_CAVERN_FINAL_ARCHIE", "SPECIES_KYOGRE"): "Archie's story-signature Primal Kyogre and Wallace's later base Kyogre ask different questions; this is an intentional legendary reprise, not roster filler.",
}

ALLOWED_INTERNAL_REUSES = {
    ("MT_CHIMNEY_MAXIE", "SPECIES_GROUDON"): "Base Groudon foreshadows Maxie's later Primal Groudon; the same leader's signature threat gains its transformation and full geometry only at the finale.",
    ("MT_PYRE_MATT", "SPECIES_DHELMISE"): "Dhelmise is Matt's recurring anchor; its Mt. Pyre grave-tide role matures into the later boarding-party trap without repeating the surrounding roster.",
    ("MT_CHIMNEY_TABITHA", "SPECIES_COALOSSAL"): "Coalossal is Tabitha's recurring engine; Mt. Chimney shows the unactivated prototype, while Magma Hideout later adds the one tournament-grade Surf ignition.",
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


def maxie_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen8randomdoublesbattle:012": (
            "selected-core",
            "The generated Groudon-Oranguru roster proves the exact restricted plus Telepathy-support pairing. Maxie authors Gravity and Instruct as visible earth geometry rather than importing its unrelated snow and healing pieces.",
        ),
        "vgc:naic-2022": (
            "selected-history",
            "The 2022 North American International Champion roster validates Groudon as the center of a top-level sun offense. Maxie rejects Zacian, Gigantamax, generic screens, and the complete tournament shell.",
        ),
        "vgc:regional-melbourne-2025": (
            "selected-history",
            "The 2025 Melbourne-winning sun roster validates Walking Wake beside harsh sunlight. Maxie imports only Hydro Steam's anti-Water inversion and no Koraidon, Tera, or restricted-pair assumptions.",
        ),
        "showdown:gen9randomdoublesbattle:012": (
            "selected-set",
            "The generated Great Tusk set supplies Headlong Rush, Close Combat, and Knock Off as legitimate doubles pressure. Maxie adds Assault Vest and Rock Slide for a finite heavy relay.",
        ),
        "showdown:gen7randomdoublesbattle:004": (
            "adapted-role",
            "The generated Cherrim roster validates Flower Gift support in doubles. Maxie makes it a frail visible physical amplifier with no recovery, evasion, or sleep.",
        ),
        "showdown:gen9championsrandomdoublesbattle:025": (
            "adapted-set",
            "The Champions generator validates Camerupt as a slow special doubles threat. Maxie reserves its Mega form as the volcanic signature and removes the source team's sand, Trick Room, and Choice-disruption shell.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current roster is generic sun plus sleep and repeats Maxie's earlier Crobat and Victreebel language. The final fight should express land control through Gravity, Telepathy, Flower Gift, Instruct, ancient sun, and a signature Mega rather than accuracy variance.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_GROUDON",
            "level_offset": 1,
            "item": "ITEM_RED_ORB",
            "ability": "ABILITY_DROUGHT",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_PRECIPICE_BLADES", "MOVE_HEAT_CRASH", "MOVE_SWORDS_DANCE", "MOVE_PROTECT"],
            "role": "Primal land engine and public center of gravity; its spread attack, harsh sunlight, and physical setup create the board Maxie's support pieces manipulate.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_CHERRIM",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_FLOWER_GIFT",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_HELPING_HAND", "MOVE_SOLAR_BEAM", "MOVE_WEATHER_BALL", "MOVE_PROTECT"],
            "role": "Frail public land-alliance amplifier: Flower Gift and Helping Hand make the physical board lethal, but removing it immediately collapses that layer.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ORANGURU",
            "level_offset": 2,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_TELEPATHY",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_GRAVITY", "MOVE_INSTRUCT", "MOVE_PSYCHIC", "MOVE_PROTECT"],
            "role": "Telepathic geometry controller that can ground airborne answers, improve Precipice accuracy, or repeat a disclosed partner attack without taking allied spread damage.",
            "lead_group": "geometry-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_GREAT_TUSK",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_PROTOSYNTHESIS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HEADLONG_RUSH", "MOVE_CLOSE_COMBAT", "MOVE_ROCK_SLIDE", "MOVE_KNOCK_OFF"],
            "role": "Ancient physical land relay whose sun activation and four attacks convert Maxie's field state into immediate mixed coverage rather than another setup turn.",
            "lead_group": "heavy-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_WALKING_WAKE",
            "level_offset": 3,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_PROTOSYNTHESIS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HYDRO_STEAM", "MOVE_DRAGON_PULSE", "MOVE_FLAMETHROWER", "MOVE_PROTECT"],
            "role": "The anti-Water inversion: Maxie's sunlight strengthens rather than suppresses its signature Water attack, forcing the player to read the move instead of the type chart alone.",
            "lead_group": "inversion-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_CAMERUPT",
            "level_offset": 4,
            "item": "ITEM_CAMERUPTITE",
            "ability": "ABILITY_ANGER_POINT",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_ERUPTION", "MOVE_EARTH_POWER", "MOVE_HEAT_WAVE", "MOVE_PROTECT"],
            "role": "Maxie's sole Mega and volcanic final landmass: slow, visibly damage-sensitive, and brutally strong without borrowing Flannery's After You or Trick Room engines.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MAGMA_HIDEOUT_FINAL_MAXIE",
        "planning_tier": "faction_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Final solo Maxie battle in Magma Hideout after six Badges and before the Mossdeep invasion",
            "location": "MagmaHideout_4F",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and all legal catches and progression items available through Mt. Pyre and Magma Hideout",
                "The reusable Leveler, every legal move source, on-demand legal abilities, and free ordinary competitive held items",
                "Mega Bracelet and all Mega Stones earned before the hideout",
                "Manual party preparation immediately before the final chamber",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Maxie uses one Mega Camerupt plus the explicitly allowed Primal Groudon. Primal Reversion does not consume the sole Mega slot; no other gimmick appears.",
            "evolution_phase": "Late campaign faction climax: fully evolved, legendary, paradox, and signature Mega threats are appropriate.",
            "preparation_access": "The player may heal and rebuild before the chamber. The preceding Magma trainers are not a no-menu League-style party lock.",
            "gauntlet_position": "Magma's final solo ideological exam. It must reserve Flannery's move-order heat and make land geometry, ally immunity, and physical amplification the faction's identity.",
            "mechanics_baseline_id": "faction_finale",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_MAXIE_MAGMA_HIDEOUT"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "magma_hideout_final", "trainer_ids": ["TRAINER_MAXIE_MAGMA_HIDEOUT"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "mt_chimney_maxie", "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required battle"},
                {"variant_id": "mossdeep_multi_maxie", "trainer_ids": ["TRAINER_MAXIE_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MaxieMagmaHideout",
                "src/data/trainers.h:TRAINER_MAXIE_MAGMA_HIDEOUT",
                "data/maps/MagmaHideout_4F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Magma Hideout, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MAGMA_HIDEOUT_TABITHA", "MAGMA_HIDEOUT_COURTNEY", "MOSSDEEP_SPACE_CENTER_MULTI_CLIMAX", "LAVARIDGE_GYM_FLANNERY"],
            "required_preimplementation_review": "Refresh the final ten hideout encounters. Preserve Gravity plus Telepathy, Flower Gift physical land pressure, Walking Wake's sun inversion, Primal Groudon, and Mega Camerupt unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Maxie makes the battlefield itself fall toward him: Cherrim strengthens the land, Oranguru removes flight and repeats tremors, ancient beasts exploit the sun, and Mega Camerupt becomes the last volcano.",
            "story_fit": "Team Magma's ideology becomes tactical geometry. Maxie does not merely make sunlight; he removes safe airspace, protects his own controller from allied tremors, and turns every acre into attacking ground.",
            "primary_player_question": "Can the player break Maxie's land-support geometry before Flower Gift, Gravity, Telepathy, or Instruct turns Primal Groudon and Great Tusk into repeated board-wide pressure, while preserving a plan for sun-boosted Hydro Steam and Mega Camerupt?",
            "primary_mode": "Primal Groudon plus Cherrim exposes harsh sun, physical amplification, and Precipice pressure immediately; Cherrim is powerful but deliberately targetable.",
            "secondary_mode": "Oranguru can ground immunities and repeat disclosed attacks, Great Tusk supplies direct physical coverage, Walking Wake inverts Water counterplay, and Mega Camerupt closes without artificial speed control.",
            "preview_pressure": "The preview openly shows one Primal, one Mega, one frail Flower Gift amplifier, one Telepathy controller, and two ancient sun beneficiaries. The puzzle is which support layer must die first.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 61 through 64 against cap 60 with one Primal, one Mega, spread Ground pressure, physical amplification, ally immunity, accuracy and grounding control, attack repetition, and an anti-Water inversion. Support pieces are frail or finite and the team has no sleep, redirection, healing loop, or automatic speed mode.",
            "pressure_sources": [
                "Primal Groudon harsh sunlight, Precipice Blades, setup threat, and broad physical bulk",
                "Focus Sash Cherrim Flower Gift plus Helping Hand",
                "Mental Herb Telepathy Oranguru using Gravity or Instruct",
                "Sun-activated Assault Vest Great Tusk with four immediate attacks",
                "Life Orb Walking Wake turning sunlight into Hydro Steam pressure",
                "Mega Camerupt high-HP Eruption and mixed Fire/Ground spread pressure",
            ],
            "resource_tax": "The fight taxes support-target priority, Wide Guard and Protect timing, weather control, Ground immunity, physical Intimidate or burn, Water-counter discipline, and enough late special bulk for Mega Camerupt.",
            "tuning_order": [
                "Preserve land geometry, one Primal, one Mega, Flower Gift, and Hydro Steam inversion",
                "Validate Gravity, Telepathy, Instruct, Precipice, and transformation timing before changing sets",
                "Adjust offsets within +1 to +4, beginning with Camerupt, Walking Wake, and Groudon",
                "Then adjust Oranguru or Cherrim survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_GROUDON", "SPECIES_CHERRIM"],
            "mandatory_order_reason": "The lead makes Maxie's thesis public at once. Later reserves are board-state selections, not a scripted sequence of paired modules.",
            "reserve_sequence": [
                "Use Oranguru when Gravity meaningfully removes an immunity or Instruct can repeat an already selected visible attack; otherwise it attacks or protects.",
                "Use Great Tusk when physical Ground/Fighting/Rock pressure is correct and the partner can tolerate its line.",
                "Use Walking Wake against disclosed Water plans or when its fast special coverage is the best sun beneficiary.",
                "Preserve Mega Camerupt as the final landmass when practical, but deploy it earlier if its bulk and coverage are the only correct reserve.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Groudon and Cherrim jointly: Helping Hand requires meaningful same-turn damage and Cherrim should attack when support is redundant.",
                "Use Gravity only when accuracy or grounding changes a visible target interaction; do not ground Maxie's own vulnerable partner into allied spread damage without superior payoff.",
                "Use Instruct only after a legal partner has a valuable repeatable last move and can act; never target Protect, setup, an incapacitated ally, or a move that would self-sabotage the board.",
                "Account for Telepathy before choosing allied spread Ground moves and use independent attacks when the intended immune partner is absent.",
                "Evaluate Eruption from current HP and trigger Primal Groudon and Mega Camerupt through normal transformation timing.",
            ],
            "forbidden_behaviors": [
                "Do not use sleep, evasion, hidden information, or guaranteed Precipice accuracy outside active Gravity.",
                "Do not spam Gravity or Instruct merely because Oranguru is active.",
                "Do not import Flannery's After You, Trick Room, or thermal trap sequence.",
                "Do not add Tera, Z-Move, Dynamax, Gigantamax, a second Mega, or another Primal.",
            ],
            "state_machine": "State A opens Primal Groudon-Cherrim land pressure. State B enables Oranguru geometry only when grounding, accuracy, or repetition changes the visible board. State C selects Great Tusk or Walking Wake as physical or anti-Water inversion. State D exposes Mega Camerupt as the final landmass. Each state has independent-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Remove or suppress Cherrim, replace harsh sun where possible, Intimidate or burn the physical core, and use Wide Guard or Protect against spread turns.",
                "Taunt, double-target, Encore, or pressure Oranguru before Gravity and Instruct compound; exploit that it is the only geometry controller.",
                "Use Flying or Levitate before Gravity, reposition when Gravity expires, and exploit Ground immunities or Telepathy asymmetry rather than assuming Precipice always connects.",
                "Answer Walking Wake with Dragon, Fairy, special bulk, priority, or weather discipline instead of sending Water into Hydro Steam blindly.",
                "Damage Mega Camerupt before Eruption, use Water after harsh sun ends, or preserve Ground, special bulk, Wide Guard, and priority for its slow finale.",
            ],
            "intentional_weakness": "Cherrim is frail and support-dependent; Oranguru is the only Gravity/Instruct controller; Great Tusk drops defenses; Walking Wake and Camerupt lack recovery; Camerupt is extremely slow; and the team has no redirection, Fake Out, sleep, healing loop, or dedicated speed control.",
            "first_loss_lesson": "The land itself is the combo. Remove Cherrim if physical amplification is killing you, remove Oranguru if geometry is, never assume Water is safe into Walking Wake, and arrive at Mega Camerupt with a way to reduce Eruption before it moves.",
            "revealed_information": [
                "Harsh sunlight, Flower Gift, Gravity turns, Telepathy interactions, prior moves available to Instruct, current HP, Primal Reversion, and Mega evolution are public state.",
                "Instruct can repeat only a real previously used legal move; no custom action bypass is proposed.",
                "Gravity supplies ordinary accuracy and grounding effects only while active.",
                "Primal Groudon and Mega Camerupt are the only transformations.",
            ],
            "unacceptable_failure_modes": [
                "AI uses Gravity without changing accuracy or immunity value",
                "Instruct repeats Protect, setup, or an illegal/dead partner action",
                "Groudon damages a non-Telepathy partner with a worse spread line",
                "Walking Wake is treated as ordinary Water under harsh sun",
                "Maxie becomes a generic sleep-and-sun team or duplicates Flannery",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Groudon Oranguru doubles", "Groudon tournament champion", "Walking Wake sun winner", "Great Tusk doubles", "Cherrim Flower Gift", "Mega Camerupt Champions"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Zacian, Koraidon, Tera, Gigantamax, generic screens, redirection, sleep, and complete tournament shells are not imported.",
                "Flannery's After You Eruption and Trick Room heat modes remain exclusive to her Gym.",
                "Accuracy variance is reduced through conditional Gravity rather than Hypnosis or Sleep Powder.",
                "No second Mega or second Primal appears.",
            ],
            "imported_elements": [
                "Generated Groudon-Oranguru role legitimacy",
                "Tournament-proven Groudon sun pressure",
                "Tournament-proven Walking Wake sun inversion",
                "Generated Great Tusk and Cherrim doubles roles",
                "Champions-generator Camerupt adapted into Maxie's signature Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Primal Groudon as Maxie's final solo centerpiece",
                "The campaign's definitive Gravity plus Telepathy Ground geometry",
                "Cherrim Flower Gift as visible land solidarity",
                "Walking Wake's sunlight-powered Water inversion",
                "Mega Camerupt as Maxie's signature final landmass",
            ],
            "preserves": [
                "Flannery's After You and Fire-native Trick Room thermal lesson",
                "Archie's priority, pivot, and rain-current identity",
                "Wallace's dual-speed rain championship exam",
                "Other Groudon teams as earlier incomplete lessons rather than repeats of this full geometry",
            ],
            "releases": [
                "Crobat, Victreebel, Hydreigon, and Lycanroc leave final Maxie for earlier faction or unrelated encounters",
                "Other Ground, Fire, sun, and paradox species remain available if they do not recreate Gravity-Instruct land geometry",
            ],
            "collision_notes": [
                "No species overlaps the six Gym anchors, five League anchors, or final Archie's paired design.",
                "Camerupt was deliberately removed from Flannery so its Mega belongs to Maxie without duplication.",
                "Sun is a shared world resource, but Maxie owns terrain geometry and physical land amplification rather than move-order heat.",
            ],
        },
        "presentation": {
            "intro_concept": "Maxie declares that expanding land means removing every refuge above it; the battlefield will obey the same geometry as his new world.",
            "defeat_concept": "He recognizes that the player dismantled the support holding his land together rather than merely overpowering Groudon.",
            "post_battle_concept": "Native story progression remains unchanged. His loss exposes the flaw in treating a controlled battlefield as a controlled world.",
            "hint_concept": "Nearby Magma dialogue warns that the flower strengthens the earth, the sage can pull flyers down and repeat tremors, and sunlight may empower a Water attack instead of weakening it.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, Primal Groudon-Cherrim opening, Oranguru Gravity/Instruct and Telepathy, Great Tusk physical relay, Walking Wake sun inversion, Mega Camerupt finale, transformation timing, broad counterplay, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Gravity, Telepathy, Instruct, Flower Gift, and Hydro Steam all express Maxie's ideology through battle mechanics; this is land control, not just six sun-abusing attackers.",
            "weakest_link": "Primal Groudon plus support can become oppressive before the player sees the full lesson. Cherrim's fragility, conditional Oranguru logic, absent speed mode, and broad Wide Guard, Taunt, weather, Intimidate, burn, and support-removal counterplay must survive implementation.",
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
                "The source guide places final Maxie at strict cap 60 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Red Orb and Cameruptite are the only transformation items; Cameruptite maps to Mega Camerupt.",
                "All six competitive references exist in the current indexed corpus, including champion, generated, and tournament-winning evidence.",
                "No exact source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_MaxieMagmaHideout with the six exact authored sets and offsets.",
                "Add partner, HP, field, and combo AI flags and implement Maxie's board-state reserve selector.",
                "Implement and regression-test Gravity, Telepathy, Instruct legality, Flower Gift, harsh sun, Hydro Steam, Primal timing, Mega timing, current-HP Eruption, and simultaneous replacements.",
                "Prove Red Orb and Cameruptite coexist under the intended one-Mega-plus-one-Primal rule.",
                "Write and font-measure exact dialogue; update the source-derived guide and reservation ledger.",
                "Run cap-60 Wide Guard, Taunt, weather, Flying/Levitate, Ground, Water, Dragon/Fairy, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def archie_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen9randomdoublesbattle:025": (
            "selected-core",
            "The generated Kyogre-Tsareena roster validates rain pressure beside priority denial. Archie authors the pair as a Water Spout launch rather than importing its unrelated Fake Out and terrain pieces.",
        ),
        "elite:wolfe:toronto-2024": (
            "selected-role",
            "Wolfe Glick's Toronto rain team validates Archaludon as a primary rain attacker. Archie imports instant Electro Shot and accumulated Stamina but explicitly rejects the team's Perish trap, redirection, sleep, and Shadow Tag mode.",
        ),
        "showdown:gen9randomdoublesbattle:019": (
            "selected-set",
            "The generated Palafin set supplies Zero to Hero, Flip Turn, Wave Crash, and Jet Punch as a complete momentum cycle. Local legality replaces unsupported Close Combat with Ice Punch.",
        ),
        "elite:paul-chua:euic-2026": (
            "adapted-architecture",
            "The 2026 EUIC Champion roster validates Urshifu-Rapid in no-Tailwind priority balance. Archie imports pivot and priority tempo, not the event's grassy terrain, Tera, or full balance shell.",
        ),
        "showdown:gen6randomdoublesbattle:004": (
            "adapted-set",
            "The generated Sharpedo roster validates late Speed Boost offense. Archie upgrades the local signature Mega into the final current rather than importing unrelated sand and setup pieces.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current roster is six largely independent rain beneficiaries with Hypnosis, passive Rest, and limited partner logic. The final Aqua fight should feel like an accelerating current: priority denial, Water Spout thresholds, rain charging, forced pivots, Protect pressure, and one fast Mega finish.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_KYOGRE",
            "level_offset": 1,
            "item": "ITEM_BLUE_ORB",
            "ability": "ABILITY_DRIZZLE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_WATER_SPOUT", "MOVE_ORIGIN_PULSE", "MOVE_THUNDER", "MOVE_PROTECT"],
            "role": "Primal flood engine whose current HP is the public pressure gauge; Water Spout can be blunted while Origin Pulse and Thunder prevent a one-answer plan.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_TSAREENA",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_QUEENLY_MAJESTY",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_POWER_WHIP", "MOVE_TRIPLE_AXEL", "MOVE_U_TURN", "MOVE_PROTECT"],
            "role": "Shoreline guard that blocks easy priority into Kyogre, threatens opposing Water and Grass answers, and can hand momentum to the next current.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ARCHALUDON",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_STAMINA",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_ELECTRO_SHOT", "MOVE_FLASH_CANNON", "MOVE_DRAGON_PULSE", "MOVE_BODY_PRESS"],
            "role": "Rain-charged breakwater: Electro Shot attacks immediately, Stamina converts contact into Body Press pressure, and its typing punishes simplistic anti-Water teams.",
            "lead_group": "breakwater-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_PALAFIN",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_ZERO_TO_HERO",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FLIP_TURN", "MOVE_WAVE_CRASH", "MOVE_ICE_PUNCH", "MOVE_JET_PUNCH"],
            "role": "Literal receding current: it must leave once through Flip Turn before returning as Hero form with rain-amplified pressure and priority.",
            "lead_group": "pivot-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_URSHIFU_RAPID_STRIKE_STYLE",
            "level_offset": 3,
            "item": "ITEM_CHOICE_BAND",
            "ability": "ABILITY_UNSEEN_FIST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_SURGING_STRIKES", "MOVE_CLOSE_COMBAT", "MOVE_AQUA_JET", "MOVE_U_TURN"],
            "role": "Protect-punishing breaker whose Choice lock is both extreme pressure and exploitable commitment; U-turn sustains Archie's momentum without a speed setter.",
            "lead_group": "breaker-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_SHARPEDO",
            "level_offset": 4,
            "item": "ITEM_SHARPEDONITE",
            "ability": "ABILITY_SPEED_BOOST",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_LIQUIDATION", "MOVE_CRUNCH", "MOVE_PSYCHIC_FANGS", "MOVE_PROTECT"],
            "role": "Archie's sole Mega and final rip current: one protected Speed Boost can create the endgame, but its physical frailty keeps the answer interactive.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "SEAFLOOR_CAVERN_FINAL_ARCHIE",
        "planning_tier": "faction_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Final solo Archie battle in Seafloor Cavern after seven Badges and immediately before the weather crisis",
            "location": "SeafloorCavern_Room9",
            "strict_cap": 70,
            "player_tools": [
                "Seven Badges and the complete pre-crisis Surf, Dive, Waterfall, route, cave, and faction catch pools",
                "The reusable Leveler, every legal move source, on-demand legal abilities, and free ordinary competitive held items",
                "Mega Bracelet and all campaign Mega Stones earned before Seafloor Cavern",
                "Manual party preparation before the final cavern room",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Archie uses one Mega Sharpedo plus the explicitly allowed Primal Kyogre. Primal Reversion does not consume the sole Mega slot; no other gimmick appears.",
            "evolution_phase": "Late campaign faction climax: fully evolved, legendary, Gen 9, and signature Mega threats are appropriate.",
            "preparation_access": "The player may heal and rebuild before Archie. Shelly is a preceding separate battle, not a no-menu League-style party lock.",
            "gauntlet_position": "Aqua's final solo ideological exam and the direct prelude to the weather crisis. It must be relentless momentum rather than Maxie's geometry or Wallace's dual-speed rain control.",
            "mechanics_baseline_id": "faction_finale",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_ARCHIE"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "seafloor_cavern_final", "trainer_ids": ["TRAINER_ARCHIE"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "slateport_interception", "trainer_ids": ["TRAINER_ARCHIE_SLATEPORT"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required museum battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Archie",
                "src/data/trainers.h:TRAINER_ARCHIE",
                "data/maps/SeafloorCavern_Room9/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Seafloor Cavern, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["SEAFLOOR_CAVERN_SHELLY", "SOOTOPOLIS_GYM_JUAN", "CHAMPION_WALLACE", "MAGMA_HIDEOUT_FINAL_MAXIE"],
            "required_preimplementation_review": "Refresh the final ten cavern encounters. Preserve Kyogre-Tsareena priority denial, Archaludon rain charge, Palafin's required pivot, Choice-locked Urshifu pressure, and Mega Sharpedo's final Speed Boost unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Archie's tide never holds still: Kyogre floods behind Tsareena, Archaludon charges in the storm, Palafin leaves and returns transformed, Urshifu breaks shelter, and Mega Sharpedo becomes the rip current.",
            "story_fit": "Team Aqua's ideology becomes momentum. Water pressure rises, recedes, pivots, bypasses protection, and returns stronger; Archie wins by never letting the opponent establish a still board.",
            "primary_player_question": "Can the player reduce Primal Kyogre's Water Spout before Tsareena denies priority, then survive an accelerating chain of rain-charged Archaludon, forced Palafin and Urshifu pivots, Protect punishment, and Mega Sharpedo speed?",
            "primary_mode": "Primal Kyogre plus Tsareena creates a visible Water Spout threshold protected from easy priority but still vulnerable to spread defense, faster direct damage, weather, and Tsareena removal.",
            "secondary_mode": "Archaludon converts rain into instant charge, Palafin must visibly cycle into Hero form, Urshifu punishes passive Protect, and Mega Sharpedo turns one protected speed turn into the finale.",
            "preview_pressure": "Four Water attackers advertise rain, but the two off-type supports specifically punish anti-Water autopilot. No Tailwind or Trick Room appears; momentum comes from field weather, pivots, priority, and Speed Boost.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 71 through 74 against cap 70 with one Primal, one Mega, high-HP Water Spout, priority denial, rain-charged Electro Shot, Hero-form cycling, Protect punishment, Choice pressure, and late Speed Boost. The team has no sleep, redirection, healing loop, Tailwind, or Trick Room and exposes clear HP, lock, pivot, and physical-bulk counterplay.",
            "pressure_sources": [
                "Primal Kyogre Water Spout and Origin Pulse under permanent heavy rain",
                "Focus Sash Tsareena blocking priority and pivoting with U-turn",
                "Assault Vest Stamina Archaludon charging Electro Shot instantly",
                "Life Orb Palafin cycling through Flip Turn into Hero form",
                "Choice Band Urshifu-Rapid bypassing Protect contact safety",
                "Mega Sharpedo using one Protect and Speed Boost to create a final rip current",
            ],
            "resource_tax": "The fight taxes immediate HP control, Wide Guard, weather replacement, priority planning, anti-pivot positioning, physical Intimidate or burn, Choice-lock exploitation, and enough reserve speed or priority for Mega Sharpedo.",
            "tuning_order": [
                "Preserve priority-protected Water Spout, rain charge, Hero pivot, Protect punishment, and final Speed Boost",
                "Validate HP-aware move choice, Zero to Hero, pivot targets, Unseen Fist, Choice lock, and transformation timing before changing sets",
                "Adjust offsets within +1 to +4, beginning with Sharpedo, Urshifu, and Kyogre",
                "Then adjust Tsareena or Archaludon survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_KYOGRE", "SPECIES_TSAREENA"],
            "mandatory_order_reason": "The lead establishes Archie's pressure gauge and priority shield. The four reserves are selected by current and board state rather than appearing as fixed pairs.",
            "reserve_sequence": [
                "Use Archaludon while rain is active and its typing, Stamina, or Electro Shot punishes the disclosed response.",
                "Use Palafin early enough to complete one legal Flip Turn cycle; after Hero activation, choose direct pressure instead of forcing another pivot.",
                "Use Urshifu when Protect pressure, Fighting coverage, priority, or U-turn gives immediate visible value; account for its Choice lock.",
                "Preserve Mega Sharpedo for the final rip current when practical, but deploy it earlier if its coverage or speed is the only correct reserve.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Evaluate Water Spout from Kyogre's current HP and prefer Origin Pulse, Thunder, or Protect when the visible damage line is stronger.",
                "Account for Queenly Majesty when scoring priority from either side without reading hidden player move selection.",
                "Give base Palafin one high-priority legal Flip Turn cycle with a valid reserve, then recognize Hero form and stop sacrificing damage to redundant pivots.",
                "Score Urshifu's Unseen Fist and Choice lock from public state; U-turn only when the matchup and reserve improve.",
                "Use a current-based reserve selector and Mega Evolve Sharpedo normally; Protect for Speed Boost only when survival and next-turn value justify it.",
            ],
            "forbidden_behaviors": [
                "Do not spam low-HP Water Spout, redundant Palafin pivots, or Sharpedo Protect when a knockout is available.",
                "Do not infer hidden priority, Protect, switches, or items.",
                "Do not import Wallace's Tailwind-Trick Room rain arc or Wolfe Toronto's Perish trap.",
                "Do not add Tera, Z-Move, Dynamax, Gigantamax, a second Mega, or another Primal.",
            ],
            "state_machine": "State A opens Kyogre-Tsareena pressure. State B selects Archaludon as the rain-charged breakwater. State C ensures base Palafin completes one legal outward current and permits Hero return. State D uses Urshifu for Protect-punishing commitment. State E exposes Mega Sharpedo as the final rip current. Every state has a direct-attack and missing-reserve fallback.",
        },
        "counterplay": {
            "classes": [
                "Damage Kyogre immediately, replace or suppress rain, use Wide Guard, Water immunity, special bulk, Snarl, or Protect sequencing to reduce Water Spout value.",
                "Remove Tsareena, use non-priority speed control, or exploit its Sash and modest bulk before relying on priority.",
                "Pressure Archaludon specially, deny rain charge, exploit Ground or Fighting coverage, or avoid feeding Stamina with weak physical contact.",
                "Punish Palafin's forced first pivot with hazards, matchup pressure, trapping where legal, or a reserve plan for Hero form; exploit Urshifu's public Choice lock.",
                "Use Intimidate, burn, Rocky Helmet, physical bulk, faster control, or priority after Queenly Majesty leaves to stop Urshifu and Mega Sharpedo.",
            ],
            "intentional_weakness": "Kyogre's strongest move is HP-sensitive; Tsareena is frail; Archaludon can be hit specially and lacks Protect; base Palafin must spend a pivot; Urshifu is Choice-locked; Mega Sharpedo is physically frail and exposes a Protect turn. There is no redirection, sleep, healing loop, Tailwind, or Trick Room.",
            "first_loss_lesson": "This fight accelerates if you let it flow. Cut Kyogre's HP before Water Spout, remove Tsareena before depending on priority, punish Palafin's first exit, exploit Urshifu's lock, and do not give Mega Sharpedo a free protected speed turn.",
            "revealed_information": [
                "Heavy rain, current HP, Queenly Majesty, Stamina boosts, Palafin form, Choice lock after move use, Speed Boost, Primal Reversion, and Mega evolution are public state.",
                "Palafin must complete an ordinary legal switch before Hero form; no scripted form grant is proposed.",
                "Urshifu receives ordinary Unseen Fist behavior only on qualifying contact moves.",
                "Primal Kyogre and Mega Sharpedo are the only transformations.",
            ],
            "unacceptable_failure_modes": [
                "AI uses low-HP Water Spout over a stronger visible move",
                "Base Palafin fails to pivot despite a safe legal reserve or keeps pivoting after Hero activation",
                "Urshifu ignores its Choice lock or predicts hidden Protect",
                "Sharpedo Protect loops instead of closing",
                "Archie becomes a generic Swift Swim team or duplicates Wallace's dual-speed rain",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Kyogre Tsareena doubles", "Wolfe Archaludon rain", "Palafin random doubles", "Urshifu no Tailwind priority balance", "Mega Sharpedo doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Wolfe Toronto's Perish, Shadow Tag, redirection, sleep, and Trick Room modes are explicitly rejected.",
                "Tera, grassy terrain, generic Incineroar balance, Tailwind, and a complete tournament shell are not imported.",
                "Hypnosis and passive Rest loops from the current source roster are removed.",
                "No second Mega or second Primal appears.",
            ],
            "imported_elements": [
                "Generated Kyogre-Tsareena pressure and priority-denial legitimacy",
                "Wolfe-documented Archaludon primary rain offense without the trap mode",
                "Generated Palafin Zero to Hero momentum cycle",
                "Tournament-winning no-Tailwind Urshifu priority balance",
                "Generated Sharpedo adapted into Archie's signature Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Primal Kyogre as Archie's final solo centerpiece",
                "The campaign's definitive priority-protected Water Spout opening",
                "Archaludon rain-charged breakwater",
                "Palafin's required Zero to Hero current cycle",
                "Mega Sharpedo as Archie's signature final rip current",
            ],
            "preserves": [
                "Wallace's dual-speed rain championship arc",
                "Juan's rainless Surf-absorption relay",
                "Maxie's Gravity-Instruct land geometry",
                "Wolfe Toronto's complete Perish trap for another late notorious encounter if used at all",
            ],
            "releases": [
                "Goodra, Tentacruel, Poliwrath, and Eelektross leave final Archie for other Aqua, route, or specialist battles",
                "Other rain and Water species remain available if they do not duplicate the full priority-pivot-current sequence",
            ],
            "collision_notes": [
                "Kyogre intentionally returns later on Wallace: Archie owns Primal flood momentum, while Wallace uses base Kyogre inside a dual-speed champion arc. No other species overlaps the six Gyms, five League anchors, or final Maxie.",
                "Four of six members are Water types; Tsareena and Archaludon earn their slots by protecting and charging the flood rather than diluting Aqua's identity.",
                "Archie owns momentum, form cycling, and Protect pressure. Wallace retains dual speed and Juan retains ally Surf activation.",
            ],
        },
        "presentation": {
            "intro_concept": "Archie says the sea never asks permission and never stays where it was; every attempt to shelter will only redirect the next wave.",
            "defeat_concept": "He recognizes that the player broke the current at each handoff instead of trying to hold back the entire ocean at once.",
            "post_battle_concept": "Native story progression into the weather crisis remains unchanged. His defeat does not stop the consequence of awakening Kyogre.",
            "hint_concept": "Nearby Aqua dialogue warns that the queen blocks priority, the bridge charges in rain, the dolphin must leave before returning stronger, and Sharpedo only needs one safe turn to become the rip current.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 70, Primal Kyogre-Tsareena Water Spout opening, Archaludon rain charge, Palafin Hero cycle, Choice Band Urshifu Protect pressure, Mega Sharpedo finale, HP-aware and pivot-aware AI, broad counterplay, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Every reserve advances the same tidal verb in a different way—charge, leave, break shelter, accelerate—so the battle feels like Aqua without defaulting to six Swift Swim attackers.",
            "weakest_link": "Kyogre, Palafin, Urshifu, and Mega Sharpedo stack enormous Water physical pressure. Tsareena removal, Water Spout HP dependence, Palafin's required exit, Urshifu's Choice lock, Sharpedo's frailty, and the absence of Tailwind, redirection, and healing must keep the puzzle broad rather than automatic.",
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
                "The source guide places final Archie at strict cap 70 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Blue Orb and Sharpedonite are the only transformation items; Sharpedonite maps to Mega Sharpedo.",
                "All five competitive references exist in the current indexed corpus, including Wolfe, champion, and generated full-set evidence.",
                "No exact source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Archie with the six exact authored sets and offsets.",
                "Add partner, HP, field, and combo AI flags and implement Archie's current-based reserve selector.",
                "Implement and regression-test Water Spout HP choice, Queenly Majesty, Stamina, rain Electro Shot, Zero to Hero pivot state, Unseen Fist, Choice lock, Primal timing, Mega timing, and simultaneous replacements.",
                "Prove Blue Orb and Sharpedonite coexist under the intended one-Mega-plus-one-Primal rule.",
                "Write and font-measure exact dialogue; update the source-derived guide and reservation ledger.",
                "Run cap-70 Wide Guard, weather, Water immunity, special bulk, Ground/Fighting, anti-pivot, Intimidate/burn, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def courtney_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen5randomdoublesbattle:025": (
            "selected-role",
            "The generated Victini set validates Victory Star and mixed legendary pressure. Courtney turns that ability into a visible accuracy-calibration lead rather than copying its unrelated hazard and sleep roster.",
        ),
        "elite:giovanni-cischke:worlds-2025": (
            "adapted-role",
            "The 2025 World Champion roster validates Chi-Yu as elite sun pressure. Courtney imports its special-defense compression but rejects Koraidon, Tera, redirection, sleep, and the complete dual-speed shell.",
        ),
        "showdown:gen9randomdoublesbattle:026": (
            "selected-set",
            "The generated Glimmora set validates fast Rock/Poison pressure and hazards. Courtney makes Toxic Debris and Stealth Rock a finite switching tax before a safe Steelix-plus-Sludge-Wave formation.",
        ),
        "elite:wolfe:indianapolis-2026": (
            "adapted-set",
            "Wolfe Glick's Indianapolis team validates Steelix as top-level doubles board durability. Courtney uses ordinary Air Balloon Steelix so it can visibly share the board with Glimmora's Sludge Wave while leaving Metagross to Steven and Mega Excadrill to Tabitha.",
        ),
        "showdown:gen9championsrandomdoublesbattle:011": (
            "adapted-set",
            "The Champions generator validates Houndoom as a positioned special attacker. Courtney reserves Mega Houndoom as her solar calculation's sole climax and removes unrelated redirection and recovery.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team has excellent pieces but relies on Hypnosis and six loosely connected attacks. Courtney should feel obsessive and exact: calibrated accuracy, visible special-defense compression, safe ally geometry, and one solar Mega calculation.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_NINETALES",
            "level_offset": 1,
            "item": "ITEM_HEAT_ROCK",
            "ability": "ABILITY_DROUGHT",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_WILL_O_WISP", "MOVE_ENCORE", "MOVE_PROTECT"],
            "role": "Visible calibration field: extends ordinary sun and chooses among spread heat, a Victory-Star-assisted burn, Encore, or protection.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_VICTINI",
            "level_offset": 1,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_VICTORY_STAR",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_NAIVE",
            "moves": ["MOVE_V_CREATE", "MOVE_BLUE_FLARE", "MOVE_BOLT_STRIKE", "MOVE_U_TURN"],
            "role": "Courtney's accuracy standard and mixed legendary striker; Victory Star stabilizes the lead while Assault Vest forces four immediate choices.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_CHI_YU",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_BEADS_OF_RUIN",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_FLAMETHROWER", "MOVE_OVERHEAT", "MOVE_DARK_PULSE", "MOVE_FIRE_BLAST"],
            "role": "Fast special-defense compressor whose public Choice lock can amplify Courtney's special board or become an exploitable overcommitment.",
            "lead_group": "special-pressure-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_GLIMMORA",
            "level_offset": 2,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_TOXIC_DEBRIS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_STEALTH_ROCK", "MOVE_SLUDGE_WAVE", "MOVE_POWER_GEM", "MOVE_PROTECT"],
            "role": "Fragile mineral instrument: creates a finite switching tax and a spread Poison line that becomes intentionally ally-safe only beside Steelix.",
            "lead_group": "geometry-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_STEELIX",
            "level_offset": 3,
            "item": "ITEM_AIR_BALLOON",
            "ability": "ABILITY_SHEER_FORCE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_HEAVY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Physical calibration weight and Glimmora's safe partner: immune to Sludge Wave, visibly Ground-immune until Balloon breaks, and free of setup dependency.",
            "lead_group": "geometry-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_HOUNDOOM",
            "level_offset": 4,
            "item": "ITEM_HOUNDOOMINITE",
            "ability": "ABILITY_FLASH_FIRE",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_DARK_PULSE", "MOVE_HEAT_WAVE", "MOVE_SOLAR_BEAM", "MOVE_SLUDGE_BOMB"],
            "role": "Courtney's sole Mega and solved final equation: fast solar special coverage with no Protect, recovery, or setup turn.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MAGMA_HIDEOUT_COURTNEY",
        "planning_tier": "faction_admin_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Courtney's final Magma Hideout battle in the six-Badge chapter",
            "location": "MagmaHideout_4F",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and the full pre-Mossdeep catch, move, ability, leveling, and ordinary held-item toolkit",
                "Mega Bracelet and all campaign Mega Stones earned before the hideout",
                "Manual healing and party rebuilding before the admin chamber",
                "Prior knowledge of ordinary sun, spread moves, Choice items, hazards, and ally immunities",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Courtney uses exactly one Mega Houndoom and no Primal. Maxie's later Primal Groudon remains his reveal.",
            "evolution_phase": "Late campaign admin boss: fully evolved, legendary, Gen 9, and one signature Mega are appropriate.",
            "preparation_access": "The player may heal and rebuild before Courtney; this is not a no-menu party lock.",
            "gauntlet_position": "Magma's final precision exam before Maxie's land-geometry climax. Courtney must calibrate attacks and safe zones without spending his Primal or Gravity/Instruct reveal.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_COURTNEY_MAGMA_HIDEOUT"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "magma_hideout_courtney", "trainer_ids": ["TRAINER_COURTNEY_MAGMA_HIDEOUT"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "meteor_falls_courtney", "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required battle"},
                {"variant_id": "mossdeep_multi_courtney", "trainer_ids": ["TRAINER_COURTNEY_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_CourtneyMagmaHideout",
                "src/data/trainers.h:TRAINER_COURTNEY_MAGMA_HIDEOUT",
                "data/maps/MagmaHideout_4F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Magma Hideout, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MAGMA_HIDEOUT_TABITHA", "MAGMA_HIDEOUT_FINAL_MAXIE", "MT_CHIMNEY_MAXIE", "LAVARIDGE_GYM_FLANNERY"],
            "required_preimplementation_review": "Refresh the last ten hideout battles. Preserve Victory Star calibration, Beads of Ruin commitment, Glimmora-Steelix ally safety, and Mega Houndoom unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Courtney measures the battlefield: Victini calibrates every risky attack, Chi-Yu changes the special equation, Glimmora marks unsafe ground, Steelix occupies the one safe tile, and Mega Houndoom supplies the solved answer.",
            "story_fit": "Courtney's obsessive precision becomes a battle built from tolerances and safe zones rather than Maxie's ideology or Tabitha's machinery.",
            "primary_player_question": "Can the player disrupt Courtney's calibrated lead and recognize when Glimmora's Sludge Wave is safe beside Steelix, while exploiting Chi-Yu's Choice commitment before Mega Houndoom solves the final coverage equation?",
            "primary_mode": "Drought Ninetales and Victory Star Victini establish accurate, mixed sun pressure without Hypnosis, sleep, redirection, or setup.",
            "secondary_mode": "Chi-Yu compresses special defense, Glimmora and Steelix create one readable ally-safe spread formation, and Mega Houndoom closes with immediate solar coverage.",
            "preview_pressure": "Four Fire members make Magma legible, while Glimmora and Steelix visibly advertise mineral safe-zone geometry. No Primal or second Mega steals Maxie's climax.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 61 through 64 against cap 60 with Victory Star accuracy, sun, mixed legendary pressure, special-defense compression, hazards, ally-safe Sludge Wave, physical Steel/Ground coverage, and one fast Mega. Choice lock, frail support, shared Ground/Water pressure, and absent healing keep broad answers.",
            "pressure_sources": [
                "Ninetales sun, spread Heat Wave, Encore, and burn pressure",
                "Assault Vest Victory Star Victini with four calibrated attacks",
                "Choice Scarf Chi-Yu special-defense compression and overcommitment",
                "Focus Sash Glimmora hazards and Sludge Wave",
                "Air Balloon Steelix as the ally-safe physical partner",
                "Mega Houndoom immediate Dark, Fire, Grass, and Poison coverage",
            ],
            "resource_tax": "The battle taxes weather, mixed bulk, Ground and Water preservation, Choice-lock exploitation, hazard tolerance, spread positioning, and enough priority or speed for Mega Houndoom.",
            "tuning_order": [
                "Preserve calibrated accuracy, safe Sludge Wave geometry, and sole Mega Houndoom climax",
                "Validate partner-safe spread scoring and Choice logic before changing sets",
                "Adjust offsets within +1 to +4, beginning with Houndoom, Chi-Yu, and Victini",
                "Then adjust Glimmora or Ninetales survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_NINETALES", "SPECIES_VICTINI"],
            "mandatory_order_reason": "The lead exposes Courtney's calibration thesis. The safe mineral pair is a preferred formation, not an inviolable scripted wave.",
            "reserve_sequence": [
                "Use Chi-Yu when its Choice-locked special compression creates a visible advantage and a suitable move remains safe.",
                "Prefer Glimmora when hazards or Rock/Poison pressure matter; do not force Sludge Wave without an immune or protected partner.",
                "Prefer Steelix beside Glimmora or when its Steel/Ground/Rock coverage is independently correct; publicly account for Air Balloon.",
                "Preserve Mega Houndoom as the solved final equation when practical, but deploy it earlier if its coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Account for Victory Star's real accuracy modifier without treating Will-O-Wisp, Blue Flare, Bolt Strike, or Rock Slide as guaranteed.",
                "Score Ninetales and Victini jointly and use U-turn when the visible reserve genuinely improves the board.",
                "Respect Chi-Yu's public Choice lock and avoid selecting it into a disclosed immunity or forced failure.",
                "Use Sludge Wave only beside Steelix, a protected ally, or a board where ally damage is outweighed by a real knockout; otherwise choose single-target STAB or Protect.",
                "Mega Evolve Houndoom normally and use immediate coverage rather than inventing a safe setup turn.",
            ],
            "forbidden_behaviors": [
                "Do not use Hypnosis, sleep, evasion, or hidden player information.",
                "Do not treat Victory Star as perfect accuracy or ignore Choice lock.",
                "Do not Sludge Wave a vulnerable ally by default.",
                "Do not add a Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A calibrates sun through Ninetales-Victini. State B selects Chi-Yu for special compression. State C enables Glimmora-Steelix only when safe-zone geometry is real. State D exposes Mega Houndoom as the final equation. Every state permits independent attacks and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Replace sun, pressure Ninetales, use Ground/Water/Rock, Intimidate, Snarl, or mixed bulk against the calibrated opening.",
                "Exploit Victini's V-create drops and Chi-Yu's Choice lock through Protect, immunity, resist pivots, or forced target changes.",
                "Remove Glimmora before hazards accumulate, avoid weak physical contact that triggers Toxic Debris, and punish Sludge Wave boards lacking Steelix.",
                "Break Steelix's visible Balloon before Ground pressure or exploit Water, Fire, Fighting, Ground, and special attacks.",
                "Preserve priority, speed control, bulky Water/Ground, Fighting, or special defense for Mega Houndoom's no-Protect finale.",
            ],
            "intentional_weakness": "Ninetales and Glimmora are frail; Victini drops defenses; Chi-Yu is Choice-locked; Steelix's Ground immunity is a visible one-use Balloon and its special bulk is exploitable; Mega Houndoom has no Protect or recovery. Four members retain real Ground, Water, or Rock pressure and the team has no redirection or healing loop.",
            "first_loss_lesson": "Courtney's danger comes from precision, not randomness. Break the calibrated lead, identify the Choice move, never donate a safe Glimmora-Steelix spread turn, pop the Balloon deliberately, and keep a fast answer for Houndoom.",
            "revealed_information": [
                "Sun, Victory Star, Choice lock, hazards, Toxic Debris, Air Balloon, spread targeting, and Mega evolution are public state.",
                "Victory Star uses ordinary engine accuracy and does not guarantee attacks.",
                "Sludge Wave ally safety comes from normal Steel immunity or Protect, not a custom exemption.",
                "Mega Houndoom is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "AI treats calibrated moves as perfectly accurate",
                "Chi-Yu violates its Choice lock",
                "Glimmora repeatedly damages vulnerable partners",
                "Mega Houndoom receives Primal-level weather or another gimmick",
                "Courtney becomes generic sun or copies Maxie's land geometry",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Victini Victory Star doubles", "Chi-Yu world champion sun", "Glimmora doubles", "Wolfe Steelix doubles", "Mega Houndoom doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Koraidon, Tera, dual speed, redirection, sleep, and the complete 2025 champion shell are not imported.",
                "Hypnosis and low-accuracy win conditions are removed from the current Courtney team.",
                "Maxie's Primal, Gravity, Instruct, and Flower Gift remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Generated Victini and Victory Star role legitimacy",
                "World-champion Chi-Yu special compression",
                "Generated Glimmora plus Wolfe-validated Steelix doubles roles",
                "Champions-generator Houndoom adapted into Courtney's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Courtney's definitive Victory Star calibration lead",
                "The campaign's Glimmora-Steelix safe Sludge Wave formation",
                "Choice Scarf Chi-Yu as an exploitable special equation",
                "Mega Houndoom as Courtney's final calculated answer",
            ],
            "preserves": [
                "Maxie's Primal Groudon, Gravity/Instruct, Flower Gift, and Mega Camerupt",
                "Tabitha's future machinery or self-activation identity",
                "Flannery's move-order thermal lesson",
                "Shelly's snow phase-change and Archie's rain momentum",
            ],
            "releases": [
                "Krookodile, Landorus, Chandelure, and ordinary Houndoom variants leave final Courtney for other encounters",
                "Other sun and Fire species remain available if they do not recreate calibration plus safe-zone geometry",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, final-leader, or paired Shelly anchors.",
                "Ninetales provides ordinary sun only; Courtney's defining question is accuracy and ally-safe geometry, not Maxie's land control.",
                "Mega Houndoom is reserved here; non-Mega Houndoom may appear elsewhere only if its lesson is materially different.",
            ],
        },
        "presentation": {
            "intro_concept": "Courtney says emotion is noise; every angle, tolerance, and safe tile in this chamber has already been calculated.",
            "defeat_concept": "She records that the player's uncertainty was itself the variable her model failed to contain.",
            "post_battle_concept": "Native story progression remains unchanged. Her loss clears the way to Maxie's less controlled ideological climax.",
            "hint_concept": "Nearby Magma dialogue warns that Victini corrects accuracy, Chi-Yu forces a commitment, and only steel can stand safely beside the poison crystal's wave.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, Ninetales-Victini calibration, Choice Scarf Chi-Yu, Glimmora hazards and ally-safe Sludge Wave with Air Balloon Steelix, Mega Houndoom finale, AI accuracy/lock/ally checks, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Victory Star and Glimmora-Steelix safe-zone geometry make Courtney feel exact and personal rather than a smaller Maxie.",
            "weakest_link": "Four Fire attackers can still read as generic sun. The actual play must emphasize accuracy calibration, Choice commitment, hazards, and ally-safe spread geometry; if runtime AI cannot execute those, the roster must be revised before source closure.",
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
                "The source guide places final Courtney at strict cap 60 with a six-Pokemon authored roster.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Houndoomite maps Houndoom to Mega Houndoom and no other transformation item appears.",
                "All five selected references exist in the current corpus and include generated full sets plus a World Champion team.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_CourtneyMagmaHideout with the exact six sets and offsets.",
                "Add partner, field, and combo flags; implement calibrated lead and safe-zone reserve scoring.",
                "Regression-test Victory Star accuracy, Choice lock, U-turn, Toxic Debris, hazards, Sludge Wave ally safety, Air Balloon, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-60 weather, Ground/Water/Rock, mixed bulk, Protect, hazards, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def shelly_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen9championsrandomdoublesbattle:024": (
            "selected-lead",
            "The Champions generator validates Alolan Ninetales as a snow-screen doubles lead. Shelly keeps Aurora Veil and Freeze-Dry while rejecting unrelated Fake Out, recovery, and setup pieces.",
        ),
        "showdown:gen8randomdoublesbattle:002": (
            "selected-set",
            "The generated Arctozolt roster validates it as a real doubles attacker. Shelly gives it Slush Rush, Bolt Beak, and four immediate attacks rather than importing Tailwind or hazards.",
        ),
        "showdown:gen6randomdoublesbattle:002": (
            "adapted-set",
            "The generated Rotom-Frost set validates Choice disruption and pivoting. Shelly uses the local appliance Freeze-Dry mapping to make the form part of her phase-change identity.",
        ),
        "showdown:gen8randomdoublesbattle:003": (
            "adapted-role",
            "The generated Primarina roster validates bulky special doubles pressure. Shelly converts Liquid Voice Hyper Voice into a finite Throat Spray wave under snow screens.",
        ),
        "showdown:gen7randomdoublesbattle:017": (
            "adapted-set",
            "The generated Lapras set validates Freeze-Dry, Hydro Pump, and broad doubles coverage. Shelly upgrades it to Mega Lapras so Ice Body turns her snow into a visible final sustain layer.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current snow idea is promising but mixes unrelated Clefable, Ludicolo, and Beedrill lines. Shelly should freeze Aqua's usual momentum into one coherent snow-to-water phase boundary with screens, Slush Rush, Freeze-Dry, pivots, sound pressure, and Mega Lapras.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_NINETALES_ALOLAN",
            "level_offset": 1,
            "item": "ITEM_LIGHT_CLAY",
            "ability": "ABILITY_SNOW_WARNING",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_AURORA_VEIL", "MOVE_BLIZZARD", "MOVE_FREEZE_DRY", "MOVE_PROTECT"],
            "role": "Visible freezing point: establishes snow and a contestable screen, then attacks instead of maintaining a passive support loop.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_SANDSLASH_ALOLAN",
            "level_offset": 1,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_SLUSH_RUSH",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ICICLE_CRASH", "MOVE_IRON_HEAD", "MOVE_DRILL_RUN", "MOVE_PROTECT"],
            "role": "Immediate physical Slush Rush blade that punishes Fairy, Rock, and Steel counterplans while retaining one readable Protect turn.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ARCTOZOLT",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_SLUSH_RUSH",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_BOLT_BEAK", "MOVE_ICICLE_CRASH", "MOVE_STOMPING_TANTRUM", "MOVE_ROCK_SLIDE"],
            "role": "Fossil hydroelectric breaker: snow order powers Bolt Beak while four attacks and Assault Vest prevent a setup or recovery loop.",
            "lead_group": "snow-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_ROTOM_FROST",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_FREEZE_DRY", "MOVE_THUNDERBOLT", "MOVE_VOLT_SWITCH", "MOVE_TRICK"],
            "role": "Phase-change pivot whose appliance Freeze-Dry punishes Water answers and whose public Scarf can be spent through Trick.",
            "lead_group": "pivot-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_PRIMARINA",
            "level_offset": 3,
            "item": "ITEM_THROAT_SPRAY",
            "ability": "ABILITY_LIQUID_VOICE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HYPER_VOICE", "MOVE_MOONBLAST", "MOVE_ICE_BEAM", "MOVE_PROTECT"],
            "role": "The thawing wave: Liquid Voice turns sound into spread Water and consumes one finite Throat Spray before direct Fairy and Ice coverage.",
            "lead_group": "phase-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_LAPRAS",
            "level_offset": 4,
            "item": "ITEM_LAPRASITE",
            "ability": "ABILITY_WATER_ABSORB",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_FREEZE_DRY", "MOVE_HYDRO_PUMP", "MOVE_THUNDER", "MOVE_PROTECT"],
            "role": "Shelly's sole Mega and final phase boundary: Water/Ice coverage, Water Absorb before transformation, and visible Ice Body sustain only while snow remains.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "SEAFLOOR_CAVERN_SHELLY",
        "planning_tier": "faction_admin_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Shelly's final Seafloor Cavern battle immediately before Archie in the seven-Badge chapter",
            "location": "SeafloorCavern_Room3",
            "strict_cap": 70,
            "player_tools": [
                "Seven Badges and the complete pre-crisis catch, move, ability, leveling, and ordinary held-item toolkit",
                "Mega Bracelet and all campaign Mega Stones earned before the cavern",
                "Manual healing and party rebuilding before and after Shelly",
                "Prior knowledge of weather, screens, speed abilities, Choice items, pivots, and spread damage",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Shelly uses exactly one Mega Lapras and no Primal. Archie's later Primal Kyogre remains his reveal.",
            "evolution_phase": "Late campaign admin boss: fully evolved, fossil, appliance, and one signature Mega threat are appropriate.",
            "preparation_access": "The player may heal and rebuild after Shelly before Archie; their battles are connected fiction, not a no-menu party lock.",
            "gauntlet_position": "Aqua's final weather-science exam before Archie's flood. Shelly freezes momentum into screens and phase changes without spending his Primal or rain-current sequence.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_SHELLY_SEAFLOOR_CAVERN"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "seafloor_cavern_shelly", "trainer_ids": ["TRAINER_SHELLY_SEAFLOOR_CAVERN"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "weather_institute_shelly", "trainer_ids": ["TRAINER_SHELLY_WEATHER_INSTITUTE"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_ShellySeafloorCavern",
                "src/data/trainers.h:TRAINER_SHELLY_SEAFLOOR_CAVERN",
                "data/maps/SeafloorCavern_Room3/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Seafloor Cavern, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["SEAFLOOR_CAVERN_FINAL_ARCHIE", "AQUA_HIDEOUT_MATT", "SOOTOPOLIS_GYM_JUAN", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Refresh the last ten cavern battles. Preserve snow plus Aurora Veil, two distinct Slush Rush attackers, Rotom-Frost's appliance pivot, Liquid Voice thaw, and Mega Lapras unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Shelly freezes the current: Ninetales raises the screen, steel and fossil blades accelerate over ice, Rotom moves the charge, Primarina turns sound into thawing water, and Mega Lapras becomes the boundary between both states.",
            "story_fit": "As Aqua's weather-minded admin, Shelly controls water by changing its phase. Her battle is not weaker rain; it is the decision to freeze momentum, shelter it, and release it again as sound and surf.",
            "primary_player_question": "Can the player deny snow or Aurora Veil before Shelly's two Slush Rush attackers exploit it, then navigate Freeze-Dry and Choice pivots while preserving weather control for Ice Body Mega Lapras?",
            "primary_mode": "Alolan Ninetales and Alolan Sandslash expose snow, Aurora Veil, and physical Slush Rush immediately with no evasion or sleep.",
            "secondary_mode": "Arctozolt adds hydroelectric Bolt Beak, Rotom-Frost pivots and can spend its Scarf, Primarina thaws into spread Liquid Voice, and Mega Lapras sustains only while the player permits snow.",
            "preview_pressure": "Ice and Water dominate the preview, but Electric coverage and Freeze-Dry punish ordinary anti-Water autopilot. No rain setter appears, preserving Archie's reveal.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 71 through 74 against cap 70 with Light Clay Aurora Veil, two Slush Rush physical attackers, Bolt Beak order pressure, Choice disruption, Freeze-Dry, one finite sound boost, and Ice Body Mega Lapras. Weather replacement, screen removal, Rock/Steel/Fighting/Fire, special pressure, and Choice exploitation remain broad answers.",
            "pressure_sources": [
                "Alolan Ninetales snow, Aurora Veil, Blizzard, and Freeze-Dry",
                "Life Orb Alolan Sandslash physical Slush Rush coverage",
                "Assault Vest Arctozolt Bolt Beak and mixed physical coverage",
                "Choice Scarf Rotom-Frost Freeze-Dry pivot and Trick",
                "Liquid Voice Primarina with one Throat Spray activation",
                "Mega Lapras Water/Ice/Electric coverage and snow-dependent Ice Body",
            ],
            "resource_tax": "The battle taxes weather replacement, screen removal, physical and special Ice answers, Ground positioning against Electric coverage, Choice scouting, and enough offense to stop snow-dependent Lapras sustain.",
            "tuning_order": [
                "Preserve phase-change identity, contestable screen, Slush Rush pair, appliance pivot, and Mega Lapras",
                "Validate snow, Aurora Veil, Bolt Beak order, Rotom form move, Liquid Voice, and Ice Body before changing sets",
                "Adjust offsets within +1 to +4, beginning with Lapras, Primarina, and Arctozolt",
                "Then adjust Ninetales or Sandslash survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_NINETALES_ALOLAN", "SPECIES_SANDSLASH_ALOLAN"],
            "mandatory_order_reason": "The lead makes Shelly's phase and screen public. Later snow, pivot, thaw, and Mega roles are selected by board state rather than fixed pairs.",
            "reserve_sequence": [
                "Use Arctozolt while snow makes Bolt Beak order pressure real; otherwise value its independent coverage and Assault Vest bulk honestly.",
                "Use Rotom-Frost when Freeze-Dry, Electric pressure, Volt Switch, or a valuable Scarf Trick improves the visible board.",
                "Use Primarina when spread Liquid Voice and Fairy coverage create the best thawing line; account for one finite Throat Spray.",
                "Preserve Mega Lapras as the final phase boundary when practical, but deploy it earlier if Water Absorb or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Ninetales and Sandslash jointly: Aurora Veil requires active snow and meaningful remaining duration; Ninetales attacks when the screen is active or denial is likely.",
                "Evaluate Slush Rush and Bolt Beak from actual move order rather than assuming snow guarantees maximum power.",
                "Treat Rotom-Frost's Freeze-Dry as the local appliance form move, respect Choice lock, and use Trick or Volt Switch only when the visible board improves.",
                "Recognize Liquid Voice spread targeting and one Throat Spray activation without farming support turns.",
                "Mega Evolve Lapras normally and value Ice Body only while snow remains; do not Protect-loop for passive healing.",
            ],
            "forbidden_behaviors": [
                "Do not use snow evasion, sleep, hidden information, or guaranteed Icicle Crash or Hydro Pump accuracy.",
                "Do not cast Aurora Veil outside snow or when already active without a real renewal need.",
                "Do not violate Rotom's Choice lock or fake its appliance move legality.",
                "Do not add rain, Primal Kyogre, a second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A freezes the board with Ninetales-Sandslash. State B selects Arctozolt for snow order pressure. State C pivots through Rotom-Frost. State D thaws into Primarina's Liquid Voice. State E exposes Mega Lapras as the final boundary. Each state retains independent attacks and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Replace snow, deny Ninetales, remove Aurora Veil, use Brick Break or screen-aware damage, and exploit the lead's shared Fire/Rock/Steel/Fighting pressure.",
                "Use priority, Trick Room, paralysis, faster weather, physical Intimidate or burn, and Protect to deny Slush Rush and Bolt Beak order value.",
                "Exploit Rotom's public Choice lock, Ground immunity positioning, or Trick commitment; use special bulk against Freeze-Dry and Electric coverage.",
                "Use Wide Guard or sound resistance where applicable, special walls, Poison/Steel, or immediate pressure before Primarina's finite boost snowballs.",
                "End snow, use Fighting/Rock/Steel/Electric/Grass, Taunt, or concentrated damage so Mega Lapras cannot convert Ice Body and Protect into a loop.",
            ],
            "intentional_weakness": "Ninetales is frail and the only snow setter; Sandslash and Arctozolt depend on weather for order; Rotom is Choice-locked; Primarina has one finite boost; Mega Lapras is slow and heals only in snow. There is no redirection, sleep, recovery move, rain, Primal, or second Mega.",
            "first_loss_lesson": "Shelly's screen and weather are the engine. Deny one before attacking into both, break Bolt Beak's move order, exploit Rotom's lock, and end snow before trying to grind through Mega Lapras.",
            "revealed_information": [
                "Snow, Aurora Veil duration, Slush Rush order, Choice lock, Rotom form, Throat Spray consumption, Ice Body healing, and Mega evolution are public state.",
                "Freeze-Dry on Rotom-Frost is the repository's explicit appliance form move exception.",
                "Bolt Beak uses ordinary move-order power and no custom guarantee.",
                "Mega Lapras is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "AI wastes turns recasting an active screen",
                "Bolt Beak receives doubled power without moving first",
                "Rotom violates Choice or form-move rules",
                "Mega Lapras Protect-loops indefinitely",
                "Shelly becomes generic rain or duplicates Archie's current",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Alolan Ninetales snow screens doubles", "Arctozolt doubles", "Rotom Frost doubles", "Primarina doubles", "Lapras doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Snow evasion, sleep, generic Tailwind, redirection, passive healing loops, and unrelated setup are not imported.",
                "Archie's rain, Primal Kyogre, priority shield, Palafin cycle, and Protect punishment remain protected.",
                "The current Clefable, Ludicolo, and Mega Beedrill lines are released from Shelly's final team.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Champions-generator Alolan Ninetales screen lead",
                "Generated Arctozolt Slush Rush legitimacy",
                "Generated Rotom-Frost Choice pivoting adapted to local Freeze-Dry",
                "Generated Primarina special pressure",
                "Generated Lapras coverage adapted into Shelly's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Shelly's definitive Aurora Veil snow opening",
                "The campaign's paired Slush Rush blade and Bolt Beak order test",
                "Rotom-Frost's appliance Freeze-Dry Choice pivot",
                "Liquid Voice Primarina as the thawing phase",
                "Mega Lapras as Shelly's final snow-water boundary",
            ],
            "preserves": [
                "Archie's Primal rain momentum and Palafin-Urshifu-Sharpedo current",
                "Glacia's detonation and trap attrition rather than weather screens",
                "Juan's Surf relay and Wallace's dual-speed rain",
                "Matt's future Aqua identity and ordinary snow teams elsewhere",
            ],
            "releases": [
                "Empoleon, Crawdaunt, Clefable, Ludicolo, and Mega Beedrill leave final Shelly",
                "Other Ice and Water species remain available if they do not recreate the full phase-change chain",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, final-leader, or paired Courtney anchors.",
                "Shelly uses snow and no rain; her question is phase and screen denial, not Archie's momentum.",
                "Mega Lapras is reserved here and Glacia retains her entirely different Mega Glalie detonation lesson.",
            ],
        },
        "presentation": {
            "intro_concept": "Shelly asks whether the player understands that water is most dangerous when they assume it must stay liquid.",
            "defeat_concept": "She admits the player controlled the phase boundary instead of merely enduring the cold.",
            "post_battle_concept": "Native progression toward Archie remains unchanged; Shelly's defeat releases the frozen pause before his flood.",
            "hint_concept": "Nearby Aqua dialogue warns to break the fox's screen, never let the fossil move first, watch the appliance's locked move, and end snow before Lapras settles in.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 70, Alolan Ninetales-Sandslash snow screen lead, Arctozolt Bolt Beak, Rotom-Frost Freeze-Dry pivot, Liquid Voice Primarina, Mega Lapras Ice Body finale, weather/screen/order AI, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Snow, screens, Slush Rush, Freeze-Dry, Liquid Voice, and Ice Body turn water's phase change into one coherent Aqua-admin puzzle without stealing Archie's rain.",
            "weakest_link": "Two Slush Rush attackers can feel repetitive if their roles collapse into fast physical damage. Sandslash must remain the precise Steel blade, Arctozolt the Bolt Beak order test, and the reserve selector must not deploy them as interchangeable modules.",
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
                "The source guide places final Shelly at strict cap 70 in a required six-Pokemon double.",
                "Every proposed ordinary move, item, spread, species, and ability slot exists; Rotom-Frost Freeze-Dry is the repository's explicit form-move exception.",
                "Laprasite maps Lapras to Mega Lapras and no other transformation item appears.",
                "All five selected references exist in the current corpus and are full-set generated or Champions evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_ShellySeafloorCavern with the exact six sets and offsets.",
                "Add partner, speed, field, and combo flags; implement phase-aware reserve scoring.",
                "Regression-test snow, Aurora Veil duration, Slush Rush, Bolt Beak order, Rotom form move and Choice lock, Liquid Voice, Throat Spray, Ice Body, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-70 weather replacement, screen removal, Rock/Steel/Fighting/Fire, special pressure, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def tabitha_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "elite:wolfe:players-cup-ii-2020": (
            "selected-core",
            "Wolfe Glick's Players Cup II core is the exact competitive proof for allied Surf activating Coalossal's Steam Engine and Weakness Policy. Tabitha substitutes fast Sniper Inteleon because Dragapult is reserved for Drake, and imports no Dynamax, residual damage, redirection, or complete balance shell.",
        ),
        "showdown:gen8randomdoublesbattle:005": (
            "adapted-set",
            "The generated Coalossal set validates its doubles offense outside Dynamax. Tabitha uses Heat Wave, Power Gem, Body Press, and Protect after one finite self-activation.",
        ),
        "showdown:gen7randomdoublesbattle:020": (
            "selected-role",
            "The generated Stakataka roster validates it as immediate heavy doubles machinery. Tabitha uses Wide Guard and three attacks without importing Tailwind or setup.",
        ),
        "showdown:gen8randomdoublesbattle:011": (
            "selected-set",
            "The generated Darmanitan set validates direct physical doubles pressure. Tabitha uses Life Orb Sheer Force and four attacks as the non-combo fallback.",
        ),
        "showdown:gen9championsrandomdoublesbattle:019": (
            "adapted-set",
            "The Champions generator validates Excadrill as a four-attack doubles threat. Tabitha reserves its custom Mega as the final machine and removes the source team's unrelated Trick Room shell.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "The current Coalossal-Dragapult-Gastrodon shell is the right foundation. Dragapult moves to Drake, fast Inteleon becomes the coolant, Gigalith's odd Solar Power line is removed, and Stakataka makes the machinery identity explicit.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_INTELEON",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_SNIPER",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_SURF", "MOVE_SNIPE_SHOT", "MOVE_ICE_BEAM", "MOVE_PROTECT"],
            "role": "Fast coolant and ignition switch: Surf may activate Coalossal or safely feed Gastrodon, while Snipe Shot, Ice Beam, and Protect remain real independent choices.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_COALOSSAL",
            "level_offset": 1,
            "item": "ITEM_WEAKNESS_POLICY",
            "ability": "ABILITY_STEAM_ENGINE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_BODY_PRESS", "MOVE_PROTECT"],
            "role": "Tabitha's primary engine: one weak allied Surf can trigger speed and offense, after which it must attack rather than farm activation turns.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_GASTRODON",
            "level_offset": 2,
            "item": "ITEM_LEFTOVERS",
            "ability": "ABILITY_STORM_DRAIN",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_EARTH_POWER", "MOVE_ICE_BEAM", "MOVE_CLEAR_SMOG", "MOVE_PROTECT"],
            "role": "Hydraulic safety valve: turns Surf or hostile Water into a special boost, clears opposing setup, and prevents the machinery core from being solved by Water alone.",
            "lead_group": "hydraulic-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_STAKATAKA",
            "level_offset": 2,
            "item": "ITEM_MENTAL_HERB",
            "ability": "ABILITY_BEAST_BOOST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_WIDE_GUARD", "MOVE_ROCK_SLIDE", "MOVE_GYRO_BALL", "MOVE_BODY_PRESS"],
            "role": "Ultra-heavy chassis that shields the assembly from spread retaliation and supplies slow physical pressure without requiring Trick Room.",
            "lead_group": "chassis-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_DARMANITAN",
            "level_offset": 3,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_SHEER_FORCE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FLARE_BLITZ", "MOVE_ZEN_HEADBUTT", "MOVE_ROCK_SLIDE", "MOVE_SUPERPOWER"],
            "role": "Uncomplicated piston: four immediate physical attacks ensure Tabitha remains dangerous after the engineered combo is broken.",
            "lead_group": "fallback-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_EXCADRILL",
            "level_offset": 4,
            "item": "ITEM_EXCADRITE",
            "ability": "ABILITY_MOLD_BREAKER",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_IRON_HEAD", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Tabitha's sole Mega and final drill head: immediate Ground/Steel/Rock pressure with no sand, setup, or second activation dependency.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MAGMA_HIDEOUT_TABITHA",
        "planning_tier": "faction_admin_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Tabitha's final Magma Hideout battle in the six-Badge chapter",
            "location": "MagmaHideout_4F",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and the complete pre-Mossdeep catch, move, ability, leveling, and ordinary held-item toolkit",
                "Mega Bracelet and all campaign Mega Stones earned before the hideout",
                "Manual healing and party rebuilding before the admin chamber",
                "Prior exposure to spread moves, ally abilities, Weakness Policy, and speed changes",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Tabitha uses exactly one Mega Excadrill and no Primal. Maxie's Primal Groudon remains protected.",
            "evolution_phase": "Late campaign admin boss: fully evolved, Ultra Beast, and one signature Mega are appropriate.",
            "preparation_access": "The player may heal and rebuild before Tabitha; this is not a no-menu party lock.",
            "gauntlet_position": "Magma's machinery and self-activation exam. It must contrast Courtney's calibration and Maxie's land ideology while foreshadowing the faction's engineered ambition.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_TABITHA_MAGMA_HIDEOUT"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "magma_hideout_tabitha", "trainer_ids": ["TRAINER_TABITHA_MAGMA_HIDEOUT"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "mt_chimney_tabitha", "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required battle"},
                {"variant_id": "mossdeep_multi_tabitha", "trainer_ids": ["TRAINER_TABITHA_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_TabithaMagmaHideout",
                "src/data/trainers.h:TRAINER_TABITHA_MAGMA_HIDEOUT",
                "data/maps/MagmaHideout_4F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Magma Hideout, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MAGMA_HIDEOUT_COURTNEY", "MAGMA_HIDEOUT_FINAL_MAXIE", "MT_CHIMNEY_TABITHA", "MOSSDEEP_SPACE_CENTER_MULTI_CLIMAX"],
            "required_preimplementation_review": "Refresh the last ten hideout battles. Preserve the single Inteleon-Coalossal activation, Gastrodon hydraulic safety, Stakataka chassis, and Mega Excadrill unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Tabitha starts the machine: Inteleon supplies coolant, Coalossal redlines, Gastrodon catches overflow, Stakataka braces the frame, Darmanitan is the piston, and Mega Excadrill is the drill head.",
            "story_fit": "Tabitha becomes Team Magma's engineer. His battle demonstrates the apparatus needed to reshape land, not Courtney's precision or Maxie's belief.",
            "primary_player_question": "Can the player interrupt or survive one legal Inteleon-to-Coalossal self-activation, then distinguish Gastrodon's hydraulic defense from Stakataka's spread shield before Mega Excadrill drills through the remaining answers?",
            "primary_mode": "Inteleon Surf may trigger Coalossal's Steam Engine and Weakness Policy once; both members retain independent attacks and the combo is targetable from preview.",
            "secondary_mode": "Gastrodon catches Water, Stakataka shields spread, Darmanitan supplies raw fallback force, and Mega Excadrill closes without needing the original engine.",
            "preview_pressure": "The machinery is public: Surf, Coalossal, Storm Drain, Wide Guard, and Mega Excadrill advertise roles rather than hiding a scripted activation.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 61 through 64 against cap 60 with one tournament-proven self-activation, speed and offensive boosts, Water redirection, spread defense, mixed physical and special pressure, and one Mega. The combo is finite, exposed, and supported by no redirection, sleep, healing loop, or second speed mode.",
            "pressure_sources": [
                "Focus Sash Sniper Inteleon providing fast Surf activation or independent special coverage",
                "Weakness Policy Steam Engine Coalossal with mixed coverage",
                "Storm Drain Gastrodon as Water safety and Clear Smog control",
                "Mental Herb Stakataka Wide Guard and heavy physical attacks",
                "Life Orb Sheer Force Darmanitan fallback offense",
                "Mega Excadrill immediate Ground/Steel/Rock finale",
            ],
            "resource_tax": "The battle taxes activation denial, Haze or Clear Smog, priority, Wide Guard counterplay, Water targeting discipline, Fighting/Ground answers, and enough physical bulk for Darmanitan and Mega Excadrill.",
            "tuning_order": [
                "Preserve one finite self-activation, hydraulic safety, chassis shield, and Mega drill finale",
                "Validate Surf ally damage, Steam Engine, Weakness Policy, Storm Drain, and AI combo scoring before changing sets",
                "Adjust offsets within +1 to +4, beginning with Excadrill, Coalossal, and Darmanitan",
                "Then adjust Inteleon or Gastrodon survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_INTELEON", "SPECIES_COALOSSAL"],
            "mandatory_order_reason": "The opening makes Tabitha's machine visible. Later hydraulic, chassis, piston, and drill roles are selected by board state rather than fixed pairs.",
            "reserve_sequence": [
                "Use Gastrodon when hostile or allied Water pressure creates real Storm Drain value, not as a mandatory second combo.",
                "Use Stakataka when Wide Guard or heavy Steel/Rock/Body Press coverage is correct; do not pretend it has Trick Room.",
                "Use Darmanitan when immediate physical coverage is more valuable than preserving machinery roles.",
                "Preserve Mega Excadrill as the drill-head finale when practical, but deploy it earlier if its coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Use Inteleon Surf only when Coalossal is unactivated and survives the allied hit with meaningful same-turn or next-turn value, or when Gastrodon gains meaningful Storm Drain value.",
                "Stop self-activating after Steam Engine or Weakness Policy has fired and prefer direct attacks, burn, or Protect when Surf becomes wasteful.",
                "Account for Storm Drain and allied spread targeting without reading hidden player choices.",
                "Use Wide Guard only against disclosed spread pressure and let Stakataka attack otherwise.",
                "Mega Evolve Excadrill normally and use board-correct direct pressure rather than waiting for sand or setup that does not exist.",
            ],
            "forbidden_behaviors": [
                "Do not Surf a low-HP Coalossal to death or repeat a spent activation.",
                "Do not make Weakness Policy or Steam Engine trigger without actual damage and engine rules.",
                "Do not spam Wide Guard without visible spread value.",
                "Do not import Dynamax, Gigantamax residual damage, sand dependence, a Primal, second Mega, Tera, or Z-Move.",
            ],
            "state_machine": "State A attempts one legal Inteleon-Coalossal ignition. State B selects Gastrodon for hydraulic safety. State C uses Stakataka as the spread shield. State D releases Darmanitan as the piston. State E exposes Mega Excadrill as the drill head. Every state has independent-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Fake Out, priority, Taunt, double-targeting, Wide Guard, Haze, Clear Smog, Unaware, or immediate Coalossal damage can deny or contain the activation.",
                "Use Water only after accounting for Gastrodon, or target Gastrodon first with Grass, special pressure, Taunt, item disruption, and concentrated attacks.",
                "Punish Stakataka with Fighting/Ground/Water special pressure and avoid telegraphing every spread turn into Wide Guard.",
                "Use Intimidate, burn, Rocky Helmet, physical bulk, speed control, or priority against Darmanitan and Mega Excadrill.",
                "Exploit Coalossal's common Water/Ground weaknesses once Storm Drain and speed boosts are denied or removed.",
            ],
            "intentional_weakness": "Inteleon and Coalossal expose the combo from preview; Inteleon is frail and Coalossal's activation is finite; Gastrodon is the only Water safety; Stakataka lacks Protect; Darmanitan and Excadrill lack recovery. The team has no redirection, sleep, healing loop, or alternate weather/speed engine.",
            "first_loss_lesson": "Treat Surf as an ignition wire. Cut it, clear the boosts, or survive it once—then stop feeding Gastrodon, vary spread and single-target pressure around Stakataka, and keep physical control for the drill head.",
            "revealed_information": [
                "Surf targeting, current HP, Steam Engine, Weakness Policy consumption, Storm Drain, Wide Guard, Beast Boost, and Mega evolution are public state.",
                "The self-activation uses ordinary ally damage and can fail if the target faints or the item has been consumed.",
                "There is no Dynamax-era residual effect or hidden speed grant.",
                "Mega Excadrill is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "AI repeats a spent or lethal self-activation",
                "Storm Drain or Weakness Policy triggers without legal move resolution",
                "Stakataka Wide Guard loops blindly",
                "Mega Excadrill waits for nonexistent sand",
                "Tabitha becomes a generic sun team or copies Courtney and Maxie",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Wolfe Coalossal allied Surf self activation", "Coalossal doubles", "Stakataka doubles", "Darmanitan doubles", "Mega Excadrill Champions doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Dynamax, Gigantamax residual damage, redirection, generic Incineroar-Rillaboom balance, and a second activation partner are not imported.",
                "Trick Room and sand are removed so the machine owns one clear ignition rather than three engines.",
                "Courtney's calibration and Maxie's Primal land geometry remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Wolfe's tournament-proven allied Surf plus Steam Engine and Weakness Policy core, with Inteleon replacing reserved Dragapult",
                "Generated Coalossal offense outside Dynamax",
                "Generated Stakataka chassis and Darmanitan fallback roles",
                "Champions-generator Excadrill adapted into Tabitha's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Tabitha's definitive Inteleon-Coalossal self-activation",
                "Gastrodon as the machine's hydraulic safety valve",
                "Stakataka as the no-Trick-Room heavy chassis",
                "Mega Excadrill as Tabitha's final drill head",
            ],
            "preserves": [
                "Courtney's Victory Star and mineral safe-zone calibration",
                "Maxie's Flower Gift, Gravity/Instruct, Primal Groudon, and Mega Camerupt",
                "Wolfe's complete Players Cup balance shell for no verbatim copy",
                "Matt's rain boarding party and Archie's flood momentum",
            ],
            "releases": [
                "Gigalith leaves final Tabitha and remains available for sand, rock, or route teams",
                "Other self-activation mechanics remain available only if they do not repeat Surf-Coalossal",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, leader, Courtney, Shelly, or paired Matt anchors.",
                "Excadrill was removed from Courtney specifically so Mega Excadrill belongs to Tabitha without duplication.",
                "The only shared faction resource is heat; Tabitha's actual puzzle is machinery and self-activation.",
            ],
        },
        "presentation": {
            "intro_concept": "Tabitha boasts that unlike ideals and calculations, a properly assembled machine produces the same power every time.",
            "defeat_concept": "He concedes that the player found the ignition wire and dismantled the system under load.",
            "post_battle_concept": "Native progression remains unchanged; his failed machine clears the path toward Maxie's unstable larger plan.",
            "hint_concept": "Nearby Magma dialogue warns that the dragon cools the furnace on purpose, the slug catches overflow, the tower blocks spread attacks, and the final drill needs no sand.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, one Inteleon-Coalossal Surf activation, Gastrodon Storm Drain, Stakataka Wide Guard, Darmanitan fallback, Mega Excadrill finale, activation-aware AI, broad counterplay, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "The famous self-activation core is not merely copied; every reserve is another visible component of one Magma machine, and the combo remains finite and targetable.",
            "weakest_link": "Coalossal activation can dominate the entire fight if the AI forces it regardless of board state. The single-use predicates and direct-attack fallbacks are mandatory, not polish.",
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
                "The source guide places final Tabitha at strict cap 60 with a six-Pokemon authored roster.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Excadrite maps Excadrill to Mega Excadrill and no other transformation item appears.",
                "All five selected references exist, led by Wolfe's documented Players Cup II activation core.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_TabithaMagmaHideout with the exact six sets and offsets.",
                "Add partner, HP, speed, field, and combo flags and implement the one-activation state machine.",
                "Regression-test allied Surf damage, Steam Engine, Weakness Policy, Storm Drain, Clear Smog, Wide Guard, Beast Boost, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-60 activation denial, Water/Grass, Fighting/Ground, Haze, priority, spread/single mix, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def matt_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "vgc:regional-baltimore-2025": (
            "selected-history",
            "The 2024 Baltimore-winning rain roster validates Pelipper as a serious tournament rain setter. Matt imports only rain and positioning pressure, not the complete Archaludon-Maushold balance shell.",
        ),
        "showdown:gen8randomdoublesbattle:029": (
            "selected-set",
            "The generated Heliolisk set validates Dry Skin offense and pivoting. Matt converts it to rain-perfect Thunder and Weather Ball with Protect.",
        ),
        "showdown:gen9championsrandomdoublesbattle:016": (
            "adapted-set",
            "The Champions generator validates Toxicroak as Fake Out and setup pressure. Matt removes setup and Choice assumptions for immediate Dry Skin boarding combat.",
        ),
        "showdown:gen5randomdoublesbattle:018": (
            "adapted-set",
            "The generated Seismitoad roster validates Swift Swim physical pressure. Matt gives it four direct attacks and no spread Earthquake so every partner remains legal.",
        ),
        "showdown:gen9championsrandomdoublesbattle:001": (
            "adapted-set",
            "The Champions generator validates Gyarados as a doubles setup closer. Matt reserves Mega Gyarados as the final boarding captain with one conditional Dragon Dance.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_revision": "The current species are already an excellent Aqua boarding party. The revision gives them exact roles, meaningful offsets, Tailwind and Wide Guard, two distinct Dry Skin fighters, a trapped target through Dhelmise, and conditional rather than automatic Mega Gyarados setup.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_PELIPPER",
            "level_offset": 1,
            "item": "ITEM_DAMP_ROCK",
            "ability": "ABILITY_DRIZZLE",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_TAILWIND", "MOVE_HURRICANE", "MOVE_WEATHER_BALL", "MOVE_WIDE_GUARD"],
            "role": "Storm lookout: establishes finite rain, controls speed, and shields the boarding party from spread attacks while retaining two direct moves.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_DHELMISE",
            "level_offset": 1,
            "item": "ITEM_SPELL_TAG",
            "ability": "ABILITY_STEELWORKER",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_ANCHOR_SHOT", "MOVE_POWER_WHIP", "MOVE_POLTERGEIST", "MOVE_ROCK_SLIDE"],
            "role": "The anchor: traps one target through ordinary Anchor Shot and supplies Grass, Ghost, Steel, and spread Rock pressure without a passive trap loop.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_HELIOLISK",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_DRY_SKIN",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDER", "MOVE_WEATHER_BALL", "MOVE_GRASS_KNOT", "MOVE_PROTECT"],
            "role": "Dry Skin lightning gun that heals in rain and punishes Water, Ground-weight, and passive positions from the special side.",
            "lead_group": "dry-skin-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_TOXICROAK",
            "level_offset": 2,
            "item": "ITEM_BLACK_SLUDGE",
            "ability": "ABILITY_DRY_SKIN",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_FAKE_OUT", "MOVE_DRAIN_PUNCH", "MOVE_GUNK_SHOT", "MOVE_SUCKER_PUNCH"],
            "role": "Physical boarding fighter that turns rain into sustain and layers Fake Out and Sucker Punch without setup or Protect.",
            "lead_group": "dry-skin-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_SEISMITOAD",
            "level_offset": 3,
            "item": "ITEM_CHOICE_BAND",
            "ability": "ABILITY_SWIFT_SWIM",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_LIQUIDATION", "MOVE_DRAIN_PUNCH", "MOVE_POWER_WHIP", "MOVE_ICE_PUNCH"],
            "role": "Choice-committed Swift Swim deckbreaker with four ally-safe attacks and broad anti-Water, Grass, Dragon, and Steel coverage.",
            "lead_group": "breaker-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_GYARADOS",
            "level_offset": 4,
            "item": "ITEM_GYARADOSITE",
            "ability": "ABILITY_INTIMIDATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DRAGON_DANCE", "MOVE_WATERFALL", "MOVE_CRUNCH", "MOVE_POWER_WHIP"],
            "role": "Matt's sole Mega and boarding captain: one conditional Dragon Dance can close, but no Protect or recovery grants a free turn.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "AQUA_HIDEOUT_MATT",
        "planning_tier": "faction_admin_finale",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Matt's Aqua Hideout battle in the six-Badge chapter before the submarine departure",
            "location": "AquaHideout_B2F",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and the complete pre-Mossdeep catch, move, ability, leveling, and ordinary held-item toolkit",
                "Mega Bracelet and all campaign Mega Stones earned before the hideout",
                "Manual healing and party rebuilding before the admin chamber",
                "Prior knowledge of rain, Tailwind, Wide Guard, traps, Choice items, and priority",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Matt uses exactly one Mega Gyarados and no Primal. Archie's later Primal Kyogre remains protected.",
            "evolution_phase": "Late campaign admin boss: fully evolved and one signature Mega are appropriate.",
            "preparation_access": "The player may heal and rebuild before Matt; this is not a no-menu party lock.",
            "gauntlet_position": "Aqua's brute boarding-party exam. It must contrast Shelly's phase science and Archie's accelerating current through anchors, sustain, and direct deck pressure.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_MATT"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "aqua_hideout_matt", "trainer_ids": ["TRAINER_MATT"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "mt_pyre_matt", "trainer_ids": ["TRAINER_MATT_MT_PYRE"], "format": "double", "scope": "separate-backward-anchor", "reachability": "earlier required battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Matt",
                "src/data/trainers.h:TRAINER_MATT",
                "data/maps/AquaHideout_B2F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Aqua Hideout, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["AQUA_HIDEOUT_GRUNTS", "MT_PYRE_MATT", "SEAFLOOR_CAVERN_SHELLY", "SEAFLOOR_CAVERN_FINAL_ARCHIE"],
            "required_preimplementation_review": "Refresh the last ten hideout battles. Preserve Pelipper-Dhelmise storm anchoring, distinct Dry Skin attackers, Choice Seismitoad, and Mega Gyarados unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Matt boards the player's team: Pelipper brings the storm, Dhelmise drops anchor, Heliolisk and Toxicroak thrive on the wet deck, Seismitoad breaks the line, and Mega Gyarados takes command.",
            "story_fit": "Matt is Aqua's muscle, so his rain team is a boarding party rather than weather science or abstract momentum. Every member either holds the deck, heals in the storm, or hits through resistance.",
            "primary_player_question": "Can the player escape Dhelmise's anchor and contest Pelipper's Tailwind/Wide Guard while distinguishing two Dry Skin sustain threats, then exploit Seismitoad's Choice lock before Mega Gyarados finds a Dragon Dance?",
            "primary_mode": "Pelipper and Dhelmise establish finite rain, speed, spread defense, and one ordinary trapped target with direct offensive coverage.",
            "secondary_mode": "Heliolisk and Toxicroak convert rain into distinct special and physical sustain, Seismitoad makes a fast Choice commitment, and Mega Gyarados seeks one earned setup turn.",
            "preview_pressure": "The roster is visibly nautical and rain-based, but no Primal, Palafin cycle, priority shield, or screen phase steals Archie or Shelly's identity.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 61 through 64 against cap 60 with finite rain, Tailwind, Wide Guard, trapping, two Dry Skin sustain axes, Swift Swim Choice pressure, layered priority, and one setup Mega. Pelipper is the sole weather source, locks are public, and the team has no sleep, redirection, or recovery move loop.",
            "pressure_sources": [
                "Damp Rock Pelipper Tailwind, Wide Guard, Hurricane, and Weather Ball",
                "Assault Vest Steelworker Dhelmise Anchor Shot and four-type pressure",
                "Life Orb Dry Skin Heliolisk perfect rain Thunder",
                "Black Sludge Dry Skin Toxicroak Fake Out and Sucker Punch",
                "Choice Band Swift Swim Seismitoad ally-safe coverage",
                "Mega Gyarados conditional Dragon Dance and three attacks",
            ],
            "resource_tax": "The battle taxes weather and speed control, trapped-target rescue, spread-versus-single variation, Electric/Grass/Psychic/Fairy coverage, Choice scouting, and enough Intimidate, burn, priority, or physical bulk for Mega Gyarados.",
            "tuning_order": [
                "Preserve storm anchoring, two distinct Dry Skin roles, Choice deckbreaker, and Mega captain",
                "Validate Tailwind, Wide Guard, Anchor Shot, Dry Skin, Choice lock, and setup predicates before changing sets",
                "Adjust offsets within +1 to +4, beginning with Gyarados, Seismitoad, and Heliolisk",
                "Then adjust Pelipper or Dhelmise survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_PELIPPER", "SPECIES_DHELMISE"],
            "mandatory_order_reason": "The opening establishes Matt's storm and anchor. The two Dry Skin roles, breaker, and captain are board-state reserves rather than scripted pairs.",
            "reserve_sequence": [
                "Use Heliolisk when special Electric, Grass, or rain Weather Ball pressure and Dry Skin sustain are correct.",
                "Use Toxicroak when Fake Out, priority, Poison/Fighting coverage, or physical Dry Skin sustain creates more value.",
                "Use Seismitoad when rain speed and a public Choice commitment produce an immediate breaking line.",
                "Preserve Mega Gyarados as the boarding captain when practical, but deploy it earlier if Intimidate or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Pelipper's Tailwind and Wide Guard from actual speed and disclosed spread pressure; attack when support is redundant.",
                "Treat Anchor Shot as an ordinary trap with legal switches and fainting behavior, not a permanent scripted lock.",
                "Recognize Dry Skin healing and Fire vulnerability under active weather without stalling solely for recovery.",
                "Respect Seismitoad's Choice lock and choose an ally-safe move for the current partner.",
                "Use Dragon Dance only when Mega Gyarados survives the visible turn and the boost improves a real next-turn line; otherwise attack immediately.",
            ],
            "forbidden_behaviors": [
                "Do not loop Wide Guard, Fake Out, Dry Skin sustain, or Dragon Dance without visible value.",
                "Do not treat Anchor Shot as Shadow Tag or inspect hidden switches.",
                "Do not violate Choice lock or use inaccurate rain moves as guaranteed outside actual rain.",
                "Do not add Primal rain, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes Pelipper-Dhelmise storm anchoring. State B selects the correct special or physical Dry Skin boarder. State C commits Seismitoad to one breaking move. State D exposes Mega Gyarados as captain and permits one earned Dragon Dance. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Remove Pelipper, replace rain, contest Tailwind, bait Wide Guard with single-target attacks, or use Electric/Rock pressure before support stabilizes.",
                "Pivot or concentrate damage before Anchor Shot lands, use Ghost immunity where applicable, or break Dhelmise with Fire/Dark/Ghost/Ice/Flying pressure.",
                "Use Ground against Heliolisk with care, Psychic/Flying against Toxicroak, weather replacement, Taunt, item disruption, or concentrated damage to overcome Dry Skin sustain.",
                "Exploit Seismitoad's public Choice lock through immunity, resist pivots, Protect, and forced target changes.",
                "Use Intimidate, burn, Haze, Unaware, phazing, priority, Electric/Rock/Fairy, or immediate double-targeting before Mega Gyarados converts Dragon Dance.",
            ],
            "intentional_weakness": "Pelipper is the only rain source; Dhelmise lacks Protect; Heliolisk and Toxicroak share Ground/Psychic pressure; Seismitoad is Choice-locked; Mega Gyarados has no Protect or recovery. There is no redirection, sleep, permanent trap, or Primal.",
            "first_loss_lesson": "Matt wins by holding you on a wet deck. Break Pelipper or vary your targeting around Wide Guard, escape the anchor, choose the correct Dry Skin target, exploit Seismitoad's commitment, and never donate a free dance to Gyarados.",
            "revealed_information": [
                "Rain turns, Tailwind, Wide Guard, Anchor Shot trapping, Dry Skin healing, Choice lock, Intimidate, Dragon Dance, and Mega evolution are public state.",
                "Anchor Shot follows ordinary engine trapping and does not survive invalid conditions.",
                "Dry Skin uses ordinary weather healing and weakness behavior.",
                "Mega Gyarados is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Pelipper loops support into single-target attacks",
                "Anchor Shot behaves as permanent or hidden trapping",
                "Dry Skin turns become passive stalls",
                "Seismitoad violates Choice lock",
                "Gyarados dances into a visible knockout or copies Archie's momentum sequence",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Pelipper tournament rain", "Dhelmise Anchor Shot doubles", "Heliolisk Dry Skin doubles", "Toxicroak doubles", "Seismitoad rain", "Mega Gyarados doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Archaludon, Basculegion, Maushold, redirection, complete tournament rain balance, and Primal weather are not imported.",
                "Archie's Tsareena, Palafin, Urshifu, and Sharpedo current remains protected.",
                "Shelly's snow, screens, and phase-change chain remains protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Tournament-proven Pelipper rain legitimacy",
                "Generated Heliolisk Dry Skin special offense",
                "Champions-generator Toxicroak immediate utility",
                "Generated Seismitoad Swift Swim pressure",
                "Champions-generator Gyarados adapted into Matt's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Matt's definitive Pelipper-Dhelmise storm-and-anchor opening",
                "Parallel special and physical Dry Skin boarding roles",
                "Choice Band Seismitoad as rain deckbreaker",
                "Mega Gyarados as Matt's boarding captain",
            ],
            "preserves": [
                "Shelly's snow screen and phase-change science",
                "Archie's Primal flood, Palafin cycle, Protect punishment, and Mega Sharpedo",
                "Juan's Surf relay and Wallace's dual-speed champion rain",
                "Other Pelipper rain teams only when they do not repeat Anchor Shot and Dry Skin boarding",
            ],
            "releases": [
                "Matt's exact current species remain because their roles already form a coherent identity; only their sets and ordering are sharpened",
                "Other nautical and rain species remain available for grunts and routes without the full boarding-party sequence",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, leader, Courtney, Shelly, or paired Tabitha anchors.",
                "Matt uses finite Pelipper rain and no Primal; Archie retains the story-signature Kyogre reveal.",
                "His question is trapping and boarding sustain, not Shelly's phase or Archie's momentum.",
            ],
        },
        "presentation": {
            "intro_concept": "Matt tells the player that once his anchor drops, there is nowhere left to run on this deck.",
            "defeat_concept": "He admits the player cut the storm line, raised the anchor, and beat the crew member by member.",
            "post_battle_concept": "Native hideout progression remains unchanged; Matt's loss cannot prevent the submarine's larger mission.",
            "hint_concept": "Nearby Aqua dialogue warns that the anchor holds one target, two fighters heal in rain for different reasons, the toad commits to one move, and the captain only needs one dance.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, Pelipper-Dhelmise storm anchoring, Heliolisk and Toxicroak Dry Skin roles, Choice Band Seismitoad, Mega Gyarados finale, trap/weather/lock/setup AI, broad counterplay, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Matt's existing roster becomes one vivid boarding party: storm, anchor, wet-deck fighters, deckbreaker, captain. It is factional without repeating either Aqua strategist.",
            "weakest_link": "Dry Skin plus rain can become tedious if the AI stalls. Neither user has Protect or a recovery move, and the AI must keep attacking; that constraint is essential.",
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
                "The source guide places Matt at strict cap 60 with a six-Pokemon authored roster.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Gyaradosite maps Gyarados to Mega Gyarados and no other transformation item appears.",
                "All five selected references exist and cover tournament rain plus generated role evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_Matt with the exact six sets and offsets.",
                "Add partner, HP, speed, field, and combo flags and implement storm, anchor, Dry Skin, and setup scoring.",
                "Regression-test rain duration, Tailwind, Wide Guard, Anchor Shot, Dry Skin, Fake Out, Sucker Punch, Choice lock, Dragon Dance predicate, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-60 weather, Electric/Grass/Psychic/Fairy, anti-trap, Choice exploitation, Haze/Unaware, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def mt_chimney_maxie_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "vgc:naic-2022": (
            "selected-history",
            "The 2022 North American International Champion roster validates Groudon sun at elite stakes. Mt. Chimney uses base Groudon as foreshadowing and reserves Primal Reversion and the full land-geometry shell for Maxie's finale.",
        ),
        "showdown:gen4randomdoublesbattle:007": (
            "adapted-set",
            "The generated Shiftry set validates fast sun pressure and priority. Maxie removes sleep and setup for Focus Sash Fake Out, Leaf Blade, Knock Off, and Protect.",
        ),
        "showdown:gen9championsrandomdoublesbattle:003": (
            "selected-set",
            "The Champions generator validates Salazzle as a fast doubles utility attacker. Maxie uses Fake Out and mixed spread pressure without importing recovery or unrelated teammates.",
        ),
        "vgc:regional-portland-2024": (
            "selected-history",
            "The Portland-winning roster validates Entei as modern high-level doubles pressure. Maxie uses an Assault Vest four-attack set with no Tera or full balance shell.",
        ),
        "showdown:gen5randomdoublesbattle:002": (
            "adapted-set",
            "The generated Flygon set validates it as a doubles Ground/Dragon attacker. Maxie upgrades the custom Mega into his volcanic-ridge ace and retains only direct coverage.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current fight spends Primal Groudon too early and relies on Sleep Powder. The new version reveals base Groudon and high-level sun, but makes ridge positioning, two distinct Fake Out users, priority, and Mega Flygon the chapter lesson.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_GROUDON",
            "level_offset": 1,
            "item": "ITEM_HEAT_ROCK",
            "ability": "ABILITY_DROUGHT",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_PRECIPICE_BLADES", "MOVE_FIRE_PUNCH", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Base-form land foreshadowing: establishes ordinary sun and direct spread pressure without Primal weather, setup, or the finale's geometry engine.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_CROBAT",
            "level_offset": 1,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_INFILTRATOR",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_SUPER_FANG", "MOVE_TAUNT", "MOVE_QUICK_GUARD"],
            "role": "Ridge scout: contests speed, halves bulky answers, denies setup, and protects against priority without dealing the fight's damage itself.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_SHIFTRY",
            "level_offset": 2,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_CHLOROPHYLL",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_LEAF_BLADE", "MOVE_KNOCK_OFF", "MOVE_PROTECT"],
            "role": "Sun-accelerated physical ambusher whose Fake Out and Grass/Dark coverage punish Water and item-dependent counters without sleep.",
            "lead_group": "sun-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_SALAZZLE",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_CORROSION",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_SLUDGE_BOMB", "MOVE_FAKE_OUT", "MOVE_PROTECT"],
            "role": "Fast special ambusher and second distinct Fake Out user; Heat Wave and Poison pressure make it unlike Shiftry rather than a redundant support module.",
            "lead_group": "sun-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_ENTEI",
            "level_offset": 3,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_INNER_FOCUS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_SACRED_FIRE", "MOVE_EXTREME_SPEED", "MOVE_STOMPING_TANTRUM", "MOVE_SNARL"],
            "role": "Rare volcanic guardian with burn, priority, Ground coverage, and Snarl; four attacks keep it active rather than creating a sustain loop.",
            "lead_group": "guardian-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_FLYGON",
            "level_offset": 4,
            "item": "ITEM_FLYGONITE",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_EARTHQUAKE", "MOVE_DRAGON_CLAW", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Maxie's sole Mega and ridge apex: uses Earthquake only beside Crobat, a protected ally, or a favorable damage board, with Dragon and Rock alternatives always available.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MT_CHIMNEY_MAXIE",
        "planning_tier": "faction_leader_midpoint",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "First full Maxie boss at Mt. Chimney after Wattson and before Flannery",
            "location": "MtChimney",
            "strict_cap": 40,
            "player_tools": [
                "Three Badges and every catch and progression tool through Meteor Falls, the cable car, and Mt. Chimney",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet access established before Wattson and the Mega Stones obtainable before the summit",
                "Manual healing and party preparation before the summit confrontation",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Maxie uses one Mega Flygon. Groudon remains base form with ordinary Drought; Red Orb and Primal Reversion are reserved for Magma Hideout.",
            "evolution_phase": "Midgame boss phase at levels 41-44: fully evolved and single-stage threats are appropriate for a faction leader, while route trainers may still use middle stages.",
            "preparation_access": "The player may heal and rebuild before Maxie; the summit is not a no-menu party lock.",
            "gauntlet_position": "The first mature Magma leader exam. It introduces Groudon and ridge positioning without spending the finale's Primal, Flower Gift, Gravity, Instruct, or Mega Camerupt.",
            "mechanics_baseline_id": "faction_leader",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "mt_chimney_maxie", "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "magma_hideout_final", "trainer_ids": ["TRAINER_MAXIE_MAGMA_HIDEOUT"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
                {"variant_id": "mossdeep_multi", "trainer_ids": ["TRAINER_MAXIE_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MaxieMtChimney",
                "src/data/trainers.h:TRAINER_MAXIE_MT_CHIMNEY",
                "data/maps/MtChimney/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Mt. Chimney, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MT_CHIMNEY_TABITHA", "LAVARIDGE_GYM_FLANNERY", "MAGMA_HIDEOUT_FINAL_MAXIE", "METEOR_FALLS_COURTNEY"],
            "required_preimplementation_review": "Refresh the last ten volcanic encounters. Preserve base Groudon foreshadowing, Crobat ridge control, sleep-free dual ambush, Entei, and Mega Flygon unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Maxie claims the summit: Groudon raises the heat, Crobat controls the air lane, two ambushers attack from the brush and fumes, Entei guards the crater, and Mega Flygon owns the ridge.",
            "story_fit": "This is territorial conquest at the top of a volcano, not yet world reshaping. Maxie controls lanes, elevation, and priority while revealing that Groudon is already part of his plan.",
            "primary_player_question": "Can the player contest Crobat's ridge control and two different Fake Out ambushers while managing base Groudon's spread pressure, then preserve an Earthquake-safe formation and speed answer for Mega Flygon?",
            "primary_mode": "Base Groudon plus Crobat exposes ordinary sun, Tailwind, Taunt, Quick Guard, Super Fang, and Precipice pressure with no Primal or setup.",
            "secondary_mode": "Shiftry and Salazzle create distinct physical and special ambushes, Entei supplies rare direct pressure, and Mega Flygon turns partner positioning into the final ridge puzzle.",
            "preview_pressure": "Groudon appears early as intended, but its missing Red Orb is visible. Mega Flygon—not Groudon—is the transformation and makes this fight a midpoint rather than the finale.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 41 through 44 against cap 40 with base Groudon, Tailwind, Taunt, Quick Guard, two distinct Fake Out attackers, mixed sun offense, a rare legendary, and Mega Flygon. The team has no sleep, redirection, healing loop, Primal weather, or secondary speed mode.",
            "pressure_sources": [
                "Heat Rock base Groudon spread Ground, Fire, and Rock pressure",
                "Crobat Tailwind, Taunt, Quick Guard, and Super Fang",
                "Focus Sash Chlorophyll Shiftry physical Fake Out and item removal",
                "Life Orb Salazzle special Fake Out and spread heat",
                "Assault Vest Entei burn, priority, Snarl, and Ground coverage",
                "Mega Flygon Ground/Dragon/Rock spread and partner-positioning pressure",
            ],
            "resource_tax": "The fight taxes speed control, Fake Out sequencing, Wide Guard and Protect, Ground immunities, mixed bulk, priority awareness, and a safe way to pressure Mega Flygon without donating Earthquake value.",
            "tuning_order": [
                "Preserve base Groudon foreshadowing, ridge control, dual ambush, and Mega Flygon",
                "Validate Tailwind, Quick Guard, Fake Out, and ally-safe Earthquake scoring before changing sets",
                "Adjust offsets within +1 to +4, beginning with Flygon, Entei, and Groudon",
                "Then adjust Crobat or the ambushers' survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_GROUDON", "SPECIES_CROBAT"],
            "mandatory_order_reason": "The opening makes the summit-control thesis and base-form Groudon public. Later ambush, guardian, and Mega roles are board-state reserves.",
            "reserve_sequence": [
                "Use Shiftry when physical Fake Out, Grass/Dark coverage, or item removal best exploits active sun.",
                "Use Salazzle when special Fake Out, Fire/Poison spread, or speed produces the better ambush.",
                "Use Entei when direct bulky priority and Snarl pressure are more valuable than preserving a support mode.",
                "Preserve Mega Flygon as ridge apex when practical, but use it earlier if its immunity or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_FIELD_CONTROL"],
            "required_flags": ["AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Crobat's Tailwind, Taunt, Quick Guard, and Super Fang from actual speed, priority, and target HP rather than fixed first-turn scripts.",
                "Choose between Shiftry and Salazzle Fake Out based on physical versus special coverage and do not layer redundant Fake Out when a knockout is available.",
                "Use Groudon's spread pressure without assuming Gravity or Primal weather that is not present.",
                "Use Mega Flygon Earthquake only beside Crobat, a protected ally, or a board where ally damage is outweighed by a real knockout; otherwise choose Dragon Claw, Rock Slide, or Protect.",
                "Mega Evolve Flygon normally and preserve Groudon's base form throughout this encounter.",
            ],
            "forbidden_behaviors": [
                "Do not equip or trigger Red Orb, Primal Reversion, sleep, evasion, or hidden information.",
                "Do not Tailwind, Quick Guard, Taunt, or Fake Out without visible value.",
                "Do not Earthquake vulnerable partners by default.",
                "Do not add a second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes base Groudon-Crobat summit control. State B chooses one matchup-correct ambusher. State C deploys Entei as guardian. State D exposes Mega Flygon as ridge apex. Every state retains independent attacks and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Contest Tailwind, Taunt Crobat first, use opposing speed control, or exploit Crobat's low direct damage before Super Fang compounds.",
                "Use Wide Guard, Flying/Levitate, Water after sun control, Grass/Fairy/Fighting, and mixed bulk against Groudon and the two ambushers.",
                "Protect, Ghost types, Inner Focus, priority, or double-targeting can reduce Fake Out tempo; neither ambusher is durable after Sash or Life Orb pressure.",
                "Use Water/Rock/Ground, special bulk, or concentrated damage against Entei while respecting Sacred Fire burn and Extreme Speed.",
                "Position Ground immunities, Intimidate or burn, Ice/Fairy/Dragon, Wide Guard, and speed control against Mega Flygon's no-recovery finale.",
            ],
            "intentional_weakness": "Crobat has little direct damage; Shiftry and Salazzle are frail; Entei lacks Protect; base Groudon has no setup; Mega Flygon must manage ally Earthquake and has no recovery. There is no Primal, redirection, sleep, or healing loop.",
            "first_loss_lesson": "This is a fight for the ridge. Break Crobat's control, do not let both ambushers steal tempo, remember Groudon is not yet Primal, and enter the Flygon endgame with your positioning rather than only your type chart intact.",
            "revealed_information": [
                "Ordinary sun, Tailwind, Quick Guard, Taunt, Fake Out, Focus Sash, Airborne types, spread targeting, and Mega evolution are public state.",
                "Groudon visibly lacks Red Orb and never Primal Reverts.",
                "Earthquake ally damage follows ordinary engine rules.",
                "Mega Flygon is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Groudon Primal Reverts or receives finale-only geometry",
                "Crobat support loops blindly",
                "Both ambushers spam redundant Fake Out",
                "Flygon Earthquakes vulnerable partners without payoff",
                "Mt. Chimney duplicates the final Maxie battle instead of foreshadowing it",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["base Groudon tournament sun", "Shiftry doubles", "Salazzle Champions doubles", "Entei tournament doubles", "Mega Flygon doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Primal Groudon, Red Orb, Flower Gift, Gravity, Instruct, and Mega Camerupt are reserved for final Maxie.",
                "Sleep Powder, generic setup, Tera, and complete tournament balance shells are not imported.",
                "Flannery's After You and Trick Room heat remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Tournament-proven Groudon and Entei pressure",
                "Generated sleep-free Shiftry doubles offense",
                "Champions-generator Salazzle utility",
                "Generated Flygon direct coverage adapted into Maxie's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Base Groudon's first Maxie reveal",
                "Crobat summit Tailwind-Taunt-Quick Guard control",
                "Sleep-free Shiftry and Salazzle dual ambush",
                "Entei as the rare volcanic guardian",
                "Mega Flygon as Mt. Chimney ridge apex",
            ],
            "preserves": [
                "Final Maxie's Primal Groudon, Cherrim, Oranguru geometry, Walking Wake inversion, and Mega Camerupt",
                "Flannery's thermal timing and Tabitha's machinery",
                "Other Groudon appearances only as deliberate same-character progression",
                "Archie's rain current and Matt's grave tide",
            ],
            "releases": [
                "Zweilous, Weezing, Victreebel, and ordinary Camerupt leave Mt. Chimney",
                "Other volcanic and sun species remain available if they do not repeat the full ridge-control sequence",
            ],
            "collision_notes": [
                "Groudon is an intentional same-character reprise: base here, Primal with complete geometry in Magma Hideout.",
                "No other species overlaps the protected Gym, League, or designed faction anchors.",
                "Mega Flygon is unique to this midpoint and keeps Mega Camerupt exclusive to the finale.",
            ],
        },
        "presentation": {
            "intro_concept": "Maxie says holding the summit proves land belongs to whoever controls every approach to it.",
            "defeat_concept": "He acknowledges that the player took the ridge without mistaking Groudon's presence for the whole plan.",
            "post_battle_concept": "Native Mt. Chimney progression remains unchanged; Maxie withdraws with his larger Primal ambition still unrevealed.",
            "hint_concept": "Nearby Magma dialogue warns that the bat owns the air lane, two ambushers can steal a turn in different ways, Groudon is not yet transformed, and the dragon's quake needs a safe partner.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 40, base Groudon-Crobat summit control, Shiftry and Salazzle dual ambush, Entei guardian, Mega Flygon finale, explicit no-Primal rule, partner-safe Earthquake AI, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "The battle introduces Groudon without spending Primal Groudon and makes the summit itself—air lane, ambushes, ridge apex—the puzzle.",
            "weakest_link": "Two Fake Out users can feel repetitive. Their physical Grass/Dark versus special Fire/Poison jobs and mutually exclusive reserve selection must stay distinct in AI and guide text.",
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
                "The source guide places Mt. Chimney Maxie at strict cap 40 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Flygonite maps Flygon to Mega Flygon; Groudon holds Heat Rock rather than Red Orb.",
                "All five references exist and include champion, tournament, generated, and Champions evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_MaxieMtChimney with the exact six sets and offsets.",
                "Add HP, speed, and combo flags and implement ridge-control and ally-safe Earthquake scoring.",
                "Regression-test ordinary Drought, no-Primal behavior, Tailwind, Quick Guard, Taunt, Fake Out selection, Super Fang, Mega Flygon, Earthquake partner safety, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-40 Wide Guard, speed control, Flying/Levitate, Water/Rock/Ground, fake-out immunity, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def source_closed_mt_chimney_maxie_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    """Reconcile the soft summit anchor with reached context, evolution legality, and source."""
    dossier = mt_chimney_maxie_design(meta, records, source)
    team = [
        {
            "order": 1, "species": "SPECIES_GROUDON", "level_offset": 1, "item": "ITEM_HEAT_ROCK",
            "ability": "ABILITY_DROUGHT", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_PRECIPICE_BLADES", "MOVE_FIRE_PUNCH", "MOVE_STONE_EDGE", "MOVE_PROTECT"],
            "role": "Base-form land foreshadowing: ordinary sun and direct Ground/Fire/Rock pressure without Red Orb, setup, or finale geometry.",
            "lead_group": "guaranteed-lead", "mega_candidate": False,
        },
        {
            "order": 2, "species": "SPECIES_CROBAT", "level_offset": 1, "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_INFILTRATOR", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPEED_TIMID",
            "moves": ["MOVE_TAILWIND", "MOVE_SUPER_FANG", "MOVE_TAUNT", "MOVE_QUICK_GUARD"],
            "role": "Ridge scout: controls speed and priority, halves bulky answers, and denies setup with little direct damage.",
            "lead_group": "guaranteed-lead", "mega_candidate": False,
        },
        {
            "order": 3, "species": "SPECIES_SHIFTRY", "level_offset": 2, "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_CHLOROPHYLL", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_LEAF_BLADE", "MOVE_KNOCK_OFF", "MOVE_PROTECT"],
            "role": "Sun-accelerated physical ambusher and the roster's sole Fake Out user, with Grass/Dark item pressure.",
            "lead_group": "sun-reserve", "mega_candidate": False,
        },
        {
            "order": 4, "species": "SPECIES_SALAZZLE", "level_offset": 2, "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_CORROSION", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_SLUDGE_BOMB", "MOVE_ENCORE", "MOVE_HELPING_HAND"],
            "role": "Fast special ambusher: spread Fire/Poison pressure, Encore punishment, and Helping Hand rather than a second Fake Out/Protect module.",
            "lead_group": "sun-reserve", "mega_candidate": False,
        },
        {
            "order": 5, "species": "SPECIES_ENTEI", "level_offset": 3, "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_INNER_FOCUS", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_SACRED_FIRE", "MOVE_EXTREME_SPEED", "MOVE_STOMPING_TANTRUM", "MOVE_SNARL"],
            "role": "Rare volcanic guardian with burn, priority, Ground coverage, and Snarl; four attacks prevent sustain loops.",
            "lead_group": "guardian-reserve", "mega_candidate": False,
        },
        {
            "order": 6, "species": "SPECIES_FLYGON", "level_offset": 5, "item": "ITEM_FLYGONITE",
            "ability": "ABILITY_LEVITATE", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_EARTHQUAKE", "MOVE_DRAGON_CLAW", "MOVE_STONE_EDGE", "MOVE_PROTECT"],
            "role": "Sole Mega and legal level-45 ridge apex; Earthquake requires a safe ally or real payoff, with single-target alternatives.",
            "lead_group": "ace-reserve", "mega_candidate": True,
        },
    ]
    dossier["status"] = {"design": "design-complete", "source": "source-closed", "static": "design-validated", "runtime": "unplayed"}
    dossier["campaign_state"].update({
        "evolution_phase": "Midgame boss levels 41-45: fully evolved and single-stage threats are appropriate; Flygon appears at its exact source evolution level 45.",
        "preparation_access": "Tabitha returns control before Maxie's object interaction, and Maxie's own script checks for two usable Pokemon before locking the full scene; Bag healing and rebuilding are available.",
        "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+5; Medium subtracts two and Easy subtracts four from final opponent levels only.",
    })
    dossier["runtime"]["variants"][0]["scope"] = "source-closed"
    dossier["runtime"]["current_source_baseline"] = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "implementation_note": "Reached source removes premature Red Orb/Sleep Powder, uses one Fake Out role, and places Mega Flygon at its legal level 45.",
    }
    dossier["rolling_context"] = {
        "available": True,
        "reason": "The chronological ledger reached Maxie after source-closing Battles 100-109 and proving field-menu access after Tabitha.",
        "previous_encounters": [f"BATTLE_{index:03d}" for index in range(100, 110)],
        "protected_neighbor_anchors": ["MT_CHIMNEY_TABITHA", "LAVARIDGE_GYM_FLANNERY", "MAGMA_HIDEOUT_FINAL_MAXIE", "METEOR_FALLS_COURTNEY"],
        "required_preimplementation_review": "Complete: no previous-ten species collision exists; Protect/Rock Slide/Fake Out density was reduced without changing base Groudon, Crobat ridge control, Entei, or Mega Flygon.",
    }
    dossier["identity"] = {
        "memory_hook": "Maxie claims the summit: base Groudon raises heat, Crobat controls the air, Shiftry steals one turn, Salazzle punishes commitment, Entei guards the crater, and Mega Flygon owns the ridge.",
        "story_fit": "This is territorial conquest, not world reshaping. Groudon is publicly present but visibly lacks the Red Orb; Mega Flygon, not Groudon, is the midpoint transformation.",
        "primary_player_question": "Can the player contest Crobat's ridge control, manage base Groudon's spread pressure, and sequence around one physical Fake Out plus special Encore/Helping Hand before preserving a safe formation for Mega Flygon?",
        "primary_mode": "Base Groudon plus Crobat establish ordinary sun, Tailwind, Taunt, Quick Guard, Super Fang, and Precipice pressure with no Primal or setup.",
        "secondary_mode": "Shiftry and Salazzle create distinct physical and special tempo, Entei supplies rare direct pressure, and legal level-45 Mega Flygon makes positioning the final exam.",
        "preview_pressure": "Heat Rock rather than Red Orb is visible; Crobat owns the one speed field; Salazzle has no second Fake Out; Flygon is the sole Mega.",
    }
    dossier["difficulty"] = {
        "target": 10, "observed": None,
        "rationale": "Hard places levels 41-45 against cap 40 with base Groudon, Tailwind/Taunt/Quick Guard, one Fake Out, special Encore/Helping Hand, Assault Vest Entei, and Mega Flygon positioning. No sleep, redirection, healing loop, Primal, second setup, or second speed field exists.",
        "pressure_sources": [
            "Heat Rock base Groudon spread Ground and direct Fire/Rock pressure",
            "Crobat Tailwind, Taunt, Quick Guard, and Super Fang",
            "Focus Sash Chlorophyll Shiftry Fake Out and item removal",
            "Life Orb Salazzle spread offense, Encore, and Helping Hand",
            "Assault Vest Entei burn, priority, Snarl, and Ground coverage",
            "Level-45 Mega Flygon Earthquake positioning and direct Dragon/Rock coverage",
        ],
        "resource_tax": "The fight taxes speed control, one Fake Out sequence, Wide Guard/Protect, Ground immunities, mixed bulk, item awareness, and a safe Ice/Fairy/Dragon route into Mega Flygon.",
        "tuning_order": [
            "Preserve base Groudon, ridge control, split ambush, Entei, and legal Mega Flygon",
            "Validate Tailwind/Quick Guard/Taunt/Fake Out/Encore/Helping Hand and ally-safe Earthquake",
            "Adjust Flygon +5 first, then Entei +3 and the +2 ambushers",
            "Then adjust Groudon or Crobat survivability",
            "Change roles only after Hard/Medium/Easy runtime tests",
        ],
    }
    dossier["team"] = team
    dossier["ordering"] = {
        "intended_lead": ["SPECIES_GROUDON", "SPECIES_CROBAT"],
        "mandatory_order_reason": "The opening makes summit control and base Groudon public. Ambush, guardian, and Mega roles remain board-state reserves.",
        "reserve_sequence": [
            "Use Shiftry when physical Fake Out, Grass/Dark coverage, or item removal best exploits sun.",
            "Use Salazzle when special spread, Encore, or Helping Hand creates the better tempo line.",
            "Use Entei when bulky priority, burn, and Snarl matter more than support.",
            "Preserve Mega Flygon as ridge apex when practical, but deploy it earlier when its immunity or coverage is uniquely correct.",
        ],
    }
    dossier["ai"] = {
        "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
        "required_flags": [],
        "custom_requirements": [
            "Score Crobat's Tailwind, Taunt, Quick Guard, and Super Fang from actual board state.",
            "Use Shiftry Fake Out only with visible tempo and Salazzle Encore/Helping Hand from the partner's selected action.",
            "Use Groudon spread pressure without assuming Primal weather or Gravity.",
            "Use Mega Flygon Earthquake only beside Crobat, a protected ally, or a board where payoff outweighs ally damage.",
            "Mega Evolve Flygon normally and keep Groudon base form throughout.",
        ],
        "forbidden_behaviors": [
            "Do not equip/trigger Red Orb, Primal Reversion, sleep, evasion, or hidden information.",
            "Do not use support without visible value or add a second Fake Out user.",
            "Do not Earthquake vulnerable partners by default.",
            "Do not add a second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
        ],
        "state_machine": "State A establishes base Groudon/Crobat summit control. State B selects one matchup-correct physical or special ambusher. State C deploys Entei as guardian. State D exposes Mega Flygon as ridge apex.",
    }
    dossier["counterplay"] = {
        "classes": [
            "Contest Tailwind, Taunt/focus Crobat, use opposing speed control, or exploit its low direct damage before Super Fang compounds.",
            "Use Wide Guard, Flying/Levitate, weather control, Water/Grass/Fairy/Fighting, and mixed bulk against Groudon and ambushers.",
            "Protect, Ghost/Inner Focus, priority, or double-targeting reduces Shiftry's one Fake Out; Taunt/Encore awareness limits Salazzle support.",
            "Use Water/Rock/Ground and special bulk against Entei while respecting Sacred Fire and Extreme Speed.",
            "Use Ground-immunity positioning, Intimidate/burn, Ice/Fairy/Dragon, Wide Guard, and speed control against Mega Flygon.",
        ],
        "intentional_weakness": "Crobat has little damage; Shiftry and Salazzle are frail; Entei lacks Protect; base Groudon has no setup; Flygon must manage ally Earthquake and has no recovery. There is no Primal, sleep, redirection, or healing loop.",
        "first_loss_lesson": "This is a fight for the ridge. Break Crobat's control, distinguish physical Fake Out from special Encore support, remember Groudon is not Primal, and enter Flygon with positioning intact.",
        "revealed_information": [
            "Ordinary sun, Tailwind, Quick Guard, Taunt, one Fake Out, Encore, Helping Hand, Airborne types, spread targeting, and Mega evolution are public.",
            "Groudon visibly holds Heat Rock rather than Red Orb and never Primal Reverts.",
            "Earthquake ally damage follows ordinary rules.",
            "Mega Flygon is the only transformation.",
        ],
        "unacceptable_failure_modes": [
            "Groudon Primal Reverts or receives finale-only geometry",
            "Crobat support loops blindly",
            "Salazzle behaves like a second identical Fake Out module",
            "Flygon Earthquakes vulnerable partners without payoff",
            "Mt. Chimney duplicates final Maxie instead of foreshadowing it",
        ],
    }
    dossier["campaign_reservations"] = {
        "spends": ["base Groudon's first Maxie reveal", "Crobat ridge control", "Shiftry/Salazzle split ambush", "Entei volcanic guardian", "legal level-45 Mega Flygon ridge apex"],
        "preserves": ["final Maxie's Primal Groudon/Cherrim/Oranguru/Walking Wake/Mega Camerupt", "Flannery's thermal timing", "Tabitha's machinery", "Archie's rain current and Matt's grave tide"],
        "releases": ["Zweilous, Weezing, Victreebel, ordinary Camerupt, premature Red Orb, Sleep Powder, and Swords Dance leave this source fight", "Other volcanic/sun species remain available outside the full ridge-control sequence"],
        "collision_notes": ["Groudon intentionally progresses from base here to Primal later.", "Crobat/Flygon/Salazzle are distant role-changed reprises outside the previous ten.", "Mega Flygon is unique here and Mega Camerupt remains finale-only."],
    }
    dossier["presentation"] = {
        "intro_concept": "Source story remains intact and ends by naming Groudon heat, Crobat air control, tempo ambushers, and Flygon ridge ownership.",
        "defeat_concept": "Maxie concedes the ridge, not his land ideology.",
        "post_battle_concept": "Native retreat and Orb foreshadowing remain unchanged without claiming Groudon transformed.",
        "hint_concept": "The visible Heat Rock, Crobat support, distinct ambushers, and Flygonite expose every key role.",
        "native_width_status": "pass; intro extension, defeat, retreat, and refusal text are source-implemented within the native 36-character line gate",
        "guide_summary": "Document cap 40, base Groudon/Crobat control, Shiftry Fake Out, Salazzle Encore/Helping Hand, Entei guardian, legal level-45 Mega Flygon, no-Primal rule, AI, counterplay, and difficulty offsets.",
    }
    dossier["author_self_check"] = {
        "strongest_part": "The battle introduces Groudon without spending Primal and turns the summit itself—air lane, tempo, and safe Earthquake positioning—into the puzzle.",
        "weakest_link": "Sun plus Tailwind plus Fake Out can feel familiar. One Fake Out, special Encore support, Stone Edge substitutions, no sleep, and the level-legal positioning ace keep the parts distinct.",
    }
    dossier["verification"] = {
        "design_schema": "pass", "species_items_moves_abilities": "pass", "source_implementation": "pass",
        "script_and_format": "pass", "dialogue_width": "pass", "guide": "pass", "runtime": "unplayed", "observed_difficulty": None,
        "evidence": [
            "Source matches six exact sets at +1,+1,+2,+2,+3,+5 and Flygon evolves at 45.",
            "Maxie scene performs an explicit two-usable-mon guard before the no-intro battle.",
            "Groudon holds Heat Rock, no Red Orb appears, and Flygonite is the only transformation item.",
            "All five selected competitive references remain indexed and source roles are legal.",
            "Rolling context and campaign anchors have zero unwaived collisions.",
        ],
        "source_blockers": ["Run cap-40 Wide Guard/speed/weather/Ground-immunity/Fake-Out-immunity and Hard/Medium/Easy real-ROM tests before observed difficulty is recorded."],
    }
    return dossier


def mt_pyre_matt_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "vgc:korean-spring-league-2018": (
            "selected-history",
            "The winning 2018 rain roster validates Politoed as tournament-grade weather control. Matt imports finite rain and direct pressure without its Shadow Tag, Trick Room, or full balance shell.",
        ),
        "showdown:gen8randomdoublesbattle:002": (
            "selected-set",
            "The generated roster validates Cramorant as a legitimate doubles pivot and attacker. Matt authors its speed-control seabird role around Mt. Pyre rather than copying Tailwind and unrelated attackers.",
        ),
        "showdown:gen9randomdoublesbattle:008": (
            "selected-set",
            "The generated Hoopa set validates a rare mythical doubles attacker. Matt uses confined Hoopa's Hyperspace Hole as a public shelter-breaking spirit without importing Choice disruption or Tailwind.",
        ),
        "vgc:regional-dallas-tx-2020": (
            "selected-history",
            "The Dallas-winning roster validates Jellicent on a major doubles team. Matt uses Water Absorb, Water Spout, burn, and protection without importing Trick Room or Togekiss support.",
        ),
        "showdown:gen9championsrandomdoublesbattle:023": (
            "adapted-set",
            "The Champions generator validates Feraligatr as a setup attacker. Matt reserves custom Mega Feraligatr as the grave-tide closer with one conditional Dragon Dance.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current Mt. Pyre team duplicates five of Matt's later hideout species. The new version keeps only Dhelmise as his recurring anchor and builds a site-specific grave tide from Politoed, Hoopa, Jellicent, Cramorant, and Mega Feraligatr.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_POLITOED",
            "level_offset": 1,
            "item": "ITEM_DAMP_ROCK",
            "ability": "ABILITY_DRIZZLE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_WEATHER_BALL", "MOVE_PROTECT"],
            "role": "Grave-rain conductor: controls speed, amplifies a partner, and attacks without Perish Song, sleep, or a trapping clock.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_DHELMISE",
            "level_offset": 1,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_STEELWORKER",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_ANCHOR_SHOT", "MOVE_POWER_WHIP", "MOVE_POLTERGEIST", "MOVE_PROTECT"],
            "role": "Matt's recurring anchor in its grave-tide phase: holds one target and pressures Water, Ghost, and Fairy answers without the later boarding roster.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_HOOPA",
            "level_offset": 2,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_MAGICIAN",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_HYPERSPACE_HOLE", "MOVE_SHADOW_BALL", "MOVE_FOCUS_BLAST", "MOVE_PROTECT"],
            "role": "Rare summit spirit whose Hyperspace Hole breaks ordinary shelter while Shadow Ball and Focus Blast provide risky but visible special coverage.",
            "lead_group": "spirit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_JELLICENT",
            "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_WATER_ABSORB",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_WATER_SPOUT", "MOVE_SHADOW_BALL", "MOVE_WILL_O_WISP", "MOVE_PROTECT"],
            "role": "Bulky drowned spirit whose Water Spout is explicitly HP-sensitive and whose burn controls physical answers without recovery looping.",
            "lead_group": "spirit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_CRAMORANT",
            "level_offset": 3,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_GULP_MISSILE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_HYDRO_PUMP", "MOVE_AIR_SLASH", "MOVE_ICY_WIND", "MOVE_PROTECT"],
            "role": "Unpredictable seabird pressure that changes speed and attacks from the special side; Gulp Missile remains ordinary engine behavior rather than a scripted gimmick.",
            "lead_group": "tide-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_FERALIGATR",
            "level_offset": 4,
            "item": "ITEM_FERALIGITE",
            "ability": "ABILITY_STRONG_JAW",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DRAGON_DANCE", "MOVE_WATERFALL", "MOVE_CRUNCH", "MOVE_ICE_PUNCH"],
            "role": "Matt's sole Mega and grave-tide beast: one earned Dragon Dance turns rain and Strong Jaw coverage into the finale, but it has no Protect or recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MT_PYRE_MATT",
        "planning_tier": "faction_admin_midpoint",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Matt's Mt. Pyre summit battle in the six-Badge chapter before the Aqua Hideout",
            "location": "MtPyre_Summit",
            "strict_cap": 60,
            "player_tools": [
                "Six Badges and all catches and progression tools available through Route 122 and Mt. Pyre",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet and the Mega Stones available before Mt. Pyre",
                "Manual healing and party preparation before the summit confrontation",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Matt uses one Mega Feraligatr and no Primal. His later Mega Gyarados is a distinct progression rather than a repeated Mega.",
            "evolution_phase": "Late campaign admin phase: fully evolved, mythical, and one signature Mega are appropriate.",
            "preparation_access": "The player may heal and rebuild before Matt; the summit is not a no-menu party lock.",
            "gauntlet_position": "Matt's first major identity battle. Mt. Pyre demands a ghostly grave tide, while his later hideout battle matures into a physical boarding party.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_MATT_MT_PYRE"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "mt_pyre_matt", "trainer_ids": ["TRAINER_MATT_MT_PYRE"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "aqua_hideout_matt", "trainer_ids": ["TRAINER_MATT"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_MattMtPyre",
                "src/data/trainers.h:TRAINER_MATT_MT_PYRE",
                "data/maps/MtPyre_Summit/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Mt. Pyre, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MT_PYRE_AQUA_GRUNTS", "AQUA_HIDEOUT_MATT", "WEATHER_INSTITUTE_SHELLY", "MAGMA_HIDEOUT_FINAL_MAXIE"],
            "required_preimplementation_review": "Refresh the last ten Mt. Pyre battles. Preserve Politoed-Dhelmise grave rain, Hoopa and Jellicent spirit pressure, Cramorant, and Mega Feraligatr unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Matt raises a grave tide: Politoed calls cold rain, Dhelmise drops the old anchor, Hoopa and Jellicent rise from the memorial, Cramorant circles above, and Mega Feraligatr climbs out last.",
            "story_fit": "Mt. Pyre should affect the team. Matt's Water identity becomes drowned ghosts and a grave-beast here, then changes into the hideout boarding party later.",
            "primary_player_question": "Can the player escape Dhelmise's grave anchor while controlling Politoed's speed and Helping Hand, then withstand Hoopa's shelter-breaking pressure and damage Jellicent before its Water Spout, burn, and Mega Feraligatr endgame compound?",
            "primary_mode": "Politoed plus Dhelmise establishes finite rain, Icy Wind, Helping Hand, and an ordinary anchored target without Perish Song or Shadow Tag.",
            "secondary_mode": "Hoopa breaks ordinary shelter, Jellicent supplies an HP-sensitive drowned Water Spout, Cramorant changes speed, and Mega Feraligatr seeks one earned Dragon Dance.",
            "preview_pressure": "Three Ghost types and four Water-associated members make the summit theme immediate. Only Dhelmise recurs later, as Matt's deliberate signature anchor.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 61 through 64 against cap 60 with finite rain, speed control, Helping Hand, trapping, protect-bypassing mythical pressure, HP-sensitive spread damage, burn, and one setup Mega. There is no Perish, Shadow Tag, sleep, recovery move, redirection, or second speed mode.",
            "pressure_sources": [
                "Damp Rock Politoed Icy Wind, Helping Hand, and rain Weather Ball",
                "Assault Vest Dhelmise Anchor Shot and three offensive types",
                "Focus Sash Hoopa Hyperspace Hole, Shadow Ball, and Focus Blast coverage",
                "Water Absorb Jellicent current-HP Water Spout and burn",
                "Life Orb Cramorant special damage and Icy Wind",
                "Mega Feraligatr conditional Dragon Dance and rain-amplified physical coverage",
            ],
            "resource_tax": "The fight taxes rain and speed control, trap escape, setup discipline, Ghost/Dark/Fairy answers, HP pressure, physical burn protection, and enough Haze, Unaware, priority, or bulk for Mega Feraligatr.",
            "tuning_order": [
                "Preserve grave tide, recurring anchor, shelter-breaking spirit, HP-sensitive drowned spirit, and Mega beast",
                "Validate Anchor Shot, Hyperspace Hole, Water Spout HP, Gulp Missile, and Dragon Dance predicates before changing sets",
                "Adjust offsets within +1 to +4, beginning with Feraligatr, Hoopa, and Jellicent",
                "Then adjust Politoed or Dhelmise survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_POLITOED", "SPECIES_DHELMISE"],
            "mandatory_order_reason": "The opening establishes grave rain and the recurring anchor. Spirits, seabird, and Mega beast are selected by board state rather than appearing in rigid pairs.",
            "reserve_sequence": [
                "Use Hoopa when Hyperspace Hole or its special coverage creates immediate visible value; do not infer hidden Protect or guarantee Focus Blast.",
                "Use Jellicent while its HP supports Water Spout or when Water Absorb and burn are matchup-correct; use Shadow Ball after HP falls.",
                "Use Cramorant when special Flying/Water coverage or a second Icy Wind line is needed.",
                "Preserve Mega Feraligatr as grave-tide beast when practical, but deploy it earlier if its coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Score Icy Wind, Helping Hand, Weather Ball, and Protect from actual speed, damage, and survival rather than fixed support turns.",
                "Treat Anchor Shot as ordinary visible trapping and never as Shadow Tag or a hidden-switch prediction.",
                "Use Hyperspace Hole through ordinary move rules and score Focus Blast with its real accuracy rather than as guaranteed coverage.",
                "Evaluate Jellicent Water Spout from current HP and switch to Shadow Ball, burn, or Protect when stronger.",
                "Use Dragon Dance only when Mega Feraligatr survives and the boost improves a real next-turn line; otherwise attack immediately.",
            ],
            "forbidden_behaviors": [
                "Do not add Perish Song, Shadow Tag, sleep, hidden information, or recovery loops.",
                "Do not treat trapping or Hyperspace Hole as predictive omniscience.",
                "Do not spam low-HP Water Spout or redundant Icy Wind.",
                "Do not add Primal rain, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes Politoed-Dhelmise grave tide. State B selects Hoopa for shelter-breaking spirit pressure. State C uses Jellicent as the HP-sensitive drowned spirit. State D uses Cramorant for aerial pressure. State E exposes Mega Feraligatr as the grave beast. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Replace rain, contest Icy Wind, Taunt or pressure Politoed, and vary Protect or target selection so Helping Hand cannot convert every turn.",
                "Escape or prevent Anchor Shot, use Ghost immunity where applicable, or break Dhelmise with Fire/Dark/Ghost/Ice/Flying pressure.",
                "Break Hoopa's Sash with spread or priority and use Dark/Ghost pressure, special bulk, or faster concentrated attacks rather than relying on Protect alone.",
                "Damage Jellicent immediately, use Electric/Grass/Dark/Ghost, Taunt, special pressure, or Water immunity so Water Spout loses force.",
                "Use Intimidate, burn prevention or healing, Haze, Unaware, phazing, priority, Electric/Grass/Fairy, or double-targeting before Mega Feraligatr dances.",
            ],
            "intentional_weakness": "Politoed is the only rain setter; Dhelmise lacks speed and recovery; Hoopa is Sash-dependent and has exposed Dark/Ghost weaknesses; Jellicent's strongest attack is HP-sensitive; Cramorant is frail; Mega Feraligatr has no Protect or recovery. There is no Perish, Shadow Tag, redirection, or sleep.",
            "first_loss_lesson": "Mt. Pyre punishes passive shelter and slow damage. Break the conductor or anchor, do not expect Protect to solve Hoopa, cut Jellicent's HP early, and never hand Feraligatr a free dance.",
            "revealed_information": [
                "Rain turns, Icy Wind, Helping Hand, Anchor Shot, Hyperspace Hole targeting, current HP, Gulp Missile forms, Dragon Dance, and Mega evolution are public state.",
                "Dhelmise is the sole deliberate recurring signature between Matt's two major teams.",
                "No Perish or Shadow Tag state exists in this battle.",
                "Mega Feraligatr is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "AI predicts hidden Protect or treats Focus Blast as guaranteed",
                "Anchor Shot behaves as permanent trapping",
                "Jellicent spams low-HP Water Spout",
                "Feraligatr dances into a visible knockout",
                "Mt. Pyre duplicates Matt's later boarding-party roster",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Politoed tournament rain", "Hoopa random doubles", "Cramorant random doubles", "Jellicent tournament doubles", "Mega Feraligatr Champions doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Shadow Tag, Perish, Trick Room, sleep, redirection, and complete rain balance shells are not imported.",
                "Matt's later Pelipper, Heliolisk, Toxicroak, Seismitoad, and Mega Gyarados boarding party remains protected.",
                "Archie's Primal momentum and Shelly's snow phase remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Tournament-winning Politoed rain legitimacy",
                "Generated Hoopa and Cramorant roles adapted to Mt. Pyre",
                "Tournament Jellicent legitimacy",
                "Champions-generator Feraligatr adapted into Matt's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Matt's Mt. Pyre grave-tide composition",
                "Dhelmise as his recurring signature anchor",
                "Hoopa shelter-breaking pressure at the memorial summit",
                "Jellicent HP-sensitive drowned spirit",
                "Mega Feraligatr as the grave-tide beast",
            ],
            "preserves": [
                "Matt's later Pelipper storm, Dry Skin boarders, Choice Seismitoad, and Mega Gyarados",
                "Phoebe's dedicated Perish clock and Shadow Tag",
                "Archie's Primal current and Shelly's phase science",
                "Other Ghost and Water species outside this exact grave-tide theme",
            ],
            "releases": [
                "Pelipper, Heliolisk, Toxicroak, and Seismitoad leave Mt. Pyre for Matt's later hideout team",
                "Other rain and ghost teams remain available if they do not combine this anchor, shelter-breaking spirit, and drowned-spirit sequence",
            ],
            "collision_notes": [
                "Dhelmise is an intentional same-character reprise; its context changes from grave anchor here to boarding anchor later.",
                "No other species overlaps the protected Gym, League, or designed faction anchors.",
                "Mega Feraligatr appears here while Mega Gyarados marks Matt's later growth.",
            ],
        },
        "presentation": {
            "intro_concept": "Matt says every ship has ghosts beneath it and Mt. Pyre is where his anchor can call them up.",
            "defeat_concept": "He admits the player cut through the grave tide before its beast could drag them under.",
            "post_battle_concept": "Native Mt. Pyre progression remains unchanged; Matt retreats toward the more physical Aqua Hideout confrontation.",
            "hint_concept": "Nearby Aqua dialogue warns that the anchor returns later, the ringed spirit attacks through shelter, the jellyfish must be damaged before it attacks, and the final beast wants one dance.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 60, Politoed-Dhelmise grave rain, Hoopa shelter-breaking pressure, Jellicent current-HP Water Spout, Cramorant pressure, Mega Feraligatr finale, explicit no-Perish rule, AI predicates, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "The map matters: three spirits, cold rain, and an old anchor turn Matt into a Mt. Pyre boss rather than an early copy of his hideout team.",
            "weakest_link": "Hoopa is extraordinarily strong and can feel arbitrary. Its map fit, Sash dependence, confined form, absent setup, Focus Blast variance, and broad Dark/Ghost counterplay must be tested honestly.",
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
                "The source guide places Mt. Pyre Matt at strict cap 60 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Feraligite maps Feraligatr to Mega Feraligatr and no other transformation item appears.",
                "All four selected references exist and include winning tournament, generated, and Champions evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_MattMtPyre with the exact six sets and offsets.",
                "Add partner, HP, speed, field, and combo flags and implement grave-tide reserve scoring.",
                "Regression-test rain duration, Icy Wind, Helping Hand, Anchor Shot, Hyperspace Hole, Water Spout HP, Gulp Missile, Dragon Dance predicate, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-60 weather, Ghost/Dark/Fairy/Psychic, anti-trap, no-setup, Haze/Unaware, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def mt_chimney_tabitha_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen8randomdoublesbattle:005": (
            "selected-set",
            "The generated Coalossal set validates it as doubles machinery before any self-activation. Mt. Chimney keeps Steam Engine public but supplies no allied Water trigger, reserving that upgrade for Tabitha's finale.",
        ),
        "showdown:gen6randomdoublesbattle:024": (
            "selected-set",
            "The generated Klinklang roster validates Shift Gear and direct Gear Grind pressure. Tabitha uses it as the only self-boosting prototype rather than adding a second weather engine.",
        ),
        "vgc:ocic-2017": (
            "selected-history",
            "The 2017 Oceania International Champion roster validates Magnezone at major doubles stakes. Tabitha turns it into a Choice Specs analytic circuit rather than importing the full champion team.",
        ),
        "vgc:ocic-2020": (
            "selected-history",
            "The 2020 Oceania International Champion roster validates Rotom-Heat as tournament machinery. Tabitha uses its pivot, burn, and Electroweb functions without importing the complete team.",
        ),
        "showdown:gen9championsrandomdoublesbattle:015": (
            "adapted-set",
            "The Champions generator validates Rhyperior as a heavy doubles attacker. Tabitha removes Trick Room and setup, using Assault Vest and four direct attacks.",
        ),
        "showdown:gen7randomdoublesbattle:012": (
            "adapted-set",
            "The generated Machamp set validates wide physical coverage. Tabitha upgrades custom Mega Machamp into the final assembly worker with no confusion or No Guard dependency in its moves.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current sand-and-snow mixture has no single Tabitha identity. The new team is a prototype assembly line: unactivated Coalossal, gears, magnets, an electrical controller, heavy chassis, and one Mega laborer.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_COALOSSAL",
            "level_offset": 1,
            "item": "ITEM_AIR_BALLOON",
            "ability": "ABILITY_STEAM_ENGINE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_BODY_PRESS", "MOVE_PROTECT"],
            "role": "Prototype engine: Steam Engine is visible but has no allied Water trigger, so it must attack and may only accelerate if the player chooses that interaction.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_KLINKLANG",
            "level_offset": 1,
            "item": "ITEM_WHITE_HERB",
            "ability": "ABILITY_CLEAR_BODY",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_GEAR_GRIND", "MOVE_WILD_CHARGE", "MOVE_SHIFT_GEAR", "MOVE_PROTECT"],
            "role": "Visible gear train and the team's only self-boosting prototype; White Herb is finite and Clear Body keeps the setup question readable.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_MAGNEZONE",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SPECS",
            "ability": "ABILITY_ANALYTIC",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_THUNDERBOLT", "MOVE_FLASH_CANNON", "MOVE_VOLT_SWITCH", "MOVE_ELECTROWEB"],
            "role": "Choice-locked magnetic circuit that trades flexibility for immediate special pressure and can pass the board through Volt Switch.",
            "lead_group": "circuit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_ROTOM_HEAT",
            "level_offset": 2,
            "item": "ITEM_SITRUS_BERRY",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_OVERHEAT", "MOVE_VOLT_SWITCH", "MOVE_ELECTROWEB", "MOVE_WILL_O_WISP"],
            "role": "Electrical controller: changes speed, burns physical answers, pivots, and pays an explicit Special Attack cost for Overheat.",
            "lead_group": "circuit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_RHYPERIOR",
            "level_offset": 3,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_SOLID_ROCK",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE", "MOVE_ICE_PUNCH", "MOVE_FIRE_PUNCH"],
            "role": "Heavy chassis with four direct attacks; it does not require Trick Room, Weakness Policy, or an activation partner to matter.",
            "lead_group": "chassis-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_MACHAMP",
            "level_offset": 4,
            "item": "ITEM_MACHAMPITE",
            "ability": "ABILITY_GUTS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_CLOSE_COMBAT", "MOVE_ICE_PUNCH", "MOVE_THUNDER_PUNCH", "MOVE_BULLET_PUNCH"],
            "role": "Tabitha's sole Mega and final assembly worker: four accurate direct attacks, priority, and no Dynamic Punch confusion crutch.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "MT_CHIMNEY_TABITHA",
        "planning_tier": "faction_admin_midpoint",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Tabitha's Mt. Chimney battle immediately before Maxie at the three-Badge cap",
            "location": "MtChimney",
            "strict_cap": 40,
            "player_tools": [
                "Three Badges and all catch and progression tools through Meteor Falls and the cable car",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet and pre-summit Mega Stones",
                "Manual healing and party preparation before the summit sequence",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Tabitha uses one Mega Machamp. Coalossal remains an unactivated prototype; Mega Excadrill and the Surf ignition are reserved for Magma Hideout.",
            "evolution_phase": "Midgame boss phase at levels 41-44: fully evolved and single-stage machinery is appropriate for an admin.",
            "preparation_access": "The player may prepare before the summit; confirm the native script permits healing between Tabitha and Maxie before source implementation claims independence.",
            "gauntlet_position": "The prototype machinery exam before Maxie's ridge battle. It must foreshadow, not duplicate, Tabitha's later self-activation machine.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "mt_chimney_tabitha", "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "magma_hideout_tabitha", "trainer_ids": ["TRAINER_TABITHA_MAGMA_HIDEOUT"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
                {"variant_id": "mossdeep_multi", "trainer_ids": ["TRAINER_TABITHA_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_TabithaMtChimney",
                "src/data/trainers.h:TRAINER_TABITHA_MT_CHIMNEY",
                "data/maps/MtChimney/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached Mt. Chimney, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["MT_CHIMNEY_MAXIE", "LAVARIDGE_GYM_FLANNERY", "MAGMA_HIDEOUT_TABITHA", "METEOR_FALLS_COURTNEY"],
            "required_preimplementation_review": "Refresh the last ten volcanic battles and exact Tabitha-to-Maxie healing rules. Preserve unactivated Coalossal, assembly-line circuits, heavy chassis, and Mega Machamp unless those interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Tabitha unveils the prototype: Coalossal is the boiler, Klinklang the gears, Magnezone the circuit, Rotom the controller, Rhyperior the chassis, and Mega Machamp the laborer.",
            "story_fit": "Before Tabitha perfects self-activation, the summit team is an assembly line of independent machinery. It demonstrates engineering progress without pretending the final ignition already exists.",
            "primary_player_question": "Can the player stop Klinklang's one Shift Gear and exploit Magnezone's Choice lock while navigating Electroweb, Volt Switch, burn, and heavy mixed machinery before Mega Machamp applies four direct punches?",
            "primary_mode": "Coalossal and Klinklang expose the boiler and gear train, but Coalossal has no allied activation and Klinklang is the only setup user.",
            "secondary_mode": "Magnezone and Rotom form a pivoting electrical circuit, Rhyperior is the unassisted chassis, and Mega Machamp is the direct final laborer.",
            "preview_pressure": "The roster reads as a machine from preview. Coalossal's lack of Weakness Policy and allied Water signals that this is a prototype, not the later tournament ignition.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 41 through 44 against cap 40 with one setup gear, Choice special pressure, two speed-control pivots, burn, heavy four-attack chassis, and a priority Mega. No self-activation, weather, sleep, redirection, recovery loop, or second setup engine exists.",
            "pressure_sources": [
                "Air Balloon Coalossal mixed Fire/Rock/Fighting pressure",
                "White Herb Klinklang Shift Gear and dual attacks",
                "Choice Specs Analytic Magnezone circuit pressure",
                "Rotom-Heat Electroweb, pivot, burn, and Overheat",
                "Assault Vest Solid Rock Rhyperior four-attack chassis",
                "Mega Machamp Fighting, Ice, Electric, and priority coverage",
            ],
            "resource_tax": "The fight taxes setup denial, Choice-lock exploitation, speed control, Ground and Fighting pressure, burn management, mixed bulk, and enough Psychic/Fairy/Flying/Ghost offense for Mega Machamp.",
            "tuning_order": [
                "Preserve prototype-versus-final progression, one setup gear, electrical circuit, chassis, and Mega laborer",
                "Validate Shift Gear, Choice lock, Electroweb, Volt Switch, and Mega timing before changing sets",
                "Adjust offsets within +1 to +4, beginning with Machamp, Rhyperior, and Magnezone",
                "Then adjust Klinklang or Coalossal survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_COALOSSAL", "SPECIES_KLINKLANG"],
            "mandatory_order_reason": "The lead exposes the prototype boiler and gears. Electrical, chassis, and labor roles are board-state reserves rather than scripted pairs.",
            "reserve_sequence": [
                "Use Magnezone when its public Choice move and Analytic pressure create a favorable circuit line.",
                "Use Rotom-Heat when speed control, burn, immunity, or Volt Switch improves the visible board.",
                "Use Rhyperior when its direct Ground/Rock/Ice/Fire chassis coverage is correct without requiring speed reversal.",
                "Preserve Mega Machamp as final laborer when practical, but deploy it earlier if its coverage or priority is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Use Shift Gear only when Klinklang survives the visible turn and the speed or damage boost creates a real next-turn line.",
                "Respect Magnezone's Choice lock and use Volt Switch only when the reserve improves the board.",
                "Use Electroweb from actual speed relationships and avoid redundant dual speed drops when direct damage wins.",
                "Account for Rotom's Levitate and Coalossal's Air Balloon without manufacturing ally spread interactions.",
                "Mega Evolve Machamp normally and choose direct coverage; no Dynamic Punch confusion plan is allowed.",
            ],
            "forbidden_behaviors": [
                "Do not self-activate Coalossal, equip Weakness Policy, or import the later Surf ignition.",
                "Do not violate Choice lock, Shift Gear into a visible knockout, or spam Electroweb blindly.",
                "Do not use sleep, confusion dependency, hidden information, or recovery loops.",
                "Do not add Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A exposes boiler and gear prototype. State B selects Magnezone or Rotom as the circuit. State C deploys Rhyperior as chassis. State D exposes Mega Machamp as laborer. Every state has direct-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Taunt, Haze, Unaware, phazing, priority, Fighting/Ground/Fire, or immediate pressure can deny Klinklang's single setup.",
                "Exploit Magnezone's Choice lock with Ground, immunity, resist pivots, Protect, and forced target changes.",
                "Use Ground carefully around Levitate and Balloon, remove speed drops, and punish Rotom's Overheat Special Attack loss.",
                "Break Rhyperior with Water/Grass/Fighting/Ground special pressure while respecting Solid Rock and Assault Vest.",
                "Use Psychic/Fairy/Flying/Ghost, Intimidate, burn, physical bulk, priority, or speed control against Mega Machamp's no-Protect finale.",
            ],
            "intentional_weakness": "Coalossal is not self-activated; Klinklang is the only setup; Magnezone is Choice-locked; Rotom pays Overheat drops; Rhyperior is slow; Mega Machamp has no Protect or recovery. The team shares real Ground/Fighting pressure and has no redirection or healing loop.",
            "first_loss_lesson": "This is the prototype, not the finished machine. Stop the one gear boost, force the circuit into a bad lock, exploit Overheat's cost, crack the chassis specially, and save a clean type or speed answer for Machamp.",
            "revealed_information": [
                "Air Balloon, Steam Engine, Shift Gear, White Herb, Choice lock, Electroweb, Volt Switch, Overheat drops, Levitate, and Mega evolution are public state.",
                "No allied Water move or Weakness Policy exists in this roster.",
                "Machamp's moves do not rely on confusion or hidden accuracy manipulation.",
                "Mega Machamp is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Coalossal receives an unearned activation",
                "Klinklang setup ignores survival",
                "Magnezone violates Choice lock",
                "Electroweb loops without speed value",
                "The prototype duplicates Tabitha's later Surf machine",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Coalossal doubles without activation", "Klinklang doubles", "Magnezone tournament doubles", "Rotom Heat champion", "Rhyperior Champions doubles", "Mega Machamp doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Coalossal Surf activation, Weakness Policy, Dragapult/Inteleon ignition, and Mega Excadrill are reserved for final Tabitha.",
                "Sand, snow, Trick Room, sleep, and confusion dependence are removed from the current team.",
                "Complete tournament teams, Dynamax, and Gigantamax are not imported.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Generated unactivated Coalossal and Klinklang roles",
                "Tournament-proven Magnezone and Rotom-Heat legitimacy",
                "Champions Rhyperior heavy attacker",
                "Generated Machamp coverage adapted into Tabitha's sole Mega",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Tabitha's Mt. Chimney prototype assembly line",
                "Unactivated Coalossal as recurring engine foreshadowing",
                "Klinklang's sole Shift Gear",
                "Magnezone-Rotom electrical circuit",
                "Mega Machamp as prototype laborer",
            ],
            "preserves": [
                "Final Tabitha's one Surf activation, Gastrodon safety, Stakataka chassis, Darmanitan piston, and Mega Excadrill",
                "Maxie's ridge and land identities and Courtney's calibration",
                "Other machinery teams if they do not duplicate the full prototype sequence",
                "Shelly's Weather Institute forecasting exam",
            ],
            "releases": [
                "Gigalith, Steelix, Gliscor, Glalie, and Aurorus leave Mt. Chimney Tabitha",
                "Other Steel, Rock, and Electric species remain available outside this assembly-line combination",
            ],
            "collision_notes": [
                "Coalossal is an intentional same-character reprise: unactivated prototype here, Surf-activated engine later.",
                "No other species overlaps the protected Gym, League, or designed faction anchors.",
                "Mega Machamp is unique here and Mega Excadrill remains the later completed machine.",
            ],
        },
        "presentation": {
            "intro_concept": "Tabitha calls this his prototype line and promises every part can crush the player even before the final ignition is installed.",
            "defeat_concept": "He concedes that the player found the weak coupling between gears, circuit, chassis, and labor.",
            "post_battle_concept": "Native summit progression remains unchanged; his loss motivates the more sophisticated Magma Hideout machine.",
            "hint_concept": "Nearby Magma dialogue says the boiler has no coolant yet, the gears are the only setup, the magnets commit to one move, and the final worker punches cleanly rather than confusing foes.",
            "native_width_status": "concept-only; exact intro, defeat, surrounding faction, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 40, unactivated Coalossal-Klinklang prototype, Magnezone-Rotom circuit, Rhyperior chassis, Mega Machamp laborer, explicit no-self-activation rule, AI predicates, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "The same Coalossal tells a progression story: the prototype can be dangerous on its own here, then Tabitha later installs the famous ignition.",
            "weakest_link": "Six mechanical-looking species can still feel like a visual theme rather than a battle. Shift Gear, Choice commitment, speed circuit, Overheat cost, chassis, and direct Mega coverage must remain mechanically distinct.",
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
                "The source guide places Mt. Chimney Tabitha at strict cap 40 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Machampite maps Machamp to Mega Machamp and no other transformation item appears.",
                "All six references exist across tournament, generated, and Champions sources.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Confirm exact native healing/menu access between Tabitha and Maxie, then replace sParty_TabithaMtChimney.",
                "Add partner, HP, speed, field, and combo flags and implement prototype reserve scoring.",
                "Regression-test no allied activation, Shift Gear survival, Choice lock, Electroweb, Volt Switch, Overheat drops, Air Balloon, Levitate, Mega timing, and simultaneous replacements.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-40 setup denial, Ground/Fighting/Water/Grass, Choice exploitation, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def source_closed_mt_chimney_tabitha_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    """Reconcile the soft backward anchor with the reached chronological context and source."""
    dossier = mt_chimney_tabitha_design(meta, records, source)
    selected = {
        "showdown:gen8randomdoublesbattle:005": (
            "selected-set",
            "The generated Coalossal set validates an independent doubles engine. Mt. Chimney exposes Steam Engine but supplies no allied Water trigger or Weakness Policy.",
        ),
        "showdown:gen6randomdoublesbattle:024": (
            "selected-set",
            "The generated Klinklang set validates Shift Gear and Gear Grind as the prototype's one setup clock.",
        ),
        "showdown:gen6randombattle:025": (
            "selected-set",
            "The generated Electivire set validates Motor Drive and Expert Belt Electric/Ice/Fire/Fighting coverage as the prototype controller.",
        ),
        "showdown:gen8randombattle:014": (
            "adapted-set",
            "The generated Xurkitree set validates Choice Specs electrical pressure and Volt Switch. Tabitha imports no speed field or unrelated roster.",
        ),
        "showdown:gen9championsrandomdoublesbattle:015": (
            "adapted-set",
            "The Champions generator validates Rhyperior as a heavy doubles attacker. Tabitha uses Assault Vest and four direct attacks without Trick Room or setup.",
        ),
        "showdown:gen7randomdoublesbattle:012": (
            "adapted-set",
            "The generated Machamp set validates direct Fighting coverage. Custom Mega Machamp is the final worker without confusion dependence.",
        ),
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_COALOSSAL",
            "level_offset": 1,
            "item": "ITEM_AIR_BALLOON",
            "ability": "ABILITY_STEAM_ENGINE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_BODY_PRESS", "MOVE_PROTECT"],
            "role": "Prototype boiler: Steam Engine is visible but no allied Water trigger or Weakness Policy exists, so Coalossal must attack independently.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_KLINKLANG",
            "level_offset": 1,
            "item": "ITEM_WHITE_HERB",
            "ability": "ABILITY_CLEAR_BODY",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_GEAR_GRIND", "MOVE_WILD_CHARGE", "MOVE_SHIFT_GEAR", "MOVE_PROTECT"],
            "role": "Gear train and sole setup user; Shift Gear is one visible survival-dependent clock rather than a second field engine.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_ELECTIVIRE",
            "level_offset": 2,
            "item": "ITEM_EXPERT_BELT",
            "ability": "ABILITY_MOTOR_DRIVE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_WILD_CHARGE", "MOVE_ICE_PUNCH", "MOVE_FIRE_PUNCH", "MOVE_CROSS_CHOP"],
            "role": "Prototype motor controller: Motor Drive punishes careless Electric targeting and Expert Belt rewards four physical coverage types.",
            "lead_group": "circuit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_XURKITREE",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SPECS",
            "ability": "ABILITY_BEAST_BOOST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDERBOLT", "MOVE_VOLT_SWITCH", "MOVE_DAZZLING_GLEAM", "MOVE_ENERGY_BALL"],
            "role": "Choice-locked power supply: immense special pressure and public commitment, with Volt Switch as the only circuit handoff.",
            "lead_group": "circuit-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_RHYPERIOR",
            "level_offset": 3,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_SOLID_ROCK",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_STONE_EDGE", "MOVE_ICE_PUNCH", "MOVE_FIRE_PUNCH"],
            "role": "Heavy chassis with four direct attacks and no Trick Room, spread Rock repetition, setup, or activation partner.",
            "lead_group": "chassis-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_MACHAMP",
            "level_offset": 4,
            "item": "ITEM_MACHAMPITE",
            "ability": "ABILITY_GUTS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_CLOSE_COMBAT", "MOVE_ICE_PUNCH", "MOVE_THUNDER_PUNCH", "MOVE_BULLET_PUNCH"],
            "role": "Sole Mega and final assembly worker: four direct accurate attacks, priority, and no confusion or recovery crutch.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    dossier["status"] = {"design": "design-complete", "source": "source-closed", "static": "design-validated", "runtime": "unplayed"}
    dossier["campaign_state"]["preparation_access"] = (
        "The Tabitha script returns to field control after its post-battle message; Maxie is a separate object interaction five map rows later, so Bag healing and menu preparation remain available between bosses."
    )
    dossier["runtime"]["variants"][0]["scope"] = "source-closed"
    dossier["runtime"]["current_source_baseline"] = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "implementation_note": "The reached rolling context replaced recent Magnezone/Rotom-Heat with fresh Electivire/Xurkitree and preserved Genesect for Noland.",
    }
    dossier["rolling_context"] = {
        "available": True,
        "reason": "The chronological ledger reached Tabitha after source-closing Battles 99-108 and proving the Mt. Chimney path.",
        "previous_encounters": [f"BATTLE_{index:03d}" for index in range(99, 109)],
        "protected_neighbor_anchors": ["MT_CHIMNEY_MAXIE", "LAVARIDGE_GYM_FLANNERY", "MAGMA_HIDEOUT_TABITHA", "METEOR_FALLS_COURTNEY"],
        "required_preimplementation_review": "Complete: Magnezone and Rotom-Heat collided at Battles 104 and 102 and were replaced; Coalossal, Klinklang, Rhyperior, Mega Machamp, prototype progression, and Maxie/Flannery reservations remain intact.",
    }
    dossier["identity"] = {
        "memory_hook": "Tabitha unveils the prototype: Coalossal is the boiler, Klinklang the gears, Electivire drives the motors, Xurkitree powers the circuit, Rhyperior is the chassis, and Mega Machamp is the laborer.",
        "story_fit": "The summit machine works independently but lacks its final ignition. Electivire and Xurkitree make the prototype feel electrical and technical without stealing Noland's Genesect or pretending the later Surf engine exists.",
        "primary_player_question": "Can the player stop Klinklang's one Shift Gear, exploit Xurkitree's Choice lock, and contain Electivire's coverage before Rhyperior and Mega Machamp turn the prototype into direct force?",
        "primary_mode": "Coalossal and Klinklang expose boiler and gears; Coalossal has no allied activation and Klinklang is the only setup user.",
        "secondary_mode": "Electivire and Xurkitree are physical and special circuit halves, Rhyperior is an unassisted chassis, and Mega Machamp is the final worker.",
        "preview_pressure": "The machine is legible from preview. Coalossal visibly lacks Weakness Policy and allied Water; Xurkitree visibly commits to one move; Mega Machamp is the only transformation.",
    }
    dossier["difficulty"] = {
        "target": 10,
        "observed": None,
        "rationale": "Hard uses levels 41-44 against cap 40 with one setup clock, Expert Belt Motor Drive coverage, Choice Specs Xurkitree, Volt Switch, Solid Rock Assault Vest chassis, and a priority Mega. No self-activation, weather, speed field, sleep, redirection, recovery loop, or second setup engine exists.",
        "pressure_sources": [
            "Air Balloon Coalossal mixed Fire/Rock/Fighting pressure",
            "White Herb Klinklang Shift Gear and dual attacks",
            "Expert Belt Motor Drive Electivire physical coverage",
            "Choice Specs Beast Boost Xurkitree coverage and Volt Switch",
            "Assault Vest Solid Rock Rhyperior four-attack chassis",
            "Mega Machamp Fighting, Ice, Electric, and priority coverage",
        ],
        "resource_tax": "The fight taxes setup denial, Choice exploitation, Ground/Fighting/Water/Grass pressure, pivot tracking, mixed bulk, and enough Psychic/Fairy/Flying/Ghost offense for Mega Machamp.",
        "tuning_order": [
            "Preserve prototype progression, sole setup gear, rare circuit, chassis, and Mega laborer",
            "Validate no Coalossal activation, Shift Gear survival, Choice lock, Motor Drive, recoil, pivoting, and Mega timing",
            "Adjust Machamp +4 first, then Rhyperior +3 and circuit +2 offsets",
            "Then adjust Klinklang or Coalossal survivability",
            "Change roles only after Hard/Medium/Easy runtime tests",
        ],
    }
    dossier["team"] = team
    dossier["ordering"] = {
        "intended_lead": ["SPECIES_COALOSSAL", "SPECIES_KLINKLANG"],
        "mandatory_order_reason": "Boiler and gears make the unfinished prototype public. Circuit, chassis, and labor roles remain board-state reserves.",
        "reserve_sequence": [
            "Use Electivire when Expert Belt coverage and Motor Drive pressure create the best physical circuit line.",
            "Use Xurkitree when a public Choice attack or Volt Switch can power the board without donating a bad lock.",
            "Use Rhyperior when direct chassis coverage is correct without speed reversal.",
            "Preserve Mega Machamp as final worker when practical, but deploy it earlier when priority or coverage is uniquely correct.",
        ],
    }
    dossier["ai"] = {
        "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE"],
        "required_flags": [],
        "custom_requirements": [
            "Use Shift Gear only when Klinklang survives and the boost creates a real next-turn line.",
            "Respect Xurkitree's Choice lock and use Volt Switch only when a reserve improves the board.",
            "Score Electivire's Motor Drive immunity, recoil, and four coverage types from the visible matchup.",
            "Account for Coalossal's Air Balloon without manufacturing an allied Water activation.",
            "Mega Evolve Machamp normally and choose direct coverage without confusion dependence.",
        ],
        "forbidden_behaviors": [
            "Do not self-activate Coalossal, equip Weakness Policy, or import the later Surf ignition.",
            "Do not violate Choice lock or Shift Gear into a visible knockout.",
            "Do not add a speed field, sleep, hidden information, or a recovery loop.",
            "Do not add Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
        ],
        "state_machine": "State A exposes boiler and gears. State B chooses Electivire or Xurkitree as the circuit. State C deploys Rhyperior as chassis. State D exposes Mega Machamp as laborer. Every state retains direct attacks and missing-partner fallbacks.",
    }
    dossier["counterplay"] = {
        "classes": [
            "Taunt, Haze, Unaware, phazing, priority, Fighting/Ground/Fire, or focus can deny Klinklang's one setup.",
            "Exploit Xurkitree's Choice lock with Ground, immunity, resist pivots, Protect, and forced targets.",
            "Exploit Electivire's ordinary bulk and Wild Charge recoil while respecting Motor Drive and coverage.",
            "Break Rhyperior with Water/Grass/Fighting/Ground special pressure through Solid Rock and Assault Vest.",
            "Use Psychic/Fairy/Flying/Ghost, Intimidate, burn, physical bulk, priority, or speed control against Mega Machamp.",
        ],
        "intentional_weakness": "Coalossal is unactivated; Klinklang is the only setup; Xurkitree is Choice-locked; Electivire takes recoil and has ordinary bulk; Rhyperior is slow; Mega Machamp has no Protect or recovery. Ground and Fighting pressure remain real and no redirection/healing loop exists.",
        "first_loss_lesson": "This is an unfinished machine. Stop the one gear boost, force the power supply into a bad lock, contain Electivire's coverage, crack the chassis specially, and preserve a clean answer for Machamp.",
        "revealed_information": [
            "Air Balloon, Steam Engine, Shift Gear, White Herb, Motor Drive, Choice lock, Volt Switch, Beast Boost, Solid Rock, and Mega evolution are public.",
            "No allied Water move or Weakness Policy exists.",
            "No speed field or circuit status loop exists.",
            "Mega Machamp is the only transformation.",
        ],
        "unacceptable_failure_modes": [
            "Coalossal receives an unearned activation",
            "Klinklang setup ignores survival",
            "Xurkitree violates Choice lock",
            "Electivire ignores Motor Drive immunity or its recoil cost",
            "The prototype duplicates Tabitha's later Surf machine",
        ],
    }
    dossier["competitive_research"] = {
        "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
        "queries": ["Coalossal doubles without activation", "Klinklang doubles", "Electivire Expert Belt", "Xurkitree Choice", "Rhyperior Champions doubles", "Mega Machamp doubles"],
        "candidates": [reference_digest(records[ref], *decision) for ref, decision in selected.items()],
        "selected_reference_ids": list(selected),
        "rejected_gimmicks": [
            "Coalossal Surf activation, Weakness Policy, and Mega Excadrill remain final-Tabitha mechanics.",
            "Recent Magnezone and Rotom-Heat collisions are removed from the soft anchor.",
            "Sand, snow, Trick Room, sleep, speed fields, Dynamax, and Gigantamax are not imported.",
            "Genesect remains reserved for Noland and Mega Machamp is the only transformation.",
        ],
        "imported_elements": [
            "Generated unactivated Coalossal and sole-setup Klinklang roles",
            "Generated Choice Specs Xurkitree commitment",
            "Generated Motor Drive Electivire physical circuit",
            "Champions Rhyperior chassis and generated Machamp coverage",
        ],
    }
    dossier["campaign_reservations"] = {
        "spends": [
            "Tabitha's Mt. Chimney prototype assembly line",
            "Unactivated Coalossal recurring-engine foreshadowing",
            "Klinklang's sole Shift Gear",
            "Electivire-Xurkitree physical/special circuit",
            "Mega Machamp prototype laborer",
        ],
        "preserves": [
            "Final Tabitha's Surf activation, Gastrodon safety, Stakataka chassis, Darmanitan piston, and Mega Excadrill",
            "Maxie's Groudon/Crobat ridge and Flannery's Torkoal timing",
            "Magnezone and Rotom-Heat remain tied to their recent Route 114 roles rather than immediate repetition",
            "Other machinery teams only if they do not duplicate this prototype sequence",
        ],
        "releases": [
            "Gigalith, Darmanitan, Steelix, Gliscor, Glalie, Aurorus, Magnezone, and Rotom-Heat leave this source battle",
            "Other Steel, Rock, Electric, and artificial species remain available outside this exact assembly line",
        ],
        "collision_notes": [
            "Coalossal is an intentional same-character reprise: independent prototype here, Surf-activated engine later.",
            "Electivire and Xurkitree replace recent collisions, and Genesect remains exclusive to Noland.",
            "Mega Machamp remains unique here and Mega Excadrill remains the completed machine.",
        ],
    }
    dossier["presentation"] = {
        "intro_concept": "Source text names boiler, gears, data reader, circuit, chassis, and laborer in order.",
        "defeat_concept": "Tabitha recognizes that the player found the weak coupling.",
        "post_battle_concept": "Tabitha admits the prototype lacks an ignition while urging Maxie to use the Meteorite.",
        "hint_concept": "The visible roster shows no allied Water, one setup gear, one Choice power supply, and one Mega worker.",
        "native_width_status": "pass; intro, defeat, post-battle, and refusal text are source-implemented and fit the native 36-character line gate",
        "guide_summary": "Document cap 40, unactivated Coalossal/Klinklang lead, Electivire-Xurkitree circuit, Rhyperior chassis, Mega Machamp laborer, exact no-self-activation rule, counterplay, and live difficulty offsets.",
    }
    dossier["author_self_check"] = {
        "strongest_part": "The same Coalossal tells a clean progression story, while fresh Electivire/Xurkitree avoid recent and protected collisions.",
        "weakest_link": "The machine can still read as six strong parts. Shift Gear, Motor Drive coverage, Choice commitment, Beast Boost, chassis bulk, and direct Mega coverage must remain distinct in AI and guide text.",
    }
    dossier["verification"] = {
        "design_schema": "pass",
        "species_items_moves_abilities": "pass",
        "source_implementation": "pass",
        "script_and_format": "pass",
        "dialogue_width": "pass",
        "guide": "pass",
        "runtime": "unplayed",
        "observed_difficulty": None,
        "evidence": [
            "Source exactly matches the six team entries at cap-relative +1,+1,+2,+2,+3,+4.",
            "The required script is a guarded double and returns to field control before separate Maxie interaction.",
            "Every move, item, ability slot, spread, and Mega pairing is locally legal.",
            "Six indexed references cover every exact role and Genesect remains exclusive to Noland.",
            "Rolling context and campaign anchors contain no unwaived collision.",
        ],
        "source_blockers": [
            "Run cap-40 setup denial, Ground/Fighting/Water/Grass, Choice exploitation, fast, slow, and Hard/Medium/Easy real-ROM tests before observed difficulty is recorded.",
        ],
    }
    return dossier


def weather_institute_shelly_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen4randomdoublesbattle:003": (
            "selected-role",
            "The generated roster validates Castform and Machamp at doubles stakes. Shelly takes only Forecast legitimacy; Machamp remains Tabitha's separate Mega laborer.",
        ),
        "vgc:regional-baltimore-2025": (
            "adapted-history",
            "The Baltimore-winning rain roster validates manually prepared rain offense as a major competitive structure. Shelly replaces the automatic Pelipper engine with vulnerable Castform Rain Dance.",
        ),
        "vgc:regional-portland-2024": (
            "adapted-role",
            "The Portland-winning roster validates Raikou as elite doubles pressure. Shelly uses Assault Vest Thunder, Snarl, Aura Sphere, and Volt Switch without importing the full balance team.",
        ),
        "vgc:regional-dallas-tx-2020": (
            "adapted-role",
            "The Dallas-winning weather-era roster validates a bulky special Water/Dragon slot. Shelly uses Hydration Goodra as the rain specimen without Trick Room or recovery.",
        ),
        "showdown:gen7randomdoublesbattle:014": (
            "adapted-set",
            "The generated Scizor roster validates immediate Technician pressure. Shelly uses rain's Fire reduction to support a four-move physical pivot.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "reason_for_replacement": "The current team prematurely duplicates Shelly's later snow screen. The Weather Institute should instead be a vulnerable forecasting experiment: Castform must create rain manually, and every other specimen demonstrates a different consequence.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_CASTFORM",
            "level_offset": 1,
            "item": "ITEM_DAMP_ROCK",
            "ability": "ABILITY_FORECAST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_RAIN_DANCE", "MOVE_WEATHER_BALL", "MOVE_THUNDER", "MOVE_PROTECT"],
            "role": "The exposed experiment: must spend a legal turn creating rain, visibly changes form, and remains capable of direct Weather Ball or Thunder pressure.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_LUDICOLO",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_SWIFT_SWIM",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_FAKE_OUT", "MOVE_HYDRO_PUMP", "MOVE_GIGA_DRAIN", "MOVE_ICE_BEAM"],
            "role": "Rain-speed specimen whose Fake Out may buy Castform's experiment one turn, but whose frailty and missing Protect expose the plan.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_RAIKOU",
            "level_offset": 2,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_INNER_FOCUS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDER", "MOVE_AURA_SPHERE", "MOVE_SNARL", "MOVE_VOLT_SWITCH"],
            "role": "Rare lightning specimen that gains perfect Thunder while retaining Snarl, Fighting coverage, and a pivot outside rain.",
            "lead_group": "rain-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_GOODRA",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_HYDRATION",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_MUDDY_WATER", "MOVE_DRAGON_PULSE", "MOVE_THUNDER", "MOVE_PROTECT"],
            "role": "Hydration specimen and bulky special attacker; no Rest or recovery loop is present, so weather changes offense and status resilience rather than stalling.",
            "lead_group": "rain-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_SCIZOR",
            "level_offset": 3,
            "item": "ITEM_EXPERT_BELT",
            "ability": "ABILITY_TECHNICIAN",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_BULLET_PUNCH", "MOVE_U_TURN", "MOVE_SUPERPOWER", "MOVE_PROTECT"],
            "role": "Rain-shielded physical specimen: reduced Fire pressure and Technician priority create value without setup, healing, or a second weather mode.",
            "lead_group": "rain-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_KINGLER",
            "level_offset": 4,
            "item": "ITEM_KINGLERITE",
            "ability": "ABILITY_HYPER_CUTTER",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_CRABHAMMER", "MOVE_HIGH_HORSEPOWER", "MOVE_X_SCISSOR", "MOVE_PROTECT"],
            "role": "Shelly's sole Mega and final pressure specimen: rain-amplified Crabhammer plus Ground and Bug coverage with no setup or recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "WEATHER_INSTITUTE_SHELLY",
        "planning_tier": "faction_admin_midpoint",
        "status": {"design": "design-complete", "source": "unimplemented", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Shelly's Weather Institute battle after Norman and before Fortree",
            "location": "Route119_WeatherInstitute_2F",
            "strict_cap": 55,
            "player_tools": [
                "Five Badges and all catches and progression tools through Route 119",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet and all campaign Mega Stones earned before the Institute",
                "Manual healing and party preparation before the second-floor confrontation",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Shelly uses one Mega Kingler and no Primal. Automatic rain and the later Mega Lapras snow phase remain protected.",
            "evolution_phase": "Mid-late campaign boss phase: fully evolved, legendary, and one custom Mega are appropriate.",
            "preparation_access": "The player can prepare before Shelly; this is not a no-menu party lock.",
            "gauntlet_position": "Aqua's vulnerable weather experiment. It must make the Institute itself matter while reserving Shelly's later snow phase and Archie's Primal rain.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_SHELLY_WEATHER_INSTITUTE"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "weather_institute_shelly", "trainer_ids": ["TRAINER_SHELLY_WEATHER_INSTITUTE"], "format": "double", "scope": "designed-here", "reachability": "required main story"},
                {"variant_id": "seafloor_cavern_shelly", "trainer_ids": ["TRAINER_SHELLY_SEAFLOOR_CAVERN"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_ShellyWeatherInstitute",
                "src/data/trainers.h:TRAINER_SHELLY_WEATHER_INSTITUTE",
                "data/maps/Route119_WeatherInstitute_2F/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": False,
            "reason": "The chronological physical ledger has not reached the Weather Institute, so an exact previous-ten window would be fabricated.",
            "previous_encounters": [],
            "protected_neighbor_anchors": ["ROUTE_119_RIVAL", "FORTREE_GYM_WINONA", "SEAFLOOR_CAVERN_SHELLY", "AQUA_HIDEOUT_MATT"],
            "required_preimplementation_review": "Refresh the last ten Route 119 and Institute battles. Preserve vulnerable Castform rain, five distinct specimens, and Mega Kingler unless those exact interactions cluster immediately beforehand.",
        },
        "identity": {
            "memory_hook": "Shelly starts an experiment: Castform predicts rain, Ludicolo proves speed, Raikou proves lightning, Goodra proves hydration, Scizor proves shielding, and Mega Kingler proves pressure.",
            "story_fit": "The Weather Institute becomes a real laboratory. Shelly does not arrive with automatic weather; she must create a forecast and demonstrate its consequences specimen by specimen.",
            "primary_player_question": "Can the player deny Castform's vulnerable Rain Dance through Ludicolo's Fake Out, then adapt as five different specimens turn the same forecast into speed, accuracy, hydration, Fire shielding, and Mega Water pressure?",
            "primary_mode": "Castform and Ludicolo openly attempt one manual Rain Dance turn protected only by Fake Out and Focus Sash, not an automatic weather ability.",
            "secondary_mode": "Raikou, Goodra, Scizor, and Mega Kingler each demonstrate a distinct rain consequence without a second weather setter or alternate mode.",
            "preview_pressure": "The team shows one humble Castform beside rare and powerful specimens. Stopping the experiment early is broad counterplay; letting it run creates a real boss.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 56 through 59 against cap 55 with a vulnerable manual weather turn, Fake Out, Swift Swim, perfect Thunder, Snarl, Hydration, rain-shielded Scizor, and one Mega. The whole engine can be denied, rain is finite, and there is no automatic setter, sleep, redirection, recovery loop, or secondary speed mode.",
            "pressure_sources": [
                "Damp Rock Castform manual Rain Dance, Forecast, Weather Ball, and Thunder",
                "Focus Sash Swift Swim Ludicolo Fake Out and three-type special pressure",
                "Assault Vest Raikou perfect Thunder, Snarl, Aura Sphere, and pivot",
                "Life Orb Hydration Goodra mixed Water/Dragon/Electric spread pressure",
                "Technician Scizor priority and rain-reduced Fire weakness",
                "Mega Kingler rain-amplified Crabhammer and direct coverage",
            ],
            "resource_tax": "The fight taxes Fake Out counterplay, manual weather denial, speed and accuracy control, mixed bulk, Electric/Grass/Fighting/Ground coverage, and enough physical control for Scizor and Mega Kingler.",
            "tuning_order": [
                "Preserve vulnerable experiment, distinct specimen lessons, and sole Mega Kingler climax",
                "Validate Forecast, manual Rain Dance, Swift Swim, Hydration, rain Fire reduction, and transformation timing before changing sets",
                "Adjust offsets within +1 to +4, beginning with Kingler, Goodra, and Raikou",
                "Then adjust Castform or Ludicolo survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_CASTFORM", "SPECIES_LUDICOLO"],
            "mandatory_order_reason": "The lead exposes the manual experiment. Every reserve is a specimen selected by board state, not a fixed paired module.",
            "reserve_sequence": [
                "Use Raikou when Thunder, Snarl, Aura Sphere, or Volt Switch best demonstrates the live forecast.",
                "Use Goodra when Hydration, bulk, or Water/Dragon/Electric pressure is matchup-correct.",
                "Use Scizor when priority and rain-reduced Fire pressure create the best physical line.",
                "Preserve Mega Kingler as the pressure specimen when practical, but deploy it earlier if its coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING"],
            "required_flags": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "custom_requirements": [
                "Use Rain Dance only when rain is absent, Castform survives the visible turn, and at least one active or reserve specimen gains meaningful value.",
                "Score Ludicolo Fake Out jointly with Castform and attack when weather is already active or Fake Out is invalid.",
                "Recompute Weather Ball type, Thunder accuracy, Swift Swim speed, Hydration, and Fire damage from actual weather each turn.",
                "Use Volt Switch and U-turn only when the reserve improves the board, and respect all Choice or item state.",
                "Mega Evolve Kingler normally and choose immediate coverage rather than inventing setup.",
            ],
            "forbidden_behaviors": [
                "Do not begin with automatic rain, cast Rain Dance while already active, or treat weather as permanent.",
                "Do not use hidden information, sleep, redirection, or recovery loops.",
                "Do not grant Swift Swim, perfect Thunder, Hydration, or Fire reduction outside actual rain.",
                "Do not add Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A attempts one vulnerable Castform forecast. State B selects the correct rain specimen among Raikou, Goodra, and Scizor. State C exposes Mega Kingler as pressure result. If rain is denied or ends, every member uses its legal non-weather fallback.",
        },
        "counterplay": {
            "classes": [
                "Inner Focus, Ghost, Protect, priority, Taunt, double-targeting, faster weather, or direct Castform pressure can deny the opening experiment.",
                "Replace or suppress rain, stall finite turns, use Cloud Nine, or exploit each specimen's ordinary non-weather speed and accuracy.",
                "Use Electric/Grass/Flying/Poison against Ludicolo, Ground against Raikou, and Fairy/Ice/Dragon against Goodra while accounting for coverage.",
                "Exploit Scizor with Fire once rain ends, burn, Intimidate, Rocky Helmet, or special pressure; it has no recovery.",
                "Use Electric/Grass, burn, Intimidate, physical bulk, priority, or speed control against Mega Kingler's direct no-setup finish.",
            ],
            "intentional_weakness": "Castform must spend a turn and is the only weather setter; Ludicolo is Sash-dependent; Raikou lacks Protect; Goodra has no recovery; Scizor relies on rain to soften Fire; Mega Kingler has no setup or recovery. The team has no redirection, sleep, or alternate weather.",
            "first_loss_lesson": "The entire lab starts with one forecast. Deny or outlast it if you can; if rain begins, identify which single benefit each specimen is using rather than treating them as interchangeable Water attackers.",
            "revealed_information": [
                "Rain turns, Forecast form, Weather Ball type, Swift Swim, Thunder accuracy, Hydration, Fire damage, item consumption, and Mega evolution are public state.",
                "Castform is the only weather setter and no automatic callback restores rain.",
                "Every specimen has a legal non-rain move line.",
                "Mega Kingler is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Rain starts automatically or refreshes redundantly",
                "Ludicolo uses invalid or redundant Fake Out",
                "Weather benefits persist after rain ends",
                "Pivot moves ignore board value",
                "Weather Institute Shelly duplicates her later snow phase or Archie's Primal rain",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Castform doubles", "manual rain tournament", "Raikou doubles", "Goodra rain", "Scizor doubles", "Mega Kingler doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Automatic Pelipper, Politoed, or Primal rain is removed so Castform's experiment remains vulnerable.",
                "Shelly's later snow, Aurora Veil, Slush Rush, Freeze-Dry, and Mega Lapras remain protected.",
                "Sleep, redirection, recovery loops, Trick Room, and full tournament shells are not imported.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Generated Castform Forecast legitimacy",
                "Tournament-proven rain structures adapted to manual setup",
                "Tournament Raikou and bulky Water/Dragon legitimacy",
                "Generated Scizor pressure",
                "Mega Kingler authored as immediate pressure rather than copied from an unsupported gimmick",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Shelly's Weather Institute Castform experiment",
                "Manual rain plus five distinct specimen consequences",
                "Raikou as rare lightning specimen",
                "Rain-shielded Scizor demonstration",
                "Mega Kingler as pressure result",
            ],
            "preserves": [
                "Final Shelly's automatic snow, Aurora Veil, Slush Rush, phase change, and Mega Lapras",
                "Archie's Primal Kyogre momentum and Matt's rain boarding party",
                "Juan's Surf relay and Wallace's dual-speed champion rain",
                "Other Castform or weather battles only if they ask a different question",
            ],
            "releases": [
                "Alolan Ninetales, Empoleon, Crawdaunt, Clefable, and Mega Beedrill leave Weather Institute Shelly",
                "Other rain species remain available outside the manual experiment sequence",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, or designed faction anchors.",
                "This is manual Castform rain; Shelly's later team uses snow and Archie alone owns Primal rain.",
                "Mega Kingler is unique here and Mega Lapras remains her later evolution in style rather than species.",
            ],
        },
        "presentation": {
            "intro_concept": "Shelly calls the stolen Institute data useless unless she can reproduce the forecast under battle pressure.",
            "defeat_concept": "She admits the player broke the experiment at its exposed first step and adapted to every surviving specimen.",
            "post_battle_concept": "Native Weather Institute progression remains unchanged; Shelly leaves with enough data to refine her later snow phase.",
            "hint_concept": "Institute staff warn that Castform must create rain manually, the dancer may buy one turn, each later specimen demonstrates one benefit, and the final crab simply hits harder.",
            "native_width_status": "concept-only; exact intro, defeat, Institute, and hint text require native font-width validation at implementation",
            "guide_summary": "Document cap 55, Castform-Ludicolo vulnerable manual rain, Raikou lightning, Hydration Goodra, rain-shielded Scizor, Mega Kingler finale, exact weather-state AI, broad counterplay, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Castform makes the map and research premise mechanically real, while every reserve demonstrates a different weather consequence rather than another generic rain sweeper.",
            "weakest_link": "Manual Rain Dance can make the boss collapse too easily if Castform is removed. That is intentional counterplay, but Hard offsets and every reserve's functional non-rain line must keep the fight above the game's floor.",
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
                "The source guide places Weather Institute Shelly at strict cap 55 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Kinglerite maps Kingler to Mega Kingler and no other transformation item appears.",
                "All five selected references exist across tournament and generated evidence.",
                "No source party, dialogue, or guide entry has been changed and no real-ROM battle has been run.",
            ],
            "source_blockers": [
                "Replace sParty_ShellyWeatherInstitute with the exact six sets and offsets.",
                "Add partner, HP, speed, field, and combo flags and implement forecast and specimen reserve scoring.",
                "Regression-test Forecast, manual Rain Dance, Weather Ball, Thunder, Swift Swim, Hydration, rain Fire reduction, pivot moves, Mega timing, and weather expiration.",
                "Write and font-measure exact dialogue and update the source-derived guide and reservations.",
                "Run cap-55 rain denial, faster weather, Cloud Nine, Electric/Grass/Ground/Fairy/Ice, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def meteor_falls_courtney_design(meta: dict, records: dict[str, dict], source: dict, grunt_source: dict) -> dict:
    selected = {
        "showdown:gen5randomdoublesbattle:021": (
            "selected-role",
            "The generated Jirachi roster validates it as a speed-control pivot at doubles stakes. Courtney uses Choice Scarf Iron Head, Icy Wind, U-turn, and Trick without importing healing or setup.",
        ),
        "showdown:gen8randomdoublesbattle:014": (
            "selected-role",
            "The generated Celesteela roster validates it as heavy mixed doubles pressure. Courtney removes Leech Seed and recovery, using Assault Vest and four attacks as the largest meteor fragment.",
        ),
        "showdown:gen9championsrandomdoublesbattle:005": (
            "adapted-set",
            "The Champions generator validates Aerodactyl as offensive speed and Rock pressure. Courtney reserves its Mega as the final impact and rejects the source team's sand, screens, and Tailwind shell.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party_by_owner": {
            "TRAINER_COURTNEY_METEOR_FALLS": [mon["species"] for mon in source["mons"]],
            "TRAINER_GRUNT_METEOR_FALLS": [mon["species"] for mon in grunt_source["mons"]],
        },
        "level_offsets_by_owner": {
            "TRAINER_COURTNEY_METEOR_FALLS": [mon["level_offset"] for mon in source["mons"]],
            "TRAINER_GRUNT_METEOR_FALLS": [mon["level_offset"] for mon in grunt_source["mons"]],
        },
        "format": "multi_2_vs_2",
        "quality_score": min(source["quality_score"], grunt_source["quality_score"]),
        "implementation_note": "The engine hard-caps each opposing owner at three in a two-opponent Multi Battle, so the protected six-member impact roster is source-partitioned across Courtney and her Grunt.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_SOLROCK",
            "level_offset": 1,
            "item": "ITEM_FOCUS_SASH",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_ZEN_HEADBUTT", "MOVE_WILL_O_WISP", "MOVE_EXPLOSION"],
            "role": "The first impact body: attacks, burns, and may detonate only under an explicit low-HP or protected-partner condition.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_LUNATONE",
            "level_offset": 1,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_LEVITATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_POWER_GEM", "MOVE_PSYCHIC", "MOVE_ICY_WIND", "MOVE_PROTECT"],
            "role": "The surviving twin: changes speed, supplies special Rock/Psychic pressure, and owns the lead's visible Protect against a conditional detonation.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_MINIOR",
            "level_offset": 2,
            "item": "ITEM_WHITE_HERB",
            "ability": "ABILITY_SHIELDS_DOWN",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_SHELL_SMASH", "MOVE_ACROBATICS", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
            "role": "Small meteor with one readable shell break; Shields Down and White Herb are finite public state rather than hidden durability.",
            "lead_group": "impact-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_JIRACHI",
            "level_offset": 2,
            "item": "ITEM_CHOICE_SCARF",
            "ability": "ABILITY_SERENE_GRACE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_IRON_HEAD", "MOVE_ICY_WIND", "MOVE_U_TURN", "MOVE_TRICK"],
            "role": "Mythical meteor pivot whose Choice commitment, speed control, and Serene Grace pressure remain public and exploitable.",
            "lead_group": "impact-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_CELESTEELA",
            "level_offset": 3,
            "item": "ITEM_ASSAULT_VEST",
            "ability": "ABILITY_BEAST_BOOST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HEAVY_SLAM", "MOVE_FLAMETHROWER", "MOVE_GIGA_DRAIN", "MOVE_ROCK_SLIDE"],
            "role": "The largest falling fragment: four immediate attacks, mixed coverage, and Beast Boost with no seed, recovery, or stall loop.",
            "lead_group": "heavy-impact-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_AERODACTYL",
            "level_offset": 4,
            "item": "ITEM_AERODACTYLITE",
            "ability": "ABILITY_ROCK_HEAD",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_DUAL_WINGBEAT", "MOVE_ICE_FANG", "MOVE_PROTECT"],
            "role": "Courtney's sole Mega and final impact fossil: immediate Rock/Flying/Ice pressure with no setup, weather, or recovery.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "METEOR_FALLS_COURTNEY",
        "planning_tier": "faction_admin_opening",
        "status": {"design": "design-complete", "source": "source-closed", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Courtney's Meteor Falls confrontation in the three-Badge chapter before Mt. Chimney",
            "location": "MeteorFalls_1F_1R",
            "strict_cap": 40,
            "player_tools": [
                "Three Badges and all catches and progression tools through Route 114 and Meteor Falls",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet and Mega Stones available before Meteor Falls",
                "Manual healing and party preparation before the story confrontation",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Courtney uses one Mega Aerodactyl and no Primal. Mega Houndoom and her calibration identity remain later progression.",
            "evolution_phase": "Midgame boss phase at levels 41-44: fully evolved, mythical, Ultra Beast, and one Mega are appropriate for a major admin puzzle.",
            "preparation_access": "The player may prepare and choose three party members before the required battle; the rival supplies a source-authored three-member partner party in every starter/gender branch.",
            "gauntlet_position": "Courtney's opening site-specific identity. Meteor impact and controlled detonation must precede, not duplicate, her later Victory Star calibration.",
            "mechanics_baseline_id": "faction_admin",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"],
            "canonical_format": "multi_2_vs_2",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "meteor_falls_courtney", "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"], "format": "multi_2_vs_2", "scope": "source-closed", "reachability": "six rival gender/starter branches in required main story"},
                {"variant_id": "magma_hideout_courtney", "trainer_ids": ["TRAINER_COURTNEY_MAGMA_HIDEOUT"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
                {"variant_id": "mossdeep_multi", "trainer_ids": ["TRAINER_COURTNEY_MOSSDEEP"], "format": "multi", "scope": "separate-coordinated-climax", "reachability": "later required multi battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Courtney_MeteorFalls",
                "src/data/trainer_parties.h:sParty_GruntMeteorFalls",
                "src/data/trainers.h:TRAINER_COURTNEY_METEOR_FALLS",
                "src/data/trainers.h:TRAINER_GRUNT_METEOR_FALLS",
                "data/maps/MeteorFalls_1F_1R/scripts.inc",
            ],
        },
        "rolling_context": {
            "available": True,
            "reason": "The chronological frontier reached Meteor Falls after source-closing all ten Route 114 encounters.",
            "previous_encounters": [f"BATTLE_{index:03d}" for index in range(95, 105)],
            "protected_neighbor_anchors": ["MT_CHIMNEY_TABITHA", "MT_CHIMNEY_MAXIE", "MAGMA_HIDEOUT_COURTNEY", "MOSSDEEP_GYM_TATE_AND_LIZA"],
            "required_preimplementation_review": "Complete: Route 114 spends no Solrock, Lunatone, Minior, Jirachi, Celesteela, Aerodactyl, controlled detonation, or impact sequence; the protected identity remains distinct.",
        },
        "identity": {
            "memory_hook": "Courtney stages an impact: Solrock may detonate beside protected Lunatone, Minior cracks open, Jirachi redirects the fall, Celesteela is the largest fragment, and Mega Aerodactyl is the fossil left behind.",
            "story_fit": "Meteor Falls finally matters. Courtney's first battle treats every team member as a different phase of impact rather than importing another generic Magma sun roster.",
            "primary_player_question": "Can the player read and punish one conditional Solrock detonation, stop Minior's shell break, exploit Jirachi's Choice lock, and preserve enough Rock/Steel/Water/Electric pressure for Celesteela and Mega Aerodactyl?",
            "primary_mode": "Solrock and Lunatone expose mixed Rock/Psychic pressure, Icy Wind, burn, Protect, and one controlled Explosion condition.",
            "secondary_mode": "Minior supplies a finite shell break, Jirachi a committed pivot, Celesteela a heavy fragment, and Mega Aerodactyl the immediate fossil impact.",
            "preview_pressure": "The site theme is unmistakable, but the battle does not borrow Tate and Liza's Psychic Terrain, Trick Room, or cosmic legendary formation.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 41 through 44 against cap 40 with mixed Rock/Psychic spread, burn, Icy Wind, a conditional detonation, Shell Smash, Choice Serene Grace pressure, Beast Boost, and one fast Mega. Weather, terrain, Trick Room, redirection, sleep, and recovery loops are absent.",
            "pressure_sources": [
                "Focus Sash Solrock Rock Slide, burn, and conditional Explosion",
                "Life Orb Lunatone special pressure and Icy Wind",
                "White Herb Minior Shell Smash and Shields Down",
                "Choice Scarf Jirachi Iron Head, Icy Wind, U-turn, and Trick",
                "Assault Vest Celesteela four-attack Beast Boost pressure",
                "Mega Aerodactyl fast Rock/Flying/Ice coverage",
            ],
            "resource_tax": "The fight taxes Protect timing, Ghost/Steel/Rock/Water/Electric answers, setup denial, Choice exploitation, mixed bulk, spread pressure, and enough priority or speed for Mega Aerodactyl.",
            "tuning_order": [
                "Preserve impact identity, controlled detonation, finite shell break, and Mega fossil",
                "Validate Explosion ally safety, Shell Smash survival, Choice lock, and reserve scoring before changing sets",
                "Adjust offsets within +1 to +4, beginning with Aerodactyl, Celesteela, and Minior",
                "Then adjust Solrock or Lunatone survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_LUNATONE", "SPECIES_SOLROCK"],
            "source_ownership": {
                "TRAINER_COURTNEY_METEOR_FALLS": ["SPECIES_LUNATONE", "SPECIES_JIRACHI", "SPECIES_AERODACTYL"],
                "TRAINER_GRUNT_METEOR_FALLS": ["SPECIES_SOLROCK", "SPECIES_MINIOR", "SPECIES_CELESTEELA"],
            },
            "mandatory_order_reason": "The lead exposes impact timing. Smaller meteor, mythical pivot, heavy fragment, and fossil are board-state reserves rather than scripted waves.",
            "reserve_sequence": [
                "Use Minior when one survivable Shell Smash creates real pressure; otherwise attack or Protect.",
                "Use Jirachi when Icy Wind, Choice pressure, U-turn, or Trick improves the visible board and honor its lock.",
                "Use Celesteela when heavy mixed coverage and Beast Boost create a direct line without passive stalling.",
                "Preserve Mega Aerodactyl as final impact when practical, but deploy it earlier if speed or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "required_flags": ["AI_FLAG_WILL_SUICIDE on the Grunt's Solrock owner"],
            "custom_requirements": [
                "Use Explosion only when Solrock is low enough or the damage wins a real board, and only when Lunatone or the active ally is protected, immune, or expendable for superior payoff.",
                "Use Lunatone Protect in coordination with a justified detonation, but attack or control speed when Explosion is wrong.",
                "Use Shell Smash only when Minior survives the visible turn and the boost improves a real next-turn line.",
                "Respect Jirachi's Choice lock and use Trick or U-turn only when the visible board improves.",
                "Mega Evolve Aerodactyl normally and use immediate coverage rather than inventing setup.",
            ],
            "forbidden_behaviors": [
                "Do not explode beside an unprotected valuable ally or on turn one by script.",
                "Do not Shell Smash into a visible knockout or violate Choice lock.",
                "Do not use sleep, hidden information, passive Celesteela recovery, or cosmic terrain/Trick Room.",
                "Do not add Primal, second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A establishes twin meteor impact and permits one justified detonation. State B selects Minior or Jirachi as break or trajectory control. State C deploys Celesteela as heavy fragment. State D exposes Mega Aerodactyl as fossil impact. Every state has independent-attack and missing-partner fallbacks.",
        },
        "counterplay": {
            "classes": [
                "Use Wide Guard, Ghost immunity, Protect, priority, double-targeting, or immediate Solrock pressure to deny or blunt Explosion and Rock Slide.",
                "Taunt, Haze, Unaware, phazing, priority, Steel/Water/Electric/Rock, or concentrated damage can stop Minior's one setup.",
                "Exploit Jirachi's public Choice lock with resist pivots, Ground/Fire/Ghost/Dark, Protect, and forced targets.",
                "Use Electric/Fire special pressure and avoid feeding Beast Boost through sacrificial weak targets against Celesteela.",
                "Use Water/Electric/Ice/Steel/Rock, Intimidate or burn, priority, or speed reversal against Mega Aerodactyl's no-recovery finish.",
            ],
            "intentional_weakness": "Solrock and Lunatone share Water/Grass/Ghost/Dark/Steel pressure; Minior is the only setup; Jirachi is Choice-locked; Celesteela lacks recovery; Mega Aerodactyl is frail. There is no weather, terrain, redirection, sleep, or sustain loop.",
            "first_loss_lesson": "Courtney's impact is timed, not random. Deny or survive the one detonation, stop Minior before it cracks open, force Jirachi into the wrong trajectory, and arrive at Aerodactyl with priority or speed intact.",
            "revealed_information": [
                "Current HP, Protect, Explosion targeting, White Herb, Shell Smash, Shields Down, Choice lock, Beast Boost, and Mega evolution are public state.",
                "Explosion and ally damage follow ordinary engine rules.",
                "No Psychic Terrain, Trick Room, or cosmic restricted pair exists.",
                "Mega Aerodactyl is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Explosion is scripted or destroys a valuable unprotected ally",
                "Minior sets up without survival value",
                "Jirachi violates Choice lock",
                "Celesteela becomes a passive stall piece",
                "Meteor Falls Courtney duplicates Tate and Liza or later Courtney",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Solrock Lunatone doubles", "Minior doubles", "Jirachi random doubles", "Celesteela doubles", "Mega Aerodactyl Champions doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Tate and Liza's Psychic Terrain, Trick Room, Cresselia, Calyrex, Solgaleo, and Lunala structure remains protected.",
                "Generic Magma sun, sleep, Leech Seed, recovery, sand, screens, and Tailwind are not imported.",
                "Courtney's later Victory Star, Chi-Yu, Glimmora-Metagross safe zone, and Mega Houndoom remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Generated Jirachi speed-control pivot",
                "Generated Celesteela heavy doubles legitimacy without stall",
                "Champions Aerodactyl pressure adapted into Courtney's sole Mega",
                "Locally legal Solrock, Lunatone, and Minior site-specific roles",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Courtney's Meteor Falls impact composition",
                "The campaign's one controlled Solrock detonation beside Lunatone",
                "Minior Shell Smash meteor reveal",
                "Jirachi and Celesteela as rare impact bodies",
                "Mega Aerodactyl as the fossil climax",
            ],
            "preserves": [
                "Later Courtney's calibrated sun, Chi-Yu commitment, mineral safe zone, and Mega Houndoom",
                "Tate and Liza's cosmic Psychic formations",
                "Maxie's Groudon land story and Tabitha's machinery",
                "Other meteor and fossil species outside this impact sequence",
            ],
            "releases": [
                "Ninetales, Krookodile, Houndoom, Volcanion, Darmanitan, and Shiftry leave Meteor Falls Courtney",
                "Other Rock, Psychic, Steel, and celestial teams remain available if they do not repeat the controlled impact arc",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, or designed faction anchors.",
                "The celestial visual theme overlaps Mossdeep only superficially; Courtney has no terrain, Trick Room, or cosmic restricted formation.",
                "Mega Aerodactyl appears only here and Mega Houndoom remains Courtney's later progression.",
            ],
        },
        "presentation": {
            "intro_concept": "Courtney says Meteor Falls provides perfect data: trajectory, collision, fracture, and the fossil left after impact.",
            "defeat_concept": "She records that the player survived the impact by changing its timing rather than absorbing it head-on.",
            "post_battle_concept": "Native story progression remains unchanged; this failed impact model leads Courtney toward later precision and safe-zone geometry.",
            "hint_concept": "Nearby Magma dialogue warns that the sun rock explodes only when the moon can shelter, the small meteor may crack itself open, the wish star commits to one move, and the fossil lands last.",
            "native_width_status": "pass; exact Courtney and Grunt battle text is source-implemented and stays within the native 36-character line gate",
            "guide_summary": "Document cap 40, Solrock-Lunatone impact lead, conditional Explosion, Minior Shell Smash, Choice Jirachi, Assault Vest Celesteela, Mega Aerodactyl finale, exact AI conditions, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Every slot narrates a different stage of a meteor impact, making Meteor Falls mechanically relevant without borrowing Mossdeep's cosmic battle system.",
            "weakest_link": "Explosion can feel cheap even when telegraphed. The low-HP/protected-partner predicate, broad Ghost/Wide Guard/Protect counterplay, and absence of a second detonation are mandatory.",
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
                "The source guide places the required Meteor Falls encounter at strict cap 40 as two three-member opponent owners in multi_2_vs_2.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Aerodactylite maps Aerodactyl to Mega Aerodactyl and no other transformation item appears.",
                "All three selected references exist; unindexed site-specific roles are locally authored rather than falsely attributed.",
                "All six rival gender/starter branches resolve to the same two exact opponent owners and source-authored ally parties.",
            ],
            "source_blockers": [
                "Regression-test Explosion ally safety, Protect, Focus Sash, Shell Smash, White Herb, Shields Down, Choice lock, Trick, Beast Boost, Mega timing, and simultaneous replacements.",
                "Run cap-40 Wide Guard/Ghost, setup denial, Water/Electric/Steel/Rock, Choice exploitation, fast, slow, Hard, Medium, and Easy tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def slateport_archie_design(meta: dict, records: dict[str, dict], source: dict) -> dict:
    selected = {
        "showdown:gen6randomdoublesbattle:018": (
            "selected-role",
            "The generated Liepard roster validates Prankster utility and priority at doubles stakes. Archie authors manual Rain Dance, Fake Out, Encore, and Foul Play as heist control.",
        ),
        "elite:wolfe:worlds-2016": (
            "selected-support",
            "Wolfe Glick's World-winning Hitmontop validates layered Fake Out, Wide Guard, and Feint. Archie imports that tactical support language without Kyogre, Primal weather, or the full champion roster.",
        ),
        "showdown:gen6randomdoublesbattle:009": (
            "selected-set",
            "The generated Manaphy roster validates it as a rare doubles attacker. Archie uses four immediate coverage/protection moves rather than Tail Glow setup.",
        ),
        "elite:wolfe:toronto-2024": (
            "adapted-role",
            "Wolfe's Toronto team validates Kingdra as primary rain offense. Archie imports only Swift Swim and direct coverage, rejecting Politoed, Perish, Shadow Tag, redirection, sleep, and Trick Room.",
        ),
        "showdown:gen9championsrandomdoublesbattle:011": (
            "adapted-set",
            "The Champions generator validates Qwilfish as doubles utility. Archie uses Intimidate, Thunder Wave, and direct Water/Poison attacks without hazards or redirection.",
        ),
        "showdown:gen9championsrandomdoublesbattle:002": (
            "adapted-set",
            "The Champions generator validates Malamar as a doubles threat. Archie reserves its custom Mega as the heist leader's Contrary ace without importing sleep or dual-speed modes.",
        ),
    }
    candidates = [reference_digest(records[ref], *decision) for ref, decision in selected.items()]
    current = {
        "party": [mon["species"] for mon in source["mons"]],
        "level_offsets": [mon["level_offset"] for mon in source["mons"]],
        "format": source["format"],
        "quality_score": source["quality_score"],
        "existing_design_status": source.get("design_status"),
        "reason_for_replacement": "The currently closed Battle 48 is mechanically strong but gives Archie Primal Kyogre before the story awakens Kyogre and repeats later signature species. This anchor requires a deliberate source backfill: a tactical heist crew with vulnerable manual rain and no Kyogre at all.",
    }
    team = [
        {
            "order": 1,
            "species": "SPECIES_LIEPARD",
            "level_offset": 1,
            "item": "ITEM_DAMP_ROCK",
            "ability": "ABILITY_PRANKSTER",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPEED_TIMID",
            "moves": ["MOVE_RAIN_DANCE", "MOVE_FAKE_OUT", "MOVE_ENCORE", "MOVE_FOUL_PLAY"],
            "role": "Heist coordinator: manually creates rain, steals one tempo turn, punishes passive repetition, and remains vulnerable with no Protect.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 2,
            "species": "SPECIES_HITMONTOP",
            "level_offset": 1,
            "item": "ITEM_EJECT_BUTTON",
            "ability": "ABILITY_INTIMIDATE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_FAKE_OUT", "MOVE_WIDE_GUARD", "MOVE_FEINT", "MOVE_CLOSE_COMBAT"],
            "role": "World-proven tactical entry: may buy the manual weather turn, shield spread, break Protect, or attack before Eject Button hands the heist forward.",
            "lead_group": "guaranteed-lead",
            "mega_candidate": False,
        },
        {
            "order": 3,
            "species": "SPECIES_MANAPHY",
            "level_offset": 2,
            "item": "ITEM_LIFE_ORB",
            "ability": "ABILITY_HYDRATION",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_ENERGY_BALL", "MOVE_PROTECT"],
            "role": "Rare museum prize and immediate special attacker; Hydration is relevant but no Rest or Tail Glow turns it into a stall or setup engine.",
            "lead_group": "rain-reserve",
            "mega_candidate": False,
        },
        {
            "order": 4,
            "species": "SPECIES_KINGDRA",
            "level_offset": 2,
            "item": "ITEM_MYSTIC_WATER",
            "ability": "ABILITY_SWIFT_SWIM",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_SCALD", "MOVE_DRACO_METEOR", "MOVE_ICY_WIND", "MOVE_PROTECT"],
            "role": "Primary rain getaway attacker with spread pressure, one explicit Draco Meteor cost, and secondary speed control.",
            "lead_group": "rain-reserve",
            "mega_candidate": False,
        },
        {
            "order": 5,
            "species": "SPECIES_QWILFISH",
            "level_offset": 3,
            "item": "ITEM_BLACK_SLUDGE",
            "ability": "ABILITY_INTIMIDATE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_WATERFALL", "MOVE_POISON_JAB", "MOVE_THUNDER_WAVE", "MOVE_PROTECT"],
            "role": "Physical getaway control: second Intimidate, paralysis, Water/Poison pressure, and one Protect without hazards or setup.",
            "lead_group": "control-reserve",
            "mega_candidate": False,
        },
        {
            "order": 6,
            "species": "SPECIES_MALAMAR",
            "level_offset": 4,
            "item": "ITEM_MALAMARITE",
            "ability": "ABILITY_CONTRARY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_SUPERPOWER", "MOVE_PSYCHO_CUT", "MOVE_KNOCK_OFF", "MOVE_PROTECT"],
            "role": "Archie's sole Mega and heist mastermind: Contrary Superpower can snowball, but it is slow, targetable, and has no recovery or second setup move.",
            "lead_group": "ace-reserve",
            "mega_candidate": True,
        },
    ]
    return {
        "anchor_id": "SLATEPORT_ARCHIE",
        "planning_tier": "faction_leader_opening",
        "status": {"design": "design-complete", "source": "source-closed", "static": "design-validated", "runtime": "unplayed"},
        "campaign_state": {
            "canonical_stage": "Archie's Slateport Oceanic Museum interception after Brawly and before Wattson",
            "location": "SlateportCity_OceanicMuseum_2F",
            "strict_cap": 30,
            "player_tools": [
                "Two Badges and all catches and progression tools through Granite Cave, Route 109, Slateport, and the museum",
                "The reusable Leveler, every legal move source, legal ability switching, and free ordinary battle items",
                "Mega Bracelet obtained through the revised early progression and Mega Stones available before Slateport",
                "Manual healing and party preparation before entering the museum sequence",
                "Live Hard, Medium, or Easy opposing-level setting; Hard is authored",
            ],
            "mega_access": "Archie uses one Mega Malamar. Kyogre and Blue Orb are absent because Kyogre has not been awakened; Primal Kyogre is reserved for Seafloor Cavern.",
            "evolution_phase": "Early-mid boss transition at levels 31-34: fully evolved and one mythical threat are intentionally boss-exclusive while ordinary route teams still mix evolution stages.",
            "preparation_access": "The player may prepare before the museum sequence. The two preceding grunts and Archie require exact script review for between-battle menu access during backfill.",
            "gauntlet_position": "Aqua's opening leader exam and tactical heist. It must establish intelligence and manual coordination without breaking story chronology through premature Kyogre.",
            "mechanics_baseline_id": "faction_leader",
            "live_difficulty": "Hard uses offsets +1,+1,+2,+2,+3,+4; Medium subtracts two and Easy subtracts four from final opponent levels only.",
        },
        "runtime": {
            "trainer_ids": ["TRAINER_ARCHIE_SLATEPORT"],
            "canonical_format": "double",
            "party_size": 6,
            "required": True,
            "variants": [
                {"variant_id": "slateport_archie", "trainer_ids": ["TRAINER_ARCHIE_SLATEPORT"], "format": "double", "scope": "source-closed-backfill", "reachability": "required main story; Battle 48 now uses the story-correct manual-rain heist"},
                {"variant_id": "seafloor_cavern_final", "trainer_ids": ["TRAINER_ARCHIE"], "format": "double", "scope": "later-backward-anchor", "reachability": "later required battle"},
            ],
            "current_source_baseline": current,
            "source_paths": [
                "src/data/trainer_parties.h:sParty_Archie1",
                "src/data/trainers.h:TRAINER_ARCHIE_SLATEPORT",
                "data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc",
                "docs/verdant_bespoke_battle_designs.json:BATTLE_048_SLATEPORT_MUSEUM_ARCHIE",
            ],
        },
        "rolling_context": {
            "available": True,
            "reason": "Battle 48 is already in the closed chronological ledger, so its actual previous-ten context is available in the canonical sequence and existing Battle 48 dossier.",
            "previous_encounters": [
                "BATTLE_038_SLATEPORT_BEACH_RICKY",
                "BATTLE_039_SLATEPORT_BEACH_EDMOND",
                "BATTLE_040_SLATEPORT_BEACH_HAILEY",
                "BATTLE_041_SLATEPORT_BEACH_DWAYNE",
                "BATTLE_042_SLATEPORT_BEACH_AUSTINA_GWEN",
                "BATTLE_043_SLATEPORT_BEACH_LOLA_CHANDRA",
                "BATTLE_044_SLATEPORT_MARKET_GAIL",
                "BATTLE_045_OCEANIC_MUSEUM_GRUNT_1",
                "BATTLE_046_OCEANIC_MUSEUM_GRUNT_2",
                "BATTLE_047_OCEANIC_MUSEUM_GRUNT_3",
            ],
            "protected_neighbor_anchors": ["MAUVILLE_GYM_WATTSON", "WEATHER_INSTITUTE_SHELLY", "SEAFLOOR_CAVERN_FINAL_ARCHIE", "CHAMPION_WALLACE"],
            "required_preimplementation_review": "Backfill completed. Before release, re-run the exact previous-ten review after the unrelated closed-battle source drift is reconciled and perform real-ROM Hard/Medium/Easy playtests.",
        },
        "identity": {
            "memory_hook": "Archie's heist crew makes its own storm: Liepard calls the rain, Hitmontop opens and closes lanes, Manaphy is the stolen prize, Kingdra is the getaway speed, Qwilfish slows pursuit, and Mega Malamar planned it all.",
            "story_fit": "Archie has not awakened Kyogre. His first battle should prove he is dangerous through planning and rare stolen power, while leaving the legendary he seeks for the story's actual climax.",
            "primary_player_question": "Can the player deny Prankster manual rain through layered Fake Out, Wide Guard, Feint, and Encore, then survive Manaphy and Kingdra while preventing Intimidate control from feeding Mega Malamar's Contrary Superpower endgame?",
            "primary_mode": "Liepard and Hitmontop create a vulnerable heist opening: one manual Rain Dance protected by matchup-dependent Fake Out, Wide Guard, Feint, or Encore rather than automatic weather.",
            "secondary_mode": "Manaphy and Kingdra are special getaway pressure, Qwilfish slows pursuit physically, and Mega Malamar converts careless stat drops and Superpower into the finale.",
            "preview_pressure": "A mythical and a custom Mega make the early boss exciting, but no Kyogre or Primal appears before the story earns it.",
        },
        "difficulty": {
            "target": 10,
            "observed": None,
            "rationale": "Hard places levels 31 through 34 against cap 30 with Prankster manual rain, layered Fake Out, Wide Guard, Feint, Encore, Eject Button, a mythical attacker, Swift Swim, Icy Wind, dual Intimidate, paralysis, and one Contrary Mega. The engine is vulnerable and has no automatic weather, sleep, recovery loop, redirection, or Primal.",
            "pressure_sources": [
                "Damp Rock Prankster Liepard manual rain, Fake Out, Encore, and Foul Play",
                "Eject Button Intimidate Hitmontop layered tactical support",
                "Life Orb Manaphy three-type special coverage",
                "Mystic Water Swift Swim Kingdra spread, Draco Meteor, and Icy Wind",
                "Black Sludge Intimidate Qwilfish paralysis and physical pressure",
                "Mega Malamar Contrary Superpower snowball and Knock Off",
            ],
            "resource_tax": "The fight taxes Fake Out immunity and sequencing, manual weather denial, spread-versus-Feint/Wide Guard reads, special bulk, speed control, Intimidate discipline, Fairy/Bug pressure, and item preservation.",
            "tuning_order": [
                "Preserve no-Kyogre chronology, tactical heist, vulnerable rain, mythical pressure, and Mega Malamar",
                "Validate Rain Dance, layered support, Eject Button, Swift Swim, Intimidate, paralysis, and Contrary before changing sets",
                "Adjust offsets within +1 to +4, beginning with Malamar, Qwilfish, and Manaphy",
                "Then adjust Liepard or Hitmontop survivability",
                "Change moves or species only after Hard/Medium/Easy tests",
            ],
        },
        "team": team,
        "ordering": {
            "intended_lead": ["SPECIES_LIEPARD", "SPECIES_HITMONTOP"],
            "mandatory_order_reason": "The lead exposes manual coordination. Prize, getaway attacker, pursuit control, and mastermind are board-state reserves rather than scripted pairs.",
            "reserve_sequence": [
                "Use Manaphy when immediate special coverage and Hydration create the best visible rain line.",
                "Use Kingdra while rain and Swift Swim produce real pressure; account for Draco Meteor drops and use Icy Wind only when speed matters.",
                "Use Qwilfish when Intimidate, paralysis, Water/Poison pressure, or physical bulk is matchup-correct.",
                "Preserve Mega Malamar as mastermind when practical, but deploy it earlier if Contrary or coverage is uniquely correct.",
            ],
        },
        "ai": {
            "existing_flags": ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_CHECK_FOE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"],
            "required_flags": ["AI_FLAG_HP_AWARE"],
            "custom_requirements": [
                "Score Liepard and Hitmontop actions jointly: Rain Dance, either Fake Out, Wide Guard, Feint, and Encore require visible value and may not stack redundant support.",
                "Use Rain Dance only when rain is absent, Liepard survives, and active or reserve attackers gain meaningful value.",
                "Resolve Eject Button and reserve selection normally, with a legal fallback when Hitmontop leaves earlier than expected.",
                "Respect Draco Meteor drops, Icy Wind speed value, Thunder Wave status, both Intimidates, and Contrary without reading hidden player choices.",
                "Mega Evolve Malamar normally and avoid Protect or Superpower loops when direct coverage or survival says otherwise.",
            ],
            "forbidden_behaviors": [
                "Do not use Kyogre, Blue Orb, Primal Reversion, automatic rain, or story-incoherent legendary ownership.",
                "Do not layer redundant Fake Out/support or predict hidden Protect with Feint.",
                "Do not grant rain benefits after weather ends or violate stat-stage rules.",
                "Do not add second Mega, Tera, Z-Move, Dynamax, or Gigantamax.",
            ],
            "state_machine": "State A attempts the Liepard-Hitmontop heist opening. State B selects Manaphy or Kingdra as rain pressure. State C deploys Qwilfish as pursuit control. State D exposes Mega Malamar as mastermind. If rain is denied, every state uses its legal non-rain fallback.",
        },
        "counterplay": {
            "classes": [
                "Inner Focus, Ghost types, Protect, priority, Taunt, double-targeting, faster weather, or immediate Liepard pressure can deny the manual rain opening.",
                "Use single-target attacks around Wide Guard, non-Protect lines around Feint, resist pivots, and Eject Button awareness rather than guessing every support move.",
                "Use Electric/Grass against Water pressure, Fairy/Dragon special bulk against Kingdra, and exploit Draco Meteor drops or finite rain turns.",
                "Use Ground/Psychic/Electric, Taunt, status immunity, or special pressure against Qwilfish before paralysis and Intimidate accumulate.",
                "Avoid careless stat drops into Contrary, use Fairy/Bug, burn, Haze, Unaware, priority, or concentrated special damage against Mega Malamar.",
            ],
            "intentional_weakness": "Liepard is frail and the only weather setter; Hitmontop can be ejected; Manaphy has no setup; Kingdra depends on finite rain; Qwilfish is specially vulnerable; Mega Malamar is slow with severe Bug/Fairy pressure. There is no automatic rain, Kyogre, sleep, redirection, or recovery loop.",
            "first_loss_lesson": "This is a planned robbery, not a legendary flood. Break the coordinator, read support from the board instead of guessing, outlast the finite getaway rain, and do not feed Contrary before focusing Malamar.",
            "revealed_information": [
                "Rain turns, Prankster priority, Fake Out validity, Wide Guard, Feint resolution, Encore target, Eject Button, Swift Swim, stat drops, Intimidate, paralysis, Contrary, and Mega evolution are public state.",
                "Kyogre and Blue Orb are absent from both preview and runtime party.",
                "Every rain benefit ends with ordinary weather expiration.",
                "Mega Malamar is the only transformation.",
            ],
            "unacceptable_failure_modes": [
                "Kyogre or Primal appears before awakening",
                "The lead stacks redundant support or predicts hidden actions",
                "Rain benefits persist after weather ends",
                "Contrary or stat drops resolve incorrectly",
                "The backfill is documented but the old Battle 48 source remains release-closed",
            ],
        },
        "competitive_research": {
            "index": {"version": meta["version"], "record_count": meta["record_count"], "sha256": meta["sha256"]},
            "queries": ["Liepard doubles", "Wolfe Hitmontop support", "Manaphy random doubles", "Wolfe Kingdra rain", "Qwilfish Champions doubles", "Mega Malamar doubles"],
            "candidates": candidates,
            "selected_reference_ids": list(selected),
            "rejected_gimmicks": [
                "Current Battle 48 Primal Kyogre, Raichu, Ludicolo, Crobat, and automatic rain are removed.",
                "Wolfe Toronto's Perish, Shadow Tag, redirection, sleep, and Trick Room modes are not imported.",
                "Archie's final Tsareena, Archaludon, Palafin, Urshifu, Mega Sharpedo, and Primal Kyogre remain protected.",
                "No second Mega or unsupported transformation appears.",
            ],
            "imported_elements": [
                "Generated Liepard Prankster utility",
                "Wolfe World Champion Hitmontop support language",
                "Generated Manaphy immediate pressure",
                "Wolfe-documented Kingdra primary rain offense without the trap mode",
                "Champions Qwilfish and Malamar roles",
            ],
        },
        "campaign_reservations": {
            "spends": [
                "Archie's Slateport tactical heist identity",
                "Prankster manual rain plus layered Hitmontop support",
                "Manaphy as the rare museum prize",
                "Kingdra getaway speed and Qwilfish pursuit control",
                "Mega Malamar as the mastermind",
            ],
            "preserves": [
                "Archie's first Kyogre and only Primal for Seafloor Cavern",
                "Matt's Politoed/Pelipper rain battles and Shelly's Castform/snow science",
                "Wattson's Raichu and Wallace's later champion rain",
                "Other heist and manual-weather teams only if they ask a different question",
            ],
            "releases": [
                "Kyogre, Raichu, Ludicolo, Crobat, and the old Battle 48 party leave Slateport Archie",
                "Other Water, Dark, Fighting, and Poison species remain available outside this tactical crew",
            ],
            "collision_notes": [
                "No species overlaps the protected Gym, League, or designed faction anchors.",
                "Manual rain overlaps Weather Institute only as a resource: Archie layers tactical support and a heist crew, while Shelly studies distinct specimen effects.",
                "Mega Malamar is unique here; Mega Sharpedo and Primal Kyogre remain Archie's final progression.",
            ],
        },
        "presentation": {
            "intro_concept": "Archie says a good heist never waits for the ocean to arrive; his crew brings its own storm and leaves before anyone can react.",
            "defeat_concept": "He admits the player broke the plan at its handoffs and protected the museum without needing a legendary.",
            "post_battle_concept": "Native museum progression remains unchanged and no dialogue implies Archie already owns or awakened Kyogre.",
            "hint_concept": "The museum grunts warn that the cat must call rain manually, the fighter can open or break shelter, the stolen prince is the special prize, and the squid turns stat drops backward.",
            "native_width_status": "pass; revised Archie text is branch-neutral, removes premature Kyogre, and passes the native 36-character line gate",
            "guide_summary": "Battle 48 at cap 30 uses Liepard-Hitmontop manual-rain tactics, Manaphy, Kingdra, Qwilfish, Mega Malamar, explicit no-Kyogre chronology, exact AI, previous-ten review, and live difficulty offsets.",
        },
        "author_self_check": {
            "strongest_part": "Removing premature Primal Kyogre fixes the story and actually makes Archie more impressive: the first battle is won through a tactical crew, while the final battle earns the legendary flood.",
            "weakest_link": "Manual rain also appears later at the Weather Institute. The support-dense heist, early cap, mythical prize, and Contrary mastermind must keep this encounter tactically distinct from Shelly's specimen experiment.",
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
                "Battle 48 source now exactly matches the story-correct six-member manual-rain heist and contains no Kyogre or Blue Orb.",
                "The source guide places Slateport Archie at strict cap 30 in a required six-Pokemon double.",
                "Every proposed move, item, spread, species, and selected ability slot exists and passes local legality.",
                "Malamarite maps Malamar to Mega Malamar and no other transformation item appears.",
                "All six selected references exist across Wolfe, generated, tournament, and Champions evidence.",
            ],
            "source_blockers": [
                "Reconcile unrelated source drift in other Battles 1-68 before the whole 72-battle frontier is called release-clean.",
                "Regression-test manual rain, layered support, Eject Button, Swift Swim, Hydration, Icy Wind, Intimidate, paralysis, Contrary, Mega timing, and all museum branches.",
                "Run cap-30 no-rain, support denial, Electric/Grass/Fairy/Bug, fast, slow, Hard, Medium, and Easy real-ROM tests before observed difficulty is recorded.",
            ],
        },
        "mechanics_proposal": None,
    }


def build() -> dict:
    meta = json.loads(META_PATH.read_text())
    records = {record["reference_id"]: record for record in competitive.load_records()}
    source_teams = {team["trainer_id"]: team for team in quality.audit()["teams"]}
    designs = {
        "MAGMA_HIDEOUT_FINAL_MAXIE": maxie_design(meta, records, source_teams["TRAINER_MAXIE_MAGMA_HIDEOUT"]),
        "SEAFLOOR_CAVERN_FINAL_ARCHIE": archie_design(meta, records, source_teams["TRAINER_ARCHIE"]),
        "MAGMA_HIDEOUT_COURTNEY": courtney_design(meta, records, source_teams["TRAINER_COURTNEY_MAGMA_HIDEOUT"]),
        "SEAFLOOR_CAVERN_SHELLY": shelly_design(meta, records, source_teams["TRAINER_SHELLY_SEAFLOOR_CAVERN"]),
        "MAGMA_HIDEOUT_TABITHA": tabitha_design(meta, records, source_teams["TRAINER_TABITHA_MAGMA_HIDEOUT"]),
        "AQUA_HIDEOUT_MATT": matt_design(meta, records, source_teams["TRAINER_MATT"]),
        "MT_CHIMNEY_MAXIE": source_closed_mt_chimney_maxie_design(meta, records, source_teams["TRAINER_MAXIE_MT_CHIMNEY"]),
        "MT_PYRE_MATT": mt_pyre_matt_design(meta, records, source_teams["TRAINER_MATT_MT_PYRE"]),
        "MT_CHIMNEY_TABITHA": source_closed_mt_chimney_tabitha_design(meta, records, source_teams["TRAINER_TABITHA_MT_CHIMNEY"]),
        "WEATHER_INSTITUTE_SHELLY": weather_institute_shelly_design(meta, records, source_teams["TRAINER_SHELLY_WEATHER_INSTITUTE"]),
        "METEOR_FALLS_COURTNEY": meteor_falls_courtney_design(meta, records, source_teams["TRAINER_COURTNEY_METEOR_FALLS"], source_teams["TRAINER_GRUNT_METEOR_FALLS"]),
        "SLATEPORT_ARCHIE": slateport_archie_design(meta, records, source_teams["TRAINER_ARCHIE_SLATEPORT"]),
    }
    return {
        "version": 1,
        "title": "Emerald Champions paired Magma and Aqua anchor designs",
        "phase": "factions_backward_in_paired_rounds",
        "expected_pair_order": EXPECTED_PAIR_ORDER,
        "completed_pairs": 6,
        "designs": designs,
        "pair_review": pair_review(designs),
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
        result[match.group(1)] = set(re.findall(r"\.type[12]\s*=\s*(TYPE_[A-Z0-9_]+)", match.group(2)))
    return result


def protected_species() -> set[str]:
    league = json.loads(LEAGUE_PATH.read_text())
    gyms = json.loads(GYMS_PATH.read_text())
    return {
        mon["species"]
        for payload in (league["designs"], gyms["designs"])
        for dossier in payload.values()
        for mon in dossier["team"]
    }


def pair_review(designs: dict[str, dict]) -> dict:
    uses: dict[str, list[str]] = {}
    for anchor_id, dossier in designs.items():
        for mon in dossier["team"]:
            uses.setdefault(mon["species"], []).append(anchor_id)
    maxie = designs["MAGMA_HIDEOUT_FINAL_MAXIE"]
    archie = designs["SEAFLOOR_CAVERN_FINAL_ARCHIE"]
    questions = [dossier["identity"]["primary_player_question"] for dossier in designs.values()]
    protected = protected_species()
    protected_collisions = [
        {"anchor_id": anchor_id, "species": mon["species"]}
        for anchor_id, dossier in designs.items()
        for mon in dossier["team"]
        if mon["species"] in protected
    ]
    unwaived = [
        row for row in protected_collisions
        if (row["anchor_id"], row["species"]) not in ALLOWED_PROTECTED_REUSES
    ]
    internal_collisions = {species: anchors for species, anchors in uses.items() if len(anchors) > 1}
    unwaived_internal = {
        species: anchors
        for species, anchors in internal_collisions.items()
        if any((anchor_id, species) not in ALLOWED_INTERNAL_REUSES for anchor_id in anchors[1:])
    }
    return {
        "status": "pass",
        "completed_pairs": len(designs) // 2,
        "unique_species_count": len(uses),
        "pair_species_collisions": internal_collisions,
        "allowed_internal_reuses": [
            {"anchor_id": anchor_id, "species": species, "reason": reason}
            for (anchor_id, species), reason in ALLOWED_INTERNAL_REUSES.items()
            if anchor_id in designs and species in uses
        ],
        "unwaived_pair_species_collisions": unwaived_internal,
        "protected_anchor_collisions": protected_collisions,
        "allowed_protected_reuses": [
            {"anchor_id": anchor_id, "species": species, "reason": reason}
            for (anchor_id, species), reason in ALLOWED_PROTECTED_REUSES.items()
            if anchor_id in designs and species in uses
        ],
        "unwaived_protected_collisions": unwaived,
        "maxie_primary_verbs": ["ground", "amplify", "repeat", "invert", "erupt"],
        "archie_primary_verbs": ["flood", "charge", "pivot", "break shelter", "accelerate"],
        "primary_questions_distinct": maxie["identity"]["primary_player_question"] != archie["identity"]["primary_player_question"],
        "all_primary_questions_distinct": len(questions) == len(set(questions)),
        "transformation_contract": {
            "maxie": ["ITEM_RED_ORB", "ITEM_CAMERUPTITE"],
            "archie": ["ITEM_BLUE_ORB", "ITEM_SHARPEDONITE"],
            "rule": "Exactly one Primal and one Mega per final leader; no other battle gimmick.",
        },
        "judgment": "Maxie controls stable geometry and repeated land pressure; Archie creates unstable momentum and forced handoffs. Weather is shared fiction, not a shared puzzle.",
    }


def validate(payload: dict) -> None:
    contract = json.loads(OS_PATH.read_text())["dossier_contract"]
    expected = [anchor_id for pair in EXPECTED_PAIR_ORDER[: payload["completed_pairs"]] for anchor_id in pair]
    if list(payload["designs"]) != expected:
        raise AssertionError("Faction anchors are not advancing in paired backward order")
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
        expected_source_status = "source-closed" if anchor_id in {"SLATEPORT_ARCHIE", "METEOR_FALLS_COURTNEY", "MT_CHIMNEY_TABITHA", "MT_CHIMNEY_MAXIE"} else "unimplemented"
        if dossier["status"] != {"design": "design-complete", "source": expected_source_status, "static": "design-validated", "runtime": "unplayed"}:
            raise AssertionError(f"{anchor_id} status drifted")
        if dossier["difficulty"]["target"] != 10 or dossier["difficulty"]["observed"] is not None:
            raise AssertionError(f"{anchor_id} difficulty status is dishonest")
        if len(dossier["team"]) != 6 or sum(mon["mega_candidate"] for mon in dossier["team"]) != 1:
            raise AssertionError(f"{anchor_id} requires six Pokemon and one Mega")
        primal_items = [mon["item"] for mon in dossier["team"] if mon["item"] in {"ITEM_RED_ORB", "ITEM_BLUE_ORB"}]
        expected_primal = {
            "MAGMA_HIDEOUT_FINAL_MAXIE": ["ITEM_RED_ORB"],
            "SEAFLOOR_CAVERN_FINAL_ARCHIE": ["ITEM_BLUE_ORB"],
        }.get(anchor_id, [])
        if primal_items != expected_primal:
            raise AssertionError(f"{anchor_id} Primal contract drifted")
        for mon in dossier["team"]:
            if set(contract["mon_required"]) - set(mon):
                raise AssertionError(f"{anchor_id} {mon.get('species')} lacks required Pokemon fields")
            if mon["species"] in protected and (anchor_id, mon["species"]) not in ALLOWED_PROTECTED_REUSES:
                raise AssertionError(f"{anchor_id} collides on protected species {mon['species']}")
            if mon["species"] in seen and (anchor_id, mon["species"]) not in ALLOWED_INTERNAL_REUSES:
                raise AssertionError(f"{anchor_id} collides within the faction board on {mon['species']} from {seen[mon['species']]}")
            seen.setdefault(mon["species"], anchor_id)
            legal = dex.legal_moves(mon["species"])
            if mon["species"] == "SPECIES_ROTOM_FROST":
                legal = set(legal) | {"MOVE_FREEZE_DRY"}
            illegal = [move for move in mon["moves"] if move not in legal]
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
            raise AssertionError(f"{anchor_id} competitive references are missing")
        if len(dossier["counterplay"]["classes"]) < 3:
            raise AssertionError(f"{anchor_id} lacks broad counterplay")
        active = json.dumps({
            "identity": dossier["identity"],
            "team": dossier["team"],
            "ordering": dossier["ordering"],
            "state_machine": dossier["ai"]["state_machine"],
            "custom_requirements": dossier["ai"]["custom_requirements"],
        }).lower()
        if any(word in active for word in ("terastallization", "z-move", "dynamax", "gigantamax")):
            raise AssertionError(f"{anchor_id} imports an unsupported battle gimmick")

    review = payload["pair_review"]
    expected_distinct = len(payload["designs"]) * 6 - len(review["allowed_internal_reuses"])
    if review["status"] != "pass" or review["completed_pairs"] != payload["completed_pairs"] or review["unique_species_count"] != expected_distinct:
        raise AssertionError("Faction pair review is incomplete")
    if review["unwaived_pair_species_collisions"] or review["unwaived_protected_collisions"] or not review["primary_questions_distinct"] or not review["all_primary_questions_distinct"]:
        raise AssertionError("Faction leaders collide with protected campaign anchors")


def markdown(payload: dict) -> str:
    lines = [
        "# Emerald Champions paired Magma and Aqua anchor designs",
        "",
        f"Progress: {payload['completed_pairs']}/{len(EXPECTED_PAIR_ORDER)} paired rounds are design-complete; source status is reported per anchor.",
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
            transformation = "; Mega" if mon["mega_candidate"] else ("; Primal" if mon["item"] in {"ITEM_RED_ORB", "ITEM_BLUE_ORB"} else "")
            lines.append(
                f"  - `{mon['species']}` +{mon['level_offset']} — `{mon['item']}`, `{mon['ability']}`{transformation}; "
                + ", ".join(f"`{move}`" for move in mon["moves"])
            )
        lines.extend(["", f"AI must execute: {' '.join(dossier['ai']['custom_requirements'])}", ""])
    review = payload["pair_review"]
    lines.extend([
        "## Pair review",
        "",
        f"- Distinct species: {review['unique_species_count']} across {len(payload['designs']) * 6} slots; unwaived faction collisions: {len(review['unwaived_pair_species_collisions'])}; unwaived protected-anchor collisions: {len(review['unwaived_protected_collisions'])}.",
        f"- Intentional recurring faction signatures: {len(review['allowed_internal_reuses'])}.",
        f"- Intentional protected reprises: {len(review['allowed_protected_reuses'])} (Archie's Primal Kyogre versus Wallace's later base Kyogre).",
        f"- Judgment: {review['judgment']}",
    ])
    if payload["completed_pairs"] < len(EXPECTED_PAIR_ORDER):
        lines.extend([
            "",
            "## Next paired round",
            "",
            f"`{EXPECTED_PAIR_ORDER[payload['completed_pairs']][0]}` and `{EXPECTED_PAIR_ORDER[payload['completed_pairs']][1]}`",
            "",
        ])
    else:
        lines.extend([
            "",
            "## Backward faction board complete",
            "",
            "Next: rival milestones, Steven, rematches, and superbosses, followed by the campaign-wide anchor collision review.",
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
            raise SystemExit("FAIL: faction anchor JSON is missing or stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: faction anchor Markdown is missing or stale")
    print(f"PASS: {payload['completed_pairs']}/{len(EXPECTED_PAIR_ORDER)} Magma/Aqua rounds are paired, design-complete, and source-honest")
    print(f"PASS: {payload['pair_review']['unique_species_count']} distinct species across {len(payload['designs']) * 6} slots, one Mega per anchor, one Primal per final leader, and zero unwaived collisions")
    if payload["completed_pairs"] < len(EXPECTED_PAIR_ORDER):
        print(f"NEXT: {EXPECTED_PAIR_ORDER[payload['completed_pairs']][0]} + {EXPECTED_PAIR_ORDER[payload['completed_pairs']][1]}")
    else:
        print("NEXT: rival milestones, Steven, rematches, and superbosses")


if __name__ == "__main__":
    main()
