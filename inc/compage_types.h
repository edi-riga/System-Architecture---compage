#ifndef _COMPAGE_TYPES_H_
#define _COMPAGE_TYPES_H_

#include <stdint.h>
#include <stdlib.h>

typedef enum {
  COMPAGE_SUCCESS=0,
  COMPAGE_NO_COMPONENTS,
  COMPAGE_DUPLICATE_COMPONENTS,
  COMPAGE_WRONG_ARGS,
  COMPAGE_ERROR,
} compageStatus_t;


#pragma pack(push, 1)
typedef struct {
  const char*  name; // string representation of the id
} compageId_t;


typedef struct {
  void*      id;     // address of the streang representation pointer
  void*      addr;   // address of the pdata struct
  uint64_t   size;   // size of the pdata struct
} compagePdata_t;


typedef struct {
  void*              id; // address of the streang representation pointer
  compageStatus_t    (*handler)(void*); // init handler
} compageInit_t;


typedef struct {
  void*              id; // address of the streang representation pointer
  compageStatus_t    (*handler)(void*); // loop handler
} compageLoop_t;


typedef struct {
  void*              id; // address of the streang representation pointer
  compageStatus_t    (*handler)(void*); // exit handler
} compageExit_t;


typedef struct {
  void*        id;     // component's id
  const char*  name;   // name of the configuration variable
  uint32_t     type;   // type identifiactor for the variable
  size_t       offset; // variable's offset in the struct
} compageConfig_t;
#pragma pack(pop)



#ifdef __cplusplus
    #include <typeinfo>
    #define typeof(t) decltype(t)
    #define COMPAGE_TYPEID(x) *(uint32_t*)typeid(x).name()

#else
    #define COMPAGE_TYPEID(x) _Generic((x), \
        _Bool:    'b',   \
        int8_t:   'a',   \
        uint8_t:  'h',   \
        int16_t:  's',   \
        uint16_t: 't',   \
        int32_t:  'i',   \
        uint32_t: 'j',   \
        int64_t:  'l',   \
        uint64_t: 'm',   \
        float:    'f',   \
        double:   'd',   \
        char:     'a',   \
        char*:    0x6350,\
        default:  0xffffffff)
#endif



#endif
