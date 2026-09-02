#!/usr/bin/env python3
"""Fail closed when protected Inclement v1.13 visual source drifts.

This gate covers source assets whose modern-engine equivalents have already
been mapped and visually approved.  Runtime screenshots remain a separate
requirement; source identity alone is not presentation proof.
"""

from __future__ import annotations

import hashlib
import re
import shlex
import struct
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

PROTECTED_TREES = {
    "graphics/pokedex/hgss": (33, "c402f103203759f3066291ad4b5aea67d64ce6365326045155b85a8a5ae8fdc9"),
    "graphics/summary_screen": (16, "455b808e38cb3ca5bf8b4da0b787590e931077d4c3a147f3dc0de37341b4b8c4"),
    "graphics/party_menu": (13, "a9823b46dd2aed17626018e567ba2cfcaa7f71559ba9e79029c5c6b35ecab54a"),
}

PROTECTED_FILES = {
    "graphics/object_events/pics/people/nurse.png":
        "6a85fba961746f9b096f60c398944343ba5bd4bff0f512f3049f419f783342e5",
}

DOOR_CANONICAL_COMPILED_SHA256 = "766257854603b723c5c3b95e14a9dc3158efc7254d6a2f5261639c0a9e6e5f71"
DOOR_TABLE_TOPOLOGY_SHA256 = "a28c2d7622132963d84d02fa2a9a8e65693ee773e18a4eb138b9f57c8d45c79e"
DOOR_CANONICAL_ALIASES = {
    "petalburg_gym": "battle_tower_corridor",
    "unused_battle_frontier": "unknown",
}


def fail(message: str) -> None:
    raise SystemExit(message)


def require_ordered(source: str, tokens: list[str], label: str) -> None:
    cursor = 0
    for token in tokens:
        position = source.find(token, cursor)
        if position < 0:
            fail(f"{label} lost ordered visual command: {token}")
        cursor = position + len(token)


def tree_digest(relative: str) -> tuple[int, str]:
    root = ROOT / relative
    files = sorted(path for path in root.rglob("*") if path.is_file())
    digest = hashlib.sha256()
    for path in files:
        digest.update(path.relative_to(root).as_posix().encode())
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return len(files), digest.hexdigest()


def png_dimensions(path: Path) -> tuple[int, int]:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        fail(f"invalid PNG used by door renderer: {path}")
    return struct.unpack(">II", data[16:24])


def emerald_door_assets(source: str) -> list[tuple[str, str, str]]:
    declarations = source[: source.index("#if IS_FRLG")]
    rows = re.findall(
        r"static const u8 (sDoorAnimTiles_[A-Za-z0-9_]+)\[\] = "
        r"INCGFX_U8\(\"graphics/door_anims/([a-z0-9_]+)\.png\", "
        r"\"\.4bpp\"(?:, \"([^\"]+)\")?\);",
        declarations,
    )
    if len(rows) != 53 or len({row[0] for row in rows}) != 53:
        fail(f"Emerald door asset declaration roster drifted: {len(rows)}")
    return rows


def door_table_rows(source: str) -> list[tuple[str, ...]]:
    start = source.index("static const struct DoorGraphics sDoorAnimGraphicsTable")
    start = source.index("#if !IS_FRLG", start)
    end = source.index("#else", start)
    rows = []
    for block in re.findall(r"\{(.*?)\}", source[start:end], re.DOTALL):
        def field(pattern: str) -> str:
            match = re.search(pattern, block)
            return match.group(1) if match else ""

        metatile = field(r"\.metatileNum\s*=\s*([A-Za-z0-9_]+)")
        if not metatile:
            continue
        rows.append((
            metatile,
            field(r"\.tileset\s*=\s*&([A-Za-z0-9_]+)"),
            field(r"\.sound\s*=\s*([A-Za-z0-9_]+)"),
            field(r"\.size\s*=\s*([A-Za-z0-9_]+)"),
            field(r"(sDoorAnimTiles_[A-Za-z0-9_]+)"),
            field(r"\.palettes\s*=\s*([A-Za-z0-9_]+)"),
        ))
    return rows


def door_table_digest(source: str) -> str:
    payload = "\n".join("|".join(row) for row in door_table_rows(source)) + "\n"
    return hashlib.sha256(payload.encode()).hexdigest()


def compiled_door_digest(source: str) -> tuple[int, str]:
    gbagfx = ROOT / "tools/gbagfx/gbagfx"
    if not gbagfx.is_file():
        fail(f"door byte gate requires the canonical converter: {gbagfx}")

    compiled: list[tuple[str, bytes]] = []
    with tempfile.TemporaryDirectory(prefix="ec-door-contract-") as temp:
        temp_root = Path(temp)
        for variable, asset_name, options in emerald_door_assets(source):
            source_png = ROOT / "graphics/door_anims" / f"{asset_name}.png"
            if not source_png.is_file():
                fail(f"missing Emerald door asset: {source_png}")
            output = temp_root / f"{variable}.4bpp"
            command = [str(gbagfx), str(source_png), str(output)]
            if options:
                command.extend(shlex.split(options))
            subprocess.run(command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            canonical_name = DOOR_CANONICAL_ALIASES.get(asset_name, asset_name)
            compiled.append((canonical_name, output.read_bytes()))

    digest = hashlib.sha256()
    for canonical_name, payload in sorted(compiled):
        digest.update(canonical_name.encode())
        digest.update(b"\0")
        digest.update(payload)
        digest.update(b"\0")
    return len(compiled), digest.hexdigest()


def verify_door_semantics() -> None:
    source = (ROOT / "src/field_door.c").read_text()
    rows = door_table_rows(source)
    if len(rows) != 53 or door_table_digest(source) != DOOR_TABLE_TOPOLOGY_SHA256:
        fail("Emerald door metatile/tileset/sound/size topology drifted")

    asset_count, compiled_digest = compiled_door_digest(source)
    if asset_count != 53 or compiled_digest != DOOR_CANONICAL_COMPILED_SHA256:
        fail(
            "compiled Emerald door bytes drifted from Inclement: "
            f"count={asset_count} digest={compiled_digest}"
        )

    multi = ROOT / "graphics/door_anims/battle_tower_multi_corridor.png"
    if png_dimensions(multi) != (32, 96):
        fail(f"Battle Tower multi-corridor source repack drifted: {png_dimensions(multi)}")
    multi_decl = next(row for row in emerald_door_assets(source) if row[1] == "battle_tower_multi_corridor")
    if multi_decl[2] != "-mwidth 2 -mheight 4":
        fail(f"Battle Tower multi-corridor compile geometry drifted: {multi_decl[2]}")

    mutated_source = source.replace(".sound = DOOR_SOUND_NORMAL", ".sound = DOOR_SOUND_SLIDING", 1)
    if door_table_digest(mutated_source) == DOOR_TABLE_TOPOLOGY_SHA256:
        fail("door-table mutation probe escaped")

    mutated_digest = hashlib.sha256()
    mutated_digest.update(bytes.fromhex(compiled_digest))
    mutated_digest.update(b"mutated-door-byte")
    if mutated_digest.hexdigest() == DOOR_CANONICAL_COMPILED_SHA256:
        fail("door-byte mutation probe escaped")


def verify_nurse_frames() -> None:
    source = (ROOT / "src/data/object_events/object_event_pic_tables.h").read_text()
    match = re.search(
        r"static const struct SpriteFrameImage sPicTable_Nurse\[\] = \{(?P<body>.*?)\n\};",
        source,
        re.DOTALL,
    )
    if match is None:
        fail("Inclement Nurse Joy picture table is missing")
    frames = [
        int(frame)
        for frame in re.findall(
            r"overworld_frame\(gObjectEventPic_Nurse, 2, 4, (\d+)\)",
            match.group("body"),
        )
    ]
    if frames != list(range(10)):
        fail(f"Inclement Nurse Joy ten-frame sequence drifted: {frames}")


def verify_nurse_choreography() -> None:
    source = (ROOT / "data/scripts/pkmn_center_nurse.inc").read_text()
    match = re.search(
        r"EventScript_PkmnCenterNurse_TakeAndHealPkmn::(?P<body>.*?)"
        r"\nEventScript_PkmnCenterNurse_CheckTrainerHillAndUnionRoom::",
        source,
        re.DOTALL,
    )
    if match is None:
        fail("Nurse Joy healing choreography is missing")

    body = match.group("body")
    visual_commands = [
        line.strip()
        for line in body.splitlines()
        if line.strip().startswith((
            "applymovement VAR_0x800B,",
            "waitmovement ",
            "dofieldeffect ",
            "waitfieldeffect ",
        ))
    ]
    expected_visual_commands = [
        "applymovement VAR_0x800B, Common_Movement_FaceLeft",
        "waitmovement 0",
        "dofieldeffect FLDEFF_POKECENTER_HEAL",
        "waitfieldeffect FLDEFF_POKECENTER_HEAL",
        "applymovement VAR_0x800B, Common_Movement_FaceDown",
        "waitmovement 0",
    ]
    if visual_commands != expected_visual_commands:
        fail(
            "Nurse Joy healing choreography drifted: "
            f"{visual_commands} != {expected_visual_commands}"
        )

    preserved_service_steps = [
        "hidefollower 0",
        "special HealPlayerParty",
        "copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES",
        "callnative UpdateFollowingPokemon",
    ]
    expected_lifecycle = [
        preserved_service_steps[0],
        *expected_visual_commands,
        *preserved_service_steps[1:],
    ]
    lifecycle_commands = [
        line.strip()
        for line in body.splitlines()
        if line.strip() in set(expected_lifecycle)
    ]
    if lifecycle_commands != expected_lifecycle:
        fail("Nurse Joy healing no longer preserves follower, party, and Vial updates")


def verify_story_choreography() -> None:
    sootopolis = (ROOT / "data/maps/SootopolisCity/scripts.inc").read_text()
    require_ordered(sootopolis, [
        "addobject LOCALID_SOOTOPOLIS_RAYQUAZA",
        "special Script_DoRayquazaScene",
        "applymovement LOCALID_SOOTOPOLIS_RAYQUAZA, SootopolisCity_Movement_RayquazaFlyOff",
        "removeobject LOCALID_SOOTOPOLIS_RAYQUAZA",
    ], "Sootopolis Rayquaza fly-off")
    rayquaza_region = sootopolis.split(
        "SootopolisCity_EventScript_RayquazaSceneFromPokeCenter::", 1
    )[1].split("SootopolisCity_EventScript_RayquazaSceneFromDive::", 1)[0]
    if "hideobject" in rayquaza_region or "setobjectinvisible" in rayquaza_region:
        fail("Sootopolis Rayquaza is hidden before its visible fly-off")

    mt_pyre = (ROOT / "data/maps/MtPyre_Summit/scripts.inc").read_text()
    require_ordered(mt_pyre, [
        "fadescreenswapbuffers FADE_TO_BLACK",
        "removeobject LOCALID_MT_PYRE_SUMMIT_ARCHIE",
        "removeobject LOCALID_MT_PYRE_SUMMIT_GRUNT_4",
        "removeobject LOCALID_EC_MATT_MT_PYRE",
        "fadescreenswapbuffers FADE_FROM_BLACK",
    ], "Mt. Pyre Team Aqua departure")

    magma = (ROOT / "data/maps/MagmaHideout_4F/scripts.inc").read_text()
    require_ordered(magma, [
        "MagmaHideout_4F_Movement_GroudonApproach",
        "special FadeOutOrbEffect",
        "playmoncry SPECIES_GROUDON, CRY_MODE_ENCOUNTER",
        "waitmoncry",
        "special ShakeCamera",
        "MagmaHideout_4F_Movement_GroudonExit",
    ], "Groudon awakening")

    seafloor = (ROOT / "data/maps/SeafloorCavern_Room9/scripts.inc").read_text()
    require_ordered(seafloor, [
        "SeafloorCavern_Room9_Movement_KyogreApproach",
        "special FadeOutOrbEffect",
        "playmoncry SPECIES_KYOGRE, CRY_MODE_ENCOUNTER",
        "waitmoncry",
        "special ShakeCamera",
        "SeafloorCavern_Room9_Movement_KyogreExit",
    ], "Kyogre awakening")

    leaf = (ROOT / "data/maps/AlteringCave_B1F/scripts.inc").read_text()
    require_ordered(leaf, [
        "applymovement LOCALID_EC_LEAF, Common_Movement_ExclamationMark",
        "applymovement LOCALID_EC_LEAF, AlteringCave_B1F_Movement_LeafApproach",
        "applymovement LOCALID_PLAYER, AlteringCave_B1F_Movement_PlayerApproach",
        "trainerbattle_no_intro_double TRAINER_LEAF_ALTERING_CAVE",
        "applymovement LOCALID_PLAYER, AlteringCave_B1F_Movement_PlayerGiveWay",
        "applymovement LOCALID_EC_LEAF, AlteringCave_B1F_Movement_LeafLeaves",
        "removeobject LOCALID_EC_LEAF",
    ], "Leaf approach/departure")

    rotom = (ROOT / "data/maps/NewMauville_Inside/scripts.inc").read_text()
    rotom_region = rotom.split("NewMauville_Inside_EventScript_RotomEncounter::", 1)[1]
    require_ordered(rotom_region, [
        "playmoncry SPECIES_ROTOM, CRY_MODE_ENCOUNTER",
        "applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp",
        "applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark",
        "applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_Delay48",
        "msgbox NewMauville_Inside_Text_RotomAppears",
    ], "Rotom surprise reaction")


def main() -> None:
    for relative, expected in PROTECTED_TREES.items():
        actual = tree_digest(relative)
        if actual != expected:
            fail(f"protected Inclement visual tree drifted: {relative}: {actual} != {expected}")
        print(f"PASS: {relative} ({actual[0]} files)")

    for relative, expected in PROTECTED_FILES.items():
        actual = hashlib.sha256((ROOT / relative).read_bytes()).hexdigest()
        if actual != expected:
            fail(f"protected Inclement visual file drifted: {relative}: {actual} != {expected}")
        print(f"PASS: {relative}")

    verify_nurse_frames()
    print("PASS: Nurse Joy uses Inclement's ten unique frames")

    verify_nurse_choreography()
    print("PASS: Nurse Joy uses Inclement's healing choreography")

    verify_story_choreography()
    print("PASS: high-risk Inclement story choreography is ordered and visible")

    verify_door_semantics()
    print("PASS: all 53 Emerald door assets and renderer mappings compile to Inclement bytes")

    graphics = (ROOT / "src/graphics.c").read_text()
    if 'INCGFX_U16("graphics/summary_screen/tiles.pal", ".gbapal")' not in graphics:
        fail("Summary screen no longer loads Inclement's complete eight-bank palette")
    print("INCLEMENT VISUAL SOURCE GATE: PASS")


if __name__ == "__main__":
    main()
