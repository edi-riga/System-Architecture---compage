#include <stdio.h>
#include "compage.h"

typedef struct{
  const char *a;
  unsigned    b;
  int         c;
}pdata_t;

pdata_t pdata = {"asd", 1, 2};

void callback(void *arg, pdata_t *pdata){
  printf("Callback called for \"%s\" in state \"%s\"\n",
    compage_get_sid(pdata),
    compage_get_current_state_str(pdata));
}

compageStatus_t init(pdata_t *p){
  printf("Executing init for id:%u name:\"%s\" sid:\"%s\" with a=%s, b=%u, c=%d\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->a, p->b, p->c);
  return COMPAGE_SUCCESS;
}
compageStatus_t loop(pdata_t *p){
  printf("Executing loop for id:%u name:\"%s\" sid:\"%s\" with a=%s, b=%u, c=%d\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->a, p->b, p->c);
  return COMPAGE_SUCCESS;
}
compageStatus_t texit(pdata_t *p){
  printf("Executing exit for id:%u name:\"%s\" sid:\"%s\" with a=%s, b=%u, c=%d\n",
  compage_get_id(p), compage_get_name(p), compage_get_sid(p), p->a, p->b, p->c);
  return COMPAGE_SUCCESS;
}


COMPAGE_REGISTER_ID(test0);
COMPAGE_REGISTER_PDATA(test0, pdata);
COMPAGE_REGISTER_INIT(test0, init);
COMPAGE_REGISTER_LOOP(test0, loop);
COMPAGE_REGISTER_EXIT(test0, texit);
COMPAGE_ADD_CONFIG(test0, pdata, a, b, c);

COMPAGE_REGISTER_ID(test1);
COMPAGE_REGISTER_PDATA(test1, pdata);
COMPAGE_REGISTER_INIT(test1, init);
COMPAGE_REGISTER_LOOP(test1, loop);
COMPAGE_REGISTER_EXIT(test1, texit);
COMPAGE_ADD_CONFIG(test1, pdata, a);
COMPAGE_ADD_CONFIG(test1, pdata, b);
COMPAGE_ADD_CONFIG(test1, pdata, c);


int main(int argc, char *argv[]){
  printf("ID:        %p, %s \n",  &test0_id, test0_id.name);
  printf("PDATA:     %p, %p, %lu\n", test0_pdata.id, test0_pdata.addr, test0_pdata.size);
  printf("INIT:      %p, %p\n", test0_init.id, test0_init.handler);
  printf("LOOP:      %p, %p\n", test0_loop.id, test0_loop.handler);
  printf("EXIT:      %p, %p\n", test0_exit.id, test0_exit.handler);
  printf("CONFIG_A:  %p, %s %lu %lu\n", test0_config_a.pdata, test0_config_a.name, test0_config_a.type, test0_config_a.offset);
  printf("CONFIG_B:  %p, %s %lu %lu\n", test0_config_b.pdata, test0_config_b.name, test0_config_b.type, test0_config_b.offset);
  printf("CONFIG_C:  %p, %s %lu %lu\n", test0_config_c.pdata, test0_config_c.name, test0_config_c.type, test0_config_c.offset);

  printf("ID:        %p, %s \n",  &test1_id, test1_id.name);
  printf("PDATA:     %p, %p, %lu\n", test1_pdata.id, test1_pdata.addr, test1_pdata.size);
  printf("INIT:      %p, %p\n", test1_init.id, test1_init.handler);
  printf("LOOP:      %p, %p\n", test1_loop.id, test1_loop.handler);
  printf("EXIT:      %p, %p\n", test1_exit.id, test1_exit.handler);
  printf("CONFIG_A:  %p, %s %lu %lu\n", test1_config_a.pdata, test1_config_a.name, test1_config_a.type, test1_config_a.offset);
  printf("CONFIG_B:  %p, %s %lu %lu\n", test1_config_b.pdata, test1_config_b.name, test1_config_b.type, test1_config_b.offset);
  printf("CONFIG_C:  %p, %s %lu %lu\n", test1_config_c.pdata, test1_config_c.name, test1_config_c.type, test1_config_c.offset);

  compage_callback_register(callback, COMPAGE_CALLBACK_PREINIT,  NULL);
  compage_callback_register(callback, COMPAGE_CALLBACK_POSTINIT, NULL);
  compage_callback_register(callback, COMPAGE_CALLBACK_PRELOOP,  NULL);
  compage_callback_register(callback, COMPAGE_CALLBACK_POSTLOOP, NULL);
  compage_callback_register(callback, COMPAGE_CALLBACK_PREEXIT,  NULL);
  compage_callback_register(callback, COMPAGE_CALLBACK_POSTEXIT, NULL);

  return compage_main(argc, argv);
}
