#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>

#include "ini/ini.h"

#include "compage.h"
#include "compage_hash.h"
#include "compage_macro.h"
#include "compage_llist.h"
#include "compage_config.h"
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
/* We support multiple segments with the same name so we maintain unique ids */
static unsigned entry_id = 0;
/* Shared handlers for monitoring the component lifecyles */
static compageCallback_t callbacks[COMPAGE_CALLBACK_COUNT];
/* We maintain a list of state string representations for convenience */
static const char *state_representations[] = {
  "IDLE",                      // COMPAGE_STATE_IDLE
  "PREINIT",                   // COMPAGE_STATE_PREINIT
  "INIT",                      // COMPAGE_STATE_INIT
  "POSTINIT",                  // COMPAGE_STATE_POSTINIT
  "PRELOOP",                   // COMPAGE_STATE_PRELOOP
  "LOOP",                      // COMPAGE_STATE_LOOP
  "POSTLOOP",                  // COMPAGE_STATE_POSTLOOP
  "PREEXIT",                   // COMPAGE_STATE_PREEXIT
  "EXIT",                      // COMPAGE_STATE_EXIT
  "POSTEXIT",                  // COMPAGE_STATE_POSTEXIT
  "COMPLETED",                 // COMPAGE_STATE_COMPLETED_SUCCESS
  "COMPLETED (WITH FAILURE)",  // COMPAGE_STATE_COMPLETED_FAILURE
  "ILLEGAL",                   // COMPAGE_STATE_ILLEGAL
};


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

static void print_help_message(const char *appName){  // refactor
  puts("USAGE:");
  printf("   %s -h, --help             - prints this message\n", appName);
  printf("   %s -d, --default          - run application with default configuration\n", appName);
  printf("   %s -g, --generate <fname> - generate default config file as <fname>\n", appName);
  printf("   %s -l, --list             - list available components\n", appName);
  printf("   %s <fname>                - use <fname> configuration file\n", appName);
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

static compageConfig_t* locate_config_segment(compagePdata_t *pdata, const char *name){
  compageConfig_t *start, *stop;

  start = (compageConfig_t*)get_segment_config_start();
  stop  = (compageConfig_t*)get_segment_config_stop();

  while(start < stop){
    if( (start->pdata == pdata) && (strcmp(start->name, name) == 0) ){
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
        compage_cfg_get_string(buf, sizeof(buf), config_start->type,
          (void*)(((uint64_t)pdata->addr)+config_start->offset)));

      config_start++;
    }
    putc('\n', fd);
    ids_start++;
  }


  return COMPAGE_SUCCESS;
}


static int config_init_default(compage_t **entry,
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

  /* allocate entry structure with additional space for pdata struct */
  *entry = (compage_t*)calloc(1, sizeof(compage_t)+pdata->size);
  if( *entry == NULL ){
    _SE("Failed allocate memory");
    return 1;
  }

  /* find component's handlers */
  compageInit_t *init = locate_init_segment(id);
  compageLoop_t *loop = locate_loop_segment(id);
  compageExit_t *exit = locate_exit_segment(id);
  if( !init && !loop && !exit ){
    _E("Failed to locate any of component's handlers");
    free(*entry);
    return 1;
  }

  /* At this point we have all the required data, lets continue with
   * the initialization of the default component's configuration */
  if( ((*entry)->name = strdup(componentName)) == NULL){
    _SE("Failed allocate memory");
    free(*entry);
    return 1;
  }

  memcpy((*entry)->pdata, pdata->addr, pdata->size);
  (*entry)->enabled      = 1; // initially component is always enabled
  (*entry)->compageId    = id;
  (*entry)->compagePdata = pdata;
  (*entry)->handlerInit  = (init) ? init->handler : NULL;
  (*entry)->handlerLoop  = (loop) ? loop->handler : NULL;
  (*entry)->handlerExit  = (exit) ? exit->handler : NULL;
  (*entry)->sid          = (*entry)->name;
  return 0;
}

static int config_parse_key_value(compage_t *entry, const char *key, const char *value){
  compageConfig_t *config;

  if( (config = locate_config_segment(entry->compagePdata, key)) == NULL){
    _W("Configuration for \"%s\" is not found", key);
    return 0; // TODO: for now don't fail the setup, but should we?
  }

  if(compage_cfg_set_value((char*)entry->pdata + config->offset,
  value, config->type) != 0){
    _W("Failed to set configured value");
  }

  return 0;
}


static void llist_entry_deinit(compage_t *entry){
  if(entry->sid != entry->name){
    free(entry->sid);
  }
  free(entry->name);
  free(entry);
}


static int ini_parser_handler(void *pdata,
  const char *section, const char *key, const char *value, int is_new_section)
{
  _D("INI Section: %s; Key: %s; Value: %s; New section: %d",
    section, key, value, is_new_section);

  /* we support multiple segments with the same name, but because segment name
   * is also component's string-id we need a unique identification mechanism
   * for all the components. Here we solve this issue by just asigning a unique
   * number to each component that is incremented with every new section */
  if(is_new_section){
    entry_id++;
  }

  /* try to find entry in the forwardly linked list, if entry is nonexistsant
   * allocate new compage_t entry, set section as its ID and add it to the
   * linked list */
  compage_t *entry = llist_entry_find_by_id(llistHead, entry_id);
  if(entry == NULL){
    /* allocate and fill the compage component's data structure */
    if( config_init_default(&entry, section, section) != 0){
      _E("Failed to initialize compage component");
      return 0;
    }

    /* set entrie's unique ID*/
    entry->id = entry_id;

    /* add entry to the global llist */
    llist_entry_add(&llistHead, entry);
  }


  /* "enable" may be present, can disable the worker, while keeping its
   * configuration in the file */
  if(strcmp("enabled", key) == 0){
    entry->enabled = atoi(value); // TODO: erro checking
    return 1;
  }

  /* "sid" key represents component's "string-name", which can be used later for
   * readable component identification and logging */
  if(strcmp("sid", key) == 0){
    entry->sid = strdup(value);
    if(!entry->sid){
      _SE("Failed allocate memory");
      return 1;
    }
    return 0;
  }

  /* other keys should correspond to default configuration */
  if(config_parse_key_value(entry, key, value) != 0){
    return 0;
  }

  return 1;
}


static compageStatus_t compage_check_segments(){
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

/* ============================================================================ */
/* PTHREAD HANDLERS */
/* ============================================================================ */
static inline void pthread_handler_execute_callback(compage_t *entry,
 compageState_t state, compageCallbackType_t callback_type){
  if(callbacks[callback_type].handler){
    entry->state = state;
    callbacks[callback_type].handler(callbacks[callback_type].arg, entry->pdata);
  }
}

static void pthread_handler_cleanup(compage_t *entry){
  _D("Pthread cleanup function called for \"%s\" component", entry->sid);

  /* there is nothing to clean */
  if(entry->state < COMPAGE_STATE_INIT){
    return;
  }

  /* we have been stopped in the middle of initialization stage, generally we
   cannot run deinitialize handler */
  if(entry->state == COMPAGE_STATE_INIT){
    _W("Requested cleanup during \"%s\" initialization, exit handler will not"
      " be called", entry->sid);
    return;
  }

  /* we have been stopped in the middle of initialization stage, generally we
   cannot run deinitialize handler */
  if(entry->state == COMPAGE_STATE_EXIT){
    _W("Requested cleanup during ongoing \"%s\" cleanup, might stay initialized",
       entry->sid);
    return;
  }

  /* check if we are in states where data is still initialized */
  if((entry->state >= COMPAGE_STATE_POSTINIT)
  && (entry->state <= COMPAGE_STATE_PREEXIT)){
    if(entry->handlerExit){
      entry->handlerExit(entry->pdata);
    }
  }
}

static void *pthread_handler(compage_t *entry){
  compageStatus_t status;

  /* setup pthread cancellation */
  if( pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0){
    _W("Failed to enable thread cancellation");
  }
  if( pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0){
    _W("Failed to setup asynchronous cancellation mode");
  }
  pthread_cleanup_push( (void (*)(void*))pthread_handler_cleanup, entry);

  /* initialization: pre-callback, handler, post-callback */
  if(entry->handlerInit){
    pthread_handler_execute_callback(entry,
      COMPAGE_STATE_PREINIT, COMPAGE_CALLBACK_PREINIT);

    entry->state = COMPAGE_STATE_INIT;
    status = entry->handlerInit(entry->pdata);
    if(status != COMPAGE_SUCCESS){
      entry->state = COMPAGE_STATE_COMPLETED_FAILURE;
      return (void*)status; // TODO: utilize signaling mechanism for handling
    }

    pthread_handler_execute_callback(entry,
      COMPAGE_STATE_POSTINIT, COMPAGE_CALLBACK_POSTINIT);
  }

  /* loop (control): pre-callback, handler, post-callback */
  if(entry->handlerLoop){
    do{
      pthread_handler_execute_callback(entry,
        COMPAGE_STATE_PRELOOP, COMPAGE_CALLBACK_PRELOOP);

      entry->state = COMPAGE_STATE_LOOP;
      status = entry->handlerLoop(entry->pdata);

      pthread_handler_execute_callback(entry,
        COMPAGE_STATE_POSTLOOP, COMPAGE_CALLBACK_POSTLOOP);
    } while(status == COMPAGE_SUCCESS);

    if(status != COMPAGE_EXIT_LOOP){
      entry->state = COMPAGE_STATE_COMPLETED_FAILURE;
      return (void*)status; // TODO: utilize signaling mechanism for handling
    }
  }

  /* exit: pre-callback, handler, post-callback */
  if(entry->handlerExit){
    pthread_handler_execute_callback(entry,
      COMPAGE_STATE_PREEXIT, COMPAGE_CALLBACK_PREEXIT);

    entry->state = COMPAGE_STATE_EXIT;
    status = entry->handlerExit(entry->pdata);
    if(status != COMPAGE_SUCCESS){
      entry->state = COMPAGE_STATE_COMPLETED_FAILURE;
      return (void*)status; // TODO: utilize signaling mechanism for handling
    }

    pthread_handler_execute_callback(entry,
      COMPAGE_STATE_POSTEXIT, COMPAGE_CALLBACK_POSTEXIT);
  }

  pthread_cleanup_pop(1); // here we actually don't require a handler pop

  entry->state = COMPAGE_STATE_COMPLETED_SUCCESS;
  return (void*)0;
}


/* ============================================================================ */
/* PUBLIC API */
/* ============================================================================ */
compageStatus_t compage_init_from_file(const char *fpath){
  if(ini_parse(fpath, ini_parser_handler, NULL) < 0){
    _E("Failed to load \"%s\" configuration file", fpath);
    return COMPAGE_PARSER_ERROR;
  }

  return COMPAGE_SUCCESS;
}

compageStatus_t compage_init_default(){
  compageId_t *ids_start, *ids_stop;

  ids_start = (compageId_t*)get_segment_ids_start();
  ids_stop  = (compageId_t*)get_segment_ids_stop();
  while(ids_start < ids_stop){

    /* ignore dummy component */
    if(!ids_start->name){
      ids_start++;
      continue;
    }

    compage_t *entry;
    if( config_init_default(&entry, ids_start->name, ids_start->name) != 0){
      _E("Failed to initialize compage component");
      return COMPAGE_PARSER_ERROR;
    }

    /* set entrie's unique ID*/
    entry->id = entry_id++;

    /* add entry to the global llist */
    llist_entry_add(&llistHead, entry);

    ids_start++;
  }

  return COMPAGE_SUCCESS;
}

void compage_deinit(){
  while(llistHead != NULL){
    llist_entry_deinit(llist_entry_remove(&llistHead));
  }
}


compageStatus_t compage_print_components(const char *config_file){
  compageConfig_t *config_start, *config_stop;
  compageStatus_t status;
  compage_t *it;
  char buf[256];

  /* initialize the compage component linked list, use either the supplied
   * configuration file of initialize list from the default configuration in
   * the elf compage_* sections */
  if(config_file){
    status = compage_init_from_file(config_file);
  } else {
    status = compage_init_default();
  }

  if(status != COMPAGE_SUCCESS){
    _E("Failed to initialize compage component list");
    return status;
  }

  /* iterate through all components in the linked list */
  it = llistHead;
  while(it != NULL){

    /* print component's string id, universal id and name */
    printf("component: " COLOR_YELLOW "%s " COLOR_DEFAULT "(uid=%u, name=%s)\n",
      it->sid, it->id, it->name);

    /* iterate through all configs and identify the relevant configurations
     * for this particular component. The config section holds necessary info
     * regarding the actual layout of the pdata struct (just the configurable
     * parts) */
    config_start = (compageConfig_t*)get_segment_config_start();
    config_stop  = (compageConfig_t*)get_segment_config_stop();
    while(config_start < config_stop){

      /* ignore dummy configuration and check if config corresponds to pdata */
      if( (config_start->pdata) == NULL || (config_start->pdata != it->compagePdata)){
        config_start++;
        continue;
      }

      /* print the actula parameter */
      printf("  - parameter: " COLOR_GRAY "%s" COLOR_DEFAULT
        " (type: 0x%.4lx; offset: %.3lu; default value: %s)\n",
        config_start->name,
        config_start->type,
        config_start->offset,
        compage_cfg_get_string(buf, sizeof(buf), config_start->type,
          (void*)(((uint64_t)it->pdata)+config_start->offset)));

        config_start++;
    }

    it = it->next;
  }

  /* deinitialize compage linked list */
  compage_deinit();
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



compageStatus_t compage_launch(){
  compage_t *it = llistHead;

  while(it != NULL){
    if(it->enabled){
      if(pthread_create(&(it->pid), NULL, (void*(*)(void*))pthread_handler, it) != 0){
        _E("Failed to initialize pthread");
        return COMPAGE_FAILED_LAUNCH;
      }
      it->launched = 1;
    }

    it = it->next;
  }

  return COMPAGE_SUCCESS;
}

compageStatus_t compage_launch_by_name(const char *name){
  compage_t *entry = llist_entry_find_by_name(llistHead, name);
  if(entry == NULL){
    return COMPAGE_PARSER_ERROR;
  }

  if(pthread_create(&(entry->pid),NULL, (void*(*)(void*))pthread_handler,entry) != 0){
    _E("Failed to initialize pthread");
    return COMPAGE_FAILED_LAUNCH;
  }
  entry->launched = 1;

  return COMPAGE_SUCCESS;
}

compageStatus_t compage_launch_by_sid(const char *sid){
  compage_t *entry = llist_entry_find_by_sid(llistHead, sid);
  if(entry == NULL){
    return COMPAGE_PARSER_ERROR;
  }

  if(pthread_create(&(entry->pid),NULL, (void*(*)(void*))pthread_handler,entry) != 0){
    _E("Failed to initialize pthread");
    return COMPAGE_FAILED_LAUNCH;
  }
  entry->launched = 1;

  return COMPAGE_SUCCESS;
}

compageStatus_t compage_launch_by_id(unsigned id){
  compage_t *entry = llist_entry_find_by_id(llistHead, id);
  if(entry == NULL){
    return COMPAGE_PARSER_ERROR;
  }

  if(pthread_create(&(entry->pid),NULL, (void*(*)(void*))pthread_handler,entry) != 0){
    _E("Failed to initialize pthread");
    return COMPAGE_FAILED_LAUNCH;
  }
  entry->launched = 1;

  return COMPAGE_SUCCESS;
}

void compage_join_pthreads(){
  _I("Joining pthreads");
  while(llistHead != NULL){
    if(llistHead->enabled && llistHead->launched){
      pthread_join(llistHead->pid, NULL); // TODO: use return code
    }

    llist_entry_deinit(llist_entry_remove(&llistHead));
  }
}

void compage_cancel_pthreads(){
  compage_t *it = llistHead;

  _I("Cancelling pthreads");
  while(it != NULL){
    if(it->enabled && it->launched){
      pthread_cancel(it->pid); // TODO: use return code
    }

    it = it->next;
  }
}

void compage_signal_handler(int sig){
  _I("COMPAGE signal handler called with signal: %d", sig);
  compage_cancel_pthreads(); // TODO: use return code
  compage_join_pthreads();   // TODO: use return code
  exit(0);
}

compageStatus_t compage_configure_signaling(){
  if(signal(SIGINT,  compage_signal_handler) == SIG_ERR){
    _SE("Failed to register signal handler");
    return COMPAGE_SYSTEM_ERROR;
  }

  //if(signal(SIGKILL, compage_signal_handler) == SIG_ERR){
  //  _SE("Failed to register signal handler");
  //  return COMPAGE_SYSTEM_ERROR;
  //}

  return COMPAGE_SUCCESS;
}

compageStatus_t compage_main(int argc, char *argv[]){
  compageStatus_t status;

  _D("ID size:     %lu bytes\n", get_segment_ids_size());
  _D("INIT size:   %lu bytes\n", get_segment_init_size());
  _D("LOOP size:   %lu bytes\n", get_segment_loop_size());
  _D("EXIT size:   %lu bytes\n", get_segment_exit_size());
  _D("CONFIG size: %lu bytes\n", get_segment_config_size());

  _D("Checking the supplied argument count");
  if(argc == 1){
    print_help_message(argv[0]);
    return COMPAGE_WRONG_ARGS;
  }

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
    {"default",   no_argument,       0, 'd'},
    {"list",      optional_argument, 0, 'l'},
    {"generate",  required_argument, 0, 'g'},
    {0, 0, 0, 0}
  };

  _D("Parsing command line arguments");
  int c;
  while( (c = getopt_long(argc, argv, "hdl::g:", long_options, NULL)) != -1){
    switch(c){
      case 'l':  // LIST COMPONENTS
        // in case of '-largument' syntax the optarg will be set
        if(optarg != NULL){
          return compage_print_components(optarg);
        }

        // in case of '-l argument' syntax the next argv value is the argument
        // (optind is index of the next value to be processed)
        if(optind < argc && argv[optind][0] != '-'){
          return compage_print_components(argv[optind]);
        }

        // in case of '-l' syntax there is no argument
        return compage_print_components(NULL);

      case 'g':  // GENERATE CONFIGURATION FILE
        return compage_generate_config(optarg);

      case 'd':  // LAUNCH APPLICATION USING DEFAULT CONFIG
        status = compage_init_default();
        if(status != COMPAGE_SUCCESS){
          _E("Failed to initialize components using default configuration");
          return status;
        }
        break;

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
    status = compage_init_from_file(argv[optind]);
    if(status != COMPAGE_SUCCESS){
      _E("Failed to initialize components from file: \"%s\"", argv[optind]);
      return status;
    }
  }

  _D("Launchig compage components using pthread API");
  status = compage_launch();
  if(status != COMPAGE_SUCCESS){
    _E("Failed to execute components");
    return status;
  }

  _D("Configuring signal handling");
  status = compage_configure_signaling();
  if(status != COMPAGE_SUCCESS){
    _E("Failed to configure signal handlers");
    return status;
  }

  _D("Main thread going to sleep");
  compage_join_pthreads();
  return COMPAGE_SUCCESS;
}


/* -------------------------------------------------------------------------- */
/* Component destruction */
/* -------------------------------------------------------------------------- */
compageStatus_t compage_kill_all(){
  if(kill(0, SIGINT) != 0){
    _SE("Failed to signal components");
    return COMPAGE_SYSTEM_ERROR;
  }

  return COMPAGE_SUCCESS;
}

static inline compageStatus_t compage_kill_common(compage_t *entry){
  if(entry == NULL){
    return COMPAGE_PARSER_ERROR;
  }

  if(entry->enabled && entry->launched){
    pthread_cancel(entry->pid);     // TODO: error checking
    pthread_join(entry->pid, NULL); // TODO: what if we don't join
    entry->launched = 0;
    return COMPAGE_SUCCESS;
  }

  return COMPAGE_ERROR;
}

compageStatus_t compage_kill_by_name(const char *name){
  compage_t *entry = llist_entry_find_by_name(llistHead, name);
  return compage_kill_common(entry);
}

compageStatus_t compage_kill_by_sid(const char *sid){
  compage_t *entry = llist_entry_find_by_sid(llistHead, sid);
  return compage_kill_common(entry);
}

compageStatus_t compage_kill_by_id(unsigned id){
  compage_t *entry = llist_entry_find_by_id(llistHead, id);
  return compage_kill_common(entry);
}

/* -------------------------------------------------------------------------- */
/* Component getters */
/* -------------------------------------------------------------------------- */
const char* compage_get_name(void *p){
  return CONTAINER_OF(compage_t, pdata, p)->name;
}

const char* compage_get_sid(void *p){
  return CONTAINER_OF(compage_t, pdata, p)->sid;
}

unsigned compage_get_id(void *p){
  return CONTAINER_OF(compage_t, pdata, p)->id;
}

compageState_t compage_get_state(void *p){
  return CONTAINER_OF(compage_t, pdata, p)->state;
}


compageState_t compage_get_state_by_name(const char *name){
  compage_t *entry = llist_entry_find_by_name(llistHead, name);
  if(entry == NULL){
    return COMPAGE_STATE_ILLEGAL;
  }

  return entry->state;
}

compageState_t compage_get_state_by_sid(const char *sid){
  compage_t *entry = llist_entry_find_by_sid(llistHead, sid);
  if(entry == NULL){
    return COMPAGE_STATE_ILLEGAL;
  }

  return entry->state;
}

compageState_t compage_get_state_by_id(unsigned id){
  compage_t *entry = llist_entry_find_by_id(llistHead, id);
  if(entry == NULL){
    return COMPAGE_STATE_ILLEGAL;
  }

  return entry->state;
}

const char* compage_get_state_str(compageState_t state){
  if(state > COMPAGE_STATE_ILLEGAL){
    state = COMPAGE_STATE_ILLEGAL;
  }

  return state_representations[state];
}


/* -------------------------------------------------------------------------- */
/* Other */
/* -------------------------------------------------------------------------- */
compageStatus_t _compage_callback_register(
  compageCallbackHandler_t handler,
  compageCallbackType_t type,
  void *arg)
{
  if(type >= COMPAGE_CALLBACK_COUNT){
    return COMPAGE_INVALID_TYPE;
  }

  callbacks[type].handler = handler;
  callbacks[type].arg     = arg;
  return COMPAGE_SUCCESS;
}
