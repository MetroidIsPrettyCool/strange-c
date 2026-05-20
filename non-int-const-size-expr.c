/* This is an example of the diagnostic potential (but also limitations) of the
 * C99 static array size keyword when the size expression references another
 * parameter.
 *
 * This program demonstrates three kinds of UB relating to size expressions in
 * array declarations:
 *
 * 1. passing a null pointer as an argument where the parameter is required by
 *    the static keyword to be a non-null pointer (N3220 $6.7.7.4 paragraph 6),
 *
 * 2. a non-constant size expression evaluating to a value not greater than zero
 *    (N3220 $6.7.7.3 paragraph 5), and,
 *
 * 3. passing an array that is smaller than the size expression (N3220 $6.7.7.4
 *    paragraph 6, again).
 *
 * At time of writing, gcc is able to warn on the first and third, and clang is
 * able to warn on only the first. (I should note that gcc is ONLY able to warn
 * on the first UB if the static size expression is no more complicated than an
 * integer constant expression [e.g. "4" or "sizeof(foo)"] or exactly one
 * variable identifier [e.g. "n" or "len" or "size"]. If the size expression
 * were, say, "str_len + 1", then gcc would not produce a -Wstringop-overread
 * diagnostic in the case of a mismatched string length.)
 *
 * $ gcc -std=c99 -Wall -Wextra -pedantic non-int-const-size-expr.c
 * non-int-const-size-expr.c: In function ‘main’:
 * non-int-const-size-expr.c:87:5: warning: argument 2 null where non-null expected [-Wnonnull]
 *    87 |     print_bytes(sizeof(array1),   NULL, 8, 4);
 *       |     ^~~~~~~~~~~
 * non-int-const-size-expr.c:42:6: note: in a call to function ‘print_bytes’ declared ‘nonnull’
 *    42 | void print_bytes(
 *       |      ^~~~~~~~~~~
 * non-int-const-size-expr.c:89:5: warning: ‘print_bytes’ reading 32 bytes from a region of size 5 [-Wstringop-overread]
 *    89 |     print_bytes(sizeof(array1), array2, 8, 4);
 *       |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * non-int-const-size-expr.c:89:5: note: referencing argument 2 of type ‘const char[]’
 * non-int-const-size-expr.c:42:6: note: in a call to function ‘print_bytes’
 *    42 | void print_bytes(
 *       |      ^~~~~~~~~~~
 *
 * $ clang -std=c99 -Wall -Wextra -pedantic non-int-const-size-expr.c
 * non-int-const-size-expr.c:87:5: warning: null passed to a callee that requires a non-null argument
 *       [-Wnonnull]
 *    87 |     print_bytes(sizeof(array1),   NULL, 8, 4);
 *       |     ^                             ~~~~
 * non-int-const-size-expr.c:44:16: note: callee declares array parameter as static here
 *    44 |     char const buf[const restrict static n],
 *       |                ^  ~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1 warning generated.
 *
 * Requires C99, or C11 and later with VLAs. */

#include <stdio.h>

#include <assert.h>
#include <stddef.h>

#define MAX_TAB_WIDTH 256

/* Note that we have to declare n before buf for n to be defined in the size
 * expression. This would not be legal:
 *
 * void print_bytes(
 *     char const buf[const restrict static n],
 *     size_t n,
 *     size_t columns,
 *     size_t tab_width
 * ) { ... }
 *
 * gcc offers an extension called "forward parameter declarations", which would
 * allow us to declare our parameters before the actual parameter list like so:
 *
 * void print_bytes(
 *     size_t n;
 *
 *     char const buf[const restrict static n],
 *     size_t n,
 *     size_t columns,
 *     size_t tab_width
 * ) { ... }
 *
 * But this obviously isn't portable. Alas! */
void print_bytes(
    size_t n,
    char const buf[const restrict static n],
    size_t columns,
    size_t tab_width
) {
    static char spaces[MAX_TAB_WIDTH + 1] = {
        #include "inc/256-space-characters.inc"
        , '\0'
    };

    assert(columns != 0);
    assert(tab_width <= MAX_TAB_WIDTH);

    spaces[tab_width] = '\0';

    for (size_t i = 0; i < n; ++i) {
        fprintf(stdout, "0x%02X", buf[i]);
        fputs((i % columns == columns - 1 || i == n - 1)
              ? "\n"
              : spaces,
              stdout);
    }

    spaces[tab_width] = ' ';
}

int main(void) {
    char array1[] = {
        0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07,
        0x08,  0x09,  0x0A,  0x0B,  0x0C,  0x0D,  0x0E,  0x0F,
        0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,
        0x18,  0x19,  0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F
    };

    char array2[] = {
        1, 2, 3, 4, 5
    };

    /* These are all legal: */
    print_bytes(sizeof(array1), array1, 8,  4);
    print_bytes(sizeof(array1), array1, 5,  0);
    print_bytes(sizeof(array2), array1, 15, 1);
    print_bytes(sizeof(array2), array1, 3,  9);

    /* These are UB: */
    print_bytes(sizeof(array1),   NULL, 8, 4);
    print_bytes(             0, array1, 8, 4);
    print_bytes(sizeof(array1), array2, 8, 4);
}
