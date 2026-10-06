"""The PokeNav map's info box fits every place and landmark name.

src/pokenav_region_map.c prints the box's lines with PrintInfoLine, which picks FONT_NARROW or, for
a name too wide, FONT_NARROWER (GetFontIdToFit). This test measures every map section name, every
landmark name and every place-choice label with the fonts' real glyph widths: each must fit
INFO_TEXT_WIDTH in one of those fonts, so none can run past the box's frame.
"""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REGION_MAP = (ROOT / 'src/pokenav_region_map.c').read_text()


def define(name):
    return re.search(rf'^#define {name}\s+(.+?)\s*(?://.*)?$', REGION_MAP, re.M).group(1)


def value(name):
    expr = define(name)
    for ref in sorted(set(re.findall(r'[A-Z_][A-Z0-9_]+', expr)), key=len, reverse=True):
        expr = re.sub(rf'\b{ref}\b', str(value(ref)), expr)
    return eval(expr)


def charmap():
    cm = {}
    for line in (ROOT / 'charmap.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r"^'(\\?.)'\s*=\s*([0-9A-F]{2})\s*(?:@.*)?$", line)
        if m:
            cm[m.group(1)[-1]] = int(m.group(2), 16)
    return cm


def widths(table):
    src = (ROOT / 'src/fonts.c').read_text()
    body = src.split(f'{table}[] = {{', 1)[1].split('};', 1)[0]
    return [int(n) for n in re.findall(r'\d+', body)]


def text_width(text, cm, table):
    return sum(table[cm[ch]] for ch in text)


def names():
    secs = json.loads((ROOT / 'src/data/region_map/region_map_sections.json').read_text())['map_sections']
    found = {('map section', s['name']) for s in secs if s.get('name')}
    landmarks = (ROOT / 'src/landmark.c').read_text()
    found |= {('landmark', n) for n in re.findall(r'struct Landmark Landmark_\w+ = \{COMPOUND_STRING\("(.*?)"\)', landmarks)}
    choices = REGION_MAP.split('sPlaceChoiceTexts[PLACE_CHOICE_COUNT] =', 1)[1].split('};', 1)[0]
    found |= {('place choice', n) for n in re.findall(r'COMPOUND_STRING\("(.*?)"\)', choices)}
    return found


class PokenavLayout(unittest.TestCase):
    def test_every_name_fits_the_info_box(self):
        cm = charmap()
        narrow, narrower = widths('gFontNarrowLatinGlyphWidths'), widths('gFontNarrowerLatinGlyphWidths')
        room = value('INFO_TEXT_WIDTH')
        cursor = 8  # place choices sit after the menu cursor
        found = names()
        self.assertTrue(any(kind == 'landmark' for kind, _ in found))
        for kind, name in sorted(found):
            limit = room - cursor if kind == 'place choice' else room
            fits = min(text_width(name, cm, narrow), text_width(name, cm, narrower))
            self.assertLessEqual(fits, limit, f'{kind} "{name}" is wider than the PokeNav info box')


if __name__ == '__main__':
    unittest.main()
