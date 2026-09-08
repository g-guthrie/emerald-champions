#!/usr/bin/env python3
"""Assemble the main reading edition without duplicating the detailed catalogues."""
from pathlib import Path
import posixpath
import re

B = Path(__file__).resolve().parents[1]
sources = [B / 'DECISIONS.md', *sorted((B / 'chapters').glob('*.md'))]
parts = [
    '# Emerald Champions — Cohesion and Rearchitecture Book',
    '',
    '**Complete proposed implementation specification · September 8, 2026.**',
    '',
    'This reading edition combines the decision index and all ten main chapters. '
    'The [full index](README.md) links every battle, map, encounter table, species/form and Mega Stone review. '
    'The live game has not been reimplemented or playtested as part of writing this book.',
    '',
    '## Contents',
    '',
]
for p in sources:
    title = p.read_text().splitlines()[0].removeprefix('# ')
    parts.append(f'- [{title}]({p.relative_to(B).as_posix()})')

def move_link(match, directory):
    target = match[2]
    if re.match(r'^(?:[a-z][a-z0-9+.-]*:|/|#)', target.strip('<>'), re.I):
        return match[0]
    target = posixpath.normpath(posixpath.join(directory, target))
    return match[1] + target + ')'

for p in sources:
    content = p.read_text().rstrip()
    directory = p.parent.relative_to(B).as_posix()
    # Local book/source paths retain their line suffixes and anchors.
    content = re.sub(r'(\[[^\]]+\]\()([^)]+)\)', lambda m: move_link(m, directory), content)
    chunks = re.split(r'(```[\s\S]*?```)', content)
    for i in range(0, len(chunks), 2):
        chunks[i] = re.sub(r'(?m)^(#{1,5}) ', r'#\1 ', chunks[i])
    parts.extend(['', '---', '', ''.join(chunks)])

(B / 'BOOK.md').write_text('\n'.join(parts) + '\n')
print(f'Compiled {len(sources)} source documents into BOOK.md.')
