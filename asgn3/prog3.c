#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{   
    int *addresses, i=0;
    addresses = (int*)malloc(sizeof(int)*500000);
    FILE *fd;
    if (argc != 5)
    {
        fprintf(stderr, "incorrect number of args\n");
        return 1;
    }
    if(!(fd = fopen(argv[4], "r"))){
        fprintf(stderr, "file not found\n");
        return 1;
    }
    // stored addresses
    while(fscanf(fd, "%x", &addresses[i])!=EOF){
        printf("%u\n",addresses[i]);
        i++;
    }
    return 0;
}