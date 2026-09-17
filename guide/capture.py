#!/usr/bin/env python3
"""Capture real screenshots for the field guide by warping the game to each spot.

Every picture in the guide comes out of the actual ROM. This boots the release
build headlessly through the Studio core, warps to a map and tile, optionally
presses buttons to open a door, talk to somebody or bring up a menu, then saves
the framebuffer. Because it walks the whole campaign it doubles as a sweep: a
map that will not load, a warp that lands in a wall or a script that never
returns to an idle field shows up here as a failed shot rather than a bad page.

    .venv-studio/bin/python guide/capture.py guide/shots/petalburg.json

Shot file format (JSON list):

    [
      {"id": "petalburg-city",
       "map": "MAP_PETALBURG_CITY", "x": 15, "y": 8, "facing": 2,
       "settle": 90,
       "caption": "Petalburg City, looking toward the Gym"},

      {"id": "petalburg-gym",
       "map": "MAP_PETALBURG_CITY_GYM", "x": 4, "y": 8,
       "flags": {"FLAG_HIDE_PETALBURG_GYM_NORMAN": false},
       "steps": [{"hold": "UP", "frames": 40}, {"press": "A", "frames": 90}],
       "caption": "Norman will not battle you yet"}
    ]

Shot keys: id, map, x, y, facing, settle, chapter, flags, vars, items, steps,
caption. flags/vars/items are applied before the warp.
"""
import argparse
import asyncio
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools/studio"))

import server                      # noqa: E402
from scenes import keys, packet_state  # noqa: E402
from PIL import Image              # noqa: E402

SETTLE_DEFAULT = 60


async def hold(studio, mask, frames):
    """Deliver a button mask to the core for a number of frames."""
    packet = None
    remaining = max(1, frames)
    while remaining:
        chunk = min(remaining, 30)
        packet = await studio.core.tick(keys=mask, frames=chunk)
        remaining -= chunk
    return packet


async def run_steps(studio, steps):
    packet = None
    for step in steps or []:
        frames = int(step.get("frames", 30))
        if "hold" in step:
            packet = await hold(studio, keys(step["hold"]), frames)
        elif "press" in step:
            # A tap the game will register, then let the result play out.
            await studio.core.tick(keys=keys(step["press"]), frames=6)
            packet = await hold(studio, 0, max(1, frames - 6))
        else:
            # A step with only `frames` just waits. Dialogue shots use this:
            # the text printer is slow, so give a box 500-600 frames to finish.
            packet = await hold(studio, 0, frames)
    return packet


def save(packet, path, scale):
    img = Image.frombytes("RGBA", (240, 160), packet[16:153616]).convert("RGB")
    if scale > 1:
        img = img.resize((240 * scale, 160 * scale), Image.NEAREST)
    path.parent.mkdir(parents=True, exist_ok=True)
    img.save(path)


async def capture(shots, out_dir, scale, chapter):
    studio = server.Studio(0)
    server.WORK = out_dir / "runtime"
    server.WORK.mkdir(parents=True, exist_ok=True)
    build = await studio.stage()
    studio.build = build
    studio.build_id = build["id"]
    studio.core, packet = await studio.boot(build, chapter=chapter)
    studio.packet = packet
    studio.keys = studio.key_pulses = 0
    current_chapter = chapter

    results = []
    for shot in shots:
        entry = dict(id=shot["id"], map=shot["map"], caption=shot.get("caption", ""),
                     chapter=int(shot.get("chapter", chapter)))
        try:
            # Campaign state decides who is standing where, so a shot may ask for
            # a different chapter than the one before it. Re-boot when it does.
            want = int(shot.get("chapter", chapter))
            if want != current_chapter:
                await studio.core.close()
                studio.core, packet = await studio.boot(build, chapter=want)
                studio.packet = packet
                current_chapter = want
            # Stage story state before the warp, so the map loads with the right
            # actors standing on it. Flags decide who is visible.
            const = studio.cat.constants
            for name, value in (shot.get("flags") or {}).items():
                if name not in const:
                    raise KeyError(f"unknown flag {name}")
                studio.ingest(await studio.core.field_command(6, [const[name], 1 if value else 0]))
            for name, value in (shot.get("vars") or {}).items():
                if name not in const:
                    raise KeyError(f"unknown var {name}")
                studio.ingest(await studio.core.field_command(7, [const[name], int(value)]))
            for name, count in (shot.get("items") or {}).items():
                if name not in const:
                    raise KeyError(f"unknown item {name}")
                studio.ingest(await studio.core.field_command(8, [const[name], int(count)]))

            m = studio.cat.maps[shot["map"]]
            args = [m["group"], m["num"], int(shot["x"]), int(shot["y"]), int(shot.get("facing", 1))]
            studio.ingest(await studio.core.field_command(2, args))

            packet = await studio.core.tick(frames=int(shot.get("settle", SETTLE_DEFAULT)))
            stepped = await run_steps(studio, shot.get("steps"))
            if stepped is not None:
                packet = stepped
            studio.packet = packet

            state = packet_state(packet)
            if state["group"] != m["group"] or state["num"] != m["num"]:
                raise RuntimeError(
                    f"landed on map {state['group']}/{state['num']}, expected {m['group']}/{m['num']}")

            path = out_dir / f"{shot['id']}.png"
            save(packet, path, scale)
            try:
                shown = str(path.relative_to(ROOT))
            except ValueError:
                shown = str(path)
            entry.update(ok=True, file=shown,
                         x=state["x"], y=state["y"], battle=state["battle"],
                         actors=[dict(local_id=a["local_id"], graphic=a["graphic"],
                                      x=a["x"], y=a["y"], invisible=a["invisible"])
                                 for a in state["actors"]])
            if shot.get("debug"):
                print(f"       actors: {entry['actors']}")
            print(f"  ok   {shot['id']:<34} {shot['map']} ({state['x']},{state['y']})")
        except Exception as exc:                      # noqa: BLE001 - reported, not raised
            entry.update(ok=False, error=f"{type(exc).__name__}: {exc}")
            print(f"  FAIL {shot['id']:<34} {entry['error']}")
        results.append(entry)

    await studio.core.close()
    return results


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("shots", type=Path, help="JSON list of shot specs")
    ap.add_argument("--out", type=Path, default=ROOT / "guide/assets/captures")
    ap.add_argument("--scale", type=int, default=3, help="integer upscale, nearest neighbour")
    ap.add_argument("--chapter", type=int, default=7, choices=(3, 4, 7))
    args = ap.parse_args()

    shots = json.loads(args.shots.read_text())
    print(f"{len(shots)} shots -> {args.out}")
    results = asyncio.run(capture(shots, args.out.resolve(), args.scale, args.chapter))

    manifest = args.out / "manifest.json"
    manifest.write_text(json.dumps(results, indent=2) + "\n")
    failed = [r for r in results if not r.get("ok")]
    print(f"\n{len(results) - len(failed)} captured, {len(failed)} failed -> {manifest}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
