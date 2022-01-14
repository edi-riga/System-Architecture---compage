#ifndef _COMPAGE_REGISTER_H_
#define _COMPAGE_REGISTER_H_

#include <stdint.h>
#include "compage_hash.h"
#include "compage_types.h"
#include "compage_macro.h"



#define _COMPAGE_REGISTER_ID(id)                                               \
compageId_t id##_id __attribute__((section("compage_ids"))) =                  \
{                                                                              \
  STRINGIFY(id)                                                                \
};


#define _COMPAGE_REGISTER_PDATA(id, pdata)                                     \
compagePdata_t id##_pdata __attribute__((section("compage_pdata"))) =          \
{                                                                              \
  &id##_id,                                                                    \
  &pdata,                                                                      \
  sizeof(pdata)                                                                \
};


#define _COMPAGE_REGISTER_INIT(id, handler)                                    \
compageInit_t id##_init __attribute__((section("compage_init"))) =             \
{                                                                              \
  &id##_id,                                                                    \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_REGISTER_LOOP(id, handler)                                    \
compageLoop_t id##_loop __attribute__((section("compage_loop"))) =             \
{                                                                              \
  &id##_id,                                                                    \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_REGISTER_EXIT(id, handler)                                    \
compageLoop_t id##_exit __attribute__((section("compage_exit"))) =             \
{                                                                              \
  &id##_id,                                                                    \
  (compageStatus_t(*)(void*))handler                                           \
};


#define _COMPAGE_ADD_CONFIG(id, type, ...)\
COMPAGE_PDATA_ADD_CONFIGS_(ARGUMENT_COUNT(__VA_ARGS__), id, type, __VA_ARGS__)

#define COMPAGE_PDATA_ADD_CONFIGS_(N, id, type, ...) \
CONCATENATE(COMPAGE_PDATA_ADD_CONFIGS_,N)(id, type, __VA_ARGS__)


#define COMPAGE_PDATA_ADD_CONFIGS_1(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);
#define COMPAGE_PDATA_ADD_CONFIGS_2(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_1(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_3(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_2(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_4(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_3(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_5(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_4(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_6(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_5(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_7(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_6(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_8(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_7(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_9(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_8(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_10(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_9(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_11(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_10(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_12(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_11(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_13(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_12(id, type, __VA_ARGS__)
#define COMPAGE_PDATA_ADD_CONFIGS_14(id, type, config, ...)\
  COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config);\
  COMPAGE_PDATA_ADD_CONFIGS_13(id, type, __VA_ARGS__)

#define COMPAGE_PDATA_ADD_CONFIG_SINGLE(id, type, config)                      \
compageConfig_t id##_config_##config __attribute__((section("compage_config"))) =\
{                                                                              \
  &id##_id,                                                                    \
  STRINGIFY(config),                                                           \
  COMPAGE_TYPEID(((type*)0)->config),                                          \
  (size_t)&((type*)0)->config                                                  \
};


#endif
