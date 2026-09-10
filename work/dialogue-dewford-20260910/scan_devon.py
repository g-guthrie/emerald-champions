from pathlib import Path
import re,json
ROOT=Path(__file__).resolve().parents[2]
src=(ROOT/'src/fonts.c').read_text()
widths=[int(x) for x in re.findall(r'\d+',re.search(r'gFontNormalLatinGlyphWidths\[\] = \{(.*?)\};',src,re.S)[1])]
cmap={}
for line in (ROOT/'charmap.txt').read_text().splitlines():
 m=re.match(r"^'(.*)'\s*=\s*([0-9A-Fa-f]+)\s*(?:@.*)?$",line)
 if m:cmap[m[1]]=int(m[2],16)
def width(t):
 return sum(widths[cmap[c]] if c in cmap and cmap[c]<len(widths) else 6 for c in t)
paths=[ROOT/'data/maps'/('RustboroCity_DevonCorp_'+floor)/'scripts.inc' for floor in ['1F','2F','3F']]
rows=[]
for p in paths:
 s=p.read_text()
 for m in re.finditer(r'^(\w+):{1,2}\s*\n((?:\s*\.string[^\n]*\n)+)',s,re.M):
  text=''.join(re.findall(r'\.string\s+"(.*)"',m[2]))
  row={'file':str(p.relative_to(ROOT)), 'label':m[1], 'line':s[:m.start()].count('\n')+1,'text':text,'lines':[]}
  for t in re.split(r'\\[nlp]|\$',text):
   if not t:continue
   unknown=[]
   def expand(k):
    name=k[1]
    if name in ['PLAYER','RIVAL']:return 'W'*7
    if name=='KUN':return ''
    if name=='REGION':return 'HOENN'
    if name.endswith('ARROW'):return 'W'
    if name.startswith('STR_VAR'):
     if 'EeveeResearcher' in m[1]:return 'W'*12
     if 'Fossil' in m[1] or 'ReceivedMonFromResearcher' in m[1]:return 'W'*13
     if 'Flat1_2F' in m[1]:return 'W'*15
     if 'House1_Text' in m[1]:return 'W'*12
     if 'PetalburgCity_PokemonCenter' in m[1]:return 'W'*12
     if 'StarterRegionChosen' in m[1]:return 'Sinnoh'
     if 'OhYoureTheNewNeighbor' in m[1]:return 'daughter'
     if any(x in m[1] for x in ['YourTwoPartners','NameOpeningPartner']):return 'W'*10
     if 'NameRatersHouse_Text' in m[1]:return 'W'*12
     if 'MoveDeletersHouse_Text' in m[1]:return 'W'*(12 if name=='STR_VAR_1' else 16)
     if 'WhichXmove' in m[1]:return 'move'
     if 'Nature' in m[1] and name=='STR_VAR_2':return 'W'*7
     if m[1].startswith('EmeraldChampions_Text_'):
      return 'W'*(12 if name=='STR_VAR_1' else 20)
     unknown.append(name);return '' 
    unknown.append(name);return ''
   expanded=re.sub(r'\{([^}]+)\}',expand,t)
   row['lines'].append({'text':t,'width':width(expanded),'unresolved':unknown})
  rows.append(row)
(ROOT/'work/dialogue-dewford-20260910/devon-text-layout.json').write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
issues=[]
for r in rows:
 for l in r['lines']:
  if l['width']>200:issues.append({'file':r['file'],'label':r['label'],'width':l['width'],'text':l['text']})
print('blocks',len(rows),'lines',sum(len(x['lines']) for x in rows),'over200',len(issues))
for r in issues:print(json.dumps(r,ensure_ascii=False))
