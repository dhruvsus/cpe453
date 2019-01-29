#include "prog1.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>

typedef struct roundRobinNode
{
    pid_t pid;
    struct roundRobinNode *next;
    struct roundRobinNode *prev;
} roundRobinNode;

//global variables
roundRobinNode head = {.pid = 0, .next = &head, .prev = &head};
roundRobinNode *curr = &head;

long quantum = -1;

int main(int argc, char *argv[])
{
    quantum = strtol(argv[1], NULL, 10);
    //convert quantum to microseconds
    quantum = quantum * 1000;
    // starting address of first program name
    int index = 2;
    //pointer to first program name
    char **first = &argv[2];
    // int status=0;
    while (index < argc)
    {
        if (!strcmp(argv[index], ":"))
        {
            //index points to index of the :
            //set : to char* NULL for execvp
            argv[index] = NULL;
            //schedule job with arguments from first+1 to index
            scheduleJob(first);
            //set first to point to new job
            first = argv + index + 1;
        }
        index++;
    }
    // the while loop won't catch the last job as it doesn't have a :
    scheduleJob(first);
    curr = head.next;

    // alarm handler
    struct sigaction action = {{0}};
    action.sa_handler = handler;
    sigaction(SIGALRM, &action, NULL);

    kill(curr->pid, SIGCONT);
    setTimer();

    pid_t child;
    int status;
    while (curr->pid != 0)
    {
        child = waitpid(curr->pid, &status, WUNTRACED);
        if (child <= 0)
        {
            continue;
        }
        // cancel timer
        static struct itimerval cancelAlarm = {{0}};
        setitimer(ITIMER_REAL, &cancelAlarm, NULL);
        removeJob();
        if (curr->pid == 0)
        {
            break;
        }

        kill(curr->pid, SIGCONT);
        setTimer();
    }

    return 0;
}
void scheduleJob(char **first)
{
    // launch and stop child
    pid_t child = fork();
    if (child == 0)
    {
        // this is the child
        raise(SIGSTOP);
        //after getting the first SIGCONT
        execvp(*first, first);
        // kill since the process has been done and is no longer needed
        exit(0);
    }
    // parent error checks and inserts node into list
    if (child == -1)
    {
        return;
    }
    roundRobinNode *node = (roundRobinNode *)malloc(sizeof(roundRobinNode));
    //insertion up front
    node->pid = child;
    node->prev = head.prev;
    node->next = &head;
    head.prev->next = node;
    head.prev = node;
    return;
}
void setTimer()
{
    // convert microseconds to xsec + ymicroseconds
    long sec = quantum / 1000000;
    long usec = quantum % 1000000;

    static struct itimerval itimer;

    itimer.it_value.tv_sec = sec;
    itimer.it_value.tv_usec = usec;
    itimer.it_interval.tv_sec = sec;
    itimer.it_interval.tv_usec = usec;
    setitimer(ITIMER_REAL, &itimer, NULL);
}

void handler(int signum)
{
    // stop current process and move cur along
    kill(curr->pid, SIGSTOP);
    curr = curr->next;
    // if cur lands on head
    if (curr->pid == 0)
    {
        curr = curr->next;
    }
    // if the list of pcb's is empty
    if (&head == head.next)
    {
        return;
    }
    // start the next process
    int result = kill(curr->pid, SIGCONT);

    // Only one process left in the list, but it is no longer running.
    if (result && head.prev == head.next)
    {
        exit(0);
    }
}
void removeJob()
{
    roundRobinNode *next = curr->next;
    roundRobinNode *prev = curr->prev;

    // break connections between curr->prev and curr->next to remove cur
    next->prev = prev;
    prev->next = next;
    free(curr);
    // move curr along
    curr = next;
    // if the process that cur lands on is the head wrap around
    if (curr->pid == 0)
    {
        curr = curr->next;
    }
}
