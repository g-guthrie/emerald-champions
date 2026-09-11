#ifndef EC_RUNTIME_H
#define EC_RUNTIME_H
#include <stdint.h>
#include <stddef.h>
typedef uint32_t ECValue;
enum {EC_NIL,EC_INT,EC_STRING,EC_ARRAY,EC_OBJECT,EC_REF,EC_BOOL};
enum {EC_DONE,EC_TEXT,EC_SHOP,EC_MOTION,EC_NATIVE,EC_UNKNOWN,EC_LIMIT};
enum {EC_HP,EC_ATK,EC_DEF,EC_SPA,EC_SPD,EC_SPE,EC_STATS};
typedef struct {uint32_t magic,records,record_offset,string_offset,string_size,value_offset,value_size,items,vars,flags;} ECHeader;
typedef struct {const uint8_t *bytes;size_t size;} ECModel;
typedef struct {uint32_t record,pc;} ECFrame;
typedef struct {const char *key;const ECValue *values;uint32_t count;} ECInput;
typedef struct {
    ECModel model;
    ECValue *vars;
    uint32_t *bag,*pc_items;
    uint8_t *flags;
    uint32_t bag_limit,pc_limit;
    const ECInput *inputs;
    uint32_t input_count;
    ECFrame frame,stack[64];
    uint32_t depth;
    int comparison;
    ECValue argument;
    const char *operation;
    int reason;
} ECState;
int ec_open(ECModel *model,const void *bytes,size_t size);
size_t ec_memory_size(ECModel model);
int ec_init(ECState *state,ECModel model,void *memory,size_t size);
ECValue ec_read(ECState *state,uint32_t record);
ECValue ec_eval(ECState *state,ECValue expression);
ECValue ec_evolution(ECState *state,uint32_t species,const char *mode);
size_t ec_text(ECState *state,uint32_t record,char *output,size_t capacity);
ECValue ec_presets(ECModel model,uint32_t species,const char *format);
int ec_can_learn(ECModel model,uint32_t species,uint32_t move);
uint32_t ec_learning_moves(ECModel model,uint32_t species,const ECValue *known,uint32_t known_count,ECValue *moves,uint32_t capacity);
uint32_t ec_initial_moves(ECModel model,uint32_t species,uint32_t level,ECValue moves[4]);
int ec_stats(ECModel model,uint32_t species,uint32_t nature,uint32_t level,const uint16_t evs[EC_STATS],uint32_t stats[EC_STATS]);
int ec_write(ECState *state,uint32_t record,ECValue value);
uint32_t ec_slot(ECModel model,uint32_t record);
uint32_t ec_find(ECModel model,const char *key);
ECValue ec_record(ECModel model,uint32_t id);
ECValue ec_field(ECModel model,ECValue object,const char *key);
ECValue ec_at(ECModel model,ECValue array,uint32_t index);
uint32_t ec_count(ECModel model,ECValue array);
const char *ec_string(ECModel model,ECValue value);
int32_t ec_number(ECValue value);
void ec_start(ECState *state,uint32_t event);
int ec_step(ECState *state,uint32_t budget);
int ec_offer(ECState *state,uint32_t offer,uint32_t quantity);
int ec_check_offer(ECState *state,uint32_t offer,uint32_t quantity);
#endif
