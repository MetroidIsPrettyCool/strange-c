/* I'm pretty sure this is legal C23. Demonstration of how _Generic can be used
   to take the size of a string constant, then instantiate a variable with a
   type derived from that size. A possibly useful technique for constructor
   macros that need to select a fixed size composite type from its
   parameters.

   Clang, as of version 23.1.1, doesn't compile this quite right yet because
   they're missing support for constexpr literals (N3200 §6.5.3.6 ¶7). You can
   get around this nonconformance if you change the definition of MAKE_SIZED_STR
   to say "const" instead. */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>

#ifdef __has_c_attribute
    #if __has_c_attribute(gnu::nonstring)
        #define ATTR_NOT_NUL_TERMINATED gnu::nonstring
    #else
        #define ATTR_NOT_NUL_TERMINATED
    #endif
#else
    #define ATTR_NOT_NUL_TERMINATED
#endif

struct strstruct1 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[1]; };
struct strstruct2 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[2]; };
struct strstruct3 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[3]; };
struct strstruct4 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[4]; };
struct strstruct5 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[5]; };
struct strstruct6 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[6]; };
struct strstruct7 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[7]; };
struct strstruct8 { int n; [[ATTR_NOT_NUL_TERMINATED]] char a[8]; };

#define SEL_TYPE_FROM_LEN(X) typeof(_Generic(                      \
                             &(char[]){X},                         \
                             char(*)[2]: ((struct strstruct1){}),  \
                             char(*)[3]: ((struct strstruct2){}),  \
                             char(*)[4]: ((struct strstruct3){}),  \
                             char(*)[5]: ((struct strstruct4){}),  \
                             char(*)[6]: ((struct strstruct5){}),  \
                             char(*)[7]: ((struct strstruct6){}),  \
                             char(*)[8]: ((struct strstruct7){}),  \
                             char(*)[9]: ((struct strstruct8){})))

#define MAKE_SIZED_STR(X) ((constexpr SEL_TYPE_FROM_LEN(X)){(int)(sizeof X-1),X})

constexpr auto CES = MAKE_SIZED_STR("abc");

int main() {
    auto s = MAKE_SIZED_STR("neat!");

    printf("%.*s\n", s.n, s.a);
    printf("%.*s\n", CES.n, CES.a);

    memcpy(&s, &CES, sizeof CES);
    printf("%.*s\n", s.n, s.a);
}
