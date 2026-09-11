#include "runtime.h"
#include <string.h>
#define TAG(v) ((v)>>28)
#define PAY(v) ((v)&0x0fffffff)
#define VAL(t,v) (((uint32_t)(t)<<28)|((uint32_t)(v)&0x0fffffff))
#define NONE UINT32_MAX
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint32_t header(ECModel m,unsigned i){return word(m.bytes+4*i);}
static const uint8_t *data(ECModel m,ECValue v){return m.bytes+header(m,5)+PAY(v);}
static const char *string(ECModel m,uint32_t offset){return (const char *)m.bytes+header(m,3)+offset;}
int ec_open(ECModel *m,const void *bytes,size_t size){
    if(size<40)return 0;
    m->bytes=bytes;m->size=size;
    if(header(*m,0)!=0x32474345)return 0;
    return header(*m,2)<=size && (uint64_t)header(*m,1)*12<=size-header(*m,2)
        && header(*m,3)<=size && header(*m,4)<=size-header(*m,3)
        && header(*m,5)<=size && header(*m,6)<=size-header(*m,5);
}
static uint32_t lower_bound(ECModel m,const char *key){
    uint32_t lo=0,hi=header(m,1);
    while(lo<hi){uint32_t mid=lo+(hi-lo)/2;int c=strcmp(key,string(m,word(m.bytes+header(m,2)+mid*12)));
        if(c<=0)hi=mid;else lo=mid+1;}
    return lo;
}
uint32_t ec_find(ECModel m,const char *key){
    uint32_t id=lower_bound(m,key);
    return id<header(m,1)&&!strcmp(key,string(m,word(m.bytes+header(m,2)+id*12)))?id:NONE;
}
ECValue ec_record(ECModel m,uint32_t id){return id<header(m,1)?word(m.bytes+header(m,2)+id*12+4):0;}
static ECValue deref(ECModel m,ECValue v){return TAG(v)==EC_REF?ec_record(m,PAY(v)):v;}
const char *ec_string(ECModel m,ECValue v){return TAG(v)==EC_STRING?string(m,PAY(v)):"";}
int32_t ec_number(ECValue v){return (int32_t)(v<<4)>>4;}
uint32_t ec_count(ECModel m,ECValue v){v=deref(m,v);return TAG(v)==EC_ARRAY||TAG(v)==EC_OBJECT?word(data(m,v)):0;}
ECValue ec_at(ECModel m,ECValue v,uint32_t i){v=deref(m,v);return TAG(v)==EC_ARRAY&&i<ec_count(m,v)?word(data(m,v)+4+i*4):0;}
ECValue ec_field(ECModel m,ECValue v,const char *key){
    v=deref(m,v);if(TAG(v)!=EC_OBJECT)return 0;
    for(uint32_t i=0;i<ec_count(m,v);i++){
        const uint8_t *p=data(m,v)+4+i*8;
        if(!strcmp(string(m,word(p)),key))return word(p+4);
    }return 0;
}
static const char *record_name(ECModel m,uint32_t id){return id<header(m,1)?string(m,word(m.bytes+header(m,2)+id*12)):"";}
int ec_stats(ECModel m,uint32_t species,uint32_t nature,uint32_t level,const uint16_t evs[EC_STATS],uint32_t stats[EC_STATS]){
    ECValue mon=ec_record(m,species),base=ec_field(m,mon,"stats"),bias=ec_field(m,ec_record(m,nature),"stats");
    if(ec_count(m,base)!=EC_STATS||ec_count(m,bias)!=2)return 0;
    uint32_t iv=ec_number(ec_field(m,ec_record(m,ec_find(m,"rule:stats")),"iv"));
    uint32_t up=ec_number(ec_at(m,bias,0)),down=ec_number(ec_at(m,bias,1));
    for(uint32_t i=0;i<EC_STATS;i++){
        uint32_t value=((2*ec_number(ec_at(m,base,i))+iv+evs[i]/4)*level)/100;
        if(i==EC_HP){ECValue fixed=ec_field(m,mon,"fixed_hp");value=fixed?(uint32_t)ec_number(fixed):value+level+10;}
        else {value+=5;if(up!=down)value=value*(i==up?110:i==down?90:100)/100;}
        stats[i]=value;
    }return 1;
}
static ECValue direct_presets(ECModel m,ECValue species,const char *format){
    return ec_field(m,ec_field(m,species,"preparation"),format);
}
ECValue ec_presets(ECModel m,uint32_t species,const char *format){
    ECValue mon=ec_record(m,species),sets=direct_presets(m,mon,format);
    if(ec_count(m,sets))return sets;
    ECValue family=ec_field(m,mon,"family");
    for(uint32_t i=0;i<ec_count(m,family);i++){
        sets=direct_presets(m,ec_at(m,family,i),format);if(ec_count(m,sets))return sets;
    }return 0;
}
static void mark_moves(ECModel m,ECValue moves,uint8_t *mask,uint32_t first,uint32_t count){
    for(uint32_t i=0;i<ec_count(m,moves);i++){
        ECValue move=ec_at(m,moves,i);uint32_t index=PAY(move)-first;
        if(TAG(move)==EC_REF&&index<count)mask[index/8]|=1u<<(index%8);
    }
}
static void learn_access(ECModel m,uint32_t species,uint8_t *mask,uint32_t first,uint32_t count){
    memset(mask,0,(count+7)/8);
    mark_moves(m,ec_field(m,ec_record(m,species),"pool"),mask,first,count);
    uint32_t none=ec_find(m,"move:NONE")-first;
    if(none<count)mask[none/8]&=~(1u<<(none%8));
}
int ec_can_learn(ECModel m,uint32_t species,uint32_t move){
    uint32_t first=lower_bound(m,"move:"),count=lower_bound(m,"move;")-first,index=move-first;
    if(index>=count)return 0;
    uint8_t mask[(count+7)/8];learn_access(m,species,mask,first,count);
    return (mask[index/8]>>(index%8))&1;
}
uint32_t ec_initial_moves(ECModel m,uint32_t species,uint32_t level,ECValue moves[4]){
    ECValue learn=ec_field(m,ec_record(m,species),"levelUpLearnset"),none=VAL(EC_REF,ec_find(m,"move:NONE"));
    uint32_t count=0;for(uint32_t i=0;i<4;i++)moves[i]=none;
    for(uint32_t i=0;i<ec_count(m,learn);i++){
        ECValue row=ec_at(m,learn,i),move=ec_at(m,row,1);uint32_t at=ec_number(ec_at(m,row,0));
        if(at>level)break;if(!at)continue;
        uint32_t slot=0;while(slot<count&&moves[slot]!=move)slot++;
        if(slot<count)continue;
        if(count<4)moves[count++]=move;
        else {for(uint32_t j=0;j<3;j++)moves[j]=moves[j+1];moves[3]=move;}
    }return count;
}
uint32_t ec_learning_moves(ECModel m,uint32_t species,const ECValue *known,uint32_t known_count,ECValue *moves,uint32_t capacity){
    uint32_t first=lower_bound(m,"move:"),count=lower_bound(m,"move;")-first,n=0;
    uint8_t mask[(count+7)/8+1];learn_access(m,species,mask,first,count);
    for(uint32_t i=0;i<known_count;i++){
        uint32_t index=PAY(known[i])-first;
        if(TAG(known[i])==EC_REF&&index<count)mask[index/8]&=~(1u<<(index%8));
    }
    for(uint32_t i=0;i<count;i++)if(mask[i/8]&(1u<<(i%8))){if(moves&&n<capacity)moves[n]=VAL(EC_REF,first+i);n++;}
    return n;
}
uint32_t ec_slot(ECModel m,uint32_t id){return word(m.bytes+header(m,2)+id*12+8);}
size_t ec_memory_size(ECModel m){return (size_t)header(m,7)*8+(size_t)header(m,8)*4+header(m,9);}
int ec_init(ECState *s,ECModel m,void *memory,size_t size){
    if(size<ec_memory_size(m))return 0;
    memset(s,0,sizeof(*s));memset(memory,0,ec_memory_size(m));s->model=m;
    s->bag=memory;s->pc_items=s->bag+header(m,7);s->vars=s->pc_items+header(m,7);
    s->flags=(uint8_t *)(s->vars+header(m,8));
    for(uint32_t i=0;i<header(m,8);i++)s->vars[i]=VAL(EC_INT,0);
    for(uint32_t i=0;i<header(m,1);i++){
        ECValue initial=ec_field(m,ec_record(m,i),"initial");if(initial)ec_write(s,i,initial);
    }
    uint32_t rule=ec_find(m,"rule:inventory");
    s->bag_limit=ec_number(ec_field(m,ec_record(m,rule),"bag_limit"));
    s->pc_limit=ec_number(ec_field(m,ec_record(m,rule),"pc_limit"));
    s->frame.record=NONE;return 1;
}
static int numeric(ECValue v){return TAG(v)==EC_INT||TAG(v)==EC_BOOL;}
static int equal(ECValue a,ECValue b){return numeric(a)&&numeric(b)?ec_number(a)==ec_number(b):a==b;}
static const ECInput *input(ECState *s,const char *key){
    for(uint32_t i=0;i<s->input_count;i++)if(!strcmp(key,s->inputs[i].key))return s->inputs+i;
    return NULL;
}
static ECValue number(int64_t n){return n>=-0x08000000&&n<=0x07ffffff?VAL(EC_INT,n):NONE;}
static int item_id(ECState *s,ECValue v,uint32_t *id){
    v=ec_eval(s,v);if(TAG(v)!=EC_REF||strncmp(record_name(s->model,PAY(v)),"item:",5))return 0;
    *id=ec_slot(s->model,PAY(v));return 1;
}
static int resource(ECState *s,ECValue v,uint32_t *kind,uint32_t *slot,uint32_t *limit){
    ECModel m=s->model;
    if(!strcmp(ec_string(m,ec_at(m,v,0)),"if")){
        ECValue test=ec_eval(s,ec_at(m,v,1));if(!numeric(test))return 0;
        return resource(s,ec_at(m,v,ec_number(test)?2:3),kind,slot,limit);
    }
    if(TAG(v)==EC_REF){
        uint32_t id=PAY(v);const char *name=record_name(m,id);
        if(!strncmp(name,"var:",4))*kind=0;
        else if(!strncmp(name,"flag:",5)&&!ec_field(m,v,"constant"))*kind=1;
        else return 0;
        *slot=ec_slot(m,id);ECValue max=ec_field(m,v,"max");
        *limit=*kind?1:max?ec_number(max):0x07ffffff;return 1;
    }
    const char *op=ec_string(m,ec_at(m,v,0));
    if(!strcmp(op,"bag"))*kind=2;else if(!strcmp(op,"pc"))*kind=3;else return 0;
    *limit=*kind==2?s->bag_limit:s->pc_limit;
    return item_id(s,ec_at(m,v,1),slot);
}
static ECValue resource_read(ECState *s,uint32_t kind,uint32_t slot){
    if(kind==0)return s->vars[slot];
    return VAL(EC_INT,kind==1?s->flags[slot]:kind==2?s->bag[slot]:s->pc_items[slot]);
}
static void resource_write(ECState *s,uint32_t kind,uint32_t slot,ECValue v){
    if(kind==0)s->vars[slot]=v;else if(kind==1)s->flags[slot]=ec_number(v)!=0;
    else if(kind==2)s->bag[slot]=ec_number(v);else s->pc_items[slot]=ec_number(v);
}
ECValue ec_eval(ECState *s,ECValue v){
    ECModel m=s->model;
    if(TAG(v)==EC_REF){uint32_t id=PAY(v);const char *name=record_name(m,id);
        if(!strncmp(name,"var:",4))return s->vars[ec_slot(m,id)];
        if(!strncmp(name,"flag:",5)){ECValue c=ec_field(m,v,"constant");return c?c:VAL(EC_BOOL,s->flags[ec_slot(m,id)]!=0);}
    }
    if(TAG(v)!=EC_ARRAY)return v;
    const char *op=ec_string(m,ec_at(m,v,0));ECValue a=ec_at(m,v,1),b=ec_at(m,v,2);
    if(!strcmp(op,"arg")){
        const ECInput *arg=input(s,ec_string(m,a));uint32_t i=b?ec_number(b):0;
        return arg&&i<arg->count?arg->values[i]:NONE;
    }
    if(!strcmp(op,"has")){
        const ECInput *arg=input(s,ec_string(m,a));b=ec_eval(s,b);
        if(!arg||b==NONE)return NONE;
        ECValue field=ec_at(m,v,3);
        for(uint32_t i=0;i<arg->count;i++){
            ECValue value=arg->values[i];if(field)value=ec_field(m,value,ec_string(m,field));
            if(equal(value,b))return VAL(EC_BOOL,1);
            for(uint32_t j=0;j<ec_count(m,value);j++)if(equal(ec_at(m,value,j),b))return VAL(EC_BOOL,1);
        }return VAL(EC_BOOL,0);
    }
    if(!strcmp(op,"in")){
        a=ec_eval(s,a);if(a==NONE)return NONE;
        for(uint32_t i=0;i<ec_count(m,b);i++)if(equal(a,ec_at(m,b,i)))return VAL(EC_BOOL,1);
        return VAL(EC_BOOL,0);
    }
    if(!strcmp(op,"field")){
        ECValue result=TAG(a)==EC_ARRAY?ec_eval(s,a):a;
        for(uint32_t i=2;i<ec_count(m,v);i++){
            ECValue key=ec_at(m,v,i);
            result=TAG(key)==EC_INT?ec_at(m,result,ec_number(key)):ec_field(m,result,ec_string(m,key));
        }return result?ec_eval(s,result):NONE;
    }
    if(!strcmp(op,"count")){
        uint32_t kind,slot,limit;
        return resource(s,a,&kind,&slot,&limit)?resource_read(s,kind,slot):NONE;
    }
    if(!strcmp(op,"if")){
        a=ec_eval(s,a);if(!numeric(a))return NONE;
        return ec_eval(s,ec_at(m,v,ec_number(a)?2:3));
    }
    if(!strcmp(op,"and")||!strcmp(op,"or")){
        int all=!strcmp(op,"and");
        for(uint32_t i=1;i<ec_count(m,v);i++){
            ECValue value=ec_eval(s,ec_at(m,v,i));if(!numeric(value))return NONE;
            if((ec_number(value)!=0)!=all)return VAL(EC_BOOL,!all);
        }return VAL(EC_BOOL,all);
    }
    a=ec_eval(s,a);if(a==NONE)return NONE;
    if(!strcmp(op,"not"))return numeric(a)?VAL(EC_BOOL,!ec_number(a)):NONE;
    b=ec_eval(s,b);if(b==NONE)return NONE;
    if(!strcmp(op,"eq")||!strcmp(op,"ne")){
        int same=equal(a,b);
        return VAL(EC_BOOL,!strcmp(op,"eq")?same:!same);
    }
    if(!numeric(a)||!numeric(b))return NONE;
    int64_t x=ec_number(a),y=ec_number(b);
    if(!strcmp(op,"ge"))return VAL(EC_BOOL,x>=y);if(!strcmp(op,"gt"))return VAL(EC_BOOL,x>y);
    if(!strcmp(op,"le"))return VAL(EC_BOOL,x<=y);if(!strcmp(op,"lt"))return VAL(EC_BOOL,x<y);
    if(!strcmp(op,"+"))return number(x+y);if(!strcmp(op,"-"))return number(x-y);
    if(!strcmp(op,"*"))return number(x*y);if(!strcmp(op,"min"))return number(x<y?x:y);
    if(!strcmp(op,"/"))return y?number(x/y):NONE;if(!strcmp(op,"%"))return y?number(x%y):NONE;
    if(!strcmp(op,"max"))return number(x>y?x:y);
    return NONE;
}
ECValue ec_read(ECState *s,uint32_t id){return ec_eval(s,VAL(EC_REF,id));}
int ec_write(ECState *s,uint32_t id,ECValue value){
    const char *name=record_name(s->model,id);
    if(!strncmp(name,"var:",4)){s->vars[ec_slot(s->model,id)]=value;return 1;}
    if(!strncmp(name,"flag:",5)){
        if(ec_field(s->model,ec_record(s->model,id),"constant"))return 1;
        if(!numeric(value))return 0;
        s->flags[ec_slot(s->model,id)]=ec_number(value)!=0;return 1;
    }return 0;
}
static int integer(ECState *s,ECValue v,int32_t *n){
    v=ec_eval(s,v);if(numeric(v)){*n=ec_number(v);return 1;}return 0;
}
static int compare(ECState *s,ECValue a,ECValue b){
    a=ec_eval(s,a);b=ec_eval(s,b);
    if(a==NONE||b==NONE)return 2;
    if(numeric(a)&&numeric(b)){
        int32_t x=ec_number(a),y=ec_number(b);return x<y?-1:x>y;
    }return a<b?-1:a>b;
}
static int condition(const char *name,int comparison){
    if(!strcmp(name,"eq"))return comparison==0;if(!strcmp(name,"ne"))return comparison!=0;
    if(!strcmp(name,"lt"))return comparison<0;if(!strcmp(name,"le"))return comparison<=0;
    if(!strcmp(name,"gt"))return comparison>0;if(!strcmp(name,"ge"))return comparison>=0;
    return -1;
}
static int transaction(ECState *s,uint32_t offer,uint32_t quantity,int apply){
    ECModel m=s->model;ECValue def=ec_record(m,offer),when=ec_field(m,def,"when");
    if(TAG(def)!=EC_OBJECT||!quantity)return -1;
    if(when){when=ec_eval(s,when);if(!numeric(when))return -1;if(!ec_number(when))return 0;}
    ECValue parts[]={ec_field(m,def,"pay"),ec_field(m,def,"gain"),ec_field(m,def,"set")};
    uint32_t capacity=ec_count(m,parts[0])+ec_count(m,parts[1])+ec_count(m,parts[2]),count=0;
    struct Change {uint32_t kind,slot,limit;ECValue before,after;int64_t delta,debit;int assigned;} changes[capacity?capacity:1];
    for(unsigned p=0;p<3;p++)for(uint32_t i=0;i<ec_count(m,parts[p]);i++){
        ECValue row=ec_at(m,parts[p],i),value=ec_eval(s,ec_at(m,row,1));uint32_t kind,slot,limit;
        if(!resource(s,ec_at(m,row,0),&kind,&slot,&limit)||value==NONE)return -1;
        uint32_t j=0;while(j<count&&(changes[j].kind!=kind||changes[j].slot!=slot))j++;
        if(j==count){changes[count++]=(struct Change){.kind=kind,.slot=slot,.limit=limit,.before=resource_read(s,kind,slot)};}
        struct Change *c=&changes[j];
        if(p==2){
            if(c->assigned||c->delta||c->debit)return -1;
            if(kind&&!numeric(value))return -1;
            c->assigned=1;c->after=value;
        }else{
            if(c->assigned||!numeric(value)||!numeric(c->before)||ec_number(value)<0)return -1;
            int64_t n=(int64_t)ec_number(value)*quantity;c->delta+=p?n:-n;if(!p)c->debit+=n;
        }
    }
    for(uint32_t i=0;i<count;i++){
        struct Change *c=&changes[i];
        if(!c->assigned){
            int64_t before=ec_number(c->before),after=before+c->delta;
            if(before<c->debit||after<0||after>c->limit)return 0;
            c->after=number(after);
        }else if(numeric(c->after)&&(ec_number(c->after)<0||(uint32_t)ec_number(c->after)>c->limit))return 0;
    }
    if(apply)for(uint32_t i=0;i<count;i++)resource_write(s,changes[i].kind,changes[i].slot,changes[i].after);
    return 1;
}
int ec_offer(ECState *s,uint32_t offer,uint32_t quantity){return transaction(s,offer,quantity,1);}
int ec_check_offer(ECState *s,uint32_t offer,uint32_t quantity){return transaction(s,offer,quantity,0);}
static void put(char *out,size_t cap,size_t *n,char c){if(*n+1<cap)out[*n]=c;(*n)++;}
size_t ec_text(ECState *s,uint32_t id,char *out,size_t cap){
    ECModel m=s->model;ECValue def=ec_record(m,id),slots=ec_field(m,def,"slots");
    const char *body=ec_string(m,ec_field(m,def,"body"));size_t n=0;
    while(*body){
        const char *end=*body=='{'?strchr(body,'}'):NULL;
        if(end){
            size_t length=end-body-1;char key[length+1];memcpy(key,body+1,length);key[length]=0;
            ECValue slot=ec_field(m,slots,key);
            if(slot){
                int upper=!strcmp(ec_string(m,ec_at(m,slot,0)),"upper");
                ECValue value=ec_eval(s,upper?ec_at(m,slot,1):slot);
                if(TAG(value)==EC_STRING){
                    for(const char *p=ec_string(m,value);*p;p++){char c=*p;if(upper&&c>='a'&&c<='z')c-=32;put(out,cap,&n,c);}
                }else if(numeric(value)){
                    int32_t x=ec_number(value);char digits[12];unsigned count=0;
                    if(x<0){put(out,cap,&n,'-');x=-x;}
                    do{digits[count++]=(char)('0'+x%10);x/=10;}while(x);
                    while(count)put(out,cap,&n,digits[--count]);
                }else return SIZE_MAX;
                body=end+1;continue;
            }
        }
        put(out,cap,&n,*body++);
    }
    if(cap)out[n<cap?n:cap-1]=0;return n;
}
static int set_result(ECState *s,ECValue value){
    uint32_t id=ec_find(s->model,"var:RESULT");if(id==NONE)return 0;s->vars[ec_slot(s->model,id)]=value;return 1;
}
void ec_start(ECState *s,uint32_t event){s->frame=(ECFrame){event,0};s->depth=0;s->reason=EC_DONE;s->argument=0;s->operation="";}
static int jump(ECState *s,ECValue target,int call){
    if(TAG(target)!=EC_REF)return 0;
    if(call){if(s->depth==64)return 0;s->stack[s->depth++]=s->frame;}
    s->frame=(ECFrame){PAY(target),0};return 1;
}
static int yield(ECState *s,int reason,ECValue arg,const char *op){s->reason=reason;s->argument=arg;s->operation=op;return reason;}
int ec_step(ECState *s,uint32_t budget){
    ECModel m=s->model;
    while(budget--&&s->frame.record!=NONE){
        ECValue block=ec_record(m,s->frame.record),ops=ec_at(m,block,0);
        if(s->frame.pc>=ec_count(m,ops)){
            ECValue next=ec_at(m,block,1);if(!jump(s,next,0))s->frame.record=NONE;continue;
        }
        ECValue ins=ec_at(m,ops,s->frame.pc++),a=ec_at(m,ins,1),b=ec_at(m,ins,2),c=ec_at(m,ins,3);
        const char *op=ec_string(m,ec_at(m,ins,0));int32_t x,y;
        if(!strcmp(op,"end")||!strcmp(op,"endram")){s->frame.record=NONE;continue;}
        if(!strcmp(op,"return")){s->frame=s->depth?s->stack[--s->depth]:(ECFrame){NONE,0};continue;}
        if(!strcmp(op,"goto")||!strcmp(op,"call")){if(!jump(s,a,!strcmp(op,"call")))goto unknown;continue;}
        if(!strcmp(op,"set")||!strcmp(op,"setvar")||!strcmp(op,"copyvar")||!strcmp(op,"addvar")||!strcmp(op,"subvar")){
            if(TAG(a)!=EC_REF||strncmp(record_name(m,PAY(a)),"var:",4))goto unknown;
            ECValue *v=&s->vars[ec_slot(s->model,PAY(a))];
            if(!strcmp(op,"setvar"))*v=b;
            else if(!strcmp(op,"copyvar")||!strcmp(op,"set")){ECValue value=ec_eval(s,b);if(value==NONE)goto unknown;*v=value;}
            else {if(!integer(s,*v,&x)||!integer(s,b,&y))goto unknown;*v=VAL(EC_INT,(uint16_t)(!strcmp(op,"addvar")?x+y:x-y));}
            continue;
        }
        if(!strcmp(op,"setflag")||!strcmp(op,"clearflag")){
            if(TAG(a)!=EC_REF||strncmp(record_name(m,PAY(a)),"flag:",5))goto unknown;
            ec_write(s,PAY(a),VAL(EC_BOOL,!strcmp(op,"setflag")));continue;
        }
        if(!strcmp(op,"compare")){s->comparison=compare(s,a,b);if(s->comparison==2)goto unknown;continue;}
        if(!strcmp(op,"checkflag")){if(!integer(s,a,&x))goto unknown;s->comparison=x?0:-1;continue;}
        if(!strncmp(op,"goto_if_",8)||!strncmp(op,"call_if_",8)){
            const char *cond=op+8;int yes;ECValue target;
            if(!strcmp(cond,"set")||!strcmp(cond,"unset")){
                if(!integer(s,a,&x))goto unknown;s->comparison=x?0:-1;
                yes=!strcmp(cond,"set")?x!=0:x==0;target=b;
            }else{
                if(ec_count(m,ins)==2)target=a;
                else {s->comparison=compare(s,a,b);if(s->comparison==2)goto unknown;target=c;}
                yes=condition(cond,s->comparison);if(yes<0)goto unknown;
            }
            if(yes&&!jump(s,target,op[0]=='c'))goto unknown;continue;
        }
        if(!strcmp(op,"switch")){ec_write(s,ec_find(m,"var:0x8000"),ec_eval(s,a));continue;}
        if(!strcmp(op,"case")){s->comparison=compare(s,ec_read(s,ec_find(m,"var:0x8000")),a);if(s->comparison==2)goto unknown;if(!s->comparison&&!jump(s,b,0))goto unknown;continue;}
        if(!strcmp(op,"offer")){
            if(TAG(a)!=EC_REF)goto unknown;int result=ec_offer(s,PAY(a),1);
            if(result<0)goto unknown;set_result(s,VAL(EC_BOOL,result));continue;
        }
        if(!strcmp(op,"msgbox")||!strcmp(op,"message"))return yield(s,EC_TEXT,ins,op);
        if(!strcmp(op,"pokemart"))return yield(s,EC_SHOP,a,op);
        if(!strcmp(op,"applymovement"))return yield(s,EC_MOTION,ins,op);
        if(!strcmp(op,"special")||!strcmp(op,"specialvar"))return yield(s,EC_NATIVE,ins,op);
unknown:
        s->frame.pc--;return yield(s,EC_UNKNOWN,ins,op);
    }
    return yield(s,s->frame.record==NONE?EC_DONE:EC_LIMIT,0,"");
}
