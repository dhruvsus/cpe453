#include "prog2.h"
//global vars
time_t t;
char *curT;
char stat[25], statm[25];
FILE *pidstatm, *pidstat;

int main(int argc, char **argv)
{
	int systemWide = 0;
	long interval = 0;
	char *logFile;
	char *executable;
	//check number of arguments
	if (argc < 4 || argc > 5)
	{
		fprintf(stderr, "Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
		return 1;
	}
	//check systemwide
	if (argc == 5)
	{
		if (!strcmp(argv[1], "-s"))
		{
			systemWide = 1;
			executable = argv[2];
			interval = strtol(argv[3], NULL, 10);
			if (!interval)
			{
				fprintf(stderr, "Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
				return 1;
			}
			logFile = argv[4];
		}
		else
		{
			fprintf(stderr, "Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
			return 1;
		}
	}
	if (argc == 4)
	{
		//parsing variables
		executable = argv[1];
		interval = strtol(argv[2], NULL, 10);
		if (!interval)
		{
			fprintf(stderr, "Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
			return 1;
		}
		logFile = argv[3];
	}
	//fork and exec
	return forkAndExec(executable, interval, systemWide, logFile);
}
int forkAndExec(char *executable, long interval, int systemWide, char *logFileName)
{
	int status = 0;
	pid_t pid = fork();
	status = pid;
	FILE *logFile = fopen(logFileName, "a");
	if (pid == 0)
	{
		//child
		execlp(executable, executable, (char *)NULL);
	}
	else
	{
		sprintf(stat, "/proc/%d/stat", pid);
		sprintf(statm, "/proc/%d/statm", pid);
		while (!waitpid(pid, NULL, WNOHANG))
		{
			//FILE *logFile = fopen(logFileName, "a");
			pidstatm = fopen(statm, "r");
			pidstat = fopen(stat, "r");
			if (pidstat == NULL || pidstatm == NULL)
			{
				break;
			}
			//parent sleeps and then parses files to report stats
			//time
			/* get the current time */
			time(&t);
			curT = ctime(&t);
			curT[strlen(curT) - 1] = ']';
			if (systemWide)
			{
				fprintf(logFile, "[%s ", curT);
				fprintf(logFile, "System ");
				getStat(logFile);
				getMeminfo(logFile);
				getLoadavg(logFile);
				getDiskstats(logFile);
				fprintf(logFile, "\n");
			}
			fprintf(logFile, "[%s ", curT);
			fprintf(logFile, "Process(%d) ", pid);
			getPidStat(&logFile, &pidstat);
			getPidStatm(&logFile, &pidstatm);
			fclose(pidstatm);
			fclose(pidstat);
			fprintf(logFile, "\n");
			usleep(interval);
		}
	}
	return status;
}
void getStat(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	char *tmp;
	FILE *stat;
	stat = fopen(STAT, "r");
	getline(&line, &size, stat);
	fprintf(logfile, "[PROCESS] ");
	strtok(line, " ");
	fprintf(logfile, "cpuusermode ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	strtok(NULL, " ");
	fprintf(logfile, " cpusystemmode ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " idletaskrunning ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " iowaittime ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " irqservicetime ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " softirqservicetime ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, stat);
	while (strcmp(strtok(line, " "), "intr") != 0)
	{
		getline(&line, &size, stat);
	}
	fprintf(logfile, " intr ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, stat);
	fprintf(logfile, " ctxt ");
	strtok(line, " ");
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);
	getline(&line, &size, stat);
	getline(&line, &size, stat);
	strtok(line, " ");
	fprintf(logfile, " forks ");
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);
	getline(&line, &size, stat);
	strtok(line, " ");
	fprintf(logfile, " runnable ");
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);
	getline(&line, &size, stat);
	strtok(line, " ");
	fprintf(logfile, " blocked ");
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);
	fclose(stat);
}

void getMeminfo(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	FILE *meminfo;
	meminfo = fopen(MEMINFO, "r");
	fprintf(logfile, " [MEMORY] ");
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, "memtotal ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " memfree ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, meminfo);
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " cached ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " swapcached ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " active ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " inactive ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fclose(meminfo);
}

void getDiskstats(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	FILE *diskstats;
	diskstats = fopen(DISKSTATS, "r");
	while (1)
	{
		getline(&line, &size, diskstats);
		strtok(line, " ");
		strtok(NULL, " ");
		if (!strcmp(strtok(NULL, " "), "sda"))
		{
			break;
		}
	}
	fprintf(logfile, " [DISKSTATS(sda)] ");
	fprintf(logfile, "totalnoreads ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	strtok(NULL, " ");
	fprintf(logfile, " totalsectorsread ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " nomsread ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " totalnowrites ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	strtok(NULL, " ");
	fprintf(logfile, " nosectorswritten ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " nomswritten ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fclose(diskstats);
}

void getLoadavg(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	FILE *loadavg;
	loadavg = fopen(LOADAVG, "r");
	getline(&line, &size, loadavg);
	fprintf(logfile, " [LOADAVG] ");
	fprintf(logfile, "1min ");
	fprintf(logfile, "%s", strtok(line, " "));
	fprintf(logfile, " 5min ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " 15min ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fclose(loadavg);
}

void getPidStat(FILE **logfile, FILE **pidstat)
{
	char line[256], *format;
	int field = 0;
	fprintf(*logfile, "[STAT] ");
	fgets(line, sizeof(line), *pidstat);
	format = strtok(line, " \n");
	while (format != NULL)
	{
		if (field == 1)
		{
			fprintf(*logfile, "executable %s ", format);
		}
		else if (field == 2)
		{
			fprintf(*logfile, "stat %s ", format);
		}
		else if (field == 10)
		{
			fprintf(*logfile, "minorfaults %s ", format);
		}
		else if (field == 12)
		{
			fprintf(*logfile, "majorfaults %s ", format);
		}
		else if (field == 14)
		{
			fprintf(*logfile, "usermodetime %s ", format);
		}
		else if (field == 15)
		{
			fprintf(*logfile, "kernelmodetime %s ", format);
		}
		else if (field == 18)
		{
			fprintf(*logfile, "priority %s ", format);
		}
		else if (field == 19)
		{
			fprintf(*logfile, "nice %s ", format);
		}
		else if (field == 20)
		{
			fprintf(*logfile, "nothreads %s ", format);
		}
		else if (field == 23)
		{
			fprintf(*logfile, "vsize %s ", format);
		}
		else if (field == 24)
		{
			fprintf(*logfile, "rss %s ", format);
		}
		++field;
		if (field > 24)
		{
			break;
		}
		format = strtok(NULL, " \n");
	}
}

void getPidStatm(FILE **logfile, FILE **pidstatm)
{
	char line[256], *format;
	int field = 0;
	fprintf(*logfile, "[STATM] ");
	fgets(line, sizeof(line), *pidstatm);
	format = strtok(line, " \n");
	while (format != NULL)
	{
		if (field == 0)
		{
			fprintf(*logfile, "program %s ", format);
		}
		else if (field == 1)
		{
			fprintf(*logfile, "residentset %s ", format);
		}
		else if (field == 2)
		{
			fprintf(*logfile, "share %s ", format);
		}
		else if (field == 3)
		{
			fprintf(*logfile, "text %s ", format);
		}
		else if (field == 5)
		{
			fprintf(*logfile, "data %s", format);
		}
		++field;
		if (field > 5)
		{
			break;
		}
		format = strtok(NULL, " \n");
	}
}
