/* This demo requires only C89. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

/* The C preprocessor forbids recursive macros, presumably to prevent infinite
   loops from hanging the compiler. The following will expand to simply
   2*f(): */

void a(void) {
    int f(void);
    #define f() 2*f()

    assert(f() == 8);

    #undef f
}
int f(void) { return 4; }

/* This cannot be worked around with mutually recursive macros. */
void b(void) {
    #define f() 2*g()
    #define g() 2*f()

    /* expands to 2*2*f() */
    assert(f() == 16);

    #undef f
    #undef g
}

/* The preprocessor keeps a notional "stack" of identifiers as it expands
   macros. We expand f, so we put f on the stack. Within that expansion, we
   expand g, which also goes on the stack. Within THAT expansion, we see f
   again, but f is already on the stack. So it's skipped. */

/* The solution is the following: */
int (*g(void))(void) { return f; }
void c(void) {
    #define empty()
    #define eval(x) x
    #define f() 2*(g empty()()())
    #define g() f

    /* expands to 2*(2*(g ()())) */
    assert(f() == 16);
    /* expands to 2*(2*(2*(g ()()))) */
    assert(eval(f()) == 32);
    /* expands to 2*(2*(2*(2*(g ()())))) */
    assert(eval(eval(f())) == 64);

    #undef f
    #undef g
    #undef eval
    #undef empty
}

/* The empty() macro serves only to make the preprocessor stumble a bit. It
   evaluates to nothing, allowing g to attach to the parentheses *after* the
   scanning engine had already popped it off the no-no identifier stack. Every
   every eval() we wrap f() in then triggers another expansion pass, allowing it
   to call itself. */

/* Using token joining, we can construct a rudimentary loop counter: */
void d(void) {
    #define f0(x)
    #define f1(x) g empty()()(0,x)
    #define f2(x) g empty()()(1,x)
    #define f3(x) g empty()()(2,x)
    #define f4(x) g empty()()(3,x)
    #define f5(x) g empty()()(4,x)
    #define f6(x) g empty()()(5,x)

    #define eval(x) eval1(eval1(eval1(eval1(x))))
    #define eval1(x) eval2(eval2(eval2(eval2(x))))
    #define eval2(x) eval3(eval3(eval3(eval3(x))))
    #define eval3(x) x

    #define empty()
    #define f(i,x) x*f##i(x)
    #define g() f

    /* expands to 2* 4 */
    assert(eval(f(0,2) 4) == 8);
    /* expands to 2*2* 4 */
    assert(eval(f(1,2) 4) == 16);
    /* expands to 2*2*2* 4 */
    assert(eval(f(2,2) 4) == 32);
    /* expands to 10*10*10* 4 */
    assert(eval(f(2,10) 4) == 4000);

    #undef f0
    #undef f1
    #undef f2
    #undef f3
    #undef f4
    #undef f5
    #undef f6
    #undef eval
    #undef eval1
    #undef eval2
    #undef eval3
    #undef empty
    #undef f
    #undef g
}

int main(void) {
    a();
    b();
    c();
    d();
    return EXIT_SUCCESS;
}
