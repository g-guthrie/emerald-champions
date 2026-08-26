#!/usr/bin/env python3
"""Reject Z-Move/Ultra Burst battle-engine and animation residue."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = ("asm", "data", "include", "src")
SOURCE_SUFFIXES = {".c", ".h", ".inc", ".s"}

FORBIDDEN = {
    "Z Crystal hold effect": re.compile(r"\bHOLD_EFFECT_Z_CRYSTAL\b"),
    "Z-Move runtime hook": re.compile(r"\b(?:IsZMove|ITEM_Z_POWER_RING)\b"),
    "Z/Ultra animation tag": re.compile(
        r"\bANIM_TAG_(?:ULTRA_BURST_SYMBOL|Z_MOVE_SYMBOL|REALLY_BIG_ROCK|"
        r"COCOON|CORKSCREW|HAVOC_SPEAR|PURPLE_DRAKE)\b"
    ),
    "Z-Move background id": re.compile(
        r"\bBG_(?:ZMOVE_ACTIVATE|TECTONIC_RAGE|ZMOVE_MOUNTAIN|"
        r"NEVERENDING_NIGHTMARE|INFERNO_OVERDRIVE|BLOOM_DOOM|"
        r"SHATTERED_PSYCHE|TWINKLE_TACKLE|BLACKHOLE_ECLIPSE|"
        r"SOULSTEALING_7STAR_STRIKE|MALICIOUS_MOONSAULT|"
        r"CLANGOROUS_SOULBLAZE|SNUGGLE_FOREVER)\b"
    ),
    "Z/Ultra animation symbol": re.compile(
        r"\bgBattleAnim(?:Sprite(?:Gfx|Pal)|Bg(?:Image|Palette|Tilemap))_"
        r"(?:ZMove(?:Symbol|Activate|Mountain)|NecrozmaStar|BigRock|Cacoon|"
        r"GigavoltHavocSpear|BlackholeEclipse|BloomDoom|ClangorousSoulblaze|"
        r"InfernoOverdrive|MaliciousMoonsault|NeverendingNightmare|"
        r"ShatteredPsyche|SnuggleForever|SoulStealing7StarStrike|"
        r"TectonicRage)\b"
    ),
    "Arceus Z form rule": re.compile(
        r"FORM_ITEM_HOLD_ABILITY[^\n]*\bITEM_[A-Z0-9_]*IUM_Z\b"
    ),
    "legacy signature Z animation name": re.compile(
        r"\b(?:TwinkleTackle|BlackHoleEclipse|ContinentalCrush|"
        r"StokedSparksurfer|LightThatBurnsTheSky|MaxKnuckle)\w*\b"
    ),
}

ASSET_STEMS = {
    "blackhole_eclipse",
    "bloom_doom",
    "clangorous_soulblaze",
    "inferno_overdrive",
    "malicious_moonsault",
    "neverending_nightmare",
    "shattered_psyche",
    "snuggle_forever",
    "soulstealing_7star_strike",
    "tectonic_rage",
    "twinkle_tackle",
    "zmove_activate",
    "zmove_mountain",
    "big_rock",
    "cacoon",
    "drill",
    "gigavolt_havoc_spear",
    "necrozma_star",
    "z_move_symbol",
}


def main() -> int:
    failures: list[str] = []

    for source_root in SOURCE_ROOTS:
        for path in (ROOT / source_root).rglob("*"):
            if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
                continue
            text = path.read_text(errors="replace")
            for label, pattern in FORBIDDEN.items():
                for match in pattern.finditer(text):
                    line = text.count("\n", 0, match.start()) + 1
                    failures.append(
                        f"{path.relative_to(ROOT)}:{line}: {label}: {match.group(0)}"
                    )

    for root in (
        ROOT / "graphics/battle_anims/backgrounds/new",
        ROOT / "graphics/battle_anims/sprites/new",
    ):
        for path in root.iterdir():
            if path.is_file() and any(
                path.name == stem or path.name.startswith(f"{stem}.")
                for stem in ASSET_STEMS
            ):
                failures.append(
                    f"Z/Ultra animation asset still present: {path.relative_to(ROOT)}"
                )

    if failures:
        print("Z-Move engine removal verification failed:")
        for failure in failures:
            print(f"  - {failure}")
        return 1

    print("Z-Move engine removal verification passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
