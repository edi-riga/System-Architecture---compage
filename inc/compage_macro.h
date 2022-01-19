#ifndef _COMPAGE_MACRO_H_
#define _COMPAGE_MACRO_H_

/* Quite standard macro stringification approach */
#define STRINGIFY(s)   _STRINGIFY(s)
#define _STRINGIFY(s)  #s

/* Concatination for building new macro function names */
#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

/* Magical (expansion-based) argument counting implementation */
#define ARGUMENT_COUNT(...) ARGUMENT_COUNT_(__VA_ARGS__, ARGUMENT_INDEXES())
#define ARGUMENT_COUNT_(...) ARGUMENT_COUNT_PREPEND(__VA_ARGS__)
#define ARGUMENT_COUNT_PREPEND(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, N, ...) N
#define ARGUMENT_INDEXES() 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/* The famous container_of macro, the macro retreives the member container
 * structure when given a member of this function (reference name and address).
 * Macro takes the following arguments
 * - container_t   data type of the encapsulating structure
 * - member_ref    name / reference of the member whoms address we are passing
 * - member_addr   the address of the member
 *
 * For example, we can retreive compage_t from its pdata member with:
 *     CONTAINER_OF(compage_t, pdata, pdata_ptr) */
#define CONTAINER_OF(container_t, member_ref, member_addr) \
((container_t*)( \
  (uint64_t)member_addr - \
  (uint64_t)(((container_t*)0)->member_ref) \
))

#endif
