"""Production animation callbacks with controlled trig/translation/lifecycle APIs."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATHS = ('src/battle_anim_dragon.c', 'src/battle_anim_effects_1.c', 'src/battle_anim_ice.c', 'src/battle_anim_effects_3.c', 'src/battle_anim_rock.c')


def function(source, name):
    return re.search(r'(?:static )?void ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S).group()


def animation_harness(root):
    sources = {path: (root / path).read_text() for path in PATHS}
    dragon_init = function(sources[PATHS[0]], 'AnimDragonRush')
    avalanche_init = function(sources[PATHS[2]], 'AvalancheAnim_Step')
    strike = re.search(r'sprite->callback = (\w+);', dragon_init)[1]
    fragment = re.search(r'gWoodHammerSmallSpriteTemplate\s*=\s*\{.*?\.callback = (\w+),', sources[PATHS[1]], re.S)[1]
    falling = re.search(r'StoreSpriteCallbackInData6\(sprite, (\w+)\)', avalanche_init)[1]
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int16_t s16;
struct Sprite {s16 x,y,x2,y2,data[8]; unsigned animEnded; void (*callback)(struct Sprite *);};
static s16 gBattleAnimArgs[8];
static unsigned gBattleAnimTarget,gBattleAnimAttacker,trace[16],count,mode,expectedAngle;
static void (*stored)(struct Sprite *);
static void Record(unsigned e){assert(count<16);trace[count++]=e;}
static unsigned IsOnPlayerSide(unsigned battler){Record(3);return battler==0;}
static s16 Cos(unsigned angle,unsigned size){assert(angle==expectedAngle && size==20);Record(4);return angle+20;}
static s16 Sin(unsigned angle,unsigned size){assert(angle==expectedAngle && size==20);Record(5);return 20-(int)angle;}
static void DestroyAnimSprite(struct Sprite *s){assert(s->data[2]==5);Record(6);s->data[2]=30;}
static void DestroySpriteAndMatrix(struct Sprite *s){assert(0);}
static void StartSpriteAnim(struct Sprite *s,unsigned anim){assert(anim==(unsigned)gBattleAnimArgs[mode==2?1:5]);Record(1);}
static void StartSpriteAffineAnim(struct Sprite *s,unsigned anim){assert(anim==1);Record(9);}
static void AnimateSprite(struct Sprite *s){Record(2);s->x++;}
static void TranslateSpriteLinearFixedPoint(struct Sprite *s){assert(0);}
static void TranslateSpriteInEllipse(struct Sprite *s){Record(8);s->y2++;}
static void SetAverageBattlerPositions(unsigned battler,unsigned unused,s16 *x,s16 *y){assert(battler==gBattleAnimTarget && unused==0);Record(10);*x=50;*y=60;}
static void InitSpriteDataForLinearTranslation(struct Sprite *s){Record(7);assert(s->data[0]==gBattleAnimArgs[4] && s->data[1]==s->x && s->data[2]==s->x+gBattleAnimArgs[2] && s->data[3]==s->y && s->data[4]==s->y+gBattleAnimArgs[3]);}
static void StoreSpriteCallbackInData6(struct Sprite *s,void (*callback)(struct Sprite *)){
 Record(11);stored=callback;
 if(mode==1){assert(s->callback==TranslateSpriteLinearFixedPoint && s->data[3]==0 && s->data[4]==0);}
}
'''
    for name in (strike, fragment, falling):
        matches = [function(text, name) for text in sources.values() if re.search(r'(?:static )?void ' + name + r'\([^;]+?\)\n\{', text)]
        assert len(matches) == 1, name
        code += matches[0] + '\n'
    code += dragon_init + '\n' + avalanche_init + '\n'
    code += f'#define Strike {strike}\n#define Fragment {fragment}\n#define Falling {falling}\n'
    code += r'''
static uint64_t hash=1469598103934665603ULL;
static void Hash(unsigned v){hash^=v;hash*=1099511628211ULL;}
static void HashSprite(struct Sprite *s){Hash(s->x);Hash(s->y);Hash(s->x2);Hash(s->y2);for(unsigned i=0;i<8;i++)Hash(s->data[i]);for(unsigned i=0;i<count;i++)Hash(trace[i]);}
int main(void){
 unsigned scenarios=0;const int increments[]={-300,-11,-1,0,1,11,300};
 for(gBattleAnimTarget=0;gBattleAnimTarget<2;gBattleAnimTarget++)for(unsigned angle=0;angle<256;angle++)for(unsigned i=0;i<7;i++)for(unsigned ended=0;ended<2;ended++){
  struct Sprite s={.x=100,.y=80,.data={increments[i],angle,5,9},.animEnded=ended};
  mode=0;count=0;expectedAngle=(angle+increments[i])&255;Strike(&s);
  assert(s.data[1]==expectedAngle && s.x2==expectedAngle+20 && s.y2==20-(int)expectedAngle);
  assert(s.data[2]==(ended?31:6));assert(count==(ended?4:3));assert(trace[0]==3 && trace[1]==4 && trace[2]==5);if(ended)assert(trace[3]==6);
  HashSprite(&s);scenarios++;
 }
 for(gBattleAnimTarget=0;gBattleAnimTarget<2;gBattleAnimTarget++){
  struct Sprite s={.x=100,.y=80};gBattleAnimArgs[0]=7;gBattleAnimArgs[1]=-3;count=0;AnimDragonRush(&s);
  assert(s.callback==Strike && s.data[0]==(gBattleAnimTarget?11:-11) && s.data[1]==192 && s.x==(gBattleAnimTarget?107:93) && s.y==77);
  assert(count==(gBattleAnimTarget?1:2) && trace[0]==3);if(!gBattleAnimTarget)assert(trace[1]==9);
 }
 const int offsets[]={-7,0,13};
 for(gBattleAnimAttacker=0;gBattleAnimAttacker<2;gBattleAnimAttacker++)for(unsigned x=0;x<3;x++)for(unsigned y=0;y<3;y++)for(unsigned duration=0;duration<3;duration++)for(unsigned anim=0;anim<3;anim++){
  struct Sprite s={.x=100,.y=80};mode=1;count=0;
  gBattleAnimArgs[0]=offsets[x];gBattleAnimArgs[1]=offsets[y];gBattleAnimArgs[2]=-17;gBattleAnimArgs[3]=23;gBattleAnimArgs[4]=duration*15;gBattleAnimArgs[5]=anim;
  Fragment(&s);assert(s.x==101+(gBattleAnimAttacker?-offsets[x]:offsets[x]) && s.y==80+offsets[y]);
  assert(s.data[3]==0 && s.data[4]==0 && s.callback==TranslateSpriteLinearFixedPoint && stored==DestroySpriteAndMatrix);
  const unsigned expected[]={1,2,3,7,11};assert(count==5);for(unsigned i=0;i<5;i++)assert(trace[i]==expected[i]);HashSprite(&s);scenarios++;
 }
 for(int offset=-64;offset<=64;offset++){
  struct Sprite s={.x=100,.y=80,.y2=3,.data={1,2,3,4,5,offset,7,8}};mode=2;count=0;Falling(&s);
  assert(s.x==100+offset && s.y==80 && s.y2==4 && s.data[0]==192 && s.data[1]==offset && s.data[2]==4 && s.data[3]==32 && s.data[4]==-24 && s.data[5]==offset);
  assert(stored==DestroySpriteAndMatrix && s.callback==TranslateSpriteInEllipse && count==2 && trace[0]==11 && trace[1]==8);HashSprite(&s);scenarios++;
 }
 for(unsigned average=0;average<2;average++){
  struct Sprite s={.x=100,.y=80,.y2=3};mode=2;count=0;gBattleAnimArgs[0]=7;gBattleAnimArgs[1]=2;gBattleAnimArgs[2]=-9;gBattleAnimArgs[3]=average;
  AvalancheAnim_Step(&s);assert(stored==Falling && s.callback==TranslateSpriteInEllipse && s.y2==4);
  assert(s.x==(average?58:108) && s.y==(average?74:94) && s.data[5]==-9);
  unsigned at=0;if(average)assert(trace[at++]==10);assert(trace[at++]==1 && trace[at++]==2 && trace[at++]==11 && trace[at++]==8 && at==count);
 }
 printf("animation scenarios=%u trace=%016llx\n",scenarios,(unsigned long long)hash);return 0;
}
'''
    return code


def run_harness(code):
    with tempfile.TemporaryDirectory() as temp:
        path = Path(temp)
        (path / 'test.c').write_text(code)
        subprocess.run(['cc', '-O1', '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
        return subprocess.check_output([str(path / 'test')], text=True, timeout=30)


class AnimationOwnerIntegrity(unittest.TestCase):
    def test_callbacks_and_consumers_preserve_traces_and_lifecycle(self):
        run_harness(animation_harness(ROOT))


if __name__ == '__main__':
    unittest.main()
