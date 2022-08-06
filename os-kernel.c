//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "os-kernel.h"

int main(int argc,char** argv){
    pthread_mutex_init(&mutex1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    printf("Number of command line arguments: %d\n",argc);
    argcount=argc;
    start(argv);
    displayProcesses();
    numContextSwitches=0;
    pthread_t tid;
    pthread_create(&tid,NULL,Controller,NULL);
    pthread_join(tid,NULL);
    displayProcesses();
    FILE* fptr;
    fptr=fopen("results.txt","a");
    fprintf(fptr,"Number of context Switches: %d \n",numContextSwitches);
    printf("Number of context Switches: %d \n",numContextSwitches);
    float avgTurn=findAvgTurnAround();
    float avgWait=findAvgWaitTime();
    fprintf(fptr,"Average Turn Around Time : %.1f\n",avgTurn);
    printf("Average Turn Around Time : %.1f\n",avgTurn);
    fprintf(fptr,"Average Waiting Time : %.1f\n",avgWait);
    printf("Average Waiting Time : %.1f\n",avgWait);
    fclose(fptr);
    return 0;
}