#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "compage.h"

/* global configuration */
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


/* register configurations with the compage framework */
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


/* (alternative way) initialize and register configuration */
COMPAGE_INIT_ADD_GLOBAL_CONFIG(int32_t, g_example_12, 0);
COMPAGE_INIT_ADD_GLOBAL_CONFIG(const char*, g_example_13, "Already initialized");


int main(int argc, char *argv[]){
  /* The goal of this example is to illustrate how compage can be used to
   * configure global scope variables. The following steps will help in
   * verifying the intended functionality:
   * 1) List the available configuration:
   *   ./example -l
   *
   * 2) Generate default configuration file:
   *   ./example -g config.ini
   *
   * 3) Update/change values in the configuration file:
   *
   * 4) List configuration using the updated configuration file:
   *   ./example -l config.ini
   */


  /* A basic wrapper simply passing the command line arguments to the
   * framework's default command line (fine for the most cases). Run "help"
   * to list available commands. */
  return compage_main(argc, argv);
}
