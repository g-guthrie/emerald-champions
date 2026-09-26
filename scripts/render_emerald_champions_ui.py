#!/usr/bin/env python3
"""Render deterministic Emerald Champions UI/world screenshots with libmGBA."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile
import zlib


import native_tools

ROOT = Path(__file__).resolve().parents[1]
RUNNER_SOURCE = ROOT / "tests/headless/emerald_champions_mgba_runner.c"
DEFAULT_ROM = ROOT / "pokeemerald-headless.gba"
DEFAULT_ELF = ROOT / "pokeemerald-headless.elf"
DEFAULT_OUT = ROOT / "work/visual-audit/rendered/current"
OVERWORLD_FIXTURE_TABLE = ROOT / "include/emerald_champions_headless_overworld_fixtures.h"
HEADLESS_HEADER = ROOT / "include/emerald_champions_headless.h"


def scenario_id(name: str) -> int:
    """Index of EC_HEADLESS_SCENARIO_<name>, read from the native enum so the
    fixture ids cannot drift when scenarios are added or removed."""
    header = HEADLESS_HEADER.read_text()
    body = header.split("enum EmeraldChampionsHeadlessScenario", 1)[1].split("};", 1)[0]
    names = re.findall(r"EC_HEADLESS_SCENARIO_\w+", body)
    return names.index("EC_HEADLESS_SCENARIO_" + name)


SCENARIO_SYMBOL = "gEcHeadlessFixtureScenario"
GENERIC_OVERWORLD_SCENARIO_ID = scenario_id("SPECIES_OVERWORLD")
RESULT_PATTERN = re.compile(r"^RESULT .*video_hash=(?P<video>[0-9a-f]{16}) ", re.MULTILINE)
READ_PATTERN = re.compile(
    r"^READ width=4 address=(?P<address>[0-9a-f]{8}) value=(?P<value>[0-9a-f]{8})$",
    re.MULTILINE,
)


SCENARIOS: dict[str, dict[str, object]] = {
    "center-oldale": {"id": scenario_id("CENTER_OLDALE"), "frames": 600, "keys": []},
    "nurse-heal-facing-machine": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 1,
        "frames": 1690,
        "keys": [
            (200, 100, "UP"),
            (360, 2, "A"),
            (1400, 2, "A"),
            (1480, 2, "A"),
            (1560, 2, "A"),
        ],
        "verify": True,
    },
    "nurse-heal-tray": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 1,
        "frames": 1750,
        "keys": [
            (200, 100, "UP"),
            (360, 2, "A"),
            (1400, 2, "A"),
            (1480, 2, "A"),
            (1560, 2, "A"),
        ],
        "verify": True,
    },
    "nurse-heal-return": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 1,
        "frames": 1900,
        "keys": [
            (200, 100, "UP"),
            (360, 2, "A"),
            (1400, 2, "A"),
            (1480, 2, "A"),
            (1560, 2, "A"),
        ],
        "verify": True,
    },
    "whiteout-heal-placement": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 2,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "whiteout-heal-league-placement": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 4,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "whiteout-heal-lavaridge-placement": {
        "id": scenario_id("CENTER_OLDALE"),
        "param": 6,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "center-lavaridge": {"id": scenario_id("CENTER_LAVARIDGE"), "frames": 600, "keys": []},
    "ability-menu": {
        "id": scenario_id("ABILITY_MENU"),
        "frames": 520,
        "keys": [(210, 2, "A"), (250, 2, "DOWN"), (290, 2, "A")],
    },
    "ability-back-to-actions": {
        "id": scenario_id("ABILITY_MENU"),
        "frames": 680,
        "keys": [(210, 2, "A"), (250, 2, "DOWN"), (290, 2, "A"), (560, 2, "B")],
    },
    "ability-cancel-to-actions": {
        "id": scenario_id("ABILITY_MENU"),
        "frames": 720,
        "keys": [
            (210, 2, "A"),
            (250, 2, "DOWN"),
            (290, 2, "A"),
            (560, 2, "UP"),
            (600, 2, "A"),
        ],
    },
    "ability-applied-message": {
        "id": scenario_id("ABILITY_MENU"),
        "frames": 680,
        "keys": [
            (210, 2, "A"),
            (250, 2, "DOWN"),
            (290, 2, "A"),
            (560, 2, "DOWN"),
            (600, 2, "A"),
        ],
    },
    "ability-applied-return": {
        "id": scenario_id("ABILITY_MENU"),
        "frames": 820,
        "keys": [
            (210, 2, "A"),
            (250, 2, "DOWN"),
            (290, 2, "A"),
            (560, 2, "DOWN"),
            (600, 2, "A"),
            (720, 2, "A"),
        ],
    },
    "party-overview": {"id": scenario_id("ABILITY_MENU"), "frames": 300, "keys": []},
    "party-action-menu": {"id": scenario_id("ABILITY_MENU"), "frames": 245, "keys": [(210, 2, "A")]},
    "options": {"id": scenario_id("OPTIONS"), "frames": 520, "keys": []},
    "battle-vendor": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 760,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A"), (460, 2, "A")],
    },
    "battle-vendor-category-back": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 900,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "B"),
        ],
    },
    "battle-vendor-postbadge-root": {
        "id": scenario_id("BATTLE_VENDOR"),
        "param": 1,
        "frames": 620,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A")],
    },
    "battle-vendor-postbadge-held-items": {
        "id": scenario_id("BATTLE_VENDOR"),
        "param": 1,
        "frames": 900,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (600, 2, "A"),
        ],
    },
    "battle-vendor-shop": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1040,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
        ],
    },
    "battle-vendor-quantity": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1240,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
        ],
    },
    "battle-vendor-quantity-adjusted": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1300,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1200, 2, "UP"),
        ],
    },
    "battle-vendor-quantity-back": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1480,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1320, 2, "B"),
        ],
    },
    "battle-vendor-confirm": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1580,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1320, 2, "A"),
        ],
    },
    "battle-vendor-confirm-no": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1760,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1320, 2, "A"),
            (1540, 2, "B"),
        ],
    },
    "battle-vendor-purchase-success": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 3400,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1320, 2, "A"),
            (1540, 2, "A"),
        ],
    },
    "battle-vendor-purchase-return": {
        "id": scenario_id("BATTLE_VENDOR"),
        "frames": 1940,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (460, 2, "A"),
            (800, 2, "A"),
            (1080, 2, "A"),
            (1320, 2, "A"),
            (1540, 2, "A"),
            (1820, 2, "A"),
        ],
    },
    "move-specialist-root": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 350,
        "keys": [(220, 2, "UP"), (250, 2, "A")],
    },
    "move-specialist-root-back": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 520,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "B")],
    },
    "move-specialist-party-prompt": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "A")],
    },
    "move-specialist-party-back": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 880,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (740, 2, "B"),
        ],
    },
    "capture-to-party": {
        "id": scenario_id("CAPTURE_TO_PARTY"),
        "frames": 3000,
        "keys": [
            (500, 2, "A"), (700, 2, "A"), (900, 2, "A"),
            (1100, 2, "A"), (1300, 2, "A"), (1500, 2, "A"),
            (1700, 2, "A"), (1900, 2, "A"), (2100, 2, "A"),
            (2300, 2, "A"), (2500, 2, "A"), (2700, 2, "A"),
        ],
        "verify": True,
    },
    "capture-to-pc": {
        "id": scenario_id("CAPTURE_TO_PC"),
        "frames": 3000,
        "keys": [
            (500, 2, "A"), (700, 2, "A"), (900, 2, "A"),
            (1100, 2, "A"), (1300, 2, "A"), (1500, 2, "A"),
            (1700, 2, "A"), (1900, 2, "A"), (2100, 2, "A"),
            (2300, 2, "A"), (2500, 2, "A"), (2700, 2, "A"),
        ],
        "verify": True,
    },
    "capture-quest-diancie": {
        "id": scenario_id("CAPTURE_QUEST_DIANCIE"),
        "frames": 4200,
        "keys": [
            (300, 20, "UP"), (380, 2, "A"),
            (600, 2, "A"), (800, 2, "A"), (1000, 2, "A"),
            (1200, 2, "A"), (1400, 2, "A"), (1600, 2, "A"),
            (1800, 2, "A"), (2000, 2, "A"), (2200, 2, "A"),
            (2400, 2, "A"), (2600, 2, "A"), (2800, 2, "A"),
            (3000, 2, "A"), (3200, 2, "A"), (3400, 2, "A"),
            (3600, 2, "A"), (3800, 2, "A"), (4000, 2, "A"),
        ],
        "verify": True,
    },
    "capture-quest-registeel": {
        "id": scenario_id("CAPTURE_QUEST_REGISTEEL"),
        "frames": 4200,
        "keys": [
            (300, 20, "UP"), (380, 2, "A"),
            (600, 2, "A"), (800, 2, "A"), (1000, 2, "A"),
            (1200, 2, "A"), (1400, 2, "A"), (1600, 2, "A"),
            (1800, 2, "A"), (2000, 2, "A"), (2200, 2, "A"),
            (2400, 2, "A"), (2600, 2, "A"), (2800, 2, "A"),
            (3000, 2, "A"), (3200, 2, "A"), (3400, 2, "A"),
            (3600, 2, "A"), (3800, 2, "A"), (4000, 2, "A"),
        ],
        "verify": True,
    },
    "capture-quest-latios": {
        "id": scenario_id("CAPTURE_QUEST_LATIOS"),
        "frames": 5000,
        "keys": [
            (300, 20, "UP"), (380, 2, "A"),
            (600, 2, "A"), (800, 2, "A"), (1000, 2, "A"),
            (1200, 2, "A"), (1400, 2, "A"), (1600, 2, "A"),
            (1800, 2, "A"), (2000, 2, "A"), (2200, 2, "A"),
            (2400, 2, "A"), (2600, 2, "A"), (2800, 2, "A"),
            (3000, 2, "A"), (3200, 2, "A"), (3400, 2, "A"),
            (3600, 2, "A"), (3800, 2, "A"), (4000, 2, "A"),
            (4200, 2, "A"), (4400, 2, "A"), (4600, 2, "A"),
            (4800, 2, "A"),
        ],
        "verify": True,
    },
    "capture-ordinary-first": {
        "id": scenario_id("CAPTURE_ORDINARY_FIRST"),
        "frames": 3200,
        "keys": [
            (500, 2, "A"), (700, 2, "A"), (900, 2, "A"),
            (1100, 2, "A"), (1300, 2, "A"), (1500, 2, "A"),
            (1700, 2, "A"), (1900, 2, "A"), (2100, 2, "A"),
            (2300, 2, "A"), (2500, 2, "A"), (2700, 2, "A"),
            (2900, 2, "A"), (3100, 2, "A"),
        ],
        "verify": True,
    },
    "roxanne-victory": {
        "id": scenario_id("ROXANNE_VICTORY"),
        "frames": 6500,
        "keys": [(300, 20, "UP"), (380, 2, "A")],
        "repeat_key": (420, 6460, 40, 2, "A"),
        "verify": True,
    },
    "first-center-acquisition": {
        "id": scenario_id("FIRST_CENTER_ACQUISITION"),
        "frames": 7500,
        "keys": [(300, 20, "UP"), (380, 2, "A")],
        "repeat_key": (420, 7460, 40, 2, "A"),
        "verify": True,
        "stop_on_observed": True,
    },
    "move-specialist-learn-move-party": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 940,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "A"), (640, 2, "A")],
    },
    "move-specialist-learn-move-back": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 1120,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "A"), (640, 2, "A"), (980, 2, "B")],
    },
    "all-legal-moves": {
        "id": scenario_id("MOVE_SPECIALIST"),
        "frames": 1050,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (340, 2, "DOWN"),
            (380, 2, "DOWN"),
            (420, 2, "A"),
            (560, 2, "A"),
            (700, 2, "A"),
        ],
    },
    "game-corner-prizes": {
        "id": scenario_id("GAME_CORNER"),
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A"), (480, 2, "A")],
    },
    "game-corner-regions": {
        "id": scenario_id("GAME_CORNER"),
        "frames": 980,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (480, 2, "A"),
            (620, 2, "DOWN"),
            (650, 2, "DOWN"),
            (680, 2, "A"),
            (800, 2, "A"),
        ],
    },
    "game-corner-region-list": {
        "id": scenario_id("GAME_CORNER"),
        "frames": 760,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (360, 2, "A"),
            (480, 2, "A"),
            (620, 2, "DOWN"),
            (650, 2, "DOWN"),
            (680, 2, "A"),
        ],
    },
    "circuit-lobby": {"id": scenario_id("CIRCUIT_LOBBY"), "frames": 600, "keys": []},
    "circuit-welcome": {
        "id": scenario_id("CIRCUIT_LOBBY"),
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A")],
    },
    "leveler-complete": {"id": scenario_id("LEVELER"), "frames": 700, "keys": []},
    "all-legal-moves-direct": {"id": scenario_id("ALL_LEGAL_MOVES"), "frames": 650, "keys": []},
    "all-legal-move-selected": {
        "id": scenario_id("ALL_LEGAL_MOVES"),
        "frames": 1050,
        "keys": [(700, 2, "A")],
    },
    "all-legal-move-selected-back": {
        "id": scenario_id("ALL_LEGAL_MOVES"),
        "frames": 1250,
        "keys": [(700, 2, "A"), (1080, 2, "B")],
    },
    "all-legal-move-confirmed": {
        "id": scenario_id("ALL_LEGAL_MOVES"),
        "frames": 1500,
        "keys": [(700, 2, "A"), (1080, 2, "A")],
    },
    "all-legal-move-give-up": {
        "id": scenario_id("ALL_LEGAL_MOVES"),
        "frames": 1000,
        "keys": [(700, 2, "B")],
    },
    "all-legal-move-give-up-no": {
        "id": scenario_id("ALL_LEGAL_MOVES"),
        "frames": 1220,
        "keys": [(700, 2, "B"), (1040, 2, "B")],
    },
    "all-legal-moves-mew": {"id": scenario_id("ALL_LEGAL_MOVES_MEW"), "frames": 650, "keys": []},
    "all-legal-moves-mew-middle": {
        "id": scenario_id("ALL_LEGAL_MOVES_MEW"),
        "frames": 1500,
        "keys": [(400, 1000, "DOWN")],
    },
    "all-legal-moves-mew-final": {
        "id": scenario_id("ALL_LEGAL_MOVES_MEW"),
        "frames": 3500,
        "keys": [(400, 3000, "DOWN")],
    },
    "all-legal-move-replacement": {
        "id": scenario_id("MOVE_REPLACEMENT"),
        "frames": 1000,
        "keys": [(400, 2, "A")],
        "verify": True,
        "stop_on_observed": True,
    },
    "dewford-gym-entry": {
        "id": scenario_id("DEWFORD_GYM_ENTRY"),
        "frames": 1000,
        "keys": [(300, 120, "UP")],
        "verify": True,
    },
    "start-menu-full": {"id": scenario_id("START_MENU_FULL"), "frames": 700, "keys": [(400, 2, "START")]},
    "start-menu-scroll-bottom": {
        "id": scenario_id("START_MENU_FULL"),
        "frames": 900,
        "keys": [(400, 2, "START"), (480, 2, "DOWN"), (492, 2, "DOWN"), (504, 2, "DOWN"), (516, 2, "DOWN"), (528, 2, "DOWN"), (540, 2, "DOWN"), (552, 2, "DOWN"), (564, 2, "DOWN")],
    },
    "start-menu-wrap-up": {"id": scenario_id("START_MENU_FULL"), "frames": 800, "keys": [(400, 2, "START"), (480, 2, "UP")]},
    "start-menu-wrap-down": {
        "id": scenario_id("START_MENU_FULL"),
        "frames": 900,
        "keys": [(400, 2, "START"), (480, 2, "DOWN"), (492, 2, "DOWN"), (504, 2, "DOWN"), (516, 2, "DOWN"), (528, 2, "DOWN"), (540, 2, "DOWN"), (552, 2, "DOWN"), (564, 2, "DOWN"), (576, 2, "DOWN")],
    },
    "wild-action-menu": {
        "id": scenario_id("WILD_ACTION_MENU"),
        "frames": 1500,
        "keys": [(900, 2, "A"), (1100, 2, "A")],
        "verify": True,
    },
    "wild-foe-types": {
        "id": scenario_id("WILD_FOE_TYPES"),
        "frames": 1500,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1300, 2, "L")],
        "verify": True,
    },
    "move-details": {
        "id": scenario_id("MOVE_DETAILS"),
        "frames": 1750,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1400, 2, "A"), (1550, 2, "R")],
        "verify": True,
    },
    "move-foe-types": {
        "id": scenario_id("MOVE_FOE_TYPES"),
        "frames": 1750,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1400, 2, "A"), (1550, 2, "L")],
        "verify": True,
    },
    "naming": {"id": scenario_id("NAMING"), "frames": 600, "keys": []},
    "storage-root": {"id": scenario_id("STORAGE"), "frames": 700, "keys": []},
    "storage-boxes": {"id": scenario_id("STORAGE"), "frames": 1400, "keys": [(760, 2, "A")]},
    "storage-box-popup": {
        "id": scenario_id("STORAGE"),
        "frames": 1900,
        "keys": [(760, 2, "A"), (1250, 2, "START"), (1370, 2, "A"), (1510, 2, "A")],
    },
    "storage-move-items": {
        "id": scenario_id("STORAGE"),
        "frames": 2800,
        "keys": [
            (900, 2, "DOWN"),
            (1100, 2, "A"),
            (1600, 2, "A"),
            (2000, 2, "DOWN"),
            (2200, 2, "DOWN"),
            (2400, 2, "A"),
        ],
    },
    "starter-regions": {
        "id": scenario_id("STARTER_REGIONS"),
        "frames": 1100,
        "keys": [(700, 2, "A"), (840, 2, "A")],
    },
    "circuit-room": {"id": scenario_id("CIRCUIT_ROOM"), "frames": 900, "keys": []},
    "pokedex": {"id": scenario_id("POKEDEX"), "param": 0, "frames": 900, "keys": [], "verify": True},
    "pokedex-info": {
        "id": scenario_id("POKEDEX"),
        "param": 1,
        "frames": 1200,
        "keys": [(900, 2, "A")],
        "verify": True,
    },
    "pokedex-area": {
        "id": scenario_id("POKEDEX"),
        "param": 2,
        "frames": 1400,
        "keys": [(900, 2, "A"), (1200, 2, "RIGHT")],
        "verify": True,
    },
    "pokedex-area-ashen-woods": {"id": scenario_id("POKEDEX"), "param": 20, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-dewford-meadow": {"id": scenario_id("POKEDEX"), "param": 21, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-verdanturf-meadow": {"id": scenario_id("POKEDEX"), "param": 22, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-late": {"id": scenario_id("POKEDEX"), "param": 20, "frames": 1460, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-verdanturf-meadow-late": {"id": scenario_id("POKEDEX"), "param": 22, "frames": 1460, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-b": {"id": scenario_id("POKEDEX"), "param": 20, "frames": 1413, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-c": {"id": scenario_id("POKEDEX"), "param": 20, "frames": 1426, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-stats": {
        "id": scenario_id("POKEDEX"),
        "param": 3,
        "frames": 1580,
        "keys": [(900, 2, "A"), (1200, 2, "RIGHT"), (1380, 2, "RIGHT")],
        "verify": True,
    },
    "pokedex-evolutions": {
        "id": scenario_id("POKEDEX"),
        "param": 4,
        "frames": 1760,
        "keys": [
            (900, 2, "A"),
            (1200, 2, "RIGHT"),
            (1380, 2, "RIGHT"),
            (1560, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "pokedex-forms": {
        "id": scenario_id("POKEDEX"),
        "param": 5,
        "frames": 2350,
        "keys": [
            (900, 2, "A"),
            (1200, 2, "RIGHT"),
            (1380, 2, "RIGHT"),
            (1560, 2, "RIGHT"),
            (1900, 2, "START"),
        ],
        "verify": True,
    },
    "pokedex-cry": {
        "id": scenario_id("POKEDEX"),
        "param": 6,
        "frames": 2060,
        "keys": [
            (900, 2, "A"),
            (1200, 2, "RIGHT"),
            (1380, 2, "RIGHT"),
            (1560, 2, "RIGHT"),
            (1740, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "pokedex-size": {
        "id": scenario_id("POKEDEX"),
        "param": 7,
        "frames": 2240,
        "keys": [
            (900, 2, "A"),
            (1200, 2, "RIGHT"),
            (1380, 2, "RIGHT"),
            (1560, 2, "RIGHT"),
            (1740, 2, "RIGHT"),
            (1920, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "pokedex-search": {
        "id": scenario_id("POKEDEX"),
        "param": 8,
        "frames": 1200,
        "keys": [(900, 2, "SELECT")],
        "verify": True,
    },
    "pokedex-search-results": {
        "id": scenario_id("POKEDEX"),
        "param": 9,
        "frames": 3800,
        "keys": [
            (900, 2, "SELECT"),
            (1200, 2, "A"),
            (1400, 2, "DOWN"),
            (1550, 2, "DOWN"),
            (1700, 2, "DOWN"),
            (1850, 2, "DOWN"),
            (2000, 2, "DOWN"),
            (2200, 2, "A"),
            (2900, 2, "A"),
        ],
        "verify": True,
    },
    "summary-info": {"id": scenario_id("SUMMARY"), "param": 0, "frames": 900, "keys": [], "verify": True},
    "summary-skills": {
        "id": scenario_id("SUMMARY"),
        "param": 1,
        "frames": 1050,
        "keys": [(800, 2, "RIGHT")],
        "verify": True,
    },
    "summary-moves": {
        "id": scenario_id("SUMMARY"),
        "param": 2,
        "frames": 1200,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT")],
        "verify": True,
    },
        "summary-move-detail": {
        "id": scenario_id("SUMMARY"),
        "param": 3,
        "frames": 1400,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT"), (1160, 2, "A")],
        "verify": True,
    },
    "summary-party-roundtrip": {
        "id": scenario_id("SUMMARY"),
        "param": 4,
        "frames": 1200,
        "keys": [(820, 2, "B")],
        "verify": True,
    },
    "bag": {"id": scenario_id("BAG"), "param": 2, "frames": 650, "keys": [], "verify": True},
    "bag-berries": {
        "id": scenario_id("BAG"),
        "param": 3,
        "frames": 800,
        "keys": [(700, 2, "RIGHT")],
        "verify": True,
    },
    "bag-poke-balls": {
        "id": scenario_id("BAG"),
        "param": 4,
        "frames": 980,
        "keys": [(700, 2, "RIGHT"), (880, 2, "RIGHT")],
        "verify": True,
    },
    "bag-key-items": {
        "id": scenario_id("BAG"),
        "param": 5,
        "frames": 1160,
        "keys": [(700, 2, "RIGHT"), (880, 2, "RIGHT"), (1060, 2, "RIGHT")],
        "verify": True,
    },
    "bag-mega-stones": {
        "id": scenario_id("BAG"),
        "param": 6,
        "frames": 1340,
        "keys": [
            (700, 2, "RIGHT"),
            (880, 2, "RIGHT"),
            (1060, 2, "RIGHT"),
            (1240, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "bag-items": {
        "id": scenario_id("BAG"),
        "param": 0,
        "frames": 1520,
        "keys": [
            (700, 2, "RIGHT"),
            (880, 2, "RIGHT"),
            (1060, 2, "RIGHT"),
            (1240, 2, "RIGHT"),
            (1420, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "bag-medicine": {
        "id": scenario_id("BAG"),
        "param": 1,
        "frames": 1700,
        "keys": [
            (700, 2, "RIGHT"),
            (880, 2, "RIGHT"),
            (1060, 2, "RIGHT"),
            (1240, 2, "RIGHT"),
            (1420, 2, "RIGHT"),
            (1600, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "bag-battle-items": {
        "id": scenario_id("BAG"),
        "param": 2,
        "frames": 1880,
        "keys": [
            (700, 2, "RIGHT"),
            (880, 2, "RIGHT"),
            (1060, 2, "RIGHT"),
            (1240, 2, "RIGHT"),
            (1420, 2, "RIGHT"),
            (1600, 2, "RIGHT"),
            (1780, 2, "RIGHT"),
        ],
        "verify": True,
    },
    "frontier-pass": {"id": scenario_id("FRONTIER_PASS"), "frames": 900, "keys": []},
    "frontier-pass-map": {
        "id": scenario_id("FRONTIER_PASS"),
        "frames": 1500,
        "keys": [(900, 2, "A")],
    },
    "ember-path-warden": {"id": scenario_id("EMBER_PATH_WARDEN"), "frames": 650, "keys": []},
    "double-status-ability": {
        "id": scenario_id("DOUBLE_STATUS_ABILITY"),
        "frames": 1640,
        "keys": [(500, 2, "A"), (700, 2, "A"), (900, 2, "A"), (1100, 2, "A")],
        "trigger_frame": 1600,
        "verify": True,
        "stop_on_observed": True,
    },
    "mega-ready": {
        "id": scenario_id("MEGA"),
        "param": 0,
        "frames": 2400,
        "keys": [(900, 2, "A"), (1120, 2, "A"), (1400, 2, "A")],
        "verify": True,
    },
    "mega-active": {
        "id": scenario_id("MEGA"),
        "param": 1,
        "frames": 5000,
        "keys": [
            (900, 2, "A"),
            (1120, 2, "A"),
            (1400, 2, "A"),
            (1650, 2, "START"),
            (1750, 2, "A"),
            (2100, 2, "A"),
            (2400, 2, "A"),
            (2700, 2, "A"),
        ],
        "verify": True,
    },
    "opposing-primals": {
        "id": scenario_id("PRIMALS"),
        "frames": 4200,
        "keys": [
            (500, 2, "A"),
            (700, 2, "A"),
            (900, 2, "A"),
            (1100, 2, "A"),
            (1400, 2, "A"),
            (1700, 2, "A"),
            (2000, 2, "A"),
            (2300, 2, "A"),
        ],
        "verify": True,
    },
    "safari-action": {
        "id": scenario_id("SAFARI"),
        "frames": 1800,
        "keys": [(1000, 2, "A")],
        "verify": True,
    },
    "title-live": {"id": scenario_id("TITLE"), "frames": 900, "keys": []},
    "birch-introduction": {"id": scenario_id("BIRCH"), "frames": 1500, "keys": [(600, 2, "A")]},
    "trainer-card-gold": {"id": scenario_id("TRAINER_CARD"), "frames": 1000, "keys": []},
    "slot-machine": {"id": scenario_id("SLOT_MACHINE"), "frames": 1200, "keys": []},
    "fairy-summary-info": {
        "id": scenario_id("FAIRY_SUMMARY"),
        "param": 0,
        "frames": 900,
        "keys": [],
        "verify": True,
    },
    "fairy-summary-moves": {
        "id": scenario_id("FAIRY_SUMMARY"),
        "param": 2,
        "frames": 1200,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT")],
        "verify": True,
    },
    "magma-sparkle-placement": {
        "id": scenario_id("MAGMA_SPARKLE"),
        "frames": 608,
        "keys": [],
        "trigger_frame": 600,
        "verify": True,
    },
    "furfrou-trims": {
        "id": scenario_id("FURFROU_TRIMS"),
        "param": 0,
        "frames": 700,
        "keys": [],
        "verify": True,
    },
    "furfrou-trims-scrolled": {
        "id": scenario_id("FURFROU_TRIMS"),
        "param": 1,
        "frames": 1550,
        "keys": [
            (700, 2, "DOWN"),
            (780, 2, "DOWN"),
            (860, 2, "DOWN"),
            (940, 2, "DOWN"),
            (1020, 2, "DOWN"),
            (1100, 2, "DOWN"),
            (1180, 2, "DOWN"),
            (1260, 2, "DOWN"),
            (1340, 2, "DOWN"),
            (1420, 2, "DOWN"),
        ],
        "verify": True,
    },
    "furfrou-trims-b-cancel": {
        "id": scenario_id("FURFROU_TRIMS"),
        "param": 2,
        "frames": 1000,
        "keys": [(800, 2, "B")],
        "verify": True,
    },
    "furfrou-trims-back": {
        "id": scenario_id("FURFROU_TRIMS"),
        "param": 3,
        "frames": 1750,
        "keys": [
            (700, 2, "DOWN"),
            (780, 2, "DOWN"),
            (860, 2, "DOWN"),
            (940, 2, "DOWN"),
            (1020, 2, "DOWN"),
            (1100, 2, "DOWN"),
            (1180, 2, "DOWN"),
            (1260, 2, "DOWN"),
            (1340, 2, "DOWN"),
            (1420, 2, "DOWN"),
            (1550, 2, "A"),
        ],
        "verify": True,
    },
    "hall-of-fame-record-1": {
        "id": scenario_id("HALL_OF_FAME_RECORD"), "param": 1, "frames": 1600, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "hall-of-fame-record-6": {
        "id": scenario_id("HALL_OF_FAME_RECORD"), "param": 6, "frames": 1600, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-left-open": {
        "id": scenario_id("MULTI_CORRIDOR_DOOR"), "param": 0, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-right-open": {
        "id": scenario_id("MULTI_CORRIDOR_DOOR"), "param": 1, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-left-close": {
        "id": scenario_id("MULTI_CORRIDOR_DOOR"), "param": 2, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-right-close": {
        "id": scenario_id("MULTI_CORRIDOR_DOOR"), "param": 3, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    # Field moves without HM carriers: a Zigzagoon that could learn the move
    # (but does not know it) performs it once the badge is held. UP turns to
    # face the obstacle; a steady A advances the obstacle text, confirms Yes,
    # and dismisses the used-move text; the run stops at the showcase.
    "field-move-cut-fallback": {
        "id": scenario_id("FIELD_MOVE_CUT"), "frames": 1500, "keys": [(200, 2, "UP")],
        "repeat_key": (260, 1400, 80, 2, "A"),
        "verify": True, "stop_on_observed": True,
    },
    "field-move-rock-smash-fallback": {
        "id": scenario_id("FIELD_MOVE_ROCK_SMASH"), "frames": 1500, "keys": [(200, 2, "UP")],
        "repeat_key": (260, 1400, 80, 2, "A"),
        "verify": True, "stop_on_observed": True,
    },
    "field-move-strength-fallback": {
        "id": scenario_id("FIELD_MOVE_STRENGTH"), "frames": 1500, "keys": [(200, 2, "UP")],
        "repeat_key": (260, 1400, 80, 2, "A"),
        "verify": True, "stop_on_observed": True,
    },
    # The Flight Beacon: the trigger opens the fly map, A picks the current
    # town, and the boxed Wingull that could learn Fly is shown flying.
    "flight-beacon-fly": {
        "id": scenario_id("FLIGHT_BEACON"), "frames": 1500,
        "keys": [(560, 2, "A")],
        "trigger_frame": 300, "verify": True, "stop_on_observed": True,
    },
}


SERVICE_UI_SCENARIOS = (
    "nurse-heal-facing-machine",
    "nurse-heal-tray",
    "nurse-heal-return",
    "whiteout-heal-placement",
    "whiteout-heal-league-placement",
    "whiteout-heal-lavaridge-placement",
    "party-overview",
    "party-action-menu",
    "ability-menu",
    "ability-back-to-actions",
    "ability-cancel-to-actions",
    "ability-applied-message",
    "ability-applied-return",
    "move-specialist-root",
    "move-specialist-root-back",
    "field-move-cut-fallback",
    "field-move-rock-smash-fallback",
    "field-move-strength-fallback",
    "flight-beacon-fly",
    "move-specialist-party-prompt",
    "move-specialist-party-back",
    "move-specialist-learn-move-party",
    "move-specialist-learn-move-back",
    "all-legal-moves-direct",
    "all-legal-move-selected",
    "all-legal-move-selected-back",
    "all-legal-move-confirmed",
    "all-legal-move-give-up",
    "all-legal-move-give-up-no",
    "all-legal-moves-mew-middle",
    "all-legal-moves-mew-final",
    "battle-vendor",
    "battle-vendor-category-back",
    "battle-vendor-postbadge-root",
    "battle-vendor-postbadge-held-items",
    "battle-vendor-shop",
    "battle-vendor-quantity",
    "battle-vendor-quantity-adjusted",
    "battle-vendor-quantity-back",
    "battle-vendor-confirm",
    "battle-vendor-confirm-no",
    "battle-vendor-purchase-success",
    "battle-vendor-purchase-return",
)


OVERWORLD_FIXTURE_PATTERN = re.compile(
    r"^EC_HEADLESS_OVERWORLD_FIXTURE\(\s*(\d+),\s*(MAP_[A-Z0-9_]+),\s*"
    r"(SPECIES_[A-Z0-9_]+),\s*(-?\d+),\s*(-?\d+)\)\s*$",
    re.MULTILINE,
)


def load_overworld_fixtures() -> list[dict[str, object]]:
    rows = []
    for index, map_name, species, player_x, player_y in OVERWORLD_FIXTURE_PATTERN.findall(
        OVERWORLD_FIXTURE_TABLE.read_text()
    ):
        rows.append(
            {
                "index": int(index),
                "map": map_name,
                "species": species,
                "player": [int(player_x), int(player_y)],
            }
        )
    if [row["index"] for row in rows] != list(range(1, len(rows) + 1)):
        raise RuntimeError("overworld fixture rows must be contiguous in reviewed order")
    if len(rows) != 8:
        raise RuntimeError("overworld fixture table must match Inclement's eight physical encounters")
    return rows


OVERWORLD_FIXTURES = load_overworld_fixtures()
for fixture in OVERWORLD_FIXTURES:
    species_slug = str(fixture["species"]).removeprefix("SPECIES_").lower().replace("_", "-")
    name = f"encounter-{fixture['index']:02d}-{species_slug}"
    if name in SCENARIOS:
        raise RuntimeError(f"duplicate headless scenario name: {name}")
    SCENARIOS[name] = {
        "id": GENERIC_OVERWORLD_SCENARIO_ID,
        "param": int(fixture["index"]) - 1,
        "frames": 650,
        "keys": [],
        "verify": True,
        "fixture_map": fixture["map"],
        "fixture_species": fixture["species"],
        "player": fixture["player"],
    }


def load_hoenn_heal_fixtures() -> list[dict[str, object]]:
    maps = {
        data["id"]: data
        for path in (ROOT / "data/maps").glob("*/map.json")
        for data in (json.loads(path.read_text()),)
    }
    rows = []
    for index, entry in enumerate(
        json.loads((ROOT / "src/data/heal_locations.json").read_text())["heal_locations"],
        1,
    ):
        respawn_map = maps.get(entry.get("respawn_map"))
        if (respawn_map is None or respawn_map.get("region", "REGION_HOENN") != "REGION_HOENN"):
            continue
        if entry.get("respawn_npc") in (None, "LOCALID_NONE"):
            continue
        rows.append({**entry, "heal_location_id": index})
    if len(rows) != 21:
        raise RuntimeError(f"expected 21 active Hoenn heal locations, found {len(rows)}")
    return rows


HOENN_HEAL_FIXTURES = load_hoenn_heal_fixtures()
for fixture in HOENN_HEAL_FIXTURES:
    slug = str(fixture["id"]).removeprefix("HEAL_LOCATION_").lower().replace("_", "-")
    name = f"heal-whiteout-{slug}"
    SCENARIOS[name] = {
        "id": scenario_id("HEAL_LOCATION_WHITEOUT"),
        "param": fixture["heal_location_id"],
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
        "heal_location": fixture["id"],
        "respawn_map": fixture["respawn_map"],
        "respawn_npc": fixture["respawn_npc"],
    }

INCLEMENT_SEAM_SCENARIOS = tuple(dict.fromkeys((
    "center-oldale",
    "center-lavaridge",
    *SERVICE_UI_SCENARIOS,
    "pokedex",
    "pokedex-info",
    "pokedex-area",
    "pokedex-stats",
    "pokedex-evolutions",
    "pokedex-forms",
    "pokedex-cry",
    "pokedex-size",
    "pokedex-search",
    "pokedex-search-results",
    "summary-info",
    "summary-skills",
    "summary-moves",
    "summary-move-detail",
    "summary-party-roundtrip",
    "fairy-summary-info",
    "fairy-summary-moves",
    "bag",
    "bag-medicine",
    "bag-battle-items",
    "bag-berries",
    "bag-poke-balls",
    "bag-key-items",
    "bag-mega-stones",
    "bag-items",
    "furfrou-trims",
    "furfrou-trims-scrolled",
    "furfrou-trims-b-cancel",
    "furfrou-trims-back",
    "hall-of-fame-record-1",
    "hall-of-fame-record-6",
    "multi-corridor-door-left-open",
    "multi-corridor-door-right-open",
    "multi-corridor-door-left-close",
    "multi-corridor-door-right-close",
    *(name for name in SCENARIOS if name.startswith("heal-whiteout-")),
    *(name for name in SCENARIOS if name.startswith("encounter-")),
)))


def fail(message: str) -> None:
    raise RuntimeError(message)


def _runtime_library_path() -> None:
    """The runners link Homebrew ffmpeg, which can trail an x265 upgrade.

    Offer every installed x265 so the dylib ffmpeg was built against resolves.
    Hardened Python strips DYLD_* from its own environment, so set it here for
    the child processes rather than relying on the caller's shell."""
    dirs = sorted(str(p) for p in Path("/opt/homebrew/Cellar/x265").glob("*/lib"))
    if dirs:
        current = os.environ.get("DYLD_LIBRARY_PATH", "")
        os.environ["DYLD_LIBRARY_PATH"] = ":".join([d for d in dirs if d not in current] + ([current] if current else []))


_runtime_library_path()


def run(command: list[str], *, timeout: int = 120) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        timeout=timeout,
        check=False,
    )
    if result.returncode != 0:
        fail(f"command failed ({result.returncode}): {' '.join(command)}\n{result.stdout[-6000:]}")
    return result


def require_resident_file(path: Path, label: str) -> Path:
    path = path.expanduser().resolve()
    if not path.is_file():
        fail(f"{label} is missing: {path}")
    stat_result = path.stat()
    if stat_result.st_size and stat_result.st_blocks == 0:
        fail(f"{label} is dataless/offloaded and must be downloaded first: {path}")
    return path


def build_runner() -> Path:
    try:
        return native_tools.build_runner(RUNNER_SOURCE, ROOT / "build/headless/emerald_champions_mgba_runner")
    except native_tools.NativeToolError as error:
        fail(str(error))


def resolve_symbol(elf: Path, name: str) -> int:
    try:
        found = native_tools.symbols(elf, ROOT, first=True)
    except native_tools.NativeToolError as error:
        fail(str(error))
    if name not in found:
        fail(f"ELF symbol is missing: {name}")
    return found[name]


def validate_screenshot_png(path: Path, *, allow_uniform: bool = False) -> str:
    data = path.read_bytes()
    if not data.startswith(b"\x89PNG\r\n\x1a\n"):
        fail(f"screenshot output is not PNG: {path}")

    offset = 8
    ihdr: tuple[int, int, int, int, int, int, int] | None = None
    idat = bytearray()
    saw_iend = False
    while offset + 12 <= len(data):
        length = struct.unpack_from(">I", data, offset)[0]
        chunk_type = data[offset + 4 : offset + 8]
        payload_start = offset + 8
        payload_end = payload_start + length
        crc_end = payload_end + 4
        if crc_end > len(data):
            fail(f"truncated PNG chunk in {path}")
        payload = data[payload_start:payload_end]
        stored_crc = struct.unpack_from(">I", data, payload_end)[0]
        actual_crc = zlib.crc32(chunk_type + payload) & 0xFFFFFFFF
        if stored_crc != actual_crc:
            fail(f"PNG chunk CRC mismatch in {path}")
        if chunk_type == b"IHDR":
            if length != 13 or ihdr is not None:
                fail(f"invalid PNG IHDR in {path}")
            ihdr = struct.unpack(">IIBBBBB", payload)
        elif chunk_type == b"IDAT":
            idat.extend(payload)
        elif chunk_type == b"IEND":
            if length != 0:
                fail(f"invalid PNG IEND in {path}")
            saw_iend = True
            offset = crc_end
            break
        offset = crc_end

    if ihdr is None or ihdr[:2] != (240, 160):
        fail(f"screenshot must be exactly 240x160: {path}")
    width, height, bit_depth, color_type, compression, filter_method, interlace = ihdr
    if (
        bit_depth != 8
        or color_type not in (2, 6)
        or compression != 0
        or filter_method != 0
        or interlace != 0
        or not idat
        or not saw_iend
        or offset != len(data)
    ):
        fail(f"unsupported or incomplete screenshot PNG structure: {path}")

    bytes_per_pixel = 3 if color_type == 2 else 4
    stride = width * bytes_per_pixel
    try:
        filtered = zlib.decompress(bytes(idat))
    except zlib.error as error:
        fail(f"invalid compressed screenshot pixels in {path}: {error}")
    if len(filtered) != height * (stride + 1):
        fail(f"unexpected screenshot pixel payload length in {path}")

    rows: list[bytearray] = []
    cursor = 0
    for _ in range(height):
        filter_type = filtered[cursor]
        cursor += 1
        row = bytearray(filtered[cursor : cursor + stride])
        cursor += stride
        previous = rows[-1] if rows else bytearray(stride)
        for x in range(stride):
            left = row[x - bytes_per_pixel] if x >= bytes_per_pixel else 0
            up = previous[x]
            up_left = previous[x - bytes_per_pixel] if x >= bytes_per_pixel else 0
            if filter_type == 1:
                row[x] = (row[x] + left) & 0xFF
            elif filter_type == 2:
                row[x] = (row[x] + up) & 0xFF
            elif filter_type == 3:
                row[x] = (row[x] + ((left + up) >> 1)) & 0xFF
            elif filter_type == 4:
                predictor = left + up - up_left
                pa = abs(predictor - left)
                pb = abs(predictor - up)
                pc = abs(predictor - up_left)
                nearest = left if pa <= pb and pa <= pc else up if pb <= pc else up_left
                row[x] = (row[x] + nearest) & 0xFF
            elif filter_type != 0:
                fail(f"unsupported PNG row filter in {path}")
        rows.append(row)

    first_pixel = bytes(rows[0][:bytes_per_pixel])
    # Captured transition frames may be a legitimate full-screen fade. Final
    # scenario/checkpoint callers retain the nonblank requirement by default.
    if not allow_uniform and all(
        bytes(row[x : x + bytes_per_pixel]) == first_pixel
        for row in rows
        for x in range(0, stride, bytes_per_pixel)
    ):
        fail(f"scenario produced a uniform blank screenshot: {path}")

    # Hash normalized decoded RGB pixels, not PNG container bytes.  This is
    # stable across valid encoder/filter/compression differences in CI.
    pixels = bytearray()
    for row in rows:
        if bytes_per_pixel == 3:
            pixels.extend(row)
        else:
            for x in range(0, stride, bytes_per_pixel):
                pixels.extend(row[x : x + 3])
    return hashlib.sha256(pixels).hexdigest()


def render_one(
    name: str,
    spec: dict[str, object],
    *,
    runner: Path,
    rom: Path,
    scenario_address: int,
    param_address: int,
    trigger_address: int,
    setup_address: int,
    observed_address: int,
    out: Path,
) -> dict[str, object]:
    screenshot = out / f"{name}.png"
    keys = list(spec["keys"])
    if "repeat_key" in spec:
        start, stop, step, duration, repeated = spec["repeat_key"]
        keys.extend((frame, duration, repeated) for frame in range(start, stop, step))
    with tempfile.TemporaryDirectory(prefix="emerald-champions-render-") as scratch_dir:
        scratch = Path(scratch_dir)
        scratch_rom = scratch / "Emerald Champions Headless.gba"
        shutil.copy2(rom, scratch_rom)
        command = [
            str(runner),
            "--rom",
            str(scratch_rom),
            "--frames",
            str(spec["frames"]),
            "--rtc",
            "946684800",
        ]
        if "param" in spec:
            command.extend(("--write", f"59:4:0x{param_address:x}:{spec['param']}"))
        if "trigger_frame" in spec:
            command.extend(("--write", f"{spec['trigger_frame']}:4:0x{trigger_address:x}:1"))
        command.extend(
            (
                "--write",
                f"60:4:0x{scenario_address:x}:{spec['id']}",
                "--screenshot",
                str(screenshot),
            )
        )
        for frame, duration, pressed in keys:
            command.extend(("--key", f"{frame}:{duration}:{pressed}"))
        if spec.get("verify"):
            command.extend(("--read", f"4:0x{setup_address:x}"))
            command.extend(("--read", f"4:0x{observed_address:x}"))
        if spec.get("stop_on_observed"):
            command.extend(("--until", f"4:0x{observed_address:x}:0xffffffff:1"))
        result = run(command)

    if not screenshot.is_file():
        fail(f"scenario {name} did not produce a screenshot")
    pixel_sha256 = validate_screenshot_png(screenshot)
    match = RESULT_PATTERN.search(result.stdout)
    if match is None:
        fail(f"scenario {name} produced no result line: {result.stdout}")
    reads = {int(address, 16): int(value, 16) for address, value in READ_PATTERN.findall(result.stdout)}
    if spec.get("verify"):
        if reads.get(setup_address) != 1 or reads.get(observed_address) != 1:
            fail(
                f"scenario {name} did not reach its native UI contract: "
                f"setup={reads.get(setup_address)} observed={reads.get(observed_address)}"
            )
    rendered = {
        "name": name,
        "scenario_id": spec["id"],
        "frames": spec["frames"],
        "keys": keys,
        "video_hash": match.group("video"),
        "png_sha256": hashlib.sha256(screenshot.read_bytes()).hexdigest(),
        "pixel_sha256": pixel_sha256,
        "screenshot": str(screenshot),
        "verified_runtime_state": bool(spec.get("verify")),
    }
    for field in (
        "fixture_map", "fixture_species", "player", "param",
        "heal_location", "respawn_map", "respawn_npc",
    ):
        if field in spec:
            rendered[field] = spec[field]
    return rendered


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "scenario",
        choices=["all", "service-ui", "inclement-seams", "overworld-encounters", *SCENARIOS],
    )
    parser.add_argument("--rom", type=Path, default=DEFAULT_ROM)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()

    rom = require_resident_file(args.rom, "headless fixture ROM")
    elf = require_resident_file(args.elf, "headless fixture ELF")
    runner = build_runner()
    scenario_address = resolve_symbol(elf, SCENARIO_SYMBOL)
    param_address = resolve_symbol(elf, "gEcHeadlessFixtureParam")
    trigger_address = resolve_symbol(elf, "gEcHeadlessFixtureTrigger")
    setup_address = resolve_symbol(elf, "gEcHeadlessFixtureSetupResult")
    observed_address = resolve_symbol(elf, "gEcHeadlessFixtureObservedResult")
    args.out.mkdir(parents=True, exist_ok=True)
    if args.scenario == "all":
        names = list(SCENARIOS)
    elif args.scenario == "service-ui":
        names = list(SERVICE_UI_SCENARIOS)
    elif args.scenario == "inclement-seams":
        names = list(INCLEMENT_SEAM_SCENARIOS)
    elif args.scenario == "overworld-encounters":
        names = [name for name in SCENARIOS if name.startswith("encounter-")]
    else:
        names = [args.scenario]
    rendered = [
        render_one(
            name,
            SCENARIOS[name],
            runner=runner,
            rom=rom,
            scenario_address=scenario_address,
            param_address=param_address,
            trigger_address=trigger_address,
            setup_address=setup_address,
            observed_address=observed_address,
            out=args.out,
        )
        for name in names
    ]
    manifest = {
        "schema_version": 1,
        "rom": str(rom),
        "rom_sha256": hashlib.sha256(rom.read_bytes()).hexdigest(),
        "elf": str(elf),
        "scenario_symbol": f"0x{scenario_address:08x}",
        "param_symbol": f"0x{param_address:08x}",
        "rendered": rendered,
    }
    # Keep the complete evidence manifest authoritative. A focused iteration
    # writes beside it instead of replacing it with a one-row partial result.
    manifest_name = "manifest.json" if args.scenario == "all" else f"manifest.{args.scenario}.json"
    manifest_path = args.out / manifest_name
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
    print(json.dumps(manifest, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(f"headless render: FAIL: {error}", file=os.sys.stderr)
        raise SystemExit(1)
