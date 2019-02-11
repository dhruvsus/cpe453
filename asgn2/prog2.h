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
void getStatData(FILE *logfile);
void getMeminfoData(FILE *logfile);
void getDiskstatsData(FILE *logfile);
void getLoadavgData(FILE *logfile);
void getPidStatData(FILE **logfile, FILE **pidstat);
void getPidStatmData(FILE **logfile, FILE **pidstatm);
int forkAndExec(char *executable, long interval, int systemWide, char *logFile);
#endif