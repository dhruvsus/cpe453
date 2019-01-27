#ifndef PROG_H
#define PROG_H

#define MAX_PROCESSES 100
#define MAX_ARGUMENTS 10
void schedule_job(char** first);
typedef struct roundRobinNode {
   pid_t pid;
   struct roundRobinNode* next;
   struct roundRobinNode* prev;
} roundRobinNode;
#endif