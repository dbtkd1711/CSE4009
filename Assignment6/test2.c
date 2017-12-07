#include "hybrid_lock.h"

long g_count = 0;
pthread_mutex_t g_mutex;
pthread_spinlock_t g_spin;
hybrid_lock g_hybrid;

void *thread_func_mutex(void *arg);
void *thread_func_spin(void *arg);
void *hybrid_func(void *arg);

int main(int argc, char *argv[])
{
	pthread_t *tid;
	long i, thread_count, value;
	int rc, command;

	/*
	 * Check that the program has three arguments
	 * If the number of arguments is not 3, terminate the process.
	 */
	if (argc != 3) {
		fprintf(stderr, "usage: %s <thread count> <value>\n", argv[0]);
		exit(0);
	}

	/*
	 * Get the values of the each arguments as a long type.
	 * It is better that use long type instead of int type,
	 * because sizeof(long) is same with sizeof(void*).
	 * It will be better when we pass argument to thread
	 * that will be created by this thread.
	 */
	thread_count = atol(argv[1]);
	value = atol(argv[2]);

	// Get Wanted lock from user
	printf("Which lock ? Enter number\n");
	printf("1. mutex, 2. spin, 3. hybrid : ");

	scanf("%d", &command);

	/*
	 * Create array to get tids of each threads that will
	 * be created by this thread.
	 */
	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if (!tid) {
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	/*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */
	
	// 1. Using mutex lock
	// -> thread will execute thread_func_mutex
	if(command == 1){
		pthread_mutex_init(&g_mutex, NULL);
		for (i = 0; i<thread_count; i++) {
			rc = pthread_create(&tid[i], NULL, thread_func_mutex, (void*)value);
			if (rc) {
				fprintf(stderr, "pthread_create() error\n");
				free(tid);
				pthread_mutex_destroy(&g_mutex);
				exit(0);
			}
		}
	}
	// 2. Uisng spin lock
	// -> thread will execute thread_func_spin
	else if(command == 2){
		pthread_spin_init(&g_spin, 0);
		for (i = 0; i<thread_count; i++) {
			rc = pthread_create(&tid[i], NULL, thread_func_spin, (void*)value);
			if (rc) {
				fprintf(stderr, "pthread_create() error\n");
				free(tid);
				pthread_spin_destroy(&g_spin);
				exit(0);
			}
		}

	}
	else if(command == 3){
		hybrid_lock_init(&g_hybrid);
		for (i = 0; i<thread_count; i++) {
			rc = pthread_create(&tid[i], NULL, hybrid_func, (void*)value);
			if (rc) {
				fprintf(stderr, "pthread_create() error\n");
				free(tid);
				hybrid_lock_destroy(&g_hybrid);
				exit(0);
			}
		}

	}
	// 3. Using hybrid lock
	// -> thread will execute hybrid_func
	else{
		printf("**Wrong input**\n");
		exit(0);
	}


	/*
	 * Wait until all the threads you created above are finished.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_join(tid[i], NULL);
		if (rc) {
			fprintf(stderr, "pthread_join() error\n");
			free(tid);
			if(command == 1)
				pthread_mutex_destroy(&g_mutex);
			else if(command == 2)
				pthread_spin_destroy(&g_spin);
			else
				hybrid_lock_destroy(&g_hybrid);

			exit(0);
		}
	}
	/*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */ 
	printf("value: %ld\n", g_count);

	free(tid);
}

void *thread_func_mutex(void *arg){
	long i, j, k, count = (long)arg;
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
		pthread_mutex_lock(&g_mutex);
		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		pthread_mutex_unlock(&g_mutex);
		/**************************************************************/
	}
}

void *thread_func_spin(void *arg){
	long i, j, k, count = (long)arg;
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
		pthread_spin_lock(&g_spin);
		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		pthread_spin_unlock(&g_spin);
		/**************************************************************/
	}
}

void *hybrid_func(void *arg){
	long i, j, k, count = (long)arg;
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
}