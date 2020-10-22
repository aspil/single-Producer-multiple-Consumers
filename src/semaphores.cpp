#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/semaphores.h"

Semaphore::Semaphore(int m_num_sems, int val) {
    union semun sem_un;
    if ((this->m_id = semget(IPC_PRIVATE, m_num_sems, 0666 | IPC_CREAT)) == -1) {
        perror("Error in semget() called by Semaphore constructor : ");
        exit(EXIT_FAILURE);
    }

    sem_un.val = val;
    for(int i = 0; i < m_num_sems; i++){
        if (semctl(this->m_id, i, SETVAL, sem_un) == -1) {
            perror("Error in semctl() called by Semaphore constructor : ");
            exit(EXIT_FAILURE);
        }
    }
}

Semaphore::~Semaphore(){
    
}

void Semaphore::up(int sem_num){
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = 1;
    sem_b.sem_flg = 0;
    if (semop(this->m_id, &sem_b, 1) == -1) {
        perror("Error in semop() called by Semaphore::up(1) : ");
        exit(EXIT_FAILURE);
    }
}

void Semaphore::up(int sem_num, int val) {
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = val;
    sem_b.sem_flg = 0;
    if (semop(this->m_id, &sem_b, 1) == -1) {
        perror("Error in semop() called by Semaphore::up(2) : ");
        exit(EXIT_FAILURE);
    }
}

void Semaphore::down(int sem_num){
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = -1;
    sem_b.sem_flg = 0;

    if (semop(this->m_id, &sem_b, 1) == -1) {
        perror("Error in semop() called by Semaphore::down(1) : ");
        exit(EXIT_FAILURE);
    }
}
void Semaphore::down(int sem_num, int val) {
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = -val;
    sem_b.sem_flg = 0;

    if (semop(this->m_id, &sem_b, 1) == -1) {
        perror("Error in semop() called by Semaphore::down(2) : ");
        exit(EXIT_FAILURE);
    }
}
void Semaphore::waitforzero(int sem_num){
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = 0;
    sem_b.sem_flg = 0;

    if (semop(this->m_id, &sem_b, 1) == -1) {
		perror("Error in semop() called by Semaphore::waitforzero() : ");
        exit(EXIT_FAILURE);
	}
}
void Semaphore::clean() {
    if(semctl(this->m_id, this->m_num_sems, IPC_RMID,NULL) == -1) {
        perror("Error in semctl() called by Semaphore::clean() : ");
        exit(EXIT_FAILURE);
    }
}

