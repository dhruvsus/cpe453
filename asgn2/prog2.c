#include <stdio.h>
#include <string.h>
int main(int argc, char** argv)
{
	int systemWide=0;
	//check number of arguments
	if(argc<3||argc>4)
	{
		fprintf(stderr,"Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
		return 1;
	}
	//check systemwide
	if(argc==4)
	{
		if(!strcmp(argv[1],"-s"))
		{
			systemWide=1;
		}
		else
		{	
			fprintf(stderr,"Usage: prog2 [-s] <executable> <interval> <logFilename>\n");
			return 1;

		}
	}
	//testing stage 1
	printf("systemwide status=%d, argc=%d\n",systemWide,argc);
	//fork and exec
	
	return 0;
}
