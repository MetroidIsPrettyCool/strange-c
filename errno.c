/* This demo requires nothing, it's just comments. */

/* Suppose we want to write a wrapper around some standard library functions
   that return a struct like the following: */

/* struct my_result { int foo, bar; }; */

/* The two members of this struct will be used to store the return value and the
   value of errno, respectively.

   What should we call them? */

/* What about this? */

/* struct my_result { int ok, errno; }; */

/* Nope! Prior to C11, implementations could choose to implement errno as either
   "an identifier with external linkage", or an object-like macro. (As of C11,
   errno is always an object-like macro.)

   If errno is an object-like macro, then anyone who includes our my_result
   struct definition after errno.h will get a mangled struct definition like
   this:

   struct my_result { int ok, (*__errno_location ()); };

   which declares my_result as a struct with two members: the int ok, and the
   function returning a pointer-to-int __errno_location. Syntactically
   comprehensible, but function members are a constraint violation. And that's
   clearly not what we wanted anyway.

   Code like the following (C23) also won't work: */

/* #include <uchar.h> */
/* struct my_result my_mbrtoc8_wrapper( */
/*     char8_t * restrict pc8, */
/*     const char * restrict s, */
/*     int n, */
/*     mbstate_t * restrict ps */
/* ) { */
/*     struct my_result result = {0, 0}; */
/*     size_t tmp = mbrtoc8(pc8, s, n, ps); */
/*     if (tmp == (size_t)(-1))      result.errno = errno; */
/*     else if (tmp == (size_t)(-2)) result.ok = -2; */
/*     else if (tmp == (size_t)(-3)) result.ok = -3; */
/*     else                          result.ok = (int)tmp; */
/*     return result; */
/* } */

/* Because, again, errno may be an object-like macro, and it doesn't matter that
   struct members are their own namespace. result.errno = errno can pre-process
   to the following:

   result.(*__errno_location ()) = (*__errno_location ());

   which is incomprehensible. */

/* Well, okay, what if we just don't include errno directly? Imagine we create a
   translation unit with the following content:

   // errno_wrapper.c
   #include <errno.h>

   int get_errno() {
       return errno;
   }

   and then everywhere else in our project, we just use get_errno instead of
   including <errno.h>. Now we're safe from object-like macros! */

/* #include <uchar.h> */
/* extern int get_errno(); */

/* struct my_result my_mbrtoc8_wrapper( */
/*     char8_t * restrict pc8, */
/*     const char * restrict s, */
/*     int n, */
/*     mbstate_t * restrict ps */
/* ) { */
/*     struct my_result result = {0, 0}; */
/*     size_t tmp = mbrtoc8(pc8, s, n, ps); */
/*     if (tmp == (size_t)(-1))      result.errno = get_errno(); */
/*     else if (tmp == (size_t)(-2)) result.ok = -2; */
/*     else if (tmp == (size_t)(-3)) result.ok = -3; */
/*     else                          result.ok = (int)tmp; */
/*     return result; */
/* } */

/* Nope.

   "If a macro definition is suppressed to access an actual object, or a program
   defines an identifier with the name errno, the behavior is undefined." (N3220
   §7.5 ¶2)

   The scope of this sentence is...vague. Does it apply to ANY C program, or
   just programs that include errno.h?

   I dunno. But we're in trouble either way: our program definitely includes
   errno.h, even if just in the one translation unit. */

/* errno sucks. */
