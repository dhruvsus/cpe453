#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>
#include "prog3.h"

// global variables, for which I will burn in hell
ptEntry *pageTable;
int *physicalMemory, *frameBuffer, pageFaults, requests, swaps, bufferIdx = 0, clockHand = 0;
unsigned rval;

int main(int argc, char *argv[])
{
    int *addresses;
    int j = 0, i = 0, numFrames, pageSize, policy, numAddresses, numPages, frameBits;
    pageFaults = 0;
    swaps = 0;
    requests = 0;
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
    frameBuffer = (int *)calloc(numFrames, sizeof(int));
    if (strcmp(argv[3], "rand") == 0)
    {
        policy = 1;
        srand48(1);
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
    pageTable = (ptEntry *)calloc(numPages, sizeof(ptEntry));
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
    frameBits = j;
    //printf("bits needed to represent frames = %d\n", frameBits);
    for (i = 0; i < numAddresses; i++)
    {
        placeInTable(addresses[i], pageSize, numPages, frameBits, numFrames, policy);
    }
    //printf("numFrames = %d, pageSize = %d, policy = %d, pages = %d\n", numFrames, pageSize, policy, numPages);
    printf("%d pagefaults %d requests %d swaps\n", pageFaults, requests, swaps);
    return 0;
}
void placeInTable(int address, int pageSize, int numPages, int frameBits, int numFrames, int policy)
{
    requests++;
    int emptyFrame = -1;
    int pIndex = (address / pageSize) % numPages;
    //printf("page table index = %d\n", pIndex);
    // if page valid, then there is a frame allocated to it
    // we can check that frame, and utilize it if it refers to a different reference
    if (pageTable[pIndex].valid == 1)
    {
        //printf("valid\n");
        if (pageTable[pIndex].address == address)
        {
            //printf("hit\n");
            pageTable[pIndex].recent = 1;
        }
        else
        {
            pageFaults++;
            swaps++;
            pageTable[pIndex].address = address;
            physicalMemory[pageTable[pIndex].frame] = address;
            frameBuffer[bufferIdx] = pageTable[pIndex].frame;
            bufferIdx = (bufferIdx + 1) % numFrames;
        }
    }
    // page is not valid, so try to find an empty frame
    else
    {
        //printf("invalid\n");
        emptyFrame = findEmptyFrame(numFrames);
        if (emptyFrame < 0)
        {
            //printf("physical memory full\n");
            // do a swap
            swap(policy, address, pIndex, numFrames, pageSize);
            pageFaults++;
            swaps++;
        }
        else
        {
            //printf("found empty frame\n");
            pageFaults++;
            pageTable[pIndex].address = address;
            pageTable[pIndex].frame = emptyFrame;
            pageTable[pIndex].valid = 1;
            physicalMemory[emptyFrame] = address;
            frameBuffer[bufferIdx] = emptyFrame;
            bufferIdx = (bufferIdx + 1) % numFrames;
        }
    }
}
int findEmptyFrame(int numFrames)
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
void swap(int policy, int address, int pIndex, int numFrames, int pageSize)
{
    int swapFrameAddress;
    int swapPIndex;
    // rand
    if (policy == 1)
    {
        rval = lrand48() % numFrames;
    }
    // fifo
    if (policy == 2)
    {
        rval = frameBuffer[bufferIdx];
    }
    // clock
    if (policy == 3)
    {
        while (pageTable[physicalMemory[clockHand] / pageSize].recent != 0)
        {
            pageTable[physicalMemory[clockHand] / pageSize].recent = 0;
            clockHand = (clockHand + 1) % numFrames;
        }
        rval = clockHand;
    }
    swapFrameAddress = physicalMemory[rval];
    swapPIndex = swapFrameAddress / pageSize;
    pageTable[swapPIndex].address = 0;
    pageTable[swapPIndex].valid = 0;
    pageTable[swapPIndex].frame = 0;
    pageTable[swapPIndex].recent = 0;
    pageTable[pIndex].address = address;
    pageTable[pIndex].valid = 1;
    pageTable[pIndex].recent = 0;
    pageTable[pIndex].frame = rval;
    physicalMemory[rval] = address;
    frameBuffer[bufferIdx] = rval;
    bufferIdx = (bufferIdx + 1) % numFrames;
}