#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 4000

void mul_mat(int a[][SIZE], int b[][SIZE], long long int c[][SIZE]);
long long int get_sum(long long int  c[][SIZE]);
double get_elapse_time(struct timeval a, struct timeval y);

// Argument 1 : file name1
// Argument 2 : file name2
int main(int argc, char ** argv){
	if(argc < 3){
		printf("usage : ./thread <pathname1> <pathname2>\n");
		exit(0);
	}

	// define matrices and allocate
	int (*a)[SIZE] = (int(*)[4000])malloc(sizeof(*a)*SIZE);
	int (*b)[SIZE] = (int(*)[4000])malloc(sizeof(*b)*SIZE);
	long long int (*c)[SIZE] = (long long int(*)[4000])malloc(sizeof(*c)*SIZE);



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

	// for measure time
	struct timeval start_mul, end_mul, start_sum, end_sum;
	double mul_time, sum_time;

	gettimeofday(&start_mul, NULL);
	mul_mat(a,b,c);
	printf("multiplication finished\n");
	gettimeofday(&end_mul, NULL);

	mul_time = get_elapse_time(start_mul, end_mul);
	printf("multiplication time : %f seconds\n", mul_time);


	gettimeofday(&start_sum, NULL);
	long long int sum = get_sum(c);
	printf("summation finished\n");
	gettimeofday(&end_sum, NULL);

	sum_time = get_elapse_time(start_sum, end_sum);
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

void mul_mat(int a[][SIZE], int b[][SIZE], long long int c[][SIZE]){
	for(int i=0 ; i < SIZE ; i++){
		for(int j=0 ; j < SIZE ; j++){
			c[i][j] = 0;
			for(int k=0 ; k < SIZE ; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	return;
}

long long int get_sum(long long int c[][SIZE]){
	long long int sum = 0;

	for(int i=0 ; i < SIZE ; i++){
		for(int j=0 ; j < SIZE ; j++){
			sum += c[i][j];
		}
	}
	return sum;
}

double get_elapse_time(struct timeval a, struct timeval b){
	double time = (double)(b.tv_sec) + (double)(b.tv_usec)/1000000.0
					- (double)(a.tv_sec) - (double)(a.tv_usec)/1000000.;
	return time;
}