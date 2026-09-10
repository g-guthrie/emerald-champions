from pathlib import Path
import re,json
root=Path.cwd();font=(root/'src/fonts.c').read_text();widths=[int(x) for x in re.findall(r'\d+',re.search(r'gFontNormalLatinGlyphWidths\[\] = \{(.*?)\};',font,re.S)[1])];cm={}
for l in (root/'charmap.txt').read_text().splitlines():
 m=re.match(r"^'(.*)'\s*=\s*([0-9A-Fa-f]+)\s*(?:@.*)?$",l)
 if m:cm[m[1]]=int(m[2],16)
def width(t):return sum(widths[cm[c]] if c in cm and cm[c]<len(widths) else 6 for c in t)
words=[]
for p in (root/'src/data/easy_chat').glob('easy_chat_group_*.h'):
 words+=re.findall(r'\.text = COMPOUND_STRING\("([^"\n]*)"\)',p.read_text())
moves=(root/'src/data/moves_info.h').read_text()
for p in (root/'src/data/easy_chat').glob('easy_chat_group_move_*.h'):
 for move in re.findall(r'\bMOVE_\w+',p.read_text()):
  m=re.search(r'\['+move+r'\]\s*=\s*\{.*?\.name = COMPOUND_STRING\("([^"\n]*)"\)',moves,re.S);assert m,move;words.append(m[1].upper())
# Species names are shorter than the widest ordinary phrase token; include
# all species names as a conservative superset of the two species groups.
for p in (root/'src/data/pokemon/species_info').glob('*.h'):words+=re.findall(r'\.speciesName = _\("([^"\n]*)"\)',p.read_text())
words=[w.upper() for w in words if '{' not in w];longest=max(words,key=width);phrase=longest+' '+longest
print('widest token',longest,width(longest),'phrase',width(phrase))
rows=[]
for name in ['DewfordTown','DewfordTown_Hall']:
 p=root/'data/maps'/name/'scripts.inc';s=p.read_text()
 for m in re.finditer(r'^(\w+):\n((?:\s*\.string[^\n]*\n)+)',s,re.M):
  t=''.join(re.findall(r'\.string "(.*)"',m[2]));
  if '{STR_VAR' not in t:continue
  if any(x in m[1] for x in ['TradeTropius','TradeComplete','NeedRequestedMon']):continue
  for l in re.split(r'\\[nlp]|\$',t):
   v=l.replace('{STR_VAR_1}',phrase).replace('{STR_VAR_2}',phrase).replace('{PLAYER}','WWWWWWW')
   rows.append({'file':str(p.relative_to(root)),'label':m[1],'text':l,'width':width(v)})
(root/'work/dialogue-dewford-20260910/trend-widths.json').write_text(json.dumps(rows,indent=2)+'\n')
for r in rows:
 if r['width']>216:print(r)
