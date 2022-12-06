#ifndef _COMPAGE_HASH_H_
#define _COMPAGE_HASH_H_

#include "compage_macro.h"

/**
 * Implement compile-time string hashing on string literals. Taken from
 * https://stackoverflow.com/questions/28654707/how-to-calculate-the-hash-of-a-string-literal-using-only-the-c-preprocessor
 *
 * This macro implements the widely used "djb" hash apparently posted
 * by Daniel Bernstein to comp.lang.c some time ago.  The 32 bit
 * unsigned hash value starts at 5381 and for each byte 'c' in the
 * string, is updated: ``hash = hash * 33 + c``.  This
 * function uses the signed value of each byte.
 *
 * note: this is the same hash method that glib 2.34.0 uses.
 */

#ifndef COMPAGE_SEED
  #define COMPAGE_SEED 5381
#endif

#if 1
  // correct but causes insane expansion
  #define _SH(e, c) (((e) << 5) + (e) + (unsigned char)(c))
#elif defined(__GNUC__)
  // Use statement-expression extension
  #define _SH(e, c) ((e<<5) + e + (unsigned char)c)
  //#define _SH(e, c) ({ unsigned int _e = (unsigned int)(e); (_e << 5) + _e + (unsigned char)c; })
#else
  // use an inline function, the compiler will be able to optimize this out.
  static inline unsigned int _SH(unsigned int e, unsigned char c)
  {
      unsigned int _e = (unsigned int)e;
      return (_e << 5) + _e + (unsigned char)c;
  }
#endif

#define _SH_1(a) _SH(COMPAGE_SEED, (a)[0])
#define _SH_2(a) _SH(_SH_1(a), (a)[1])
#define _SH_3(a) _SH(_SH_2(a), (a)[2])
#define _SH_4(a) _SH(_SH_3(a), (a)[3])
#define _SH_5(a) _SH(_SH_4(a), (a)[4])
#define _SH_6(a) _SH(_SH_5(a), (a)[5])
#define _SH_7(a) _SH(_SH_6(a), (a)[6])
#define _SH_8(a) _SH(_SH_7(a), (a)[7])
#define _SH_9(a) _SH(_SH_8(a), (a)[8])
#define _SH_10(a) _SH(_SH_9(a), (a)[9])
#define _SH_11(a) _SH(_SH_10(a), (a)[10])
#define _SH_12(a) _SH(_SH_11(a), (a)[11])
#define _SH_13(a) _SH(_SH_12(a), (a)[12])
#define _SH_14(a) _SH(_SH_13(a), (a)[13])
#define _SH_15(a) _SH(_SH_14(a), (a)[14])
#define _SH_16(a) _SH(_SH_15(a), (a)[15])
#define _SH_17(a) _SH(_SH_16(a), (a)[16])
#define _SH_18(a) _SH(_SH_17(a), (a)[17])
#define _SH_19(a) _SH(_SH_18(a), (a)[18])
#define _SH_20(a) _SH(_SH_19(a), (a)[19])
#define _SH_21(a) _SH(_SH_20(a), (a)[20])
#define _SH_22(a) _SH(_SH_21(a), (a)[21])
#define _SH_23(a) _SH(_SH_22(a), (a)[22])
#define _SH_24(a) _SH(_SH_23(a), (a)[23])
#define _SH_25(a) _SH(_SH_24(a), (a)[24])
#define _SH_26(a) _SH(_SH_25(a), (a)[25])
#define _SH_27(a) _SH(_SH_26(a), (a)[26])
#define _SH_28(a) _SH(_SH_27(a), (a)[27])
#define _SH_29(a) _SH(_SH_28(a), (a)[28])
#define _SH_30(a) _SH(_SH_29(a), (a)[29])
#define _SH_31(a) _SH(_SH_30(a), (a)[30])
#define _SH_32(a) _SH(_SH_31(a), (a)[31])

#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<sizeof(s)?sizeof(s)-1-(i):sizeof(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))

#define HASH(s)    ((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

// initial check prevents too-large strings from compiling
#define STRHASH(a) ( \
    (void)(sizeof(int[(sizeof(a) > 33 ? -1 : 1)])), \
    (sizeof(a) == 1) ? COMPAGE_SEED : \
    (sizeof(a) == 2) ? _SH_1(a) : \
    (sizeof(a) == 3) ? _SH_2(a) : \
    (sizeof(a) == 4) ? _SH_3(a) : \
    (sizeof(a) == 4) ? _SH_3(a) : \
    (sizeof(a) == 5) ? _SH_4(a) : \
    (sizeof(a) == 6) ? _SH_5(a) : \
    (sizeof(a) == 7) ? _SH_6(a) : \
    (sizeof(a) == 8) ? _SH_7(a) : \
    (sizeof(a) == 9) ? _SH_8(a) : \
    (sizeof(a) == 10) ? _SH_9(a) : \
    (sizeof(a) == 11) ? _SH_10(a) : \
    (sizeof(a) == 12) ? _SH_11(a) : \
    (sizeof(a) == 13) ? _SH_12(a) : \
    (sizeof(a) == 14) ? _SH_13(a) : \
    (sizeof(a) == 15) ? _SH_14(a) : \
    (sizeof(a) == 16) ? _SH_15(a) : \
    (sizeof(a) == 17) ? _SH_16(a) : \
    (sizeof(a) == 18) ? _SH_17(a) : \
    (sizeof(a) == 19) ? _SH_18(a) : \
    (sizeof(a) == 20) ? _SH_19(a) : \
    (sizeof(a) == 21) ? _SH_20(a) : \
    (sizeof(a) == 22) ? _SH_21(a) : \
    (sizeof(a) == 23) ? _SH_22(a) : \
    (sizeof(a) == 24) ? _SH_23(a) : \
    (sizeof(a) == 25) ? _SH_24(a) : \
    (sizeof(a) == 26) ? _SH_25(a) : \
    (sizeof(a) == 27) ? _SH_26(a) : \
    (sizeof(a) == 28) ? _SH_27(a) : \
    (sizeof(a) == 29) ? _SH_28(a) : \
    (sizeof(a) == 30) ? _SH_29(a) : \
    (sizeof(a) == 31) ? _SH_30(a) : \
    (sizeof(a) == 32) ? _SH_31(a) : \
    (sizeof(a) == 33) ? _SH_32(a) : \
    0)
// last zero is unreachable


#endif
