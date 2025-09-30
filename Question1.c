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
void memtest(int *bytes) {
    uint64_t start, end, clock;
    int b = *bytes;
    
    char *lineBuffer = (char*) malloc(b);
    char *lineBufferCopy = (char*) malloc(b);
    char filename[100];
    sprintf(filename, "memtest%dB.csv", b);
    FILE *fp;
    fp = fopen(filename, "w");
    for (int i = 0; i < b; i++) {
        lineBuffer[i] = '1';
    }
    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {
        start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, b);
        end = rdtsc();
        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (end - start);
        printf("%lu ticks to copy %iB\n", (end - start), b);
        fprintf(fp, "%lu\n", (end - start));
    }
    printf("took %lu ticks total \n", clock);
    fclose(fp);
    free(lineBuffer);
    free(lineBufferCopy);
}

int main(int ac, char **av) {
    printf("------------------------------\n");
    int byte_counts[] = {64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 1048576,2097152};
    int i;
    for (i=0;i<sizeof(byte_counts)/sizeof(byte_counts[0]);i++) {
        memtest(&byte_counts[i]);
    }
    
    return 0;
}