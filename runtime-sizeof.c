/* C99 introduced a concept known as Variable Length Arrays (VLAs) -- an array
 * whose size is determined at runtime by evaluating a "size expression" where a
 * constant expression would normally go (i.e. int a[n] vs. int a[4]) --
 * alongside a more general notion of "Variably Modified Types" (VMTs). These
 * have a number of strange quirks, such as requiring a sizeof() operator that's
 * evaluated at runtime.
 *
 * Requires C99, or C11 and later with VLAs. (VLAs were made optional in
 * C11.) */

#include <stdio.h>

#include <assert.h>
#include <stddef.h>

int foo(size_t size, int some_array[size]);

/* If we wanted to leave the size of some_array unspecified (but the type
 * complete) for this prototype, we could write this instead:
 *
 * int foo(size_t, int[*]);
 *
 * It's treated the same as the specified version in function prototypes (which
 * this is) according to N1256 $6.7.5.2 paragraph 5, but Clang doesn't like it
 * -- with -Wall it trips -Warray-parameter. Silly! */

int main(void) {
    for (size_t i = 1; i <= 10; ++i) {
        int some_array[i];

        /* We can't make any C11 static_assert()s with the result of
         * sizeof(some_array) because it's not a constant expression. */

        #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
            /* static_assert(sizeof(some_array) == sizeof(int*), "FAIL!"); */
        #endif

        /* Regular asserts are fine though. */
        assert(sizeof(some_array) == sizeof(int) * i);

        for (size_t j = 0; j < sizeof(some_array) / sizeof(int); ++j)
            some_array[j] = j;

        i += 7;
        printf("%2zd != %2zd (or %2zd), sum = %2d\n",
               sizeof(some_array),
               i * sizeof(int),
               i,
               foo(sizeof(some_array) / sizeof(int), some_array));
        i -= 7;

        /* Notice when running this printf that sizeof(some_array) reflects the
         * actual size of some_array, and not just the current value of i times
         * the size of int. */
    }
}

int foo(size_t size, int some_array[size]) {
    int sum = 0;

    for (size_t i = 0; i < size; ++i) {
        sum += some_array[i];
    }

    /* We have to refer to size and not sizeof(some_array) in this for loop,
     * even though some_array is a VLA parameter, even though we *have*
     * specified the size some_array by N1256 $6.7.5.2 paragraph 1; because by
     * N1256 $6.7.5.3 paragraph 7 the type of some_array in the function body is
     * still just adjusted to "pointer to int". sizeof(some_array) will be equal
     * to sizeof(int*).
     *
     * GCC and Clang will warn about this fact with -Wsizeof-array-argument.
     *
     * Because sizeof(int*) is a constant expression, we can do a C11
     * static_assert() here instead of a regular assert (if the compiler
     * supports C11). */

    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        static_assert(sizeof(some_array) == sizeof(int*), "FAIL!");
    #else
        assert(sizeof(some_array) == sizeof(int*));
    #endif

    /* We can't get around this by casting to a VLA, like
     * "(int[size])some_array", or by declaring a new VLA and initializing it to
     * some_array, like "int some_array1[size] = some_array"; because you can't
     * cast to an array type (N1256 $6.5.4 paragraph 2 [can't cast to a type
     * that isn't scalar or void] and N1256 $6.2.5 paragraph 21 [arrays are not
     * scalar types]), or initialize an non-char, non-wchar_t array type with
     * anything other than an initializer list (N1256 $6.7.8 paragraph 16), or
     * even initialize a VLA whatsoever (N1256 $6.7.8 paragraph 3). */

    return sum;
}
