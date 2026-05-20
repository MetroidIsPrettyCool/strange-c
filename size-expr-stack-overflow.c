/* Size expressions can be /any/ expression, and are evaluated before their
 * respective blocks "each time the declaration is reached in the order of
 * execution" ---N3220 $6.8.1 paragraph 3.
 *
 * Here's an amusing program that calls foo() but causes a stack overflow before
 * it can even reach foo()'s function body. Requires C99, or C11 and later with
 * VLAs. */

#include <stdio.h>

size_t foo(char s[*]);

size_t foo(char s[static foo("lol")]) {
    (void)s;
    puts("got here!");
    return 0xBEEF;
}

int main(void) {
    foo("lmao");
}
