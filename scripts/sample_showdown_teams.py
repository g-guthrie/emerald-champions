#!/usr/bin/env python3
"""Generate deterministic Pokémon Showdown teams for Verdant design research."""

from __future__ import annotations

import argparse
import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str], cwd: Path, stdin: str | None = None) -> str:
    return subprocess.run(
        command,
        cwd=cwd,
        input=stdin,
        text=True,
        capture_output=True,
        check=True,
    ).stdout


def seed_for(index: int) -> str:
    # Showdown expects four unsigned 16-bit integers.
    values = (
        index + 1,
        ((index + 1) * 7919) % 65535 or 1,
        ((index + 1) * 1543 + 97) % 65535 or 1,
        ((index + 1) * 31337 + 11) % 65535 or 1,
    )
    return ",".join(str(value) for value in values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--showdown-root", type=Path, required=True)
    parser.add_argument(
        "--unpack-root",
        type=Path,
        help="Optional newer Showdown checkout used only to unpack legacy packed teams",
    )
    parser.add_argument("--format", default="gen9championsrandomdoublesbattle")
    parser.add_argument("--count", type=int, default=30)
    parser.add_argument("--seed-offset", type=int, default=0)
    parser.add_argument(
        "--output",
        type=Path,
        default=ROOT / "docs/showdown_champions_random_doubles_30.json",
    )
    args = parser.parse_args()

    showdown_root = args.showdown_root.resolve()
    launcher = showdown_root / "pokemon-showdown"
    unpack_root = args.unpack_root.resolve() if args.unpack_root else showdown_root
    unpack_launcher = unpack_root / "pokemon-showdown"
    if not launcher.is_file() or not unpack_launcher.is_file():
        raise SystemExit("Showdown launcher is missing")
    if args.count <= 0:
        raise SystemExit("count must be positive")

    samples = []
    help_text = run([str(unpack_launcher), "help"], unpack_root)
    json_command = "json-team" if "pokemon-showdown json-team" in help_text else "unpack-team"

    for index in range(args.count):
        seed = seed_for(index + args.seed_offset)
        packed = run(
            [str(launcher), "generate-team", args.format, seed],
            showdown_root,
        ).strip()
        if packed.startswith("["):
            unpacked = json.loads(packed)
        else:
            unpacked = json.loads(
                run(
                    [str(unpack_launcher), json_command],
                    unpack_root,
                    stdin=packed + "\n",
                )
            )
        samples.append({"seed": seed, "team": unpacked})

    payload = {
        "source": "https://github.com/smogon/pokemon-showdown",
        "showdown_commit": run(["git", "rev-parse", "HEAD"], showdown_root).strip(),
        "format": args.format,
        "sample_count": len(samples),
        "samples": samples,
    }
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2) + "\n")
    print(f"wrote {len(samples)} deterministic teams to {output}")


if __name__ == "__main__":
    main()
