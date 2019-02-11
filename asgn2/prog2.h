#ifndef PROG2_H
#define PROG2_H
#define STAT "/proc/stat"
#define MEMINFO "/proc/meminfo"
#define LOADAVG "/proc/loadavg"
#define DISKSTATS "/proc/diskstats"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
void getStat(FILE *logfile);
void getMeminfo(FILE *logfile);
void getDiskstats(FILE *logfile);
void getLoadavg(FILE *logfile);
void getPidStat(FILE **logfile, FILE **pidstat);
void getPidStatm(FILE **logfile, FILE **pidstatm);
int forkAndExec(char *executable, long interval, int systemWide, char *logFile);
#endif