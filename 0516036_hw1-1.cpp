#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
	pid_t getpid(void); // ¨ú±oprocess ID
	int pid = fork();
	if (pid) {//parent process
		printf("This is Parent process PID = %d, waiting for my child\n", getpid());
		int temp = wait(NULL);
		printf("This is Parent process, catch my child PID =  %d\n", pid);
	}
	else {//child process
		printf("This is Child process PID = %d, My parent's PID = %d\n", getpid(), getppid());
	}	
	exit(0);
	return 0;
}
