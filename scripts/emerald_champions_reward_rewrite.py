#!/usr/bin/env python3
"""Replace redundant TM rewards with finite campaign progression rewards."""

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
    item: str


REWARDS = (
    Reward("data/scripts/secret_power_tm.inc", "Route111_EventScript_GiveSecretPower", "ITEM_ULTRA_BALL"),
    Reward("data/maps/RustboroCity_Gym/scripts.inc", "RustboroCity_Gym_EventScript_GiveRoxanneRewards", "ITEM_AERODACTYLITE"),
    Reward("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_GiveLucarionite", "ITEM_LUCARIONITE"),
    Reward("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_GiveLucarionite2", "ITEM_LUCARIONITE"),
    Reward("data/maps/GraniteCave_StevensRoom/scripts.inc", "GraniteCave_StevensRoom_EventScript_CheckKnuckleBadge", "ITEM_MEGA_RING"),
    Reward("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_GiveManectite", "ITEM_MANECTITE"),
    Reward("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_GiveManectite2", "ITEM_MANECTITE"),
    Reward("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_EventScript_GiveCameruptite", "ITEM_CAMERUPTITE"),
    Reward("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_EventScript_GiveCameruptite2", "ITEM_CAMERUPTITE"),
    Reward("data/maps/PetalburgCity_Gym/scripts.inc", "PetalburgCity_Gym_EventScript_GiveLopunnite", "ITEM_LOPUNNITE"),
    Reward("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_GiveAltarianite", "ITEM_ALTARIANITE"),
    Reward("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_GiveAltarianite2", "ITEM_ALTARIANITE"),
    Reward("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_GiveMetagrossite", "ITEM_METAGROSSITE"),
    Reward("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_GiveMetagrossite2", "ITEM_METAGROSSITE"),
    Reward("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_GiveGyaradosite", "ITEM_GYARADOSITE"),
    Reward("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_GiveGyaradosite2", "ITEM_GYARADOSITE"),
    Reward("data/maps/Route114_FossilManiacsHouse/scripts.inc", "Route114_FossilManiacsHouse_EventScript_FossilManiacsBrother", "ITEM_METAL_COAT"),
    Reward("data/maps/SlateportCity_BattleTentLobby/scripts.inc", "SlateportCity_BattleTentLobby_EventScript_PrismScaleGiver", "ITEM_PRISM_SCALE"),
    Reward("data/maps/SlateportCity_OceanicMuseum_1F/scripts.inc", "SlateportCity_OceanicMuseum_1F_EventScript_FamiliarGrunt", "ITEM_DEEP_SEA_TOOTH"),
    Reward("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_EventScript_DubiousDiscGiver", "ITEM_DUBIOUS_DISC"),
    Reward("data/maps/VerdanturfTown_BattleTentLobby/scripts.inc", "VerdanturfTown_BattleTentLobby_EventScript_AttractGiver", "ITEM_SHINY_STONE"),
    Reward("data/maps/Route104/scripts.inc", "Route104_EventScript_Boy2", "ITEM_LEAF_STONE"),
    Reward("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_EventScript_PlayerHasMeteorite", "ITEM_DAWN_STONE"),
    Reward("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_EventScript_GiveReturn", "ITEM_SUN_STONE"),
    Reward("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_EventScript_GiveFrustration", "ITEM_MOON_STONE"),
    Reward("data/maps/DewfordTown_Hall/scripts.inc", "DewfordTown_Hall_EventScript_DeepSeaScaleMan", "ITEM_DEEP_SEA_SCALE"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle5", "ITEM_METAL_ALLOY"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle5Reward", "ITEM_METAL_ALLOY"),
    Reward("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_EventScript_ReaperClothGiver", "ITEM_REAPER_CLOTH"),
    Reward("data/maps/Route114/scripts.inc", "Route114_EventScript_RoarGentleman", "ITEM_DRAGON_SCALE"),
    Reward("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_EventScript_RazorClawBlackBelt", "ITEM_RAZOR_CLAW"),
    Reward("data/maps/Route123/scripts.inc", "Route123_EventScript_SweetAppleGirl", "ITEM_SWEET_APPLE"),
    Reward("data/maps/LilycoveCity_House2/scripts.inc", "LilycoveCity_House2_EventScript_FatMan", "ITEM_MOON_STONE"),
    Reward("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_EventScript_CompletedNewMauville", "ITEM_ELECTIRIZER"),
    Reward("data/maps/Route104/scripts.inc", "Route104_EventScript_LeafStoneFlorist", "ITEM_LEAF_STONE"),
    Reward("data/maps/FortreeCity_House4/scripts.inc", "FortreeCity_House4_EventScript_WingullReturned", "ITEM_SACHET"),
    Reward("data/maps/LavaridgeTown_HerbShop/scripts.inc", "LavaridgeTown_HerbShop_EventScript_OldMan", "ITEM_FIRE_STONE"),
    Reward("data/maps/PetalburgWoods/scripts.inc", "PetalburgWoods_EventScript_Girl", "ITEM_TART_APPLE"),
    Reward("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_EventScript_Teacher", "ITEM_SUN_STONE"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle3", "ITEM_WHIPPED_DREAM"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle3Reward", "ITEM_WHIPPED_DREAM"),
    Reward("data/maps/Route110_TrickHouseEnd/scripts.inc", "Route110_TrickHouseEnd_EventScript_CompletedPuzzle6", "ITEM_PROTECTOR"),
    Reward("data/maps/Route110_TrickHouseEntrance/scripts.inc", "Route110_TrickHouseEntrance_EventScript_GivePuzzle6Reward", "ITEM_PROTECTOR"),
    Reward("data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc", "ShoalCave_LowTideEntranceRoom_EventScript_ShellBellExpert", "ITEM_LINKING_CORD"),
    Reward("data/maps/Route109/scripts.inc", "Route109_EventScript_WaterStoneGirl", "ITEM_WATER_STONE"),
    Reward("data/maps/ShoalCave_LowTideLowerRoom/scripts.inc", "ShoalCave_LowTideLowerRoom_EventScript_BlackBelt", "ITEM_DEEP_SEA_SCALE"),
    Reward("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_EventScript_Man", "ITEM_REAPER_CLOTH"),
)


PICKUP_STONES = {
    "ITEM_TM_EARTHQUAKE": "ITEM_GARCHOMPITE",
    "ITEM_TM_SANDSTORM": "ITEM_TYRANITARITE",
    "ITEM_TM_SOLAR_BEAM": "ITEM_VENUSAURITE",
    "ITEM_TM_DRAGON_CLAW": "ITEM_SALAMENCITE",
    "ITEM_TM_SUNNY_DAY": "ITEM_CHARIZARDITE_Y",
    "ITEM_TM_ICE_BEAM": "ITEM_GLALITITE",
    "ITEM_TM_HAIL": "ITEM_ABOMASITE",
    "ITEM_TM_SHADOW_BALL": "ITEM_BANETTITE",
    "ITEM_TM_TOXIC": "ITEM_HOUNDOOMINITE",
    "ITEM_TM_FOCUS_PUNCH": "ITEM_MEDICHAMITE",
    "ITEM_TM_SKILL_SWAP": "ITEM_GARDEVOIRITE",
    "ITEM_TM_PSYCHIC": "ITEM_ALAKAZITE",
    "ITEM_TM_IRON_TAIL": "ITEM_AGGRONITE",
    "ITEM_TM_RAIN_DANCE": "ITEM_SWAMPERTITE",
}

FINITE_PICKUPS = {
    ("data/maps/SSTidalLowerDeck/map.json", "ITEM_LEFTOVERS"): "ITEM_ULTRA_BALL",
    ("data/maps/Route116/map.json", "ITEM_BLACK_GLASSES"): "ITEM_DUSK_STONE",
    ("data/maps/Route116/map.json", "ITEM_X_SPECIAL"): "ITEM_THUNDER_STONE",
    ("data/maps/ShoalCave_LowTideIceRoom/map.json", "ITEM_NEVER_MELT_ICE"): "ITEM_ICE_STONE",
    ("data/maps/Seaspray_Cave_B1F/map.json", "ITEM_ABOMASITE"): "ITEM_SLOWBRONITE",
    ("data/maps/DewfordManor_1F/map.json", "ITEM_BANETTITE"): "ITEM_SABLENITE",
    ("data/maps/EmberPath/map.json", "ITEM_CHARIZARDITE_Y"): "ITEM_BLAZIKENITE",
    ("data/maps/SeafloorCavern_Room9/map.json", "ITEM_GARCHOMPITE"): "ITEM_SHARPEDONITE",
    ("data/maps/Route111_RuinsExterior/map.json", "ITEM_MEDICHAMITE"): "ITEM_STEELIXITE",
    ("data/maps/ScorchedSlab_B2F/map.json", "ITEM_TYRANITARITE"): "ITEM_CHARIZARDITE_X",
    ("data/maps/SandstrewnRuins/map.json", "ITEM_OLD_AMBER"): "ITEM_BLACK_AUGURITE",
}


DIALOGUE = {
    ("data/maps/RustboroCity_Gym/scripts.inc", "RustboroCity_Gym_Text_ExplainRoxanneRewards"): ("Aerodactylite lets Aerodactyl Mega\n", "Evolve once you possess a Mega Ring.\\p", "DEVON can restore that OLD AMBER.\n", "Consider Aerodactyl your first Mega\\l", "project.$"),
    ("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_Text_ExplainLucarionite"): ("Lucarionite lets Lucario Mega Evolve.\n", "You'll soon receive the Ring it needs.$"),
    ("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_Text_ExplainManectite"): ("Manectite lets Manectric Mega Evolve.\n", "Let it hold the stone in battle.$"),
    ("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_Text_ExplainCameruptite"): ("Cameruptite lets Camerupt Mega Evolve.\n", "Let it hold the stone in battle.$"),
    ("data/maps/PetalburgCity_Gym/scripts.inc", "PetalburgCity_Gym_Text_ExplainLopunnite"): ("Dad: Lopunnite lets Lopunny Mega Evolve.\n", "Let it hold the stone in battle.$"),
    ("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_Text_ExplainAltarianite"): ("Altarianite lets Altaria Mega Evolve.\n", "Let it hold the stone in battle.$"),
    ("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_Text_ExplainMetagrossite"): ("Tate: Metagrossite lets Metagross Mega\n", "Evolve. Let it hold the stone.$"),
    ("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_Text_ExplainGyaradosite"): ("Gyaradosite lets Gyarados Mega Evolve.\n", "Let it hold the stone in battle.$"),
    ("data/scripts/secret_power_tm.inc", "Route111_Text_ExplainSecretPower"): ("Center tutors can teach Secret Power.\\p", "Take this Ultra Ball for exploring,\n", "and use Secret Power at marked trees.$"),
    ("data/maps/Route114_FossilManiacsHouse/scripts.inc", "Route114_FossilManiacsHouse_Text_HaveThisToDigLikeMyBrother"): ("My brother dug up this Metal Coat.\\p", "It evolves Onix or Scyther when used.$"),
    ("data/maps/SlateportCity_BattleTentLobby/scripts.inc", "SlateportCity_BattleTentLobby_Text_ExplainPrismScale"): ("A Prism Scale evolves Feebas.\n", "Use it from the Bag when ready.$"),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_Text_YourHiddenPowerHasAwoken"): ("Your insight has awoken!\\p", "Take this Dubious Disc for Porygon2.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_PleaseTakeThisDawnStone"): ("This Dawn Stone is my thanks.\n", "Please put it to good use.$"),
    ("data/maps/DewfordTown_Hall/scripts.inc", "DewfordTown_Hall_Text_GiveYouDeepSeaScale"): ("The Deep Sea Scale is trending!\\p", "It evolves Clamperl into Gorebyss.$"),
    ("data/maps/Route114/scripts.inc", "Route114_Text_ExplainRoar"): ("A Dragon Scale evolves Seadra.\n", "Use it from the Bag when ready.$"),
    ("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_Text_ExplainRazorClaw"): ("A Razor Claw evolves Sneasel.\n", "Use it at night when ready.$"),
    ("data/maps/Route104/scripts.inc", "Route104_Text_DontNeedThisTakeIt"): ("I found this Leaf Stone among the\n", "flowers. You should have it!$"),
    ("data/maps/FortreeCity_House4/scripts.inc", "FortreeCity_House4_Text_WelcomeWingullTakeSachet"): ("Welcome back, Wingull! And thank you.\\p", "Please take this Sachet as my thanks.$"),
    ("data/maps/LavaridgeTown_HerbShop/scripts.inc", "LavaridgeTown_HerbShop_Text_ExplainFireStone"): ("A Fire Stone evolves certain Pokémon.\n", "Use it from the Bag when ready.$"),
    ("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_Text_ExplainSunStone"): ("A Sun Stone evolves certain Pokémon.\n", "Use it from the Bag when ready.$"),
    ("data/maps/ShoalCave_LowTideLowerRoom/scripts.inc", "ShoalCave_LowTideLowerRoom_Text_CanOvercomeColdWithFocus"): ("Your focus overcame the cold!\\p", "Take this Deep Sea Scale.$"),
    ("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_Text_ExplainReaperCloth"): ("A Reaper Cloth evolves Dusclops.\n", "Use it from the Bag when ready.$"),
    ("data/scripts/secret_power_tm.inc", "Route111_Text_MakingRoomUseTMToMakeYourOwn"): ("I'm making my own room here with the\n", "POKéMON move SECRET POWER.\\p", "Center tutors can teach it anytime.\n", "Want an ULTRA BALL for the road?$"),
    ("data/maps/Route104/scripts.inc", "Route104_Text_LikeFillingMouthWithSeedsTakeThis"): ("I found this LEAF STONE beneath the\n", "flowers! Some GRASS POKéMON evolve\\l", "when exposed to it. You should have it!$"),
    ("data/maps/Route104/scripts.inc", "Route104_Text_LeafStoneExplanation"): ("That LEAF STONE can evolve certain\n", "GRASS POKéMON. Use it when ready.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_IsThatMeteoriteMayIHaveIt"): ("Oh! Is that the METEORITE TEAM MAGMA\n", "took from METEOR FALLS?\\p", "Please, may I have it? I can offer this\n", "rare DAWN STONE in exchange.$"),
    ("data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_Text_MayIHaveMeteorite"): ("PROF. COZMO: Please, may I have that\n", "METEORITE?\\p", "I can offer this rare DAWN STONE\n", "in exchange.$"),
    ("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_Text_WattsonThanksTakeElectirizer"): ("WATTSON: Wahahahaha! I knew I'd made\n", "the right choice asking you!\\p", "This ELECTIRIZER came from the old\n", "GENERATOR. You've earned it!$"),
    ("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_Text_AdoringPokemonTakeThis"): ("It clearly likes you very much.\\p", "A POKéMON that bright deserves this\n", "SUN STONE, don't you think?$"),
    ("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_Text_ViciousPokemonTakeThis"): ("It has a fierce look to it.\\p", "A POKéMON that intense deserves this\n", "MOON STONE.$"),
    ("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_Text_ExplainReturnFrustration"): ("SUN and MOON STONES evolve different\n", "POKéMON. Use this one when ready.$"),
    ("data/maps/PacifidlogTown_House2/scripts.inc", "PacifidlogTown_House2_Text_GetGoodStoneInXDays"): ("In {STR_VAR_1} days, I'll receive more\n", "evolution stones.\\p", "Come see me then, and I'll choose one\n", "that suits your lead POKéMON.$"),
    ("data/maps/Route114/scripts.inc", "Route114_Text_AllMyMonDoesIsRoarTakeThis"): ("All my POKéMON does is ROAR…\n", "No one dares to come near me…\\p", "I found this DRAGON SCALE nearby.\n", "Please, take it off my hands.$"),
    ("data/maps/SlateportCity_OceanicMuseum_1F/scripts.inc", "SlateportCity_OceanicMuseum_1F_Text_HopeINeverSeeYouAgain"): ("That DEEP SEA TOOTH suits you more\n", "than it does me. It evolves CLAMPERL.\\p", "Hope I never see you again!\n", "Wahahaha!$"),
    ("data/maps/SootopolisCity_House1/scripts.inc", "SootopolisCity_House1_Text_FoundThisRazorClaw"): ("For thirty years I've remained in\n", "SOOTOPOLIS honing my skills.\\p", "This RAZOR CLAW is just as sharp.\n", "I bequeath it to you!$"),
    ("data/maps/VerdanturfTown_BattleTentLobby/scripts.inc", "VerdanturfTown_BattleTentLobby_Text_AttractionRunsDeep"): ("My feelings toward my POKéMON…\n", "The attraction runs deep…\\p", "Oh, hi! A bond like yours deserves\n", "this SHINY STONE.$"),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_Text_ExplainDubiousDisc"): ("A DUBIOUS DISC evolves PORYGON2.\n", "Use it from the Bag when ready.$"),
    ("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_Text_NotSuspiciousTakeThis"): ("Uh… Hi! I'm not acting suspicious!\\p", "I didn't SNATCH this REAPER CLOTH\n", "from anyone. It's clean! Take it!$"),
    ("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_Text_ExplainReaperCloth"): ("A REAPER CLOTH evolves DUSCLOPS.\n", "Use it from the Bag when ready.$"),
    ("data/maps/RustboroCity_PokemonSchool/scripts.inc", "RustboroCity_PokemonSchool_Text_StudentsWhoStudyEveryTool"): ("A good TRAINER studies every tool.\\p", "This SUN STONE evolves certain POKéMON.\n", "Show me that you know when to use it.$"),
    ("data/maps/DewfordTown_House2/scripts.inc", "DewfordTown_House2_Text_WantYouToHaveReaperCloth"): ("Gorge your eyes on this rare\n", "REAPER CLOTH! Spooky, yet stylish!\\p", "You appreciate my dazzling taste.\n", "Here, I want you to have it!$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_BringMe4ShoalSaltAndShells"): ("Bring me four each of SHOAL SALT\n", "and SHOAL SHELLS, and I can weave\\l", "a reusable LINKING CORD.$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_WouldYouLikeShellBell"): ("Oh, hey! You found enough SHOAL SALT\n", "and SHOAL SHELLS!\\p", "Would you like me to weave them into\n", "a LINKING CORD?$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_MakeShellBellRightAway"): ("All right! I'll make that LINKING CORD\n", "right away.\\p", "… … … … … … … …\n", "There! Done!$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_ExplainShellBell"): ("This reusable LINKING CORD lets\n", "certain POKéMON evolve without a trade.$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_WantedToMakeShellBell"): ("Oh… Is that so?\n", "I wanted to weave a LINKING CORD…$"),
    ("data/text/shoal_cave.inc", "ShoalCave_LowTideEntranceRoom_Text_NoSpaceInYourBag"): ("There is no room in your KEY ITEMS\n", "pocket. Make space and come back.$"),
}


def label_block(text: str, label: str) -> re.Match[str]:
    match = re.search(rf"(^\s*{re.escape(label)}(?:::|:).*?)(?=^\s*[A-Za-z0-9_]+(?:::|:)|\Z)", text, re.M | re.S)
    if match is None:
        raise ValueError(f"missing label {label}")
    return match


def rewrite_reward(text: str, reward: Reward) -> str:
    match = label_block(text, reward.label)
    block = match.group(1)
    changed, count = re.subn(r"giveitem\s+ITEM_[A-Z0-9_]+", f"giveitem {reward.item}", block, count=1)
    if count == 0:
        if f"giveitem {reward.item}" in block:
            return text
        raise ValueError(f"{reward.path}:{reward.label} has no TM reward")
    return text[:match.start(1)] + changed + text[match.end(1):]


def rewrite_dialogue(text: str, label: str, lines: tuple[str, ...]) -> str:
    match = label_block(text, label)
    encoded = (line.replace("\n", "\\n") for line in lines)
    block = label + ":\n" + "".join(f'\t.string "{line}"\n' for line in encoded)
    return text[:match.start(1)] + block + text[match.end(1):]


def write() -> None:
    by_path: dict[str, list[Reward]] = {}
    for reward in REWARDS:
        by_path.setdefault(reward.path, []).append(reward)
    for path, rewards in by_path.items():
        target = ROOT / path
        text = target.read_text()
        for reward in rewards:
            text = rewrite_reward(text, reward)
        target.write_text(text)

    for target in (ROOT / "data" / "maps").glob("*/map.json"):
        payload = json.loads(target.read_text())
        changed = False
        for section in ("object_events", "bg_events"):
            for event in payload.get(section, []):
                for field in ("trainer_sight_or_berry_tree_id", "item"):
                    old = event.get(field)
                    if isinstance(old, str) and old.startswith("ITEM_TM_"):
                        event[field] = PICKUP_STONES.get(old, "ITEM_ULTRA_BALL")
                        changed = True
        if changed:
            target.write_text(json.dumps(payload, indent=2) + "\n")

    for (path, old_item), new_item in FINITE_PICKUPS.items():
        target = ROOT / path
        payload = json.loads(target.read_text())
        matches = []
        for section in ("object_events", "bg_events"):
            for event in payload.get(section, []):
                for field in ("trainer_sight_or_berry_tree_id", "item"):
                    if event.get(field) in (old_item, new_item):
                        matches.append((event, field))
        if len(matches) != 1:
            raise ValueError(f"{path}: expected one {old_item}/{new_item} pickup, found {len(matches)}")
        matches[0][0][matches[0][1]] = new_item
        target.write_text(json.dumps(payload, indent=2) + "\n")

    by_path_dialogue: dict[str, list[tuple[str, tuple[str, ...]]]] = {}
    for (path, label), lines in DIALOGUE.items():
        by_path_dialogue.setdefault(path, []).append((label, lines))
    for path, entries in by_path_dialogue.items():
        target = ROOT / path
        text = target.read_text()
        for label, lines in entries:
            text = rewrite_dialogue(text, label, lines)
        target.write_text(text)


def check() -> None:
    for reward in REWARDS:
        block = label_block((ROOT / reward.path).read_text(), reward.label).group(1)
        if f"giveitem {reward.item}" not in block or re.search(r"giveitem\s+ITEM_TM_", block):
            raise ValueError(f"stale reward {reward.path}:{reward.label}")
    for path in (ROOT / "data" / "maps").glob("*/map.json"):
        payload = json.loads(path.read_text())
        for section in ("object_events", "bg_events"):
            for event in payload.get(section, []):
                values = (event.get("trainer_sight_or_berry_tree_id"), event.get("item"))
                if any(isinstance(value, str) and value.startswith("ITEM_TM_") for value in values):
                    raise ValueError(f"TM pickup remains in {path}")
    for (path, old_item), new_item in FINITE_PICKUPS.items():
        text = (ROOT / path).read_text()
        if old_item in text or new_item not in text:
            raise ValueError(f"stale free-item pickup {path}:{old_item}")
    for (path, label), lines in DIALOGUE.items():
        block = label_block((ROOT / path).read_text(), label).group(1)
        for line in lines:
            visible = line.replace("\n", "\\n")
            if visible not in block:
                raise ValueError(f"stale reward dialogue {path}:{label}")

    shoal = (ROOT / "data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc").read_text()
    if "checkitem ITEM_LINKING_CORD" not in shoal or "checkitemspace ITEM_LINKING_CORD" not in shoal:
        raise ValueError("Linking Cord craft lacks one-time ownership and Key Item space checks")
    if "checkitemspace ITEM_SHELL_BELL" in shoal:
        raise ValueError("Linking Cord craft still checks the obsolete Shell Bell pocket")
    print(f"PASS: {len(REWARDS)} scripted rewards and all map TM pickups are finite progression rewards")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    if args.write:
        write()
    check()


if __name__ == "__main__":
    main()
