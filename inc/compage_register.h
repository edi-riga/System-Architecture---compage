#ifndef _COMPAGE_REGISTER_H_
#define _COMPAGE_REGISTER_H_

#include <stdint.h>
#include "compage_hash.h"
#include "compage_types.h"
#include "compage_macro.h"


#define _COMPAGE_REGISTER_ID(id)                                               \
compageId_t __##id##_id __attribute__((used,section("compage_ids"))) =         \
{                                                                              \
  STRINGIFY(id)                                                                \
};


#define _COMPAGE_REGISTER_PDATA(id, pdata)                                     \
compagePdata_t __##id##_pdata __attribute__((used,section("compage_pdata"))) = \
{                                                                              \
  &__##id##_id,                                                                \
  &pdata,                                                                      \
  sizeof(pdata)                                                                \
};


#define _COMPAGE_REGISTER_INIT(id, handler)                                    \
compageInit_t __##id##_init __attribute__((used,section("compage_init"))) =    \
{                                                                              \
  &__##id##_id,                                                                \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_REGISTER_LOOP(id, handler)                                    \
compageLoop_t __##id##_loop __attribute__((used,section("compage_loop"))) =    \
{                                                                              \
  &__##id##_id,                                                                \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_REGISTER_EXIT(id, handler)                                    \
compageExit_t __##id##_exit __attribute__((used,section("compage_exit"))) =    \
{                                                                              \
  &__##id##_id,                                                                \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_ADD_CONFIG(id, pdata, ...)\
COMPAGE_PDATA_ADD_CONFIGS_(COMPAGE_ARGUMENT_COUNT(__VA_ARGS__), id, pdata, __VA_ARGS__)

#define COMPAGE_PDATA_ADD_CONFIGS_(N, id, pdata, ...) \
COMPAGE_CONCATENATE(COMPAGE_PDATA_ADD_CONFIGS_,N)(id, pdata, __VA_ARGS__)


#define COMPAGE_PDATA_ADD_CONFIGS_1(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);
#define COMPAGE_PDATA_ADD_CONFIGS_2(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_1(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_3(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_2(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_4(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_3(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_5(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_4(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_6(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_5(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_7(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_6(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_8(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_7(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_9(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_8(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_10(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_9(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_11(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_10(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_12(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_11(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_13(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_12(id, pdata, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_14(id, pdata, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config);\
  COMPAGE_PDATA_ADD_CONFIGS_13(id, pdata, __VA_ARGS__)

#define COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, pdata, config)                     \
compageConfig_t id##_config_##config __attribute__((section("compage_config"))) =\
{                                                                              \
  &__##id##_pdata,                                                                 \
  STRINGIFY(config),                                                           \
  COMPAGE_TYPEID(((typeof(pdata)*)0)->config),                                 \
  (size_t)&((typeof(pdata)*)0)->config                                         \
};

compageStatus_t _compage_callback_register(
  compageCallbackHandler_t handler,
  compageCallbackType_t type,
  void *arg);

#endif
