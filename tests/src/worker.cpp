extern "C" {
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "compage.h"
}

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
    char*    val_char_null;
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
    .val_char  = (char*)"test",
    .val_char_null = NULL,
};

/* worker */
static void *worker(pdata_t *pdata){
    const char *componentID = COMPAGE_GET_COMPONENT_ID(pdata);

    printf("%s - starting\n", componentID);
    printf("%s - val_int8:      %d\n",  componentID, pdata->val_int8);
    printf("%s - val_uint8:     %u\n",  componentID, pdata->val_uint8);
    printf("%s - val_int16:     %d\n",  componentID, pdata->val_int16);
    printf("%s - val_uint16:    %u\n",  componentID, pdata->val_uint16);
    printf("%s - val_int32:     %d\n",  componentID, pdata->val_int32);
    printf("%s - val_uint32:    %u\n",  componentID, pdata->val_uint32);
    printf("%s - val_int64:     %ld\n", componentID, pdata->val_int64);
    printf("%s - val_uint64:    %lu\n", componentID, pdata->val_uint64);
    printf("%s - val_float:     %f\n",  componentID, pdata->val_float);
    printf("%s - val_double:    %f\n",  componentID, pdata->val_double);
    printf("%s - val_char:      %s\n",  componentID, pdata->val_char);
    printf("%s - val_char_null: %s\n",  componentID, (pdata->val_char_null==NULL) ? "NULL":"NOT NULL");
    printf("%s - stopping\n", componentID);
    return NULL;
}

/* register this worker and worker's configuration */
COMPAGE_REGISTER(worker, worker_pdataDefualt);
//COMPAGE_PDATA_ADD_CONFIG_MULTIPLE(worker, pdata_t, val_int8);
COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, 
    val_int8, val_uint8, val_int16, val_uint16,
    val_int32, val_uint32, val_int64, val_uint64,
    val_float, val_double, val_char, val_char_null)
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_float);
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_double);
//COMPAGE_PDATA_ADD_CONFIG(worker, pdata_t, val_char);
