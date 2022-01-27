#include <stdio.h>
#include <unistd.h>
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


int main(int argc, char *argv[]){
  /* optional callback registration, the callback could be used for implementing
   * fail-safe mechanisms and monitoring the characteristics of the handler
   * execution. Also the callback registration can be seperated for each 
   * pre/post phase */
  compage_callback_register_all(callback, NULL);

  /* the basic futher use is just to pass the command line arguments to the
   * framework, it enables listing components, generating and applying
   * configurations */
  return compage_main(argc, argv);
}
