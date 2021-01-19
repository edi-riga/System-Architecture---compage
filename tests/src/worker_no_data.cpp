extern "C" {
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "compage.h"
}

static void *worker_no_data(void *pdata){
    const char *componentID = COMPAGE_GET_COMPONENT_ID(pdata);
    printf("%s - starting\n", componentID);
    printf("%s - stopping\n", componentID);
    return NULL;
}

COMPAGE_REGISTER(worker_no_data);
