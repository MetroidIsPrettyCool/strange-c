/* Here's an example copied directly from the C23 standard (N3220); $6.7.13.8.3,
 * paragraph 8, showing an interesting "emergent" consequence of the unsequenced
 * attribute not being part of the function prototype -- you can declare that
 * your calls to external functions will treat that function as unsequenced (or
 * reproducible, for that matter)!
 *
 * Requires C23 or later, of course, as well as support for these attributes,
 * and the STDC floating-point pragmas. */

#include <math.h>
#include <fenv.h>

inline double distance (double const x[static 2]) [[reproducible]] {
    #pragma STDC FP_CONTRACT OFF
    #pragma STDC FENV_ROUND  FE_TONEAREST
    // We assert that sqrt will not be called with invalid arguments
    // and the result only depends on the argument value.
    extern typeof(sqrt) [[unsequenced]] sqrt;
    return sqrt(x[0]*x[0] + x[1]*x[1]);
}
