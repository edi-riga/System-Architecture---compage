#ifndef _COMPAGE_REGISTER_H_
#define _COMPAGE_REGISTER_H_

#include <stdint.h>
#include "compage_hash.h"
#include "compage_types.h"
#include "compage_macro.h"




#define _COMPAGE_REGISTER_ID(id) \
compageId_t id##_id __attribute__((section("compage_ids"))) = \
{\
  STRINGIFY(id)\
};

#define _COMPAGE_REGISTER_PDATA(id, pdata) \
compagePdata_t id##_pdata __attribute__((section("compage_pdata"))) = \
{\
  &id##_id,\
  &pdata,\
  sizeof(pdata)\
};

#define _COMPAGE_REGISTER_INIT(id, handler)\
compageInit_t id##_init __attribute__((section("compage_init"))) = \
{\
  &id##_id,\
  (compageStatus_t(*)(void*))handler\
};

#define _COMPAGE_REGISTER_LOOP(id, handler)\
compageLoop_t id##_loop __attribute__((section("compage_loop"))) = \
{\
  &id##_id,\
  (compageStatus_t(*)(void*))handler\
};


#define _COMPAGE_REGISTER_EXIT(id, handler)\
compageLoop_t id##_exit __attribute__((section("compage_exit"))) = \
{\
  &id##_id,\
  (compageStatus_t(*)(void*))handler\
};



#define _COMPAGE_REGISTER_CONFIG(id, type, ...)


#endif
