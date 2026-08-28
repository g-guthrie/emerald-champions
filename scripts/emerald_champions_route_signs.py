#!/usr/bin/env python3
"""Wire Hoenn wayfinding signs to the native live encounter formatter."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SIGNS_BY_MAP = {
    "Route101": ("Route101_EventScript_RouteSign",),
    "Route102": ("Route102_EventScript_RouteSignOldale", "Route102_EventScript_RouteSignPetalburg"),
    "Route103": ("Route103_EventScript_RouteSign",),
    "Route104": ("Route104_EventScript_RouteSignPetalburg", "Route104_EventScript_RouteSignRustboro"),
    "Route110": ("Route110_EventScript_SlateportCitySign", "Route110_EventScript_Route103Sign", "Route110_EventScript_MauvilleCitySign"),
    "Route111": ("Route111_EventScript_RouteSignMauville", "Route111_EventScript_RouteSign112", "Route111_EventScript_RouteSign113"),
    "Route112": ("Route112_EventScript_MtChimneyCableCarSign", "Route112_EventScript_MtChimneySign", "Route112_EventScript_RouteSignLavaridge"),
    "Route113": ("Route113_EventScript_RouteSign111", "Route113_EventScript_RouteSignFallarbor"),
    "Route114": ("Route114_EventScript_MeteorFallsSign",),
    "Route115": ("Route115_EventScript_RouteSignRustboro", "Route115_EventScript_MeteorFallsSign"),
    "Route116": ("Route116_EventScript_RouteSignRustboro", "Route116_EventScript_RusturfTunnelSign"),
    "Route117": ("Route117_EventScript_RouteSignVerdanturf", "Route117_EventScript_RouteSignMauville"),
    "Route118": ("Route118_EventScript_RouteSignMauville", "Route118_EventScript_RouteSign119"),
    "Route119": ("Route119_EventScript_RouteSignFortree",),
    "Route120": ("Route120_EventScript_RouteSignFortree", "Route120_EventScript_RouteSign121"),
    "Route121": ("Route121_EventScript_MtPyrePierSign",),
    "Route123": ("Route123_EventScript_RouteSign", "Route123_EventScript_RouteSignMtPyre"),
}


def block(text: str, label: str) -> re.Match[str]:
    match = re.search(rf"(^\s*{re.escape(label)}(?:::|:).*?)(?=^\s*[A-Za-z0-9_]+(?:::|:)|\Z)", text, re.M | re.S)
    if match is None:
        raise ValueError(f"missing label {label}")
    return match


def write() -> None:
    for map_name, labels in SIGNS_BY_MAP.items():
        path = ROOT / "data" / "maps" / map_name / "scripts.inc"
        text = path.read_text()
        for label in labels:
            match = block(text, label)
            body = match.group(1)
            if "goto Common_EventScript_ShowRouteSpecies" in body:
                continue
            changed, count = re.subn(r"\n\s*end\s*$", "\n\tgoto Common_EventScript_ShowRouteSpecies\n", body, count=1)
            if count != 1:
                raise ValueError(f"{map_name}:{label} is not a simple sign script")
            text = text[:match.start(1)] + changed + text[match.end(1):]
        path.write_text(text)


def check() -> None:
    failures = []
    for map_name, labels in SIGNS_BY_MAP.items():
        map_path = ROOT / "data" / "maps" / map_name / "map.json"
        script_path = ROOT / "data" / "maps" / map_name / "scripts.inc"
        payload = json.loads(map_path.read_text())
        sign_scripts = {event.get("script") for event in payload.get("bg_events", []) if event.get("type") == "sign"}
        text = script_path.read_text()
        for label in labels:
            if label not in sign_scripts:
                failures.append(f"{map_name}:{label} is not a physical sign")
                continue
            if "goto Common_EventScript_ShowRouteSpecies" not in block(text, label).group(1):
                failures.append(f"{map_name}:{label} lacks the live encounter page")
    source = (ROOT / "src" / "wild_encounter.c").read_text()
    for token in (
        "BufferCurrentMapRouteSignSpecies", "sText_RouteSignGrass", "sText_RouteSignSurf",
        "sText_RouteSignRockSmash", "sText_RouteSignOldRod", "sText_RouteSignGoodRod",
        "sText_RouteSignSuperRod", "sText_RouteSignHidden", "sText_RouteSignUnderBridge",
    ):
        if token not in source:
            failures.append(f"formatter missing {token}")
    if "percent" in source[source.find("ROUTE_SIGN_MAX_METHOD_SPECIES"):source.find("bool8 CheckFeebasAtCoords")].lower():
        failures.append("route sign formatter still exposes percentages")
    if failures:
        raise SystemExit("\n".join(failures))
    print(f"PASS: {sum(map(len, SIGNS_BY_MAP.values()))} wayfinding signs show method-grouped live encounters")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    if args.write:
        write()
    check()


if __name__ == "__main__":
    main()
