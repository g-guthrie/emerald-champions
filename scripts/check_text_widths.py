#!/usr/bin/env python3
"""
Pixel-accurate line-width / box-length checker for Emerald Champions dialogue.

Replaces the old 34-character heuristic (R5 in work/dialogue-survey/findings.json)
with a measurement against the actual FONT_NORMAL glyph width table and the
actual field message box pixel width, both read straight out of the game
source instead of guessed.

Facts this script is built on (see report.md for the full citation list):

  * The overworld/field message box ("MSGBOX") is drawn with the window
    template `sStandardTextBox_WindowTemplates[0]` in src/menu.c:
        .tilemapLeft = 2, .tilemapTop = 15, .width = 27, .height = 4
    i.e. 27 tiles wide. DrawDialogueFrame()'s border tiles are all drawn in
    tilemap columns *outside* that 27-tile span (left-2, left-1, left+width),
    so none of the frame overlaps the printable interior -> the full
    27 tiles x 8px = 216px is usable text width.

  * AddTextPrinterForMessage() (src/menu.c) calls
        AddTextPrinterParameterized2(0, FONT_NORMAL, gStringVar4, ...)
    with printer.x = 0 (src/menu.c ~line 178), so there is no left margin
    to subtract: the usable pixel budget is exactly 216px, not 208.

  * FONT_NORMAL's per-glyph pixel widths are gFontNormalLatinGlyphWidths[]
    in src/fonts.c, indexed directly by the charmap byte value (confirmed by
    GetGlyphWidth_Normal() / DecompressGlyph_Normal() in src/text.c).

  * Every font in sFontInfos[] (src/text.c) has letterSpacing = 0, so pixel
    width is a plain sum of glyph widths with no per-character padding.

  * Scripts can switch fonts mid-string with {FONT_...} control codes
    (EXT_CTRL_CODE_FONT, charmap byte FC 06 <fontId>). Of the fonts that
    actually appear in data/maps/*/scripts.inc and data/scripts/*.inc, only
    {FONT_NORMAL} (id 1) and {FONT_SMALL_NARROWER} (id 11) show up ({FONT_MALE}
    and {FONT_FEMALE} are '#define'd to FONT_NORMAL in include/text.h and
    don't change the table). This script tracks that state per box.

  * {PLAYER}/{RIVAL}/{STR_VAR_n} are runtime placeholders with no fixed text,
    so per the task spec they're measured as a stand-in name of average
    per-glyph width: 7 characters for {PLAYER}/{RIVAL}, 10 for {STR_VAR_n}.
  * {KUN} expands to an empty string in this project (src/strings.c:
    gText_ExpandedPlaceholder_Kun/_Chan are both `_("")`) -> 0px.
  * {REGION} expands to the literal "HOENN" or "KANTO" (src/strings.c) -> both
    5 letters, so it's measured as the real string "HOENN".
  * All other charmap tokens that are NOT runtime placeholders (PKMN,
    POKEBLOCK, arrows, suit/button icons, {PLAY_BGM}/{MUS_*}/{PAUSE_MUSIC}/
    {RESUME_MUSIC} control codes, etc.) resolve to fixed byte sequences
    defined right there in charmap.txt, so they're measured exactly via the
    glyph table instead of guessed.
"""
import glob
import json
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHARMAP_PATH = os.path.join(ROOT, "charmap.txt")
FONTS_C_PATH = os.path.join(ROOT, "src", "fonts.c")
OUT_PATH = os.path.join(ROOT, "work", "dialogue-survey", "width-findings.json")

# --- box geometry (see module docstring for citations) ---------------------
BOX_WIDTH_TILES = 27          # src/menu.c sStandardTextBox_WindowTemplates[0].width
TILE_PX = 8
BOX_WIDTH_PX = BOX_WIDTH_TILES * TILE_PX   # 216
MAX_LINES_PER_BOX = 2          # >2 lines before \p is flagged (matches old R5 rule)
FONT_MEASURED = "FONT_NORMAL (gFontNormalLatinGlyphWidths, src/fonts.c:148), " \
                "216px box width (27 tiles x 8px, src/menu.c sStandardTextBox_WindowTemplates, " \
                "printer.x=0), letterSpacing=0 for all fonts (src/text.c sFontInfos[])"

CHAR_KEYPAD_ICON = 0xF8
CHAR_EXTRA_SYMBOL = 0xF9

# Hardcoded from src/text.c sKeypadIcons[] (rare in dialogue; button-icon glyphs).
KEYPAD_ICON_WIDTHS = {
    0x00: 8, 0x01: 8, 0x02: 16, 0x04: 16, 0x06: 24, 0x09: 24,
    0x0C: 8, 0x0D: 8, 0x0E: 8, 0x0F: 8, 0x20: 8, 0x21: 8, 0x22: 8,
}

# Runtime (FD-prefixed) placeholders with special, spec-driven handling.
# Everything else FD-prefixed and not listed here is "unmeasurable" (logged).
DYNAMIC_PLACEHOLDER_KIND = {
    "PLAYER": ("avg_chars", 7),
    "RIVAL": ("avg_chars", 7),
    "STR_VAR_1": ("avg_chars", 10),
    "STR_VAR_2": ("avg_chars", 10),
    "STR_VAR_3": ("avg_chars", 10),
    "KUN": ("literal", ""),       # gText_ExpandedPlaceholder_Kun/_Chan = _("")
    "REGION": ("literal", "HOENN"),  # gText_Hoenn / gText_Kanto, both 5 letters
}

FONT_SWITCH_TABLE = {
    "FONT_NORMAL": "NORMAL",
    "FONT_MALE": "NORMAL",     # #define FONT_MALE FONT_NORMAL (include/text.h)
    "FONT_FEMALE": "NORMAL",   # #define FONT_FEMALE FONT_NORMAL
    "FONT_SMALL_NARROWER": "SMALL_NARROWER",
}

# Control-code-ish placeholders that render nothing (0px) beyond what
# FONT_SWITCH_TABLE already zero-costs.
ZERO_WIDTH_PREFIXES = ("MUS_", "SE_")
ZERO_WIDTH_NAMES = {"PLAY_BGM", "PAUSE_MUSIC", "RESUME_MUSIC"}


def parse_charmap(path):
    """Return (char_to_bytes, name_to_bytes) from charmap.txt."""
    char_to_bytes = {}
    name_to_bytes = {}
    char_re = re.compile(r"^'((?:\\.|[^'\\])*)'\s*=\s*([0-9A-Fa-f ]+)")
    name_re = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*([0-9A-Fa-f ]+)")
    simple_escapes = {"\\'": "'", "\\\\": "\\"}
    skip_escapes = {"\\n", "\\l", "\\p"}  # handled separately as line/box breaks

    with open(path, encoding="utf-8") as f:
        for raw_line in f:
            line = raw_line.split("@", 1)[0].rstrip("\n")
            if not line.strip():
                continue
            m = char_re.match(line)
            if m:
                content, hexbytes = m.group(1), m.group(2)
                if content in skip_escapes:
                    continue
                if content.startswith("\\"):
                    ch = simple_escapes.get(content)
                    if ch is None:
                        continue
                else:
                    ch = content
                if len(ch) == 1:
                    try:
                        char_to_bytes[ch] = [int(t, 16) for t in hexbytes.split()]
                    except ValueError:
                        pass
                continue
            m = name_re.match(line)
            if m:
                name, hexbytes = m.group(1), m.group(2)
                try:
                    name_to_bytes[name] = [int(t, 16) for t in hexbytes.split()]
                except ValueError:
                    pass
    return char_to_bytes, name_to_bytes


def parse_glyph_width_table(path, array_name):
    text = open(path, encoding="utf-8").read()
    m = re.search(re.escape(array_name) + r"\[\]\s*=\s*\{(.*?)\};", text, re.S)
    if not m:
        raise RuntimeError(f"could not find {array_name} in {path}")
    nums = [int(x) for x in re.findall(r"-?\d+", m.group(1))]
    return nums


def build_font_tables():
    normal = parse_glyph_width_table(FONTS_C_PATH, "gFontNormalLatinGlyphWidths")
    small_narrower = parse_glyph_width_table(FONTS_C_PATH, "gFontSmallNarrowerLatinGlyphWidths")
    return {"NORMAL": normal, "SMALL_NARROWER": small_narrower}


def avg_letter_width(table):
    letters = [chr(c) for c in range(ord('A'), ord('Z') + 1)] + \
              [chr(c) for c in range(ord('a'), ord('z') + 1)]
    widths = []
    for ch in letters:
        b = CHAR_TO_BYTES.get(ch)
        if b and all(x < len(table) for x in b):
            widths.append(sum(table[x] for x in b))
    return sum(widths) / len(widths) if widths else 6.0


def literal_width(s, table):
    total = 0
    for ch in s:
        b = CHAR_TO_BYTES.get(ch)
        if not b:
            continue
        total += sum(table[x] if x < len(table) else 8 for x in b)
    return total


def bytes_width(byte_list, table):
    """Width of a raw byte sequence (as stored for a static charmap token),
    honoring the CHAR_EXTRA_SYMBOL / CHAR_KEYPAD_ICON two-byte icon forms."""
    total = 0
    i = 0
    n = len(byte_list)
    while i < n:
        b = byte_list[i]
        if b == CHAR_EXTRA_SYMBOL and i + 1 < n:
            idx = 0x100 + byte_list[i + 1]
            total += table[idx] if idx < len(table) else 8
            i += 2
        elif b == CHAR_KEYPAD_ICON and i + 1 < n:
            total += KEYPAD_ICON_WIDTHS.get(byte_list[i + 1], 8)
            i += 2
        elif b == 0xFC:
            # extended control code sequence with no visible glyph
            i = n  # the rest of this token is control-code payload; stop
        else:
            total += table[b] if b < len(table) else 8
            i += 1
    return total


PLACEHOLDER_RE = re.compile(r"\{([A-Za-z0-9_]+)\}")


def measure_line(line, font_state, tables, name_to_bytes, avg_widths):
    """Measure one already-split (no \\n \\l \\p) line of dialogue text.

    Returns (width_px, unknown_tokens: list[str], new_font_state)
    """
    width = 0
    unknown = []
    pos = 0
    for m in PLACEHOLDER_RE.finditer(line):
        # literal text before the placeholder
        literal = line[pos:m.start()]
        for ch in literal:
            b = CHAR_TO_BYTES.get(ch)
            table = tables[font_state]
            if not b:
                unknown.append(f"char {ch!r}")
                continue
            width += sum(table[x] if x < len(table) else 8 for x in b)

        name = m.group(1)
        table = tables[font_state]
        if name in FONT_SWITCH_TABLE:
            font_state = FONT_SWITCH_TABLE[name]
            # zero visible width, no glyphs
        elif name in DYNAMIC_PLACEHOLDER_KIND:
            kind, val = DYNAMIC_PLACEHOLDER_KIND[name]
            if kind == "avg_chars":
                width += round(avg_widths[font_state] * val)
            else:  # literal
                width += literal_width(val, table)
        elif name.startswith("FD") or name in (
            "VERSION", "RIVAL_NAME", "AQUA", "MAGMA", "ARCHIE", "MAXIE",
            "KYOGRE", "GROUDON",
        ) or name.startswith("B_PLAYER") or name.startswith("B_LINK"):
            # dynamic runtime placeholder with no fixed text and not covered
            # by the task's spec -> can't be measured, flag it.
            unknown.append("{" + name + "}")
        elif name in ZERO_WIDTH_NAMES or any(name.startswith(p) for p in ZERO_WIDTH_PREFIXES):
            pass  # control code / sound cue, 0 visible width
        elif name in name_to_bytes:
            width += bytes_width(name_to_bytes[name], table)
        else:
            unknown.append("{" + name + "}")
        pos = m.end()

    literal = line[pos:]
    table = tables[font_state]
    for ch in literal:
        b = CHAR_TO_BYTES.get(ch)
        if not b:
            unknown.append(f"char {ch!r}")
            continue
        width += sum(table[x] if x < len(table) else 8 for x in b)

    return width, unknown, font_state


# ---------------------------------------------------------------------------
# File discovery (mirrors the original dialogue survey's scope notes)
# ---------------------------------------------------------------------------

BATTLE_FRONTIER_INTERIOR_RE = re.compile(
    r"^BattleFrontier_(BattlePike|BattleArena|BattleTower|BattleFactory|"
    r"BattleDome|BattlePalace|BattlePyramid)(Corridor|Lobby|BattleRoom|"
    r"PreBattleRoom|Elevator|MultiCorridor|MultiBattleRoom|MultiPartnerRoom|"
    r"RoomFinal|RoomNormal|RoomWildMons|ThreePathRoom|Floor|Top)$"
)
BATTLE_PYRAMID_SQUARE_RE = re.compile(r"^BattlePyramidSquare\d+$")


def discover_files():
    files = []

    for path in sorted(glob.glob(os.path.join(ROOT, "data", "maps", "*", "scripts.inc"))):
        map_name = os.path.basename(os.path.dirname(path))
        if BATTLE_FRONTIER_INTERIOR_RE.match(map_name):
            continue
        if BATTLE_PYRAMID_SQUARE_RE.match(map_name):
            continue
        files.append(path)

    for path in sorted(glob.glob(os.path.join(ROOT, "data", "scripts", "*.inc"))):
        files.append(path)

    return files


# ---------------------------------------------------------------------------
# .string block extraction
# ---------------------------------------------------------------------------

LABEL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)::?\s*(?:@.*)?$")
STRING_RE = re.compile(r'^\s*\.string\s+"(.*)"\s*(?:@.*)?$')
COMMENT_OR_BLANK_RE = re.compile(r"^\s*(@.*)?$")


def extract_string_blocks(path):
    """Yield (label, first_line_no, full_text) for each label's .string block."""
    with open(path, encoding="utf-8", errors="replace") as f:
        lines = f.readlines()

    current_label = None
    buf = []
    first_line = None

    def flush():
        if current_label is not None and buf:
            yield_item = (current_label, first_line, "".join(buf))
            return yield_item
        return None

    results = []
    for i, raw in enumerate(lines, start=1):
        line = raw.rstrip("\n")
        lm = LABEL_RE.match(line)
        if lm:
            item = flush()
            if item:
                results.append(item)
            current_label = lm.group(1)
            buf = []
            first_line = None
            continue
        sm = STRING_RE.match(line)
        if sm:
            if first_line is None:
                first_line = i
            buf.append(sm.group(1))
            continue
        if COMMENT_OR_BLANK_RE.match(line):
            continue
        # any other directive/content ends the current block
        item = flush()
        if item:
            results.append(item)
        current_label = None
        buf = []
        first_line = None

    item = flush()
    if item:
        results.append(item)
    return results


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    global CHAR_TO_BYTES
    CHAR_TO_BYTES, name_to_bytes = parse_charmap(CHARMAP_PATH)
    tables = build_font_tables()
    avg_widths = {state: avg_letter_width(tbl) for state, tbl in tables.items()}

    files = discover_files()

    findings = []
    unmeasurable = []
    per_file_counts = defaultdict(int)
    total_overflow_lines = 0
    total_overflow_boxes = 0
    total_flagged_labels = 0

    for path in files:
        rel = os.path.relpath(path, ROOT)
        blocks = extract_string_blocks(path)
        for label, line_no, text in blocks:
            if text.endswith("$"):
                text = text[:-1]

            boxes = text.split("\\p")
            font_state = "NORMAL"  # reset per label: fresh AddTextPrinterForMessage call
            label_hits = []
            label_unknown = set()

            for box_idx, box in enumerate(boxes):
                if box_idx == len(boxes) - 1 and box == "":
                    continue  # trailing empty box from a terminal \p
                raw_lines = re.split(r"\\n|\\l", box)
                if len(raw_lines) > MAX_LINES_PER_BOX + 1:
                    label_hits.append({
                        "kind": "box_too_many_lines",
                        "lines": len(raw_lines),
                        "box_index": box_idx,
                        "text": box[:80],
                    })
                for ln in raw_lines:
                    if ln == "":
                        continue
                    w, unk, font_state = measure_line(ln, font_state, tables, name_to_bytes, avg_widths)
                    if unk:
                        label_unknown.update(unk)
                    elif w > BOX_WIDTH_PX:
                        label_hits.append({
                            "kind": "line_too_wide",
                            "width_px": w,
                            "over_by_px": w - BOX_WIDTH_PX,
                            "box_index": box_idx,
                            "text": ln,
                        })

            if label_unknown:
                unmeasurable.append({
                    "label": label,
                    "file": rel,
                    "line": line_no,
                    "unknown": sorted(label_unknown),
                })

            if label_hits:
                total_flagged_labels += 1
                per_file_counts[rel] += 1
                n_lines = sum(1 for h in label_hits if h["kind"] == "line_too_wide")
                n_boxes = sum(1 for h in label_hits if h["kind"] == "box_too_many_lines")
                total_overflow_lines += n_lines
                total_overflow_boxes += n_boxes
                findings.append({
                    "label": label,
                    "file": rel,
                    "line": line_no,
                    "n_line_hits": n_lines,
                    "n_box_hits": n_boxes,
                    "hits": label_hits,
                })

    findings.sort(key=lambda f: (f["n_line_hits"] + f["n_box_hits"]), reverse=True)
    worst40 = findings[:40]

    per_file_sorted = sorted(per_file_counts.items(), key=lambda kv: kv[1], reverse=True)

    out = {
        "box_width_px": BOX_WIDTH_PX,
        "box_width_tiles": BOX_WIDTH_TILES,
        "font_measured_against": FONT_MEASURED,
        "max_lines_per_box_before_p": MAX_LINES_PER_BOX,
        "files_scanned": len(files),
        "labels_flagged": total_flagged_labels,
        "total_overflowing_lines": total_overflow_lines,
        "total_oversized_boxes": total_overflow_boxes,
        "labels_with_unmeasurable_placeholders": len(unmeasurable),
        "per_file_counts": per_file_sorted,
        "worst_40_labels": worst40,
        "all_findings": findings,
        "unmeasurable": unmeasurable,
    }

    os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
    with open(OUT_PATH, "w", encoding="utf-8") as f:
        json.dump(out, f, indent=2, ensure_ascii=False)

    print(f"Scanned {len(files)} files")
    print(f"Box width measured: {BOX_WIDTH_PX}px ({BOX_WIDTH_TILES} tiles x {TILE_PX}px), FONT_NORMAL")
    print(f"Labels flagged: {total_flagged_labels}")
    print(f"  overflowing lines: {total_overflow_lines}")
    print(f"  oversized boxes (>{MAX_LINES_PER_BOX} lines before \\p): {total_overflow_boxes}")
    print(f"Labels with unmeasurable placeholders (excluded from overflow calc): {len(unmeasurable)}")
    print()
    print("Top 15 files by flagged labels:")
    for rel, cnt in per_file_sorted[:15]:
        print(f"  {cnt:4d}  {rel}")
    print()
    print("Worst 10 labels:")
    for f in worst40[:10]:
        print(f"  {f['n_line_hits'] + f['n_box_hits']:2d} hits  {f['label']}  ({f['file']}:{f['line']})")


if __name__ == "__main__":
    sys.exit(main())
