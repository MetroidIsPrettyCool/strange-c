/* There are three different, mostly unrelated, meanings of the static keyword
 * in C as of C99: */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

/* When static is used as a storage class specifier in file scope, it gives the
 * identifier internal linkage. This is the default for objects declared at file
 * scope ("global variables") but not for functions.
 *
 * This function "foo" cannot be referenced by any other translation units
 * (object files). */

static /* <- 1 */ void foo(size_t n,

/* When static is used in the brackets of an array declarator, it indicates that
 * it is UB to pass an array of fewer elements than the value of the size
 * expression. In this case, arr must be at least as big as the value of n.
 * (This requires VLA support in C11 and later if the size expression is not a
 * constant expression, like we've written here.) */

#if defined(__STDC_VERSION__) \
    && (__STDC_VERSION__ >= 199901L \
        || (__STDC_VERSION__ >= 201112L \
            && (!defined(__STDC_NO_VLA__) || __STDC_NO_VLA__ != 1)))
                           int arr[static /* <- 2 */ n]
#else
                           int arr[]
#endif
) {

/* Finally, when static is used as a storage class specifier in block scope, it
 * indicates that an object will persist for the life of the program and its
 * initializer will only run once -- at program initialization. See N3220
 * $6.2.4.
 *
 * This sum variable will retain its value across calls. */

    static /* <- 3 */ int sum = 0;
    size_t i;

    for (i = 0; i < n; ++i) {
        sum += arr[i];
    }

    printf("%d\n", sum);

/* For a fourth use of the noun "static", if not the keyword: C11 added a
 * _Static_assert keyword (with the alternate spelling "static_assert" available
 * by #including <assert.h> in C11 and C17 -- C23 makes this the default) for
 * performing compile-time assertions on constant expressions. */

    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        static_assert /* <- 4? */ (sizeof(size_t) == sizeof(sizeof(int)),
                                   "FAIL!"); /* This diagnostic message became
                                              * optional in C23. */
    #endif
}

/* For clarify, here's that same function definition without comments or #if
 * guards:
 *
 * static void foo(size_t n, int arr[static n]) {
 *     static int sum = 0;
 *
 *     for (size_t i = 0; i < n; ++i) {
 *         sum += arr[i];
 *     }
 *
 *     printf("%d\n", sum);
 *     static_assert(sizeof(size_t) == sizeof(sizeof(int)), "FAIL!");
 * } */

int main(void) {
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        foo(3, (int[]){1, 2, 3}); /* C99 array compound literals FTW! */
        foo(3, (int[]){4, 5, 6});
        foo(4, (int[]){-1, -2, -3, -4});
    #else
        { int a[3] = {1, 2, 3};        foo(3, a); }
        { int a[3] = {4, 5, 6};        foo(3, a); }
        { int a[4] = {-1, -2, -3, -4}; foo(4, a); }
        return EXIT_SUCCESS;
    #endif
}
