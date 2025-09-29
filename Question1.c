#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REPEAT 1000000
inline void clflush(volatile void *p) {
    asm volatile ("clflush(%0)" :: "r"(p));
}

inline uint64_t rdtsc() {
    unsigned long a, d;
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    return a | ((uint64_t)d << 32);
}

char lineBuffer[64];
long int rep;

inline void memtest(int *bytes) {
    uint64_t start, end, clock;
    char *lineBuffer = (char*) malloc(64);
    char *lineBufferCopy = (char*) malloc(64);
    char *filename;
    sprintf(filename, "memtest%dB.csv", bytes);
    FILE *fp;
    fp = fopen(filename, 'w');
    for (int i = 0; i < 64; i++) {
        lineBuffer[i] = '1';
    }
    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {
        start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, 64);
        end = rdtsc();
        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (end - start);
        printf("%llu ticks to copy 64B\n", (end - start));
        fprintf(fp, "%llu\n", (end - start));
    }
    printf("took %llu ticks total \n", clock);
}

int main(int ac, char **av) {
    printf("------------------------------\n");
    memtest(64);
    return 0;
}