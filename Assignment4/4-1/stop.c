#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

struct sigaction act_new;
void sig_fn(int signo);

int main(){
	act_new.sa_handler = sig_fn;
	sigemptyset(&act_new.sa_mask);

	if(sigaction(SIGINT, &act_new, NULL)<0)
		printf("sigaction error\n");

	while(1)
		pause();
}

void sig_fn(int signo){
	printf("\nCtrl-C is pressed. Try Again\n");
	printf("Ctrl-C generates number %d\n", signo);
}
