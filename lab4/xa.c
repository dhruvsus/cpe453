/* xa.c

   Semaphore producer consumer example code 

   gcc xa.c -lpthread -o xa 

   Use the example code to get started

   Modify the code as described below:

   Add code for argv[1] for an integer n value

   Add code for argv[2] for the size of a circular buffer (bufsize)
   Use malloc to create an array [] of given bufsize and use that array
   as a circular buffer, instead of the global int buf.

   Display an error msg if user does not provide int argv[1], argv[2] values

   Add code to storing each value from 1 thru n in a circular buffer,
   that is an array of integers of given argv[2] bufsize
   You will also need to declare variables for keeping track of positions
   in the circular buffer

   Increase the size of the semaphore hole to same as the bufsize

   Use the producer and consumer functions to calc the sum as follows:

   The producer should store each value from 1 thru n in the circular buffer
   and overwrite any previous values in the buffer

   The consumer should read each value from the circular buffer to calculate  
   the total sum 

   The producer and consumer threads should exit after the nth value
   is stored in the buffer and added to the sum at which point the main
   should display the sum

*/

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *producer(void *arg);
void *consumer(void *arg);

sem_t e; // semaphore empty
sem_t h; // semaphore hole
int n; // numbers from 1 to n added to buffer
int bufSize; // size of bounded buffer
int bufLocation=0;
int bufValue=0;
int sum=0;
int* buf;
int main (int argc, char** argv)
{
	printf("%d\n",argc);
	if(argc!=3){
		fprintf(stderr, "invalid number of arguments\n");
		fprintf(stderr,"format: ./xa n bufsize\n");
		return 1;
	}
	
	// getting variables from argv
	n=atoi(argv[1]);
	bufSize=atoi(argv[2]);
	
	buf=(int*)malloc(bufSize*sizeof(int));

	//producer and consumer threads
	pthread_t p, c;

	sem_init(&e, 0, 0); // starts with empty semaphore
	sem_init(&h, 0, bufSize); // starts hole size 1, need to change 1 to bufsize

	pthread_create(&p, NULL, producer, NULL);
	pthread_create(&c, NULL, consumer, NULL);

	pthread_join(p, NULL);
	pthread_join(c, NULL);

	sem_destroy(&h);
	sem_destroy(&e);

	return 0;     
}

void *producer (void *arg) {
	while ( 1 ) {
		sem_wait(&h); // decrement h
		bufValue=bufValue%n+1;
		bufLocation=bufLocation%bufSize;
		buf[bufLocation]=bufValue;
		bufValue++;
		bufLocation++;
		sem_post(&e); // increment e
	}
	pthread_exit(0);
}

void *consumer (void *arg) {
	while ( 1 ) {
		sem_wait(&e); // decrement e
		//printf("%d\n", buf[bufLocation]);
		//fflush(stdin);
		sum+=buf[bufLocation];
		sem_post(&h); // increment h
	}
	pthread_exit(0);
}
