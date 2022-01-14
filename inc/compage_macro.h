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

#endif
