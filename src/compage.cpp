#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ini/ini.h"
#include "utils.h"
extern "C" {
#include "compage.h"
}

/* we register a dummy component without an actual procedure, just to make sure
 * the compage section will exits */
COMPAGE_REGISTER(NULL)

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
    // debug
    _D("Type: 0x%x; Hex value: 0x%x", (unsigned)type, *(unsigned*)(addr));

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
        case 0x6350: /* char* */
            if( *(char**)addr == NULL ){   /* check if string is NULL */
                buf[0]='\0';
            } else{
                snprintf(buf, bufSize, "%s", *(char**)addr);
            }
            return buf;
        default:
            snprintf(buf, bufSize, "<Data type not supported>");
            return buf;
    }
}
 
/* convert representation from string to type */
static int compage_setValueType(void *dst, const void *src, size_t type){
    // debug
    _D("Type: 0x%x", (unsigned)type);

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
        case 0x6350: /* char* */
            if( ((char*)src)[0] == '\0' ){ /* check if string is supposed to be NULL */
                *(char**)dst = NULL;
            } else {
                *(char**)dst = strdup((char*)src);
            }
            return 0;
        default:
            return -1;
    }
}


static void compage_freeValue(void *dst, size_t type){
    // debug
    _D("Type: 0x%x", (unsigned)type);

    /* char* */
    if( (type == 0x6350) && (*(char**)dst) ){
          free(*(char**)dst);
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

        if(handlerProc == NULL)
            continue;

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

        if(handlerProc == NULL)
            continue;

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

static int compage_entryFreePdata(compage_t *entry){
    char *start = (char*)entry->recordConfig;
    char *stop  = (char*)compage_getSectionStop();

    while((start < stop) && (*(size_t*)start != DELIMETER)){
        char *configId = *(char**)start;
        start += sizeof(configId);

        size_t configType = *(size_t*)start;
        start += sizeof(configType);

        size_t configOffset = *(size_t*)start;
        start += sizeof(configOffset);

        compage_freeValue((char*)entry->pdata + configOffset, configType);
    }

    return 0;
}

static compage_t *compage_entryAlloc(const char *id){
    compage_t *entry = (compage_t*)malloc(sizeof(compage_t));
    entry->id     = strdup(id);

    entry->enabled      = 1;
    entry->next         = NULL;
    entry->recordCommon = NULL;
    entry->recordConfig = NULL;
    entry->handler      = NULL;

    return entry;
}

static void compage_entryDealloc(compage_t *entry){
    /* check if entry pdata has data to free */
    compage_entryFreePdata(entry);
    /* free entrie's ID */
    free((void*)entry->id);
    /* free the entry */
    free(entry);
}


static void compage_entryAdd(compage_t *entry){
    entry->next = head;
    head = entry;
}

static compage_t *compage_entryRemove(compage_t *entry){
    compage_t **indir = &head;

    while(*indir != entry)
        indir = &((*indir)->next);

    *indir = entry->next;
    return entry;
}

static int compage_entryParseEnable(compage_t *entry, const char *value){
    entry->enabled = atoi(value);
    return 0;
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

static compage_t **compage_entryFindReference(const char *id){
    compage_t **it = &head;

    while(*it != NULL){
        if(strcmp((*it)->id, id) == 0)
            return it;

        it = &((*it)->next);
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

static int compage_entryParseBase(compage_t *entry, struct configBase *cfg){
    entry->recordCommon = cfg;
    entry->recordConfig = cfg->options;
    entry->handler      = cfg->handler;

    /* duplicate pdata structure
     * what we do here might seem strange, but in fact, instead of allocating 
     * separate buffer for the pdata, we would like to increase the entry size
     * to and have pdata at the end of compage_t structure, thiw allows us to 
     * get data from the structure even when the user passes us pdata address
     * for more information, check the holy "container_of" macro (as used in 
     * Linux kernel)
     *
     * to achieve this, we first find the reference to the entry (this would be
     * the linked list's next element address), which we further reallocate */
    compage_t **entryRef = compage_entryFindReference(entry->id);
    if(entryRef == NULL){
        _E("Unexpected error during compage internal structure creation, exiting...");
        _exit(1);
    }

    /* now we have the reference, let's realloc
     * note that realloc keeps the contents of the memory */
    *entryRef = (compage_t*)realloc(*entryRef, sizeof(compage_t) + cfg->pdataSize);
    if(entryRef == NULL){
        _E("Realloc failed, data has been lost, exiting...");
        _exit(1);
    }

    /* finally, we just copy the default data structure */
    memcpy((*entryRef)->pdata, cfg->pdataDefault, cfg->pdataSize);
    return 0;
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
     * configuration section, here we try to find the structure and 
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

unsigned compage_getDefinedComponentCount(){
    unsigned componentCount = 0;
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

        if(handlerProc == NULL)
            continue;

        while((start < stop) && (*(size_t*)start != DELIMETER)){
            char *configId = *(char**)start;
            start += sizeof(configId);

            size_t configType = *(size_t*)start;
            start += sizeof(configType);

            size_t configOffset = *(size_t*)start;
            start += sizeof(configOffset);

            char buf[256];
            compage_getValueStr(buf, sizeof(buf), configType, (char*)pdataDefault + configOffset);
        }
        componentCount++;
    }

    return componentCount;
}

void compage_printComponentList(){
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

        if(handlerProc == NULL)
            continue;

        _I("COMPONENT: %s", handlerId);
        while((start < stop) && (*(size_t*)start != DELIMETER)){
            char *configId = *(char**)start;
            start += sizeof(configId);

            size_t configType = *(size_t*)start;
            start += sizeof(configType);

            size_t configOffset = *(size_t*)start;
            start += sizeof(configOffset);

            char buf[256];
            compage_getValueStr(buf, sizeof(buf), configType, (char*)pdataDefault + configOffset);
            _I(" - (config) %-20s => (default) %s", configId, buf);
        }

    }
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
        return -1;
    }

    return 0;
}


int compage_doPthreads(void){
    compage_entryPrint();

    compage_t *it = head;

    while(it != NULL){
        if(it->enabled){
            _I("Initializing thread %p(it) %p(handler), %p(pdata)", it, it->handler, it->pdata);
            int err = pthread_create(&(it->pid), NULL, it->handler, it->pdata);
            if(err != 0)
                _W("Failed to initialize pthread");
        }

        it = it->next;
    }

    return 0;
}

void compage_help(const char *appName){
    printf("USAGE:\n");
    printf("\t%s generate/gen <fname>  - generate default config file as <fname>\n", appName);
    printf("\t%s <fname>               - use <fname config file\n", appName);
    printf("\t%s list                  - list available components\n", appName);
}

int compage_main(int argc, char *argv[]){
    /* check if there is anything in compage segment */
    if(compage_getDefinedComponentCount() == 0){
        _I("No compage components have been defined");
        return 1;
    }

    /* parse input */
    if(argc < 2){
        compage_help(argv[0]);
        return 0;
    }

    /* generate config */
    if( (strcmp(argv[1], "generate") == 0) || (strcmp(argv[1], "gen") == 0)){
        if(argc < 3){
            compage_help(argv[0]);
            return 0;
        }
        return compage_createDefaultConfig(argv[2]);
    }

    /* list available components (TODO: generic command API) */
    if( (strcmp(argv[1], "list") == 0) ){
        compage_printComponentList();
        return 0;
    }

    _I("COMPAGE: loading configuration");
    if(compage_initFromConfig(argv[1]) == -1){
        _E("COMPAGE: failed to load configuration");
        return -1;
    }

    _I("COMPAGE: launching pthreads");
    if(compage_doPthreads() == -1){
        _E("COMPAGE: failed to launch pthreads"); 
        return -1;
    }

    _I("COMPAGE: main thread going to sleep");
    /* Actually let's wait for any thread to finish, if this happens => cleanup */
    compage_t *it = head;
    compage_t *it_cleanup;
    while( it!=NULL ){
		// join thread only if it is enabled
		if(it->enabled)
        	pthread_join(it->pid, NULL);

        it_cleanup = it;
        it = it->next;
        compage_entryDealloc(compage_entryRemove(it_cleanup));
    }

    return 0;
}
