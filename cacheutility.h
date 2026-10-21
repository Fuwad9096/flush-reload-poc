/*
 * CACHE UTILITY HEADER FILE FOR FLUSH+RELOAD PRIMITIVES
 * 
 * ///Author: Tasmin Hasan Fuwad
 * Date: October, 2026
 * 
 * Improvised by me :)
*/

#ifndef _CACHEUTILITY_H_
#define _CACHEUTILITY_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <x86intrin.h>

#define USE_RDTSC_BEGIN_END 0
#define USE_RDTSCP 1

size_t CACHE_MISS = 150;
uint64_t start = 0, end = 0;

uint64_t rdtsc() {
    uint64_t a,d;
    _mm_lfence();
    #if USE_RDTSCP
        asm volatile("rdtscp" : "=a"(a), "=d"(d) :: "rcx");
    #else
        asm volatile("rdtsc" : "=a"(a), "=d"(d));
    #endif
    a = (d << 32) | a;
    _mm_lfence();
    return a;
}

void maccess(void *p) {asm volatile("movq (%0), %%rax\n" :: "c"(p) : "rax");}

int flush_reload(void *ptr) {
    #if USE_RDTSC_BEGIN_END
        start = rdtsc_begin();
    #else
        start = rdtsc();
    #endif
    maccess(ptr);
    #if USE_RDTSC_BEGIN_END
        end = rdtsc_end();
    #else
        end = rdtsc();
    #endif

    _mm_lfence();
    _mm_clflush(ptr);

    if((end - start) < CACHE_MISS) return 1;

    return 0;
}

int flush_reload_t(void *ptr) {
    #if USE_RDTSC_BEGIN_END
        start = rdtsc_begin();
    #else
        start = rdtsc();
    #endif
    maccess(ptr);
    #if USE_RDTSC_BEGIN_END
        end = rdtsc_end();
    #else
        end = rdtsc();
    #endif

    _mm_lfence();
    _mm_clflush(ptr);

    return (int)(end - start);
}

int reload_t(void* ptr) {
    #if USE_RDTSC_BEGIN_END
        start = rdtsc_begin();
    #else
        start = rdtsc();
    #endif
    maccess(ptr);
    #if USE_RDTSC_BEGIN_END
        end = rdtsc_end();
    #else
        end = rdtsc();
    #endif

    _mm_lfence();
    return (int)(end - start);
}

size_t flush_reload_threshold_detection() {
    size_t reload_time = 0, flush_reload_time = 0, i, count = 1000000;
    size_t dummy[16];
    size_t *ptr = dummy + 8;

    maccess(ptr);

    for(i = 0; i < count; i++) {
        reload_time += reload_t(ptr);
    }
    for(i = 0; i < count; i++) {
        flush_reload_time += flush_reload_t(ptr);
    }

    reload_time /= count;
    flush_reload_time /= count;

    return (flush_reload_time + reload_time * 2)/3;
}

#endif