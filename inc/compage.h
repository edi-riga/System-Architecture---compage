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


/** @def compage_callback_register(handler, type, arg)
 *
 * @brief Register callback function associated with one of the component
 *        execution phases: preinit, postinit, preloop, postloop, preexit,
 *        postexit.
 *
 * @param handler Address of the to-be-called function during the component
 *        execution phases. The callback has the following prototype:
 *        void handler(void *pdata_cb, void *pdata_component); the pdata_cb
 *        is the callback's set private data (argument parameter) and
 *        pdata_component is the componet's private data.
 * @param type Type of the callback to be registered, according to the
 *        compageCallbackType_t type.
 * @param argument Private data of the callback function, which can be used
 *        to transfer information from one callback to another, logging,
 *        execution time measurements, etc.
 **/
#define compage_callback_register(handler, type, arg)\
  _compage_callback_register(\
    (compageCallbackHandler_t)handler,\
    (compageCallbackType_t)type,\
    (void*)arg)


/** @def compage_callback_register_all(handler, type, arg)
 *
 * @brief Register (associate) sinle callback function to all of the component
 *        execution phases: preinit, postinit, preloop, postloop, preexit,
 *        postexit.
 *
 * @param handler Address of the to-be-called function during the component
 *        execution phases. The callback has the following prototype:
 *        void handler(void *pdata_cb, void *pdata_component); the pdata_cb
 *        is the callback's set private data (argument parameter) and
 *        pdata_component is the componet's private data.
 * @param type Type of the callback to be registered, according to the
 *        compageCallbackType_t type.
 * @param argument Private data of the callback function, which can be used
 *        to transfer information between the stages, logging, execution time
 *        measurements, etc.
 **/
#define compage_callback_register_all(handler, arg)\
  compage_callback_register(handler, COMPAGE_CALLBACK_PREINIT,  arg);\
  compage_callback_register(handler, COMPAGE_CALLBACK_POSTINIT, arg);\
  compage_callback_register(handler, COMPAGE_CALLBACK_PRELOOP,  arg);\
  compage_callback_register(handler, COMPAGE_CALLBACK_POSTLOOP, arg);\
  compage_callback_register(handler, COMPAGE_CALLBACK_PREEXIT,  arg);\
  compage_callback_register(handler, COMPAGE_CALLBACK_POSTEXIT, arg);

/* Force C-compatible ABI when using CPP compiler */
#ifdef __cplusplus
extern "C" {
#endif

/** @brief Default (and simplest) framework initialization using the internal
 *         compage command line. Command line arguments enables listing the
 *         components and their configuration, generating ini configuration
 *         files and launching compage application using either default
 *         configuration or supplied configuration file.
 *
 * @param argc "Argument count" in argv array.
 * @param argv "Argument values", array of pointers to the CLI arguments
 *
 * @return On success returns COMPAGE_SUCCESS, otherwise indicates the error.
 **/
compageStatus_t compage_main(int argc, char *argv[]);


/** @brief Registers SIGINT signal handler in the compage framework, e.g. in
 *         case of "Ctrl+C", all launched components will be gracefully
 *         deinitialized. Future implementations may add additional signals.
 *
 * @return On success returns COMPAGE_SUCCESS, otherwise returns
 *         COMPAGE_SYSTEM_ERROR and prints error message.
 **/
compageStatus_t compage_configure_signaling();


/** @brief Generates application's default configuration file in the fpath
 *         directory. The configuration file follows INI file format, the
 *         section names correspond to the component configuration while options
 *         are directly applied in the component's data structure.
 *
 * @param fpath Cstring containing location for writing the configration file.
 *         If there the file already exists, its contests are overwritten.
 *
 * @return On success returns COMPAGE_SUCCESS, otherwise return code indicates
 *         the error.
 **/
compageStatus_t compage_generate_config(const char *fpath);

/** TODO: Documentation **/
const char* compage_get_name(void *pdata);
/** TODO: Documentation **/
const char* compage_get_sid(void *pdata);
/** TODO: Documentation **/
unsigned    compage_get_id(void *pdata);

/** TODO: Documentation **/
compageState_t compage_get_state(void *pdata);
/** TODO: Documentation **/
compageState_t compage_get_state_by_name(const char *name);
/** TODO: Documentation **/
compageState_t compage_get_state_by_sid(const char *sid);
/** TODO: Documentation **/
compageState_t compage_get_state_by_id(unsigned id);
/** TODO: Documentation **/
const char* compage_get_state_str(compageState_t state);

/** TODO: Documentation **/
compageStatus_t compage_launch();
/** TODO: Documentation **/
compageStatus_t compage_launch_by_name(const char *name);
/** TODO: Documentation **/
compageStatus_t compage_launch_by_sid(const char *sid);
/** TODO: Documentation **/
compageStatus_t compage_launch_by_id(unsigned id);

/** TODO: Documentation **/
compageStatus_t compage_kill_all();
/** TODO: Documentation **/
compageStatus_t compage_kill_by_name(const char *name);
/** TODO: Documentation **/
compageStatus_t compage_kill_by_sid(const char *sid);
/** TODO: Documentation **/
compageStatus_t compage_kill_by_id(unsigned id);

/** TODO: Documentation **/
compageStatus_t compage_init_from_file(const char *fpath);
/** TODO: Documentation **/
void compage_join_pthreads();

/* END - extern "C" */
#ifdef __cplusplus
}
#endif

#endif
