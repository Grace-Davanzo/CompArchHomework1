#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REPEAT 1000000

static inline void clflush(volatile void *p) {
    asm volatile ("clflush %0" :: "m"(*(volatile char*)p) : "memory");
}

static inline uint64_t rdtsc() {
    unsigned long a, d;
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    return a | ((uint64_t)d << 32);
}

long int rep;
char lineBuffer[64];

void memtest() {
    uint64_t first_start, first_copy, second_start, second_copy, clock;
    
    char *lineBuffer = (char*) malloc(64);
    char *lineBufferCopy = (char*) malloc(64);
    char filename[100];
    sprintf(filename, "q3.csv");
    FILE *fp;
    fp = fopen(filename, "w");
    for (int i = 0; i < 64; i++) {
        lineBuffer[i] = '1';
    }
    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {
        first_start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, 64);
        first_copy = rdtsc();
        clflush(lineBufferCopy);

        second_start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, 64);
        second_copy = rdtsc();
        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (second_copy - first_start);
        
        printf("%lu ticks for first copy\n", (first_copy - first_start));
        printf("%lu ticks for second copy\n", (second_copy - second_start));
        fprintf(fp, "%lu,%lu\n", (first_copy - first_start), (second_copy - second_start));
    }
    printf("took %lu ticks total\n", clock);
    fclose(fp);
    free(lineBuffer);
    free(lineBufferCopy);
}

int main(int ac, char **av) {
    printf("------------------------------\n");
    memtest();
    return 0;
}