"""Host-execute production handoff and Steven delivery helpers; inventory is stubbed."""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class StoryHandoffIntegrity(unittest.TestCase):
    def test_inventory_consumption_and_durable_starter_rewards(self):
        source = (ROOT / 'src/field_specials.c').read_text()
        helpers = source[source.index('void CheckEmeraldChampionsHandoffItem(void)'):source.index('void OpenEmeraldChampionsEvolutionItemArchive(void)')]
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint32_t u32;
typedef uint16_t u16;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define PARTY_SIZE 6
#define B_TRAINER_PLAYER 0
#define TOTAL_BOXES_COUNT 2
#define IN_BOX_COUNT 3
#define PC_ITEMS_COUNT 4
#define SPECIES_NONE 0
#define MON_DATA_SPECIES 0
#define MON_DATA_HELD_ITEM 1
#define VAR_STEVEN_STARTER_STONE_DELIVERY 0
#define GetBoxMonData GetMonData
enum Item { ITEM_NONE, X, Y, LETTER, WRONG, ITEM_COUNT };
struct ItemSlot {enum Item itemId; unsigned quantity;};
struct Save {struct ItemSlot pcItems[PC_ITEMS_COUNT];} save, *gSaveBlock1Ptr=&save;
struct Mon {unsigned species, held;};
struct Mon gParties[1][PARTY_SIZE];
struct Storage {struct Mon boxes[TOTAL_BOXES_COUNT][IN_BOX_COUNT];} storage, *gPokemonStoragePtr=&storage;
static u16 gSpecialVar_Result,gSpecialVar_0x8004,gSpecialVar_0x8005,gSpecialVar_0x8009,gSpecialVar_0x800A,delivered,first=X,second=Y;
static unsigned bag[ITEM_COUNT],bagRoom,pcRoom,added;
static unsigned GetMonData(struct Mon *p,unsigned f){return f==MON_DATA_SPECIES?p->species:p->held;}
static u16 VarGet(unsigned id){return delivered;}
static void VarSet(unsigned id,u16 value){delivered=value;}
static int CheckBagHasItem(enum Item item,unsigned n){return bag[item]>=n;}
static int CheckPCHasItem(enum Item item,unsigned n){for(unsigned i=0;i<PC_ITEMS_COUNT;i++)if(save.pcItems[i].itemId==item&&save.pcItems[i].quantity>=n)return 1;return 0;}
static int RemoveBagItem(enum Item item,unsigned n){if(bag[item]<n)return 0;bag[item]-=n;return 1;}
static void RemovePCItem(unsigned i,unsigned n){assert(save.pcItems[i].quantity>=n);save.pcItems[i].quantity-=n;}
static void CompactPCItems(void){unsigned target=0;for(unsigned i=0;i<PC_ITEMS_COUNT;i++)if(save.pcItems[i].quantity)save.pcItems[target++]=save.pcItems[i];while(target<PC_ITEMS_COUNT)save.pcItems[target++]=(struct ItemSlot){ITEM_NONE,0};}
static int AddBagItem(enum Item item,unsigned n){if(!bagRoom)return 0;bagRoom--;bag[item]+=n;added++;return 1;}
static int AddPCItem(enum Item item,unsigned n){if(!pcRoom)return 0;for(unsigned i=0;i<PC_ITEMS_COUNT;i++)if(!save.pcItems[i].quantity){save.pcItems[i]=(struct ItemSlot){item,n};pcRoom--;added++;return 1;}return 0;}
static void GetEmeraldChampionsStarterMegaStone(void){gSpecialVar_0x8004=first;gSpecialVar_0x8005=second;gSpecialVar_Result=first!=ITEM_NONE;}
static void reset(void){memset(bag,0,sizeof(bag));memset(&save,0,sizeof(save));memset(gParties,0,sizeof(gParties));memset(&storage,0,sizeof(storage));delivered=added=bagRoom=pcRoom=0;first=X;second=Y;}
'''
        harness += helpers + r'''
int main(void){
 reset();gSpecialVar_0x8004=LETTER;bag[WRONG]=1;
 CheckEmeraldChampionsHandoffItem();assert(!gSpecialVar_Result);
 TakeEmeraldChampionsHandoffItem();assert(!gSpecialVar_Result&&bag[WRONG]==1);
 save.pcItems[2]=(struct ItemSlot){LETTER,1};
 CheckEmeraldChampionsHandoffItem();assert(gSpecialVar_Result);
 TakeEmeraldChampionsHandoffItem();assert(gSpecialVar_Result&&!save.pcItems[2].quantity);
 TakeEmeraldChampionsHandoffItem();assert(!gSpecialVar_Result);
 bag[LETTER]=1;save.pcItems[0]=(struct ItemSlot){LETTER,1};
 TakeEmeraldChampionsHandoffItem();assert(gSpecialVar_Result&&!bag[LETTER]&&save.pcItems[0].quantity==1);
 reset();gSpecialVar_0x8004=LETTER;save.pcItems[0]=(struct ItemSlot){LETTER,1};save.pcItems[1]=(struct ItemSlot){WRONG,2};TakeEmeraldChampionsHandoffItem();assert(gSpecialVar_Result&&save.pcItems[0].itemId==WRONG&&save.pcItems[0].quantity==2&&!save.pcItems[1].quantity);
 reset();GiveEmeraldChampionsStarterMegaStones();assert(!gSpecialVar_Result&&!delivered&&!added);
 bagRoom=1;GiveEmeraldChampionsStarterMegaStones();assert(!gSpecialVar_Result&&delivered==1&&bag[X]==1&&!bag[Y]&&gSpecialVar_0x8009==1&&gSpecialVar_0x800A==0);
 bag[X]=0;bagRoom=1;GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&delivered==3&&!bag[X]&&bag[Y]==1&&added==2);
 bag[Y]=0;GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&added==2&&!bag[X]&&!bag[Y]&&!gSpecialVar_0x8009&&!gSpecialVar_0x800A);
 reset();pcRoom=2;GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&delivered==3&&CheckPCHasItem(X,1)&&CheckPCHasItem(Y,1)&&gSpecialVar_0x8009==2&&gSpecialVar_0x800A==2);
 reset();save.pcItems[0]=(struct ItemSlot){X,1};gParties[0][0]=(struct Mon){1,Y};
 GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&delivered==3&&!added);
 reset();gParties[0][0]=(struct Mon){1,X};storage.boxes[1][2]=(struct Mon){1,Y};
 GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&delivered==3&&!added);
 reset();first=second=ITEM_NONE;GiveEmeraldChampionsStarterMegaStones();assert(gSpecialVar_Result&&!delivered&&!added);
 return 0;
}
'''
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary)
            (path / 'test.c').write_text(harness)
            subprocess.run([shutil.which('cc'), '-std=c99', str(path / 'test.c'), '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)

    def test_shared_consumption_precedes_message_and_all_three_callers_check(self):
        shared = (ROOT / 'data/event_scripts.s').read_text().split('Common_EventScript_PlayerHandedOverTheItem::',1)[1].split('\n\n',1)[0]
        self.assertLess(shared.index('TakeEmeraldChampionsHandoffItem'), shared.index('message gText_PlayerHandedOverTheItem'))
        self.assertIn('goto_if_eq VAR_RESULT, FALSE, Common_EventScript_NopReturn', shared)
        for name in ('GraniteCave_StevensRoom', 'SlateportCity_OceanicMuseum_2F', 'FallarborTown_CozmosHouse'):
            source = (ROOT / f'data/maps/{name}/scripts.inc').read_text()
            self.assertIn('call Common_EventScript_PlayerHandedOverTheItem\n\tgoto_if_eq VAR_RESULT, FALSE, Common_EventScript_MissingHandoffItem', source)
        museum = (ROOT / 'data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc').read_text()
        self.assertLess(museum.index('special CheckEmeraldChampionsHandoffItem'), museum.index('addobject'))
        self.assertIn('trainerbattle_no_intro_double_allow_single TRAINER_GRUNT_MUSEUM_2', museum)

    def test_reward_bits_survive_mystery_gift_reset_and_pending_steven_stays(self):
        reset = (ROOT / 'src/event_data.c').read_text().split('void ClearMysteryGiftVars(void)',1)[1].split('}',1)[0]
        self.assertNotIn('VAR_STEVEN_STARTER_STONE_DELIVERY', reset)
        self.assertNotIn('VAR_GIFT_UNUSED_3', reset)
        source = (ROOT / 'data/maps/GraniteCave_StevensRoom/scripts.inc').read_text()
        branch = source.split('GraniteCave_StevensRoom_EventScript_StoneStorageFull::',1)[1].split('\n\n',1)[0]
        self.assertNotIn('FLAG_HIDE', branch)
        self.assertIn('release', branch)
        self.assertIn('goto_if_eq VAR_0x8008, FALSE, GraniteCave_StevensRoom_EventScript_StoneStorageFull', source)
        self.assertIn('call_if_ne VAR_0x8009, 0, GraniteCave_StevensRoom_EventScript_FirstStoneReceipt', source)
        self.assertIn('call_if_ne VAR_0x800A, 0, GraniteCave_StevensRoom_EventScript_SecondStoneReceipt', source)
        receipt = source.split('GraniteCave_StevensRoom_EventScript_StoneReceipt::',1)[1].split('GraniteCave_StevensRoom_Text_StoneInPC:',1)[0]
        self.assertIn('playfanfare MUS_OBTAIN_ITEM', receipt)
        self.assertIn('msgbox gText_ObtainedTheItem', receipt)
        self.assertIn('putitemaway VAR_0x8000', receipt)
        self.assertNotIn('giveitem', receipt)


if __name__ == '__main__':
    unittest.main()
