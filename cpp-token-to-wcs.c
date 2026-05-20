/* The C Preprocessor (CPP) includes two useful but slightly confusing
 * preprocessor macro-only operators: # ($6.10.5.2), and ## (6.10.5.3). Spelled
 * very similar, almost wholly unrelated purposes.
 *
 * # is a unary prefix operator that converts a token to a string literal, for
 * example
 *
 * #define STR(a) char* a = #a
 * STR(foo);
 *
 * will be preprocessed into
 *
 * char* foo = "foo";
 *
 * ## is a binary infix operator that joins two tokens -- a bit like a space
 * character that gets deleted during expansion. For example:
 *
 * #define PREFIX(name) my_prefix_##name
 * void PREFIX(putc)(char c);
 *
 * will be preprocessed into
 *
 * void my_prefix_putc(char c);
 *
 * It follows, therefore, that we can create a macro that converts an identifier
 * to a wide string literal with L###some_id. Requires C89 or later.
 *
 * (You'll probably want C95 or later if you actually intend to do anything
 * serious with wide strings, though.) */

#include <stdio.h>
#include <stdlib.h>

#include <stddef.h>

#define WCSTR(id) L###id

int main(void) {
    wchar_t* ws = WCSTR(foobar);

    {
        char buf[4096];
        if (wcstombs(buf, ws, 4096) == (size_t)(-1)) return EXIT_FAILURE;
        fputs(buf, stdout);
    }

    return EXIT_SUCCESS;
}
