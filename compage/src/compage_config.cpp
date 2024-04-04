#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "notification.h"


/* convert representation from type to string */
char *compage_cfg_get_string(char *buf, size_t bufSize, size_t type, void *addr){
  _D("Type: 0x%x; Hex value: 0x%x", (unsigned)type, *(unsigned*)(addr));

  switch(type){
    case 'b': /* bool     */
      snprintf(buf, bufSize, "%s", (*(int8_t*)addr) ? "true" : "false");
      return buf;
    case 'a': /* int8_t   */
      snprintf(buf, bufSize, "%d", *(int8_t*)addr);
      return buf;
    case 'h': /* uint8_t  */
      snprintf(buf, bufSize, "%u", *(uint8_t*)addr);
      return buf;
    case 's': /* int16_t  */
      snprintf(buf, bufSize, "%d", *(int16_t*)addr);
      return buf;
    case 't': /* uint16_t */
      snprintf(buf, bufSize, "%u", *(uint16_t*)addr);
      return buf;
    case 'i': /* int32_t  */
      snprintf(buf, bufSize, "%d", *(int32_t*)addr);
      return buf;
    case 'j': /* uint32_t */
      snprintf(buf, bufSize, "%u", *(uint32_t*)addr);
      return buf;
    case 'l': /* int64_t  */
      snprintf(buf, bufSize, "%ld", *(int64_t*)addr);
      return buf;
    case 'm': /* uint64_t */
      snprintf(buf, bufSize, "%lu", *(uint64_t*)addr);
      return buf;
    case 'f': /* float    */
      snprintf(buf, bufSize, "%f", *(float*)addr);
      return buf;
    case 'd': /* double   */
      snprintf(buf, bufSize, "%f", *(double*)addr);
      return buf;
    case 0x6350: /* char* */
    case 0x4b50: /* const char* */
      if( *(char**)addr == NULL ){   /* check if string is NULL */
        buf[0]='\0';
      } else{
        snprintf(buf, bufSize, "%s", *(char**)addr);
      }
      return buf;
    default:
      snprintf(buf, bufSize, "<Data type not supported>");
      return buf;
  }
}

/* convert representation from string to type */
int compage_cfg_set_value(void *dst, const void *src, size_t type){
  _D("Type: 0x%x", (unsigned)type);

  switch(type){
    case 'b': /* bool     */
      if ((strcmp((char*)src, "true") == 0) || (strcmp((char*)src, "1") == 0)) {
        *(int8_t*)dst = 1;
      } else if ((strcmp((char*)src, "false") == 0) || (strcmp((char*)src, "0") == 0)) {
        *(int8_t*)dst = 0;
      } else {
        return -1;
      }
      return 0;
    case 'a': /* int8_t   */
      *(int8_t*)dst = atoi((char*)src);
      return 0;
    case 'h': /* uint8_t  */
      *(uint8_t*)dst = atoi((char*)src);
      return 0;
    case 's': /* int16_t  */
      *(int16_t*)dst = atoi((char*)src);
      return 0;
    case 't': /* uint16_t */
      *(uint16_t*)dst = atoi((char*)src);
      return 0;
    case 'i': /* int32_t  */
      *(int32_t*)dst = atoi((char*)src);
      return 0;
    case 'j': /* uint32_t */
      *(uint32_t*)dst = atoi((char*)src);
      return 0;
    case 'l': /* int64_t  */
      *(int64_t*)dst = atoll((char*)src);
      return 0;
    case 'm': /* uint64_t */
      *(uint64_t*)dst = atoll((char*)src);
      return 0;
    case 'f': /* float    */
      *(float*)dst = atof((char*)src);
      return 0;
    case 'd': /* double   */
      *(double*)dst = atof((char*)src);
      return 0;
    case 0x6350: /* char* */
    case 0x4b50: /* const char* */
      if( ((char*)src)[0] == '\0' ){ /* check if string is supposed to be NULL */
          *(char**)dst = NULL;
      } else {
          *(char**)dst = strdup((char*)src);
      }
      return 0;
    default:
      return -1;
  }
}


/* copy value depending on the type */
void compage_cfg_get_value(void *dst, const void *src, size_t type){
  _D("Type: 0x%x; Hex value: 0x%x", (unsigned)type, *(unsigned*)(addr));

  switch(type){
    case 'b': /* bool     */
      *(int8_t*)dst = (*(int8_t*)src) ? true : false;
      return;
    case 'a': /* int8_t   */
      *(int8_t*)dst = *(int8_t*)src;
      return;
    case 'h': /* uint8_t  */
      *(uint8_t*)dst = *(uint8_t*)src;
      return;
    case 's': /* int16_t  */
      *(int16_t*)dst = *(int16_t*)src;
      return;
    case 't': /* uint16_t */
      *(uint16_t*)dst = *(uint16_t*)src;
      return;
    case 'i': /* int32_t  */
      *(int32_t*)dst = *(int32_t*)src;
      return;
    case 'j': /* uint32_t */
      *(uint32_t*)dst = *(uint32_t*)src;
      return;
    case 'l': /* int64_t  */
      *(int64_t*)dst = *(int64_t*)src;
      return;
    case 'm': /* uint64_t */
      *(uint64_t*)dst = *(uint64_t*)src;
      return;
    case 'f': /* float    */
      *(float*)dst = *(float*)src;
      return;
    case 'd': /* double   */
      *(double*)dst = *(double*)src;
      return;
    case 0x6350: /* char* */
    case 0x4b50: /* const char* */
      *(char**)dst = *(char**)src;
      return;
    default:
      return;
  }
}

