#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int *addresses, i = 0, numFrames, pageSize, policy, numAddresses;
    addresses = mmap(NULL, 500000 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    FILE *fd;
    if (argc != 5)
    {
        fprintf(stderr, "incorrect number of args\n");
        return 1;
    }
    if (!(fd = fopen(argv[4], "r")))
    {
        fprintf(stderr, "file not found\n");
        return 1;
    }
    while (fscanf(fd, "%x", &addresses[i]) != EOF)
        i++;
    numAddresses = i;
    numFrames = atoi(argv[1]);
    pageSize = atoi(argv[2]);
    if (strcmp(argv[3], "rand") == 0)
    {
        policy = 1;
    }
    else if (strcmp(argv[3], "fifo") == 0)
    {
        policy = 2;
    }
    else if (strcmp(argv[3], "clock") == 0)
    {
        policy = 3;
    }
    else
    {
        fprintf(stderr, "incorrect page replacement policy\n");
        return 1;
    }
    printf("numFrames = %d, pageSize = %d, policy = %d\n", numFrames, pageSize, policy);
    return 0;
}