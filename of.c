/* C has a lot of features that all end with "of", all with slightly different
 * semantics. This demo does not require any particular C version, we discuss
 * them all. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#if __STDC_VERSION__ >= 201112L && __STDC_VERSION__ <= 201710L
    #include <stdalign.h>
#endif

#if __STDC_VERSION__ > 202311L
    #include <stdcountof.h>
#endif

#if __STDC_VERSION__ == 199901L \
    || (__STDC_VERSION__ >= 201112L && !__STDC_NO_VLA__)
    #define HAVE_VLAS 1
#endif

size_t return_sizeof(int array[32]);
size_t return_sizeof2(int (*array)[32]);
                    /* This ↑ is the syntax for declaring pointers to arrays. I
                     * don't like it either. */

#if HAVE_VLAS
    size_t return_sizeof_vla(size_t n, int array[n]);
    size_t return_sizeof_vla2(size_t n, int (*array)[n]);
#endif

int main(void) {

    /* The first "of" is one you're likely familiar with: sizeof. */
    {
        size_t n = sizeof(int);
        printf("sizeof(int) = %zu\n", n);
    }

    /* sizeof has been here since C89. It's is a language-level operator that
     * takes a type or expression and evaluates to the size of that type or
     * expression in bytes -- itself an expression of type size_t. */

    /* Confusingly, sizeof has two forms depending what its argument is. If its
     * a type, you have to use parens, as if it were a function call: */
    {
        struct foo { int a; char b; };
        size_t n = sizeof(struct foo);
        printf("sizeof(struct { int a; char b; }) = %zu\n", n);
    }

    /* If it's an expression, you don't write the parens: */
    {
        int x = 4 + 3;
        size_t n = sizeof x;
        assert(n == sizeof(int));
    }

    /* But you still can anyway, since the expression (E1) is congruent to
     * E1: */
    {
        int x = 4 + 3;
        size_t n = sizeof x;
        size_t m = sizeof(x);
        assert(n == m);
    }

    /* Note that sizeof, as a unary prefix operator, has right-to-left
     * associativity and belongs to the second-lower precedence class: */
    {
        struct foo { int a; } x;
        int y;
        size_t n, m;

        n = sizeof x.a;
        m = sizeof(x.a);
        assert(n == m);

        y = 1;
        n = sizeof y++;
        m = sizeof(y++);
        assert(n == m);

        n = sizeof sizeof x;
        m = sizeof(size_t);
        assert(n == m);

        n = sizeof 23L + 420;
        m = sizeof(23L + 420);
        assert(n != m);
    }

    /* sizeof is the only operator with this syntax quirk. ¯\_(ツ)_/¯ Certain
     * style guides will argue for one or the other as a matter of brevity vs.
     * consistency. */

    /* sizeof also has a few other special quirks regarding arrays. The size of
       an array type is product of the size of the element type with the count
       of elements in the array, for example: */
    {
        int foo[32] = { 0 };
        size_t q = sizeof foo;
        assert(q == sizeof(int) * 32);
    }

    /* This can be used to create a macro for getting the count of elements in
     * an array: */
    {
        #if __STDC_VERSION__ >= 199901L
            #define COUNTOF(...) (sizeof(__VA_ARGS__) / sizeof(*(__VA_ARGS__)))
        #else
            #define COUNTOF(X) (sizeof(X) / sizeof(*(X)))
        #endif

        int foo[32] = { 0 };
        assert(sizeof(foo) == 32 * sizeof(int));
        assert(COUNTOF(foo) == 32);
    }

    /* In C99 and later we can also use sizeof to get the size of VLAs (at
     * runtime!): */
    #if HAVE_VLAS
        {
            size_t n = 32;
            int foo[n];
            n = sizeof foo;     /* note it doesn't matter if we reassign n */
            assert(n == 32 * sizeof(int));

            /* Because the size of foo is determined at runtime, we can't use
             * sizeof foo in contexts where a (integer) constant expression is
             * required, such as in C11 static_asserts: */

            /* static_assert(sizeof foo == 32 * sizeof(int), "fail!"); */
        }
    #endif

    /* Note however that this does NOT work across function boundaries. Array
     * parameters are "adjusted" to pointer type -- the only such case of "type
     * adjustment" (as opposed to conversion) in the standard. A real pain! */
    {
        int array[32];
        assert(return_sizeof(array) == sizeof(int*));

        #if HAVE_VLAS
            size_t n = 64;
            int vla[n];
            assert(return_sizeof_vla(n, vla) == sizeof(int*));
        #endif
    }

    /* We can work around this by passing pointers to the arrays instead: */
    {
        int array[32];
        assert(return_sizeof2(&array) == sizeof array);

        #if HAVE_VLAS
            size_t n = 64;
            int vla[n];
            assert(return_sizeof_vla2(n, &vla) == sizeof vla);
        #endif
    }
    /* Because who doesn't love inane indirection for indirection's sake? */


    /* The next "of" is also from C89, but its significantly more obscure. It's
     * offsetof! */

    /* offsetof is a macro, unlike sizeof, defined in <stddef.h>. It takes two
     * macro arguments: a type and a member-designator; and it expands to a
     * size_t expression containing the offset of that member from the beginning
     * of an object of said type. */
    {
        struct foo {
            int a;
            unsigned char b;
        } bar;
        unsigned char* ptr;

        assert(offsetof(struct foo, a) == 0);

        bar.a = 0x00000000;
        bar.b = 0x32;
        ptr = (unsigned char*)&bar;

        assert(ptr[offsetof(struct foo, b)] == 0x32);
    }

    /* This has been used to create a naïve sort of "alignment of" macro in many
     * older codebases: */
    {
        #if __STDC_VERSION__ <= 201710L
            #define ALIGNOF(X) offsetof(struct{char a; X b;}, b)
        #endif
    }
    /* Whether this actually works is implementation-defined -- implementors can
     * insert padding for arbitrary reasons, not simply to satisfy alignment
     * requirements -- and also UB as of C23, with the addition of the
     * requirement that "if the specified type name contains a comma not between
     * matching parentheses [...] the behavior is undefined."
     *
     * I do not recommend you use this macro. */


    /* Instead, consider using alignof, introduced in C11: */

    /* As is typical for new keywords, alignof was initially spelled "_Alignof"
     * instead, and users were required to include the <stdalign.h> header to
     * #define alignof _Alignof. As of C23, alignof is now a keyword in its own
     * right, <stdalign.h> is an empty header, and _Alignof permitted as a
     * legacy spelling. */
    {
        #if __STDC_VERSION__ >= 201112L
            static_assert(
                _Alignof(unsigned char) == _Alignof(signed char),
                "fail!"
            );
        #endif
    }

    /* Like sizeof and offsetof, alignof evaluates to a size_t. */
    {
        #if __STDC_VERSION__ >= 201112L
            size_t x = alignof(unsigned char);
            (void)x;
        #endif
    }

    /* Also like sizeof, character types are guaranteed to be the
     * smallest/weakest, although alignof(char) is NOT guaranteed to be 1 like
     * sizeof(char) is. */
    {
        #if __STDC_VERSION__ >= 201112L
            static_assert(
                alignof(unsigned char) == alignof(signed char),
                "fail!"
            );
        #endif
    }

    /* The maximum (fundamental) alignment is that of the type max_align_t from
     * <stddef.h>: */
    {
        #if __STDC_VERSION__ >= 201112L
            printf("alignof(max_align_t) = %zu\n", alignof(max_align_t));
        #endif
    }
    /* Why <stddef.h> and not <stdalign.h>? I have no idea. WG14 didn't want a
     * repeat of __bool_true_false_are_defined, I guess. */

    /* Some compilers will allow using alignof with expressions with the same
     * syntax as sizeof as an extension. GCC, for example: */
    {
        #if __GNUC__ && __STDC_VERSION__ >= 201112L
            int x = 3;
            size_t n = alignof x;
            assert(n == alignof(int));
        #endif
    }
    /* Unfortunately even with -pedantic, GCC doesn't warn that this an
     * extension. Clang does, though, with -Wgnu-alignof-expression. */


    /* The portable alternative for this comes to us in C23 with our next two
     * "of"s: typeof and typeof_unqual. */

    /* As the name implies, they evaluate to types of expressions: */
    {
        #if __STDC_VERSION__ >= 202311L
            int x = 3;
            size_t n = alignof(typeof(x));
            assert(n == alignof(int));
        #endif
    }
    /* Unlike previous "of"s, they are not operators. Also, unlike alignof,
     * they were introduced as new keywords without a _Typeof spelling and
     * corresponding header. Heavens knows why. */

    /* They can be used anywhere that a type name can: */
    {
        #if __STDC_VERSION__ >= 202311L
            typeof('xyz') x = 'xyz';
            assert(_Generic(x, int: true, default: false));
        #endif
    }
    /* (That's right. In C, character literals have the type int, and can
     * include arbitrary numbers of characters. For... reasons. The value of
     * such a literal is implementation-defined. In x64 Clang and GCC, it works
     * like a string literal but with backwards endianness.) */

    /* This is very useful for re-declaring functions with certain attributes,
     * for example: */
    {
        #if __STDC_VERSION__ >= 202311L
            extern typeof(sqrt) [[unsequenced]] sqrt;
            printf("sqrt(3.14) = %f\n", sqrt(3.14));
        #endif
    }

    /* As well as for declaring function pointers: */
    {
        #if __STDC_VERSION__ >= 202311L
            typeof(sqrt)* foo = sqrt;
            printf("sqrt(1.0/137.0) = %f\n", (*foo)(1.0/137.0));
        #endif
    }

    /* typeof_unqual behaves the same as typeof, but strips qualifiers: */
    {
        #if __STDC_VERSION__ >= 202311L
            int x = 0;
            int y = 7;
            int* const p = &x;

            typeof(p) q = p;
            typeof_unqual(p) r = p;
            printf("*q = %d, *r = %d\n", *q, *r);

            /* r may be reassigned, q may not. */
            /* q = &y; */
            r = &y;
            printf("*q = %d, *r = %d\n", *q, *r);
        #endif
    }

    /* Both typeof and typeof_unqual may also be used with types instead of
     * expressions, presumably to accommodate preprocessor nonsense: */
    {
        #if __STDC_VERSION__ >= 202311L
            typeof(int) x = 1;
            typeof(typeof(int)) y = 2;
            printf("x = %d, y = %d\n", x, y);
        #endif
    }
    /* Unlike sizeof, the parentheses are NOT optional when taking the type of
     * an expression. */


    /* And that's it as of C23! 5 different "of"s, each with its own weird
     * quirks. */

    /* It's not the end of "of", though. C2y is on the horizon with a sixth
     * "of": countof! */
    {
        #if __STDC_VERSION__ > 202311L
            int array[23] = {};
            static_assert(countof(array) == 23);
        #endif
    }
    /* countof is the operator replacement for our sizeof-based "number of
     * elements in array" macro. Like alignof, the actual keyword being
     * introduced is "_Countof", and the "countof" spelling is behind the
     * stdcountof.h header. */

    /* countof has essentially the same behavior as sizeof, although the operand
     * needs to be of array type. You can elide parentheses when the operand is
     * an expression, you can take the count of type names, and it works at
     * runtime with VLAs: */
    {
        #if __STDC_VERSION__ > 202311L
            int array[64] = {};
            static_assert(countof array == 64);
            static_assert(countof(int[64]) == 64);
            static_assert(countof(typeof(array)) == 64);

            #if HAVE_VLAS
                size_t n = 23;
                int vla[n];

                n = 99;
                assert(countof vla == 23);
            #endif
        #endif
    }

    puts("All asserts passed!");
    return EXIT_SUCCESS;
}


size_t return_sizeof(int array[32]) {
    return sizeof array;
}

size_t return_sizeof2(int (*array)[32]) {
    return sizeof *array;
}

#if HAVE_VLAS
    size_t return_sizeof_vla(size_t n, int array[n]) {
        return sizeof array;
    }

    size_t return_sizeof_vla2(size_t n, int (*array)[n]) {
        return sizeof *array;
    }
#endif
