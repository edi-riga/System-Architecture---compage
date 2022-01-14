#include <stdio.h>
#include "compage.h"

typedef struct{
  const char *a;
  unsigned    b;
  int         c;
}pdata_t;

pdata_t pdata = {"asd", 1, 2};


compageStatus_t init(pdata_t *p){
  return COMPAGE_SUCCESS;
}
compageStatus_t loop(pdata_t *p){
  return COMPAGE_SUCCESS;
}
compageStatus_t exit(pdata_t *p){
  return COMPAGE_SUCCESS;
}


COMPAGE_REGISTER_ID(test0);
COMPAGE_REGISTER_PDATA(test0, pdata);
COMPAGE_REGISTER_INIT(test0, init);
COMPAGE_REGISTER_LOOP(test0, loop);
COMPAGE_REGISTER_EXIT(test0, exit);
COMPAGE_ADD_CONFIG(test0, pdata_t, a, b, c);


int main(void){
  printf("ID:        %p, %s \n",  &test0_id, test0_id.name);
  printf("PDATA:     %p, %p, %lu\n", test0_pdata.id, test0_pdata.addr, test0_pdata.size);
  printf("INIT:      %p, %p\n", test0_init.id, test0_init.handler);
  printf("LOOP:      %p, %p\n", test0_loop.id, test0_loop.handler);
  printf("EXIT:      %p, %p\n", test0_exit.id, test0_exit.handler);
  printf("CONFIG_A:  %p, %s %u %lu\n", test0_config_a.id, test0_config_a.name, test0_config_a.type, test0_config_a.offset);
  printf("CONFIG_B:  %p, %s %u %lu\n", test0_config_b.id, test0_config_b.name, test0_config_b.type, test0_config_b.offset);
  printf("CONFIG_C:  %p, %s %u %lu\n", test0_config_c.id, test0_config_c.name, test0_config_c.type, test0_config_c.offset);
  return 0;
}
