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
    curr = head.next;

    /* Schedule alarm handler. */
    struct sigaction alarmAction = {0};
    alarmAction.sa_handler = handler;
    sigaction(SIGALRM, &alarmAction, NULL);

    kill(curr->pid, SIGCONT);
    set_timer();

    pid_t child;
    int status;
    while (curr->pid != 0)
    {
        child = waitpid(curr->pid, &status, WUNTRACED);
        if (child <= 0)
        {
            continue;
        }

        cancel_timer();
        remove_job();
        if (curr->pid == 0)
        {
            break;
        }

        kill(curr->pid, SIGCONT);
        set_timer();
    }

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
        // kill since the process has been done and is no longer needed
        exit(EXIT_SUCCESS);
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
void set_timer()
{
    static struct itimerval timer;

    long sec = quantum / 1000000;
    long usec = quantum % 1000000;

    /* Send first alarm at quantum. */
    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = usec;

    /* Send successive alarms at quantum interval. */
    timer.it_interval.tv_sec = sec;
    timer.it_interval.tv_usec = usec;

    setitimer(ITIMER_REAL, &timer, NULL);
}

void cancel_timer()
{
    static struct itimerval cancelAlarm = {0};
    setitimer(ITIMER_REAL, &cancelAlarm, NULL);
}

void handler(int signum)
{
    kill(curr->pid, SIGSTOP);

    /* Advance cursor. */
    curr = curr->next;
    if (curr == &head)
    {
        curr = curr->next;
    }

    /* List is empty. */
    if (&head == head.next)
    {
        return;
    }

    int result = kill(curr->pid, SIGCONT);

    // Only one process left in the list, but it is no longer running.
    if (result && head.prev == head.next)
    {
        exit(0);
    }
}
void remove_job()
{
    roundRobinNode *next = curr->next;
    roundRobinNode *prev = curr->prev;

    next->prev = prev;
    prev->next = next;

    free(curr);
    curr = next;
    if (curr->pid == 0)
    {
        curr = curr->next;
    }
}
