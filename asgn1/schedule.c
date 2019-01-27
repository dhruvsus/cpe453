#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>

void set_timer();
void cancel_timer();
void handler(int signum);
void add_job(char** start);
void remove_job();

typedef struct sched_ll {
   pid_t pid;
   struct sched_ll* next;
   struct sched_ll* prev;
} ll_node;

ll_node head = {0, &head, &head};
ll_node* curr = &head;
long quantum;  

void set_timer() {
   static struct itimerval timer;

   long sec = quantum / 1000000;
   long usec = quantum % 1000000;

   /* Send first alarm at quantum. */
   timer.it_value.tv_sec = sec;
   timer.it_value.tv_usec = usec;

   /* Send successive alarms at quantum interval. */
   timer.it_interval.tv_sec = sec;
   timer.it_interval.tv_usec = usec;

   setitimer(ITIMER_REAL, &timer, NULL);
}

void cancel_timer() {
   static struct itimerval cancelAlarm = {0};
   setitimer(ITIMER_REAL, &cancelAlarm, NULL);
}

void handler(int signum) {
   kill(curr->pid, SIGSTOP);

   /* Advance cursor. */
   curr = curr->next;
   if (curr == &head) {
      curr = curr->next;
   }

   /* List is empty. */
   if (&head == head.next) {
      return;
   }

   int result = kill(curr->pid, SIGCONT);

   // Only one process left in the list, but it is no longer running.
   if (result && head.prev == head.next) {
      exit(0);
   }
}

void add_job(char** start) {
   if (!*start) {
      return;
   }

   pid_t child = fork();
   if (child == 0) {
      raise(SIGSTOP);
      execvp(*start, start);
      exit(2);
   }

   if (child == -1) {
      return;
   }

   ll_node* node = (ll_node*)malloc(sizeof(ll_node));
   node->pid = child;
   node->prev = head.prev;
   node->next = &head;
   head.prev->next = node;
   head.prev = node;
}

void remove_job() {
   ll_node* next = curr->next;
   ll_node* prev = curr->prev;

   next->prev = prev;
   prev->next = next;

   free(curr);
   curr = next;
   if (curr->pid == 0) {
      curr = curr->next;
   }
}

int main(int argc, char** argv) {
   if (argc < 2) {
      fprintf(stderr,
       "usage: %s <quantum> [prog 1 [args] [: prog 2 [args] [: ...]]]\n",
       argv[0]);
      exit(1);
   }

   quantum = strtol(argv[1], NULL, 10);
   if (quantum < 1) {
      fprintf(stderr, "%s: quantum cannot be less than 1\n", argv[0]);
      exit(1);
   }
   quantum *= 1000;

   int ndx = 2;

   /* Burn any leading ':' characters. */
   while (ndx < argc && !strcmp(argv[ndx], ":")) {
      ++ndx;
   }

   /* No commands found. */
   if (argc == ndx) {
      return 0;
   }

   /* Record position of first command. */
   char** start = argv + ndx;

   /* Schedule all but last job. */
   while(ndx < argc) {
      if (!strcmp(argv[ndx], ":")) {
         /* Replace ':' characters with NULL's for execvp. */
         argv[ndx] = NULL;

         add_job(start);
         start = argv + ndx + 1;
      }
      ndx++;
   }

   /* Schedule last job. */
   add_job(start);

   curr = head.next;

   /* Schedule alarm handler. */
   struct sigaction alarmAction = {0};
   alarmAction.sa_handler = handler;
   sigaction(SIGALRM, &alarmAction, NULL);

   kill(curr->pid, SIGCONT);
   set_timer();

   pid_t child;
   int status;
   while (curr->pid != 0) {
      child = waitpid(curr->pid, &status, WUNTRACED);
      if (child <= 0) {
         continue;
      }

      cancel_timer();
      remove_job();
      if (curr->pid == 0) {
         break;
      }

      kill(curr->pid, SIGCONT);
      set_timer();
   }

   return 0;
}

