#include <stdio.h>
#include "compage.h"

/* worker's private data structure */
typedef struct {
    int x;
    int y;
    float z;
} pdata_t;

/* default configuration */
static pdata_t worker_pdataDefualt = {1, 777, 0.0};

/* worker */
static void *worker(void *p){
    pdata_t *pdata = (pdata_t*)p;

    printf("Worker thread - starting\n");
    printf("Worker data - x: %d\n", pdata->x);
    printf("Worker data - y: %d\n", pdata->y);
    printf("Worker data - z: %f\n", pdata->z);
    printf("Worker thread - stopping\n");
    return NULL;
}

/* register this worker and worker's configuration */
COMPAGE_REGISTER(worker, worker_pdataDefualt)
COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, x)
COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, y)
COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, z)
