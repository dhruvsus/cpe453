#ifndef PROG_H
#define PROG_H

#define MAX_PROCESSES 100
#define MAX_ARGUMENTS 10

void schedule_job(char** first);
void set_timer();
void cancel_timer();
void handler(int signum);
void remove_job();
#endif
