#!/usr/bin/env python3
"""Static release gates for Legendary Signs and the Showdown Circuit."""

from __future__ import annotations

import hashlib
import json
import re
import struct
from pathlib import Path

from verify_trainer_ability_legality import (
    configured_species_abilities,
    resolve_species,
    species_aliases,
)


ROOT = Path(__file__).resolve().parents[1]
EXACT_INCLEMENT_PHYSICAL = {
    "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO": ("ARTICUNO", "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM"),
    "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS": ("ZAPDOS", "MAP_NEW_MAUVILLE_INSIDE"),
    "OBJ_EVENT_GFX_INCLEMENT_MOLTRES": ("MOLTRES", "MAP_EMBER_PATH"),
    "OBJ_EVENT_GFX_INCLEMENT_MEWTWO": ("MEWTWO", "MAP_ALTERING_CAVE_B1F"),
    "OBJ_EVENT_GFX_INCLEMENT_JIRACHI": ("JIRACHI", "MAP_METEOR_FALLS_JIRACHIS_ROOM"),
    "OBJ_EVENT_GFX_INCLEMENT_HEATRAN": ("HEATRAN", "MAP_SCORCHED_SLAB_HEATRANS_ROOM"),
    "OBJ_EVENT_GFX_INCLEMENT_DIANCIE": ("DIANCIE", "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM"),
    "OBJ_EVENT_GFX_REGIGIGAS_STATUE": ("REGIGIGAS", "MAP_SEALED_CHAMBER_INNER_ROOM"),
}
SCRIPTED_VISIBLE_ROOTS = {
    "MAGEARNA": (
        "data/maps/RustboroCity_DevonCorp_2F/scripts.inc",
        ("EC_SIGN_MAGEARNA_ID", "TryGiveSelectedLegendarySignReward", "FLAG_EC_CAUGHT_MAGEARNA"),
    ),
    "PECHARUNT": (
        "data/maps/MtPyre_6F/scripts.inc",
        ("MtPyre_6F_EventScript_Pecharunt", "CreateSelectedLegendarySignEncounter", "FLAG_EC_CAUGHT_PECHARUNT"),
    ),
}


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def verify_overworld_sprite(path: Path, context: str) -> tuple[int, int]:
    """Prove a species object uses a complete, native-sized indexed sheet."""

    require(path.is_file() and path.stat().st_size > 0, f"{context} has no nonempty overworld sprite")
    data = path.read_bytes()
    require(data[:8] == b"\x89PNG\r\n\x1a\n" and data[12:16] == b"IHDR", f"{context} sprite is not a valid PNG")
    width, height, _bit_depth, color_type = struct.unpack(">IIBB", data[16:26])
    require(height in (32, 64), f"{context} sprite height is not a native 32px or 64px frame")
    require(width % height == 0 and width // height in (6, 8), f"{context} sprite sheet has an incomplete frame layout")
    require(color_type == 3, f"{context} sprite is not indexed-color artwork")
    return width, height


def main() -> None:
    manifest = json.loads(read("docs/showdown_champions_random_doubles.json"))
    generated = read("src/data/pokemon/showdown_champions_circuit.h")
    circuit = read("src/champions_circuit.c")
    circuit_lobby = read("data/maps/BattleFrontier_BattleTowerLobby/scripts.inc")
    circuit_corridor = read("data/maps/BattleFrontier_BattleTowerCorridor/scripts.inc")
    circuit_room = read("data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc")
    flags = read("include/constants/flags.h")
    migration = read("src/overworld.c")
    definitions = read("src/data/pokemon/legendary_signs.h")

    # The Frontier's architecture and amenities remain explorable, but every
    # live challenge desk must enter the Showdown-derived Doubles Circuit.
    # Native Gen 3 facility generators are retained only as inert engine code.
    frontier_lobbies = (
        "BattleFrontier_BattleArenaLobby",
        "BattleFrontier_BattleDomeLobby",
        "BattleFrontier_BattleFactoryLobby",
        "BattleFrontier_BattlePalaceLobby",
        "BattleFrontier_BattlePikeLobby",
        "BattleFrontier_BattlePyramidLobby",
        "BattleFrontier_BattleTowerLobby",
    )
    challenge_desks = []
    for map_name in frontier_lobbies:
        payload = json.loads(read(f"data/maps/{map_name}/map.json"))
        for obj in payload.get("object_events", []):
            if "ATTENDANT" not in obj.get("local_id", ""):
                continue
            challenge_desks.append((map_name, obj["local_id"], obj.get("script")))
    require(len(challenge_desks) == 13, f"Frontier challenge-desk inventory drifted: {challenge_desks}")
    require(
        all(script == "BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit"
            for _, _, script in challenge_desks),
        "a live Frontier desk still launches a native Gen 3 facility: "
        + repr([row for row in challenge_desks if row[2] != "BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit"]),
    )

    require(manifest["source_commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7", "Showdown source commit drifted")
    require(manifest["variant_count"] == 311, "Showdown variant count drifted")
    require(manifest["template_count"] == 444, "Showdown template count drifted")
    require(len(manifest["policy"]["ability_overrides"]) == 9, "Circuit Ability adaptation policy drifted")
    require(generated.count(".partySpecies =") == 311, "generated Showdown variant table is incomplete")
    require(generated.count(".role =") == 444, "generated Showdown template table is incomplete")
    require("Pokemon Showdown" in read("docs/THIRD_PARTY_NOTICES.md"), "Showdown MIT notice is missing")
    require("gShowdownCircuitVariants" in circuit, "Circuit is not using Showdown's species pool")
    require("gShowdownCircuitTemplates" in circuit, "Circuit is not using Showdown's role templates")
    require("ChooseBaseDex" in circuit and "CandidateAllowed" in circuit, "live Showdown team composition is missing")
    require("towerNumWins" not in circuit and "towerSinglesStreak" not in circuit, "Circuit contaminates Battle Tower records")
    require("VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS" in circuit, "Circuit lacks dedicated current-run state")
    require(
        "u16 wins = VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS);" in circuit,
        "Circuit reward entitlement no longer survives retirement",
    )
    require(
        circuit_lobby.count("special ChampionsCircuitTryGiveReward") == 1
        and circuit_room.count("special ChampionsCircuitTryGiveReward") == 1
        and "BattleFrontier_BattleTowerLobby_EventScript_CircuitPendingRewardNoRoom" in circuit_lobby
        and "Make room, then speak to this desk again." in circuit_lobby,
        "pending Circuit rewards are not truthfully retryable from the desk",
    )
    require(
        "#define FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED                         0x4D9" in flags
        and "FLAG_UNUSED_0x4D9" not in flags
        and "{FLAG_EC_STARTER_ARCHIVE_BULBASAUR, FLAG_RECEIVED_GAME_CORNER_POIPOLE}" in migration
        and circuit_lobby.index("goto_if_unset FLAG_SYS_GAME_CLEAR")
        < circuit_lobby.index("goto_if_set FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED")
        < circuit_lobby.index("msgbox BattleFrontier_BattleTowerLobby_Text_CircuitWelcome")
        < circuit_lobby.index("setflag FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED")
        and "BattleFrontier_BattleTowerLobby_Text_CircuitWelcomeBack" in circuit_lobby,
        "Circuit first-talk explanation is not a persistent, postgame-only one-time presentation",
    )
    require(
        "special ChampionsCircuitBegin" in circuit_lobby
        and "warp MAP_BATTLE_FRONTIER_BATTLE_TOWER_CORRIDOR, 8, 1" in circuit_lobby
        and "special ChampionsCircuitGenerateOpponent" not in circuit_lobby
        and "special BattleSetup_StartChampionsCircuitBattle" not in circuit_lobby,
        "Circuit still starts battles at the lobby desk instead of entering the native Tower rooms",
    )
    corridor_branch = circuit_corridor.split(
        "BattleFrontier_BattleTowerCorridor_EventScript_EnterCircuitCorridor::", 1
    )[1].split("BattleFrontier_BattleTowerCorridor_EventScript_WalkToFarDoor::", 1)[0]
    require(
        "goto_if_eq VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE, BattleFrontier_BattleTowerCorridor_EventScript_EnterCircuitCorridor"
        in circuit_corridor
        and "BattleFrontier_BattleTowerCorridor_Movement_AttendantWalkToDoor" in corridor_branch
        and "BattleFrontier_BattleTowerCorridor_Movement_PlayerWalkToDoor" in corridor_branch
        and "warp MAP_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM, 5, 8" in corridor_branch
        and "tower_" not in corridor_branch
        and "frontier_" not in corridor_branch,
        "Circuit corridor staging is not isolated from native Tower challenge state",
    )
    circuit_room_branch = circuit_room.split(
        "BattleFrontier_BattleTowerBattleRoom_EventScript_EnterCircuitRoom::", 1
    )[1].split("BattleFrontier_BattleTowerBattleRoom_EventScript_OpponentEnter::", 1)[0]
    require(
        "goto_if_eq VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE, BattleFrontier_BattleTowerBattleRoom_EventScript_EnterCircuitRoom"
        in circuit_room
        and all(
            token in circuit_room_branch
            for token in (
                "BattleFrontier_BattleTowerBattleRoom_Movement_PlayerEnter",
                "BattleFrontier_BattleTowerBattleRoom_Movement_AttendantApproachPlayer",
                "special ChampionsCircuitGenerateOpponent",
                "special BattleSetup_StartChampionsCircuitBattle",
                "special ChampionsCircuitHandleBattleResult",
                "special ChampionsCircuitTryGiveReward",
                "special ChampionsCircuitEnd",
                "warp MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 23, 6",
            )
        )
        and circuit_room_branch.count("special ChampionsCircuitEnd") == 2
        and circuit_room_branch.index("release\n\twarp MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 23, 6")
        > circuit_room_branch.index("BattleFrontier_BattleTowerBattleRoom_EventScript_CircuitReturnToLobby::")
        and "tower_" not in circuit_room_branch
        and "frontier_" not in circuit_room_branch,
        "Circuit battle-room staging does not preserve isolated result/reward/restoration exits",
    )
    require("CIRCUIT_MASTERY_WINS 40" in circuit, "Circuit mastery milestone drifted")
    require("Random() %" not in circuit, "Circuit still uses biased modulo sampling")
    require(
        "RandomUniform(RNG_NONE, 0, FRONTIER_TRAINERS_COUNT - 1)"
        in read("src/battle_setup.c"),
        "Circuit trainer presentation is not sampled uniformly",
    )
    legal_abilities = configured_species_abilities()
    aliases = species_aliases()
    illegal_circuit_abilities = []
    for variant in manifest["variants"]:
        configured_species = resolve_species(variant["party_species"], aliases)
        legal = legal_abilities.get(configured_species, frozenset())
        start = variant["template_offset"]
        stop = start + variant["template_count"]
        for template_index, template in enumerate(manifest["templates"][start:stop], start):
            for ability in template["abilities"]:
                if ability not in legal:
                    illegal_circuit_abilities.append(
                        f"{variant['showdown_id']} template {template_index}: {ability}"
                    )
    require(
        not illegal_circuit_abilities,
        "Circuit templates request configured-out Abilities:\n" + "\n".join(illegal_circuit_abilities),
    )

    legendary_runtime = read("src/legendary_signs.c")
    relic_contracts = {
        "SPECIES_GROUDON": {"ITEM_RED_ORB"},
        "SPECIES_KYOGRE": {"ITEM_BLUE_ORB"},
        "SPECIES_ZACIAN": {"ITEM_RUSTED_SWORD"},
        "SPECIES_ZAMAZENTA": {"ITEM_RUSTED_SHIELD"},
        "SPECIES_OGERPON_TEAL": {
            "ITEM_WELLSPRING_MASK", "ITEM_HEARTHFLAME_MASK", "ITEM_CORNERSTONE_MASK",
        },
        "SPECIES_ARCEUS": {
            "ITEM_FLAME_PLATE", "ITEM_SPLASH_PLATE", "ITEM_ZAP_PLATE",
            "ITEM_MEADOW_PLATE", "ITEM_ICICLE_PLATE", "ITEM_FIST_PLATE",
            "ITEM_TOXIC_PLATE", "ITEM_EARTH_PLATE", "ITEM_SKY_PLATE",
            "ITEM_MIND_PLATE", "ITEM_INSECT_PLATE", "ITEM_STONE_PLATE",
            "ITEM_SPOOKY_PLATE", "ITEM_DRACO_PLATE", "ITEM_DREAD_PLATE",
            "ITEM_IRON_PLATE", "ITEM_PIXIE_PLATE",
        },
    }
    for species, items in relic_contracts.items():
        case = legendary_runtime.split(f"case {species}:", 1)
        require(len(case) == 2, f"{species} has no associated relic grant")
        body = case[1].split("break;", 1)[0]
        if species == "SPECIES_ARCEUS":
            body += legendary_runtime.split("sArceusPlates[]", 1)[1].split("};", 1)[0]
        require(items <= set(re.findall(r"ITEM_[A-Z0-9_]+", body)),
                f"{species} is missing relics: {sorted(items - set(re.findall(r'ITEM_[A-Z0-9_]+', body)))}")
    require(
        "CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1)" in legendary_runtime
        and "AddBagItem(item, 1)" in legendary_runtime
        and "AddPCItem(item, 1)" in legendary_runtime,
        "legendary relic delivery is not idempotent and Bag/PC safe",
    )

    sign_ids = re.findall(r"(?:WILD|VISIBLE|OTHER)_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+)", definitions)
    require(len(sign_ids) == 82 and len(set(sign_ids)) == 82, "Legendary Sign definitions are incomplete or duplicated")
    require("MIRAGE_TOWER" not in definitions, "a Sign still depends on collapsible Mirage Tower")
    require("SAFARI_ZONE" not in definitions, "a Sign still requires Safari capture rules")
    conditional_ids = set(re.findall(r"(?m)^WILD_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+)", definitions))
    location_switch = re.search(
        r"static const u8 \*GetLegendarySignLocationName\(.*?\n\}",
        legendary_runtime,
        re.S,
    )
    require(location_switch is not None, "Legendary Sign clue-location switch is missing")
    named_location_ids = set(re.findall(r"case (LEGENDARY_SIGN_[A-Z0-9_]+):", location_switch.group(0)))
    require(
        conditional_ids <= named_location_ids,
        "conditional Signs fall back to 'an unknown place': "
        + ", ".join(sorted(conditional_ids - named_location_ids)),
    )
    require("min(MAX_LEVEL, GetCurrentLevelCap())" in legendary_runtime, "Arceus reward level is not clamped")
    require(
        "signId != LEGENDARY_SIGN_ARCEUS && !IsLegendarySignCaught(signId)" in legendary_runtime,
        "Arceus mastery does not require every other finite Sign source",
    )
    require("MarkLegendarySignCaughtBySpecies" in read("src/battle_script_commands.c"), "wild catches do not close Signs")
    require("MarkLegendarySignCaughtBySpecies" in read("src/script_pokemon_util.c"), "gift catches do not close Signs")
    require("MarkLegendarySignCaughtBySpecies" in read("src/egg_hatch.c"), "Phione hatching does not close its Sign")
    require("SPECIES_MANAPHY" in read("src/daycare.c"), "Manaphy and Ditto breeding gate is missing")
    require("FLAG_HIDE_LEGENDARY_SIGN_DARKRAI" in read("data/scripts/new_game.inc"), "visible Sign reset flags are missing")

    # Inclement's exact physical roster remains the entire production
    # overworld roster. Magearna and Pecharunt deliberately use an existing NPC
    # and tombstone interaction, respectively, rather than adding species props.
    map_rows = {}
    for map_path in (ROOT / "data/maps").glob("*/map.json"):
        row = json.loads(map_path.read_text())
        map_rows[row["id"]] = (map_path, row)
    layouts = {
        row["id"]: row
        for row in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }
    visible_rows = re.findall(
        r"VISIBLE_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+), ([A-Z0-9_]+), ([A-Z0-9_]+),",
        definitions,
    )
    require(len(visible_rows) == 6, "visible Legendary Sign count drifted")
    for sign_id, species, map_name in visible_rows:
        map_id = f"MAP_{map_name}"
        require(map_id in map_rows, f"{sign_id} points at missing {map_id}")
        map_path, map_row = map_rows[map_id]
        if species in SCRIPTED_VISIBLE_ROOTS:
            relative, tokens = SCRIPTED_VISIBLE_ROOTS[species]
            scripts = read(relative)
            require(all(token in scripts for token in tokens),
                    f"{sign_id} scripted visible root is incomplete in {relative}")
            continue
        graphics_id = next(
            (gfx for gfx, (fixed_species, fixed_map) in EXACT_INCLEMENT_PHYSICAL.items()
             if fixed_species == species and fixed_map == map_id),
            None,
        )
        require(graphics_id is not None, f"{sign_id} is not an approved Inclement physical encounter")
        matching_objects = [
            obj for obj in map_row.get("object_events", [])
            if obj.get("graphics_id") == graphics_id
        ]
        require(len(matching_objects) == 1, f"{sign_id} needs exactly one fixed {species} object")
        obj = matching_objects[0]
        require(obj.get("flag") not in (None, "0"), f"{sign_id} has no persistent hide/catch flag")
        require(obj.get("local_id"), f"{sign_id} has no stable local object ID")
        require(obj.get("script") not in (None, "0x0"), f"{sign_id} has no encounter script")
        scripts = (map_path.parent / "scripts.inc").read_text()
        require(f"{obj['script']}::" in scripts, f"{sign_id} encounter script label is missing")
        for token in (
            "CreateSelectedLegendarySignEncounter",
            "BattleSetup_StartLegendaryBattle",
            obj["flag"],
            obj["local_id"],
        ):
            require(token in scripts, f"{sign_id} encounter script is missing {token}")

    physical_encounters = []
    for map_id, (map_path, map_row) in map_rows.items():
        for obj in map_row.get("object_events", []):
            graphics_id = obj.get("graphics_id", "")
            if graphics_id in EXACT_INCLEMENT_PHYSICAL:
                species, expected_map = EXACT_INCLEMENT_PHYSICAL[graphics_id]
                require(map_id == expected_map, f"{species} moved from its exact Inclement map")
                physical_encounters.append((map_id, map_path, species, obj))
            require(
                not re.fullmatch(r"OBJ_EVENT_GFX_SPECIES\(([^)]+)\)", graphics_id),
                f"{map_id} reintroduced a non-Inclement physical species prop: {graphics_id}",
            )
    require(len(physical_encounters) == 8, "exact Inclement physical encounter count drifted")
    require(
        len({species for _map_id, _map_path, species, _obj in physical_encounters}) == len(physical_encounters),
        "a supposedly one-off overworld species is represented by multiple physical encounters",
    )
    require(
        len({obj["flag"] for _map_id, _map_path, _species, obj in physical_encounters}) == len(physical_encounters),
        "physical one-off encounters share a persistence flag",
    )
    flags_header = read("include/constants/flags.h")
    for map_id, map_path, species, obj in physical_encounters:
        map_row = map_rows[map_id][1]
        script_label = obj.get("script")
        local_id = obj.get("local_id")
        object_flag = obj.get("flag")
        context = f"{map_id} {species}"
        layout = layouts[map_row["layout"]]
        require(
            0 <= obj.get("x", -1) < layout["width"] and 0 <= obj.get("y", -1) < layout["height"],
            f"{context} is placed outside its map layout",
        )
        require(script_label not in (None, "0x0"), f"{context} has no encounter script")
        require(local_id, f"{context} has no stable local object ID")
        require(object_flag not in (None, "0"), f"{context} has no persistent object flag")
        require(object_flag in flags_header, f"{context} uses an undefined object flag")
        require(obj.get("movement_type") in {"MOVEMENT_TYPE_NONE", "MOVEMENT_TYPE_FACE_DOWN"},
                f"{context} can wander away from its authored encounter position")
        scripts = (map_path.parent / "scripts.inc").read_text()
        require(f"{script_label}::" in scripts, f"{context} script label is missing")
        require("BattleSetup_StartLegendaryBattle" in scripts, f"{context} never starts a legendary battle")
        require(local_id in scripts, f"{context} has no caught-object cleanup path")
        require(
            object_flag in scripts or object_flag in legendary_runtime,
            f"{context} hide/catch flag is never synchronized",
        )
    require("OBJ_EVENT_GFX_REGIGIGAS_STATUE" in read("data/maps/SealedChamber_InnerRoom/map.json"),
            "Inclement's giant Regigigas statue is missing")
    require("VAR_LEGENDARY_SIGNS_UNLOCKED_5" in legendary_runtime, "all 82 Sign bits are not persisted")
    require(
        "GetEggSpecies(partySpecies) == requestedRoot"
        in read("src/legendary_signs.c"),
        "legendary requirements do not accept pre-evolutions from the named family",
    )
    for ultra_beast in ("BLACEPHALON", "BUZZWOLE", "GUZZLORD", "KARTANA", "NIHILEGO", "PHEROMOSA", "STAKATAKA"):
        require(
            f"ORDINARY_WILD_SIGN(LEGENDARY_SIGN_{ultra_beast}" in definitions,
            f"{ultra_beast} is not assigned to a restored-area wild table",
        )
    require("SPECIES_GENESECT" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"), "Genesect Game Corner reward is missing")
    require("SPECIES_POIPOLE" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"), "Poipole Game Corner reward is missing")

    generator_hash_before = hashlib.sha256((ROOT / "src/data/pokemon/showdown_champions_circuit.h").read_bytes()).hexdigest()
    print(f"Legendary Signs: {len(sign_ids)} complete acquisition definitions")
    print(f"Visible overworld encounters: {len(physical_encounters)} physical one-off Pokémon")
    print(f"Showdown Circuit: {manifest['variant_count']} variants, {manifest['template_count']} templates")
    print(f"Generated table SHA256: {generator_hash_before}")


if __name__ == "__main__":
    main()
