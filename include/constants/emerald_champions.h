#ifndef GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
#define GUARD_CONSTANTS_EMERALD_CHAMPIONS_H

#define EC_FINALE_LEAGUE 0
#define EC_FINALE_WALLY 1
#define EC_FINALE_VOYAGE 2
#define EC_FINALE_STEVEN 3
#define EC_FINALE_DEOXYS 4
#define EC_FINALE_BUFFEL 5
#define EC_FINALE_COMPLETE 6

#define EC_TRAINING_EVS 0
#define EC_TRAINING_IVS 1

#define EC_SOOT_TOTAL_MASK 0x7FFF
#define EC_SOOT_CORD_RECEIVED 0x8000 // Reserved receipt bit from the retired soot reward tiers.
#define EC_SOOT_MARSHADOW_TARGET 250

// Bottle Cap services. The scripts and native payment checks share these values.
#define EC_SOOT_PER_CAP                      500
#define EC_SOOT_CAP_BATCH                      5
#define EC_SOOT_EXCHANGE_ALL                   9 // Menu selection, not a quantity.
#define EC_IV_CHANGE_CAP_COST                  1
#define EC_HIDDEN_POWER_CAP_COST               3
#define EC_ICONIC_MOVE_CAP_COST               10
#define EC_ICONIC_MOVE_GOLD_CAP_COST            1
#define EC_EVIE_PRICE_PER_EV                   2

#define EC_SOOT_EXCHANGE_REFUSED               0
#define EC_SOOT_EXCHANGE_DELIVERED             1
#define EC_SOOT_EXCHANGE_BAG_FULL              2
#define EC_IV_SERVICE_UNCHANGED                0
#define EC_IV_SERVICE_CHANGED                  1
#define EC_IV_SERVICE_NOT_ENOUGH_CAPS          2

// Preserve fixed-width, spelled-out, and comma-formatted price text.
// Repricing must update that wording as well as the transaction constants.
#if EC_SOOT_PER_CAP != 500 || EC_SOOT_CAP_BATCH != 5
#error "Update the Glass Workshop's formatted soot prices when repricing."
#endif
#if EC_IV_CHANGE_CAP_COST != 1 || EC_HIDDEN_POWER_CAP_COST != 3 || EC_ICONIC_MOVE_GOLD_CAP_COST != 1 || EC_EVIE_PRICE_PER_EV != 2
#error "Update the service dialogue's fixed price wording when repricing."
#endif

#define EC_BATTLE_SET_FAILED                 0
#define EC_BATTLE_SET_SUCCESS                1
#define EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED  2
#define EC_BATTLE_SET_MEGA                   3
#define EC_BATTLE_SET_MEGA_STONE_HELD        4

#define EC_BATTLE_FORMAT_DOUBLES             0
#define EC_BATTLE_FORMAT_SINGLES             1
#define EC_BATTLE_FORMAT_COUNT               2

#define EC_BATTLE_ITEM_CATEGORY_OFFENSE 0
#define EC_BATTLE_ITEM_CATEGORY_DEFENSE 1
#define EC_BATTLE_ITEM_CATEGORY_FIELD 2
#define EC_BATTLE_ITEM_CATEGORY_TYPE 3
#define EC_BATTLE_ITEM_CATEGORY_GEM 4
#define EC_BATTLE_ITEM_CATEGORY_SPECIES 5
#define EC_BATTLE_ITEM_CATEGORY_COUNT 6
// Bounds the vendor's filtered stock buffer; the widest non-berry category.
#define EC_BATTLE_ITEM_MAX_CATEGORY 40

// Permanent two-starter opening transaction.
#define EC_OPENING_UNSELECTED                 0
#define EC_OPENING_PAIR_GRANTED               1
#define EC_OPENING_RESCUE_WON                 2

#define EMERALD_CHAMPIONS_MEGA_ONLY          TRUE

#define EC_HARVEST_LIMIT                    255
#define EC_HARVEST_REWARD_COUNT               4
#define EC_MEGA_BERRY_TRADE_SUCCESS           0
#define EC_MEGA_BERRY_TRADE_ALREADY_DONE      1
#define EC_MEGA_BERRY_TRADE_NOT_ENOUGH        2
#define EC_MEGA_BERRY_TRADE_BAG_FULL          3
#define EC_MEGA_BERRY_TRADE_INVALID           4
#define EC_MEGA_BERRY_TRADE_LOCKED            5

#endif // GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
