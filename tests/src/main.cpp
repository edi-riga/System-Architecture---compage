#include <stdio.h>
#include "compage.h"

typedef struct{
  int a;
}pdata_t;

pdata_t pdata = {1};


compageStatus_t init(pdata_t *p){
  return COMPAGE_SUCCESS;
}
compageStatus_t loop(pdata_t *p){
  return COMPAGE_SUCCESS;
}
compageStatus_t exit(pdata_t *p){
  return COMPAGE_SUCCESS;
}


COMPAGE_REGISTER_ID(test);
COMPAGE_REGISTER_PDATA(test, pdata);
COMPAGE_REGISTER_INIT(test, init);
COMPAGE_REGISTER_LOOP(test, loop);
COMPAGE_REGISTER_EXIT(test, exit);


int main(void){
  printf("ID:    %p, %s \n",  &test_id, test_id.name);
  printf("PDATA: %p, %p, %lu\n", test_pdata.id, test_pdata.addr, test_pdata.size);
  printf("INIT:  %p, %p\n", test_init.id, test_init.handler);
  printf("LOOP:  %p, %p\n", test_loop.id, test_loop.handler);
  printf("EXIT:  %p, %p\n", test_exit.id, test_exit.handler);
  //printf("%x\n", STRHASH(STRINGIFY(test0)));
  //printf("%x\n", STRHASH("test1"));
  //printf("%x\n", STRHASH(STRINGIFY(test1)));
  //printf("%x\n", STRHASH("longlonglongtest0"));
  //printf("%x\n", STRHASH(STRINGIFY(longlonglongtest0)));
  //printf("%x\n", STRHASH("longlonglongtest1"));
  //printf("%x\n", STRHASH(STRINGIFY(longlonglongtest1)));
  return 0;
}
