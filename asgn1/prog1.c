#include "prog1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,
                "usage: %s <quantum> [prog 1 [args] [: prog 2 [args] [: ...]]]\n",
                argv[0]);
        exit(1);
    }
    long quantum = strtol(argv[1], NULL, 10);
    //error check for quantum
    if (quantum < 1)
    {
        fprintf(stderr, "%s: invalid quantum, enter quantum greater than 0", argv[1]);
        exit(1);
    }

    // starting address of first program name
    int index = 2;
    //pointer to first program name
    
    char **first=argv+index;
    // int status=0;
    while (index < argc)
    {
        if (!strcmp(argv[index], ":"))
        {
            //index points to index of the :
            //set : to char* NULL for execvp
            argv[index]=NULL;
            //schedule job with arguments from first+1 to index
            schedule_job(first);
            //set first to point to new job
            first=argv+index+1;
        }
        index++;
    }
    // the while loop won't catch the last job as it doesn't have a :
    schedule_job(first);
    
    return 0;
}

int build_args(char *argv[], char *args[], int startIndex, int argc)
{
    int argsIndex = 0;
    while (startIndex < argc && strcmp(argv[startIndex], ":") != 0)
    {
        //arguments exist
        args[argsIndex] = argv[startIndex];
        argsIndex++;
        startIndex++;
    }
    //put a null string to end args
    args[argsIndex] = (char *)NULL;
    //now, we have run out of arguments for the program
    // startIndex points to either : or end of cmd
    return startIndex;
}