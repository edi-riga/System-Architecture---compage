#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include <unistd.h> // tmp

#include "ini/ini.h"

#include "compage.h"
#include "compage_hash.h"
#include "notification.h"

/* The compage framework depends on the existence of custom segments in the ELF
 * executable file: "copmage_ids", "compage_pdata", "compage_init",
 * "compage_loop", "compage_exit" and "compage_config". Linker creates unique
 * labels for each segment, such as "__start_compage_ids" and "__stop_compage_ids".
 * The framework uses these labels in the parser logic and if, for some reason,
 * the user has not defined any compage components, linkage of the executable
 * will fail. To avoid this, we add some dummy entries for every segment. */
compageId_t dummy_id __attribute__((used,section("compage_ids"))) =
  {NULL};
compagePdata_t dummy_pdata __attribute__((used,section("compage_pdata"))) =
  {NULL, NULL, 0};
compageInit_t dummy_init __attribute__((used,section("compage_init"))) =
  {NULL, NULL};
compageLoop_t dummy_loop __attribute__((used,section("compage_loop"))) =
  {NULL, NULL};
compageExit_t dummy_exit __attribute__((used,section("compage_exit"))) =
  {NULL, NULL};
compageConfig_t dummy_config __attribute__((used,section("compage_config"))) =
  {NULL, NULL, 0, 0};


/* We utilize a linked list structure for the compage components */
static compage_t *llistHead;

/* When using custom segments, linker creates start/stop labels that further can
 * be used for segment localization and in parser logic */
extern unsigned __start_compage_ids;
extern unsigned __stop_compage_ids;

extern unsigned __start_compage_pdata;
extern unsigned __stop_compage_pdata;

extern unsigned __start_compage_init;
extern unsigned __stop_compage_init;

extern unsigned __start_compage_loop;
extern unsigned __stop_compage_loop;

extern unsigned __start_compage_exit;
extern unsigned __stop_compage_exit;

extern unsigned __start_compage_config;
extern unsigned __stop_compage_config;


inline void* get_segment_ids_start(){
    return &__start_compage_ids;
}
inline void* get_segment_ids_stop(){
    return &__stop_compage_ids;
}
inline uint64_t get_segment_ids_size(){
    return (uint64_t)get_segment_ids_stop() - (uint64_t)get_segment_ids_start();
}

inline void* get_segment_pdata_start(){
    return &__start_compage_pdata;
}
inline void* get_segment_pdata_stop(){
    return &__stop_compage_pdata;
}
inline uint64_t get_segment_pdata_size(){
    return (uint64_t)get_segment_pdata_stop() - (uint64_t)get_segment_pdata_start();
}

inline void* get_segment_init_start(){
    return &__start_compage_init;
}
inline void* get_segment_init_stop(){
    return &__stop_compage_init;
}
inline uint64_t get_segment_init_size(){
    return (uint64_t)get_segment_init_stop() - (uint64_t)get_segment_init_start();
}

inline void* get_segment_loop_start(){
    return &__start_compage_loop;
}
inline void* get_segment_loop_stop(){
    return &__stop_compage_loop;
}
inline uint64_t get_segment_loop_size(){
    return (uint64_t)get_segment_loop_stop() - (uint64_t)get_segment_loop_start();
}

inline void* get_segment_exit_start(){
    return &__start_compage_exit;
}
inline void* get_segment_exit_stop(){
    return &__stop_compage_exit;
}
inline uint64_t get_segment_exit_size(){
    return (uint64_t)get_segment_exit_stop() - (uint64_t)get_segment_exit_start();
}

inline void* get_segment_config_start(){
    return &__start_compage_config;
}
inline void* get_segment_config_stop(){
    return &__stop_compage_config;
}
inline uint64_t get_segment_config_size(){
    return (uint64_t)get_segment_config_stop() - (uint64_t)get_segment_config_start();
}


/* ============================================================================ */
/* PRIVATE API (STATIC / PRIVATE TO THIS FILE) */
/* ============================================================================ */

static uint32_t get_component_count(){
  return get_segment_ids_size()/sizeof(compageId_t);
}

static void print_help_message(const char *appName){
  puts("USAGE:");
  printf("   %s -h, --help             - prints this message\n", appName);
  printf("   %s -g, --generate <fname> - generate default config file as <fname>\n", appName);
  printf("   %s -l, --list             - list available components\n", appName);
  printf("   %s <fname>                - use <fname> configuration file\n", appName);
}


/* convert representation from type to string */
static char *get_config_string_value(char *buf, size_t bufSize, size_t type, void *addr){
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
    case 0x4b50: /* const char* */
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
static int config_set_value_by_type(void *dst, const void *src, size_t type){
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
    case 0x4b50: /* const char* */
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




static compageId_t* locate_ids_segment(const char *name){
  compageId_t *start, *stop;

  start = (compageId_t*)get_segment_ids_start();
  stop  = (compageId_t*)get_segment_ids_stop();

  while(start < stop){
    if( strcmp(start->name, name) == 0){
      return start;
    }

    start++;
  }

  return NULL;
}

static compageConfig_t* locate_config_segment(const char *name){
  compageConfig_t *start, *stop;

  start = (compageConfig_t*)get_segment_config_start();
  stop  = (compageConfig_t*)get_segment_config_stop();

  while(start < stop){
    if(strcmp(start->name, name) == 0){
      return start;
    }

    start++;
  }

  return NULL;
}

static compagePdata_t* locate_pdata_segment(void *id){
  compagePdata_t *start, *stop;

  start  = (compagePdata_t*)get_segment_pdata_start();
  stop   = (compagePdata_t*)get_segment_pdata_stop();

  while(start < stop){
    if(start->id == id){
      return start;
    }

    start++;
  }

  return NULL;
}

// TODO: Unify handler locate functions
static compageInit_t* locate_init_segment(void *id){
  compageInit_t *start, *stop;

  start = (compageInit_t*)get_segment_init_start();
  stop  = (compageInit_t*)get_segment_init_stop();

  while(start < stop){
    if(start->id == id){
      return start;
    }

    start++;
  }

  return NULL;
}

static compageLoop_t* locate_loop_segment(void *id){
  compageLoop_t *start, *stop;

  start = (compageLoop_t*)get_segment_loop_start();
  stop  = (compageLoop_t*)get_segment_loop_stop();

  while(start < stop){
    if(start->id == id){
      return start;
    }

    start++;
  }

  return NULL;
}

static compageExit_t* locate_exit_segment(void *id){
  compageExit_t *start, *stop;

  start = (compageExit_t*)get_segment_exit_start();
  stop  = (compageExit_t*)get_segment_exit_stop();

  while(start < stop){
    if(start->id == id){
      return start;
    }

    start++;
  }

  return NULL;
}


static compageStatus_t write_default_config(FILE *fd){
  compageId_t *ids_start, *ids_stop;
  compagePdata_t *pdata; //*pdata_start, *pdata_stop;
  compageConfig_t *config_start, *config_stop;
  char buf[256];

  ids_start = (compageId_t*)get_segment_ids_start();
  ids_stop  = (compageId_t*)get_segment_ids_stop();

  while(ids_start < ids_stop){

    /* ignore dummy component */
    if(!ids_start->name){
      ids_start++;
      continue;
    }

    _D("Component added: %s@%p", ids_start->name, ids_start);

    /* write section's name that is also the component's name */
    if(fprintf(fd, "[%s]\n", ids_start->name) < 0){
      _SE("Failed to write to the configuration file");
      return COMPAGE_SYSTEM_ERROR;
    }

    /* write component's enable flag */
    if(fprintf(fd, "enabled=1\n") < 0){
      _SE("Failed to write to the configuration file");
      return COMPAGE_SYSTEM_ERROR;
    }

    /* find respective private data structure for further use */
    if( (pdata = locate_pdata_segment(ids_start)) == NULL ){
      putc('\n', fd);
      ids_start++; // no pdata found, go to the next component
      continue;
    }

    /* parse and save corresponding configurations */
    config_start = (compageConfig_t*)get_segment_config_start();
    config_stop  = (compageConfig_t*)get_segment_config_stop();
    while(config_start < config_stop){

      /* ignore dummy configuration and check if config corresponds to pdata */
      if( (config_start->pdata) == NULL || (config_start->pdata != pdata)){
        config_start++;
        continue;
      }

      /* write config's name and default value */
      _D("Configuration added: %s@%p (type:%lu)", config_start->name, config_start, config_start->type);
      fprintf(fd, "%s=%s\n", config_start->name,
        get_config_string_value(buf, sizeof(buf), config_start->type,
          (void*)(((uint64_t)pdata->addr)+config_start->offset)));

      config_start++;
    }
    putc('\n', fd);
    ids_start++;
  }


  return COMPAGE_SUCCESS;
}


static compage_t *llist_entry_alloc(){
  compage_t *entry;

  if( (entry = (compage_t*)calloc(1, sizeof(compage_t))) == NULL){
    _SE("Failed allocate memory");
    return NULL;
  }

  return entry;
}


static void llist_entry_dealloc(compage_t *entry){
  free(entry);
}


static void llist_entry_add(compage_t *entry){
  entry->next = llistHead;
  llistHead   = entry;
}


static void llist_entry_remove(compage_t *entry){
  compage_t **indir = &llistHead;

  while(*indir != entry)
    indir = &((*indir)->next);

  *indir = entry->next;
}


static int config_init_default(compage_t *entry,
  const char *componentName,
  const char *configName)
{
  compageId_t *id;
  compagePdata_t *pdata;

  /* find if the component's configuration exists */
  if( (id = locate_ids_segment(configName)) == NULL){
    _E("Failed to locate component's configuration");
    return 1;
  }

  /* find component's private data structure */
  if( (pdata = locate_pdata_segment(id) ) == NULL){
    _E("Failed to locate component's privatge data structure");
    return 1;
  }

  /* find component's handlers */
  compageInit_t *init = locate_init_segment(id);
  compageLoop_t *loop = locate_loop_segment(id);
  compageExit_t *exit = locate_exit_segment(id);
  if( !init || !loop || !exit ){
    _E("Failed to locate any of component's handlers");
    return 1;
  }

  /* At this point we have all the required data, lets continue with
   * the initialization of the default component's configuration */
  if( (entry->name = strdup(componentName)) == NULL){
    _SE("Failed allocate memory");
    return 1;
  }
  if( (entry->pdata = malloc(pdata->size)) == NULL){
    free(entry->name);
    _SE("Failed allocate memory");
    return 1;
  }

  memcpy(entry->pdata, pdata->addr, pdata->size);
  entry->enabled      = 1; // initially component is always enabled
  entry->compageId    = id;
  entry->compagePdata = pdata;
  entry->handlerInit  = init->handler;
  entry->handlerLoop  = loop->handler;
  entry->handlerExit  = exit->handler;
  return 0;
}

static int config_parse_key_value(compage_t *entry, const char *key, const char *value){
  compageConfig_t *config;

  if( (config = locate_config_segment(key)) == NULL){
    _W("Configuration for \"%s\" is not found", key);
    return 0; // TODO: for now don't fail the setup, but should we?
  }

  if(config_set_value_by_type((char*)entry->pdata + config->offset,
  value, config->type) != 0){
    _W("Failed to set configured value");
  }

  return 0;
}


static void llist_entry_deinit(compage_t *entry){
  free(entry->pdata);
  free(entry->name);
}



static compage_t* llist_entry_find_by_name(const char *name){
  compage_t *it = llistHead;

  while(it != NULL){
    if(strcmp(it->name, name) == 0){
      return it;
    }

    it = it->next;
  }

  return NULL;
}


static int ini_parser_handler(void *pdata,
  const char *section, const char *key, const char *value)
{

  /* try to find entry in the forwardly linked list, if entry is nonexistsant
   * allocate new compage_t entry, set section as its ID and add it to the
   * linked list */
  compage_t *entry = llist_entry_find_by_name(section);
  if(entry == NULL){
    if( (entry = llist_entry_alloc()) == NULL){
      _E("Failed to allocate compage entry");
      return 0;
    }

    /* fill the rest of the compage component's data structure */
    if( config_init_default(entry, section, section) != 0){
      _E("Failed to initialize compage component");
      llist_entry_dealloc(entry);
      return 0;
    }

    // add entry to the global llist
    llist_entry_add(entry);
  }


  /* "enable" may be present, can disable the worker, while keeping its
   * configuration in the file */
  if(strcmp("enabled", key) == 0){
    entry->enabled = atoi(value); // TODO: erro checking
    return 1;
  }


  /* other keys are believed to correspond with default configuration */
  if(config_parse_key_value(entry, key, value) != 0){
    return 0;
  }

  return 1;
}


/* ============================================================================ */
/* PUBLIC API */
/* ============================================================================ */


compageStatus_t compage_check_segments(){
  compageId_t *id_start;
  id_start = (compageId_t*)get_segment_ids_start();

  _D("Checking for duplicate components");
  for(unsigned li=0;    li<get_component_count(); li++){
  for(unsigned ri=li+1; ri<get_component_count(); ri++){

    // ignore dummy component
    if( (id_start[li].name == NULL) || (id_start[ri].name == NULL) ){
      continue;
    }

    if(strcmp(id_start[li].name, id_start[ri].name) == 0){
      _E("Found duplicate component IDs: \"%s\"", id_start[li].name);
      return COMPAGE_DUPLICATE_COMPONENTS;
    }
  }}


  // TODO: Other checks

  return COMPAGE_SUCCESS;
}


compageStatus_t compage_print_components(){
  compageId_t *ids_start, *ids_stop;
  compageConfig_t *config_start, *config_stop;
  compagePdata_t *pdata;
  char buf[256];

  ids_start = (compageId_t*)get_segment_ids_start();
  ids_stop  = (compageId_t*)get_segment_ids_stop();
  while(ids_start < ids_stop){

    /* ignore dummy component */
    if(!ids_start->name){
      ids_start++;
      continue;
    }

    printf("component: " COLOR_YELLOW "%s\n" COLOR_DEFAULT, ids_start->name);

    /* check whether component has a registered private data structure */
    if( (pdata = locate_pdata_segment(ids_start)) == NULL ){
      ids_start++;
      continue;
    }

    /* iterate through registered configuration */
    config_start = (compageConfig_t*)get_segment_config_start();
    config_stop  = (compageConfig_t*)get_segment_config_stop();
    while(config_start < config_stop){

      /* ignore dummy configuration and check if config corresponds to pdata */
      if( (config_start->pdata) == NULL || (config_start->pdata != pdata)){
        config_start++;
        continue;
      }

      printf("  - parameter: " COLOR_GRAY "%s" COLOR_DEFAULT
        " (type: 0x%.4lx; offset: %.3lu; default value: %s)\n",
        config_start->name,
        config_start->type,
        config_start->offset,
        get_config_string_value(buf, sizeof(buf), config_start->type,
          (void*)(((uint64_t)pdata->addr)+config_start->offset)));
      config_start++;
    }
    ids_start++;
  }

  return COMPAGE_SUCCESS;
}


compageStatus_t compage_generate_config(const char *fpath){
  compageStatus_t status;
  FILE *fd;

  fd = fopen(fpath, "w");
  if(fd == NULL){
    _SE("Failed to open configuration file");
    return COMPAGE_SYSTEM_ERROR;
  }

  status = write_default_config(fd);
  fclose(fd);
  return status;
}


compageStatus_t compage_init_from_file(const char *fpath){
  if(ini_parse(fpath, ini_parser_handler, NULL) < 0){
    _E("Failed to load \"%s\" configuration file", fpath);
    return COMPAGE_PARSER_ERROR;
  }

  return COMPAGE_SUCCESS;
}

void *pthread_handler(compage_t *entry){
  compageStatus_t status;

  /* initialization */
  if(entry->handlerInit){
    /* TODO: pre-execute handler */
    status = entry->handlerInit(entry->pdata);
    /* TODO: return value */
    /* TODO: post-handler */
  }


  /* loop */
  if(entry->handlerLoop){
    /* TODO: pre-execute handler */
    status = entry->handlerLoop(entry->pdata);
    /* TODO: return value */
    /* TODO: post-execute handler */
  }


  /* exit */
  if(entry->handlerExit){
    /* TODO: pre-execute handler */
    status = entry->handlerExit(entry->pdata);
    /* TODO: return value */
    /* TODO: post-execute handler */
  }

  return (void*)0;
}

compageStatus_t compage_launch_pthreads(){
  compage_t *it = llistHead;

  while(it != NULL){
    if(it->enabled){
      if(pthread_create(&(it->pid), NULL, (void*(*)(void*))pthread_handler, it) != 0){
        _W("Failed to initialize pthread");
      }
    }

    it = it->next;
  }

  return COMPAGE_SUCCESS;
}


compageStatus_t compage_main(int argc, char *argv[]){
  compageStatus_t status;

  printf("ID:     %lu bytes\n", get_segment_ids_size());
  printf("INIT:   %lu bytes\n", get_segment_init_size());
  printf("LOOP:   %lu bytes\n", get_segment_loop_size());
  printf("EXIT:   %lu bytes\n", get_segment_exit_size());
  printf("CONFIG: %lu bytes\n", get_segment_config_size());

  _D("Checking the added component count");
  if(get_component_count() <= 1){ // note, there is 1 dummy component
    _W("No COMPAGE components added to the framework");
    return COMPAGE_NO_COMPONENTS;
  }

  _D("Checking the integrity of the segment configuration");
  if( (status = compage_check_segments()) != COMPAGE_SUCCESS){
    _E("Segment sanity check failed");
    return status;
  }

  /* list of options */
  static struct option long_options[] = {
    {"help",      no_argument,       0, 'h'},
    {"list",      no_argument,       0, 'l'},
    {"generate",  required_argument, 0, 'g'},
    {0, 0, 0, 0}
  };

  _D("Parsing command line arguments");
  int c;
  while( (c = getopt_long(argc, argv, "hlg:", long_options, NULL)) != -1){
    switch(c){
      case 'l':  // LIST COMPONENTS
        return compage_print_components();

      case 'g':  // GENERATE CONFIGURATION FILE
        return compage_generate_config(optarg);

      case 'h':  // HELP
        print_help_message(argv[0]);
        return COMPAGE_SUCCESS;

      default:   // UNRECOGNIZED OPTION
        print_help_message(argv[0]);
        return COMPAGE_WRONG_ARGS;
    }
  }

  _D("Checking (last) configuration file argument");
  if(optind < argc){
    compage_init_from_file(argv[optind]); // TODO: error checking
  }

  _D("Launchig compage components using pthread API");
  compage_launch_pthreads();
  sleep(1);

  //print_help_message(argv[0]);
  return COMPAGE_SUCCESS;
}
