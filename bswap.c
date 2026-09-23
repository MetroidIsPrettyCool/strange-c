/* Starting with the 80486, x86 processors have an instruction called "bswap"
   which reverses the order of bytes in a register. 0x01234567 becomes
   0x67452301, and with a width extension prefix 0x0123456789ABCDEF becomes
   0xEFCDAB8967452301. Very useful for converting between endians.

   Unfortunately, C does not have any standard library mechanisms that
   correspond to a bswap. Intel provides intrinsics, but what if we aren't on a
   platform with Intel intrinsics? */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <limits.h>
#include <stddef.h>

#if CHAR_BIT != 8
    #error "This demo requires 8-bit characters."
#endif

/* The easiest way to perform a portable bswap is to use uchar pointers. Clang
   and GCC are smart enough to optimize this to a simple bswap with -O2. */
inline unsigned my_bswap1(unsigned x) {
    unsigned char buf[sizeof x];
    unsigned char* p = (unsigned char*)&x;

    size_t i;
    size_t j;
    for (i = sizeof x, j = 0; i-- > 0; j++) {
        buf[i] = p[j];
    }

    return *(unsigned*)buf;
}

extern inline unsigned my_bswap1(unsigned x);

/* But what if we're using a compiler that isn't smart enough to optimize this?
   Do we really need that many local variables? We don't want our portable
   intrinsic to be slower than the alternative!

   If we know the size of an unsigned at compile time, we can use C99 compound
   literals to accomplish this in a single expression.

   Clang will optimize this to a single bswap with -O1, GCC with -O2, and
   otherwise it becomes a series of moves and shifts. */
inline unsigned my_bswap2(unsigned x) {
    assert(sizeof x == 4);
    return *(unsigned*)(unsigned char[4]){
        [0] = ((unsigned char*)&x)[3],
        [1] = ((unsigned char*)&x)[2],
        [2] = ((unsigned char*)&x)[1],
        [3] = ((unsigned char*)&x)[0],
    };
}

extern inline unsigned my_bswap2(unsigned x);

/* Here's the funny thing, though. No matter WHAT optimization settings you use,
   GCC will never inline this call to a bswap. For whatever reason, the
   optimization pass that turns the bodies of my_bswap1 and 2 to mov, bswap, ret
   happens after the body is copied into the caller, so compiling this intrinsic
   with inlining will actually give slower, longer results than just calling it.
   Stupid! Clang doesn't have this problem.

   To get a properly functioning portable intrinsic, we need this to be a
   function-like macro: */

#define my_bswap3(x) (*(unsigned*)(unsigned char[4]){ \
  [0] = ((unsigned char*)&(x))[3],                    \
  [1] = ((unsigned char*)&(x))[2],                    \
  [2] = ((unsigned char*)&(x))[1],                    \
  [3] = ((unsigned char*)&(x))[0],                    \
})

/* This still malfunctions in GCC when the optimizer trips over signed-unsigned
   conversions, but at least it works sometimes. */

int main(void) {
    unsigned x;
    scanf("%u", &x);
    printf("%X\n", my_bswap1(x));
    printf("%X\n", my_bswap2(x));
    printf("%X\n", my_bswap3(x));
}
