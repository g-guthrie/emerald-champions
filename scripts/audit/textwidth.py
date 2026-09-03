#!/usr/bin/env python3
"""Independent dialogue width check: measure every .string line in Hoenn scripts with the
FONT_NORMAL Latin glyph widths from src/fonts.c and charmap.txt."""
import re,glob,sys
import pathlib
ROOT=str(pathlib.Path(__file__).resolve().parents[2])
src=open(f"{ROOT}/src/fonts.c").read()
m=re.search(r"gFontNormalLatinGlyphWidths\[\] = \{(.*?)\};",src,re.S)
widths=[int(x) for x in re.findall(r"\d+",m.group(1))]
# charmap: 'X' = hex  ; also multi-char tokens
cmap={}
for line in open(f"{ROOT}/charmap.txt",encoding="utf-8"):
    line=line.split("@")[0].strip()
    mm=re.match(r"^'(.*)'\s*=\s*([0-9A-Fa-f ]+)$",line)
    if mm:
        ch=mm.group(1).replace("\\'","'").replace('\\"','"').replace("\\\\","\\")
        codes=mm.group(2).split()
        if len(codes)==1: cmap[ch]=int(codes[0],16)
        continue
    mm=re.match(r"^([A-Z_0-9]+)\s*=\s*([0-9A-Fa-f ]+)$",line)
    if mm:
        codes=mm.group(2).split()
        cmap["{"+mm.group(1)+"}"]=[int(c,16) for c in codes]
def line_width(text):
    w=0; i=0
    while i<len(text):
        if text[i]=="{":
            j=text.index("}",i); tok=text[i:j+1]; i=j+1
            if tok in ("{PLAYER}","{RIVAL}"): w+=7*6  # 7-char name upper bound
            elif tok.startswith("{STR_VAR_"): w+=0
            elif tok in ("{COLOR","{PAUSE"): pass
            else:
                codes=cmap.get(tok)
                if isinstance(codes,list) and len(codes)==1 and codes[0]<len(widths) and not tok.startswith("{COLOR") : w+=widths[codes[0]]
            continue
        if text[i]=="\\": i+=2; continue  # \n \l \p handled by split
        c=text[i]; i+=1
        code=cmap.get(c)
        if isinstance(code,int) and code<len(widths): w+=widths[code]
        else: w+=6
    return w
worst=[]
files=[f for f in glob.glob(f"{ROOT}/data/maps/*/scripts.inc") if "_Frlg" not in f]+glob.glob(f"{ROOT}/data/scripts/*.inc")+glob.glob(f"{ROOT}/data/text/*.inc")
for f in files:
    for n,line in enumerate(open(f,errors="ignore"),1):
        s=line.strip()
        if not s.startswith(".string"): continue
        mm=re.match(r'\.string\s+"(.*)"',s)
        if not mm: continue
        txt=mm.group(1).replace("$","")
        for part in re.split(r"\\n|\\l|\\p",txt):
            # strip control codes like {COLOR X} which take no width
            part=re.sub(r"\{(COLOR|HIGHLIGHT|SHADOW|PAUSE|PAUSE_UNTIL_PRESS|FONT|SIZE|WAIT_SE|PLAY_BGM|PLAY_SE|CLEAR|SKIP|CLEAR_TO|MIN_LETTER_SPACING|RESET_SIZE|PAUSE_MUSIC|RESUME_MUSIC|DYNAMIC|KEYPAD_ICON|RIGHT_ARROW|ESCAPE|SHIFT_TEXT|SHIFT_DOWN|FILL_WINDOW|COLOR_HIGHLIGHT_SHADOW|CIRCLE_HOLLOW)[^}]*\}","",part)
            w=line_width(part)
            if w>208: worst.append((w,f.replace(ROOT+"/",""),n,part))
worst.sort(reverse=True)
print(len(worst),"lines wider than 208px")
for w,f,n,p in worst[:80]:
    if "debug.inc" in f or "STR_VAR" in p: continue
    print(w,f,n,p)
