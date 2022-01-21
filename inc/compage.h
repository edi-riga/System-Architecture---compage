#ifndef _COMPAGE_H_
#define _COMPAGE_H_

#include "compage_register.h"



/** @def COMPAGE_REGISTER_ID(id)
 *
 * @brief Register the given string identifier with the COMPAGE framework. The
 *        macro will store address to the given string in a "compage_ids" table
 *        / segment, but the address of this location (double pointer) is
 *        further used as unique ID. Multiple components registered with the
 *        same ID should trigger a runtime error.
 *
 * @param id A string identifier of the component.
 **/
#define COMPAGE_REGISTER_ID(id)  _COMPAGE_REGISTER_ID(id)


/** @def COMPAGE_REGISTER_PDATA(id, pdata)
 *
 * @brief Associate private data structure for the given string identifier with
 *        the COMPAGE framework. The macro will store component's id, size of the
 *        private data structure and the its actual address in a "copmage_pdata"
 *        table / segment. Multiple registrations with the same string ID should
 *        trigger a runtime error.
 *
 * @param id A string identifier of the component.
 * @param pdata Default private data structure of the component (passed by value).
 **/
#define COMPAGE_REGISTER_PDATA(id, pdata) _COMPAGE_REGISTER_PDATA(id, pdata)


/** @def COMPAGE_REGISTER_INIT(id, handler)
 *
 * @brief Associate initializtion routine (handler) with the component associated
 *        to the given ID. The macro will store component's id, and actual
 *        address of the handler in a "compage_init" table / segment. Multiple
 *        registrations with the same string ID should trigger a runtime error.
 *
 * @param id A string identifier of the component.
 * @param handler Initalization routine with the following protoype
 *        "compageStatus_t handler(pdata_t *pdata);", where the pdata_t is the
 *        custom data type of the component.
 **/
#define COMPAGE_REGISTER_INIT(id, handler) _COMPAGE_REGISTER_INIT(id, handler)


/** @def COMPAGE_REGISTER_LOOP(id, handler)
 *
 * @brief Associate control loop routine (handler) with the component associated
 *        to the given ID. The macro will store component's id (addr), and actual
 *        address of the handler in a "compage_loop" table / segment. Multiple
 *        registrations with the same string ID should trigger a runtime error.
 *
 * @param id A string identifier of the component.
 * @param handler Control loop's routine with the following protoype
 *        "compageStatus_t handler(pdata_t *pdata);", where the pdata_t is the
 *        custom data type of the component.
 **/
#define COMPAGE_REGISTER_LOOP(id, handler) _COMPAGE_REGISTER_LOOP(id, handler)


/** @def COMPAGE_REGISTER_EXIT(id, handler)
 *
 * @brief Associate exit routine (handler) with the component associated to the
 *        given ID. The macro will store component's id (address) and actual
 *        address of the handler in a "compage_exit" table / segment. Multiple
 *        registrations with the same string ID should trigger a runtime error.
 *
 * @param id A string identifier of the component.
 * @param handler Deinitialization routine with the following protoype
 *        "compageStatus_t handler(pdata_t *pdata);", where the pdata_t is the
 *        custom data type of the component.
 **/
#define COMPAGE_REGISTER_EXIT(id, handler) _COMPAGE_REGISTER_EXIT(id, handler)


/** @def COMPAGE_REGISTER_EXIT(id, handler)
 *
 * @brief Mark component's private data structure variables (single or multiple)
 *        for the component associated with given ID. The macro will store
 *        component's id (address), address of the name for the configurable
 *        variable, variable's type id and its offset in the pdata structure in
 *        a "compage_config" table / segment.
 *
 * @param id A string identifier of the component.
 * @param pdata Default private data structure of the component (passed by value).
 * @param ... List of variables that should be marked for the configuration.
 **/
#define COMPAGE_ADD_CONFIG(id, pdata, ...) _COMPAGE_ADD_CONFIG(id, pdata, __VA_ARGS__)


#define compage_callback_register(handler, type, arg)\
  _compage_callback_register(\
    (compageCallbackHandler_t)handler,\
    (compageCallbackType_t)type,\
    (void*)arg)


compageStatus_t compage_main(int argc, char *argv[]);
const char* compage_get_name(void *pdata);
const char* compage_get_sid(void *pdata);
unsigned    compage_get_id(void *pdata);
compageState_t compage_get_current_state(void *pdata);
const char* compage_get_current_state_str(void *pdata);


#endif
