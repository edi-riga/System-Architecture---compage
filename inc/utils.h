#ifndef _UTILS_H_
#define _UTILS_H_

#define _I(fmt, args...)  printf(fmt "\n", ##args)
#define _W(fmt, args...)  printf("WARNING: " fmt "\n", ##args)
#define _E(fmt, args...)  fprintf(stderr, "ERROR: " fmt "\n", ##args)

#if defined(DEBUG) or defined(COMPAGE_DEBUG)
    #define _D(fmt, args...)  printf("DEBUG: %s: " fmt "\n", __func__, ##args)
#else
    #define _D(fmt, args...)
#endif

#endif
