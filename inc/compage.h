#ifndef _COMPAGE_H_
#define _COMPAGE_H_

#include <stdio.h>
#include <pthread.h>

struct configOption{
    char   *id;
    size_t  type;
    size_t  offset;
};

struct configBase{
    size_t  delimeter;
    char   *handlerId;
    void*  (*handler)(void*);
    void   *pdataDefault;
    size_t  pdataSize;
    struct configOption options[];
};

/* this structure is created with every working thread */
typedef struct compage_t{
    pthread_t         pid;             // id used for launching/releasing threads
    struct compage_t *next;            // we are using forwardly linked list format
    const char       *id;              // worker's id, as present in config file
    int               enabled;         // determines if worker should be enabled
    struct configBase   *recordCommon; // pointer to the common compage parameters
    struct configOption *recordConfig; // pointer to compage configuration
    void *(*handler)(void*);           // handler to call
    void *pdata;                       // private data structure to call handle with
} compage_t;


#ifdef __cplusplus
    #include <typeinfo>
    #define COMPAGE_TYPEID(x) *(short*)typeid(x).name()
#else
    #define COMPAGE_TYPEID(x) _Generic((x), \
        _Bool:    'b',   \
        int8_t:   'a',   \
        uint8_t:  'h',   \
        int16_t:  's',   \
        uint16_t: 't',   \
        int32_t:  'i',   \
        uint32_t: 'j',   \
        int64_t:  'l',   \
        uint64_t: 'm',   \
        float:    'f',   \
        double:   'd',   \
        char:     'a',   \
        char*:    0x6350,\
        default:  0xffffffff)
#endif

#define DELIMETER   0xdeadbeef

#define COMPAGE_DELIMETER(handler, delim) \
size_t handler##_delimeter __attribute__((section("compage"))) = \
delim;

#define COMPAGE_HANDLER_ID(handler) \
const char* handler##_handler_id __attribute__((section("compage"))) = \
#handler;

#define COMPAGE_HANDLER(handler) \
void* (*handler##_handler)(void*) __attribute__((section("compage"))) = \
handler;

#define COMPAGE_PDATA_DEFAULT(handler, pdata) \
void* handler##_pdata_default __attribute__((section("compage"))) = \
pdata;

#define COMPAGE_PDATA_SIZE(handler, pdata) \
size_t handler##_pdata_size __attribute__((section("compage"))) = \
sizeof(pdata);

#define COMPAGE_PDATA_ADD_CONFIG_ID(handler, config) \
const char* handler##_pdata_##config##_id __attribute__((section("compage"))) = \
#config; 

#define COMPAGE_PDATA_ADD_CONFIG_TYPE(handler, type, config) \
size_t handler##_pdata_##config##_type __attribute__((section("compage"))) = \
COMPAGE_TYPEID(((type*)0)->config);

#define COMPAGE_PDATA_ADD_CONFIG_OFFSET(handler, type, config) \
size_t handler##_pdata_##config##_offset __attribute__((section("compage"))) = \
(size_t)&((type*)0)->config;

/* register worker, the memory layout is as follows:
 * - delimeter (to distinguish workers)
 * - pointer to handler id
 * - pointer to handler function
 * - pointer to default pdata structure
 * - size of the pdata structure */
#define COMPAGE_REGISTER(handler, pdata) \
COMPAGE_DELIMETER(handler, DELIMETER)    \
COMPAGE_HANDLER_ID(handler)              \
COMPAGE_HANDLER(handler)                 \
COMPAGE_PDATA_DEFAULT(handler, &pdata)   \
COMPAGE_PDATA_SIZE(handler, pdata)

/* add structs elements configuration option, the memory layout is as follows:
 * - pointer to types ID string
 * - unsigned integer identifying type
 * - offset from the start of the structure */
#define COMPAGE_PDATA_ADD_CONFIG(handler, type, config) \
COMPAGE_PDATA_ADD_CONFIG_ID(handler, config)            \
COMPAGE_PDATA_ADD_CONFIG_TYPE(handler, type, config)    \
COMPAGE_PDATA_ADD_CONFIG_OFFSET(handler, type, config)

/* create default configuration file */
int compage_createDefaultConfig(const char *fpath);

/* initialize compage structure list from config file */
int compage_initFromConfig(const char *fpath);

/* initialize compage structure from default configuration */
//int compage_initFromDefault();

/* launch threads using pthreads API */
int compage_doPthreads();

/* just ptint whatever we have in the section */
void compage_debugSections(void);

#endif
