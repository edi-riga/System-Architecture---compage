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
 * @param pdata Address of the default private data structure of the component.
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
 * @param type Type of the struct container (often a type defined with typedef)
 * @param ... List of variables that should be marked for the configuration.
 **/
#define COMPAGE_ADD_CONFIG(id, type, ...) _COMPAGE_ADD_CONFIG(id, type, __VA_ARGS__)


compageStatus_t compage_main(int argc, char *argv[]);


#endif
