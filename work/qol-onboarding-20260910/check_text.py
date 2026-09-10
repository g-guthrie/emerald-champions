from pathlib import Path
import re,json
root=Path.cwd();changes=json.loads((root/'work/qol-onboarding-20260910/changed.json').read_text())
s=(root/'src/fonts.c').read_text();w=[int(x) for x in re.findall(r'\d+',re.search(r'gFontNormalLatinGlyphWidths\[\] = \{(.*?)\};',s,re.S)[1])]
cmap={}
for line in (root/'charmap.txt').read_text().splitlines():
 m=re.match(r"^'(.*)'\s*=\s*([0-9A-Fa-f]+)\s*(?:@.*)?$",line)
 if m:cmap[m[1]]=int(m[2],16)
rows=[]
for row in changes:
 s=(root/row['file']).read_text();m=re.search(r'^'+row['label']+r':{1,2}\n((?:\s*\.string[^\n]*\n)+)',s,re.M);t=''.join(re.findall(r'\.string "(.*)"',m[1])); widths=[]
 for line in re.split(r'\\[nlp]|\$',t):
  line=re.sub(r'\{(?:PLAYER|RIVAL)\}','WWWWWWW',line)
  # Extended symbols occupy at most one 8-pixel glyph cell.
  syms=re.findall(r'\{[^}]+\}',line);line=re.sub(r'\{[^}]+\}','',line)
  widths.append(sum(w[cmap[c]] if c in cmap and cmap[c]<len(w) else 6 for c in line)+8*len(syms))
 row['max_width']=max(widths);row['pages']=t.count(r'\p')+1;row['lines']=len(widths)-1
 assert max(widths)<=216,(row,widths)
 for page in t.split(r'\p'):assert page.count(r'\n')<=1,row
 rows.append(row)
(root/'work/qol-onboarding-20260910/text-layout.json').write_text(json.dumps(rows,indent=2)+'\n')
print(len(rows),'blocks;',sum(x['lines'] for x in rows),'lines; max',max(x['max_width'] for x in rows),'pixels; PASS')
