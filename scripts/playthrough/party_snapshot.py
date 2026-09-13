"""Read-only native checkpoint inspection; never advances the saved primary state.
Member offsets reflect the current engine layouts; audit them when layouts change.
"""
from pathlib import Path
import sys,json,re,shutil,hashlib,struct
root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'scripts'));import render_emerald_champions_ui as ui
out=Path(sys.argv[1]);label=sys.argv[2]
syms={p[-1]:int(p[0],16) for l in ui.run([shutil.which('arm-none-eabi-nm'),'-S',str(out/'scene.elf')]).stdout.splitlines() if len(p:=l.split())>=3 and re.fullmatch('[0-9a-fA-F]+',p[0])}
base=syms['gParties'];cmd=[str(ui.build_runner()),'--rom',str(out/'scene.gba'),'--state-in',str(out/'current.ss1'),'--frames','1']
for i in range(0,600,4):cmd+=['--read',f'4:0x{base+i:x}']
r=ui.run(cmd);v={int(a,16):int(b,16) for a,b in re.findall(r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)',r.stdout)};raw=b''.join(struct.pack('<I',v[base+i])for i in range(0,600,4));d={'sha256':hashlib.sha256(raw).hexdigest(),'raw_hex':raw.hex(),'party':[{'personality':struct.unpack_from('<I',raw,i*100)[0],'level':raw[i*100+84],'hp':struct.unpack_from('<H',raw,i*100+86)[0],'max_hp':struct.unpack_from('<H',raw,i*100+88)[0]}for i in range(6)]};(out/f'{label}-party.json').write_text(json.dumps(d,indent=2)+'\n');print(json.dumps({k:v for k,v in d.items()if k!='raw_hex'}))
