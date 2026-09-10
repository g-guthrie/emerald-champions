import sys,json
from pathlib import Path
sys.path.insert(0,'work/dialogue-slateport-20260910')
from walk_island_maps import walk,execute,position,path_in,sy

def pickup(name,state,item,xy,tag):
 start=tuple(position(state)[1:]); choices=[]
 for dx,dy,face in [(0,1,'UP'),(0,-1,'DOWN'),(-1,0,'RIGHT'),(1,0,'LEFT')]:
  target=(xy[0]+dx,xy[1]+dy)
  try: p=path_in(name,start,target)
  except RuntimeError: continue
  choices.append((len(p),target,face))
 _,target,face=min(choices)
 state=walk(name,state,tag+'-approach',target=target)
 query=[(50,4,sy['gEcHeadlessCampaignQueryKind'],4),(50,4,sy['gEcHeadlessCampaignQueryId'],sy[item])]
 execute(tag+'-before',state,100,writes=query)
 execute(tag,state,3000,[(200,2,face),(400,2,'A')]+[(f,2,'B') for f in range(1000,2800,400)],writes=query,shots=[850,1200])
 execute(tag+'-repeat',tag,2300,[(200,2,'A')]+[(f,2,'B') for f in range(900,2100,400)],writes=query)
 return tag+'-repeat'

if __name__=='__main__':
 name='ArtisanCave_B1F';s='artisan-traverse-ready'
 for item,xy,tag in [('ITEM_PEARL_STRING',(19,43),'artisan-pearl-string'),('ITEM_COMET_SHARD',(32,38),'artisan-comet-shard'),('ITEM_NUGGET',(32,29),'artisan-nugget'),('ITEM_BIG_PEARL',(7,5),'artisan-big-pearl'),('ITEM_STAR_PIECE',(27,8),'artisan-star-piece')]: s=pickup(name,s,item,xy,tag)
 s=walk(name,s,'artisan-upstairs',warp=1)
 s=pickup('ArtisanCave_1F',s,'ITEM_BIG_NUGGET',(14,5),'artisan-big-nugget')
 s=walk('ArtisanCave_1F',s,'artisan-east-exit',warp=0)
 print('FINAL',s,position(s))
