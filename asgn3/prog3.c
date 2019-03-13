#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
void placeInTable(int address, int *pageTable, int *physicalMemory, int pageSize, int numPages, int nBitsFrame, int numFrames);
int findEmptyFrame(int *physicalMemory, int numFrames);
int main(int argc, char *argv[])
{
    int *addresses, *pageTable, *physicalMemory;
    int j = 0, i = 0, numFrames, pageSize, policy, numAddresses, numPages, nBitsFrame;
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
    addresses = mmap(NULL, 500000 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    while (fscanf(fd, "%x", &addresses[i]) != EOF)
        i++;
    fclose(fd);

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
    numPages = min((int)(pow(2, 22) / pageSize), (int)pow(2, 16)); // each page table entry contains the frame number, the valid bit, and the recent bit
    pageTable = mmap(NULL, numPages * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    physicalMemory = mmap(NULL, numFrames * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (i = 0; i < numFrames; i++)
        physicalMemory[i] = -1;
    i = 1;
    j = 0;
    while (i < numFrames)
    {
        i = i * 2;
        j++;
    }
    nBitsFrame = j;
    printf("bits needed to represent frames = %d\n", nBitsFrame);
    //printf("setRecent %d\n", setRecent);
    //printf("setvalid %d\n", setValid);
    for (i = 0; i < numAddresses; i++)
    {
        placeInTable(addresses[i], pageTable, physicalMemory, pageSize, numPages, nBitsFrame, numFrames);
    }
    printf("numFrames = %d, pageSize = %d, policy = %d, pages = %d\n", numFrames, pageSize, policy, numPages);
    return 0;
}
void placeInTable(int address, int *pageTable, int *physicalMemory, int pageSize, int numPages, int nBitsFrame, int numFrames)
{
    // generate bitmasks
    int setRecent = 1 << nBitsFrame;
    int setValid = setRecent << 1;
    int pageTableIndex = (address / pageSize) % numPages;
    printf("page table index = %d\n", pageTableIndex);
    // check validity of page
    if (pageTable[pageTableIndex] >> (nBitsFrame + 1) == 1)
    {
        // valid, so check frame and then recent bit
    }
    else
    {
        int entry = 0, emptyFrame;
        entry = entry | setValid;
        emptyFrame = findEmptyFrame(physicalMemory, numFrames);
        if (emptyFrame >= 0)
        {
            // found empty frame
            entry = entry | emptyFrame;
            physicalMemory[emptyFrame]=address;
        }
        else
        {
            // replacement
        }
        printf("table entry is %d\n", entry);
        // invalid, so place here
        // check if there is a free frame, place
        // address in the free frame and update
        // page table
        // else swap
    }
}
int findEmptyFrame(int *physicalMemory, int numFrames)
{
    for (int i = 0; i < numFrames; i++)
    {
        if (physicalMemory[i] == -1)
        {
            return i;
        }
    }
    return -1;
}