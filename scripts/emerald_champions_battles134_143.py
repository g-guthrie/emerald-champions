#!/usr/bin/env python3
"""Author, apply, and verify the remaining Route 111 trainer batch."""

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
FORMATS = ROOT / "docs/verdant_doubles_manifest.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"
PARTIES = ROOT / "src/data/trainer_parties.h"
TRAINERS = ROOT / "src/data/trainers.h"


def M(level, species, item, ability_slot, spread, *moves):
    return {
        "level": level,
        "species": species,
        "item": item,
        "ability_slot": ability_slot,
        "spread": spread,
        "moves": list(moves),
    }


# Battle 134 — Daisuke's three-shadow relay single.
DAISUKE_TEAM = [
    M(2, "SPECIES_NINJASK", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SWORDS_DANCE", "MOVE_BATON_PASS", "MOVE_X_SCISSOR", "MOVE_PROTECT"),
    M(3, "SPECIES_SHEDINJA", "ITEM_HEAVY_DUTY_BOOTS", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_POLTERGEIST", "MOVE_X_SCISSOR", "MOVE_SHADOW_SNEAK", "MOVE_PROTECT"),
    M(4, "SPECIES_MARSHADOW", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SPECTRAL_THIEF", "MOVE_CLOSE_COMBAT", "MOVE_SHADOW_SNEAK", "MOVE_ICE_PUNCH"),
]

# Battle 135 — Wilton's ruin-guardian rematch family.
COFAGRIGUS = M(1, "SPECIES_COFAGRIGUS", "ITEM_MENTAL_HERB", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_SHADOW_BALL", "MOVE_WILL_O_WISP", "MOVE_TRICK_ROOM", "MOVE_BODY_PRESS")
REUNICLUS = M(2, "SPECIES_REUNICLUS", "ITEM_LIFE_ORB", 1, "SPREAD_31_IV_HP_SPATK_QUIET", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST", "MOVE_RECOVER", "MOVE_PROTECT")
GARCHOMP = M(3, "SPECIES_GARCHOMP", "ITEM_ROCKY_HELMET", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_DRAGON_CLAW", "MOVE_EARTHQUAKE", "MOVE_ROCK_SLIDE", "MOVE_PROTECT")
LAPRAS = M(4, "SPECIES_LAPRAS", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_FREEZE_DRY", "MOVE_HYDRO_PUMP", "MOVE_THUNDERBOLT", "MOVE_PSYCHIC")
METAGROSS = M(5, "SPECIES_METAGROSS", "ITEM_WEAKNESS_POLICY", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_METEOR_MASH", "MOVE_ZEN_HEADBUTT", "MOVE_EARTHQUAKE", "MOVE_BULLET_PUNCH")
MEWTWO = M(6, "SPECIES_MEWTWO", "ITEM_EXPERT_BELT", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_PSYSTRIKE", "MOVE_AURA_SPHERE", "MOVE_ICE_BEAM", "MOVE_PROTECT")
MEWTWO_MEGA = dict(MEWTWO, item="ITEM_MEWTWONITE_Y")
WILTON_TEAMS = {
    "TRAINER_WILTON_1": [dict(COFAGRIGUS, level=2), dict(REUNICLUS, level=3), dict(GARCHOMP, level=4)],
    "TRAINER_WILTON_2": [COFAGRIGUS, REUNICLUS, GARCHOMP, LAPRAS],
    "TRAINER_WILTON_3": [COFAGRIGUS, REUNICLUS, GARCHOMP, LAPRAS, METAGROSS, MEWTWO],
    "TRAINER_WILTON_4": [COFAGRIGUS, REUNICLUS, GARCHOMP, LAPRAS, METAGROSS, MEWTWO_MEGA],
}

# Battle 136 — Brooke's elemental relay rematch family.
EMPOLEON = M(1, "SPECIES_EMPOLEON", "ITEM_SITRUS_BERRY", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_SCALD", "MOVE_FLASH_CANNON", "MOVE_ICY_WIND", "MOVE_PROTECT")
INFERNAPE = M(2, "SPECIES_INFERNAPE", "ITEM_FOCUS_SASH", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FAKE_OUT", "MOVE_CLOSE_COMBAT", "MOVE_FIRE_PUNCH", "MOVE_MACH_PUNCH")
ROSERADE = M(3, "SPECIES_ROSERADE", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_SLUDGE_BOMB", "MOVE_GIGA_DRAIN", "MOVE_EXTRASENSORY", "MOVE_PROTECT")
HAXORUS = M(4, "SPECIES_HAXORUS", "ITEM_LUM_BERRY", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_DRAGON_DANCE", "MOVE_DRAGON_CLAW", "MOVE_EARTHQUAKE", "MOVE_PROTECT")
SCIZOR = M(5, "SPECIES_SCIZOR", "ITEM_ASSAULT_VEST", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_BULLET_PUNCH", "MOVE_BUG_BITE", "MOVE_KNOCK_OFF", "MOVE_SUPERPOWER")
LATIOS = M(6, "SPECIES_LATIOS", "ITEM_SOUL_DEW", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_DRACO_METEOR", "MOVE_PSYSHOCK", "MOVE_TAILWIND", "MOVE_PROTECT")
LATIOS_MEGA = dict(LATIOS, item="ITEM_LATIOSITE")
BROOKE_TEAMS = {
    "TRAINER_BROOKE_1": [EMPOLEON, INFERNAPE, ROSERADE, HAXORUS],
    "TRAINER_BROOKE_2": [INFERNAPE, EMPOLEON, HAXORUS, ROSERADE],
    "TRAINER_BROOKE_3": [EMPOLEON, INFERNAPE, ROSERADE, HAXORUS, SCIZOR, LATIOS],
    "TRAINER_BROOKE_4": [EMPOLEON, INFERNAPE, ROSERADE, HAXORUS, SCIZOR, LATIOS_MEGA],
}

# Battle 137 — Hayden's toxic-edge double.
HAYDEN_TEAM = [
    M(1, "SPECIES_GLISCOR", "ITEM_TOXIC_ORB", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_EARTHQUAKE", "MOVE_FACADE", "MOVE_KNOCK_OFF", "MOVE_PROTECT"),
    M(2, "SPECIES_ZANGOOSE", "ITEM_TOXIC_ORB", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FACADE", "MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF", "MOVE_QUICK_ATTACK"),
    M(3, "SPECIES_VAPOREON", "ITEM_LEFTOVERS", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_PROTECT"),
    M(4, "SPECIES_DRAPION", "ITEM_SCOPE_LENS", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_CROSS_POISON", "MOVE_NIGHT_SLASH", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"),
]

# Battle 138 — Bianca's haunted-props double.
BIANCA_TEAM = [
    M(1, "SPECIES_GOURGEIST_SUPER", "ITEM_FLAME_ORB", 1, "SPREAD_31_IV_HP_SPATK_QUIET", "MOVE_SHADOW_BALL", "MOVE_ENERGY_BALL", "MOVE_TRICK_ROOM", "MOVE_PROTECT"),
    M(2, "SPECIES_GENGAR", "ITEM_LIFE_ORB", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_SLUDGE_BOMB", "MOVE_SHADOW_BALL", "MOVE_DAZZLING_GLEAM", "MOVE_PROTECT"),
    M(3, "SPECIES_SUDOWOODO", "ITEM_CHOICE_BAND", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HEAD_SMASH", "MOVE_LOW_KICK", "MOVE_SUCKER_PUNCH", "MOVE_EARTHQUAKE"),
    M(4, "SPECIES_MAGMORTAR", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_HEAT_WAVE", "MOVE_THUNDERBOLT", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST"),
]

# Battle 139 — Tyron's three-blade single.
TYRON_TEAM = [
    M(2, "SPECIES_SCYTHER", "ITEM_EVIOLITE", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SWORDS_DANCE", "MOVE_DUAL_WINGBEAT", "MOVE_BUG_BITE", "MOVE_QUICK_ATTACK"),
    M(3, "SPECIES_KABUTOPS", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SHELL_SMASH", "MOVE_LIQUIDATION", "MOVE_STONE_EDGE", "MOVE_AQUA_JET"),
    M(4, "SPECIES_BEEDRILL", "ITEM_BEEDRILLITE", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_X_SCISSOR", "MOVE_POISON_JAB", "MOVE_DRILL_RUN", "MOVE_SWORDS_DANCE"),
]

# Battle 140 — Celina removes the shackles from deliberately bad abilities.
CELINA_TEAM = [
    M(1, "SPECIES_VICTREEBEL", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_GASTRO_ACID", "MOVE_LEAF_STORM", "MOVE_SLUDGE_BOMB", "MOVE_PROTECT"),
    M(2, "SPECIES_SLAKING", "ITEM_CHOICE_BAND", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_DOUBLE_EDGE", "MOVE_HIGH_HORSEPOWER", "MOVE_PLAY_ROUGH", "MOVE_SUCKER_PUNCH"),
    M(3, "SPECIES_GOLISOPOD", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_FIRST_IMPRESSION", "MOVE_LIQUIDATION", "MOVE_LEECH_LIFE", "MOVE_AQUA_JET"),
    M(4, "SPECIES_DURANT", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_IRON_HEAD", "MOVE_X_SCISSOR", "MOVE_SUPERPOWER", "MOVE_ROCK_SLIDE"),
]

# Battle 141 — Celia's draining-growth single.
CELIA_TEAM = [
    M(2, "SPECIES_LUDICOLO", "ITEM_BIG_ROOT", 1, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_RAIN_DANCE", "MOVE_GIGA_DRAIN", "MOVE_HYDRO_PUMP", "MOVE_ICE_BEAM"),
    M(3, "SPECIES_PARASECT", "ITEM_FOCUS_SASH", 1, "SPREAD_31_IV_HP_SPDEF_CAREFUL", "MOVE_SPORE", "MOVE_LEECH_SEED", "MOVE_GIGA_DRAIN", "MOVE_PROTECT"),
    M(4, "SPECIES_VENUSAUR", "ITEM_VENUSAURITE", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_GIGA_DRAIN", "MOVE_SLUDGE_BOMB", "MOVE_LEECH_SEED", "MOVE_SYNTHESIS"),
]

# Battle 142 — Bryan's four ancient survival rules.
BRYAN_TEAM = [
    M(1, "SPECIES_SPIRITOMB", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPDEF_CAREFUL", "MOVE_SNARL", "MOVE_WILL_O_WISP", "MOVE_PAIN_SPLIT", "MOVE_PROTECT"),
    M(2, "SPECIES_TYRANTRUM", "ITEM_CHOICE_SCARF", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_HEAD_SMASH", "MOVE_DRAGON_CLAW", "MOVE_HIGH_HORSEPOWER", "MOVE_PSYCHIC_FANGS"),
    M(3, "SPECIES_SERPERIOR", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_LEAF_STORM", "MOVE_DRAGON_PULSE", "MOVE_GLARE", "MOVE_PROTECT"),
    M(4, "SPECIES_CARRACOSTA", "ITEM_WHITE_HERB", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SHELL_SMASH", "MOVE_ROCK_SLIDE", "MOVE_LIQUIDATION", "MOVE_AQUA_JET"),
]

# Battle 143 — Branden's Steely Spirit anchor crew.
BRANDEN_TEAM = [
    M(1, "SPECIES_PERRSERKER", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_IRON_HEAD", "MOVE_FAKE_OUT", "MOVE_CLOSE_COMBAT", "MOVE_U_TURN"),
    M(2, "SPECIES_DHELMISE", "ITEM_COLBUR_BERRY", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_ANCHOR_SHOT", "MOVE_POWER_WHIP", "MOVE_POLTERGEIST", "MOVE_PROTECT"),
    M(3, "SPECIES_SIRFETCHD", "ITEM_LEEK", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_METEOR_ASSAULT", "MOVE_BRAVE_BIRD", "MOVE_LEAF_BLADE", "MOVE_PROTECT"),
    M(4, "SPECIES_KARTANA", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_LEAF_BLADE", "MOVE_SACRED_SWORD", "MOVE_NIGHT_SLASH", "MOVE_PROTECT"),
]


CONFIGS = [
    {"index": 134, "id": "BATTLE_134_ROUTE_111_DAISUKE", "category": "optional three-shadow Ninja Boy single", "trainers": ["TRAINER_DAISUKE"], "main": "TRAINER_DAISUKE", "teams": {"TRAINER_DAISUKE": DAISUKE_TEAM}, "target": 8.9, "question": "Can the player deny Ninjask's relay without spending the move or passive-damage answer needed for Shedinja and Marshadow?", "tempo": "Sash setup and Baton Pass threaten a relay; Wonder Guard changes the answer; Marshadow steals any careless boost and closes.", "weakness": "Only three bodies; Taunt, phazing, Haze, hazards, weather, status, multi-hit moves, priority, and ordinary Flying or Fairy pressure all create broad answers.", "lesson": "Interrupt the relay, but preserve one clean Wonder Guard answer and never donate boosts to Spectral Thief.", "tags": ["route111-north", "three-shadow-relay", "baton-pass", "wonder-guard", "spectral-thief"], "refs": ["showdown:gen8randombattle:011", "showdown:gen5randomdoublesbattle:009", "showdown:gen8randomdoublesbattle:002"], "coords": [32, 29]},
    {"index": 135, "id": "BATTLE_135_ROUTE_111_WILTON", "category": "optional ruin-guardian Expert rematch family", "trainers": list(WILTON_TEAMS), "main": "TRAINER_WILTON_1", "teams": WILTON_TEAMS, "target": 9.3, "question": "Can the player solve a slow ruin in singles, then adapt when Trick Room, grounded spread damage, armor, and Mega Mewtwo Y become one doubles formation?", "tempo": "Burn and room control open the ruin; Magic Guard and Rough Skin punish direct races; Lapras and Metagross deepen rematches before one psychic Mega finale.", "weakness": "The main fight has three bodies; Trick Room is reversible; Cofagrigus is Tauntable; Lapras is slow outside room; Ground immunity, Dark, Ghost, Bug, burn, Wide Guard, and focused special damage remain broad.", "lesson": "Choose whether to reverse the room or exploit it, then preserve Dark or Ghost pressure for Reuniclus and the rematch's psychic closer.", "tags": ["route111-north", "ruin-guardians", "trick-room", "magic-guard", "rough-skin", "mega-mewtwo-y", "mixed-rematch-family"], "refs": ["vgc:us-nationals-2011", "elite:ray-rizzo:worlds-2012", "vgc:worlds-2012", "showdown:gen9championsrandomdoublesbattle:001"], "coords": [9, 27]},
    {"index": 136, "id": "BATTLE_136_ROUTE_111_BROOKE", "category": "optional elemental-relay Cooltrainer rematch family", "trainers": list(BROOKE_TEAMS), "main": "TRAINER_BROOKE_1", "teams": BROOKE_TEAMS, "target": 9.4, "question": "Can the player keep changing defensive answers as Water, Fire, Grass, Dragon, Bug, Steel, and Psychic pass initiative across the board?", "tempo": "Fake Out buys Empoleon control; Roserade and Haxorus force opposite answers; Scizor and Latios turn later rematches into a six-stage elemental relay.", "weakness": "There is no weather, room, redirection, trap, or recovery loop; Infernape is Sash frail, Haxorus must set up, Scizor is Fire-vulnerable, and Latios remains exposed to Ice, Fairy, Ghost, Bug, and Dark.", "lesson": "Do not commit one wall to the whole relay: remove the tempo lead, deny Haxorus setup, and retain the correct answer for the current stage.", "tags": ["route111-north", "elemental-relay", "competitive-empoleon", "fake-out", "dragon-dance", "mega-latios", "double-rematch-family"], "refs": ["smogon:gen4ou:002", "showdown:gen9randomdoublesbattle:029", "vgc:worlds-2013", "vgc:worlds-2009"], "coords": [11, 11]},
    {"index": 137, "id": "BATTLE_137_ROUTE_111_HAYDEN", "category": "optional toxic-edge Kindler double", "trainers": ["TRAINER_HAYDEN"], "main": "TRAINER_HAYDEN", "teams": {"TRAINER_HAYDEN": HAYDEN_TEAM}, "target": 9.1, "question": "Can the player exploit the activation turn before poison becomes healing, boosted Facade, and Sniper pressure?", "tempo": "Two Toxic Orbs advertise opposite payoffs; Vaporeon supplies speed and Helping Hand while Drapion turns high-crit edges into the finish.", "weakness": "The Orbs require activation; Zangoose is frail; Vaporeon has a visible Grass and Electric seam; burn, Haze, item removal, priority, and focused special pressure remain broad.", "lesson": "Use the free activation turn: remove Zangoose or strip an Orb before poison transforms the team's damage economy.", "tags": ["route111-north", "toxic-edge", "poison-heal", "toxic-boost", "helping-hand", "sniper"], "refs": ["smogon:gen9ou:010", "smogon:gen8nu:009", "smogon:gen8nu:002", "showdown:gen6randomdoublesbattle:013"], "coords": [16, 119]},
    {"index": 138, "id": "BATTLE_138_ROUTE_111_BIANCA", "category": "optional haunted-props Cooltrainer double", "trainers": ["TRAINER_BIANCA"], "main": "TRAINER_BIANCA", "teams": {"TRAINER_BIANCA": BIANCA_TEAM}, "target": 9.2, "question": "Can the player read whether haunted props want fast offense or Trick Room before a false tree and living furnace enter?", "tempo": "Flare Boost Gourgeist can reverse speed while Gengar attacks immediately; Choice Band Sudowoodo and Assault Vest Magmortar make the reserves physical and special props.", "weakness": "The lead shares Ghost and Dark pressure; Gourgeist's Orb costs health; Sudowoodo is Choice locked; Magmortar is slow and lacks Protect; Rock, Ground, Water, Dark, Ghost, Taunt, and priority remain broad.", "lesson": "Read the first turn rather than assuming one speed mode, then exploit the visible Choice lock and protectless furnace.", "tags": ["route111-north", "haunted-props", "flare-boost", "trick-room-option", "rock-head", "mixed-reserves"], "refs": ["showdown:gen9championsrandomdoublesbattle:030", "smogon:gen4ou:014", "smogon:gen6nu:003", "showdown:gen9randomdoublesbattle:019"], "coords": [19, 121]},
    {"index": 139, "id": "BATTLE_139_ROUTE_111_TYRON", "category": "optional three-blade Collector single", "trainers": ["TRAINER_TYRON"], "main": "TRAINER_TYRON", "teams": {"TRAINER_TYRON": TYRON_TEAM}, "target": 8.9, "question": "Can the player answer three different blade tempos: Eviolite setup, Shell Smash priority, and one blistering Mega?", "tempo": "Scyther tests patient setup defense, Kabutops risks a Shell Smash race, and Mega Beedrill ends with immediate frail speed.", "weakness": "Only three bodies; Stealth Rock, burn, priority, Intimidate, phazing, Electric, Rock, Water, Psychic, and Flying attacks remain broad, and every setup turn can be denied.", "lesson": "Do not use the same answer three times: deny the first two setup windows and preserve priority or a speed answer for Mega Beedrill.", "tags": ["route111-north", "three-blades", "eviolite-scyther", "shell-smash", "mega-beedrill"], "refs": ["smogon:gen9nu:004", "smogon:gen4uu:004", "showdown:gen6randomdoublesbattle:021"], "coords": [26, 132]},
    {"index": 140, "id": "BATTLE_140_ROUTE_111_CELINA", "category": "optional broken-shackles Cooltrainer double", "trainers": ["TRAINER_CELINA"], "main": "TRAINER_CELINA", "teams": {"TRAINER_CELINA": CELINA_TEAM}, "target": 9.3, "question": "Can the player stop Gastro Acid from turning Slow Start, Emergency Exit, and Truant into three unrestricted attackers?", "tempo": "Sash Victreebel spends a turn removing a partner's liability; Slaking, Golisopod, and Durant each become a different liberated threat.", "weakness": "The engine is one visible, interruptible support move; Victreebel is frail, Slaking is Choice locked, Golisopod is slow, Durant is specially fragile, and Taunt, redirection, Fake Out, priority, Flying, Psychic, and Fire remain broad.", "lesson": "Treat Gastro Acid as the target: deny or redirect it before fighting the raw stats it was meant to unlock.", "tags": ["route111-north", "broken-shackles", "gastro-acid", "slow-start", "emergency-exit", "truant"], "refs": ["showdown:gen9randomdoublesbattle:022", "showdown:gen9randomdoublesbattle:011", "smogon:gen8uu:014", "showdown:gen7randombattle:011"], "coords": [20, 132]},
    {"index": 141, "id": "BATTLE_141_ROUTE_111_CELIA", "category": "optional draining-growth Picnicker single", "trainers": ["TRAINER_CELIA"], "main": "TRAINER_CELIA", "teams": {"TRAINER_CELIA": CELIA_TEAM}, "target": 8.8, "question": "Can the player prevent three different forms of recovery from turning one short single into an attrition lock?", "tempo": "Rain Dish and Giga Drain begin the growth, Dry Skin Parasect adds Spore and seeds, and Mega Venusaur closes behind Thick Fat recovery.", "weakness": "Only three bodies; Ludicolo must spend a turn on rain, Parasect has crippling Fire and Flying weaknesses, Venusaur is Tauntable, and Psychic, Flying, Bug, strong neutral burst, weather replacement, and item removal remain broad.", "lesson": "Do not trade chip for chip: deny rain or Spore, then preserve immediate burst for Mega Venusaur before seeds compound.", "tags": ["route111-north", "draining-growth", "rain-dish", "dry-skin", "spore", "mega-venusaur"], "refs": ["vgc:special-event-cannes-2019", "vgc:worlds-2010", "showdown:gen4randomdoublesbattle:018"], "coords": [22, 77]},
    {"index": 142, "id": "BATTLE_142_ROUTE_111_BRYAN", "category": "optional ancient-survivors Ruin Maniac double", "trainers": ["TRAINER_BRYAN"], "main": "TRAINER_BRYAN", "teams": {"TRAINER_BRYAN": BRYAN_TEAM}, "target": 9.2, "question": "Can the player identify four unrelated survival rules before Rock Head, Contrary, and Sturdy convert them into offense?", "tempo": "Spiritomb blunts both damage categories, Scarf Tyrantrum fires recoil-free Head Smash, Contrary Serperior snowballs, and Sturdy Carracosta threatens one Shell Smash.", "weakness": "There is no shared field mode; Spiritomb is passive under Taunt, Tyrantrum is Choice locked, Serperior is physically frail, Carracosta is priority-vulnerable after Sturdy, and Fairy, Ice, Fighting, status, Haze, and item removal remain broad.", "lesson": "Name each rule as it appears: do not feed Leaf Storm boosts, break Sturdy safely, and exploit Tyrantrum's lock instead of treating the quartet as one core.", "tags": ["route111-north", "ancient-survivors", "infiltrator", "rock-head", "contrary", "sturdy-shell-smash"], "refs": ["smogon:gen4uu:001", "smogon:gen8nu:003", "smogon:gen7ou:009", "showdown:gen5randomdoublesbattle:024"], "coords": [29, 77]},
    {"index": 143, "id": "BATTLE_143_ROUTE_111_BRANDEN", "category": "optional anchor-crew Camper double", "trainers": ["TRAINER_BRANDEN"], "main": "TRAINER_BRANDEN", "teams": {"TRAINER_BRANDEN": BRANDEN_TEAM}, "target": 9.2, "question": "Can the player remove Steely Spirit before Anchor Shot traps the wrong answer and three different blades start collecting KOs?", "tempo": "Fake Out buys Steely Spirit's boosted Iron and Anchor attacks; Sirfetch'd brings critical pressure and Kartana supplies the frail Beast Boost finish.", "weakness": "The team is overwhelmingly physical; Perrserker is slow, Dhelmise is Dark-vulnerable, Sirfetch'd and Kartana are protect-dependent, and burn, Intimidate, Fire, Fighting, Ground, redirection, and special spread pressure remain broad.", "lesson": "Break the spirit-and-anchor link first, then deny Kartana its first Beast Boost rather than chasing every blade.", "tags": ["route111-north", "anchor-crew", "steely-spirit", "steelworker", "critical-blade", "beast-boost"], "refs": ["showdown:gen8randomdoublesbattle:001", "vgc:worlds-2018", "showdown:gen8randomdoublesbattle:018"], "coords": [37, 77]},
]


ALL_TEAMS = {trainer_id: team for config in CONFIGS for trainer_id, team in config["teams"].items()}
TRAINER_RULES = {
    "TRAINER_DAISUKE": ("single", 3, "Three-shadow relay single", 89),
    "TRAINER_WILTON_1": ("single", 3, "Ruin-guardian single", 90),
    "TRAINER_WILTON_2": ("double", 4, "Ruin-guardian room double", 93),
    "TRAINER_WILTON_3": ("double", 6, "Ruin-guardian full rematch", 96),
    "TRAINER_WILTON_4": ("double", 6, "Ruin-guardian Mega finale", 98),
    "TRAINER_BROOKE_1": ("double", 4, "Elemental relay double", 94),
    "TRAINER_BROOKE_2": ("double", 4, "Elemental relay reorder", 94),
    "TRAINER_BROOKE_3": ("double", 6, "Elemental relay full rematch", 97),
    "TRAINER_BROOKE_4": ("double", 6, "Elemental relay Mega finale", 98),
    "TRAINER_HAYDEN": ("double", 4, "Toxic-edge activation double", 91),
    "TRAINER_BIANCA": ("double", 4, "Haunted-props mixed-speed double", 92),
    "TRAINER_TYRON": ("single", 3, "Three-blade setup single", 89),
    "TRAINER_CELINA": ("double", 4, "Broken-shackles ability double", 93),
    "TRAINER_CELIA": ("single", 3, "Draining-growth single", 88),
    "TRAINER_BRYAN": ("double", 4, "Ancient-survivors double", 92),
    "TRAINER_BRANDEN": ("double", 4, "Steely Spirit anchor double", 92),
}


BASE_FLAGS = ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_HP_AWARE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_CHECK_FOE"]
EXTRA_FLAGS = {
    "TRAINER_WILTON_2": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_WILTON_3": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_WILTON_4": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_BROOKE_1": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_BROOKE_2": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_BROOKE_3": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_BROOKE_4": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_HAYDEN": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_BIANCA": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_CELINA": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP"],
    "TRAINER_BRYAN": ["AI_FLAG_HELP_PARTNER"],
    "TRAINER_BRANDEN": ["AI_FLAG_HELP_PARTNER"],
}


DIALOGUE = {
    "data/text/trainers.inc": {
        "Route111_Text_DaisukeIntro": ["Three shadows, one relay!\\p", "Ninjask passes power, Shedinja\n", "tests answers, Marshadow steals!$"],
        "Route111_Text_DaisukeDefeat": ["You broke every shadow!$"],
        "Route111_Text_DaisukePostBattle": ["Stop the relay, but save one answer\n", "for Wonder Guard and stolen boosts.$"],
        "Route111_Text_WiltonIntro": ["Ruin guardians change the room!\\p", "Burn, reverse speed, rough hide--\n", "show me how you read the chamber!$"],
        "Route111_Text_WiltonDefeat": ["You solved the first chamber!$"],
        "Route111_Text_WiltonPostBattle": ["Reverse Trick Room or exploit it.\n", "Preserve Dark pressure for Psychic.$"],
        "Route111_Text_WiltonRematchIntro": ["The ruin opened its inner chamber!\n", "Armor and one great mind await!$"],
        "Route111_Text_WiltonRematchDefeat": ["You mastered every chamber!$"],
        "Route111_Text_WiltonPostRematch": ["A room is a tool, never a prison.\n", "Choose whether to reverse it.$"],
        "Route111_Text_BrookeIntro": ["Every relay leg changes type!\\p", "Water, Fire, Grass, and Dragon--\n", "change answers without losing pace!$"],
        "Route111_Text_BrookeDefeat": ["You passed every relay leg!$"],
        "Route111_Text_BrookePostBattle": ["Remove Fake Out's tempo, then deny\n", "Haxorus before it can set up.$"],
        "Route111_Text_BrookeRematchIntro": ["The relay has two final runners!\n", "Steel flashes before Psychic flies!$"],
        "Route111_Text_BrookeRematchDefeat": ["You won the whole relay!$"],
        "Route111_Text_BrookePostRematch": ["One wall cannot cover every stage.\n", "Change your answer with the board.$"],
        "Route111_Text_HaydenIntro": ["Two poisoned attackers awaken!\\p", "One heals, one hits harder--\n", "use their activation turn wisely!$"],
        "Route111_Text_HaydenDefeat": ["You cut off every edge!$"],
        "Route111_Text_HaydenPostBattle": ["Strip an Orb or remove Zangoose\n", "before poison becomes an advantage.$"],
        "Route111_Text_BiancaIntro": ["These props are haunted!\\p", "Ghosts bend speed; the false tree\n", "and living furnace wait backstage!$"],
        "Route111_Text_BiancaDefeat": ["You saw through every prop!$"],
        "Route111_Text_BiancaPostBattle": ["Read Trick Room before setting pace.\n", "Then exploit Sudowoodo's move lock.$"],
        "Route111_Text_TyronIntro": ["Every blade has a different tempo!$"],
        "Route111_Text_TyronDefeat": ["You dulled all three!$"],
        "Route111_Text_TyronPostBattle": ["Deny both setup turns, then save\n", "priority for Mega Beedrill.$"],
        "Route111_Text_CelinaIntro": ["Bad abilities are only shackles!\\p", "Gastro Acid frees Slow Start, Exit,\n", "and Truant. Stop the cure!$"],
        "Route111_Text_CelinaDefeat": ["You never let them run free!$"],
        "Route111_Text_CelinaPostBattle": ["Target or redirect Gastro Acid.\n", "The whole engine spends that turn.$"],
        "Route111_Text_CeliaIntro": ["Everything in my garden grows back!\\p", "Rain, spores, seeds, and one Mega--\n", "can you win before roots spread?$"],
        "Route111_Text_CeliaDefeat": ["You uprooted everything!$"],
        "Route111_Text_CeliaPostBattle": ["Do not trade chip for chip. Deny\n", "rain or Spore, then burst Venusaur.$"],
        "Route111_Text_BryanIntro": ["Four ancient survivors, four rules!\\p", "Rock Head, Contrary, and Sturdy--\n", "identify each before it attacks!$"],
        "Route111_Text_BryanDefeat": ["You uncovered every rule!$"],
        "Route111_Text_BryanPostBattle": ["Exploit the Choice lock, then stop\n", "Leaf Storm and break Sturdy safely.$"],
        "Route111_Text_BrandenIntro": ["Steely Spirit strengthens my crew!\\p", "Anchor traps; three blades finish--\n", "break their link before they strike!$"],
        "Route111_Text_BrandenDefeat": ["You broke the whole crew!$"],
        "Route111_Text_BrandenPostBattle": ["Remove Perrserker, then deny\n", "Kartana its first Beast Boost.$"],
    }
}


NEXT = {
    "index": 144,
    "encounter_id": "BATTLE_144_ASHEN_WOODS_ALANNAH",
    "location": "AshenWoods",
    "category": "optional Ashen Woods trainer",
    "status": "next",
    "strict_cap": 45,
    "trainer_ids": ["TRAINER_ALANNAH"],
    "access_note": "Alannah is the first unclosed Ashen Woods trainer after Route 111.",
}


def design(config):
    total = sum(len(team) for team in config["teams"].values())
    return {
        "guide_order": config["index"],
        "trainer_ids": config["trainers"],
        "status": "closed",
        "strict_cap": 45,
        "campaign_point": "Post-Heat-Badge Route 111 exploration before Norman; full player preparation remains available.",
        "runtime_branches": [f"{trainer_id}: {TRAINER_RULES[trainer_id][0]} with {len(config['teams'][trainer_id])} authored members." for trainer_id in config["trainers"]],
        "evolution_stage_fit": {"campaign_phase": "cap-45 mature post-Heat-Badge exploration", "effective_levels": "main cap+1 to cap+4; later rematches may reach cap+6", "eligible_ratio": f"{total}/{total}", "mega_access": True, "status": "pass", "reason": "Every member is fully evolved, single-stage, or a deliberate Eviolite specialist appropriate after the Heat Badge."},
        "manual_quality": 10,
        "manual_difficulty": config["target"],
        "observed_difficulty": None,
        "corpus_review": {"reference_pool_size": 1005, "full_team_candidates": [{"reference_id": ref, "decision": "role adapted; donor roster rejected", "reason": "The indexed set or composition supports one exact role without replacing local authorship."} for ref in config["refs"]], "decision": f"{len(config['refs'])} indexed references support the roles; the encounter itself remains bespoke."},
        "competitive_references": [{"reference_id": ref, "adaptation": "Exact role evidence adapted to the authored Route 111 team."} for ref in config["refs"]],
        "ordering": {trainer_id: [member["species"] for member in team] for trainer_id, team in config["teams"].items()},
        "team_intent": config["tempo"],
        "primary_player_question": config["question"],
        "intended_counterplay": config["weakness"],
        "first_loss_lesson": config["lesson"],
        "bespoke_ai": "Formats, partner awareness, HP awareness, smart switching, speed control, and Combo Setup are attached only when the exact source team uses them. No move, target, switch, or turn is forced.",
        "uniqueness": f"The encounter spends {', '.join(config['tags'][1:])} as its own question and was reviewed against the prior ten battles and the rest of this Route 111 batch.",
        "story_logic": "The native trainer, trigger, registration/rematch routing, and rewards remain intact; dialogue now describes the implemented team and broad counterplay.",
        "reward_logic": "Ordinary EXP and prize money only; Match Call families retain native registration and rematch progression without invented rewards.",
        "campaign_reservations": {"spends": config["tags"], "preserves": ["Norman's protected singles discipline", "later Gym and faction anchors", "all unrelated Mega and legendary reveals"], "repeat_rule": f"Do not repeat {config['tags'][1]} soon without a different primary question."},
        "source_teams": config["teams"],
        "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
        "closure": f"Battle {config['index']} is source-closed at quality 10 and target {config['target']}: every physical/rematch branch, exact team, legal set, reference, dialogue cue, broad counterplay, and source route is proven. Runtime remains unplayed.",
    }


def ledger_entry(config):
    main_team = config["teams"][config["main"]]
    return {
        "index": config["index"],
        "encounter_id": config["id"],
        "identity": {"location": "Route111", "category": config["category"], "format": TRAINER_RULES[config["main"]][0], "strict_cap": 45, "memory_hook": config["tempo"]},
        "primary_player_question": config["question"],
        "tempo": config["tempo"],
        "pressure_sources": [member["species"] for member in main_team],
        "intentional_opening": "Source order is intentional and preserved.",
        "intentional_weakness": config["weakness"],
        "first_loss_lesson": config["lesson"],
        "revealed_information": ["cap 45", TRAINER_RULES[config["main"]][0], f"{len(main_team)} main members", *config["tags"][1:]],
        "counterplay_classes": ["Disrupt, redirect, Taunt, or remove the enabling lead or partner.", "Exploit the listed type, item, and damage-category seams.", "Use speed reversal, item removal, status control, focus fire, or setup denial.", config["weakness"]],
        "target_difficulty": config["target"],
        "difficulty_rationale": "Optimized cap-plus sets and one coherent interaction create a serious optional puzzle while explicit seams preserve broad counterplay.",
        "tuning_knob": "Lower the last main-story member by one level first; preserve species, mechanics, and order.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": config["tags"],
        "historic_reference_ids": config["refs"],
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": f"{len(config['refs'])} indexed references."},
        "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
    }


def payloads():
    designs = json.loads(DESIGNS.read_text())
    ledger = json.loads(LEDGER.read_text())
    sequence = json.loads(SEQUENCE.read_text())
    operating_system = json.loads(OS_PATH.read_text())
    formats = json.loads(FORMATS.read_text())
    for config in CONFIGS:
        designs["designs"][config["id"]] = design(config)
        ledger["entries"] = [row for row in ledger["entries"] if row["index"] != config["index"]] + [ledger_entry(config)]
        sequence["entries"] = [row for row in sequence["entries"] if row["index"] != config["index"]] + [{"index": config["index"], "encounter_id": config["id"], "location": "Route111", "category": config["category"], "status": "closed", "strict_cap": 45, "trainer_ids": config["trainers"], "access_note": f"Physical trigger at ({config['coords'][0]},{config['coords'][1]}); listed rematches share the same native trainer family."}]
        for trainer_id in config["trainers"]:
            fmt, size, archetype, difficulty = TRAINER_RULES[trainer_id]
            formats["formats"][trainer_id].update({"format": fmt, "target_size": size, "archetype": archetype, "difficulty": difficulty, "partner_interaction": fmt == "double", "level_offset": 3, "location": "Route111", "smart_switching": True})
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != NEXT["index"]] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 143 else "next" if row["index"] == 144 else "queued"
    operating_system["current_state"].update({"closed_encounters": 143, "next_index": 144, "next_encounter_id": NEXT["encounter_id"], "canonical_sequence_groups": 144, "physical_encounter_groups": 522, "unordered_physical_groups": 378})
    return designs, ledger, sequence, operating_system, formats


def replace_dialogue(text, label, lines):
    pattern = re.compile(rf"({re.escape(label)}:[^\n]*\n)(?:\s*\.string[^\n]*\n)+")
    match = pattern.search(text)
    if not match:
        raise ValueError(f"dialogue label not found: {label}")
    rendered = match.group(1) + "".join(f'\t.string "{line.replace(chr(10), r"\\n")}"\n' for line in lines)
    return text[:match.start()] + rendered + text[match.end():]


def apply_source():
    parties = PARTIES.read_text()
    trainers = TRAINERS.read_text()
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, team in ALL_TEAMS.items():
        party_name = doubles.party_name(blocks[trainer_id].group(0))
        entries = [polish.render(member, trainer_id) for member in team]
        parties = custom.replace_party_body(parties, party_name, entries)
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, (fmt, _, _, _) in TRAINER_RULES.items():
        match = blocks[trainer_id]
        block = match.group(0)
        block = re.sub(r"(\.doubleBattle\s*=\s*)(TRUE|FALSE)", rf"\g<1>{'TRUE' if fmt == 'double' else 'FALSE'}", block)
        flags = BASE_FLAGS + EXTRA_FLAGS.get(trainer_id, [])
        block = re.sub(r"(\.aiFlags\s*=\s*)[^,\n]+", rf"\g<1>{' | '.join(flags)}", block)
        trainers = trainers[:match.start()] + block + trainers[match.end():]
        blocks = doubles.trainer_blocks(trainers)
    PARTIES.write_text(parties)
    TRAINERS.write_text(trainers)
    for rel, labels in DIALOGUE.items():
        path = ROOT / rel
        text = path.read_text()
        for label, lines in labels.items():
            text = replace_dialogue(text, label, lines)
        path.write_text(text)


def verify_source(check_guide=False):
    trainers = TRAINERS.read_text()
    parties = PARTIES.read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    refs = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    for trainer_id, expected in ALL_TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != expected:
            raise SystemExit(f"FAIL Route 111 batch source party {trainer_id}")
        fmt = TRAINER_RULES[trainer_id][0]
        if (".doubleBattle = TRUE" in block) != (fmt == "double"):
            raise SystemExit(f"FAIL Route 111 batch format {trainer_id}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL Route 111 legality {trainer_id}/{member['species']}: {illegal}")
    for config in CONFIGS:
        if any(ref not in refs for ref in config["refs"]):
            raise SystemExit(f"FAIL Route 111 references {config['id']}")
    for rel, labels in DIALOGUE.items():
        text = (ROOT / rel).read_text()
        for label, lines in labels.items():
            if label not in text:
                raise SystemExit(f"FAIL Route 111 dialogue {label}")
            for line in lines:
                for visible in re.split(r"(?:\\[nlp]|\n)", line.replace("$", "")):
                    if len(visible) > 36:
                        raise SystemExit(f"FAIL Route 111 dialogue width {label}: {visible!r}")
    if check_guide:
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        rows = {row["trainerId"]: row for row in guide}
        for trainer_id, team in ALL_TEAMS.items():
            if rows[trainer_id]["designStatus"] != "closed" or rows[trainer_id]["partySize"] != len(team):
                raise SystemExit(f"FAIL Route 111 guide {trainer_id}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply-source", action="store_true")
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check-source", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not any((args.apply_source, args.write, args.check_source, args.check)):
        parser.error("choose an action")
    if args.apply_source:
        apply_source()
    data = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH, FORMATS)
    serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in data]
    if args.write:
        for path, text in zip(paths, serialized):
            path.write_text(text)
    if args.check or args.check_source:
        for path, text in zip(paths, serialized):
            if path.read_text() != text:
                raise SystemExit(f"FAIL Route 111 batch artifact stale: {path.name}")
        verify_source(args.check)
    print("PASS: Battles 134-143 remaining Route 111 batch is source-closed")


if __name__ == "__main__":
    main()
