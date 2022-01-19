#ifndef _COMPAGE_TYPES_H_
#define _COMPAGE_TYPES_H_

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

typedef enum {
  COMPAGE_SUCCESS=0,
  COMPAGE_NO_COMPONENTS,
  COMPAGE_DUPLICATE_COMPONENTS,
  COMPAGE_WRONG_ARGS,
  COMPAGE_ERROR,
  COMPAGE_SYSTEM_ERROR,
  COMPAGE_PARSER_ERROR,
} compageStatus_t;


#pragma pack(push, 1)
typedef struct {
  const char*  name; // string representation of the id
} compageId_t;


typedef struct {
  void*      id;      // address of the streang representation pointer
  void*      addr;    // address of the pdata struct
  uint64_t   size;    // size of the pdata struct
  uint64_t   padding;
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
  compagePdata_t*  pdata;  // component's private data structure
  const char*      name;   // name of the configuration variable
  uint64_t         type;   // type identifiactor for the variable
  size_t           offset; // variable's offset in the struct
} compageConfig_t;
#pragma pack(pop)


typedef struct compage_t {
  struct compage_t   *next;      // we are using forwardly linked list format
  unsigned            id;        // unique component identificator
  char               *sid;       // optional component's string id, by default is same as name
  pthread_t           pid;       // process id used for launching/releasing threads
  int                 enabled;   // determines if the component should be launched
  char               *name;      // name / title of the component
  compageId_t        *compageId;    // address of the id struct in the compage_ids segment
  compagePdata_t     *compagePdata; // address of the pdata struct in the compage_pdata segment
  compageStatus_t   (*handlerInit)(void*); // initialization handler
  compageStatus_t   (*handlerLoop)(void*); // (control) loop handler
  compageStatus_t   (*handlerExit)(void*); // component exit handler
  char                pdata[];   // private data structure to call handle with
} compage_t;


#ifdef __cplusplus
    #include <typeinfo>
    #define typeof(t) decltype(t)
    #define COMPAGE_TYPEID(x) *(uint16_t*)typeid(x).name()

#else
    #define COMPAGE_TYPEID(x) _Generic((x), \
        _Bool:        'b',    \
        int8_t:       'a',    \
        uint8_t:      'h',    \
        int16_t:      's',    \
        uint16_t:     't',    \
        int32_t:      'i',    \
        uint32_t:     'j',    \
        int64_t:      'l',    \
        uint64_t:     'm',    \
        float:        'f',    \
        double:       'd',    \
        char:         'a',    \
        char*:        0x6350, \
        const char*:  0x4b50, \
        default:  0xffffffff)
#endif



#endif
