#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char * argv[]){
	int a[1];
	pid_t pid[1];
	int fd = open(argv[1], O_RDWR);
	pread(fd, a, 4, 0);
	printf("count : %d\n", a[0]);
	pread(fd, a, 4, 4);
	printf("want to count : %d\n", a[0]);

	pread(fd, pid, 4, 8);
	printf("process1 ID : %ld\n", pid[0]);
	pread(fd, pid, 4, 12);
	printf("process2 ID : %ld\n", pid[0]);
	pread(fd, pid, 4, 16);
	printf("process3 ID : %ld\n", pid[0]);
	return 0;
}