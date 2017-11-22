#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 4000

typedef struct argument{
	int (*a)[SIZE];
	int (*b)[SIZE];
	long long int (*c)[SIZE];
	int start_point;
	int end_point;
}argument;

void div_mat_and_threading(int a[][SIZE], int b[][SIZE], long long int c[][SIZE], int num);
void * mul_mat(void * args);
long long int get_sum(long long int c[][SIZE]);
double get_elapse_time(struct timeval a, struct timeval b);

// Argument 1 : file name1
// Argument 2 : file name2
// Argument 3 : num_of_threads user want
int main(int argc, char ** argv){

	if(argc < 4){
		printf("usage : ./threads <pathname1> <pathname2> <num_of_thread>\n");
		exit(0);
	}
	// define matrices and allocate
	int (*a)[SIZE] = (int(*)[4000])malloc(sizeof(*a)*SIZE);
	int (*b)[SIZE] = (int(*)[4000])malloc(sizeof(*b)*SIZE);
	long long int (*c)[SIZE] = (long long int(*)[4000])malloc(sizeof(*c)*SIZE);

	// define matrices and allocate
	/*int **a, **b;
	long long int **c;

	a = (int**)malloc(sizeof(int*)*SIZE);
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

	printf("matrix initialization finished\n");

	int num_of_thread = atoi(argv[3]);

	// for measure time
	struct timeval start_mul, end_mul, start_sum, end_sum;
	double mul_time, sum_time;

	gettimeofday(&start_mul, NULL);
	div_mat_and_threading(a,b,c, num_of_thread);
	gettimeofday(&end_mul, NULL);


	mul_time = get_elapse_time(start_mul, end_mul);

	printf("multiplication finished\n");
	printf("multiplication time : %f seconds\n", mul_time);


	gettimeofday(&start_sum, NULL);
	long long int sum = get_sum(c);
	gettimeofday(&end_sum, NULL);

	sum_time = get_elapse_time(start_sum, end_sum);

	printf("summation finished\n");
	printf("summation time : %f seconds\n", sum_time);
	printf("sum : %lld\n", sum);

	free(a);
	free(b);
	free(c);
	/*for(int i=0 ; i < SIZE ; i++){
		free(a[i]);
		free(b[i]);
		free(c[i]);
	}
	free(a);
	free(b);
	free(c);*/

	return 0;
}

void div_mat_and_threading(int a[][SIZE], int b[][SIZE], long long int c[][SIZE], int num){
	int quotient = SIZE / num;
	int remainder = SIZE % num;

	argument * args = (argument*)malloc(sizeof(argument)*num);

	args[0].start_point = 0;
	args[0].end_point = quotient-1;

	for(int i=1 ; i < num-1 ; i++){
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

	for(int i=0 ; i < num ; i++)
		pthread_create(threads+i, NULL, mul_mat, args+i);
	for(int i=0 ; i < num ; i++)
		pthread_join(threads[i], NULL);

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
	return NULL;
}

long long int get_sum(long long int c[][SIZE]){
	long long int sum = 0;

	for(int i=0 ; i < SIZE ; i++){
		for(int j=0 ; j < SIZE ; j++)
			sum += c[i][j];
	}
	return sum;
}

double get_elapse_time(struct timeval a, struct timeval b){
	double time = (double)(b.tv_sec) + (double)(b.tv_usec)/1000000.0
					- (double)(a.tv_sec) - (double)(a.tv_usec)/1000000.;
	return time;
}