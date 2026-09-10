from pathlib import Path
import sys,subprocess,struct
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';label=sys.argv[1];binary=sys.argv[2];s=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True);ds=symbols(root/(binary+'.elf'),root,first=True);rom=(root/(binary+'.gba')).read_bytes();i=ds['SealedChamber_InnerRoom_Braille_Regigigas']-0x08000000;end=rom.index(b'\xff',i+6)+1;msg=rom[i:end];base=s['gStringVar4'];msgaddr=base+512;scriptaddr=base+900
# Invoke the existing native braille renderer with the actual compiled inscription.
# Scratch input and bytecode fit in unused tail space of the 1000-byte text buffer.
program=bytes([0x78])+struct.pack('<I',msgaddr)+bytes([0x6d,0xfa,0x02])
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'ice-upstairs.mgba'),'--frames','200','--screenshot',str(out/('regigigas-braille-'+label+'.png'))]
for addr,b in [(msgaddr,msg),(scriptaddr,program)]:
 for j,v in enumerate(b):c+=['--write',f'50:1:{addr+j}:{v}']
ctx=s['sGlobalScriptContext'];c+=['--write',f'50:1:{ctx}:0','--write',f'50:4:{ctx+8}:{scriptaddr}','--write',f'50:1:{ctx+1}:1','--write',f'50:1:{s["sGlobalScriptContextStatus"]}:0']
r=subprocess.run(c,capture_output=True,text=True);print(label,'compiled bytes',len(msg),r.returncode,r.stdout,r.stderr);raise SystemExit(r.returncode)
