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
        inputErr();
        return 1;
    }
    int quantum = atoi(argv[1]);
    // starting address of first program name
    int index = 2;
    // int status=0;
    while (index < argc)
    {
        char *args[MAX_ARGUMENTS + 1];
        char *progName = argv[index];
        index++;
        index = build_args(argv, args, index, argc);
        int pid = fork();
        if (pid == 0)
        {
            printf("This is being printed from the child process\n");
            printf("%s\n", progName);
            printf("%s\n", args[0]);
            execvp("./two", argv[3]);
        }
        wait(NULL);
        printf("Finished executing the parent process\n"
               " - the child won't get here--you will only see this once\n");
    }
    return 0;
}
void inputErr()
{
    fprintf(stderr, "erroneous input. input format:\n");
    fprintf(stderr, "prog1 quantum [prg 1 [args] [: prg 2 [args] [: prg3 [args] [: … ]]]]\n");
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