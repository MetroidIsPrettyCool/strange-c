#ifndef P_LIT_H
#define P_LIT_H

#if __STDC_VERSION__ > 201112L
    #include <stdio.h>

    #define P_LIT(x) printf(_Generic((x),      \
        float: "%fF",                          \
        double: "%f",                          \
        unsigned: "%uU",                       \
        int: "%d",                             \
        unsigned short: "(unsigned short)%hu", \
        short: "(short)%hu",                   \
        unsigned long: "%luUL",                \
        long: "(short)%ldL",                   \
        char: "'%c'",                          \
        default: (void)0                       \
        ),                                     \
        (x))
#elif defined(__cplusplus)
    #include <cstdio>
    #include <type_traits>

    template<typename T>
    void P_LIT(T x)
    {
        if constexpr (std::is_same_v<T, float>)
            std::printf("%fF", x);

        else if constexpr (std::is_same_v<T, double>)
            std::printf("%f", x);

        else if constexpr (std::is_same_v<T, unsigned>)
            std::printf("%uU", x);

        else if constexpr (std::is_same_v<T, int>)
            std::printf("%d", x);

        else if constexpr (std::is_same_v<T, unsigned short>)
            std::printf("(unsigned short)%hu", x);

        else if constexpr (std::is_same_v<T, short>)
            std::printf("(short)%hd", x);

        else if constexpr (std::is_same_v<T, unsigned long>)
            std::printf("%luUL", x);

        else if constexpr (std::is_same_v<T, long>)
            std::printf("%ldL", x);

        else if constexpr (std::is_same_v<T, char>)
            std::printf("'%c'", x);

        else
            static_assert(!sizeof(T), "Unsupported type");
    }
#else
    #include <stdio.h>

    /* Because we don't have access to _Generic before C11, we'll use the
     * nonstandard __builtin_choose_expr() as a substitute that should work
     * regardless of the currently selected standard. This should work in both
     * GCC or Clang. */

    #define P_LIT(x) printf(__builtin_choose_expr(                   \
        __builtin_types_compatible_p(__typeof__(x), float),          \
        "%fF",                                                       \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), double),         \
        "%f",                                                        \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), unsigned),       \
        "%uU",                                                       \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), int),            \
        "%d",                                                        \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), unsigned short), \
        "(unsigned short)%hu",                                       \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), short),          \
        "(short)%hd",                                                \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), unsigned long),  \
        "%luUL",                                                     \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), long),           \
        "%ldL",                                                      \
        __builtin_choose_expr(                                       \
        __builtin_types_compatible_p(__typeof__(x), char),           \
        "'%c'",                                                      \
        (void)0                                                      \
        ))))))))),                                                   \
        (x))
#endif

#endif /* P_LIT_H */
