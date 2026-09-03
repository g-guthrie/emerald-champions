#!/usr/bin/env python3
"""Render deterministic Emerald Champions UI/world screenshots with libmGBA."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import struct
import subprocess
import tempfile
import zlib


ROOT = Path(__file__).resolve().parents[1]
RUNNER_SOURCE = ROOT / "tests/headless/emerald_champions_mgba_runner.c"
DEFAULT_ROM = ROOT / "pokeemerald-headless.gba"
DEFAULT_ELF = ROOT / "pokeemerald-headless.elf"
DEFAULT_OUT = ROOT / "work/visual-audit/rendered/current"
OVERWORLD_FIXTURE_TABLE = ROOT / "include/emerald_champions_headless_overworld_fixtures.h"
SCENARIO_SYMBOL = "gEcHeadlessFixtureScenario"
GENERIC_OVERWORLD_SCENARIO_ID = 28
RESULT_PATTERN = re.compile(r"^RESULT .*video_hash=(?P<video>[0-9a-f]{16}) ", re.MULTILINE)
READ_PATTERN = re.compile(
    r"^READ width=4 address=(?P<address>[0-9a-f]{8}) value=(?P<value>[0-9a-f]{8})$",
    re.MULTILINE,
)


SCENARIOS: dict[str, dict[str, object]] = {
    "center-oldale": {"id": 1, "frames": 600, "keys": []},
    "nurse-heal-facing-machine": {
        "id": 1,
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
        "id": 1,
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
        "id": 1,
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
        "id": 1,
        "param": 2,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "whiteout-heal-league-placement": {
        "id": 1,
        "param": 4,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "whiteout-heal-lavaridge-placement": {
        "id": 1,
        "param": 6,
        "frames": 1280,
        "keys": [(900, 2, "A"), (1160, 2, "A")],
        "verify": True,
    },
    "trainer-hill-nurse-heal-placement": {
        "id": 1,
        "param": 5,
        "frames": 1750,
        "keys": [
            (200, 2, "UP"),
            (250, 2, "A"),
            (1400, 2, "A"),
            (1480, 2, "A"),
            (1560, 2, "A"),
        ],
        "verify": True,
    },
    "center-lavaridge": {"id": 2, "frames": 600, "keys": []},
    "ability-menu": {
        "id": 3,
        "frames": 520,
        "keys": [(210, 2, "A"), (250, 2, "DOWN"), (290, 2, "A")],
    },
    "ability-back-to-actions": {
        "id": 3,
        "frames": 680,
        "keys": [(210, 2, "A"), (250, 2, "DOWN"), (290, 2, "A"), (560, 2, "B")],
    },
    "ability-cancel-to-actions": {
        "id": 3,
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
        "id": 3,
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
        "id": 3,
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
    "party-overview": {"id": 3, "frames": 300, "keys": []},
    "party-action-menu": {"id": 3, "frames": 245, "keys": [(210, 2, "A")]},
    "options": {"id": 4, "frames": 520, "keys": []},
    "battle-vendor": {
        "id": 5,
        "frames": 760,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A"), (460, 2, "A")],
    },
    "battle-vendor-category-back": {
        "id": 5,
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
        "id": 5,
        "param": 1,
        "frames": 620,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A")],
    },
    "battle-vendor-postbadge-held-items": {
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 5,
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
        "id": 6,
        "frames": 350,
        "keys": [(220, 2, "UP"), (250, 2, "A")],
    },
    "move-specialist-root-back": {
        "id": 6,
        "frames": 520,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "B")],
    },
    "move-specialist-party-prompt": {
        "id": 6,
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "A")],
    },
    "move-specialist-battle-set-party": {
        "id": 6,
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "A"), (520, 2, "A")],
    },
    "move-specialist-party-back": {
        "id": 6,
        "frames": 880,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (740, 2, "B"),
        ],
    },
    "battle-set-list": {
        "id": 6,
        "frames": 1000,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (850, 2, "A"),
        ],
    },
    "battle-set-format": {
        "id": 6,
        "frames": 800,
        "keys": [
            (220, 2, "UP"), (250, 2, "A"), (380, 2, "A"),
            (520, 2, "A"), (650, 2, "A"),
        ],
    },
    "battle-set-singles-list": {
        "id": 6,
        "frames": 1000,
        "keys": [
            (220, 2, "UP"), (250, 2, "A"), (380, 2, "A"),
            (520, 2, "A"), (650, 2, "A"),
            (800, 2, "DOWN"), (850, 2, "A"),
        ],
    },
    "battle-set-list-back": {
        "id": 6,
        "frames": 1200,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (850, 2, "A"),
            (1050, 2, "B"),
        ],
    },
    "battle-set-confirm": {
        "id": 6,
        "frames": 1200,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (850, 2, "A"),
            (1050, 2, "A"),
        ],
    },
    "battle-set-confirm-no": {
        "id": 6,
        "frames": 1450,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (850, 2, "A"),
            (1050, 2, "A"),
            (1200, 2, "B"),
        ],
    },
    "battle-set-applied": {
        "id": 6,
        "frames": 1450,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (850, 2, "A"),
            (1050, 2, "A"),
            (1200, 2, "A"),
        ],
    },
    "nature-list": {
        "id": 6, "frames": 940,
        "keys": [(220, 100, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "A"), (560, 2, "A"), (760, 2, "A")],
    },
    "stat-point-party": {
        "id": 6,
        "frames": 800,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A")],
    },
    "stat-point-egg-rejected": {
        "id": 6,
        "param": 1,
        "frames": 980,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (740, 2, "DOWN"), (800, 2, "A")],
    },
    "stat-point-external-entry": {
        "id": 6,
        "param": 2,
        "frames": 520,
        "keys": [(220, 2, "UP"), (250, 2, "A")],
    },
    "stat-point-external-exit": {
        "id": 6,
        "param": 2,
        "frames": 900,
        "keys": [
            (220, 2, "UP"), (250, 2, "A"),
            (460, 2, "A"), (700, 2, "B"),
        ],
    },
    "stat-point-list": {
        "id": 6,
        "frames": 980,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A")],
    },
    "stat-point-list-scrolled": {
        "id": 6,
        "frames": 1360,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "DOWN"), (1080, 2, "DOWN"), (1120, 2, "DOWN"), (1160, 2, "DOWN"), (1200, 2, "DOWN"), (1240, 2, "DOWN")],
    },
    "stat-point-adjust-list": {
        "id": 6,
        "frames": 1260,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1140, 2, "A")],
    },
    "stat-point-adjust-scrolled": {
        "id": 6,
        "frames": 1680,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1140, 2, "A"), (1280, 2, "DOWN"), (1320, 2, "DOWN"), (1360, 2, "DOWN"), (1400, 2, "DOWN"), (1440, 2, "DOWN"), (1480, 2, "DOWN"), (1520, 2, "DOWN"), (1560, 2, "DOWN")],
    },
    "stat-point-adjusted": {
        "id": 6,
        "frames": 1500,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1140, 2, "A"), (1280, 2, "DOWN"), (1320, 2, "DOWN"), (1360, 2, "A")],
    },
    "stat-point-boundary-feedback": {
        "id": 6,
        "frames": 1540,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1140, 2, "A"), (1280, 2, "DOWN"), (1320, 2, "DOWN"), (1360, 2, "DOWN"), (1400, 2, "A")],
    },
    "stat-point-adjust-back": {
        "id": 6,
        "frames": 1460,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1140, 2, "A"), (1280, 2, "B")],
    },
    "stat-point-list-back": {
        "id": 6,
        "frames": 1400,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "B"), (1240, 2, "A")],
    },
    "stat-point-reset-confirm": {
        "id": 6,
        "frames": 1420,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "DOWN"), (1080, 2, "DOWN"), (1120, 2, "DOWN"), (1160, 2, "DOWN"), (1200, 2, "DOWN"), (1240, 2, "DOWN"), (1300, 2, "A")],
    },
    "stat-point-reset-no": {
        "id": 6,
        "frames": 1640,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "DOWN"), (1080, 2, "DOWN"), (1120, 2, "DOWN"), (1160, 2, "DOWN"), (1200, 2, "DOWN"), (1240, 2, "DOWN"), (1300, 2, "A"), (1460, 2, "B")],
    },
    "stat-point-reset-yes": {
        "id": 6,
        "frames": 1640,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "DOWN"), (1080, 2, "DOWN"), (1120, 2, "DOWN"), (1160, 2, "DOWN"), (1200, 2, "DOWN"), (1240, 2, "DOWN"), (1300, 2, "A"), (1460, 2, "A")],
    },
    "stat-point-reset-zero-list": {
        "id": 6,
        "frames": 1940,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "DOWN"), (400, 2, "DOWN"), (440, 2, "A"), (600, 2, "A"), (800, 2, "A"), (1040, 2, "DOWN"), (1080, 2, "DOWN"), (1120, 2, "DOWN"), (1160, 2, "DOWN"), (1200, 2, "DOWN"), (1240, 2, "DOWN"), (1300, 2, "A"), (1460, 2, "A"), (1680, 2, "A")],
    },
    "move-specialist-learn-move-party": {
        "id": 6,
        "frames": 940,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "A"), (640, 2, "A")],
    },
    "move-specialist-learn-move-back": {
        "id": 6,
        "frames": 1120,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "A"), (640, 2, "A"), (980, 2, "B")],
    },
    "move-specialist-forget-intro": {
        "id": 6,
        "frames": 840,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "A")],
    },
    "move-specialist-forget-decline": {
        "id": 6,
        "frames": 1340,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "A"), (840, 2, "A"), (1040, 2, "A"), (1240, 2, "B")],
    },
    "move-specialist-forget-party": {
        "id": 6,
        "frames": 1580,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "A"), (840, 2, "A"), (1040, 2, "A"), (1240, 2, "A"), (1440, 2, "A")],
    },
    "move-specialist-forget-party-back": {
        "id": 6,
        "frames": 1820,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "A"), (840, 2, "A"), (1040, 2, "A"), (1240, 2, "A"), (1440, 2, "A"), (1640, 2, "B")],
    },
    "move-specialist-rename-prompt": {
        "id": 6,
        "frames": 860,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "DOWN"), (740, 2, "A")],
    },
    "move-specialist-rename-party": {
        "id": 6,
        "frames": 1090,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "DOWN"), (740, 2, "A"), (940, 2, "A")],
    },
    "move-specialist-rename-back": {
        "id": 6,
        "frames": 1420,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (380, 2, "DOWN"), (420, 2, "DOWN"), (460, 2, "DOWN"), (500, 2, "DOWN"), (540, 2, "A"), (690, 2, "DOWN"), (740, 2, "A"), (940, 2, "A"), (1240, 2, "B")],
    },
    "all-legal-moves": {
        "id": 6,
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
        "id": 10,
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A"), (360, 2, "A"), (480, 2, "A")],
    },
    "game-corner-regions": {
        "id": 10,
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
        "id": 10,
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
    "circuit-lobby": {"id": 11, "frames": 600, "keys": []},
    "circuit-welcome": {
        "id": 11,
        "frames": 700,
        "keys": [(220, 2, "UP"), (250, 2, "A")],
    },
    "leveler-complete": {"id": 12, "frames": 700, "keys": []},
    "all-legal-moves-direct": {"id": 13, "frames": 650, "keys": []},
    "all-legal-move-selected": {
        "id": 13,
        "frames": 1050,
        "keys": [(700, 2, "A")],
    },
    "all-legal-move-selected-back": {
        "id": 13,
        "frames": 1250,
        "keys": [(700, 2, "A"), (1080, 2, "B")],
    },
    "all-legal-move-confirmed": {
        "id": 13,
        "frames": 1500,
        "keys": [(700, 2, "A"), (1080, 2, "A")],
    },
    "all-legal-move-give-up": {
        "id": 13,
        "frames": 1000,
        "keys": [(700, 2, "B")],
    },
    "all-legal-move-give-up-no": {
        "id": 13,
        "frames": 1220,
        "keys": [(700, 2, "B"), (1040, 2, "B")],
    },
    "all-legal-moves-mew": {"id": 14, "frames": 650, "keys": []},
    "all-legal-moves-mew-middle": {
        "id": 14,
        "frames": 1500,
        "keys": [(400, 1000, "DOWN")],
    },
    "all-legal-moves-mew-final": {
        "id": 14,
        "frames": 3500,
        "keys": [(400, 3000, "DOWN")],
    },
    "all-legal-hm-replacement": {
        "id": 53,
        "frames": 1000,
        "keys": [(400, 2, "A")],
        "verify": True,
        "stop_on_observed": True,
    },
    "dewford-gym-entry": {
        "id": 54,
        "frames": 1000,
        "keys": [(300, 120, "UP")],
        "verify": True,
    },
    "start-menu-full": {"id": 55, "frames": 700, "keys": [(400, 2, "START")]},
    "start-menu-scroll-bottom": {
        "id": 55,
        "frames": 900,
        "keys": [(400, 2, "START"), (480, 2, "DOWN"), (492, 2, "DOWN"), (504, 2, "DOWN"), (516, 2, "DOWN"), (528, 2, "DOWN"), (540, 2, "DOWN"), (552, 2, "DOWN"), (564, 2, "DOWN")],
    },
    "start-menu-wrap-up": {"id": 55, "frames": 800, "keys": [(400, 2, "START"), (480, 2, "UP")]},
    "start-menu-wrap-down": {
        "id": 55,
        "frames": 900,
        "keys": [(400, 2, "START"), (480, 2, "DOWN"), (492, 2, "DOWN"), (504, 2, "DOWN"), (516, 2, "DOWN"), (528, 2, "DOWN"), (540, 2, "DOWN"), (552, 2, "DOWN"), (564, 2, "DOWN"), (576, 2, "DOWN")],
    },
    "wild-action-menu": {
        "id": 15,
        "frames": 1500,
        "keys": [(900, 2, "A"), (1100, 2, "A")],
        "verify": True,
    },
    "wild-foe-types": {
        "id": 51,
        "frames": 1500,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1300, 2, "R")],
        "verify": True,
    },
    "move-details": {
        "id": 16,
        "frames": 1750,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1400, 2, "A"), (1550, 2, "L")],
        "verify": True,
    },
    "move-foe-types": {
        "id": 52,
        "frames": 1750,
        "keys": [(900, 2, "A"), (1100, 2, "A"), (1400, 2, "A"), (1550, 2, "R")],
        "verify": True,
    },
    "battle-set-current": {
        "id": 17,
        "frames": 1200,
        "keys": [
            (220, 2, "UP"),
            (250, 2, "A"),
            (380, 2, "A"),
            (520, 2, "A"),
            (650, 2, "A"),
            (760, 2, "A"),
            (900, 2, "A"),
            (1020, 2, "A"),
        ],
    },
    "naming": {"id": 18, "frames": 600, "keys": []},
    "storage-root": {"id": 19, "frames": 700, "keys": []},
    "storage-boxes": {"id": 19, "frames": 1400, "keys": [(760, 2, "A")]},
    "storage-box-popup": {
        "id": 19,
        "frames": 1900,
        "keys": [(760, 2, "A"), (1250, 2, "START"), (1370, 2, "A"), (1510, 2, "A")],
    },
    "storage-move-items": {
        "id": 19,
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
        "id": 20,
        "frames": 1100,
        "keys": [(700, 2, "A"), (840, 2, "A")],
    },
    "circuit-room": {"id": 21, "frames": 900, "keys": []},
    "pokedex": {"id": 23, "param": 0, "frames": 900, "keys": [], "verify": True},
    "pokedex-info": {
        "id": 23,
        "param": 1,
        "frames": 1200,
        "keys": [(900, 2, "A")],
        "verify": True,
    },
    "pokedex-area": {
        "id": 23,
        "param": 2,
        "frames": 1400,
        "keys": [(900, 2, "A"), (1200, 2, "RIGHT")],
        "verify": True,
    },
    "pokedex-area-ashen-woods": {"id": 23, "param": 20, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-dewford-meadow": {"id": 23, "param": 21, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-verdanturf-meadow": {"id": 23, "param": 22, "frames": 1400, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-late": {"id": 23, "param": 20, "frames": 1460, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-verdanturf-meadow-late": {"id": 23, "param": 22, "frames": 1460, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-b": {"id": 23, "param": 20, "frames": 1413, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-area-ashen-woods-c": {"id": 23, "param": 20, "frames": 1426, "keys": [(900, 2, "A"), (1200, 2, "RIGHT")]},
    "pokedex-stats": {
        "id": 23,
        "param": 3,
        "frames": 1580,
        "keys": [(900, 2, "A"), (1200, 2, "RIGHT"), (1380, 2, "RIGHT")],
        "verify": True,
    },
    "pokedex-evolutions": {
        "id": 23,
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
        "id": 23,
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
        "id": 23,
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
        "id": 23,
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
        "id": 23,
        "param": 8,
        "frames": 1200,
        "keys": [(900, 2, "SELECT")],
        "verify": True,
    },
    "pokedex-search-results": {
        "id": 23,
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
    "summary-info": {"id": 24, "param": 0, "frames": 900, "keys": [], "verify": True},
    "summary-skills": {
        "id": 24,
        "param": 1,
        "frames": 1050,
        "keys": [(800, 2, "RIGHT")],
        "verify": True,
    },
    "summary-moves": {
        "id": 24,
        "param": 2,
        "frames": 1200,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT")],
        "verify": True,
    },
    "summary-contest-moves": {
        "id": 24,
        "param": 3,
        "frames": 1500,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT"), (1160, 2, "RIGHT")],
        "verify": True,
    },
    "summary-move-detail": {
        "id": 24,
        "param": 4,
        "frames": 1400,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT"), (1160, 2, "A")],
        "verify": True,
    },
    "summary-party-roundtrip": {
        "id": 24,
        "param": 5,
        "frames": 1200,
        "keys": [(820, 2, "B")],
        "verify": True,
    },
    "bag": {"id": 25, "param": 2, "frames": 650, "keys": [], "verify": True},
    "bag-tms-hms": {
        "id": 25,
        "param": 3,
        "frames": 800,
        "keys": [(700, 2, "RIGHT")],
        "verify": True,
    },
    "bag-berries": {
        "id": 25,
        "param": 4,
        "frames": 980,
        "keys": [(700, 2, "RIGHT"), (880, 2, "RIGHT")],
        "verify": True,
    },
    "bag-poke-balls": {
        "id": 25,
        "param": 5,
        "frames": 1160,
        "keys": [(700, 2, "RIGHT"), (880, 2, "RIGHT"), (1060, 2, "RIGHT")],
        "verify": True,
    },
    "bag-key-items": {
        "id": 25,
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
    "bag-mega-stones": {
        "id": 25,
        "param": 7,
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
    "bag-items": {
        "id": 25,
        "param": 0,
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
    "bag-medicine": {
        "id": 25,
        "param": 1,
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
    "frontier-pass": {"id": 26, "frames": 900, "keys": []},
    "frontier-pass-map": {
        "id": 26,
        "frames": 1500,
        "keys": [(900, 2, "A")],
    },
    "ember-path-warden": {"id": 27, "frames": 650, "keys": []},
    "double-status-ability": {
        "id": 29,
        "frames": 1640,
        "keys": [(500, 2, "A"), (700, 2, "A"), (900, 2, "A"), (1100, 2, "A")],
        "trigger_frame": 1600,
        "verify": True,
        "stop_on_observed": True,
    },
    "mega-ready": {
        "id": 30,
        "param": 0,
        "frames": 2400,
        "keys": [(900, 2, "A"), (1120, 2, "A"), (1400, 2, "A")],
        "verify": True,
    },
    "mega-active": {
        "id": 30,
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
        "id": 31,
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
        "id": 32,
        "frames": 1800,
        "keys": [(1000, 2, "A")],
        "verify": True,
    },
    "title-live": {"id": 33, "frames": 900, "keys": []},
    "birch-introduction": {"id": 34, "frames": 1500, "keys": [(600, 2, "A")]},
    "pokeblock-condition": {"id": 35, "frames": 1000, "keys": []},
    "trainer-card-gold": {"id": 36, "frames": 1000, "keys": []},
    "battle-dome-info-card": {"id": 37, "frames": 1500, "keys": []},
    "contest-results": {"id": 38, "frames": 1800, "keys": []},
    "slot-machine": {"id": 39, "frames": 1200, "keys": []},
    "fairy-summary-info": {
        "id": 40,
        "param": 0,
        "frames": 900,
        "keys": [],
        "verify": True,
    },
    "fairy-summary-moves": {
        "id": 40,
        "param": 2,
        "frames": 1200,
        "keys": [(800, 2, "RIGHT"), (980, 2, "RIGHT")],
        "verify": True,
    },
    "magma-sparkle-placement": {
        "id": 42,
        "frames": 608,
        "keys": [],
        "trigger_frame": 600,
        "verify": True,
    },
    "furfrou-trims": {
        "id": 43,
        "param": 0,
        "frames": 700,
        "keys": [],
        "verify": True,
    },
    "furfrou-trims-scrolled": {
        "id": 43,
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
        "id": 43,
        "param": 2,
        "frames": 1000,
        "keys": [(800, 2, "B")],
        "verify": True,
    },
    "furfrou-trims-back": {
        "id": 43,
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
        "id": 45, "param": 1, "frames": 1600, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "hall-of-fame-record-6": {
        "id": 45, "param": 6, "frames": 1600, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-left-open": {
        "id": 46, "param": 0, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-right-open": {
        "id": 46, "param": 1, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-left-close": {
        "id": 46, "param": 2, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    "multi-corridor-door-right-close": {
        "id": 46, "param": 3, "frames": 1000, "keys": [],
        "trigger_frame": 600, "verify": True, "stop_on_observed": True,
    },
    # Field moves without HM carriers: a Zigzagoon that could learn the move
    # (but does not know it) performs it once the badge is held. UP turns to
    # face the obstacle, A talks to it, A confirms; the observer latches the
    # "used <move>!" showcase.
    "field-move-cut-fallback": {
        "id": 47, "frames": 900,
        "keys": [(200, 2, "UP"), (260, 2, "A"), (420, 2, "A")],
        "verify": True, "stop_on_observed": True,
    },
    "field-move-rock-smash-fallback": {
        "id": 48, "frames": 900,
        "keys": [(200, 2, "UP"), (260, 2, "A"), (420, 2, "A")],
        "verify": True, "stop_on_observed": True,
    },
    "field-move-strength-fallback": {
        "id": 49, "frames": 900,
        "keys": [(200, 2, "UP"), (260, 2, "A"), (420, 2, "A")],
        "verify": True, "stop_on_observed": True,
    },
    # The Flight Beacon: the trigger opens the fly map, A picks the current
    # town, and the boxed Wingull that knows Fly is the one shown flying.
    "flight-beacon-fly": {
        "id": 50, "frames": 1500,
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
    "trainer-hill-nurse-heal-placement",
    "party-overview",
    "party-action-menu",
    "ability-menu",
    "ability-back-to-actions",
    "ability-cancel-to-actions",
    "ability-applied-message",
    "ability-applied-return",
    "move-specialist-root",
    "move-specialist-root-back",
    "nature-list",
    "field-move-cut-fallback",
    "field-move-rock-smash-fallback",
    "field-move-strength-fallback",
    "flight-beacon-fly",
    "move-specialist-party-prompt",
    "move-specialist-battle-set-party",
    "move-specialist-party-back",
    "battle-set-format",
    "battle-set-list",
    "battle-set-singles-list",
    "battle-set-list-back",
    "battle-set-confirm",
    "battle-set-confirm-no",
    "battle-set-applied",
    "stat-point-party",
    "stat-point-egg-rejected",
    "stat-point-external-entry",
    "stat-point-external-exit",
    "stat-point-list",
    "stat-point-list-scrolled",
    "stat-point-adjust-list",
    "stat-point-adjust-scrolled",
    "stat-point-adjusted",
    "stat-point-boundary-feedback",
    "stat-point-adjust-back",
    "stat-point-list-back",
    "stat-point-reset-confirm",
    "stat-point-reset-no",
    "stat-point-reset-yes",
    "stat-point-reset-zero-list",
    "move-specialist-learn-move-party",
    "move-specialist-learn-move-back",
    "move-specialist-forget-intro",
    "move-specialist-forget-decline",
    "move-specialist-forget-party",
    "move-specialist-forget-party-back",
    "move-specialist-rename-prompt",
    "move-specialist-rename-party",
    "move-specialist-rename-back",
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
        if (respawn_map is None or respawn_map.get("region") != "REGION_HOENN"):
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
        "id": 44,
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
    "summary-contest-moves",
    "summary-move-detail",
    "summary-party-roundtrip",
    "fairy-summary-info",
    "fairy-summary-moves",
    "bag",
    "bag-items",
    "bag-medicine",
    "bag-tms-hms",
    "bag-berries",
    "bag-poke-balls",
    "bag-key-items",
    "bag-mega-stones",
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


def find_mgba_prefix() -> Path:
    candidates: list[Path] = []
    if os.environ.get("MGBA_PREFIX"):
        candidates.append(Path(os.environ["MGBA_PREFIX"]))
    brew = shutil.which("brew")
    if brew:
        result = subprocess.run(
            [brew, "--prefix", "mgba"], text=True, capture_output=True, check=False
        )
        if result.returncode == 0 and result.stdout.strip():
            candidates.append(Path(result.stdout.strip()))
    candidates.extend((Path("/opt/homebrew/opt/mgba"), Path("/usr/local/opt/mgba")))
    # Ubuntu's libmgba-dev installs headers under /usr/include and the shared
    # library in a multiarch directory such as /usr/lib/x86_64-linux-gnu.
    candidates.append(Path("/usr"))
    for candidate in candidates:
        lib_dir = candidate / "lib"
        libraries = list(lib_dir.glob("libmgba*")) + list(lib_dir.glob("*/libmgba*"))
        if (candidate / "include/mgba/core/core.h").is_file() and libraries:
            return candidate.resolve()
    fail("native libmGBA headers/library are unavailable; set MGBA_PREFIX")


def build_runner() -> Path:
    output = ROOT / "build/headless/emerald_champions_mgba_runner"
    output.parent.mkdir(parents=True, exist_ok=True)
    newest_input = max(RUNNER_SOURCE.stat().st_mtime_ns, Path(__file__).stat().st_mtime_ns)
    if output.is_file() and output.stat().st_mtime_ns >= newest_input:
        return output
    command = [
        os.environ.get("CC", "cc"),
        "-std=c11",
        "-O2",
        "-Wall",
        "-Wextra",
        "-Werror",
        str(RUNNER_SOURCE),
    ]
    pkg_config = shutil.which("pkg-config")
    pkg_flags: list[str] = []
    if pkg_config is not None:
        result = subprocess.run(
            [pkg_config, "--cflags", "--libs", "mgba"],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        if result.returncode == 0:
            pkg_flags = shlex.split(result.stdout)
    if pkg_flags:
        command.extend(pkg_flags)
    else:
        prefix = find_mgba_prefix()
        command.extend((
            f"-I{prefix / 'include'}",
            f"-L{prefix / 'lib'}",
            "-lmgba",
            f"-Wl,-rpath,{prefix / 'lib'}",
        ))
    command.extend(("-o", str(output)))
    run(command)
    return output


def resolve_symbol(elf: Path, name: str) -> int:
    nm = shutil.which("arm-none-eabi-nm")
    if nm is None:
        fail("arm-none-eabi-nm is required")
    result = run([nm, "-S", str(elf)])
    for line in result.stdout.splitlines():
        fields = line.split()
        if fields and fields[-1] == name:
            return int(fields[0], 16)
    fail(f"ELF symbol is missing: {name}")


def validate_screenshot_png(path: Path) -> str:
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
    if all(
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
        for frame, duration, keys in spec["keys"]:
            command.extend(("--key", f"{frame}:{duration}:{keys}"))
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
        "keys": spec["keys"],
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
