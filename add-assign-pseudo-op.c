/* Like the --> pseudo-operator in iter-backwards.c, there's an "alternate
 * add-assign" pseudo-operator too. Requires C89 or later. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>

int main(void) {
    signed char x = 29;
    x -=- 1;
    x -=- (1 + 1);

    assert(x == 32);

    printf("If the current coding system is ASCII or ASCII-compatible,\n"
           "this is a space character -->%c<--\n\n",
           x);

    /* What we've actually written here is, of course, "x -= -1" and "x -= -(1 +
     * 1)", just with deceptive whitespace. Part of the trick is that the unary
     * - (sign-negation) is actually an operator, and not restricted to
     * appearing before a literal (as you might have expected).
     *
     * Thus, expressions like the following are also perfectly legal: */

    printf("%hd\n", -x);
    printf("%f\n", -sqrt(4.0F));

    /* We can also the "alternate add-assign" trick with unsigned integers: */

    {   /* New scope to get around the "no mixed declarations and code" rule. */
        unsigned int y = 40;
        y -=- 37;
        assert(y == 77);
        printf("%u\n", y);
    }

    /* Although this might appear it could cause potential portability problems
     * what with the weak type coercion and mixing signedness, consulting the
     * latest ANSI X3.159-1989 draft I can find and taking clauses $3.1.2.5,
     * $3.2.1.2, $3.2.1.5, and $3.3.16.2 together, we find that the signed
     * rvalue will be converted to unsigned int because the lvalue is an
     * unsigned int, and, since the rvalue is negative, it will be converted to
     * unsigned int by adding it to INT_MAX + 1; then when it's subtracted from
     * y, that subtraction will be performed modulo INT_MAX -- all of which
     * comes together to make this operation fully defined. Phew! */

    return EXIT_SUCCESS;
}
