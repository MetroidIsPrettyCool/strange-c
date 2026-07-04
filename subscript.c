/* This demo works in all standard C versions.
 *
 * It's a common C party trick to demonstrate that a[b] is not only morally
 * equivalent to (*(a + b)), but actually, _interchangeably_ equivalent, and
 * thus a commutative operation. For example: */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <stddef.h>
#include <string.h>

int main(void) {
    int a[6] = { 0, 1, 2, 3, 4, 5 };

    assert(*(a + 2) == *(2 + a));
    assert(*(a + 2) ==     a[2]);
    assert(*(2 + a) ==     a[2]);
    assert(a[2]     ==     2[a]);
    assert(a[2]     !=     3[a]);
    assert(a[3]     !=     2[a]);
    assert(&(a[2])  ==  &(2[a]));

    /* The relevant section of the standard is $3.3.2.1 for C89/95 (FiPS PUB
     * 160), 6.5.2.1 for C99/C11/C17 (N1256, N1570, N2310), or $6.5.3.2 for C23
     * (N3220). In every revision, it contains the following line:
     *
     * "The definition of the subscript operator [] is that E1[E2] is identical
     * to (*((E1)+(E2)))."
     *
     * Addition is commutative, and so is the subscript operator. */

    /* C2y (as of N3886) rewords the semantics of array subscripting (and of
     * pointer arithmetic in general) pretty dramatically as part of a general
     * UB cleanup push: */

    #if __STDC_VERSION__ > 202311L
        /* First, E1[E2] is only equivalent to *((E1)+(E2)) (no outer parens)
         * when either operand "has pointer type" in C2y. (N3886 $6.5.3.2 para.
         * 2-3).
         *
         * This enables a new dodgy-looking but conformant party trick, by
         * taking into account the new special-casing of adding 0 to a null
         * pointer in $6.5.7 para. 9 (N3886):
         *
         * "If the pointer operand is not null, and the pointer operand and
         * result do not point to elements of the same array object or one past
         * the last element of the array object, the behavior is undefined. If
         * the pointer operand is a null pointer value and the integer operand
         * is nonzero, the behavior is undefined. [footnote: Thus, the
         * expression ptr + N (where ptr is a null pointer value) is defined to
         * result in a null pointer value when N is zero and has undefined
         * behavior otherwise.] [...] If the pointer operand is null or the
         * result points one past the last element of the array object, it shall
         * not be used as the operand of a unary * operator that is evaluated."
         *
         * and the fact that &*(some_complete_type_t*)NULL is now defined by
         * $6.5.4.3 para. 3 (N3886) _and_ defined to not be evaluated:
         *
         * "If the operand is the result of a unary * operator, neither that
         * operator nor the & operator is evaluated and the result is as if both
         * were omitted, except that the constraints on the operators still
         * apply and the result is not an lvalue."
         *
         * to write this: */

        printf(
            "%p %p %p %p\n",
            &((char*)NULL)[0],
            &0[(char*)NULL],
            &((char*)nullptr)[0],
            &0[(char*)nullptr]
        );

        /* Note that we have to explicitly cast to char* because, of all the
         * myriad things that NULL can be defined as, none of them have "pointer
         * to complete object" type. */
    #endif

    #if __STDC_VERSION__ <= 202311L
        /* Second, if neither E1 nor E2 is a pointer, then one of them is
         * required to be an array and the other to be an integer non-negative
         * subscript. The only situation I'm aware of where this could even
         * happen in a real program would be something like this: */

        {
            struct { int x; int a[4]; } foo;
            foo.x = -1;
            foo.a[0] = 0;
            foo.a[1] = 1;
            foo.a[2] = 2;
            foo.a[3] = 3;

            printf("%d\n", foo.a[-1]);
        }

        /* But this was never conforming in the first place. If you're using GCC
         * or Clang, this goes from UB in C <= 23 (which probably works anyway)
         * to a compilation error. */
    #endif

    puts("All passed!");

    return EXIT_SUCCESS;
}
