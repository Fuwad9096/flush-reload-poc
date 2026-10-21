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

    for(;;) {
        sleep(1);
        maccess(addr);
        printf("Accessed a file\n");
    }
    return 0;
}