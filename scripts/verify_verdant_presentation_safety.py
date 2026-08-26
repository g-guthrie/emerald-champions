#!/usr/bin/env python3
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def between(source: str, start: str, end: str) -> str:
    _, found_start, tail = source.partition(start)
    if not found_start:
        return ""
    body, found_end, _ = tail.partition(end)
    return body if found_end else ""


storage = read("src/pokemon_storage_system.c")
pokedex = read("src/pokedex.c")
palette = read("include/palette_util.h")
field_tasks = read("src/field_tasks.c")
move_text = read("src/data/text/move_descriptions.h")
safari_text = read("data/scripts/safari_zone.inc")
base_stats = read("src/data/pokemon/base_stats.h")
route_111 = read("data/maps/Route111/scripts.inc")
battle_moves = read("src/data/battle_moves.h")
evolutions = read("src/data/pokemon/evolution.h")
evolution_constants = read("include/constants/pokemon.h")
battle_configuration = read("include/constants/battle_config.h")
item_source = read("src/data/items.h")
battle_main_source = read("src/battle_main.c")
battle_util_source = read("src/battle_util.c")
battle_command_source = read("src/battle_script_commands.c")
battle_struct_source = read("include/battle.h")
red_orb_record = between(item_source, "[ITEM_RED_ORB]", "[ITEM_BLUE_ORB]")
blue_orb_record = between(item_source, "[ITEM_BLUE_ORB]", "[ITEM_SCANNER]")
get_mega_species = between(
    battle_util_source, "u16 GetMegaEvolutionSpecies", "u16 GetPrimalReversionSpecies"
)
can_mega_evolve = between(
    battle_util_source, "bool32 CanMegaEvolve", "bool32 CanPrimalRevert"
)
can_primal_revert = between(
    battle_util_source, "bool32 CanPrimalRevert", "void UndoMegaEvolution"
)
controller_selection = "\n".join(
    read(path)
    for path in (
        "include/battle_controllers.h",
        "src/battle_controller_opponent.c",
        "src/battle_controller_player.c",
        "src/battle_controller_player_partner.c",
    )
)

checks = {
    "PC item icon staging buffer covers the complete sprite sheet": (
        "sItemIconGfxBuffer[0x200 / sizeof(u32)]" in storage
        and "spriteSheet.size = 0x200;" in storage
    ),
    "Pokédex display personalities cannot select shiny palettes": (
        "pokedexOtId = personality ^ SHINY_ODDS;" in pokedex
        and "CreateMonPicSprite(nationalNum, pokedexOtId, personality" in pokedex
    ),
    "pulse blend coefficient preserves the full unsigned hardware range": (
        "u8 maxBlendCoeff:4;" in palette
        and "s8 maxBlendCoeff:4;" not in palette
    ),
    "Sootopolis ice history uses a bounded direct bit test": (
        "VarGet(sSootopolisGymIceRowVars[y]) & (1 << (x - 3))" in field_tasks
        and "0x10000 << (x - 3)" not in field_tasks
    ),
    "Mystical Fire describes its guaranteed stat drop": (
        '"fire. Lowers Sp. Atk."' in move_text
        and '"fire. May lower Sp. Atk."' not in move_text
    ),
    "Mega is the sole player-selected gimmick and automatic Primal Reversion remains allowed": (
        "2x damage to Dynamaxed foes" not in move_text
        and all(
            f"[MOVE_{move}]" in battle_moves
            and "EFFECT_HIT" in battle_moves.split(f"[MOVE_{move}]", 1)[1].split("},", 1)[0]
            for move in ("DYNAMAX_CANNON", "BEHEMOTH_BLADE", "BEHEMOTH_BASH")
        )
        and "RET_MEGA_EVOLUTION" in controller_selection
        and all(
            token not in controller_selection
            for token in ("RET_PRIMAL", "RET_DYNAMAX", "RET_TERA", "RET_Z_MOVE", "RET_ULTRA_BURST")
        )
        and "EVO_PRIMAL_REVERSION" in evolution_constants
        and "EVO_PRIMAL_REVERSION" in evolutions
        and all(
            ".holdEffect = HOLD_EFFECT_PRIMAL_ORB," in record
            and ".importance = 0," in record
            and ".pocket = POCKET_MEGA_STONES," in record
            for record in (red_orb_record, blue_orb_record)
        )
        and "u8 battlerId = gBattlerByTurnOrder[i];" in battle_main_source
        and "if (CanPrimalRevert(battlerId))" in battle_main_source
        and "GetPrimalReversionSpecies(gBattleMons[battlerId].species, gBattleMons[battlerId].item)" in can_primal_revert
        and all(
            token not in can_primal_revert
            for token in (
                "ITEM_MEGA_BRACELET",
                "FLAG_SYS_RECEIVED_KEYSTONE",
                "alreadyEvolved",
                "toEvolve",
                "GetBattlerHoldEffect",
            )
        )
        and "EVO_PRIMAL_REVERSION" not in get_mega_species
        and "GetPrimalReversionSpecies" not in can_mega_evolve
        and "HOLD_EFFECT_PRIMAL_ORB" not in can_mega_evolve
        and "bufferA[battlerId][4]" in can_mega_evolve
        and "bufferA[gActiveBattler][4]" not in can_mega_evolve
        and "isPrimalReversion" not in battle_struct_source + battle_util_source
        and "if (CanPrimalRevert(gActiveBattler))" in battle_command_source
        and "gBattlescriptCurrInstr = BattleScript_PrimalReversionRet;" in battle_command_source
        and "if (!CanPrimalRevert(gActiveBattler))" in battle_command_source
        and re.search(r"^\s*#define\s+B_DYNAMAX\b", battle_configuration, re.M) is None
        and re.search(r"^\s*#define\s+B_TERA\b", battle_configuration, re.M) is None
    ),
    "Safari Zone dialogue uses native species capitalization": (
        "see Pikachu here" in safari_text
        and "see PIKACHU here" not in safari_text
    ),
    "player-facing descriptions use release rather than loose": (
        "Looses" not in move_text
        and "Looses" not in read("src/data/text/item_descriptions.h")
        and "loosing power" not in read("data/text/battle_dome.inc")
    ),
    "Granbull has an explicit second type in the standard data branch": (
        ".type1 = TYPE_FAIRY," in base_stats.split("[SPECIES_GRANBULL]", 1)[1].split("[SPECIES_QWILFISH]", 1)[0]
        and ".type2 = TYPE_FAIRY," in base_stats.split("[SPECIES_GRANBULL]", 1)[1].split("[SPECIES_QWILFISH]", 1)[0]
    ),
    "Route 111 vial nurse disappears without walking through scenery": (
        "Route111_EventScript_UpgradeVialHideNurse::" in route_111
        and "removeobject LOCALID_NURSE" in route_111.split("Route111_EventScript_UpgradeVialHideNurse::", 1)[1].split("end", 1)[0]
        and "applymovement LOCALID_NURSE" not in route_111.split("Route111_EventScript_UpgradeVialHideNurse::", 1)[1].split("end", 1)[0]
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
if failed:
    raise SystemExit(f"{len(failed)} presentation safety check(s) failed")
print(f"All {len(checks)} presentation safety checks passed")
