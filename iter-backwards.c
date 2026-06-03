/* Here are a few (potentially) slightly confusing ways to iterate backwards
 * through an array, as well as an demonstration of the upcoming C2Y countof()
 * operator (if you've got it available) for calculating the number of elements
 * in an array.
 *
 * Each method requires a different minimum language version, they've been #if
 * gated to ensure only the supported ones will be compiled. */

#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <stddef.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdint.h>
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #include <stdckdint.h>
#endif

#if defined(__STDC_VERSION__)                   \
    && __STDC_VERSION__ > 202311L               \
    && __has_include(<stdcountof.h>)

    #include <stdcountof.h>
    #define ARRAY_COUNT(a) countof(a)

#else
    #define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#endif

int main(void) {
    int const array[] = {
        0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07,
        0x08,  0x09,  0x0A,  0x0B,  0x0C,  0x0D,  0x0E,  0x0F,
        0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,
        0x18,  0x19,  0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F
    };

    puts("Method 1: the \"-->\" pseudo-operator:");
    {
        size_t i;        /* C89 does not permit for loop initial declarations */
        for (i = ARRAY_COUNT(array); i --> 0;) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    }

    /* "-->" is not a real operator, of course, although in this context it
     * looks like some kind of "iterate to this number" syntax. If we shuffle
     * the whitespace around, what we've actually written is "(i--) > 0" --
     * equivalent to testing that i is greater than 0, then decrementing i, then
     * continuing to loop if the test came back true. */

    puts("\nMethod 1a (less deceptive):");
    {
        size_t i = ARRAY_COUNT(array);
        while (i > 0) {
            --i;
            fprintf(stdout, "%02X\n", array[i]);
        }
    }

    /* Clearer? */

    puts("\nMethod 2: cast to ptrdiff_t:");
    {
        ptrdiff_t i;
        for (i = (ptrdiff_t)ARRAY_COUNT(array); i >= 0; --i) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    }

    /* There's actually nothing underhanded here, I've just found that it's very
     * apparently unintuitive for people to consider ptrdiff_t to the signed
     * equivalent to size_t. I mean, I guess the cast to signed could itself be
     * underhanded if you're iterating through a very large array... but I
     * digress.
     *
     * ptrdiff_t is not *exactly* "the signed equivalent to size_t", of course,
     * and no version of the standard makes such a claim, but if you read
     * between the lines it's hard to imagine a reasonable implementation where
     * it isn't *morally* "signed size_t". The ptrdiff_t(3type) man page
     * concurs:
     *
     * "Used for a count of elements, or an array index. It is the result of
     * subtracting two pointers. It is a signed integer type capable of storing
     * values in the range [PTRDIFF_MIN, PTRDIFF_MAX]."
     *
     * Anyway I wanted to bring this up because POSIX has a sneakily named
     * "ssize_t" type that *isn't* actually a signed equivalent of size_t, and
     * that really trips people up. ssize_t is... weird, and not guaranteed to
     * be able to store negative numbers below -1, because it's meant for
     * returning a size or an *error*, like read(3) or write(3), and not
     * representing negative indices. (That's what ptrdiff_t is for.) */

    #if defined(__STDC_VERSION__) \
        && __STDC_VERSION__ >= 202311L \
        && BITINT_MAXWIDTH >= SIZE_WIDTH + 1

        fprintf(stdout, "\nMethod 3: cast to _BitInt(%d):\n", SIZE_WIDTH + 1);
        typedef _BitInt(SIZE_WIDTH + 1) bisw1;
        for (bisw1 i = (bisw1)ARRAY_COUNT(array) - 1; i >= 0; --i) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        fprintf(stdout, "\nMethod 3: cast to _BitInt(%d):\n", SIZE_WIDTH + 1);
        puts("Not supported by your compiler! (Did you try -std=c23?)");
    #else
        puts("\nMethod 3: cast to _BitInt(%d):");
        puts("Not supported by your compiler! (Did you try -std=c23?)");
    #endif

    /* This one isn't all that underhanded either, but it is pretty ugly. All we
     * want is a signed integer type that to get a signed integer type big
     * enough to fit both SIZE_MAX and -1, so that we don't have to worry about
     * precision loss or not being able to represent negative numbers.
     *
     * The problem is that C doesn't really provide any portable facilities for
     * describing integer types relative to any of the standard synonym types,
     * so we have to lean on C23 bit-precise integers instead -- hence the
     * necessity for a #if check that SIZE_WIDTH + 1 isn't too wide for the
     * compiler to cope.
     *
     * If the compiler isn't smart enough to optimize this into a 64-bit
     * unsigned variable, having an non-power-of-2, wider-than-pointer-sized
     * index will probably incur some overhead. GCC with -O0 represents i as the
     * register pair rdx:rax, and takes two or three times as many instructions
     * to do a given operation with it when compared to any of the other methods
     * mentioned here. (This goes away with -O1, at least.)
     *
     * So, y'know, be mindful. */

    puts("\nMethod 4: != SIZE_MAX:");
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        for (size_t i = ARRAY_COUNT(array) - 1; i != SIZE_MAX; --i) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    #else
        puts("Not supported by your compiler! (Did you try -std=c99?)");
    #endif

    /* This works because, unlike signed integer arithmetic, unsigned integer
     * wraparound IS well-defined.
     *
     * From the latest ANSI X3.159-1989 draft I can find, $3.1.2.5, paragraph 5;
     * or N1256/N1570/N2176 $6.2.5, paragraph 9:
     *
     * "A computation involving unsigned operands can never overflow, because a
     * result that cannot be represented by the resulting unsigned integer type
     * is reduced modulo the number that is one greater than the largest value
     * that can be represented by the resulting type."
     *
     * (N3220 and N3854 [latest C2Y draft at time of writing] say the same thing
     * in different language in paragraphs 11 or 13, respectively, of $6.2.5.)
     *
     * Ergo, we know for a fact that SIZE_MAX will be the value of i once it's
     * decremented below 0. */

    puts("\nMethod 4a (less deceptive):");
    {
        size_t i;
        for (i = ARRAY_COUNT(array) - 1; i != (size_t)0 - (size_t)1; --i) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    }

    /* This version also compiles in C89, which is nice. */

    puts("\nMethod 5: !ckd_sub():");
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        for (size_t i = ARRAY_COUNT(array); !ckd_sub(&i, i, 1);) {
            fprintf(stdout, "%02X\n", array[i]);
        }
    #else
        puts("Not supported by your compiler! (Did you try -std=c23?)");
    #endif

    /* This one works a lot like "-->", but uses the new C23 stdbit type generic
     * checked arithmetic macros instead; in this case ckd_sub() returns false
     * (true before we logically invert it) when i goes below zero, and not
     * "when the previous value of i was 0" like how post-decrement does it.
     *
     * Some testing with godbolt indicates that (at time of writing) current
     * versions of GCC are able to optimize this version better than the other
     * two, which feels correct. Clang is smart enough to figure out they're all
     * the same. */

    puts("\nMethod 5a (less deceptive):");
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        {
            size_t i; bool i_less_than_0;
            for (i_less_than_0 = ckd_sub(&i, ARRAY_COUNT(array), 1);
                 !i_less_than_0;
                 i_less_than_0 = ckd_sub(&i, i, 1))
            {
                fprintf(stdout, "%02X\n", array[i]);
            }
        }
    #else
        puts("Not supported by your compiler! (Did you try -std=c23?)");
    #endif

    /* With optimizations GCC (and Clang, for that matter) on x64 treats this
     * exactly the same as the more deceptive version, presumably because it
     * understands i_less_than_0 is only an alias for the carry (unsigned
     * under/overflow) flag. */

    return EXIT_SUCCESS;
}
