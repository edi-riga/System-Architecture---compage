#include <stdio.h>
#include <stdint.h>
#include "compage.h"

/* worker's private data structure */
typedef struct {
    int8_t   val_int8;
    uint8_t  val_uint8;
    int16_t  val_int16;
    uint16_t val_uint16;
    int32_t  val_int32;
    uint32_t val_uint32;
    int64_t  val_int64;
    uint64_t val_uint64;
    float    val_float;
    double   val_double;
    char*    val_char;
} pdata_t;

/* default configuration */
static pdata_t worker_pdataDefualt = {
    .val_int8   = 1,
    .val_uint8  = 2,
    .val_int16  = 3,
    .val_uint16 = 4,
    .val_int32  = 5,
    .val_uint32 = 6,
    .val_int64  = 7,
    .val_uint64 = 8,
    .val_float  = 9,
    .val_double = 10,
    .val_char  = "test",
};

/* worker */
static void *worker(void *p){
    pdata_t *pdata = (pdata_t*)p;

    printf("Worker thread - starting\n");
    printf("val_int8:   %d\n",   pdata->val_int8);
    printf("val_uint8:  %u\n",   pdata->val_uint8);
    printf("val_int16:  %d\n",   pdata->val_int16);
    printf("val_uint16: %u\n",   pdata->val_uint16);
    printf("val_int32:  %d\n",   pdata->val_int32);
    printf("val_uint32: %u\n",   pdata->val_uint32);
    printf("val_int64:  %lld\n", pdata->val_int64);
    printf("val_uint64: %llu\n", pdata->val_uint64);
    printf("val_float:  %f\n",   pdata->val_float);
    printf("val_double: %f\n",   pdata->val_double);
    printf("val_char:   %s\n",   pdata->val_char);
    //printf("Worker data - z: %f\n", pdata->z);
    printf("Worker thread - stopping\n");
    return NULL;
}

/* register this worker and worker's configuration */
COMPAGE_REGISTER(worker, worker_pdataDefualt);
//COMPAGE_PDATA_ADD_CONFIG_MULTIPLE(worker, pdata_t, val_int8);
COMPAGE_PDATA_ADD_CONFIGS(worker, pdata_t, 
    val_int8, val_uint8, val_int16, val_uint16,
    val_int32, val_uint32, val_int64, val_uint64,
    val_float, val_double, val_char)
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_float);
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_double);
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_char);
