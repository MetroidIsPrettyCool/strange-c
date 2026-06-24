/* The relationship between function types and the (de)referencing operators has
 * bizarre semantics. This demo requires C23 for the typeof() operator. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <fenv.h>
#include <math.h>

void foo() {
    puts("Hello!");
}

void bar() {
    puts("Goodbye?");
}

int main() {

    /* 1. &func, func, and *func are all equivalent. */

    assert(*foo == foo);
    assert(&foo == foo);
    assert(*foo == &foo);

    /* 2. You can dereference a function type as much as you want, and it's
     *    still equivalent. */

    assert(*********************************************************foo == foo);

    /* 3. Even behind a function pointer. */

    typeof(&foo) baz = &foo;
    (********************************************************************baz)();

    /* 4. This does not apply to &. &&foo (and beyond) is UB in standard C. */

    /* (&&bar)(); */

    /* 5. *foo and foo have compatible types, so we can't write this: */

    /* puts(_Generic(foo,typeof(*foo):"1",typeof(foo):"2a")); */

    /* 6. &foo and foo have incompatible types, so we CAN write this: */

    puts(_Generic(foo,typeof(&foo):"1",typeof(foo):"2"));
    /* or this: */
    puts(_Generic(foo,typeof_unqual(&foo):"1",typeof_unqual(foo):"2"));

    /* 7. However, in a highly amusing twist, the implicit lvalue conversion
     *    performed by _Generic() means &foo, foo, *foo, **foo, etc. will never
     *    match typeof(foo) or typeof(*foo) in such an expression: */

    puts(_Generic(&foo,typeof(&foo):"1",typeof(*foo):"2"));
    puts(_Generic(foo,typeof(&foo):"1",typeof(*foo):"2"));
    puts(_Generic(*foo,typeof(&foo):"1",typeof(*foo):"2"));
    puts(_Generic(**********************foo,typeof(&foo):"1",typeof(*foo):"2"));
    puts(_Generic(&foo,typeof(&foo):"1",typeof(foo):"2"));
    puts(_Generic(foo,typeof(&foo):"1",typeof(foo):"2"));
    puts(_Generic(*foo,typeof(&foo):"1",typeof(foo):"2"));
    puts(_Generic(***********************foo,typeof(&foo):"1",typeof(foo):"2"));
    puts(_Generic(&foo,typeof(*foo):"2",typeof(&foo):"1"));
    puts(_Generic(foo,typeof(*foo):"2",typeof(&foo):"1"));
    puts(_Generic(*foo,typeof(*foo):"2",typeof(&foo):"1"));
    puts(_Generic(**********************foo,typeof(*foo):"2",typeof(&foo):"1"));
    puts(_Generic(&foo,typeof(foo):"2",typeof(&foo):"1"));
    puts(_Generic(foo,typeof(foo):"2",typeof(&foo):"1"));
    puts(_Generic(*foo,typeof(foo):"2",typeof(&foo):"1"));
    puts(_Generic(***********************foo,typeof(foo):"2",typeof(&foo):"1"));

    /* In C2y we'll be able to get around this with _Generic(typeof(),...), but
     * we aren't there just yet. */
    #if __STDC_VERSION__ > 202311L
        puts(_Generic(typeof(*foo),typeof(&foo):"1",typeof(foo):"2"));
        puts(_Generic(typeof(foo), typeof(&foo):"1",typeof(foo):"2"));
        puts(_Generic(typeof(&foo),typeof(&foo):"1",typeof(foo):"2"));
    #endif

    /* 8. That the expression before the parentheses lvalue-converts to a
     *    function pointer is all that function calls are actually looking for,
     *    by the way. You can be pair this with the comma and ternary operators
     *    for some remarkably obscure constructions: */

    typeof(sin)* a = nullptr, * b = cos, * tmp;
    double f;

    f = (a?a:b)(1.5);
    printf("%f\n", f);
    a = sin;
    f = (tmp=a,a=b,b=tmp)(1.5);
    printf("%f\n", f);

    puts("Passed!");
}
