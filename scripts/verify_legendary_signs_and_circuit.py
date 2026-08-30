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

    sign_ids = re.findall(r"(?:WILD|VISIBLE|OTHER)_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+)", definitions)
    require(len(sign_ids) == 82 and len(set(sign_ids)) == 82, "Legendary Sign definitions are incomplete or duplicated")
    require("MIRAGE_TOWER" not in definitions, "a Sign still depends on collapsible Mirage Tower")
    require("SAFARI_ZONE" not in definitions, "a Sign still requires Safari capture rules")
    legendary_runtime = read("src/legendary_signs.c")
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

    # Every visible Sign is a physical, persistent encounter rather than a
    # dossier-only promise.  Derive this from the live definition table so a
    # newly added visible quest cannot silently omit its map object or script.
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
    require(len(visible_rows) == 25, "visible Legendary Sign count drifted")
    for sign_id, species, map_name in visible_rows:
        map_id = f"MAP_{map_name}"
        require(map_id in map_rows, f"{sign_id} points at missing {map_id}")
        map_path, map_row = map_rows[map_id]
        matching_objects = [
            obj for obj in map_row.get("object_events", [])
            if obj.get("graphics_id") == f"OBJ_EVENT_GFX_SPECIES({species})"
        ]
        require(len(matching_objects) == 1, f"{sign_id} needs exactly one {species} overworld object")
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
        sprite = ROOT / "graphics/pokemon" / species.lower() / "overworld.png"
        verify_overworld_sprite(sprite, sign_id)

    physical_encounters = []
    for map_id, (map_path, map_row) in map_rows.items():
        for obj in map_row.get("object_events", []):
            match = re.fullmatch(r"OBJ_EVENT_GFX_SPECIES\(([^)]+)\)", obj.get("graphics_id", ""))
            if match and match.group(1) not in {"CARBINK", "CHANSEY"}:
                physical_encounters.append((map_id, map_path, match.group(1), obj))
    require(len(physical_encounters) == 32, "physical one-off Pokémon encounter count drifted")
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
        require(
            obj.get("movement_type") == "MOVEMENT_TYPE_FACE_DOWN",
            f"{context} can wander away from its authored encounter position",
        )
        scripts = (map_path.parent / "scripts.inc").read_text()
        require(f"{script_label}::" in scripts, f"{context} script label is missing")
        require("BattleSetup_StartLegendaryBattle" in scripts, f"{context} never starts a legendary battle")
        require(local_id in scripts, f"{context} has no caught-object cleanup path")
        require(
            object_flag in scripts or object_flag in legendary_runtime,
            f"{context} hide/catch flag is never synchronized",
        )
        sprite = ROOT / "graphics/pokemon" / species.lower() / "overworld.png"
        verify_overworld_sprite(sprite, context)

    require("OBJ_EVENT_GFX_SPECIES(DARKRAI)" in read("data/maps/MtPyre_Summit/map.json"), "Darkrai overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(CRESSELIA)" in read("data/maps/MeteorFalls_B1F_2R/map.json"), "Cresselia overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(DIALGA)" in read("data/maps/MeteorFalls_B1F_1R/map.json"), "Dialga overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(LANDORUS)" in read("data/maps/Route111_RuinsExterior/map.json"), "Landorus overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(THUNDURUS)" in read("data/maps/Route110/map.json"), "Thundurus overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(TORNADUS)" in read("data/maps/Route119/map.json"), "Tornadus overworld object is missing")
    require(
        "VISIBLE_SIGN(LEGENDARY_SIGN_THUNDURUS, THUNDURUS, ROUTE110, 3, 1, MANECTRIC, FLAG_BADGE03_GET)"
        in definitions,
        "Thundurus is not a visible early-midgame Route 110 encounter",
    )
    require(
        "VISIBLE_SIGN(LEGENDARY_SIGN_TORNADUS, TORNADUS, ROUTE119, 5, 1, CASTFORM, FLAG_BADGE05_GET)"
        in definitions,
        "Tornadus is not a visible midgame Route 119 encounter",
    )
    for map_id, species in (("MAP_ROUTE110", "THUNDURUS"), ("MAP_ROUTE119", "TORNADUS")):
        _, map_row = map_rows[map_id]
        obj = next(
            row for row in map_row["object_events"]
            if row.get("graphics_id") == f"OBJ_EVENT_GFX_SPECIES({species})"
        )
        layout = layouts[map_row["layout"]]
        width = layout["width"]
        height = layout["height"]
        blocks = (ROOT / layout["blockdata_filepath"]).read_bytes()

        def block_value(x: int, y: int) -> int:
            offset = 2 * (y * width + x)
            return int.from_bytes(blocks[offset:offset + 2], "little")

        value = block_value(obj["x"], obj["y"])
        require((value >> 10) & 3 == 0, f"{species} is standing on an impassable map block")
        open_neighbors = 0
        for x, y in (
            (obj["x"] - 1, obj["y"]), (obj["x"] + 1, obj["y"]),
            (obj["x"], obj["y"] - 1), (obj["x"], obj["y"] + 1),
        ):
            if 0 <= x < width and 0 <= y < height and ((block_value(x, y) >> 10) & 3) == 0:
                open_neighbors += 1
        require(open_neighbors >= 3, f"{species} blocks a narrow route corridor")
    require("OBJ_EVENT_GFX_SPECIES(REGIGIGAS)" in read("data/maps/SealedChamber_InnerRoom/map.json"), "giant Regigigas object is missing")
    require("SIZE_64x64" in read("src/data/pokemon/species_info/gen_4_families.h"), "Regigigas is not using its giant overworld size")
    require(
        verify_overworld_sprite(ROOT / "graphics/pokemon/regigigas/overworld.png", "REGIGIGAS") == (384, 64),
        "Regigigas does not use the complete giant 64x64 overworld sheet",
    )
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
