#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "prog2.h"
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
	//testing stage 1
	printf("systemwide status=%d, argc=%d\n", systemWide, argc);
	//fork and exec
	return forkAndExec(executable);
}
int forkAndExec(char *executable)
{
	int status = 0;
	int pid = fork();
	status = pid;
	if (pid == 0)
	{
		//child
		execlp(executable, executable, (char *)NULL);
	}
	else
	{
		waitpid(-1, NULL, 0);
	}
	return status;
}
