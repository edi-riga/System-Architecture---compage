#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "compage.h"

/* component's data structure and its default values */
typedef struct{
  const char *string;
  int         counter;
}pdata_t;

pdata_t pdata = {"String option", 0};


/* component's handlers: init (constructor), loop and exit (destructor) */
compageStatus_t init(pdata_t *p){
  printf("Executing init for id:%u name:\"%s\" sid:\"%s\" with string=%s, counter=%u\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->string, p->counter);
  return COMPAGE_SUCCESS;
}

compageStatus_t loop(pdata_t *p){
  printf("Executing loop for id:%u name:\"%s\" sid:\"%s\" with string=%s, counter=%u\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->string, p->counter);

  if(p->counter++ < 5){
    sleep(1);
    return COMPAGE_SUCCESS;
  }

  return COMPAGE_EXIT_LOOP;
}

compageStatus_t texit(pdata_t *p){
  printf("Executing exit for id:%u name:\"%s\" sid:\"%s\" with string=%s, counter=%u\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->string, p->counter);
  return COMPAGE_SUCCESS;
}


/* callback for pre/post callbacks */
void callback(void *arg, pdata_t *pdata){
  printf("Callback called for \"%s\" in state \"%s\"\n",
    compage_get_sid(pdata),
    compage_get_state_str(compage_get_state(pdata)));
}


/* "test0" component registration example (may be in a separate file) */
COMPAGE_REGISTER_ID(test0);
COMPAGE_REGISTER_PDATA(test0, pdata);
COMPAGE_REGISTER_INIT(test0, init);
COMPAGE_REGISTER_LOOP(test0, loop);
COMPAGE_REGISTER_EXIT(test0, texit);
COMPAGE_ADD_CONFIG(test0, pdata, string, counter);

/* "test1" component registration example (may be in a separate file) */
COMPAGE_REGISTER_ID(test1);
COMPAGE_REGISTER_PDATA(test1, pdata);
COMPAGE_REGISTER_INIT(test1, init);
COMPAGE_REGISTER_LOOP(test1, loop);
COMPAGE_REGISTER_EXIT(test1, texit);
COMPAGE_ADD_CONFIG(test1, pdata, string);
COMPAGE_ADD_CONFIG(test1, pdata, counter);


void print_usage(const char *fname){
  printf("USAGE:\n");
  printf("  %s generate <fname>:\n", fname);
  printf("  %s <fname>:\n", fname);
}

int main(int argc, char *argv[]){

  if(argc < 2){
    print_usage(argv[0]);
    return 0;
  }

  /* generate configuration file */
  if(strcmp(argv[1], "generate") == 0){
    if(argc < 3){
      print_usage(argv[0]);
      return 0;
    }

    if(compage_generate_config(argv[2]) != COMPAGE_SUCCESS){
      printf("Failed to create configuration file: \"%s\"\n", argv[2]);
      return 1;
    }

    return 0;
  }

  /* use supplied configuration file */
  if(compage_init_from_file(argv[1]) != COMPAGE_SUCCESS){
    printf("Faild to parse configuration file\n");
    return 1;
  }

  if(compage_launch() != COMPAGE_SUCCESS){
    printf("Faild to launch compage components\n");
    return 1;
  }

  if(compage_configure_signaling() != COMPAGE_SUCCESS){
    printf("Failed to configure signal handlers\n");
    return 1;
  }

  printf("Main thread going to sleep\n");
  compage_join_pthreads();
  return 0;
}
