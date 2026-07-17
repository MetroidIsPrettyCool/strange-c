/* Example of a non-empty array structure type using char punning and flexible
 * array members, (potentially) a useful complement for [static 1] declarators.
 * AFAICT this does not violate strict aliasing because character types are...
 * privileged.
 *
 * This example uses the C23 features char8_t and stdc_leading_ones(), I have
 * provided some (naive) C11/C17 backports to allow this to compile under
 * CompCert. */

#include <stdio.h>
#include <stdlib.h>

#include <stddef.h>
#include <string.h>

#if __STDC_VERSION__ >= 202311L
    #include <stdbit.h>
    #include <uchar.h>
#else
    #include <assert.h>
    #include <limits.h>
    #include <stdalign.h>
    #include <stdbool.h>
    typedef unsigned char char8_t;

    static inline unsigned stdc_leading_ones(unsigned char value) {
        unsigned bit = 1 << (CHAR_BIT - 1);
        unsigned count = 0;
        while (true) {
            if (value & bit) {
                ++count;
                if (bit != 1) bit >>= 1;
                else          break;
            } else break;
        }
        return count;
    }
#endif

struct mbc {
    char8_t first;
    char8_t rest[];
};
typedef struct mbc* MBC;

static_assert(
    alignof(struct mbc) == alignof(char8_t),
    "alignment of non-empty char8_t array struct differs from the fundamental"
    " alignment, precluding char8_t[] aliasing"
);
static_assert(
    sizeof(struct mbc) == sizeof(char8_t),
    "non-empty char8_t array struct has padding between fields (detected from"
    " size), precluding char8_t[] aliasing"
);
static_assert(
    offsetof(struct mbc, rest) == sizeof(char8_t),
    "non-empty char8_t array struct has padding between fields (detected from"
    " offset of .rest member), precluding char8_t[] aliasing"
);

/* Get next multi-byte character in UTF-8 string PTR.
 *
 * Call with non-null PTR to set up internal static variable and get the first
 * character, call with null PTR to get subsequent characters.
 *
 * The out-parameter MBC will be set to a pointer to the multi-byte character,
 * and the length thereof will be returned.
 *
 * Constraints:
 *
 * All returned pointers will remain valid until next_mbc is called again with a
 * non-null PTR.
 *
 * Do not pass pointers to non-UTF-8 strings in PTR. Do not call with null PTR
 * argument before calling with a non-null PTR argument. Do not call with a null
 * PTR argument after a null character (mbc->first) is returned. Do not call
 * with a null MBC argument. */
unsigned next_mbc(char8_t* ptr, MBC mbc[static 1]) {
    static char8_t* s = NULL;
    if (ptr) {
        if (s) free(s);
        size_t len = 0;
        while (ptr[len] != 0x00) ++len;
        s = malloc(len);
        memcpy(s, ptr, len);
    };

    *mbc = (MBC)s;

    unsigned len;
    {
        len = stdc_leading_ones(*s);
        len = len ? len : 1;
    }
    s += len;
    return len;
}

/* Print the characters of the UTF-8 string S one by one on individual lines. */
int main(void) {
    char8_t s[] = u8"123abcабгꙮ︘🫪";

    MBC mbc;
    for (
        unsigned len = next_mbc(s, &mbc);
        mbc->first != 0x00;
        len = next_mbc(NULL, &mbc)
    ) {
        char8_t tmp[5] = { 0 };
        memcpy(tmp, mbc, len);
        printf("%s\n", tmp);
    }

    return 0;
}
