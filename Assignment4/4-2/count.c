#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int fd = -1;
struct sigaction act;

int open_file(char * filename);
void sig_handler(void*arg);

int main(int argc, char * argv[]){
	if(argc != 3){
		fprintf(stderr, "usage : ./count <num> <filename> \n");
		return 0;
	}

	if(atoi(argv[1]) <= 0){
		fprintf(stderr, "usage : <num> > 0\n");
		return 0;
	}

	if((fd=open_file(argv[2])) == 0){
		fprintf(stderr, "open_file error\n");
		return 0;
	}

	int num[1];
	num[0] = atoi(argv[1]);
	if(pwrite(fd, num, 4, 4) != 4){
		fprintf(stderr, "pwrite error\n");
		return 0;
	}

	pid_t pid[3];

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);

	if(sigaction(SIGUSR1, &act, NULL) < 0)
			fprintf(stderr, "sigaction error\n");

	// child1 create
	if( (pid[0]=fork()) < 0 ){
		fprintf(stderr, "fork1 error\n");
		exit(1);
	}
	else if(pid[0] == 0){ // child1
		printf("process 1 created\n");

		while(1)
			pause();
	}

	// child2 create
	if( (pid[1]=fork()) < 0){
		fprintf(stderr, "fork2 error\n");
		exit(1);
	}
	else if(pid[1] == 0){ // child2
		printf("process 2 created\n");

		while(1)
			pause();
	}

	// child3 create
	if( (pid[2]=fork()) < 0 ){
		fprintf(stderr, "fork3 error\n");
		exit(1);
	}
	else if(pid[2]==0){ // child3
		printf("process 3 created\n");

		while(1)
			pause();
	}

	// wait childs created
	sleep(1);
	printf("%ld %ld %ld \n", pid[0], pid[1], pid[2]);

	// write childs' pid to the file
	if(pwrite(fd, pid, 12, 8) != 12){
		fprintf(stderr, "pwrite error\n");
		return 0;
	}

	// first signal to process1
	kill(pid[0], SIGUSR1);

	pause();
	return 0;
}

int open_file(char * filename){
	int count[1];

	if((fd = open(filename, O_RDWR)) > 0){
		if(pread(fd, count, 4, 0) != 4){
			fprintf(stderr, "read error\n");
			return 0;
		}
		if(count[0] != 0)
			count[0] = 0;
		if(pwrite(fd, count, 4, 0) != 4){
			fprintf(stderr, "pwrite error\n");
			return 0;
		}
	}
	else if((fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 0777)) > 0){
		count[0] = 0;
		if(pwrite(fd, count, 4, 0) != 4){
			fprintf(stderr, "pwrite error\n");
			return 0;
		}
	}
	return fd;
}

void sig_handler(void*arg){
	int count[1];
	int num[1];
	pid_t pid[3];

	// read 3 pid from file
	if(pread(fd, pid, 12, 8) != 12){
		fprintf(stderr, "read error in sig_handler\n");
		exit(1);
	}
	// read current count from file
	if(pread(fd, count, 4, 0) != 4){
		fprintf(stderr, "read error in signal_handler\n");
		exit(1);
	}
	// read destination number from file
	if(pread(fd, num, 4, 4) != 4){
		fprintf(stderr, "read error in signal_handler\n");
		exit(1);
	}

	// increment count and write
	count[0]++;
	if(pwrite(fd, count, 4, 0) != 4){
		fprintf(stderr, "write error in signal_handler\n");
		exit(1);
	}

	// if count=num -> kill all the processes
	if(count[0]==num[0]){
		if(getpid()==pid[0]){
			kill(getppid(), SIGUSR2);
			kill(pid[1], SIGUSR2);
			kill(pid[2], SIGUSR2);
			kill(pid[0], SIGUSR2);
		}
		else if(getpid()==pid[1]){
			kill(getppid(), SIGUSR2);
			kill(pid[2], SIGUSR2);
			kill(pid[0], SIGUSR2);
			kill(pid[1], SIGUSR2);
		}
		else{
			kill(getppid(), SIGUSR2);
			kill(pid[0], SIGUSR2);
			kill(pid[1], SIGUSR2);
			kill(pid[2], SIGUSR2);
		}
		return;
	}

	// send signal to the next process after counting
	if(getpid()==pid[0])
		kill(pid[1], SIGUSR1);
	else if(getpid()==pid[1])
		kill(pid[2], SIGUSR1);
	else
		kill(pid[0], SIGUSR1);
	return;
}







