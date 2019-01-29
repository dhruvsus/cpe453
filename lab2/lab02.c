
/* Example */                                                                   

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
int sum; // shared variable

void *runner(void *arg) {
	printf("%d ",*(int*)arg);
	pthread_mutex_lock(&mutex);
	sum = sum + *(int*)arg;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int numThreads=atoi(argv[1]);
	pthread_t t[numThreads];
	int i, a[numThreads];
	sum = 0;

	printf("sum of "); 
	for (i = 0; i < numThreads; ++i)
	{
		a[i] = i + 1;
		if ( pthread_create(&t[i], NULL, runner, a+i) )
		{
			fprintf(stderr, "Error: pthread_create\n");
			return 1;
		}
	}

	for (i = 0; i < numThreads; ++i)
		pthread_join(t[i], NULL);
	printf("is %d\n", sum);
	pthread_mutex_destroy(&mutex); 
	return 0;
}


