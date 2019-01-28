#ifndef PROG_H
#define PROG_H

#define MAX_PROCESSES 100
#define MAX_ARGUMENTS 10

void scheduleJob(char** first);
void setTimer();
void handler(int signum);
void removeJob();
#endif
