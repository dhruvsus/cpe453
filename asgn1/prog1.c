#include "prog1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

//global variables
roundRobinNode head = {.pid = 0, .next = &head, .prev = &head};
roundRobinNode *curr = &head;

long quantum = -1;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,
                "usage: %s <quantum> [prog 1 [args] [: prog 2 [args] [: ...]]]\n",
                argv[0]);
        exit(1);
    }
    quantum = strtol(argv[1], NULL, 10);
    //error check for quantum
    if (quantum < 1)
    {
        fprintf(stderr, "%s: invalid quantum, enter quantum greater than 0", argv[1]);
        exit(1);
    }

    // starting address of first program name
    int index = 2;
    //pointer to first program name

    char **first = argv + index;
    // int status=0;
    while (index < argc)
    {
        if (!strcmp(argv[index], ":"))
        {
            //index points to index of the :
            //set : to char* NULL for execvp
            argv[index] = NULL;
            //schedule job with arguments from first+1 to index
            schedule_job(first);
            //set first to point to new job
            first = argv + index + 1;
        }
        index++;
    }
    // the while loop won't catch the last job as it doesn't have a :
    schedule_job(first);
    return 0;
}
void schedule_job(char **first)
{
    // launch and stop child
    pid_t child = fork();
    if (child == 0)
    {
        // this is the child
        raise(SIGSTOP);
        //after getting the first SIGCONT
        execvp(*first, first);
    }
}