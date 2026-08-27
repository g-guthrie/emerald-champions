#!/usr/bin/env python3
"""Replace now-free held-item rewards with finite progression rewards."""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Reward:
    path: str
    label: str
    old_item: str
    new_item: str


REWARDS = (
    Reward("data/maps/Route104/scripts.inc", "Route104_EventScript_WhiteHerbFlorist", "ITEM_WHITE_HERB", "ITEM_LEAF_STONE"),
    Reward("data/maps/RustboroCity_Gym/scripts.inc", "RustboroCity_Gym_EventScript_GiveRockTomb", "ITEM_EXPERT_BELT", "ITEM_PROTECTOR"),
    Reward("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_EventScript_Teacher", "ITEM_QUICK_CLAW", "ITEM_SUN_STONE"),
    Reward("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "RustboroCity_DevonCorp_3F_EventScript_GiveScopeLens", "ITEM_SCOPE_LENS", "ITEM_UPGRADE"),
    Reward("data/maps/RustboroCity_Mart/scripts.inc", "RustboroCity_Mart_EventScript_FalseSwipeTM", "ITEM_ZOOM_LENS", "ITEM_MOON_STONE"),
    Reward("data/maps/DewfordTown_Hall/scripts.inc", "DewfordTown_Hall_EventScript_SludgeBombMan", "ITEM_BLACK_SLUDGE", "ITEM_DEEP_SEA_SCALE"),
    Reward("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_GiveBulkUp", "ITEM_FLAME_ORB", "ITEM_RAZOR_CLAW"),
    Reward("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_GiveBulkUp2", "ITEM_FLAME_ORB", "ITEM_RAZOR_CLAW"),
    Reward("data/maps/DewfordTown/scripts.inc", "DewfordTown_EventScript_OldRodFisherman", "ITEM_SCOPE_LENS", "ITEM_DRAGON_SCALE"),
    Reward("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_EventScript_Man", "ITEM_SILK_SCARF", "ITEM_REAPER_CLOTH"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle1", "ITEM_MENTAL_HERB", "ITEM_SACHET"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle1Reward", "ITEM_MENTAL_HERB", "ITEM_SACHET"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle3", "ITEM_ROOM_SERVICE", "ITEM_WHIPPED_DREAM"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle3Reward", "ITEM_ROOM_SERVICE", "ITEM_WHIPPED_DREAM"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle4", "ITEM_ROOM_SERVICE", "ITEM_METAL_ALLOY"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle4Reward", "ITEM_ROOM_SERVICE", "ITEM_METAL_ALLOY"),
    Reward("data/scripts/item_ball_scripts.inc", "TrickHouse5_EventScript_TerrainExtender", "ITEM_TERRAIN_EXTENDER", "ITEM_FOSSILIZED_BIRD"),
    Reward("data/scripts/item_ball_scripts.inc", "Route110_TrickHousePuzzle3_EventScript_ItemExpertBelt", "ITEM_EXPERT_BELT", "ITEM_FOSSILIZED_DRAKE"),
    Reward("data/scripts/item_ball_scripts.inc", "Route110_TrickHousePuzzle4_EventScript_ItemAssaultVest", "ITEM_BLUNDER_POLICY", "ITEM_FOSSILIZED_FISH"),
    Reward("data/scripts/item_ball_scripts.inc", "Route109_EventScript_ItemZoomLens", "ITEM_ZOOM_LENS", "ITEM_FOSSILIZED_DINO"),
    Reward("data/maps/SlateportCity_PokemonFanClub/scripts.inc", "SlateportCity_PokemonFanClub_EventScript_EndureTM", "ITEM_FOCUS_BAND", "ITEM_DEEP_SEA_TOOTH"),
    Reward("data/maps/SlateportCity_BattleTentLobby/scripts.inc", "SlateportCity_BattleTentLobby_EventScript_TormentGiver", "ITEM_RED_CARD", "ITEM_PRISM_SCALE"),
    Reward("data/maps/SlateportCity/scripts.inc", "SlateportCity_EventScript_GretaReward", "ITEM_THROAT_SPRAY", "ITEM_LEADERS_CREST"),
    Reward("data/maps/Route110/scripts.inc", "Route110_EventScript_ChallengeReactionBest", "ITEM_ELECTRIC_SEED", "ITEM_BOTTLE_CAP"),
    Reward("data/maps/Route110/scripts.inc", "Route110_EventScript_ChallengeReactionGood", "ITEM_QUICK_CLAW", "ITEM_RARE_CANDY"),
    Reward("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_EventScript_GyroBallTM", "ITEM_METRONOME", "ITEM_DUBIOUS_DISC"),
    Reward("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_EventScript_CompletedNewMauville", "ITEM_THROAT_SPRAY", "ITEM_GIMMIGHOUL_COIN"),
    Reward("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_GiveVoltSwitch", "ITEM_WISE_GLASSES", "ITEM_ELECTIRIZER"),
    Reward("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_GiveVoltSwitch2", "ITEM_WISE_GLASSES", "ITEM_ELECTIRIZER"),
    Reward("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_EventScript_PlayerHasMeteorite", "ITEM_EXPERT_BELT", "ITEM_DAWN_STONE"),
    Reward("data/maps/FallarborTown_Mart/scripts.inc", "FallarborTown_Mart_EventScript_DrainPunchTM", "ITEM_MUSCLE_BAND", "ITEM_SHINY_STONE"),
    Reward("data/maps/Route114_FossilManiacsHouse/scripts.inc", "Route114_FossilManiacsHouse_EventScript_FossilManiacsBrother", "ITEM_AIR_BALLOON", "ITEM_SAIL_FOSSIL"),
    Reward("data/scripts/item_ball_scripts.inc", "Route114_EventScript_ItemProtectivePads", "ITEM_PROTECTIVE_PADS", "ITEM_METAL_COAT"),
    Reward("data/maps/Route114/scripts.inc", "Route114_EventScript_RoarGentleman", "ITEM_SHED_SHELL", "ITEM_DRAGON_SCALE"),
    Reward("data/maps/LavaridgeTown_HerbShop/scripts.inc", "LavaridgeTown_HerbShop_EventScript_OldMan", "ITEM_CHARCOAL", "ITEM_FIRE_STONE"),
    Reward("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_EventScript_GiveOverheat", "ITEM_EJECT_PACK", "ITEM_MAGMARIZER"),
    Reward("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_EventScript_GiveOverheat2", "ITEM_EJECT_PACK", "ITEM_MAGMARIZER"),
    Reward("data/maps/VerdanturfTown_BattleTentLobby/scripts.inc", "VerdanturfTown_BattleTentLobby_EventScript_AttractGiver", "ITEM_BRIGHT_POWDER", "ITEM_SHINY_STONE"),
    Reward("data/maps/VerdanturfTown_Mart/scripts.inc", "VerdanturfTown_Mart_EventScript_PaybackTM", "ITEM_WEAKNESS_POLICY", "ITEM_GIMMIGHOUL_COIN"),
    Reward("data/maps/PetalburgCity_Gym/scripts.inc", "PetalburgCity_Gym_EventScript_GiveFacade", "ITEM_OVAL_STONE", "ITEM_LOPUNNITE"),
    Reward("data/scripts/item_ball_scripts.inc", "Ashen_Woods_ItemFlame_Orb", "ITEM_FLAME_ORB", "ITEM_SACHET"),
    Reward("data/scripts/item_ball_scripts.inc", "Route119_EventScript_ItemToxicOrb", "ITEM_TOXIC_ORB", "ITEM_REAPER_CLOTH"),
    Reward("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_GiveRoost", "ITEM_SHINY_STONE", "ITEM_ALTARIANITE"),
    Reward("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_GiveRoost2", "ITEM_SHINY_STONE", "ITEM_ALTARIANITE"),
    Reward("data/maps/FortreeCity_House4/scripts.inc", "FortreeCity_House4_EventScript_WingullReturned", "ITEM_MENTAL_HERB", "ITEM_SACHET"),
    Reward("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_EventScript_SleepTalkTM", "ITEM_SAFETY_GOGGLES", "ITEM_ICE_STONE"),
    Reward("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_EventScript_HiddenPowerGiver", "ITEM_WIDE_LENS", "ITEM_DUBIOUS_DISC"),
    Reward("data/maps/FortreeCity_Mart/scripts.inc", "FortreeCity_Mart_EventScript_SpenserReward", "ITEM_TERRAIN_EXTENDER", "ITEM_PROTECTOR"),
    Reward("data/scripts/item_ball_scripts.inc", "Route123_EventScript_ItemWideLens", "ITEM_WIDE_LENS", "ITEM_DAWN_STONE"),
    Reward("data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc", "LilycoveCity_DepartmentStoreRooftop_EventScript_SubstituteTM", "ITEM_PROTECTIVE_PADS", "ITEM_METAL_ALLOY"),
    Reward("data/scripts/item_ball_scripts.inc", "MagmaHideout_1F_EventScript_ItemRareCandy", "ITEM_BOOSTER_ENERGY", "ITEM_FOSSILIZED_DRAKE"),
    Reward("data/maps/ShoalCave_LowTideLowerRoom/scripts.inc", "ShoalCave_LowTideLowerRoom_EventScript_BlackBelt", "ITEM_FOCUS_BAND", "ITEM_DEEP_SEA_SCALE"),
    Reward("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_GiveCalmMind", "ITEM_DAWN_STONE", "ITEM_METAGROSSITE"),
    Reward("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_GiveCalmMind2", "ITEM_DAWN_STONE", "ITEM_METAGROSSITE"),
    Reward("data/maps/MossdeepCity/scripts.inc", "MossdeepCity_EventScript_AvalancheTM", "ITEM_SNOWBALL", "ITEM_ICE_STONE"),
    Reward("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_EventScript_BrickBreakBlackBelt", "ITEM_SCOPE_LENS", "ITEM_RAZOR_CLAW"),
    Reward("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_GiveScald", "ITEM_PRISM_SCALE", "ITEM_MILOTICITE"),
    Reward("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_GiveScald2", "ITEM_PRISM_SCALE", "ITEM_MILOTICITE"),
    Reward("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_EventScript_SnatchGiver", "ITEM_SAFETY_GOGGLES", "ITEM_REAPER_CLOTH"),
    Reward("data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc", "PacifidlogTown_PokemonCenter_1F_EventScript_ExplosionTM", "ITEM_ROOM_SERVICE", "ITEM_UPGRADE"),
    Reward("data/scripts/item_ball_scripts.inc", "AbandonedShip_Rooms_1F_EventScript_ItemLopunnite", "ITEM_LOPUNNITE", "ITEM_OVAL_STONE"),
    Reward("data/scripts/item_ball_scripts.inc", "MossdeepCity_EventScript_ItemMiloticite", "ITEM_MILOTICITE", "ITEM_PRISM_SCALE"),
    Reward("data/scripts/item_ball_scripts.inc", "VictoryRoad_1F_EventScript_ItemMetagrossite", "ITEM_METAGROSSITE", "ITEM_GOLD_BOTTLE_CAP"),
    Reward("data/maps/LilycoveCity/scripts.inc", "LilycoveCity_EventScript_YesAltaria", "ITEM_ALTARIANITE", "ITEM_BOTTLE_CAP"),
)


HIDDEN_REWARDS = {
    ("data/maps/Route116/map.json", "ITEM_BLACK_GLASSES"): "ITEM_DUSK_STONE",
    ("data/maps/Route117/map.json", "ITEM_BRIGHT_POWDER"): "ITEM_MOON_STONE",
    ("data/maps/Route123/map.json", "ITEM_BLACK_SLUDGE"): "ITEM_DUSK_STONE",
}


DIALOGUE_REWRITES = {
    ("data/maps/Route104/scripts.inc", "Route104_Text_DontNeedThisTakeIt"): ("I found this Leaf Stone among the\\n", "flowers. You should have it!$"),
    ("data/maps/RustboroCity_Gym/scripts.inc", "RustboroCity_Gym_Text_ExplainRockTomb"): ("Protector evolves Rhydon when it\\n", "levels up while holding it.$"),
    ("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_Text_StudentsWhoDontStudyGetQuickClaw"): ("A good student learns when to grow.\\p", "Take this Sun Stone and experiment!$"),
    ("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_Text_ExplainQuickClaw"): ("A Sun Stone evolves certain Pokémon.\\n", "Use it from your Bag when ready.$"),
    ("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "RustboroCity_DevonCorp_3F_Text_ExplainScopeLens"): ("The Up-Grade evolves Porygon when it\\n", "levels up while holding it.$"),
    ("data/maps/RustboroCity_Mart/scripts.inc", "RustboroCity_Mart_Text_ExcuseMeTrainer"): ("Excuse me, Trainer!\\p", "I'm offering one free Moon Stone today.\\n", "Put it to good use!$"),
    ("data/maps/DewfordTown_Hall/scripts.inc", "DewfordTown_Hall_Text_LoveSludgeBombButTrendInToo"): ("The Deep Sea Scale evolves Clamperl\\n", "into Gorebyss. That's the new trend!$"),
    ("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_Text_ExplainBulkUp"): ("A Razor Claw evolves Sneasel.\\p", "Level it at night while holding it.$"),
    ("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_Text_WantYouToHaveSilkScarf"): ("This Reaper Cloth feels strange…\\p", "I think you should take it.$"),
    ("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_Text_ExplainSilkScarf"): ("A Reaper Cloth evolves Dusclops when\\n", "it levels up while holding it.$"),
    ("data/maps/SlateportCity_PokemonFanClub/scripts.inc", "SlateportCity_PokemonFanClub_Text_GiveTM58"): ("Your Pokémon look remarkably sturdy!\\p", "Take this Deep Sea Tooth!$"),
    ("data/maps/SlateportCity_BattleTentLobby/scripts.inc", "SlateportCity_BattleTentLobby_Text_ExplainTorment"): ("A Prism Scale evolves Feebas when it\\n", "levels up while holding it.$"),
    ("data/maps/Route110/scripts.inc", "Route110_Text_GiveTM93"): ("Outstanding! Take this valuable\\n", "Bottle Cap as your prize.$"),
    ("data/maps/Route110/scripts.inc", "Route110_Text_GiveTM83"): ("Well ridden! Take this Rare Candy to\\n", "prepare another teammate.$"),
    ("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_Text_GiveTM74"): ("Here, let them try this Dubious Disc.\\n", "It evolves Porygon2 while held.$"),
    ("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_Text_HaveTM74"): ("A Dubious Disc evolves Porygon2 when\\n", "it levels up while holding it.$"),
    ("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_Text_WattsonThanksTakeTM"): ("This is my thanks--a rare Coin!\\p", "It unlocks an unusual evolution.$"),
    ("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_Text_ExplainVoltSwitch"): ("The Electirizer evolves Electabuzz.\\p", "Level it while holding this item.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_IsThatMeteoriteMayIHaveIt"): ("Is that the Meteorite? May I have it?\\p", "I'll trade you this Dawn Stone.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_PleaseUseThisTM"): ("A Dawn Stone evolves some Pokémon.\\n", "Please put it to good use.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_MayIHaveMeteorite"): ("May I have that Meteorite?\\p", "I'll trade you this Dawn Stone.$"),
    ("data/maps/FallarborTown_Mart/scripts.inc", "FallarborTown_Mart_Text_FreeSample"): ("How about this Shiny Stone as a free\\n", "sample? It evolves certain Pokémon.$"),
    ("data/maps/FallarborTown_Mart/scripts.inc", "FallarborTown_Mart_Text_HaveTM60"): ("A Shiny Stone evolves some Pokémon.\\n", "Use it from your Bag when ready.$"),
    ("data/maps/Route114_FossilManiacsHouse/scripts.inc", "Route114_FossilManiacsHouse_Text_HaveThisToDigLikeMyBrother"): ("My brother dug up this Sail Fossil.\\p", "Take it to Devon to restore its Pokémon!$"),
    ("data/maps/Route114/scripts.inc", "Route114_Text_AllMyMonDoesIsRoarTakeThis"): ("All my Pokémon does is Roar…\\p", "Please take this Dragon Scale away.$"),
    ("data/maps/LavaridgeTown_HerbShop/scripts.inc", "LavaridgeTown_HerbShop_Text_YouveComeToLookAtHerbalMedicine"): ("You've come to look at herbal medicine?\\p", "Then take this Fire Stone, too.$"),
    ("data/maps/LavaridgeTown_HerbShop/scripts.inc", "LavaridgeTown_HerbShop_Text_ExplainCharcoal"): ("A Fire Stone evolves certain Pokémon.\\n", "Use it from your Bag when ready.$"),
    ("data/maps/VerdanturfTown_Mart/scripts.inc", "VerdanturfTown_Mart_Text_SlowPokemon"): ("Do slow Pokémon tire of waiting?\\p", "Try this Gimmighoul Coin instead!$"),
    ("data/maps/VerdanturfTown_Mart/scripts.inc", "VerdanturfTown_Mart_Text_HaveTM66"): ("A Gimmighoul Coin unlocks one very\\n", "unusual evolution.$"),
    ("data/maps/PetalburgCity_Gym/scripts.inc", "PetalburgCity_Gym_Text_ExplainFacade"): ("Dad: Lopunnite enables Mega power.\\n", "Let Lopunny hold it in battle.$"),
    ("data/maps/FortreeCity_House4/scripts.inc", "FortreeCity_House4_Text_WelcomeWingullTakeMentalHerb"): ("Welcome back, Wingull! And thank you.\\p", "Please take this Sachet as my thanks.$"),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_Text_WantSleepTalk"): ("I have a spare Ice Stone.\\n", "Would you like it?$"),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_Text_GiveTM49"): ("Ah, an appreciative child!\\n", "Please take this Ice Stone.$"),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_Text_YourHiddenPowerHasAwoken"): ("Oh! Splendid! Your insight has awoken!\\p", "Take this Dubious Disc.$"),
    ("data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc", "LilycoveCity_DepartmentStoreRooftop_Text_GiveSubstitute"): ("I know! Take this Metal Alloy.\\n", "It evolves Duraludon while held.$"),
    ("data/maps/ShoalCave_LowTideLowerRoom/scripts.inc", "ShoalCave_LowTideLowerRoom_Text_CanOvercomeColdWithFocus"): ("Your focus overcame the cold!\\p", "Take this Deep Sea Scale.$"),
    ("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_Text_ExplainCalmMind"): ("Tate: Metagrossite lets Metagross Mega\\n", "Evolve. Let it hold the stone.$"),
    ("data/maps/MossdeepCity/scripts.inc", "MossdeepCity_Text_AvalancheNonsense"): ("An Ice Stone? Now that's a useful\\n", "reward. Take it!$"),
    ("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_Text_DevelopedThisTM"): ("I found this Razor Claw training.\\n", "You should have it.$"),
    ("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_Text_ExplainBrickBreak"): ("A Razor Claw evolves Sneasel.\\p", "Level it at night while holding it.$"),
    ("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_Text_NotSuspiciousTakeThis"): ("Uh… Hi! I'm not acting suspicious!\\p", "You can have this Reaper Cloth!$"),
    ("data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc", "PacifidlogTown_PokemonCenter_1F_Text_GiveExplosion"): ("I'm going to cause an Explosion of\\n", "popularity! Take this Up-Grade!$"),
    ("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_Text_ExplainRoost"): ("Altarianite lets Altaria Mega Evolve.\\n", "Let it hold the stone in battle.$"),
    ("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_Text_ExplainScald"): ("Miloticite lets Milotic Mega Evolve.\\n", "Let it hold the stone with elegance.$"),
    ("data/maps/LilycoveCity/scripts.inc", "LilycoveCity_Text_ExplainAltarianite"): ("Winona already entrusted you with her\\n", "Altarianite. Take this Bottle Cap for\\l", "showing me your Altaria instead!$"),
    ("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_Text_ExplainOverheat"): ("The Magmarizer evolves Magmar.\\p", "Level it while holding this item.$"),
}


def replace_in_label(text: str, label: str, old: str, new: str) -> str:
    pattern = rf"(^\s*{re.escape(label)}(?:::|:).*?)(?=^\s*[A-Za-z0-9_]+(?:::|:)|\Z)"
    match = re.search(pattern, text, re.M | re.S)
    if match is None:
        raise ValueError(f"missing reward label {label}")
    block = match.group(1)
    if new in block and old not in block:
        return text
    if block.count(old) < 1:
        raise ValueError(f"{label}: expected {old}")
    return text[:match.start(1)] + block.replace(old, new) + text[match.end(1):]


def rewrite() -> None:
    by_path: dict[str, list[Reward]] = {}
    for reward in REWARDS:
        by_path.setdefault(reward.path, []).append(reward)
    for path, rewards in by_path.items():
        target = ROOT / path
        text = target.read_text()
        for reward in rewards:
            text = replace_in_label(text, reward.label, reward.old_item, reward.new_item)
        target.write_text(text)

    for (path, old), new in HIDDEN_REWARDS.items():
        target = ROOT / path
        payload = json.loads(target.read_text())
        matches = [event for event in payload["bg_events"] if event.get("item") in {old, new}]
        if len(matches) != 1:
            raise ValueError(f"{path}: expected one hidden {old}/{new}, found {len(matches)}")
        matches[0]["item"] = new
        target.write_text(json.dumps(payload, indent=2) + "\n")

    by_path = {}
    for (path, label), lines in DIALOGUE_REWRITES.items():
        by_path.setdefault(path, []).append((label, lines))
    for path, entries in by_path.items():
        target = ROOT / path
        text = target.read_text()
        for label, lines in entries:
            pattern = rf"(^\s*{re.escape(label)}(?:::|:).*?)(?=^\s*[A-Za-z0-9_]+(?:::|:)|\Z)"
            match = re.search(pattern, text, re.M | re.S)
            if match is None:
                raise ValueError(f"missing dialogue label {label}")
            block = label + ":\n" + "".join(f'\t.string "{line}"\n' for line in lines)
            text = text[:match.start(1)] + block + text[match.end(1):]
        target.write_text(text)


def check() -> None:
    for reward in REWARDS:
        text = (ROOT / reward.path).read_text()
        match = re.search(
            rf"(^\s*{re.escape(reward.label)}(?:::|:).*?)(?=^\s*[A-Za-z0-9_]+(?:::|:)|\Z)",
            text,
            re.M | re.S,
        )
        if match is None or reward.new_item not in match.group(1) or reward.old_item in match.group(1):
            raise ValueError(f"reward rewrite stale: {reward.path} / {reward.label}")
    for (path, label), lines in DIALOGUE_REWRITES.items():
        block = (ROOT / path).read_text()
        if label not in block or not all(line in block for line in lines):
            raise ValueError(f"dialogue rewrite stale: {path} / {label}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    if args.write:
        rewrite()
    check()
    print(f"PASS: {len(REWARDS)} scripted and {len(HIDDEN_REWARDS)} hidden rewards are finite progression rewards")


if __name__ == "__main__":
    main()
