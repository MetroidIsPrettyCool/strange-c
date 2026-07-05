/* Commas are a headache. This demo requires C99 (or later with VLAs) for the
 * designated initializer example and the size expression VLA example. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

void foo(size_t baz, int a[*], int b[*]);
size_t bar(char* s, size_t x);

int main(void) {
    /* The comma _operator_ makes the order of evaluation explicit: */
    {
        int foo = 1;
        int bar = (foo++, foo++);
        assert(foo == 3 && bar == 2);
    }

    /* Likewise, the comma used to separate elements of init-declaration lists
     * also makes order of evaluation explicit: */
    {
        int foo = 1;
        int bar = foo++, baz = foo++;
        assert(foo == 3 && bar == 1 && baz == 2);
    }

    /* The commas used to separate braced initializers and function call
     * parameters do not, however: */
    {
        int foo = 0;
        int bar[] = { foo++, foo++, foo++ };
        for (int i = 0; i < sizeof bar / sizeof *bar; ++i) {
            printf("%d, ", bar[i]);
        }
        printf("%d\n", foo);

        foo = 0;
        printf("%d\n", div(++foo, ++foo).quot);
    }
    /* The bar initializer and the div call are both UB because they have
     * multiple unsequenced side effects on the same object (N3220 $6.5.1 para.
     * 2). GCC prints "0, 1, 2, 3" and "1". Clang, ICX, and TCC print "0, 1, 2,
     * 3" and "0". slimcc and DMD print "0, 1, 2, 3" and "2". Great. */

    /* Designated initializer lists are a special kind of hell. If multiple
     * initializers designate the same object, whichever comes last wins in a
     * well-defined manner, sort of implying there's sequencing going on.
     *
     * However, the order in which the initializer expressions are evaluated is
     * still unspecified! Moreover, it's _also_ unspecified whether the
     * overridden initializers are even run at all: */
    {
        int foo = 0;
        int bar[] = {
            [2] = foo++,
            [2] = 6,
            [0] = foo++,
            [1] = foo++,
            [2] = foo++
        };
        for (int i = 0; i < sizeof bar / sizeof *bar; ++i) {
            printf("%d, ", bar[i]);
        }
        printf("%d\n", foo);
    }
    /* Altogether this behavior is so obtuse I initially thought I'd misread the
     * standard. Also, this is UB for the same reasons as the regular braced
     * initializer list.
     *
     * On my system, under GCC, Clang, ICX, and slimcc, this prints "0, 1, 2,
     * 3". Under DMD and TCC, this prints "1, 2, 3, 4", presumably because they
     * _aren't_ discarding the side effects of the overridden initializer like
     * everyone else. Nightmarish. */

    /* Finally, when calling a function that has parameters with non-constant
     * array size expressions, each parameter is only declared for the
     * subsequent elements, just like an init-declaration list. For example,
     * given the function
     *
     *   void foo(int a, int b[*], int c, int d[*])
     *
     * then b can have a size expression that refers to a, and d to c, b, or
     * a.
     *
     * However, their evaluations are still not ordered. */
    {
        int a[1000] = {};
        int b[1000] = {};

        foo(10, a, b);
    }
    /* Very much UB, since we're doing major unsequenced side effects. GCC,
     * Clang, ICX, and slimcc print "a 10", "b 11" and "foo 12". TCC prints "foo
     * 10", which is probably just it plugging its ears regarding $6.8.1's
     * requirements for evaluating size expressions upon block entry. Or maybe
     * it just hates me. DMD doesn't compile this because it doesn't support
     * VLAs or [static] array parameter declarators. */
}

size_t bar(char* s, size_t x) {
    printf("%s %zu\n", s, x);
    return x;
}

void foo(size_t baz, int a[static bar("a", baz++)], int b[bar("b", baz++)]) {
    printf("foo %zu\n", baz);
}
