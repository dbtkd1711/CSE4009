#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define SHM_KEY 1711
#define MSQ_KEY_1 1
#define MSQ_KEY_2 2
#define MSQ_KEY_3 3
#define TEXT 1024
#define TYPE 4
#define MSG_SIZE (sizeof(Msgtype))

typedef struct{
	long mtype;
	pid_t sender;
	pid_t receiver;
	char mtext[1024];
}Msgtype;

int main(){

	int shm_id, msq_id[3];
	void * shm_addr;

	if((shm_id = shmget(SHM_KEY, MSG_SIZE*100+20, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "shmget failed\n");
		return -1;
	}
	if((shm_addr = shmat(shm_id, 0, 0)) == -1){
		fprintf(stderr, "shmat failed\n");
		return -1;
	}


	if((msq_id[0] = msgget(MSQ_KEY_1, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "msgget failed\n");
		return -1;
	}
	if((msq_id[1] = msgget(MSQ_KEY_2, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "msgget failed\n");
		return -1;
	}
	if((msq_id[2] = msgget(MSQ_KEY_3, IPC_CREAT|0666)) == -1){
		fprintf(stderr, "msgget failed\n");
		return -1;
	}

	pid_t * pid = shm_addr;
	int * count = shm_addr + 12;
	int * cycle = shm_addr + 16;

	* count = 0;
	* cycle = 0;

	int command;
	Msgtype msg;

	while(1){
		//printf("count : %d\n", *count);
		printf("1. All clients opened\n");
		printf("2. Quit the server\n");
		printf("Command : ");
		scanf("%d", &command);

		if(command == 1){
			while(1){
				if(msgrcv(msq_id[0], &msg, TEXT, 1, IPC_NOWAIT) != -1){
					if(msg.receiver==0){
						// 1. Number of chatting log < 100
						if((*count)!=100){
							memcpy(shm_addr+20+1028*(*count), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*count), &msg.mtext, TEXT);
							(*count)++;
						}
						// 2. Number of chatting log = 100
						// -> replace oldest log with new thing
						else{
							memcpy(shm_addr+20+1028*(*cycle), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*cycle), &msg.mtext, TEXT);
							(*cycle)++;
							if((*cycle)==100)
								(*cycle) = 0;
						}
						//printf("count : %d\n", *count);
					}
					else if(msg.receiver==pid[0]){
						msg.mtype = 2;
						msgsnd(msq_id[0], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[0]);
					}
					else if(msg.receiver==pid[1]){
						msg.mtype = 2;
						msgsnd(msq_id[1], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[1]);	
					}
					else{
						msg.mtype = 2;
						msgsnd(msq_id[2], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[2]);
					}
				}
				if(msgrcv(msq_id[1], &msg, TEXT, 1, IPC_NOWAIT) != -1){
					if(msg.receiver==0){
						if((*count)!=100){
							memcpy(shm_addr+20+1028*(*count), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*count), &msg.mtext, TEXT);
							(*count)++;
						}
						else{
							memcpy(shm_addr+20+1028*(*cycle), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*cycle), &msg.mtext, TEXT);
							(*cycle)++;
							if((*cycle)==100)
								(*cycle) = 0;
						}
						//printf("count : %d\n", *count);
					}
					else if(msg.receiver==pid[0]){
						msg.mtype = 2;
						msgsnd(msq_id[0], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[0]);
					}
					else if(msg.receiver==pid[1]){
						msg.mtype = 2;
						msgsnd(msq_id[1], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[1]);	
					}
					else{
						msg.mtype = 2;
						msgsnd(msq_id[2], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[2]);
					}
				}
				if(msgrcv(msq_id[2], &msg, TEXT, 1, IPC_NOWAIT) != -1){
					if(msg.receiver==0){
						if((*count)!=100){
							memcpy(shm_addr+20+1028*(*count), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*count), &msg.mtext, TEXT);
							(*count)++;
						}
						else{
							memcpy(shm_addr+20+1028*(*cycle), &msg.sender, sizeof(pid_t));
							memcpy(shm_addr+24+1028*(*cycle), &msg.mtext, TEXT);
							(*cycle)++;
							if((*cycle)==100)
								(*cycle) = 0;
						}
						//printf("count : %d\n", *count);
					}
					else if(msg.receiver==pid[0]){
						msg.mtype = 2;
						msgsnd(msq_id[0], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[0]);
					}
					else if(msg.receiver==pid[1]){
						msg.mtype = 2;
						msgsnd(msq_id[1], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[1]);	
					}
					else{
						msg.mtype = 2;
						msgsnd(msq_id[2], &msg, TEXT, IPC_NOWAIT);
						printf("Send a message to %d success\n", pid[2]);
					}
				}
				continue;
			}
		}
		else if(command == 2){
			if(shmdt(shm_addr) == -1){
				fprintf(stderr, "Shmdt failed\n");
				return -1;
			}
			return 0;
		}
		else{
			printf("Try again\n");
			continue;
		}
	}
	
	return 0;
}