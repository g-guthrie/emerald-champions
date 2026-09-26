#ifndef GUARD_CONSTANTS_SERVICE_VARS_H
#define GUARD_CONSTANTS_SERVICE_VARS_H

// Who owns the special vars when a service script calls native code.
//
// VAR_0x8000-VAR_0x8007 are call arguments and results. Any special,
// callnative, native screen or standard script may overwrite any of them
// (giveitem writes 0x8000, 0x8001, 0x8006 and 0x8007; ChoosePartyMon and
// chooseboxmon write 0x8004; ShowScrollableMultichoice reads its menu id from
// 0x8004). A service copies its arguments in right before each call and
// never expects one of these to survive a call made for another purpose.
//
// VAR_0x8008-VAR_0x800B belong to the running script: menu cursors,
// once-per-visit flags and choices that must outlive native screens. Native
// code a service can reach neither reads nor writes them; natives that need
// to resume after their own screens keep that state in their own statics.
// The only native writers left are field entry points that start their own
// one-shot script and never run inside a service: item balls (0x8009,
// 0x800A), hidden items (0x8009), the whiteout nurse's local id (0x800B), and
// GetLevelCapForScriptedGift, whose gift level in 0x800A is read by the
// givemon or setwildbattle right after it.
//
// Arguments and results of the natives these services call:
//   TeachMoveRelearnerMove            in 0x8004 party slot (or PC_MON_CHOSEN
//                                     with VAR_MON_BOX_ID/VAR_MON_BOX_POS);
//                                     waits until the move list closes
//   Bonding preview/apply             in 0x8004 party slot, 0x8005 value row
//   BufferEmeraldChampionsBattleItemStock  in 0x8004 shelf; out VAR_RESULT count
//   BufferEmeraldChampionsHarvestRecipe    in 0x8004 reward; out VAR_RESULT claimed
//   TradeEmeraldChampionsGardenBerries     in 0x8004 reward; out VAR_RESULT
//   CanReceiveBerryPair               in 0x8004 and 0x8005 Berries
//   FossilToSpecies                   in 0x8004 fossil; out 0x8006 species
//   CheckChosenMonCanGainEVs          in 0x8004 slot, 0x8005 stat, 0x8006
//                                     amount; out VAR_RESULT, 0x8007 EV total
//   ChangeChosenMonHiddenPower        in 0x8004 slot, 0x8005 type
//
// The aliases below name each service's own state. Services that call one
// another (the tutor calls Bonding; the Berry Master calls the Harvest menu)
// are laid out so their vars never overlap.

// Center clerk (data/scripts/general_mart.inc and the Held Items and Form
// Items counters in data/scripts/emerald_champions.inc).
#define VAR_CLERK_SHELF_CURSOR          VAR_0x8008 // Held Items shelf last opened
#define VAR_CLERK_SERVICE_CURSOR        VAR_0x8009 // service row last used
#define VAR_CLERK_HELD_ITEMS_GREETED    VAR_0x800A // Held Items welcome given this visit
#define VAR_CLERK_FORM_ITEMS_GREETED    VAR_0x800B // Form Items explanation given this visit

// Center move tutor. Its Bonding service uses 0x8008 and 0x800A below.
#define VAR_TUTOR_CURSOR                VAR_0x800B // service row last used

// Bonding (called from the move tutor, so never 0x800B).
#define VAR_BONDING_CURSOR              VAR_0x8008 // value row last viewed
#define VAR_BONDING_MON                 VAR_0x800A // party slot being changed

// Center local guide.
#define VAR_GUIDE_CURSOR                VAR_0x8008 // topic last read
#define VAR_GUIDE_WARNED                VAR_0x8009 // legend warning given this conversation
#define VAR_GUIDE_HAS_LEGENDS           VAR_0x800A // this city has a Rare Pokémon lead
#define VAR_GUIDE_HAS_TIPS              VAR_0x800B // this city has Things to Do

// Berry Master (Route 123). Harvest Rewards calls the Harvest menu below.
#define VAR_BERRY_MASTER_CURSOR         VAR_0x8008 // service row last used
#define VAR_BERRY_GIFT_FIRST            VAR_0x800A // daily Berry Gifts, first Berry
#define VAR_BERRY_GIFT_SECOND           VAR_0x800B // daily Berry Gifts, second Berry

// Harvest menu (Harvest Pouch from the Bag, or the Berry Master's counter).
#define VAR_HARVEST_AT_BERRY_MASTER     VAR_0x8009 // TRUE when rewards can be claimed here
#define VAR_HARVEST_LIST_CURSOR         VAR_0x800A // Berry ID under the My Harvest cursor
#define VAR_HARVEST_REWARD              VAR_0x800B // reward row being read or claimed

// Center nurse. Each Center's map script passes the nurse's local id in
// VAR_0x800B (the whiteout respawn sets it natively); the travel papers she
// hands over first report a full Key Items pocket in 0x8009.
#define VAR_NURSE_LOCAL_ID              VAR_0x800B
#define VAR_TRAVEL_DOCS_PENDING         VAR_0x8009

// Evie's EV training (Fallarbor). VAR_0x8004 is the chosen party slot and
// VAR_0x8005 the stat, as the Inclement stat services take them.
#define VAR_EV_TRAINING_CURSOR          VAR_0x800A // stat row last trained
#define VAR_EV_TRAINING_COST            VAR_0x8009 // price of the chosen amount
#define VAR_EV_TRAINING_STAT            VAR_0x800B // the stat, while 0x8005 carries the price

// Ivy's Hidden Power (Fallarbor): the party slot, while the scroll menu
// borrows VAR_0x8004 for its menu id.
#define VAR_IV_TRAINING_MON             VAR_0x800A

#endif // GUARD_CONSTANTS_SERVICE_VARS_H
