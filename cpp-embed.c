/* C23 added an "embed" preprocessor directive, which takes a file (or,
 * technically, a "a source of data accessible from the translation
 * environment") and converts it to a series of comma-separated unsigned
 * character literals; as well as a corresponding __has_embed() conditional
 * inclusion expression.
 *
 * Here's some example of their use.
 *
 * Requires C23 or later. */

#include <stdio.h>

#include <assert.h>

int main() {
    /* Embedding a binary resource with no embed parameters: */
    {
        constexpr char my_str[] = {
            #embed "inc/hello_world.txt"
            , '\0'      /* note the file is not automatically null-terminated */
        };
        fputs(my_str, stdout);
    }

    /* Embedding a binary resource, but using the suffix embed parameter to
     * null-terminate it in only one line: */
    {
        constexpr char my_str[] = {
            #embed "inc/hello_world.txt" suffix(, '\0')
        };
        fputs(my_str, stdout);
    }

    /* Embedding a binary resource using the suffix embed parameter to
     * null-terminate it in only one line, and using the prefix embed parameter
     * to prefix some additional data: */
    {
        constexpr char my_str[] = {
            #embed "inc/hello_world.txt"                            \
                prefix(                                             \
                    'T', 'h', 'i', 's', ' ', 'i', 's', ' ',         \
                    'a', ' ', 'h', 'e', 'l', 'l', 'o', ' ',         \
                    'w', 'o', 'r', 'l', 'd', ' ', '-', '>', ' ',    \
                )                                                   \
                suffix(, '\0')
        };
        fputs(my_str, stdout);
    }

    /* Embedding a binary resource with the if_empty parameter to provide an
     * alternative value if the resource is found but empty -- goodbye_world.txt
     * is such a file: */
    {
        constexpr char my_str[] = {
            #embed "inc/goodbye_world.txt"                              \
                prefix(                                                 \
                    'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a',        \
                    ' ', 'g', 'o', 'o', 'o', 'd', 'b', 'y', 'e',        \
                    ' ', 'w', 'o', 'r', 'l', 'd', ' ', '-', '>', ' ',   \
                )                                                       \
                suffix(, '\0')                                          \
                if_empty("File empty at comptime\n")
        };
        fputs(my_str, stdout);

        /* Note that the prefix and suffix are ignored when the resource is
         * empty: */
        static_assert(sizeof(my_str) == sizeof("File empty at comptime\n"));
        assert(my_str[0] == 'F');
        assert(my_str[1] == 'i');
        assert(my_str[2] == 'l');
        assert(my_str[3] == 'e');
        assert(my_str[sizeof(my_str) - 2] == '\n');
        assert(my_str[sizeof(my_str) - 1] == '\0');
        /* This means that the null terminator on its own line is not
         * semantically identical to the suffix(, '\0') version -- the latter
         * will result in an empty initializer list if the resource is empty.
         * Choose which behavior you want wisely! */
    }

    /* Another way of providing an alternative expansion using the __has_embed()
     * expression, with a file that exists but is empty: */
    {
        #define RESOURCE "inc/goodbye_world.txt"
            constexpr char my_str[] = {
                #if   __has_embed(RESOURCE) == __STDC_EMBED_FOUND__
                    #embed RESOURCE suffix(, '\0')
                #elif __has_embed(RESOURCE) == __STDC_EMBED_EMPTY__
                    "The resource "RESOURCE" was found empty at compile time\n"
                #elif __has_embed(RESOURCE) == __STDC__EMBED_NOT_FOUND__
                    "The resource "RESOURCE" was not found at compile time\n"
                #else
                    #error Unknown result from __has_embed("inc/goodbye_world.txt")
                #endif
            };
            fputs(my_str, stdout);
        #undef RESOURCE
    }

    /* ...with a file that exists and ISN'T empty: */
    {
        #define RESOURCE "inc/hello_world.txt"
            constexpr char my_str[] = {
                #if   __has_embed(RESOURCE) == __STDC_EMBED_FOUND__
                    #embed RESOURCE suffix(, '\0')
                #elif __has_embed(RESOURCE) == __STDC_EMBED_EMPTY__
                    "The resource "RESOURCE" was found empty at compile time\n"
                #elif __has_embed(RESOURCE) == __STDC__EMBED_NOT_FOUND__
                    "The resource "RESOURCE" was not found at compile time\n"
                #else
                    #error Unknown result from __has_embed("inc/hello_world.txt")
                #endif
            };
            fputs(my_str, stdout);
        #undef RESOURCE
    }

    /* ...and with a file that doesn't exist: */
    {
        #define RESOURCE "inc/see_you_later_world.txt"
            constexpr char my_str[] = {
                #if   __has_embed(RESOURCE) == __STDC_EMBED_FOUND__
                    #embed RESOURCE suffix(, '\0')
                #elif __has_embed(RESOURCE) == __STDC_EMBED_EMPTY__
                    "The resource "RESOURCE" was found empty at compile time\n"
                #elif __has_embed(RESOURCE) == __STDC__EMBED_NOT_FOUND__
                    "The resource "RESOURCE" was not found at compile time\n"
                #else
                    #error Unknown result from __has_embed("inc/see_you_later_world.txt")
                #endif
            };
            fputs(my_str, stdout);
        #undef RESOURCE
    }

    /* Here's an example of the limit parameter to only embed up to a given
     * number of bytes from a resource (only works on platforms where
     * /dev/random exists): */
    {
        #define RANDOM_NUMBER_SOURCE </dev/random>
        #if __has_embed(RANDOM_NUMBER_SOURCE) == __STDC_EMBED_FOUND__
            #define RANDOM_DATA_COUNT 256
            constexpr unsigned char random_data[] = {
                #embed RANDOM_NUMBER_SOURCE limit(RANDOM_DATA_COUNT)
            };
            static_assert(sizeof(random_data) == RANDOM_DATA_COUNT);
            for (size_t i = 0; i < RANDOM_DATA_COUNT; i += 16) {
                printf("%02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  "
                       "%02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\n",
                       random_data[i + 0], random_data[i + 1],
                       random_data[i + 2], random_data[i + 3],
                       random_data[i + 4], random_data[i + 5],
                       random_data[i + 6], random_data[i + 7],
                       random_data[i + 8], random_data[i + 9],
                       random_data[i + 10], random_data[i + 11],
                       random_data[i + 12], random_data[i + 13],
                       random_data[i + 14], random_data[i + 15]);
            }
        #endif
    }

    /* __has_embed() expressions can have parameters too, but they don't mean
     * much as of C23: */
    {
        #if __has_embed(                    \
                "inc/hello_world.txt"       \
                 limit(400)                 \
                 suffix(, '\0')             \
                 if_empty("foo")            \
            )                               \
            != __STDC_EMBED_FOUND__

            #error Huh?
        #endif

        #if __has_embed(                    \
                "inc/goodbye_world.txt"     \
                 limit(400)                 \
                 suffix(, '\0')             \
                 if_empty("foo")            \
            )                               \
            != __STDC_EMBED_EMPTY__

            #error Huh?
        #endif

        #if __has_embed(                            \
                "inc/see_you_later_world.txt"       \
                limit(400)                          \
                suffix(, '\0')                      \
                if_empty("foo")                     \
            )                                       \
            != __STDC_EMBED_NOT_FOUND__

            #error Huh?
        #endif
    }

    /* ...except for limit(), which will turn __STDC_EMBED_FOUND__ into
     * __STDC_EMBED_EMPTY__ if the resource width is 0: */
    {
        #if __has_embed(                    \
                "inc/hello_world.txt"       \
                 limit(0)                   \
                 suffix(, '\0')             \
                 if_empty("foo")            \
            )                               \
            != __STDC_EMBED_EMPTY__

            #error Huh?
        #endif

        #if __has_embed(                    \
                "inc/goodbye_world.txt"     \
                 limit(0)                   \
                 suffix(, '\0')             \
                 if_empty("foo")            \
            )                               \
            != __STDC_EMBED_EMPTY__

            #error Huh?
        #endif

        #if __has_embed(                            \
                "inc/see_you_later_world.txt"       \
                limit(0)                            \
                suffix(, '\0')                      \
                if_empty("foo")                     \
            )                                       \
            != __STDC_EMBED_NOT_FOUND__

            #error Huh?
        #endif
    }

    /* Finally, on a similar note, here's an silly example of using limit(0) to
     * deliberately trigger the is_empty parameter: */
    {
        #ifndef SOME_COMPILE_TIME_SWITCH
            #define SOME_COMPILE_TIME_SWITCH 0
        #else
            #undef  SOME_COMPILE_TIME_SWITCH
            #define SOME_COMPILE_TIME_SWITCH 1
        #endif

        const char my_str[] = {
            #embed "inc/capital_letter_s.txt"                               \
                limit(SOME_COMPILE_TIME_SWITCH)                             \
                if_empty("SOME_COMPILE_TIME_SWITCH was not set")            \
                suffix(                                                     \
                    ,                                                       \
                    'O', 'M', 'E', '_', 'C', 'O', 'M', 'P', 'I', 'L', 'E',  \
                    '_', 'T', 'I', 'M', 'E', '_', 'S', 'W', 'I', 'T', 'C',  \
                    'H', ' ', 'w', 'a', 's', ' ', 's', 'e', 't', '\0'       \
                )
        };
        fputs(my_str, stdout);
    }
}
