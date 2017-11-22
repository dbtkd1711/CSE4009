#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 4000

// argument for pthread_create
typedef struct argument{
	int (*a)[SIZE];
	int (*b)[SIZE];
	long long int (*c)[SIZE];
	int start_point;
	int end_point;
}argument;

pthread_mutex_t g_mutex;

long long int sum=0;

void div_mat_and_threading(int a[][SIZE], int b[][SIZE], long long int c[][SIZE], int num);
void * mul_mat(void * args);
long long int get_sum(long long int c[][SIZE]);
double get_elapse_time(struct timeval a, struct timeval b);

// Argument 1 : file name1
// Argument 2 : file name2
// Argument 3 : num_of_threads user want
int main(int argc, char ** argv){
	if(argc < 4){
		printf("usage : ./multi <pathname1> <pathname2> <num_of_thread>\n");
		exit(0);
	}

	// define matrices and allocate
	int (*a)[SIZE] = (int(*)[4000])malloc(sizeof(*a)*SIZE);
	int (*b)[SIZE] = (int(*)[4000])malloc(sizeof(*b)*SIZE);
	long long int (*c)[SIZE] = (long long int(*)[4000])malloc(sizeof(*c)*SIZE);
	/*a = (int**)malloc(sizeof(int*)*SIZE);
	b = (int**)malloc(sizeof(int*)*SIZE);
	c = (long long int**)malloc(sizeof(long long int*)*SIZE);

	for(int i=0 ; i < SIZE ; i++){
		a[i] = (int*)malloc(sizeof(int)*SIZE);
		b[i] = (int*)malloc(sizeof(int)*SIZE);
		c[i] = (long long int*)malloc(sizeof(long long int)*SIZE);
	}*/

	// open files
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;

	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[2], "r");

	if((fp1==NULL) || (fp2==NULL)){
		printf("file open error\n");
		exit(0);
	}

	// read matrices from files
	for(int i=0 ; i < SIZE ; i++){
		for(int j=0; j < SIZE ; j++){
			fscanf(fp1, "%d\t", &a[i][j]);
			fscanf(fp2, "%d\t", &b[i][j]);
		}
	}

	fclose(fp1);
	fclose(fp2);

	printf("matrix initialization completed\n");

	int num_of_thread = atoi(argv[3]);

	// for measure time
	struct timeval start_mul_sum, end_mul_sum;
	double mul_sum_time;

	gettimeofday(&start_mul_sum, NULL);
	div_mat_and_threading(a,b,c, num_of_thread);
	gettimeofday(&end_mul_sum, NULL);

	mul_sum_time = get_elapse_time(start_mul_sum, end_mul_sum);

	printf("multiplication + summation time : %f seconds\n", mul_sum_time);
	printf("sum : %lld\n", sum);

	free(a);
	free(b);
	free(c);
	
	return 0;
}

void div_mat_and_threading(int a[][SIZE], int b[][SIZE], long long int c[][SIZE], int num){
	int quotient = SIZE / num;
	int remainder = SIZE % num;

	// Arguments that passed to  pthread_create
	argument * args = (argument*)malloc(sizeof(argument)*num);

	args[0].start_point = 0;
	args[0].end_point = quotient-1;

	for(int i=1 ; i<num-1 ; i++){
		args[i].start_point = args[i-1].end_point+1;
		args[i].end_point = args[i].start_point+quotient-1;
	}
	args[num-1].start_point = args[num-2].end_point+1;
	args[num-1].end_point = SIZE-1;

	for(int i=0 ; i<num ; i++){
		args[i].a = a;
		args[i].b = b;
		args[i].c = c;
	}

	pthread_t * threads = (pthread_t*)malloc(sizeof(pthread_t)*num);

	pthread_mutex_init(&g_mutex, NULL);
	for(int i=0 ; i<num ; i++)
		pthread_create(threads+i, NULL, mul_mat, args+i);

	for(int i=0 ; i<num ; i++)
		pthread_join(threads[i], NULL);
	pthread_mutex_destroy(&g_mutex);

	free(args);
	free(threads);

	return;
}

void * mul_mat(void * args){

	argument * arg = (argument*) args;

	int (*a)[SIZE] = arg->a;
	int (*b)[SIZE] = arg->b;
	long long int (*c)[SIZE] = arg->c;

	int m = arg->start_point;
	int n = arg->end_point;

	for(int i=m ; i <= n ; i++){
		for(int j=0 ; j < SIZE ; j++){
			c[i][j] = 0;
			for(int k=0 ; k < SIZE ; k++){
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	long long int temp_sum = 0;

	for(int i=m ; i <= n ; i++)
		for(int j=0 ; j < SIZE ; j++)
			temp_sum += c[i][j];

	pthread_mutex_lock(&g_mutex);
	sum += temp_sum;
	pthread_mutex_unlock(&g_mutex);

	return NULL;
}

double get_elapse_time(struct timeval a, struct timeval b){
	double time = (double)(b.tv_sec) + (double)(b.tv_usec)/1000000.0
					- (double)(a.tv_sec) - (double)(a.tv_usec)/1000000.;
	return time;
}