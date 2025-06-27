#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "compage.h"

int8_t      g_example_0;
int16_t     g_example_1;
int32_t     g_example_2;
int64_t     g_example_3;
uint8_t     g_example_4;
uint16_t    g_example_5;
uint32_t    g_example_6;
uint64_t    g_example_7;
float       g_example_8;
double      g_example_9;
char*       g_example_10 = (char*)"default";
const char* g_example_11 = "default";


/* add global configurations */
COMPAGE_ADD_GLOBAL_CONFIG(g_example_0);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_1);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_2);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_3);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_4);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_5);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_6);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_7);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_8);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_9);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_10);
COMPAGE_ADD_GLOBAL_CONFIG(g_example_11);


int main(int argc, char *argv[]){

  printf("Default values:");
  printf("- g_example_0: %d\n", g_example_0);
  printf("- g_example_1: %d\n", g_example_1);
  printf("- g_example_2: %d\n", g_example_2);
  printf("- g_example_3: %d\n", g_example_3);
  printf("- g_example_4: %u\n", g_example_4);
  printf("- g_example_5: %u\n", g_example_5);
  printf("- g_example_6: %u\n", g_example_6);
  printf("- g_example_7: %u\n", g_example_7);
  printf("- g_example_8: %f\n", g_example_8);
  printf("- g_example_9: %f\n", g_example_9);
  printf("- g_example_10: %s\n", g_example_10);
  printf("- g_example_11: %s\n", g_example_11);

  /* the basic futher use is just to pass the command line arguments to the
   * framework, it enables listing components, generating and applying
   * configurations */
  printf("Launching compage framework:");
  int ret = compage_main(argc, argv);
  if(ret != 0){
    return ret;
  }

  printf("Checking values after launching framework:");
  printf("- g_example_0: %d\n", g_example_0);
  printf("- g_example_1: %d\n", g_example_1);
  printf("- g_example_2: %d\n", g_example_2);
  printf("- g_example_3: %d\n", g_example_3);
  printf("- g_example_4: %u\n", g_example_4);
  printf("- g_example_5: %u\n", g_example_5);
  printf("- g_example_6: %u\n", g_example_6);
  printf("- g_example_7: %u\n", g_example_7);
  printf("- g_example_8: %f\n", g_example_8);
  printf("- g_example_9: %f\n", g_example_9);
  printf("- g_example_10: %s\n", g_example_10);
  printf("- g_example_11: %s\n", g_example_11);
  return 0;
}
