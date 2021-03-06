/* xb.c

   Pthread producer and consumer example code using pthread_cond_wait
   and pthread_cond_signal 

   gcc xb.c -lpthread -o xb

   Use the example code to get started

   Add code for argv[1] for an integer n value 

   Add code for argv[2] for the size of a circular buffer (bufsize)
   Use malloc to create an array [] of given bufsize and use that array
   as a circular buffer, instead of the global int buf.

   Display an error if user does not provide int argv[1], argv[2] values

   Add code to storing each value from 1 thru n in a circular buffer,
   that is an array of integers of given argv[2] bufsize
   You will also need to declare variables for keeping track of positions
   in the circular buffer

   Use the producer and consumer functions to calc the sum as follows:

   The producer should store each value from 1 thru n in the circular buffer
   and overwrite any previous values in the buffer

   The consumer should read each value from the circular buffer to calculate  
   the total sum

   The producer and consumer threads should exit after the nth value
   is stored in the buffer and added to the sum at which point the main
   should display the sum

Hint: use mutex lock to ensure the consistency of the sum calculations

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void *producer (void *arg);
void *consumer (void *arg);

pthread_mutex_t mutex;
pthread_cond_t condvar;
int turn = 1;
int *buf;
//int buf=0;
int n;
int bufSize;
int bufLocation=0;
int bufValue=0;
int consumerLocation=0;
int consumerValue;
int consumerNoReads=0;
int sum=0;

int main (int argc, char** argv) {
	if(argc!=3){
		fprintf(stderr,"Error need 2 ints, datsize bufsize\n");
		return 1;
	}
	// getting variables from argv
	n=atoi(argv[1]);
	bufSize=atoi(argv[2]);
	buf=(int*)malloc(bufSize*sizeof(int));	

	pthread_t p, c;
	pthread_attr_t attr;

	pthread_attr_init (&attr);

	pthread_create (&p, &attr, producer, NULL);
	pthread_create (&c, &attr, consumer, NULL);

	pthread_join (p, NULL);
	pthread_join (c, NULL);
	printf("%d\n",sum);

	pthread_cond_destroy(&condvar);
	return 0;
}

void *producer (void *arg) {
	while ( bufValue<n ) {
		pthread_mutex_lock(&mutex);
		while ( turn != 1 )
			pthread_cond_wait (&condvar, &mutex);
		turn = 2;
		bufLocation=bufLocation%bufSize;
		bufValue = (bufValue%n) + 1;
		buf[bufLocation]=bufValue;
		printf("b[%d] %d\n",bufLocation,bufValue);
		bufLocation++;
		//sleep(1); 
		pthread_cond_signal (&condvar);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit (0);
}

void *consumer (void *arg) {
	while ( consumerNoReads<n ) {
		pthread_mutex_lock(&mutex);
		while ( turn != 2 )
			pthread_cond_wait (&condvar, &mutex);
		turn = 1;
		consumerValue=buf[consumerLocation];
		//printf("consumer consumes value %d\n",consumerValue);
		consumerNoReads++;
		consumerLocation=(consumerLocation+1)%bufSize;
		sum+=consumerValue;
		//printf("%d\n",sum);
		pthread_cond_signal (&condvar);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit (0);
}


