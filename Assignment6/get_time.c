#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>

long g_count = 0;
typedef struct hybrid_lock {
	pthread_mutex_t mutex;
}hybrid_lock;

int hybrid_lock_init(hybrid_lock * h_lock);
int hybrid_lock_destroy(hybrid_lock * h_lock);
int hybrid_lock_lock(hybrid_lock * h_lock);
int hybrid_lock_unlock(hybrid_lock * h_lock);

void *hybrid_func(void *arg);
double get_passed_time(struct timeval * a, struct timeval * b);

hybrid_lock g_hybrid;

int main(){
	int j, k, l, rc;
	struct timeval start, cur;
	double passed_time;
	pthread_t tid;

	// Get time of for loop
	gettimeofday(&start);
	for (j = 0; j<100000; j++)
		for (k = 0; k<3000; k++)
			l += j * k;
	gettimeofday(&cur);

	passed_time = get_passed_time(&start, &cur);

	printf("for loop -> %f seconds\n", passed_time);

	// Initialize g_hybrid and lock g_hybrid
	// -> thread cannot lock g_hybrid in hybrid_func
	hybrid_lock_init(&g_hybrid);
	hybrid_lock_lock(&g_hybrid);

	rc = pthread_create(&tid, NULL, hybrid_func, NULL);
	if (rc) {
		fprintf(stderr, "pthread_create() error\n");
		hybrid_lock_destroy(&g_hybrid);
		exit(0);
	}

	rc = pthread_join(tid, NULL);
	if (rc) {
		fprintf(stderr, "pthread_join() error\n");
		hybrid_lock_destroy(&g_hybrid);
		exit(0);
	}

	hybrid_lock_unlock(&g_hybrid);
	hybrid_lock_destroy(&g_hybrid);

	return 0;
}

void *hybrid_func(void *arg){
	long i, j, k, count = (long)10000000;
	long long l;

	/*
	 * Increase the global variable, g_count.
	 * This code should be protected by
	 * some locks, e.g. spin lock, and the lock that 
	 * you implemented for assignment,
	 * because g_count is shared by other threads.
	 */
	for (i = 0; i<count; i++) {

		/********************** Critical Section **********************/
		hybrid_lock_lock(&g_hybrid);
		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		hybrid_lock_unlock(&g_hybrid);
		/**************************************************************/
	}

	return ;
}

int hybrid_lock_init(hybrid_lock * h_lock){

	int err_mutex;

	err_mutex = pthread_mutex_init(&h_lock->mutex, NULL);

	if(err_mutex != 0)
		return err_mutex;
	else
		return 0;
}

int hybrid_lock_destroy(hybrid_lock * h_lock){

	int err_mutex;

	err_mutex = pthread_mutex_destroy(&h_lock->mutex);

	if(err_mutex != 0)
		return err_mutex;
	else
		return 0;
}

int hybrid_lock_lock(hybrid_lock * h_lock){

	int check;
	long long int i=0;
	struct timeval start, cur;
	double passed_time;

	// Get while loop that executed for 1 second
	// While loop executes until i = 80000000
	gettimeofday(&start, NULL);
	while((check = pthread_mutex_trylock(&h_lock->mutex)) != 0){
		i++;
		if(i == 80000000){
			gettimeofday(&cur, NULL);
			passed_time = get_passed_time(&start, &cur);
			printf("%ld -> %f seconds\n", i, passed_time);
			i = 0;
			gettimeofday(&start);
		}
	}

	if(check == 0)
		return 0;
	else
		pthread_mutex_lock(&h_lock->mutex);
}

int hybrid_lock_unlock(hybrid_lock * h_lock){

	int err_mutex;

	err_mutex = pthread_mutex_unlock(&h_lock->mutex);
	
	if(err_mutex != 0)
		return err_mutex;
	else
		return 0;
}

double get_passed_time(struct timeval * a, struct timeval * b){
	double time = (double)(b->tv_sec) + (double)(b->tv_usec)/1000000.0
					- (double)(a->tv_sec) - (double)(a->tv_usec)/1000000.;
	return time;
}