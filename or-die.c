/* Requires C23 for __VA_OPT__. */
#include <stdio.h>
#include <stdlib.h>

#include <iso646.h>

/* DIE(&optional exit-status error-message &rest unused) */
#define DIE(...) (DIE4##__VA_OPT__(4)(__VA_ARGS__)(__FILE__":"DIE1(__LINE__)":"DIE5##__VA_OPT__(5)(__VA_ARGS__)), exit(DIE3##__VA_OPT__(3)(__VA_ARGS__)), 0)
#define DIE1(X) DIE2(X)
#define DIE2(X) #X
#define DIE3(...) EXIT_FAILURE
#define DIE33(X, ...) X
#define DIE4(...) perror
#define DIE44(_, ...) DIE4##__VA_OPT__(44)(__VA_ARGS__)
#define DIE444(...) fputs
#define DIE5(...)
#define DIE55(_, ...) DIE5##__VA_OPT__(55)(__VA_ARGS__)
#define DIE555(X, ...) ": "X"\n", stderr

#define TRY(...) ((__VA_ARGS__) == 0)
#define TRY_PTR(OUT, ...) ((OUT = (__VA_ARGS__)) != NULL)
#define TRY_IO(STREAM, ...) ((__VA_ARGS__) != EOF && !ferror(STREAM))

#define ʔ(...) || DIE(__VA_ARGS__)

int main() {
    FILE* dev_full;
    (dev_full = fopen("/dev/full", "w")) != NULL ʔ
    (EXIT_FAILURE, "unable to open /dev/full -- are you not on Linux?");

    setvbuf(dev_full, NULL, _IONBF, 0) == 0 ʔ (99);

    fputc('1', dev_full) != EOF or !ferror(dev_full) ʔ ();
}
