/* Signed integer overflow is undefined by the standard. (N3220 §6.5.7 ¶9), but
 * casting (N3220 §6.5.5 ¶7) and implicit conversions (N3220 §6.3.1.3 ¶2-3) are
 * implementation-defined.
 *
 * Compile the following program with signed integer overflow defined to wrap
 * and run it with the arguments "int" and then "short", and then again with
 * signed integer overflow defined with the same arguments. In Clang and GCC,
 * this can be controlled with the -fwrapv and -ftrapv flags.
 *
 * What happens? Is it what you expected? */

#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <string.h>

unsigned int_demo(signed a, signed b) {
    return a + b;
}

unsigned short short_demo(signed short a, signed short b) {
    return a + b;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || SHRT_MAX > INT_MAX / 2) return EXIT_FAILURE;
    if (!strcmp(argv[1], "int")) {
        printf("%u\n", int_demo(INT_MAX, INT_MAX));
    } else if (!strcmp(argv[1], "short")) {
        printf("%hu\n", short_demo(SHRT_MAX, SHRT_MAX));
    }
    return EXIT_SUCCESS;
}

/* Assuming you're on a platform where shorts are smaller than ints (which you
 * almost certainly are), then this should have trapped (as expected) when you
 * tried "int", but _worked_ when when you tried "short".
 *
 * Huh?
 *
 * The relevant concept here is "integer promotion", N3220 §6.3.1.1. int can
 * represent all the values of short (definitionally), so operations on shorts
 * will always implicitly convert their operands to ints first. You straight-up
 * can't perform arithmetic on shorts in standard C, which is amusing semantic
 * gap for a "high-level assembler". If SHRT_MAX * 2 is representable in an int,
 * then overflow _never happens_. We're firmly in implementation-defined
 * territory.
 *
 * There is one additional wrinkle you might have noticed: why are our demo
 * functions returning unsigned types? Did you lie when you said implicit
 * conversions were well-defined?
 *
 * No, not this time. The wrinkle is §6.3.1.3 ¶3. Here's the paragraph in full:
 * "Otherwise, the new type is signed and the value cannot be represented in it;
 * either the result is implementation-defined or an implementation-defined
 * signal is raised."
 *
 * This differs from both explicit casts, wherein the conversion always removes
 * "any extra range and precision"; and from implicit casts to unsigned
 * integers, which always wrap around modulo N.
 *
 * To avoid the potential unportability of having to register a signal handler
 * or else crash even _without_ overflow, I opted for the better-defined
 * unsigned conversion. You're welcome to modify this demo to use only signed
 * types if you know they won't throw a signal, though. It isn't UB. */
