/* This demo requires C99 for reasons unrelated to its core point; C89 just
   requires some syntactic noise that obscures the subject. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

int main(void) {
    /* One of the more infamous ambiguities in the C grammar is the
       following: */

    /* x * y; */

    /* If x is a type, then this is a declaration of y as a pointer to x: */
    {
        typedef int x;

        x * y;

        y = &(int){0};

        printf("abcdef\n%n", y);
        printf("%d\n", *y);
        assert(*y == 7);

    }

    /* But if x is an object, then this is a (useless) multiplication: */
    {
        int x = 1, y = 2;

        x * y;

        printf("%d %d\n", x, y);
    }

    /* Most editors will assume it's the former when doing syntax highlighting,
       because without running an entire C compiler, it's impossible to know
       what identifiers have been declared in which namespaces.

       In the same way that SGML cannot be parsed without a DTD (and SGML
       declaration. and maybe LTD.), C cannot be reliably parsed by anything
       other than a C compiler.

       Especially since basically all real-world C code relies on predefined
       symbols known only to the compiler: */

    printf("%s:%d: %s\n", __FILE__, __LINE__, __func__);

    /* No amount of scraping system headers can tell you what value
       __STDC_HOSTED__ or __STDC_MB_MIGHT_NEQ_WC__ will have, or what namespace
       they'll be in. Or __has_builtin. Or __bswap. Or __int128. Or _Fract. Or
       _Accum.

       If you want to support C code written for any compiler ever made, you
       have to memorize the feature set of every compiler ever made. */

    /* There is another similar ambiguity between type names and function
       identifiers. Is (x)(y) a function call or a cast? */

    {
        typedef int x;
        double y = 3.14;
        printf("%d\n", (x)(y));
    }

    {
        int (*x)(int) = abs;
        int y = -23;
        printf("%d\n", (x)(y));
    }

    /* This is arguably worse than the * ambiguity, since cast and call
       expressions occur all the time in regular code, unlike useless
       multiplications, and parenthesizing the name of a function is actually
       the *recommended way* to suppress a definition of a standard library
       function as a function-like macro.

       "Any macro definition of a function can be suppressed locally by
       enclosing the name of the function in parentheses, because the name is
       then not followed by the left parenthesis that indicates expansion of a
       macro function name." (N3220 §7.1.4 ¶1)

                                      ---

       If you didn't know already, yeah, implementors are allowed to implement
       literally *every* standard library function as a function-like macro if
       they want, so long as there's also a regular function definition.

       "Any function declared in a header may be additionally implemented as a
       function-like macro defined in the header" (N3220 §7.1.4 ¶1) */
}
