import sys,struct,concurrent.futures
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy

def leave(suffix):
 execute('southern-'+suffix+'-outside','southern-latios-'+suffix,2400,[(400,96,'DOWN')],shots=[1800])
def sailor():
 p=bytes([0x39,26,9,255])+struct.pack('<HH',13,22)+bytes([0x27,0x6b,2]);a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext']
 w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0)]
 execute('southern-return-decline','southern-latios-capture',4500,[(700,2,'DOWN'),(900,2,'A'),(1800,2,'A'),(2500,2,'DOWN'),(2800,2,'A'),(3500,2,'B')],w,[1700,2400,3200])
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(leave,['capture','faint']))
sailor()
