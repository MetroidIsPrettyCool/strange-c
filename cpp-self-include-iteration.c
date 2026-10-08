/* Demonstration of a preprocessor iteration loop via recursive self-inclusion.
   The generic selection in our main function is expanded to have associations
   of the form "_BitInt(x): x" for all x > 1 and <= min(BITINT_MAXWIDTH, 65535).

   Thus, for GCC and any other compilers with values of BITINT_MAXWIDTH equal to
   or less than 65535, there is not a single signed bit-precise type you can put
   in this _Generic that won't be 1) matched to a non-default association, and
   2) distinguished from every other BitInt type. And you won't get compiler
   errors from referencing _BitInt types too big for your vendor, either.

   The downside is that compiling a generic selection with sixty thousand
   associations takes GCC about nine seconds to preprocess and a minute to
   compile on my machine. Don't worry if you think you've hung, it's just gonna
   be a sec.

   Maybe don't use an LSP with this one.

   This demo requires C23 for _BitInt and binary literals, although this
   recursion principle should apply to earlier versions too. */

#ifndef FOO_I
    #define FOO_I 0
    #define FOO_F(A,B,C,D,E,F,G,H) FOO_G(A,B,C,D,E,F,G,H)
    #define FOO_G(A,B,C,D,E,F,G,H) 0b##A##B##C##D##E##F##G##H

    #include <stdio.h>

    #include <limits.h>

    int main(void) {
        printf("%u\n",
               _Generic(
                   200wb
                   #include __FILE__
               )
        );
    }
#elif FOO_I == 8
    #if FOO_F(FOO_N0,FOO_N1,FOO_N2,FOO_N3,FOO_N4,FOO_N5,FOO_N6,FOO_N7) > 1 \
        && FOO_F(FOO_N0,FOO_N1,FOO_N2,FOO_N3,FOO_N4,FOO_N5,FOO_N6,FOO_N7) <= BITINT_MAXWIDTH
,_BitInt(FOO_F(FOO_N0,FOO_N1,FOO_N2,FOO_N3,FOO_N4,FOO_N5,FOO_N6,FOO_N7)):
FOO_F(FOO_N0,FOO_N1,FOO_N2,FOO_N3,FOO_N4,FOO_N5,FOO_N6,FOO_N7)
    #endif
#elif FOO_I == 7
    #undef FOO_I
    #define FOO_I 8
    #define FOO_N7 00
    #include __FILE__
    #undef FOO_N7
    #define FOO_N7 01
    #include __FILE__
    #undef FOO_N7
    #define FOO_N7 10
    #include __FILE__
    #undef FOO_N7
    #define FOO_N7 11
    #include __FILE__
    #undef FOO_N7
    #undef FOO_I
    #define FOO_I 7
#elif FOO_I == 6
    #undef FOO_I
    #define FOO_I 7
    #define FOO_N6 00
    #include __FILE__
    #undef FOO_N6
    #define FOO_N6 01
    #include __FILE__
    #undef FOO_N6
    #define FOO_N6 10
    #include __FILE__
    #undef FOO_N6
    #define FOO_N6 11
    #include __FILE__
    #undef FOO_N6
    #undef FOO_I
    #define FOO_I 6
#elif FOO_I == 5
    #undef FOO_I
    #define FOO_I 6
    #define FOO_N5 00
    #include __FILE__
    #undef FOO_N5
    #define FOO_N5 01
    #include __FILE__
    #undef FOO_N5
    #define FOO_N5 10
    #include __FILE__
    #undef FOO_N5
    #define FOO_N5 11
    #include __FILE__
    #undef FOO_N5
    #undef FOO_I
    #define FOO_I 5
#elif FOO_I == 4
    #undef FOO_I
    #define FOO_I 5
    #define FOO_N4 00
    #include __FILE__
    #undef FOO_N4
    #define FOO_N4 01
    #include __FILE__
    #undef FOO_N4
    #define FOO_N4 10
    #include __FILE__
    #undef FOO_N4
    #define FOO_N4 11
    #include __FILE__
    #undef FOO_N4
    #undef FOO_I
    #define FOO_I 4
#elif FOO_I == 3
    #undef FOO_I
    #define FOO_I 4
    #define FOO_N3 00
    #include __FILE__
    #undef FOO_N3
    #define FOO_N3 01
    #include __FILE__
    #undef FOO_N3
    #define FOO_N3 10
    #include __FILE__
    #undef FOO_N3
    #define FOO_N3 11
    #include __FILE__
    #undef FOO_N3
    #undef FOO_I
    #define FOO_I 3
#elif FOO_I == 2
    #undef FOO_I
    #define FOO_I 3
    #define FOO_N2 00
    #include __FILE__
    #undef FOO_N2
    #define FOO_N2 01
    #include __FILE__
    #undef FOO_N2
    #define FOO_N2 10
    #include __FILE__
    #undef FOO_N2
    #define FOO_N2 11
    #include __FILE__
    #undef FOO_N2
    #undef FOO_I
    #define FOO_I 2
#elif FOO_I == 1
    #undef FOO_I
    #define FOO_I 2
    #define FOO_N1 00
    #include __FILE__
    #undef FOO_N1
    #define FOO_N1 01
    #include __FILE__
    #undef FOO_N1
    #define FOO_N1 10
    #include __FILE__
    #undef FOO_N1
    #define FOO_N1 11
    #include __FILE__
    #undef FOO_N1
    #undef FOO_I
    #define FOO_I 1
#elif defined(FOO_I) && FOO_I == 0
    #undef FOO_I
    #define FOO_I 1
    #define FOO_N0 00
    #include __FILE__
    #undef FOO_N0
    #define FOO_N0 01
    #include __FILE__
    #undef FOO_N0
    #define FOO_N0 10
    #include __FILE__
    #undef FOO_N0
    #define FOO_N0 11
    #include __FILE__
    #undef FOO_N0
    #undef FOO_I
    #define FOO_I 0
#endif
