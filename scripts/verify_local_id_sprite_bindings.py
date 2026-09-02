#!/usr/bin/env python3
"""Prove that named object local IDs are bound to the sprite their name promises.

Scripts address overworld objects by ``LOCALID_*`` name.  When a map's object
list is replaced wholesale (the Inclement restoration did this to Meteor Falls
1F 1R) the names can silently land on the wrong rows: ``LOCALID_..._ARCHIE``
pointed at an Aqua grunt while the Archie sprite carried a grunt's ID, and the
scene played every line on the wrong actor.  The visual parity manifest checks
sprites and positions, not this binding, so this gate does.

The rules are name keywords with the sprite families they may legitimately
carry (vanilla uses EXPERT_M for Mr. Briney and VAR_3 for the rival's bike).
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

# keyword -> substrings that the OBJ_EVENT_GFX_ name may contain.
RULES: dict[str, tuple[str, ...]] = {
    "ARCHIE": ("ARCHIE",),
    "MAXIE": ("MAXIE",),
    "MATT": ("MATT", "AQUA_MEMBER"),
    "SHELLY": ("SHELLY", "AQUA_MEMBER"),
    "TABITHA": ("TABITHA", "MAGMA_MEMBER"),
    "COURTNEY": ("COURTNEY", "MAGMA_MEMBER"),
    "AQUA": ("AQUA_MEMBER", "ARCHIE", "MATT", "SHELLY", "SUBMARINE"),
    "MAGMA": ("MAGMA_MEMBER", "MAXIE", "TABITHA", "COURTNEY"),
    "GROUDON": ("GROUDON",),
    "KYOGRE": ("KYOGRE",),
    "RAYQUAZA": ("RAYQUAZA",),
    "STEVEN": ("STEVEN",),
    "WALLY": ("WALLY",),
    "RIVAL": ("VAR_0", "VAR_3", "RIVAL", "MAY", "BRENDAN", "BLUE"),
    "NURSE": ("NURSE",),
    "SCOTT": ("SCOTT",),
    "BIRCH": ("BIRCH",),
    "MOM": ("MOM", "WOMAN_4"),
    "BRINEY": ("BRINEY", "EXPERT_M"),
    "PEEKO": ("PEEKO", "WINGULL"),
    "KECLEON": ("KECLEON",),
    "ITEM_BALL": ("ITEM_BALL",),
    "MEGA_STONE": ("MEGA_STONE", "ITEM_BALL"),
}
# Longest keyword first so LOCALID_..._MOM never matches a bare "MO".
ORDERED_RULES = sorted(RULES.items(), key=lambda kv: -len(kv[0]))


def main() -> int:
    checked = 0
    failures: list[str] = []
    for path in sorted((ROOT / "data" / "maps").glob("*/map.json")):
        payload = json.loads(path.read_text())
        for event in payload.get("object_events", []):
            local_id = event.get("local_id")
            if not isinstance(local_id, str):
                continue
            stem = local_id.removeprefix("LOCALID_")
            graphics = str(event.get("graphics_id", "")).removeprefix("OBJ_EVENT_GFX_")
            for keyword, allowed in ORDERED_RULES:
                if re.search(rf"(^|_){keyword}(_|$)", stem):
                    checked += 1
                    if not any(family in graphics for family in allowed):
                        failures.append(f"{path.parent.name}: {local_id} is bound to {event.get('graphics_id')}")
                    break
    if failures:
        print("LOCAL-ID SPRITE BINDINGS: FAIL\n" + "\n".join(f"  - {row}" for row in failures))
        return 1
    print(f"PASS: {checked} named local IDs are bound to the sprite family their name promises")
    return 0


if __name__ == "__main__":
    sys.exit(main())
