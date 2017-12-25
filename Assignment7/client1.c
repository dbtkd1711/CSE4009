#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#define SHM_KEY 1711
#define MSQ_KEY_1 1
#define TEXT 1024
#define TYPE 4
#define MSG_SIZE (sizeof(Msgtype))

typedef struct{
	long mtype;
	pid_t sender;
	pid_t receiver;
	char mtext[1024];
}Msgtype;

void * thr_fn(void * arg);

int msq_id;
pid_t * pid;

int main(){

	int shm_id;
	void * shm_addr;

	if((shm_id = shmget(SHM_KEY, MSG_SIZE*100+20, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "shmget failed\n");
		return -1;
	}
	if((shm_addr = shmat(shm_id, 0, 0)) == -1){
		fprintf(stderr, "shmat failed\n");
		return -1;
	}

	if((msq_id = msgget(MSQ_KEY_1, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "msgget failed\n");
		return -1;
	}

	pid_t dest;
	pthread_t tid;

	// Write client1's pid to the shared memory
	pid = shm_addr;
	pid[0] = getpid();

	int command;
	Msgtype msg;
	char msg_buf[1024];

	// Create thread which checks private message
	if(pthread_create(&tid, NULL, thr_fn, pid) != 0){
		fprintf(stderr, "pthread failed\n");
		return -1;
	}

	printf("\n-----------client1[%d]-----------\n", pid[0]);
	printf("1. Print all chatting log\n");
	printf("2. Send a message to the destination process\n");
	printf("----> Usage : 2 <destination> <message>\n");
	printf("3. Quit the client\n");
	printf("\n[%d] ", pid[0]);

	while(1){
		fflush(stdout);
		scanf("%d", &command);

		// 1. Print all chatting log
		if(command == 1){
			int * count = shm_addr + 12;
			if(*count == 0){
				printf("\nEmpty chatting log\n");
				printf("\n[%d] ", pid[0]);
				continue;
			}
			else{
				printf("\n***********chatting log***********\n");
				printf("count : %d\n", *count);
				for(int i=0 ; i < *count ; i++){
					pid_t * pid = shm_addr + 20 + 1028*i;
					char * message = shm_addr + 24 + 1028*i;
					printf("[%d]%s\n", *pid, message);
				}
				printf("\nPrint all chatting log success\n");
				printf("\n[%d] ", pid[0]);
				continue;
			}
		}
		// 2. Send a message to the destination process
		else if(command == 2){
			scanf("%d", &dest);
			fgets(msg_buf, 1024, stdin);

			msg.mtype = 1;
			msg.sender = pid[0];
			msg.receiver = dest;
			strcpy(msg.mtext, msg_buf);
			msgsnd(msq_id, &msg, TEXT, IPC_NOWAIT);
			printf("\nSending a message to %d success\n", dest);
			printf("\n[%d] ", pid[0]);
			continue;
		}
		// 3. Quit the client
		else if(command == 3){
			if(shmdt(shm_addr) == -1){
				fprintf(stderr, "Shmdt failed\n");
				return -1;
			}
			return 0;
		}
		else{
			printf("\nTry again!!\n");
			printf("\n[%d] ", pid[0]);
			continue;
		}
	}
}

void * thr_fn(void * arg){
	
	Msgtype msg_rcv;
	while(1){
		if(msgrcv(msq_id, &msg_rcv, TEXT, 2, 0) != -1){
			printf("\n\n***********private message***********\n");
			printf("[%d]%s\n[%d] ", msg_rcv.sender, msg_rcv.mtext, pid[0]);
			fflush(stdout);
		}
	}
}