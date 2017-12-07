#include "hybrid_lock.h"

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

	gettimeofday(&start, NULL);

	while((check = pthread_mutex_trylock(&h_lock->mutex)) != 0){
		i++;
		if(i == 80000000){
			gettimeofday(&cur, NULL);
			passed_time = get_passed_time(&start, &cur);
			if(passed_time >= (double)1)
				break;
		}
	}
	if(check == 0){
		return 0;
	}
	else{
		pthread_mutex_lock(&h_lock->mutex);
	}
}

int hybrid_lock_unlock(hybrid_lock * h_lock){

	int err_mutex;

	err_mutex = pthread_mutex_unlock(&h_lock->mutex);\
	
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