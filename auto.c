/* Does this program compile? If so, what does it print? Think carefully. :) */

#include <stdlib.h>

#include "inc/p_lit.h" /* The P_LIT(x) macro will attempt to print its argument
                        * as a literal, e.g. 12.3F, 1UL, 'b', etc. */

int main(void) {
    auto x = 12.34F;
    auto y = 'a';

    P_LIT(x); fputs(", ", stdout);
    P_LIT(y); putchar('\n');
    return EXIT_SUCCESS;
}

/* There are three possible correct answers, depending on the language version.
 * (If your system doesn't use ASCII, ISO/IEC 8859, or Unicode, pretend I wrote
 * whatever your codepoint for 'a' is instead of 97.)
 *
 * - In C89 and C95, this prints "12, 97".
 * - In C99, C11, and C17, this doesn't compile.
 * - In C23, this prints "12.340000F, 97".
 *
 * Only in C++ does this print "12.340000F, 'a'".
 *
 * Huh?
 *
 * The C23 behavior is the easiest to explain: in C23, a declaration with the
 * auto keyword and no type tells the compiler to do type inference, just like
 * in C++. Unlike C++, however, character literals have the type int. Curveball!
 * Thus, x is a float, and y is an int.
 *
 * Next, the C99, C11 and C17 behavior: in those language versions, this won't
 * compile, but only because we haven't specified a type for x and y. auto is,
 * in fact, still a keyword, and it is legal here.
 *
 * Specifically, auto is a storage-class specifier (like static, or
 * thread_local, or constexpr) and means "automatic storage duration"; AKA "this
 * variable is freed at the end of the enclosing scope", AKA "this is a stack
 * variable". Automatic storage is the default storage class for local variables
 * (of course), and also only valid for local variables, so prior to C23
 * specifying it was always redundant. Since C23 it's been special-cased to do
 * type inference if you write a declaration without a type (a so-called "direct
 * declarator"), and to behave like it used to otherwise.
 *
 * Finally, the C89/C95 behavior: as in later C language versions, auto is a
 * storage specifier. It does not mean type inference. *Unlike* later versions,
 * however, declarations without types are automatically given the type int.
 * Thus, x is an int, and y is an int. Confusing!
 *
 * Takeaway: be careful using auto in mixed C codebases :) */
