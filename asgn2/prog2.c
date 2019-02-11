#include "prog2.h"
//global vars
time_t t;
char *curT;
char stat[25], statm[25];
FILE *pidstatm,*pidstat;

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
				fprintf(logFile, "System  ");
				getStatData(logFile);
				getMeminfoData(logFile);
				getLoadavgData(logFile);
				getDiskstatsData(logFile);
				fprintf(logFile, "\n");
			}
			fprintf(logFile, "[%s ", curT);
			fprintf(logFile, "Process(%d)  ", pid);

			getPidStatData(&logFile, &pidstat);
			getPidStatmData(&logFile, &pidstatm); //wtf how do pass FILE pointers in C?

			/* Close everything  */
			fclose(pidstatm);
			fclose(pidstat);
			fprintf(logFile, "\n");
			usleep(interval);
		}
	}
	return status;
}
void getStatData(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	char *tmp;
	FILE *stat;

	stat = fopen(STAT, "r");

	/* get cpu line */
	getline(&line, &size, stat);
	fprintf(logfile, "[PROCESS] ");
	/* skip "cpu" tok */
	strtok(line, " ");
	fprintf(logfile, "cpuusermode ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	/* skip nice */
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

	/* skipping over individual cpus */
	getline(&line, &size, stat);
	while (strcmp(strtok(line, " "), "intr") != 0)
	{
		getline(&line, &size, stat);
	}
	fprintf(logfile, " intr ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* grab ctxt */
	getline(&line, &size, stat);

	fprintf(logfile, " ctxt ");
	/* skip over ctxt */
	strtok(line, " ");

	/* getting rid of jerk \n */
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);

	/*skip over btime*/
	getline(&line, &size, stat);

	/* grab processes */
	getline(&line, &size, stat);
	/* skip "processes" tok */
	strtok(line, " ");

	fprintf(logfile, " forks ");

	/* getting rid of jerk \n */
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';

	fprintf(logfile, "%s", tmp);

	/* grab procs_running */
	getline(&line, &size, stat);
	/* skip "procs_running" tok */
	strtok(line, " ");

	fprintf(logfile, " runnable ");

	/* getting rid of jerk \n */
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);

	/* grab procs_blocked */
	getline(&line, &size, stat);
	/* skip over "procs_blocked" tok */
	strtok(line, " ");

	fprintf(logfile, " blocked ");

	/* getting rid of jerk \n */
	tmp = strtok(NULL, " ");
	tmp[strlen(tmp) - 1] = '\0';
	fprintf(logfile, "%s", tmp);

	fclose(stat);
}

void getMeminfoData(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	FILE *meminfo;

	meminfo = fopen(MEMINFO, "r");
	fprintf(logfile, " [MEMORY] ");

	/* get MemTotal line */
	getline(&line, &size, meminfo);
	/* skip over "MemTotal" tok */
	strtok(line, " ");
	fprintf(logfile, "memtotal ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* get MemFree line */
	getline(&line, &size, meminfo);
	/* skip over "MemFree" tok */
	strtok(line, " ");
	fprintf(logfile, " memfree ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* skip buffers line */
	getline(&line, &size, meminfo);

	/* get cached line */
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " cached ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* get swapcached line */
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " swapcached ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* get active line */
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " active ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	/* get inactive line */
	getline(&line, &size, meminfo);
	strtok(line, " ");
	fprintf(logfile, " inactive ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	fclose(meminfo);
}

void getDiskstatsData(FILE *logfile)
{
	char *line = NULL;
	size_t size = 0;
	FILE *diskstats;
	diskstats = fopen(DISKSTATS, "r");

	/* skipping over unnecessary junk */
	while (1)
	{
		/* grab line */
		getline(&line, &size, diskstats);
		/* skip first token */
		strtok(line, " ");
		/* skip second token */
		strtok(NULL, " ");

		if (!strcmp(strtok(NULL, " "), "sda"))
		{
			break;
		}
	}

	/* line should be sda at this point */
	fprintf(logfile, " [DISKSTATS(sda)] ");
	fprintf(logfile, "totalnoreads ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	/* skip over reads merged */
	strtok(NULL, " ");
	fprintf(logfile, " totalsectorsread ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " nomsread ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " totalnowrites ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	/* skip over writes merged */
	strtok(NULL, " ");
	fprintf(logfile, " nosectorswritten ");
	fprintf(logfile, "%s", strtok(NULL, " "));
	fprintf(logfile, " nomswritten ");
	fprintf(logfile, "%s", strtok(NULL, " "));

	fclose(diskstats);
}

void getLoadavgData(FILE *logfile)
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

void getPidStatData(FILE **logfile, FILE **pidstat)
{
	//rss here prints a space afer
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

void getPidStatmData(FILE **logfile, FILE **pidstatm)
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
