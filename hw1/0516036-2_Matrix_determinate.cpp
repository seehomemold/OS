#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>

#define SharedSize 4096
#define PARM IPC_CREAT|0666
using namespace std;

int determinantsol(int a, int Matrix[15][15]);
void * attachmem();
void one_process();
void two_process();
void three_process();
void four_process();
int a = 0;
key_t key = 0;
int shmid;
int main(int argc, char *argv[]) {
	/*create the memory*/
	shmid = shmget(key, SharedSize, IPC_PRIVATE | 0666);//IPC_CREAT | 0666
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}
	/*write into the sharedmemory*/
	int* shm, *temp;
	shm = (int*)attachmem();

	/*read the file*/
	ifstream myReadFile;
	myReadFile.open("input.txt");
	myReadFile >> a;
	if (myReadFile.is_open()) {
		for (int i = 0; i < a; i++) {
			for (int j = 0; j < a; j++) {
				myReadFile >> (*shm);
				shm++;
			}
		}
		myReadFile.close();
	}
	one_process();
	two_process();
	three_process();
	four_process();
	return 0;
}

void* attachmem() {
	int* shm;
	if ((shm = (int *)shmat(shmid, NULL, 0)) == (int *)-1) {//0: read and write
		perror("shmat");
		exit(1);
	}
	return shm;
}
int determinantsol(int a, int Matrix[15][15]) {
	double sum = 0;
	int Matrixitem[15][15];
	int i = 0, p = 0, q = 0;
	if (a == 1) {
		sum = Matrix[0][0];
	}
	else{
		for (i = 0; i < a; i++) {//這邊做降階
			for (p = 0; p < a - 1; p++) {
				for (q = 0; q < a - 1; q++) {
					if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
					if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
				}
			}
			sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
		}
	}
	return sum;
}
void one_process() {
	printf("Calculating determinant using 1 process\n");
	time_t startTime = 0;
	int sum = 0;
	startTime = time(NULL);
	/*attatach the memory to read the array*/
	int* shm, *temp;
	shm = (int*)attachmem();
	temp = shm;
	/*shm is the pointer to the shared memory*/
	/*start to load data into a [15][15]matrix*/
	int Matrix1[15][15];
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < a; j++) {
			Matrix1[i][j]= (*shm);
			shm++;
		}
	}
	/*go calculate*/
	sum = determinantsol(a, Matrix1);//算行列值
	cout << "Elapsed time: " << time(NULL) - startTime << " sec, ";
	cout << "determinant : " << sum << endl;
}
void two_process() {
	printf("Calculating determinant using 2 process\n");
	time_t startTime = 0;
	int sum = 0;
	startTime = time(NULL);
	/*attatach the memory to read the array*/
	int* shm, *temp;
	shm = (int*)attachmem();
	/*shm is the pointer to the shared memory*/
	/*start to load data into a [15][15]matrix*/
	int Matrix[15][15];
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < a; j++) {
			Matrix[i][j] = (*shm);
			shm++;
		}
	}
	int pid = fork();
	if (pid == -1)cout << "fuckyou";
	if (pid) {//parent process
		int Matrixitem[15][15];
		for (int i = 0; i < a; i=i+2) {//這邊做降階,2i
			for (int p = 0; p < a - 1; p++) {
				for (int q = 0; q < a - 1; q++) {
					if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
					if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
				}
			}
			sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
		}
		wait(NULL);
	}
	else {//child process
		int Matrixitem[15][15];
		for (int i = 1; i < a; i = i + 2) {//這邊做降階,1+2*i
			for (int p = 0; p < a - 1; p++) {
				for (int q = 0; q < a - 1; q++) {
					if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
					if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
				}
			}
			sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
		}
		(*shm) = sum;
		exit(1);
	}
	sum += (*shm);
	/*go calculate*/
	(*shm) = 0;
	cout << "Elapsed time: " << time(NULL) - startTime << " sec, ";
	cout << "determinant : " << sum << endl;
}
void three_process() {
	printf("Calculating determinant using 3 process\n");
	time_t startTime = 0;
	int sum = 0;
	startTime = time(NULL);
	/*attatach the memory to read the array*/
	int* shm;
	shm = (int*)attachmem();
	/*shm is the pointer to the shared memory*/
	/*start to load data into a [15][15]matrix*/
	int Matrix[15][15];
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < a; j++) {
			Matrix[i][j] = (*shm);
			shm++;
		}
	}
	int pid = fork();
	if (pid) {//parent process
		int Matrixitem[15][15];
		for (int i = 0; i < a; i = i + 3) {//這邊做降階,2i
			for (int p = 0; p < a - 1; p++) {
				for (int q = 0; q < a - 1; q++) {
					if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
					if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
				}
			}
			sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
		}
		wait(NULL);
	}
	else {//child process
		int ppid = fork();
		if (ppid) {//this process
			int Matrixitem[15][15];
			for (int i = 1; i < a; i = i + 3) {//這邊做降階,1+2*i
				for (int p = 0; p < a - 1; p++) {
					for (int q = 0; q < a - 1; q++) {
						if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
						if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
					}
				}
				sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
			}
			wait(NULL);
			(*shm) = sum;
			exit(1);
		}
		else {//grand child process
			int Matrixitem[15][15];
			for (int i = 2; i < a; i = i + 3) {//這邊做降階,1+2*i
				for (int p = 0; p < a - 1; p++) {
					for (int q = 0; q < a - 1; q++) {
						if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
						if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
					}
				}
				sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
			}
			shm += 1;
			(*shm) = sum;
			exit(1);
		}
		
	}
	/*go calculate*/
	sum += (*shm);
	shm++;
	sum += (*shm);
	cout << "Elapsed time: " << time(NULL) - startTime << " sec, ";
	cout << "determinant : " << sum << endl;
}
void four_process() {
	printf("Calculating determinant using 4 process\n");
	time_t startTime = 0;
	int sum = 0;
	startTime = time(NULL);
	/*attatach the memory to read the array*/
	int* shm;
	shm = (int*)attachmem();
	/*shm is the pointer to the shared memory*/
	/*start to load data into a [15][15]matrix*/
	int Matrix[15][15];
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < a; j++) {
			Matrix[i][j] = (*shm);
			shm++;
		}
	}
	int pid = fork();
	if (pid) {//parent process
		int Matrixitem[15][15];
		for (int i = 0; i < a; i = i + 4) {//這邊做降階,2i
			for (int p = 0; p < a - 1; p++) {
				for (int q = 0; q < a - 1; q++) {
					if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
					if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
				}
			}
			sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
		}
		wait(NULL);
	}
	else {//child2 process
		int ppid = fork();
		if (ppid) {
			int Matrixitem[15][15];
			for (int i = 1; i < a; i = i +4) {//這邊做降階,2+4*i
				for (int p = 0; p < a - 1; p++) {
					for (int q = 0; q < a - 1; q++) {
						if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
						if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
					}
				}
				sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
			}
			wait(NULL);
			(*shm) = sum;
			exit(1);
		}
		else {//child3 process
			int pppid = fork();
			if (pppid) {//this process
				int Matrixitem[15][15];
				for (int i = 2; i < a; i = i + 4) {//這邊做降階,2+4*i
					for (int p = 0; p < a - 1; p++) {
						for (int q = 0; q < a - 1; q++) {
							if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
							if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
						}
					}
					sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
				}
				wait(NULL);
				shm += 1;
				(*shm) = sum;
				exit(1);
			}
			else {//child4 process
				int Matrixitem[15][15];
				for (int i = 3; i < a; i = i + 4) {//這邊做降階,3+4i
					for (int p = 0; p < a - 1; p++) {
						for (int q = 0; q < a - 1; q++) {
							if (q < i) Matrixitem[p][q] = Matrix[p + 1][q];
							if (q >= i) Matrixitem[p][q] = Matrix[p + 1][q + 1];
						}
					}
					sum = sum + (pow((double)-1, i % 2) * Matrix[0][i] * determinantsol(a - 1, Matrixitem));//1和-1
				}
				shm += 2;
				(*shm) = sum;
				exit(1);
			}

		}
	}
	sum += (*shm);
	shm++;
	sum += (*shm);
	shm++;
	sum += (*shm);
	cout << "Elapsed time: " << time(NULL) - startTime << " sec, ";
	cout << "determinant : " << sum << endl;
}