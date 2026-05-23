/* C99 introduced... uh, many things...
 *
 * One of the most unambiguously useful of these things are designed initializer
 * lists. Here are some examples, some of which you might not have realized were
 * possible. Requires C99, of course. */

#include <assert.h>

int main(void) {
    /* Struct designated initialization: */
    {
        struct foo {
            int a;
            float b;
        };

        struct foo foo1 = {         1,         2 };
        struct foo foo2 = {    .a = 1, .b = 2.0F };
        struct foo foo3 = { .b = 2.0F,    .a = 1 };

        assert(foo1.a == foo2.a && foo2.a == foo3.a);
        assert(foo1.b == foo2.b && foo2.b == foo3.b);
    }

    /* Union designed initialization: */
    {
        union foo {
            int a;
            float b;
        };

        union foo foo1 = { .a = 1 };
        union foo foo2 = { .b = 2 };
        assert(foo1.a == 1);
        assert(foo2.b == 2.0);
    }

    /* Array designed initialization: */
    {
        int foo[] = {
            [3] = 3,
            [1] = 1,
            [2] = 2,
            [0] = 0
        };  /* Everything not mentioned gets zeroed (static initialization). */

        assert(foo[0] == 0);
        assert(foo[1] == 1);
        assert(foo[2] == 2);
        assert(foo[3] == 3);
        assert(sizeof(foo) == 4 * sizeof(int));
    }

    /* All together now: */
    {
        struct {
            union {
                int a;
                float b;
            } u;

            int a;
            float b;

            int c[8];
        } foo = {
            .u = { .a = 1 },

            /* Note that scalar initializers can be surrounded by braces, too:
             * (GCC and Clang both warn about this, as it's dodgy style. Maybe
             * useful for macros, though?) */
            .a = { -2 },

            .b = 7.0,

            /* You can provide an initializer for a given designator multiple
             * times, the last one wins. Useful for macros, potentially
             * confusing. Clang will warn about this with
             * "-Winitializer-overrides". */
            .b = 37.0,

            /* If you only give some initializers designations, then it just
             * continues in sequence: */
            .c = { [3] = 10, 20, [5] = 30, 16 },

            /* You can also mix and match designator "levels": */
            .c[7] = 37
        };

        assert(foo.u.a == 1);
        assert(foo.a == -2);
        assert(foo.b == 37.0F);
        assert(foo.c[0] == 0);
        assert(foo.c[1] == 0);
        assert(foo.c[2] == 0);
        assert(foo.c[3] == 10);
        assert(foo.c[4] == 20);
        assert(foo.c[5] == 30);
        assert(foo.c[6] == 16);
        assert(foo.c[7] == 37);
    }
}
