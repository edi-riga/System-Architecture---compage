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
  printf("%s: Executing init for component, waiting\n", compage_get_name(p));
  sleep(3); // Simulating cumbersome initialization
  return COMPAGE_SUCCESS;
}

compageStatus_t loop(pdata_t *p){
  printf("%s: Executing loop (%d)\n", compage_get_name(p), p->counter++);
  sleep(1); // minor delay to simulate control loop
  return COMPAGE_SUCCESS;
}

compageStatus_t texit(pdata_t *p){
  printf("%s: Executing exit\n", compage_get_name(p));
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
COMPAGE_ADD_CONFIG(test1, pdata, string, counter);

/* "test2" component registration example (may be in a separate file) */
COMPAGE_REGISTER_ID(test2);
COMPAGE_REGISTER_PDATA(test2, pdata);
COMPAGE_REGISTER_INIT(test2, init);
COMPAGE_REGISTER_LOOP(test2, loop);
COMPAGE_REGISTER_EXIT(test2, texit);
COMPAGE_ADD_CONFIG(test2, pdata, string, counter);


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

  if(compage_configure_signaling() != COMPAGE_SUCCESS){
    printf("Failed to configure signal handlers\n");
    return 1;
  }

  /* use supplied configuration file */
  if(compage_init_from_file(argv[1]) != COMPAGE_SUCCESS){
    printf("Faild to parse configuration file\n");
    return 1;
  }

  /* launch components */
  printf("Launching \"test0\" component!\n");
  if(compage_launch_by_name("test0") != COMPAGE_SUCCESS){
    printf("Failed to launch test0 component\n");
    return 1;
  }
  while(compage_get_state_by_name("test0") < COMPAGE_STATE_POSTINIT){
    sleep(1);
  }

  printf("Launching \"test1\" component!\n");
  if(compage_launch_by_name("test1") != COMPAGE_SUCCESS){
    printf("Failed to launch test1 component\n");
    return 1;
  }
  while(compage_get_state_by_name("test1") < COMPAGE_STATE_POSTINIT){
    sleep(1);
  }

  printf("Launching \"test2\" component!\n");
  if(compage_launch_by_name("test2") != COMPAGE_SUCCESS){
    printf("Failed to launch test2 component\n");
    return 1;
  }
  while(compage_get_state_by_name("test2") < COMPAGE_STATE_POSTINIT){
    sleep(1);
  }

  /* give some time for test2 component to execute */
  sleep(3);

  printf("Disabling all the components\n");
  compage_kill_all();
  compage_join_pthreads();
  return 0;
}
