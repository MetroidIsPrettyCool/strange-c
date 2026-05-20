/* An example of "reflection" in C using X-Macros and _Generic. Requires C11 or
 * later. */

#include <stdio.h>

#include <stddef.h>

/* List the types we want to be able to create a type-tagged pointer to: */
#define MY_TYPES                                            \
    X( struct, foo,  { int a; int b; } )                    \
    X( struct, bar,  { float c; float d; } )                \
    X( enum,   baz,  { BAZ_1, BAZ_2, BAZ_3 = 30, BAZ_4 } )  \
    X( union,  quux, { int e; float f; } )

/* Declare them:
 *
 * We have to use a variadic macro here because enumerator-lists are parsed as
 * multiple arguments -- "{ BAZ_1", "BAZ_2", ... "BAZ_4 }" */
#define X(type_category, id, ...)               \
    type_category id __VA_ARGS__;

    MY_TYPES
#undef X

/* Define an enum with a variant for each of our types, plus "UNKNOWN" */
#define X(type_category, id, ...) MY_TYPE_##type_category##_##id,
    enum my_types_variant_tags {
        MY_TYPE_UNKNOWN,
        MY_TYPES
    };
#undef X

/* A type-tagged pointer type. */
struct ty_ptr { void* p; enum my_types_variant_tags ty; };

/* Define a function for each type T that either casts a ty_ptr to a pointer to
 * T (if ty_ptr is tagged as a pointer to T) or returns NULL (if the ty_ptr is
 * not tagged as a pointer to T). */
#define X(type_category, id, ...)                               \
    static inline                                               \
    type_category id*                                           \
    ty_ptr_maybe_##type_category##_##id                         \
    (struct ty_ptr ty_ptr)                                      \
    {                                                           \
        if (ty_ptr.ty == MY_TYPE_##type_category##_##id)        \
            return (type_category id*)ty_ptr.p;                 \
        else                                                    \
            return NULL;                                        \
    }

    MY_TYPES
#undef X

/* This will be our definition of X() for the rest of the file -- we only care
 * about using MY_TYPES for the MAKE_TY_PTR() macro. */
#define X(type_category, id, ...)                       \
    type_category id: MY_TYPE_##type_category##_##id,

/* Macro that takes an expression and expands to a ty_ptr compound literal, with
 * a type tag value selected using the _Generic() operator. */
#define MAKE_TY_PTR(x)                                                  \
    (struct ty_ptr) {                                                   \
        .p = &x,                                                        \
        .ty = _Generic((x), MY_TYPES default: MY_TYPE_UNKNOWN)          \
    }

int main(void) {
    struct foo  a = { .a = 1, .b = 2 };
    struct bar  b = { .c = 3.14F, .d = 2.71F };
    enum   baz  c = BAZ_4;
    union  quux d = { .f = -7.0f };

    struct ty_ptr array[4] = {
        MAKE_TY_PTR(d),
        MAKE_TY_PTR(c),
        MAKE_TY_PTR(b),
        MAKE_TY_PTR(a)
    };

    for (size_t i = 0; i < 4; ++i) {
        if (ty_ptr_maybe_struct_foo(array[i])) {
            struct foo foo = *ty_ptr_maybe_struct_foo(array[i]);
            printf("%d, %d\n", foo.a, foo.b);
        }

        else if (ty_ptr_maybe_struct_bar(array[i])) {
            struct bar bar = *ty_ptr_maybe_struct_bar(array[i]);
            printf("%f, %f\n", bar.c, bar.d);
        }

        else if (ty_ptr_maybe_enum_baz(array[i])) {
            enum baz baz = *ty_ptr_maybe_enum_baz(array[i]);
            printf("%d\n", baz);
        }

        else if (ty_ptr_maybe_union_quux(array[i])) {
            union quux quux = *ty_ptr_maybe_union_quux(array[i]);
            /* Reading from the wrong union member is not actually UB, as you
             * might expect, but merely /unspecified/ behavior. See N1570 (or
             * N3220) $6.2.6.1 paragraphs 6 and 7.
             *
             * Still a portability concern, of course, but unlike UB it isn't an
             * erroneous code path. The value of the .e member in this case will
             * be *something* valid. */
            printf("%08X (%f)\n", quux.e, quux.f);
        }
    }
}
