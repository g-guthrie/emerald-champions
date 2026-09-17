#!/usr/bin/env python3
"""Turn extracted location data into guide pages.

    .venv-studio/bin/python guide/extract.py --all > guide/data/locations.json
    .venv-studio/bin/python guide/compile.py --order guide/data/order.json

Reads the location data, the capture manifest and the rendered map plates, and
writes one HTML page per location into guide/out/. Nothing is invented: if a
location has no capture yet its event block is left out rather than faked, and
the run prints what is missing so the gaps are visible.
"""
import argparse
import html
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "guide/out"
CAPTURES = ROOT / "guide/assets/captures"
MAPS = ROOT / "guide/assets/maps"

# Which warp destinations earn a label on the plate, and what to call them.
LABELS = [
    (re.compile(r"_GYM$"), "Gym"),
    (re.compile(r"_POKEMON_CENTER"), "Pokémon Center"),
    (re.compile(r"_MART$"), "Pokémart"),
    (re.compile(r"_WALLYS_HOUSE$"), "Wally's House"),
    (re.compile(r"_LAB$"), "Lab"),
    (re.compile(r"_HOUSE\d*$"), "House"),
    (re.compile(r"DEVON_CORP"), "Devon Corp"),
    (re.compile(r"_HARBOR$"), "Harbor"),
    (re.compile(r"_MUSEUM$"), "Museum"),
    (re.compile(r"_SCHOOL$"), "School"),
]


CHAPTER_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{title}</title>
<link rel="stylesheet" href="../style.css"></head>
<body>
<section class="page chapter">
  <div class="sheet">
    <div class="masthead">
      <div class="plaque"><img src="../assets/logo.png" alt="Emerald Champions"></div>
      <div class="kicker"><b>OFFICIAL</b> FIELD GUIDE</div>
    </div>
    <div class="chapter-plate">
      <div class="chapter-eyebrow">CHAPTER</div>
      <h1>{title}</h1>
      <div class="chapter-count">{count} locations</div>
      <ul class="chapter-list">
{listed}
      </ul>
    </div>
  </div>
  <div class="folio"><div class="n">{folio}</div><div class="ball"></div></div>
</section>
</body>
</html>
"""


def label_for(dest):
    for pattern, name in LABELS:
        if pattern.search(dest):
            return name
    return None


def esc(text):
    return html.escape(str(text), quote=True)


def pins(loc):
    """Callouts over the plate, one per labelled door, de-duplicated."""
    out, used = [], set()
    for warp in loc["warps"]:
        name = label_for(warp["dest"])
        if not name or name in used:
            continue
        used.add(name)
        left = (warp["x"] + 0.5) / loc["width"] * 100
        top = (warp["y"] - 0.6) / loc["height"] * 100
        out.append(f'<div class="pin" style="left:{left:.1f}%; top:{max(top, 2):.1f}%">{esc(name)}</div>')
    return "\n            ".join(out)


def items_block(loc):
    if not loc["items"]:
        return ""
    rows = "\n".join(
        f'            <li{" class=\"hidden\"" if i["kind"] == "hidden" else ""}>{esc(i["item"])}'
        f'{" <em>(hidden)</em>" if i["kind"] == "hidden" else ""}</li>'
        for i in loc["items"])
    return f'''        <div class="items">
          <h3>ITEMS</h3>
          <ul>
{rows}
          </ul>
        </div>'''


def mart_block(loc):
    if not loc["mart"]:
        return ""
    rows = "\n".join(
        f'            <tr><td>{esc(m["item"])}</td>'
        f'<td class="num">{"₽" + str(m["price"]) if m["price"] else "—"}</td></tr>'
        for m in loc["mart"])
    return f'''        <div class="tbl mart">
          <h3>POKÉMART</h3>
          <table>
            <tr><th>ITEM</th><th style="text-align:right">PRICE</th></tr>
{rows}
          </table>
        </div>'''


def encounters_block(loc):
    if not loc["encounters"]:
        return ""
    entries = loc["encounters"]
    half = (len(entries) + 1) // 2
    left, right = entries[:half], entries[half:]
    rows = []
    for i in range(half):
        cells = []
        for column in (left, right):
            if i < len(column):
                e = column[i]
                cells.append(f'<td>{esc(e["species"])}</td><td>{esc(e["method"])}</td>'
                             f'<td>{e["min"]}–{e["max"]}</td>')
            else:
                cells.append("<td></td><td></td><td></td>")
        rows.append("            <tr>" + "".join(cells) + "</tr>")
    head = ("<tr><th>POKÉMON</th><th>METHOD</th><th>LEVELS</th>"
            "<th>POKÉMON</th><th>METHOD</th><th>LEVELS</th></tr>")
    return f'''        <div class="tbl water">
          <h3>POKÉMON IN THIS AREA</h3>
          <table>
            {head}
{chr(10).join(rows)}
          </table>
        </div>'''


def trainers_block(loc):
    if not loc["trainers"]:
        return ""
    rows = []
    for t in loc["trainers"]:
        name = t["trainer"].replace("TRAINER_", "").replace("_", " ").title()
        team = ", ".join(m["species"] for m in t["party"][:6]) or "—"
        rows.append(f'            <tr><td>{esc(name)}</td><td>{esc(t["cls"])}</td>'
                    f'<td class="num">{t["cap"]}</td><td class="team">{esc(team)}</td></tr>')
    return f'''    <div class="tbl roster">
      <h3>TRAINERS HERE</h3>
      <table>
        <tr><th>TRAINER</th><th>CLASS</th><th style="text-align:right">CAP</th><th>TEAM</th></tr>
{chr(10).join(rows)}
      </table>
    </div>'''


def strip_block(shot, pretty):
    """A pan: several viewpoints stitched into one picture of the whole place."""
    rel = Path(shot["file"]).name
    frames = shot.get("frames") or []
    tall = "tall" if "vertical" in rel or shot.get("direction") == "vertical" else ""
    return f'''    <div class="strip {tall}">
      <h4>{esc(pretty).upper()} END TO END</h4>
      <img src="../assets/captures/{esc(rel)}" alt="{esc(pretty)}">
      <div class="cap">{len(frames) or "Several"} viewpoints, stitched in order of travel</div>
    </div>'''


def events_block(shots):
    if not shots:
        return ""
    out = []
    for n, shot in enumerate(shots, 1):
        caption = esc(shot.get("caption", ""))
        rel = Path(shot["file"]).name
        out.append(f'''    <div class="event">
      <h4>Event {n}</h4>
      <div class="shot">
        <img src="../assets/captures/{esc(rel)}" alt="{caption}">
        <div class="cap">{caption}</div>
      </div>
      <p>{caption}.</p>
    </div>''')
    return "\n".join(out)


def pretty_name(name):
    """Route102 -> Route 102; PetalburgCity_Gym -> Petalburg City Gym."""
    return re.sub(r"(?<=[a-z])(?=[A-Z])|(?<=[A-Za-z])(?=\d)", " ", name).replace("_", " ")


def chapter_page(title, locations, folio):
    """A divider that tells the reader where they are in the campaign."""
    listed = "\n".join(f"        <li>{esc(pretty_name(n))}</li>" for n in locations)
    body = CHAPTER_TEMPLATE.format(title=esc(title), count=len(locations),
                                   listed=listed, folio=folio)
    return body


def page(loc, shots, folio, plate_exists):
    pretty = pretty_name(loc["map"])
    plate = ""
    if plate_exists:
        plate = f'''        <div class="mapplate">
          <div class="plate">
            <img src="../assets/maps/{esc(loc["map"])}.png" alt="{esc(pretty)}">
            {pins(loc)}
          </div>
        </div>'''
    grid = "\n".join(x for x in (items_block(loc), plate, mart_block(loc),
                                 encounters_block(loc)) if x)
    pans = [s for s in shots if s["id"].endswith("-pan")]
    others = [s for s in shots if not s["id"].endswith("-pan")]
    strips = "\n".join(strip_block(s, pretty) for s in pans)
    return f'''<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{esc(pretty)}</title>
<link rel="stylesheet" href="../style.css"></head>
<body>
<section class="page">
  <div class="sheet">
    <div class="masthead">
      <div class="plaque"><img src="../assets/logo.png" alt="Emerald Champions"></div>
      <div class="kicker"><b>OFFICIAL</b> FIELD GUIDE</div>
    </div>

    <div class="location">
      <h2>{esc(pretty)}</h2>
      <div class="loc-grid">
{grid}
      </div>
    </div>

{strips}
{trainers_block(loc)}
{events_block(others)}
  </div>
  <div class="folio"><div class="n">{folio}</div><div class="ball"></div></div>
</section>
</body>
</html>
'''


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--locations", type=Path, default=ROOT / "guide/data/locations.json")
    ap.add_argument("--book", type=Path, default=ROOT / "guide/data/book.json")
    ap.add_argument("--start-page", type=int, default=18)
    args = ap.parse_args()

    locations = json.loads(args.locations.read_text())
    book = json.loads(args.book.read_text())
    manifest_path = CAPTURES / "manifest.json"
    manifest = json.loads(manifest_path.read_text()) if manifest_path.exists() else []
    by_map = {}
    for shot in manifest:
        if shot.get("ok"):
            by_map.setdefault(shot["map"], []).append(shot)

    OUT.mkdir(parents=True, exist_ok=True)
    missing_plate, missing_shots, written = [], [], []
    folio = args.start_page
    for part in book:
        (OUT / f"{folio:03d}-chapter.html").write_text(
            chapter_page(part["chapter"], part["locations"], folio))
        written.append(f"{folio:03d}-chapter.html")
        folio += 1
        for name in part["locations"]:
            loc = locations.get(name)
            if not loc:
                print(f"  skip {name}: not in the location data")
                continue
            plate = (MAPS / f"{name}.png").exists()
            if not plate:
                missing_plate.append(name)
            shots = by_map.get(name, [])
            if not shots:
                missing_shots.append(name)
            path = OUT / f"{folio:03d}-{name}.html"
            path.write_text(page(loc, shots, folio, plate))
            written.append(path.name)
            folio += 1

    print(f"{len(written)} pages -> {OUT}")
    if missing_plate:
        print(f"  no map plate ({len(missing_plate)}): {', '.join(missing_plate[:8])}"
              + (" …" if len(missing_plate) > 8 else ""))
    if missing_shots:
        print(f"  no captures ({len(missing_shots)}): {', '.join(missing_shots[:8])}"
              + (" …" if len(missing_shots) > 8 else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
