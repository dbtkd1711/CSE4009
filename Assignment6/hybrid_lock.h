#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>

typedef struct hybrid_lock {
	pthread_mutex_t mutex;
}hybrid_lock;

int hybrid_lock_init(hybrid_lock * h_lock);
int hybrid_lock_destroy(hybrid_lock * h_lock);
int hybrid_lock_lock(hybrid_lock * h_lock);
int hybrid_lock_unlock(hybrid_lock * h_lock);
double get_passed_time(struct timeval * a, struct timeval * b);
