#!/usr/bin/env python3
"""Verify that Primal Reversion remains complete and automatic."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def initializer(source: str, symbol: str) -> str:
    match = re.search(
        rf"^\s*\[{re.escape(symbol)}\]\s*=\s*\{{(.*?)^\s*\}},",
        source,
        re.MULTILINE | re.DOTALL,
    )
    return match.group(1) if match else ""


def between(source: str, start: str, end: str) -> str:
    _, found_start, tail = source.partition(start)
    if not found_start:
        return ""
    body, found_end, _ = tail.partition(end)
    return body if found_end else ""


items = read("src/data/items.h")
item_constants = read("include/constants/items.h")
item_icons = read("src/data/item_icon_table.h")
item_graphics = read("src/data/graphics/items.h")
hold_effects = read("include/constants/hold_effects.h")
evolutions = read("src/data/pokemon/evolution.h")
evolution_constants = read("include/constants/pokemon.h")
species_constants = read("include/constants/species.h")
base_stats = read("src/data/pokemon/base_stats.h")
form_tables = read("src/data/pokemon/form_species_tables.h")
form_pointers = read("src/data/pokemon/form_species_table_pointers.h")
battle_main = read("src/battle_main.c")
battle_util = read("src/battle_util.c")
battle_commands = read("src/battle_script_commands.c")
battle_scripts = read("data/battle_scripts_1.s")
animation_scripts = read("data/battle_anim_scripts.s")
animation_constants = read("include/constants/battle_anim.h")
animation_graphics = read("src/graphics.c")
battle_animation = read("src/battle_anim.c")
battle_interface = read("src/battle_interface.c")
battle_structs = read("include/battle.h")
pokemon_structs = read("include/pokemon.h")
item_menu = read("src/item_menu.c")
cry_tables = read("sound/cry_tables.inc")
cry_data = read("sound/direct_sound_data.inc")

orb_blocks = {
    "ITEM_RED_ORB": initializer(items, "ITEM_RED_ORB"),
    "ITEM_BLUE_ORB": initializer(items, "ITEM_BLUE_ORB"),
}
get_mega_species = between(
    battle_util, "u16 GetMegaEvolutionSpecies", "u16 GetPrimalReversionSpecies"
)
get_primal_species = between(
    battle_util, "u16 GetPrimalReversionSpecies", "u16 GetWishMegaEvolutionSpecies"
)
can_mega_evolve = between(
    battle_util, "bool32 CanMegaEvolve", "bool32 CanPrimalRevert"
)
can_primal_revert = between(
    battle_util, "bool32 CanPrimalRevert", "void UndoMegaEvolution"
)
switch_in_effects = between(
    battle_commands,
    "static void Cmd_switchineffects(void)\n{",
    "static void Cmd_trainerslidein(void)\n{",
)

checks = {
    "Orb item IDs retain their save-compatible slots": (
        "#define ITEM_RED_ORB    (LAST_KEY_ITEM_INDEX + 49)" in item_constants
        and "#define ITEM_BLUE_ORB    (LAST_KEY_ITEM_INDEX + 50)" in item_constants
        and "#define ITEMS_COUNT   (ITEM_SAPPHIRE + 1)" in item_constants
    ),
    "both Orbs are native giveable Primal held items": all(
        f".itemId = {item}," in block
        and ".holdEffect = HOLD_EFFECT_PRIMAL_ORB," in block
        and ".importance = 0," in block
        and ".pocket = POCKET_MEGA_STONES," in block
        for item, block in orb_blocks.items()
    ),
    "Mega Stone pocket exposes the native Give action": (
        "sContextMenuItems_MegaStonesPocket[]" in item_menu
        and "ACTION_GIVE,        ACTION_CANCEL" in item_menu
        and "case MEGA_STONES_POCKET:" in item_menu
    ),
    "Orb hold effect and species transformations are registered": (
        "#define HOLD_EFFECT_PRIMAL_ORB" in hold_effects
        and "[SPECIES_KYOGRE]     = {{EVO_PRIMAL_REVERSION, ITEM_BLUE_ORB, SPECIES_KYOGRE_PRIMAL}}" in evolutions
        and "[SPECIES_GROUDON]    = {{EVO_PRIMAL_REVERSION, ITEM_RED_ORB, SPECIES_GROUDON_PRIMAL}}" in evolutions
        and "#define EVO_PRIMAL_REVERSION" in evolution_constants
    ),
    "Primal eligibility is direct and independent of Mega state": (
        "GetPrimalReversionSpecies(gBattleMons[battlerId].species, gBattleMons[battlerId].item)" in can_primal_revert
        and "ItemId_GetHoldEffect(gBattleMons[battlerId].item) == HOLD_EFFECT_PRIMAL_ORB" in can_primal_revert
        and "ITEM_MEGA_BRACELET" not in can_primal_revert
        and "FLAG_SYS_RECEIVED_KEYSTONE" not in can_primal_revert
        and "alreadyEvolved" not in can_primal_revert
        and "toEvolve" not in can_primal_revert
        and "GetBattlerHoldEffect" not in can_primal_revert
        and "EVO_PRIMAL_REVERSION" in get_primal_species
    ),
    "Mega selection excludes Primal forms and uses its battler parameter": (
        "EVO_MEGA_EVOLUTION" in get_mega_species
        and "EVO_PRIMAL_REVERSION" not in get_mega_species
        and "GetPrimalReversionSpecies" not in can_mega_evolve
        and "HOLD_EFFECT_PRIMAL_ORB" not in can_mega_evolve
        and "bufferA[battlerId][4]" in can_mega_evolve
        and "bufferA[gActiveBattler][4]" not in can_mega_evolve
        and "isPrimalReversion" not in battle_structs + battle_util
    ),
    "battle-start leads automatically revert in speed order": (
        "// Primal Reversion" in battle_main
        and "u8 battlerId = gBattlerByTurnOrder[i];" in battle_main
        and "if (CanPrimalRevert(battlerId))" in battle_main
        and "BattleScriptExecute(BattleScript_PrimalReversion);" in battle_main
    ),
    "every mid-battle switch path uses the shared Primal entry hook": (
        "if (CanPrimalRevert(gActiveBattler))" in switch_in_effects
        and "gBattlescriptCurrInstr = BattleScript_PrimalReversionRet;" in switch_in_effects
        and switch_in_effects.index("CanPrimalRevert")
            < switch_in_effects.index("SIDE_STATUS_SPIKES_DAMAGED")
        and "jumpifcantreverttoprimal BattleScript_DoSwitchOut2" not in battle_scripts
    ),
    "Primal handler transforms, recalculates, records, and redraws": all(
        token in battle_commands
        for token in (
            "case VARIOUS_HANDLE_PRIMAL_REVERSION:",
            "GetPrimalReversionSpecies(gBattleStruct->mega.primalRevertedSpecies[gActiveBattler]",
            "BtlController_EmitSetMonData(0, REQUEST_SPECIES_BATTLE",
            "RecalcBattlerStats(gActiveBattler, mon);",
            "primalRevertedPartyIds[GetBattlerSide(gActiveBattler)] |=",
            "CreateMegaIndicatorSprite(gActiveBattler, 0);",
        )
    ),
    "Primal scripts animate, announce, and start weather abilities": (
        "BattleScript_PrimalReversion::" in battle_scripts
        and "BattleScript_PrimalReversionRet::" in battle_scripts
        and battle_scripts.count("playanimation BS_ATTACKER, B_ANIM_PRIMAL_REVERSION") == 2
        and battle_scripts.count("printstring STRINGID_PKMNREVERTEDTOPRIMAL") == 2
        and battle_scripts.count("switchinabilities BS_ATTACKER") >= 1
    ),
    "battle UI preserves Alpha and Omega indicators": (
        "primalRevertedPartyIds" in battle_interface
        and "sSpritePalette_OmegaIndicator" in battle_interface
        and "sSpritePalette_AlphaIndicator" in battle_interface
    ),
    "Primal species, abilities, and form families remain registered": (
        "#define SPECIES_KYOGRE_PRIMAL" in species_constants
        and "#define SPECIES_GROUDON_PRIMAL" in species_constants
        and "[SPECIES_KYOGRE_PRIMAL]" in base_stats
        and "ABILITY_PRIMORDIAL_SEA" in initializer(base_stats, "SPECIES_KYOGRE_PRIMAL")
        and "[SPECIES_GROUDON_PRIMAL]" in base_stats
        and "ABILITY_DESOLATE_LAND" in initializer(base_stats, "SPECIES_GROUDON_PRIMAL")
        and "SPECIES_KYOGRE_PRIMAL" in form_tables
        and "SPECIES_GROUDON_PRIMAL" in form_tables
        and "[SPECIES_KYOGRE_PRIMAL] = sKyogreFormSpeciesIdTable" in form_pointers
        and "[SPECIES_GROUDON_PRIMAL] = sGroudonFormSpeciesIdTable" in form_pointers
    ),
    "Orb icons and source art remain complete": (
        "[ITEM_RED_ORB] = {gItemIcon_Orb, gItemIconPalette_RedOrb}" in item_icons
        and "[ITEM_BLUE_ORB] = {gItemIcon_Orb, gItemIconPalette_BlueOrb}" in item_icons
        and "gItemIconPalette_RedOrb" in item_graphics
        and "gItemIconPalette_BlueOrb" in item_graphics
        and all(
            (ROOT / path).is_file()
            for path in (
                "graphics/items/icons/orb.png",
                "graphics/items/icon_palettes/red_orb.pal",
                "graphics/items/icon_palettes/blue_orb.pal",
            )
        )
    ),
    "Primal front, back, icon, normal, and shiny art remains complete": all(
        (ROOT / f"graphics/pokemon/primal_{species}/{asset}").is_file()
        for species in ("kyogre", "groudon")
        for asset in ("front.png", "back.png", "icon.png", "normal.pal", "shiny.pal")
    ),
    "Primal transformation animation assets and registrations remain complete": (
        "#define B_ANIM_PRIMAL_REVERSION" in animation_constants
        and "General_PrimalReversion::" in animation_scripts
        and "gBattleAnimSpriteGfx_PrimalParticles" in animation_graphics
        and "gBattleAnimSpritePal_PrimalParticles" in animation_graphics
        and "ANIM_TAG_PRIMAL_PARTICLES" in battle_animation
        and all(
            (ROOT / path).is_file()
            for path in (
                "graphics/battle_anims/sprites/primal_particles.png",
                "graphics/battle_anims/sprites/alpha_symbol.png",
                "graphics/battle_anims/sprites/omega_symbol.png",
                "graphics/battle_anims/sprites/new/alpha_stone.png",
                "graphics/battle_anims/sprites/new/omega_stone.png",
            )
        )
    ),
    "Primal cries remain registered with source samples": (
        "cry_uncomp Cry_KyogrePrimal" in cry_tables
        and "cry_uncomp Cry_GroudonPrimal" in cry_tables
        and "Cry_KyogrePrimal::" in cry_data
        and "Cry_GroudonPrimal::" in cry_data
        and (ROOT / "sound/direct_sound_samples/cries/uncomp_primal_kyogre.aif").is_file()
        and (ROOT / "sound/direct_sound_samples/cries/uncomp_primal_groudon.aif").is_file()
    ),
    "native save records retain 16-bit species and held-item IDs": (
        "u16 species;" in pokemon_structs
        and "u16 heldItem;" in pokemon_structs
        and "UndoMegaEvolution(gBattlerPartyIndexes[gActiveBattler]);" in battle_main
        and "UndoMegaEvolution(i);" in battle_main
        and "else if (gBattleStruct->mega.primalRevertedPartyIds" in battle_util
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
if failed:
    raise SystemExit(f"{len(failed)} Primal Reversion regression check(s) failed")
print(f"All {len(checks)} Primal Reversion checks passed")
