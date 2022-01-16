#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

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

static compagePdata_t* locate_pdata_segment(void *id){
  compagePdata_t *pdata_start, *pdata_stop;

  pdata_start  = (compagePdata_t*)get_segment_pdata_start();
  pdata_stop   = (compagePdata_t*)get_segment_pdata_stop();

  while(pdata_start < pdata_stop){
    if(pdata_start->id == id){
      return pdata_start;
    }

    pdata_start++;
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


compageStatus_t compage_init_from_file(const char *fname){
  _I("Initializing framework from file: %s", fname);
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
    return compage_init_from_file(argv[optind]);
  }

  print_help_message(argv[0]);
  return COMPAGE_SUCCESS;
}
