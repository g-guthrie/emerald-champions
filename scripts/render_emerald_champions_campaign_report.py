#!/usr/bin/env python3
"""Render a responsive visual review report from campaign simulator evidence."""

from __future__ import annotations

import argparse
from html import escape
import json
import os
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = ROOT / "tests/campaign/playthrough.json"
DEFAULT_RUN = ROOT / "work/campaign-playthrough/current/latest-run.json"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load(path: Path) -> dict:
    require(path.is_file(), f"missing JSON file: {path}")
    value = json.loads(path.read_text())
    require(isinstance(value, dict), f"top-level JSON must be an object: {path}")
    return value


def relative_image(path: str, report: Path) -> str:
    image = Path(path)
    if not image.is_absolute():
        image = ROOT / image
    return Path(os.path.relpath(image, report.parent)).as_posix()


def render(manifest: dict, run: dict, report: Path) -> None:
    manifest_rows = manifest.get("segments")
    run_rows = run.get("segments")
    require(isinstance(manifest_rows, list) and manifest_rows, "manifest has no segments")
    require(isinstance(run_rows, list) and run_rows, "run has no segments")
    specs = {row["id"]: row for row in manifest_rows}
    actual = {row["segment"]: row for row in run_rows}
    cards: list[str] = []

    for index, spec in enumerate(manifest_rows, 1):
        segment = spec["id"]
        row = actual.get(segment)
        if row is None:
            cards.append(
                f'<article class="card missing"><h2>{index}. {escape(segment)}</h2>'
                '<p class="status">NOT EXECUTED</p></article>'
            )
            continue
        telemetry = row.get("telemetry", {})
        images = []
        for shot in row.get("screenshots", []):
            path = str(shot.get("path", ""))
            src = relative_image(path, report)
            label = Path(path).name
            images.append(
                '<figure>'
                f'<a href="{escape(src)}"><img loading="lazy" src="{escape(src)}" '
                f'alt="{escape(segment + " " + label)}"></a>'
                f'<figcaption>{escape(label)}</figcaption>'
                '</figure>'
            )
        expected = spec.get("expected", {})
        coverage = spec.get("coverage", {})
        assertions = row.get("assertions", {"flags": {}, "vars": {}})
        cards.append(
            '<article class="card">'
            f'<header><div><span class="chapter">{escape(str(spec.get("chapter", "Unsorted")))}</span>'
            f'<h2>{index}. {escape(segment)}</h2></div><span class="status">PASS</span></header>'
            '<dl>'
            f'<dt>Map</dt><dd>0x{int(telemetry.get("gEcHeadlessCampaignMapId", 0)):04x}</dd>'
            f'<dt>Position</dt><dd>{escape(str([telemetry.get("gEcHeadlessCampaignPlayerX"), telemetry.get("gEcHeadlessCampaignPlayerY")]))}</dd>'
            f'<dt>Facing</dt><dd>{escape(str(telemetry.get("gEcHeadlessCampaignPlayerFacing", "not recorded")))}</dd>'
            f'<dt>Battles</dt><dd>{escape(str(telemetry.get("gEcHeadlessCampaignBattleSerial", 0)))}</dd>'
            f'<dt>Captures</dt><dd>{escape(str(telemetry.get("gEcHeadlessCampaignCaptureSerial", 0)))}</dd>'
            f'<dt>Expected</dt><dd><code>{escape(json.dumps(expected, sort_keys=True))}</code></dd>'
            f'<dt>Assertions</dt><dd><code>{escape(json.dumps(assertions, sort_keys=True))}</code></dd>'
            f'<dt>Coverage</dt><dd><code>{escape(json.dumps(coverage, sort_keys=True))}</code></dd>'
            '</dl>'
            f'<div class="shots">{"".join(images)}</div>'
            '</article>'
        )

    extra = [segment for segment in actual if segment not in specs]
    summary = (
        f"{len(actual)}/{len(manifest_rows)} declared segments executed"
        + (f"; {len(extra)} undeclared result(s)" if extra else "")
    )
    immutable = all(
        artifact.get("verified_immutable") is True
        for artifact in run.get("artifact_evidence", {}).values()
    )
    html = f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Emerald Champions campaign review</title>
<style>
:root {{ color-scheme: dark; font-family: ui-sans-serif, system-ui, sans-serif; background:#0c1510; color:#e9f4ec; }}
body {{ margin:0; padding:20px; }} main {{ max-width:1400px; margin:auto; }}
h1 {{ margin-bottom:4px; }} .summary {{ color:#a9c5b0; margin-top:0; overflow-wrap:anywhere; }}
.card {{ margin:18px 0; padding:16px; border:1px solid #31543a; border-radius:12px; background:#132219; }}
.card header {{ display:flex; justify-content:space-between; gap:12px; align-items:flex-start; }}
.card h2 {{ margin:4px 0 14px; font-size:1.05rem; overflow-wrap:anywhere; }}
.chapter {{ color:#86d49b; font-size:.82rem; text-transform:uppercase; letter-spacing:.08em; }}
.status {{ color:#8cf0a6; font-weight:700; }} .missing {{ border-color:#8a4b4b; }} .missing .status {{ color:#ff9b9b; }}
dl {{ display:grid; grid-template-columns:max-content 1fr; gap:5px 12px; margin:0 0 14px; font-size:.88rem; }}
dt {{ color:#a9c5b0; }} dd {{ margin:0; min-width:0; overflow-wrap:anywhere; }} code {{ color:#d5e8da; white-space:normal; }}
.shots {{ display:grid; grid-template-columns:repeat(auto-fit,minmax(240px,1fr)); gap:12px; }}
figure {{ margin:0; }} img {{ width:100%; image-rendering:pixelated; border-radius:6px; background:#000; }}
figcaption {{ margin-top:5px; color:#a9c5b0; font-size:.78rem; overflow-wrap:anywhere; }}
@media (max-width:520px) {{ body {{ padding:10px; }} .card {{ padding:11px; }} dl {{ grid-template-columns:1fr; }} dt {{ margin-top:5px; }} }}
</style>
</head>
<body><main>
<h1>Emerald Champions campaign review</h1>
<p class="summary">{escape(summary)}. Run {escape(str(run.get("run_id", "legacy")))}.
ROM {escape(str(run.get("rom_sha256", "unknown")))}.
Manifest {escape(str(run.get("manifest_sha256", "unknown")))}.
Immutable snapshots verified: {escape(str(immutable))}</p>
{"".join(cards)}
</main></body></html>
"""
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(html)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--run", type=Path, default=DEFAULT_RUN)
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    report = args.out or args.run.parent / "report.html"
    render(load(args.manifest), load(args.run), report)
    print(f"WROTE: {report}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, json.JSONDecodeError) as error:
        print(f"campaign report: FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
