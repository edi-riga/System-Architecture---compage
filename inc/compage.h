#ifndef _COMPAGE_H_
#define _COMPAGE_H_

#include <stdio.h>
#include <pthread.h>

/* API:
 * =============================================================================
 * COMPAGE_REGISTER(handler, pdata)
 * =============================================================================
 * @brief registers component handler and its associated data structure with 
 *     the COMPAGE framework.
 *
 * @param handler - pointer to the component function (handler) with prototype: 
 *                  "void* (*handler)(void *pdata)
 * @param pdata   - pointer to (default) data structure which is expected as 
 *                  pdata
 *
 *
 * =============================================================================
 * COMPAGE_PDATA_ADD_CONFIGS(handler, type, ...)
 * =============================================================================
 * @brief marks pdata structure variables (single or multiple) with the 
 *     framework for the configuration.
 *
 * @param handler - pointer to the component function (handler) with prototype: 
 *                  "void* (*handler)(void *pdata)
 * @param type    - type of the struct container (often a type created with
 *                  typedef)
 * @param ...     - list of member variables which should be marked for 
 *                  configuration
 * */

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
(void*(*)(void*))handler;

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

#define COMPAGE_PDATA_ADD_CONFIGS_1(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config);
#define COMPAGE_PDATA_ADD_CONFIGS_2(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_1(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_3(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_2(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_4(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_3(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_5(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_4(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_6(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_5(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_7(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_6(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_8(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_7(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_9(handler, type, config, ...)  COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_8(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_10(handler, type, config, ...) COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_9(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_11(handler, type, config, ...) COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_10(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_12(handler, type, config, ...) COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_11(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_13(handler, type, config, ...) COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_12(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_14(handler, type, config, ...) COMPAGE_PDATA_ADD_CONFIG(handler, type, config); COMPAGE_PDATA_ADD_CONFIGS_13(handler, type, __VA_ARGS__)

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define ARGUMENT_COUNT(...) ARGUMENT_COUNT_(__VA_ARGS__, ARGUMENT_INDEXES())
#define ARGUMENT_COUNT_(...) ARGUMENT_COUNT_PREPEND(__VA_ARGS__)
#define ARGUMENT_COUNT_PREPEND(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, N, ...) N
#define ARGUMENT_INDEXES() 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define COMPAGE_PDATA_ADD_CONFIGS_(N, handler, type, ...) CONCATENATE(COMPAGE_PDATA_ADD_CONFIGS_,N)(handler, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS(handler, type, ...) COMPAGE_PDATA_ADD_CONFIGS_(ARGUMENT_COUNT(__VA_ARGS__), handler, type, __VA_ARGS__)


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

/* default compage framework entry point for convinience */
int compage_main(int argc, char *argv[]);

#endif
