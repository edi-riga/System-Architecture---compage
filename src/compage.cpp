#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

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
  _I("List command!");
  return COMPAGE_SUCCESS;
}

compageStatus_t compage_generate_config(const char *optarg){
  _I("Generate command: %s", optarg);
  return COMPAGE_SUCCESS;
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
      case 'l': // LIST COMPONENTS
        return compage_print_components();

      case 'g': // GENERATE CONFIGURATION FILE
        return compage_generate_config(optarg);

      case 'h': // HELP
        print_help_message(argv[0]);
        return COMPAGE_SUCCESS;

      default:
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
