#ifndef PROG3_H
#define PROG_H
typedef struct ptEntry
{
    int valid;
    int recent;
    int address;
    int frame;
} ptEntry;
#define min(a, b) (((a) < (b)) ? (a) : (b))

void placeInTable(int address, int pageSize, int numPages, int nBitsFrame, int numFrames, int policy);
int findEmptyFrame(int numFrames);
void swap(int policy, int address, int pIndex, int numFrames, int pageSize);
#endif