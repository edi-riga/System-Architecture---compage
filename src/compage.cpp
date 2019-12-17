#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "compage.h"
#include "ini/ini.h"
#include "utils.h"

/* when we use custom sections, linker creates these labels, which we can use
 * to localize section and calculate its size */
extern unsigned __start_compage;
extern unsigned __stop_compage;

inline void* compage_getSectionStart(){
    return &__start_compage;
}
inline void* compage_getSectionStop(){
    return &__stop_compage;
}
inline unsigned compage_getSectionSize(){
    return (uint64_t)compage_getSectionStop() - (uint64_t)compage_getSectionStart();
}


/* head of our list incorporating all workers */
static compage_t *head;


/* convert representation from type to string */
static char *compage_getValueStr(char *buf, size_t bufSize, size_t type, void *addr){
    switch(type){
        case 'b': /* bool     */
            snprintf(buf, bufSize, "%s", (*(int8_t*)addr) ? "true" : "false");
            return buf;
        case 'a': /* int8_t   */
            snprintf(buf, bufSize, "%d", *(int8_t*)addr);
            return buf;
        case 'h': /* uint8_t  */
            snprintf(buf, bufSize, "%u", *(uint8_t*)addr);
            return buf;
        case 's': /* int16_t  */
            snprintf(buf, bufSize, "%d", *(int16_t*)addr);
            return buf;
        case 't': /* uint16_t */
            snprintf(buf, bufSize, "%u", *(uint16_t*)addr);
            return buf;
        case 'i': /* int32_t  */
            snprintf(buf, bufSize, "%d", *(int32_t*)addr);
            return buf;
        case 'j': /* uint32_t */
            snprintf(buf, bufSize, "%u", *(uint32_t*)addr);
            return buf;
        case 'l': /* int64_t  */
            snprintf(buf, bufSize, "%ld", *(int64_t*)addr);
            return buf;
        case 'm': /* uint64_t */
            snprintf(buf, bufSize, "%lu", *(uint64_t*)addr);
            return buf;
        case 'f': /* float    */
            snprintf(buf, bufSize, "%f", *(float*)addr);
            return buf;
        case 'd': /* double   */
            snprintf(buf, bufSize, "%f", *(double*)addr);
            return buf;
        default:
            snprintf(buf, bufSize, "<Data type not supported>");
            return buf;
    }
}

/* convert representation from string to type */
static int compage_setValueType(void *dst, const void *src, size_t type){
    switch(type){
        case 'b': /* bool     */
            *(int8_t*)dst = atoi((char*)src);
            return 0;
        case 'a': /* int8_t   */
            *(int8_t*)dst = atoi((char*)src);
            return 0;
        case 'h': /* uint8_t  */
            *(uint8_t*)dst = atoi((char*)src);
            return 0;
        case 's': /* int16_t  */
            *(int16_t*)dst = atoi((char*)src);
            return 0;
        case 't': /* uint16_t */
            *(uint16_t*)dst = atoi((char*)src);
            return 0;
        case 'i': /* int32_t  */
            *(int32_t*)dst = atoi((char*)src);
            return 0;
        case 'j': /* uint32_t */
            *(uint32_t*)dst = atoi((char*)src);
            return 0;
        case 'l': /* int64_t  */
            *(int64_t*)dst = atoll((char*)src);
            return 0;
        case 'm': /* uint64_t */
            *(uint64_t*)dst = atoll((char*)src);
            return 0;
        case 'f': /* float    */
            *(float*)dst = atof((char*)src);
            return 0;
        case 'd': /* double   */
            *(double*)dst = atof((char*)src);
            return 0;
        default:
            return -1;
    }
}


/*************************** SUB-API - SECTIONS *******************************/
/* write configuration section's content into stream */
static void compage_sectionsToStream(FILE *stream){
    char *start = (char*)compage_getSectionStart();
    char *stop  = (char*)compage_getSectionStop();

    while(start < stop){
        size_t delimeter = *(size_t*)start;
        start += sizeof(delimeter);

        char *handlerId = *(char**)start;
        start += sizeof(handlerId);

        void *handlerProc = *(void**)start;
        start += sizeof(handlerProc);

        void *pdataDefault = *(void**)start;
        start += sizeof(pdataDefault);

        size_t pdataSize = *(size_t*)start;
        start += sizeof(pdataSize);

        fprintf(stream, "[%s]\n", handlerId);
        fprintf(stream, "handler=%s\n", handlerId);
        fprintf(stream, "enabled=1\n");

        while((start < stop) && (*(size_t*)start != DELIMETER)){
            char buf[256];

            char *configId = *(char**)start;
            start += sizeof(configId);

            size_t configType = *(size_t*)start;
            start += sizeof(configType);

            size_t configOffset = *(size_t*)start;
            start += sizeof(configOffset);

            fprintf(stream, "%s=%s\n", configId, 
                compage_getValueStr(buf, 256, configType, 
                (void*)((long unsigned int)pdataDefault + configOffset)));
        }
        putc('\n', stream);
    }
}

// TODO: optimize
/**/
static void *compage_sectionsFindConfig(const char *id){
    char *start = (char*)compage_getSectionStart();
    char *stop  = (char*)compage_getSectionStop();

    while(start < stop){
        size_t delimeter = *(size_t*)start;
        start += sizeof(delimeter);

        char *handlerId = *(char**)start;
        start += sizeof(handlerId);

        if(strcmp(handlerId, id) == 0)
            return start - sizeof(handlerId) - sizeof(delimeter);

        void *handlerProc = *(void**)start;
        start += sizeof(handlerProc);

        void *pdataDefault = *(void**)start;
        start += sizeof(pdataDefault);

        size_t pdataSize = *(size_t*)start;
        start += sizeof(pdataSize);

        while((start < stop) && (*(size_t*)start != DELIMETER)){
            char *configId = *(char**)start;
            start += sizeof(configId);

            size_t configType = *(size_t*)start;
            start += sizeof(configType);

            size_t configOffset = *(size_t*)start;
            start += sizeof(configOffset);
        }
    }

    return NULL;
}

/**************************** SUB-API - ENTRY *********************************/
static compage_t *compage_entryAlloc(const char *id){
    compage_t *entry = (compage_t*)malloc(sizeof(compage_t));
    entry->id     = strdup(id);

    entry->enabled      = 1;
    entry->next         = NULL;
    entry->recordCommon = NULL;
    entry->recordConfig = NULL;
    entry->handler      = NULL;
    entry->pdata        = NULL;

    return entry;
}

static void compage_entryDealloc(compage_t *entry){
    // TODO: error checking
    free(entry->pdata);
    free(entry);
}


static void compage_entryAdd(compage_t *entry){
    entry->next = head;
    head = entry;
}

static void compage_entryRemove(compage_t *entry){
    compage_t **indir = &head;

    while(*indir != entry)
        indir = &((*indir)->next);

    *indir = entry->next;
}


static int compage_entryParseBase(compage_t *entry, struct configBase *cfg){
    entry->recordCommon = cfg;
    entry->recordConfig = cfg->options;
    entry->handler      = cfg->handler;

    // duplicate pdata structure (TODO: error checking)
    entry->pdata  = (void*)malloc(cfg->pdataSize);
    memcpy(entry->pdata, cfg->pdataDefault, cfg->pdataSize);
    return 0;
}

static int compage_entryParseEnable(compage_t *entry, const char *value){
    entry->enabled = atoi(value);
    return 0;
}

static int compage_entryParsePdata(compage_t *entry, const char *key, const char *value){
    char *start = (char*)entry->recordConfig;
    char *stop  = (char*)compage_getSectionStop();

    while((start < stop) && (*(size_t*)start != DELIMETER)){
        char *configId = *(char**)start;
        start += sizeof(configId);

        size_t configType = *(size_t*)start;
        start += sizeof(configType);

        size_t configOffset = *(size_t*)start;
        start += sizeof(configOffset);

        if(strcmp(configId, key) == 0){
            compage_setValueType(
                (char*)entry->pdata + configOffset,
                value,
                configType);
            return 0;
        }
    }

    return -1;
}

static compage_t *compage_entryFind(const char *id){
    compage_t *it = head;

    while(it != NULL){
        if(strcmp(it->id, id) == 0)
            return it;

        it = it->next;
    }

    return NULL;
}

static void compage_entryPrint(){
    compage_t *it = head;

    while(it != NULL){
        _I("Entry ID: %s; Handler ID: %s; Pdata size: %lu bytes",
            it->id,
            it->recordCommon->handlerId,
            it->recordCommon->pdataSize);

        it = it->next;
    }
}


static int compage_iniParser(void *pdata, const char *section, const char *key,
const char *value){

    /* try to find entry in the forwardly linked list, if entry is nonexistsant 
     * allocate new compage_t entry, set section as its ID and add it to the 
     * linked list */
    compage_t *entry = compage_entryFind(section);
    if(entry == NULL){
        entry = compage_entryAlloc(section);
        if(entry == NULL){
            _E("Failed to allocate compage entry");
            return 0;
        }

        compage_entryAdd(entry);
    }

    /* "handler" is obligatory key, which allows us to identify worker's 
     * configuration section, here we find try to find the structure and 
     * initialize compage structure as much as possible */
    if(strcmp("handler", key) == 0){
        struct configBase *config = (struct configBase*)compage_sectionsFindConfig(value);
        if(config == NULL){
            _E("Failed to find configuration");
            return 0;
        }

        compage_entryParseBase(entry, config);
    } 
    /* "enable" may be present, can disable the worker, while keeping its
     * configuration in the file */
    else if(strcmp("enabled", key) == 0){
        compage_entryParseEnable(entry, value);
    }
    /* other keys are believed to correspond with default configuration */
    else{
        compage_entryParsePdata(entry, key, value);
    }

    return 1;
}

void compage_debugSections(){
    compage_sectionsToStream(stdout);
}

int compage_createDefaultConfig(const char *fpath){
    FILE *fd;

    fd = fopen(fpath, "w");
    if(fd == NULL){
        _E("Failed to open configuration file");
        return -1;
    }

    compage_sectionsToStream(fd);
    fclose(fd);
    return 0;
}

int compage_initFromConfig(const char *fpath){
    if(ini_parse(fpath, compage_iniParser, NULL) < 0){
        _E("Failed to load \"%s\" configuration file", fpath);
        return 0;
    }

    return -1;
}


int compage_doPthreads(void){
    compage_entryPrint();

    compage_t *it = head;

    while(it != NULL){
        _I("Initializing thread %p(it) %p(handler), %p(pdata)", it, it->handler, it->pdata);
        int err = pthread_create(&(it->pid), NULL, it->handler, it->pdata);
        if(err != 0)
            _W("Failed to initialize pthread");

        it = it->next;
    }

    return 0;
}
