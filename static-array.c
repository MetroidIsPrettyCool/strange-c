/* A demonstration of the optimization benefits of the oft-overlooked C99 array
 * minimum size static keyword.
 *
 * At time of writing, this is the only reliable technique I've found to get an
 * inlined, vectorized string comparison in standard C on x64. It only works in
 * clang (again, at time of writing) and only with -O2 or greater. But it does
 * work.
 *
 * Requires C99 or later. */

#include <stdio.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

enum example_kind {
    EXAMPLE1,
    EXAMPLE2,
};

#define EXAMPLE1_S "This is an example string"
#define EXAMPLE2_S "And this is another example string"

static inline
bool s_is_example1(char const s[const restrict static sizeof(EXAMPLE1_S)])
{
    return strncmp(s, EXAMPLE1_S, sizeof(EXAMPLE1_S)) == 0;
}

static inline
bool s_is_example2(char const s[const restrict static sizeof(EXAMPLE2_S)])
{
    return strncmp(s, EXAMPLE2_S, sizeof(EXAMPLE2_S)) == 0;
}

static inline
int parse_example1_or_example2(char const * const restrict s)
{
    size_t size = strlen(s) + 1;
    if      (size == sizeof(EXAMPLE1_S) && s_is_example1(s)) return EXAMPLE1;
    else if (size == sizeof(EXAMPLE2_S) && s_is_example2(s)) return EXAMPLE2;
    else                                                     return -1;
}

#define BUFLEN 255

int main(void) {
    char buf[BUFLEN + 1] = { 0 };
    {
        size_t i;
        for (i = 0;
             i < BUFLEN
                 && (buf[i] = getchar()) != EOF
                 && buf[i] != '\n';
             ++i);
        buf[i] = '\0';
    }

    printf("%d\n", parse_example1_or_example2(buf));
}
