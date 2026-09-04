#!/usr/bin/env python3
"""Self-test for audit_encounter_quality.py.

An audit that reports nothing is indistinguishable from an audit whose rules no
longer fire. This builds a synthetic battle master out of deliberately broken
teams and asserts that each rule still catches its own fault, so the real audit
returning zero findings means something.

It never touches the real master: the fixtures are written to a temp file and
passed with --master.
"""
from __future__ import annotations

import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
AUDIT = ROOT / "scripts" / "audit_encounter_quality.py"


def mon(slot, species, item, ability, nature, points, moves):
    return (f"  {slot}. SPECIES_{species} @ ITEM_{item} | level_offset=0 | "
            f"ability=ABILITY_{ability} | nature=NATURE_{nature} | "
            f"stat_points={points} | moves={','.join('MOVE_' + m for m in moves)}")


def encounter(number, trainer, fmt, mons, extra_branches=(), cap=50):
    out = [f"=== ENCOUNTER {number:04d} ===",
           "location: SelfTest", f"strict_cap: {cap}", "branches:",
           f"--- BRANCH {trainer} ---", f"format: {fmt}", "team:", *mons]
    for name, bfmt, bmons in extra_branches:
        out += [f"--- BRANCH {name} ---", f"format: {bfmt}", "team:", *bmons]
    out.append("=== END ENCOUNTER ===")
    return "\n".join(out) + "\n"


# Each case: (label, expected substring in a finding, encounter text)
CASES = [
    ("Eviolite on a Pokemon that cannot evolve", "Eviolite on a Pokemon that cannot evolve",
     encounter(1, "TRAINER_SELFTEST_A", "double", [
         mon(1, "MELTAN", "EVIOLITE", "MAGNET_PULL", "RELAXED", "32/0/32/0/2/0",
             ["FLASH_CANNON", "THUNDER_WAVE", "ACID_ARMOR", "HEADBUTT"]),
         mon(2, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "PROTECT"])])),

    ("Belly Drum with no payment", "does not pay for the halved HP",
     encounter(2, "TRAINER_SELFTEST_B", "double", [
         mon(1, "AZUMARILL", "WISE_GLASSES", "HUGE_POWER", "ADAMANT", "2/32/0/0/0/32",
             ["BELLY_DRUM", "AQUA_JET", "PLAY_ROUGH", "PROTECT"]),
         mon(2, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "PROTECT"])])),

    ("weather ability with no setter", "needs SUN and the team never sets it",
     encounter(3, "TRAINER_SELFTEST_C", "double", [
         mon(1, "VICTREEBEL", "LIFE_ORB", "CHLOROPHYLL", "NAIVE", "2/0/0/32/0/32",
             ["SLEEP_POWDER", "SLUDGE_BOMB", "GIGA_DRAIN", "PROTECT"]),
         mon(2, "CROBAT", "SITRUS_BERRY", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
             ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])])),

    ("Solar Beam with no sun", "Solar Beam without sun",
     encounter(4, "TRAINER_SELFTEST_D", "double", [
         mon(1, "LUDICOLO", "LIFE_ORB", "RAIN_DISH", "MODEST", "2/0/0/32/0/32",
             ["SOLAR_BEAM", "SCALD", "ICE_BEAM", "PROTECT"]),
         mon(2, "CROBAT", "SITRUS_BERRY", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
             ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])])),

    ("team that cannot damage a type at all", "can damage GROUND at all",
     encounter(5, "TRAINER_SELFTEST_E", "double", [
         mon(1, "REGIELEKI", "MAGNET", "TRANSISTOR", "TIMID", "2/0/0/32/0/32",
             ["THUNDERBOLT", "ELECTROWEB", "VOLT_SWITCH", "PROTECT"]),
         mon(2, "RAICHU", "LIFE_ORB", "STATIC", "TIMID", "2/0/0/32/0/32",
             ["THUNDERBOLT", "THUNDER_WAVE", "VOLT_SWITCH", "PROTECT"])])),

    ("Assault Vest with a status move", "Assault Vest cannot use",
     encounter(6, "TRAINER_SELFTEST_F", "double", [
         mon(1, "SNORLAX", "ASSAULT_VEST", "THICK_FAT", "ADAMANT", "32/32/0/0/2/0",
             ["BODY_SLAM", "CRUNCH", "PROTECT", "EARTHQUAKE"]),
         mon(2, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "AQUA_TAIL"])])),

    ("Nature that lowers the only attacking stat", "lowers Attack on a physical-only set",
     encounter(7, "TRAINER_SELFTEST_G", "double", [
         mon(1, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "MODEST", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "AQUA_TAIL"]),
         mon(2, "CROBAT", "SAFETY_GOGGLES", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
             ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])])),

    ("Stat Points on a stat the set never uses", "on a special-only set",
     encounter(8, "TRAINER_SELFTEST_H", "double", [
         mon(1, "ALAKAZAM", "LIFE_ORB", "MAGIC_GUARD", "TIMID", "2/32/0/32/0/0",
             ["PSYCHIC", "SHADOW_BALL", "ENERGY_BALL", "PROTECT"]),
         mon(2, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "AQUA_TAIL"])])),

    ("duplicate species on one team", "appears 2 times",
     encounter(9, "TRAINER_SELFTEST_I", "double", [
         mon(1, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "AQUA_TAIL"]),
         mon(2, "SWAMPERT", "LIFE_ORB", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "PROTECT"])])),

    ("Choice item on a set that must switch moves", "locks into one move",
     encounter(10, "TRAINER_SELFTEST_J", "double", [
         mon(1, "DODRIO", "CHOICE_SCARF", "MOXIE", "JOLLY", "2/32/0/0/0/32",
             ["BRAVE_BIRD", "DOUBLE_EDGE", "LOW_KICK", "PROTECT"]),
         mon(2, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
             ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "AQUA_TAIL"])])),
]

GOOD_PAIR = [
    mon(1, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
        ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "PROTECT"]),
    mon(2, "CROBAT", "SAFETY_GOGGLES", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
        ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])]

# A rematch ladder that peaks and then drops: rung 2 must not be weaker than rung 1.
LADDER = (encounter(30, "TRAINER_SELFTEST_LADDER_1", "double", GOOD_PAIR, cap=50)
          + encounter(31, "TRAINER_SELFTEST_LADDER_2", "double", GOOD_PAIR, cap=40))

# A control: a well-built team must produce no findings at all, so the rules are
# not simply firing on everything.
CONTROL = encounter(11, "TRAINER_SELFTEST_CLEAN", "double", [
    mon(1, "SWAMPERT", "SITRUS_BERRY", "TORRENT", "ADAMANT", "32/32/0/0/2/0",
        ["WATERFALL", "EARTHQUAKE", "ICE_PUNCH", "PROTECT"]),
    mon(2, "CROBAT", "SAFETY_GOGGLES", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
        ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])])

# A multi battle: the partner's Drought must make Chlorophyll legitimate.
MULTI = encounter(12, "TRAINER_SELFTEST_SUN_A", "multi", [
    mon(1, "NINETALES", "CHARCOAL", "DROUGHT", "TIMID", "2/0/0/32/0/32",
        ["HEAT_WAVE", "SOLAR_BEAM", "ENCORE", "PROTECT"]),
    mon(2, "KROOKODILE", "SITRUS_BERRY", "INTIMIDATE", "ADAMANT", "2/32/0/0/0/32",
        ["HIGH_HORSEPOWER", "CRUNCH", "TAUNT", "PROTECT"])],
    extra_branches=[("TRAINER_SELFTEST_SUN_B", "multi", [
        mon(1, "VICTREEBEL", "LIFE_ORB", "CHLOROPHYLL", "NAIVE", "2/0/0/32/0/32",
            ["SLEEP_POWDER", "LEAF_STORM", "SLUDGE_BOMB", "PROTECT"]),
        mon(2, "CROBAT", "SAFETY_GOGGLES", "INNER_FOCUS", "JOLLY", "2/32/0/0/0/32",
            ["TAILWIND", "TAUNT", "BRAVE_BIRD", "U_TURN"])])])


def findings(out: str) -> list[str]:
    """Fixture trainers have no entry in trainers.party, so the AI-floor rule
    always fires on them. It is exercised against the real corpus instead."""
    return [line.strip() for line in out.splitlines()
            if line.strip().startswith("[") and not line.strip().startswith("[ai]")]


def run(text: str) -> str:
    with tempfile.NamedTemporaryFile("w", suffix=".txt", delete=False) as handle:
        handle.write(text)
        path = handle.name
    try:
        result = subprocess.run([sys.executable, str(AUDIT), "--master", path],
                                capture_output=True, text=True)
        return result.stdout + result.stderr
    finally:
        Path(path).unlink(missing_ok=True)


def main() -> int:
    failures = []
    for label, expect, text in CASES:
        out = run(text)
        hits = findings(out)
        if any(expect in h for h in hits):
            print(f"  ok      {label}")
        else:
            print(f"  FAILED  {label}")
            print(f"            expected a finding containing {expect!r}")
            for h in hits:
                print(f"            got: {h}")
            failures.append(label)

    out = run(CONTROL)
    hits = findings(out)
    total = len(hits)
    if total == 0:
        print("  ok      a well-built team produces no findings")
    else:
        print(f"  FAILED  control team produced {total} finding(s):")
        for line in hits:
            print(f"            {line}")
        failures.append("control team is clean")

    out = run(LADDER)
    if any("below rematch 1" in h for h in findings(out)):
        print("  ok      a rematch ladder that goes backwards")
    else:
        print("  FAILED  a rematch ladder that goes backwards was not caught")
        failures.append("rematch ladder direction")

    out = run(MULTI)
    hits = findings(out)
    total = len(hits)
    if total == 0:
        print("  ok      a multi battle shares weather across the two trainers")
    else:
        print(f"  FAILED  multi-battle weather sharing produced {total} finding(s):")
        for line in hits:
            print(f"            {line}")
        failures.append("multi-battle weather sharing")

    print()
    if failures:
        print(f"FAIL: {len(failures)} audit rule(s) no longer work: " + ", ".join(failures))
        return 1
    print(f"PASS: all {len(CASES) + 3} audit self-tests hold")
    return 0


if __name__ == "__main__":
    sys.exit(main())
