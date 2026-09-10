import sys,struct,re,json,subprocess
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,out
states=['circuit-entry-cancel-ready','circuit-first-win','circuit-start','circuit-second-win-retire']
data={}
for state in states:
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','1']
 for i in range(0,600,4):c+=['--read',f'4:{sy["gParties"]+i}']
 r=subprocess.run(c,capture_output=True,text=True,check=True);raw=b''.join(struct.pack('<I',int(v,16)) for v in re.findall('value=([0-9a-f]+)',r.stdout));assert len(raw)==600
 data[state]={'party_hex':raw.hex(),'levels':[raw[100*i+84] for i in range(6)],'hp':[struct.unpack_from('<HH',raw,100*i+86) for i in range(6)]}
 print(state,'levels',data[state]['levels'],'hp',data[state]['hp'],flush=True)
 for label,kind,qid in [('active',2,0x40e5),('current',2,0x40db),('best',2,0x40e3),('total',2,0x40dc),('BP',11,0),('CalyrexEligible',6,sy['SPECIES_CALYREX'])]:
  s=execute(state+'-'+label+'-read',state,30,writes=[(1,4,sy['gEcHeadlessCampaignQueryKind'],kind),(1,4,sy['gEcHeadlessCampaignQueryId'],qid)])
  data[state][label]=int(re.search('value=([0-9a-f]+)',s)[1],16)
 print('STATE',state,{k:v for k,v in data[state].items() if k!='party_hex'},flush=True)
for state in ['circuit-start','circuit-second-win-retire']:
 b=bytes.fromhex(data[states[0]]['party_hex']);a=bytes.fromhex(data[state]['party_hex']);print('RESTORED',state,'exact party bytes',a==b,'changed offsets',[i for i in range(600) if a[i]!=b[i]],flush=True)
(out/'circuit-state-inspection.json').write_text(json.dumps(data,indent=2)+'\n')
