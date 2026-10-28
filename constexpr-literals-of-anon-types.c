/* You aren't allowed to declare types in the initializers of what are called
   "underspecified" definitions -- which are any declarations that are either
   constexpr or auto. This includes compound literals, so

   (constexpr struct {int a;}){}

   is illegal. We can get around this (somewhat) by misusing _Generic.

   Requires C23. */

#include <stdio.h>
#include <assert.h>

#define GENSYM() GENSYMA(__LINE__)
#define GENSYMA(...) GENSYMB(__VA_ARGS__)
#define GENSYMB(X) GENSYM##X

#define BRACE(...) {__VA_ARGS__}
#define MK_LIT(M,...) _Generic(0,struct GENSYM()BRACE M:0,int:(constexpr struct GENSYM()){__VA_ARGS__})

int main() {
    int x = MK_LIT((int a,b,c;),1,2,3).b;
    assert(x == 2);
    printf("%d\n", x);

    typeof(MK_LIT((int a,b,c;))) y = MK_LIT((int a,b,c;),4,5,6);
    assert(y.a == 4);
    assert(y.b == 5);
    assert(y.c == 6);
    printf("%d %d %d\n", y.a, y.b, y.c);

    /* note that we can't initialize a constexpr variable with MK_LIT: */
    /* constexpr typeof(MK_LIT((int a,b,c;))) z = MK_LIT((int a,b,c;),3,1,4); */

    /* using it in just the type specifier is okay, though: */
    constexpr typeof(MK_LIT((int a,b,c;))) w = {2,7,1};
    printf("%d %d %d\n", w.a, w.b, w.c);
}
