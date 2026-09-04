"""Host execution of production earned relic debt, retries and gift ordering.

Inventory and Pokemon delivery APIs are controlled stubs; this does not replace
native Bag/PC or capture tests.
"""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class LegendaryRelicIntegrity(unittest.TestCase):
    def test_earned_pending_rewards_and_retry(self):
        source = (ROOT / 'src/legendary_signs.c').read_text()
        grant = source[source.index('static const enum Item sLegendaryRelicItems'):source.index('void TryUnlockEligibleVisibleLegendarySignsForCurrentMap')]
        reward = source[source.index('u8 GiveLegendarySignReward('):source.index('void TryGiveArceusLegendarySignMasteryReward(')]
        items = sorted(set(re.findall(r'\bITEM_[A-Z_]+', grant)))
        species = sorted(set(re.findall(r'\bSPECIES_[A-Z_]+', grant)))
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int bool32;
typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define PC_ITEMS_COUNT 32
#define MON_CANT_GIVE 0
#define MON_GIVEN_TO_PARTY 1
#define MON_GIVEN_TO_PC 2
#define OTID_STRUCT_PLAYER_ID 0
#define FLAG_SET_SEEN 0
#define FLAG_SET_CAUGHT 1
#define FLAG_GET_CAUGHT 2
#define VAR_LEGENDARY_RELIC_DELIVERY_0 0
#define VAR_LEGENDARY_RELIC_DELIVERY_1 1
'''
        harness += 'enum Item { ITEM_NONE, ' + ', '.join(items) + ', ITEM_OTHER };\n'
        harness += 'enum Species { SPECIES_NONE, ' + ', '.join(species) + ' };\n'
        harness += r'''
struct ItemSlot { enum Item itemId; unsigned quantity; };
struct Save { struct ItemSlot pcItems[PC_ITEMS_COUNT]; } save;
static struct Save *gSaveBlock1Ptr = &save;
static struct ItemSlot bag[32];
static unsigned bagCapacity, pcCapacity, delivered, caught, dex, deliveryCalls;
static int deliveryResult;
struct Pokemon { enum Species species; enum Item heldItem; };
static struct Pokemon deliveredMon;
static u16 vars[2];
static unsigned ownedSpecies[32];
static u16 VarGet(unsigned id) {return vars[id];}
static void VarSet(unsigned id,u16 value) {vars[id]=value;}
static unsigned SpeciesToNationalPokedexNum(enum Species species) {return species;}
static unsigned GetSetPokedexFlag(unsigned species,unsigned mode) {return ownedSpecies[species];}
static int Has(struct ItemSlot *slots, enum Item item, unsigned count) {
    for (unsigned i=0;i<32;i++) if (slots[i].itemId==item && slots[i].quantity>=count) return 1;
    return 0;
}
static int Add(struct ItemSlot *slots, unsigned capacity, enum Item item, unsigned count) {
    for(unsigned i=0;i<capacity;i++) if(slots[i].itemId==item) {slots[i].quantity+=count;return 1;}
    for(unsigned i=0;i<capacity;i++) if(!slots[i].itemId) {slots[i]=(struct ItemSlot){item,count};return 1;}
    return 0;
}
static int CheckBagHasItem(enum Item item,unsigned n) {return Has(bag,item,n);}
static int CheckPCHasItem(enum Item item,unsigned n) {return Has(save.pcItems,item,n);}
static int AddBagItem(enum Item item,unsigned n) {return Add(bag,bagCapacity,item,n);}
static int AddPCItem(enum Item item,unsigned n) {return Add(save.pcItems,pcCapacity,item,n);}
static void RemovePCItem(unsigned i,unsigned n) {
    assert(save.pcItems[i].quantity>=n);save.pcItems[i].quantity-=n;
    if(!save.pcItems[i].quantity) save.pcItems[i].itemId=ITEM_NONE;
}
static int RemoveBagItem(enum Item item,unsigned n) {
    for(unsigned i=0;i<32;i++) if(bag[i].itemId==item && bag[i].quantity>=n) {
        bag[i].quantity-=n;if(!bag[i].quantity)bag[i].itemId=ITEM_NONE;return 1;
    }
    return 0;
}
static unsigned Random32(void) {return 42;}
static void CreateMon(struct Pokemon *p,enum Species s,u8 level,unsigned rng,unsigned ot) {*p=(struct Pokemon){s,ITEM_NONE};}
static void ApplyNonMegaGiftSet(struct Pokemon *p) {p->heldItem=ITEM_OTHER;}
static u8 GiveCapturedMonToPlayer(struct Pokemon *p) {
    deliveryCalls++;
    if(deliveryResult) {delivered++; deliveredMon=*p;}
    return deliveryResult;
}
static void HandleSetPokedexFlagFromMon(struct Pokemon *p,unsigned flag) {dex++;}
static void CalculatePlayerPartyCount(void) {}
'''
        harness += grant
        harness += r'''
static void MarkLegendarySignCaughtBySpecies(enum Species species) {
    GiveLegendaryRelicsForSpecies(species);
    caught++;
}
'''
        harness += reward
        harness += r'''
static void Reset(unsigned b,unsigned p,int result) {
    memset(bag,0,sizeof bag);memset(&save,0,sizeof save);
    bagCapacity=b;pcCapacity=p;deliveryResult=result;
    delivered=caught=dex=deliveryCalls=0;
    memset(vars,0,sizeof vars);memset(ownedSpecies,0,sizeof ownedSpecies);
}
static unsigned Total(void) {
    unsigned n=0;for(unsigned i=0;i<32;i++)n+=bag[i].quantity+save.pcItems[i].quantity;return n;
}
int main(void) {
    // Full item stores preserve an earned gift as pending; held item is untouched.
    Reset(0,0,MON_GIVEN_TO_PARTY);
    assert(GiveLegendarySignReward(SPECIES_ZACIAN,50)==MON_GIVEN_TO_PARTY);
    assert(delivered==1 && caught==1 && dex==2 && !Total());
    assert(GetLegendaryRelicDeliveryState()==((1u<<2)|(1u<<26)));
    assert(deliveredMon.heldItem==ITEM_OTHER);
    pcCapacity=1;RetryPendingLegendaryRelics();
    assert(CheckPCHasItem(ITEM_RUSTED_SWORD,1));
    assert(GetLegendaryRelicDeliveryState()==(1u<<26));
    // Deliberately discarded delivered items are never re-created, even on Mark replay.
    RemovePCItem(0,1);RetryPendingLegendaryRelics();
    MarkLegendarySignCaughtBySpecies(SPECIES_ZACIAN);
    assert(!Total() && GetLegendaryRelicDeliveryState()==(1u<<26));
    // Partial Ogerpon survives save/restore, and retries only the missing masks.
    Reset(1,0,MON_GIVEN_TO_PARTY);
    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON_TEAL);
    assert(Total()==1 && (GetLegendaryRelicDeliveryState()&0xFFFFFF)==((1u<<5)|(1u<<6)));
    u16 saved[2];memcpy(saved,vars,sizeof saved);memset(vars,0,sizeof vars);memcpy(vars,saved,sizeof vars);
    RemoveBagItem(ITEM_WELLSPRING_MASK,1); // Deliberately discard the delivered mask.
    pcCapacity=1;RetryPendingLegendaryRelics();
    assert(Total()==2 && !CheckBagHasItem(ITEM_WELLSPRING_MASK,1));
    assert((GetLegendaryRelicDeliveryState()&0xFFFFFF)==0);
    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON_TEAL);assert(Total()==2);
    // Arceus fills split Bag/PC capacity then persists the final undelivered plate.
    Reset(8,8,MON_GIVEN_TO_PC);
    assert(GiveLegendarySignReward(SPECIES_ARCEUS,50)==MON_GIVEN_TO_PC && Total()==16);
    assert((GetLegendaryRelicDeliveryState()&0xFFFFFF)==(1u<<23));
    memcpy(saved,vars,sizeof saved);memset(vars,0,sizeof vars);memcpy(vars,saved,sizeof vars);
    pcCapacity=9;RetryPendingLegendaryRelics();
    assert(Total()==17 && (GetLegendaryRelicDeliveryState()&0xFFFFFF)==0);
    // Failed Pokemon delivery earns nothing and leaves the reward retryable.
    Reset(1,2,MON_CANT_GIVE);
    assert(GiveLegendarySignReward(SPECIES_OGERPON_TEAL,50)==MON_CANT_GIVE);
    assert(!Total() && !caught && !dex && !GetLegendaryRelicDeliveryState());
    deliveryResult=MON_GIVEN_TO_PARTY;
    assert(GiveLegendarySignReward(SPECIES_OGERPON_TEAL,50)==MON_GIVEN_TO_PARTY && Total()==3);
    // Existing quantities are preserved and never doubled.
    Reset(1,1,MON_GIVEN_TO_PARTY);
    AddBagItem(ITEM_FLAME_PLATE,7);AddPCItem(ITEM_SPLASH_PLATE,4);
    MarkLegendarySignCaughtBySpecies(SPECIES_ARCEUS);
    assert(CheckBagHasItem(ITEM_FLAME_PLATE,7) && CheckPCHasItem(ITEM_SPLASH_PLATE,4) && Total()==11);
    assert(!(GetLegendaryRelicDeliveryState()&((1u<<7)|(1u<<8))));
    // Old ownership suppresses replay but never backfills lost/discarded items.
    Reset(32,32,MON_GIVEN_TO_PARTY);
    ownedSpecies[SPECIES_GROUDON]=1;ownedSpecies[SPECIES_ARCEUS]=1;
    vars[0]=vars[1]=0xFFFF;InitializeLegendaryRelicDeliveryState();
    assert(GetLegendaryRelicDeliveryState()==((1u<<24)|(1u<<29)));
    RetryPendingLegendaryRelics();MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    assert(!Total());
    MarkLegendarySignCaughtBySpecies(SPECIES_KYOGRE);
    assert(Total()==1 && CheckBagHasItem(ITEM_BLUE_ORB,1));
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler, 'Host C compiler required')
        with tempfile.TemporaryDirectory(prefix='champions-relic-host-') as directory:
            fixture = Path(directory) / 'relic.c'
            executable = Path(directory) / 'relic'
            fixture.write_text(harness)
            subprocess.run([compiler, '-std=c99', str(fixture), '-o', str(executable)], check=True, capture_output=True, text=True)
            subprocess.run([str(executable)], check=True, capture_output=True, text=True)


if __name__ == '__main__':
    unittest.main()
