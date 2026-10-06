"""The DexNav info panel's text fits its box.

src/dexnav.c derives the panel's lower lines from DEXNAV_SMALL_LINE and refuses to build when they
overlap. This test ties that constant to the font table and checks every string the panel prints
in those slots: no more lines than the slot has, and no line wider than INFO_TEXT_WIDTH in the
small font's real glyph widths.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEXNAV = (ROOT / 'src/dexnav.c').read_text()


SCREEN = {'DISPLAY_WIDTH': '240', 'DISPLAY_HEIGHT': '160'}  # include/gba/defines.h


def define(name):
    if name in SCREEN:
        return SCREEN[name]
    m = re.search(rf'^#define {name}\s+(.+?)\s*(?://.*)?$', DEXNAV, re.M)
    return m.group(1)


def value(name):
    expr = define(name)
    for ref in sorted(set(re.findall(r'[A-Z_][A-Z0-9_]+', expr)), key=len, reverse=True):
        expr = re.sub(rf'\b{ref}\b', str(value(ref)), expr)
    return eval(expr)


def charmap():
    cm = {}
    for line in (ROOT / 'charmap.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r"^'(.)'\s*=\s*([0-9A-F]{2})\s*$", line)
        if m: cm[m.group(1)] = int(m.group(2), 16)
    return cm


def small_widths():
    src = (ROOT / 'src/fonts.c').read_text()
    body = src.split('gFontSmallLatinGlyphWidths[] = {', 1)[1].split('};', 1)[0]
    return [int(n) for n in re.findall(r'\d+', body)]


KEYPAD = {'A_BUTTON': 8, 'B_BUTTON': 8, 'L_BUTTON': 16, 'R_BUTTON': 16, 'START_BUTTON': 24, 'SELECT_BUTTON': 24}


def line_width(text, cm, widths):
    w = 0
    for tok in re.findall(r'\{[A-Z_0-9]+\}|.', text):
        if tok.startswith('{'):
            w += KEYPAD[tok[1:-1]]
        else:
            w += widths[cm[tok]]
    return w


def panel_strings():
    """name -> text for the strings printed in the panel's how-to-find and hint slots."""
    texts = dict(re.findall(r'static const u8 (sText_DexNav\w+)\[\] = _\("(.*?)"\);', DEXNAV))
    hint_fn = DEXNAV.split('static const u8 *GetDexNavEntryHint(', 1)[1].split('\n}\n', 1)[0]
    names = set(re.findall(r'return (sText_DexNav\w+);', hint_fn))
    names |= {'sText_DexNavNoSearch', 'sText_DexNavWildOnly', 'sText_DexNavHowToFind'}
    return {n: texts[n] for n in sorted(names)}


class DexNavLayout(unittest.TestCase):
    def test_line_height_matches_font(self):
        text_c = (ROOT / 'src/text.c').read_text()
        block = text_c.split('[FONT_SMALL] = {', 1)[1].split('}', 1)[0]
        height = int(re.search(r'\.maxLetterHeight = (\d+)', block).group(1))
        spacing = int(re.search(r'\.lineSpacing = (\d+)', block).group(1))
        self.assertEqual(value('DEXNAV_SMALL_LINE'), height + spacing)

    def test_panel_strings_fit(self):
        cm, widths = charmap(), small_widths()
        limit_w = value('INFO_TEXT_WIDTH')
        limit_lines = min(value('INFO_HINT_LINES'), value('INFO_HOW_LINES'))
        strings = panel_strings()
        self.assertIn('sText_DexNavHintSearch', strings)
        for name, text in strings.items():
            lines = text.split('\\n')
            self.assertLessEqual(len(lines), limit_lines, f'{name} has {len(lines)} lines')
            for line in lines:
                self.assertLessEqual(line_width(line, cm, widths), limit_w, f'{name}: "{line}" is too wide')

    def test_odds_lines_fit_without_a_narrower_font(self):
        """The search-odds lines print in FONT_SMALL itself: at their widest
        (base odds, a half-percent Pokerus chance, a three-digit chain) they fit."""
        cm, widths = charmap(), small_widths()
        limit_w = value('INFO_TEXT_WIDTH')
        texts = dict(re.findall(r'static const u8 (sText_\w+)\[\] = _\("(.*?)"\);', DEXNAV))
        for label, values in (('sText_ShinyChance', ['1/8192', '100%']), ('sText_PokerusChance', ['3/65536', '49.5%', '100%']),
                              ('sText_SearchChain', None)):
            for v in values or ['999']:
                line = texts[label].replace('{STR_VAR_3}', v).replace('{STR_VAR_1}', v)
                self.assertLessEqual(line_width(line, cm, widths), limit_w, f'{label}: "{line}" needs a narrower font')

    def test_hint_keeps_clear_of_the_border(self):
        self.assertGreaterEqual(value('INFO_BOTTOM_PAD'), 3)


if __name__ == '__main__':
    unittest.main()
