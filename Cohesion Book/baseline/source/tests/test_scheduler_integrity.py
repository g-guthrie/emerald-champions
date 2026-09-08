"""Full production scheduler against an independent stable-priority model.

Assignment instrumentation records ordered task writes without changing the
expressions. Callback words use 32-bit address values, never truncated host
function pointers; scheduler execution uses real host callbacks separately.
"""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def extract(source, name):
    return re.search(r'(?:void|u32) ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S).group()


def instrument(source):
    source = re.sub(r'(?m)^(\s*)gTasks\[(.*)\]\.data\[(.*)\] = (.*?);(\s*//.*)?$',
                    r'\1WriteData(\2, \3, \4);\5', source)
    for field in ('func', 'isActive', 'prev', 'next', 'priority'):
        source = re.sub(r'(?m)^(\s*)gTasks\[(.*)\]\.' + field + r' = (.*);$',
                        r'\1Write_' + field + r'(\2, \3);', source)
    return source


def scheduler_harness(source, header):
    for name in ('SetTaskFuncWithFollowupFunc', 'SwitchTaskToFollowupFunc'):
        source = source.replace(extract(source, name), '')
    source = re.sub(r'^#include.*\n', '', source, flags=re.M)
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;typedef uint8_t bool8;typedef int16_t s16;typedef int32_t s32;typedef uint16_t u16;typedef uint32_t u32;
#define TRUE 1
#define FALSE 0
#define COMMON_DATA
'''+header+r'''
static void A(u8 id);static void B(u8 id);
static unsigned events[2048],eventCount,calls[32],callCount,mutation,newId;
static unsigned Code(TaskFunc f){if(f==A)return 1;if(f==B)return 2;if(f==TaskDummy)return 3;assert(f==NULL);return 0;}
static void Record(unsigned field,unsigned id,unsigned value){assert(eventCount+3<=2048);events[eventCount++]=field;events[eventCount++]=id;events[eventCount++]=value;}
static void Write_func(unsigned id,TaskFunc value){Record(5,id,Code(value));gTasks[id].func=value;}
static void WriteData(unsigned id,unsigned slot,u32 value){Record(0x100+slot,id,(u16)value);gTasks[id].data[slot]=value;}
'''
    for field, number in [('isActive',1),('prev',2),('next',3),('priority',4)]:
        code += f'static void Write_{field}(unsigned id,unsigned value){{Record({number},id,(u8)value);gTasks[id].{field}=value;}}\n'
    code += instrument(source)
    code += r'''
static void A(u8 id){assert(callCount<32);calls[callCount++]=id;Record(9,id,1);unsigned op=mutation;mutation=0;if(op==1)DestroyTask(id);if(op==2)DestroyTask(1);if(op==3)newId=CreateTask(B,2);if(op==4)newId=CreateTask(B,0);}
static void B(u8 id){assert(callCount<32);calls[callCount++]=id;Record(9,id,2);}
static struct Model {TaskFunc func;unsigned active,priority,serial,prev,next;u16 data[16];} model[16];
static unsigned serial,order[16],ordered;
static void ModelReset(void){memset(model,0,sizeof model);serial=0;for(unsigned i=0;i<16;i++){model[i].func=TaskDummy;model[i].priority=255;model[i].prev=i?i:HEAD_SENTINEL;model[i].next=i==15?TAIL_SENTINEL:i+1;}}
static unsigned ModelCreate(TaskFunc func,unsigned priority){unsigned id=0;while(id<16 && model[id].active)id++;if(id==16)return 0;model[id].active=1;model[id].func=func;model[id].priority=priority;model[id].serial=serial++;memset(model[id].data,0,sizeof model[id].data);return id;}
static void ModelDestroy(unsigned id){if(model[id].active){model[id].prev=gTasks[id].prev;model[id].next=gTasks[id].next;model[id].active=0;}}
static void Check(void){
 ordered=0;
 for(unsigned id=0;id<16;id++)if(model[id].active){unsigned at=ordered++;while(at && (model[order[at-1]].priority>model[id].priority || (model[order[at-1]].priority==model[id].priority && model[order[at-1]].serial>model[id].serial))){order[at]=order[at-1];at--;}order[at]=id;}
 assert(FindFirstActiveTask()==(ordered?order[0]:NUM_TASKS));
 for(unsigned i=0;i<ordered;i++){unsigned id=order[i];assert(gTasks[id].prev==(i?order[i-1]:HEAD_SENTINEL));assert(gTasks[id].next==(i+1<ordered?order[i+1]:TAIL_SENTINEL));}
 for(unsigned i=0;i<16;i++){assert(gTasks[i].isActive==model[i].active && gTasks[i].func==model[i].func && gTasks[i].priority==model[i].priority);assert(!memcmp(gTasks[i].data,model[i].data,sizeof model[i].data));if(!model[i].active)assert(gTasks[i].prev==model[i].prev && gTasks[i].next==model[i].next);}
 TaskFunc funcs[]={NULL,A,B,TaskDummy};for(unsigned f=0;f<4;f++){unsigned first=TASK_NONE;for(unsigned i=0;i<16;i++)if(model[i].active && model[i].func==funcs[f]){first=i;break;}assert(FindTaskIdByFunc(funcs[f])==first);assert(FuncIsActiveTask(funcs[f])==(first!=TASK_NONE));}
}
static uint64_t hash=1469598103934665603ULL;
static void Hash(unsigned v){hash^=v;hash*=1099511628211ULL;}
static void Capture(void){for(unsigned i=0;i<eventCount;i++)Hash(events[i]);for(unsigned i=0;i<16;i++){Hash(Code(gTasks[i].func));Hash(gTasks[i].isActive);Hash(gTasks[i].prev);Hash(gTasks[i].next);Hash(gTasks[i].priority);for(unsigned d=0;d<16;d++)Hash((u16)gTasks[i].data[d]);}}
int main(void){
 unsigned operations=0;
 for(unsigned seed=0;seed<32;seed++){
  eventCount=0;ResetTasks();ModelReset();Check();u32 random=seed+1;
  for(unsigned step=0;step<2000;step++){
   random=random*1664525u+1013904223u;unsigned op=(random>>24)%8,id=(random>>8)%16;eventCount=0;
   if(op<4){TaskFunc func=(random&1)?A:B;unsigned priority=(random>>16)&255;assert(CreateTask(func,priority)==ModelCreate(func,priority));}
   else if(op<6){ModelDestroy(id);DestroyTask(id);}
   else if(op==6){unsigned element=random%16;SetWordTaskArg(id,element,random);if(element<15){model[id].data[element]=random;model[id].data[element+1]=random>>16;}}
   else{Check();callCount=0;RunTasks();assert(callCount==ordered);for(unsigned i=0;i<ordered;i++)assert(calls[i]==order[i]);}
   Check();Capture();operations++;
  }
 }
 // Stable equal priorities and unchanged full-capacity failure contract.
 eventCount=0;ResetTasks();ModelReset();for(unsigned i=0;i<16;i++){assert(CreateTask(A,5)==i);assert(ModelCreate(A,5)==i);}Check();
 struct Task before[16];memcpy(before,gTasks,sizeof before);assert(CreateTask(B,0)==0);assert(!memcmp(before,gTasks,sizeof before));
 for(unsigned i=0;i<16;i++){eventCount=0;ModelDestroy(i);DestroyTask(i);Check();Capture();}
 // Callback-driven mutations retain the original live-list iteration rules.
 for(unsigned mode=1;mode<=4;mode++){
  eventCount=0;ResetTasks();ModelReset();CreateTask(A,1);ModelCreate(A,1);CreateTask(B,2);ModelCreate(B,2);if(mode<=2){CreateTask(B,3);ModelCreate(B,3);}Check();
  if(mode<=2)ModelDestroy(mode==1?0:1);else assert(ModelCreate(B,mode==3?2:0)==2);
  mutation=mode;callCount=0;eventCount=0;RunTasks();Check();assert(calls[0]==0);
  if(mode==1){assert(callCount==3 && calls[1]==1 && calls[2]==2);}
  if(mode==2){assert(callCount==2 && calls[1]==2);}
  if(mode==3){assert(newId==2 && callCount==3 && calls[1]==1 && calls[2]==2);}
  if(mode==4){assert(newId==2 && callCount==2 && calls[1]==1);callCount=0;RunTasks();assert(callCount==3 && calls[0]==2 && calls[1]==0 && calls[2]==1);}
  Capture();
 }
 ResetTasks();gTasks[4].isActive=2;gTasks[4].func=A;assert(!FuncIsActiveTask(A) && FindTaskIdByFunc(A)==TASK_NONE);
 gTasks[7].isActive=TRUE;gTasks[7].func=A;assert(FuncIsActiveTask(A) && FindTaskIdByFunc(A)==7);gTasks[4].isActive=TRUE;assert(FindTaskIdByFunc(A)==4);
 printf("scheduler operations=%u trace=%016llx\n",operations,(unsigned long long)hash);return 0;
}
'''
    return code


def followup_harness(source, high_limit=65535):
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef int16_t s16;typedef u32 TaskFunc;
#define NUM_TASK_DATA 16
static struct {TaskFunc func;s16 data[NUM_TASK_DATA];} gTasks[2];
static unsigned trace[16],count;
static void WriteData(unsigned id,unsigned slot,u32 value){trace[count++]=slot;gTasks[id].data[slot]=value;}
static void Write_func(unsigned id,TaskFunc value){trace[count++]=0x100;gTasks[id].func=value;}
void SetWordTaskArg(u8,u8,u32);u32 GetWordTaskArg(u8,u8);
'''
    for name in ('SetWordTaskArg','GetWordTaskArg','SetTaskFuncWithFollowupFunc','SwitchTaskToFollowupFunc'):
        code += instrument(extract(source,name))+'\n'
    code += f'\n#define HIGH_LIMIT {high_limit}\n'
    code += r'''
int main(void){const unsigned lows[]={0,1,0x7fff,0x8000,0xffff};unsigned cases=0;
 for(unsigned high=0;high<=HIGH_LIMIT;high++)for(unsigned low=0;low<5;low++){
  memset(gTasks,0x5a,sizeof gTasks);u32 value=((u32)high<<16)|lows[low];count=0;
  SetTaskFuncWithFollowupFunc(1,0x08000101,value);
  assert(count==3 && trace[0]==14 && trace[1]==15 && trace[2]==0x100);
  assert(gTasks[1].func==0x08000101 && (u16)gTasks[1].data[14]==(u16)value && (u16)gTasks[1].data[15]==high);
  count=0;SwitchTaskToFollowupFunc(1);assert(gTasks[1].func==value && count==1 && trace[0]==0x100);
  assert(gTasks[0].func==0x5a5a5a5a);for(unsigned i=0;i<16;i++){assert((u16)gTasks[0].data[i]==0x5a5a);if(i<14)assert((u16)gTasks[1].data[i]==0x5a5a);}cases++;
 }
 printf("followup addresses=%u\n",cases);return 0;}
'''
    return code


def run_c(code):
    with tempfile.TemporaryDirectory() as temp:
        root=Path(temp);(root/'test.c').write_text(code)
        subprocess.run(['cc','-O1','-std=c11','-fsanitize=undefined','-fno-sanitize-recover=undefined',str(root/'test.c'),'-o',str(root/'test')],check=True,timeout=30)
        return subprocess.check_output([str(root/'test')],text=True,timeout=30)


class SchedulerIntegrity(unittest.TestCase):
    def test_stable_priority_model_and_callback_mutations(self):
        run_c(scheduler_harness((ROOT/'src/task.c').read_text(),(ROOT/'include/task.h').read_text()))

    def test_followup_addresses_use_last_two_slots_and_ordered_writes(self):
        run_c(followup_harness((ROOT/'src/task.c').read_text()))


if __name__ == '__main__':
    unittest.main()
