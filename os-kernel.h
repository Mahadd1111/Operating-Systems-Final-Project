//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "scheduler.c"

bool resetStatus;

//Function to Synchronise CPUs so that all wait till the rest have executed, the last one then increments time and continues program
bool synchroniseCPU(float tempTime,int cpuNum,struct processControlBlock* p){
    cpuStatus[cpuNum]=true;
    while(!checkCPUDone()){

    }
    if(resetStatus==true){
        return false;
    }
    if(compareFloat((float)tempTime,timeInterval)==1 && resetStatus==false){
        timeInterval=timeInterval+0.1;
        resetStatus=true;
        resetCPUStatus(cpuNum);
        bool check1=wake_up(cpuNum);
        bool check2=finished_io(cpuNum);
        if((check1==true || check2==true) && strcmp("p",schedulingType)==0){
            force_preEmpt(cpuNum,p);
        }
    }
}

//function to replicate what is happening on a cpu
void context_switch(struct processControlBlock* p,int timeSlice,int cpuNum){
    if(p!=NULL){
        numContextSwitches++;
    }
    float startTime=timeInterval;
    while(preEmpt(timeSlice,startTime,p)){
        pthread_mutex_trylock(&mutex2);
        resetStatus=false;
        if(p!=NULL){
            if(terminate(p)==1){
                schedule(cpuNum);
                return;
            }
            if(yield(p)==1){
                schedule(cpuNum);
                return;
            }
            int p_running=findRunningNum();
            int p_waiting=findWaitingNum();
            int p_ready=findReadyNum();
            char p_io[256];
            FILE* fptr;
            fptr=fopen("results.txt","a");
            fprintf(fptr,"%.1f\t%d\t%d\t%d\t%s\t%s\n",timeInterval,p_running,p_ready,p_waiting,p->name,findIOQueue(p_io,256));
            fclose(fptr);
            printf("%.1f\t%d\t%d\t%d\t%s\t%s\n",timeInterval,p_running,p_ready,p_waiting,p->name,findIOQueue(p_io,256));
            p->cpuDone=p->cpuDone+0.1;
            if(p->cpuTime>0){
                p->cpuTime=p->cpuTime-0.1;
            }
        }
        else{
            if(Q1front!=NULL){
                schedule(cpuNum);
                return;
            }
            int p_running=findRunningNum();
            int p_waiting=findWaitingNum();
            int p_ready=findReadyNum();
            char p_io[256];
            FILE* fptr;
            fptr=fopen("results.txt","a");
            fprintf(fptr,"%.1f%d\t%d\t%d\t(IDLE)\t%s\n",timeInterval,p_running,p_waiting,p_ready,findIOQueue(p_io,256));
            fclose(fptr);
            printf("%.1f%d\t%d\t%d\t(IDLE)\t%s\n",timeInterval,p_running,p_waiting,p_ready,findIOQueue(p_io,256));
            if(!checkTerminateProgram()){
                return;
            }
        }
        float tempTime=timeInterval;
        synchroniseCPU(tempTime,cpuNum,p);
        pthread_mutex_unlock(&mutex2);
    }
}

void* CPU(void* args){
    //Find the CPU number
    int cpuNum=*((int*)args);
    while(checkTerminateProgram()){
        wake_up(cpuNum);
        schedule(cpuNum);
        finished_io(cpuNum);
    }
    pthread_exit(NULL);
}

void* Controller(void* args){
    timeInterval=0.0;
    pthread_t cpu_threads[numOfCpu];
    if(numOfCpu==1){
        FILE* fptr;
        fptr=fopen("results.txt","w");
        fprintf(fptr,"Time\tRu\tRe\tWa\tCpu0\tI/O Queue\n");
        fprintf(fptr,"====\t==\t==\t==\t====\t=========\n");
        fclose(fptr);
        printf("Time\tRu\tRe\tWa\tCpu0\tI/O Queue\n");
        printf("====\t==\t==\t==\t====\t=========\n");
        int a=0;
        pthread_create(&cpu_threads[0],NULL,CPU,&a);
    }
    else if(numOfCpu==2){
        printf("Time\tRu\tRe\tWa\tCpu0\tCpu1\tI/O Queue\n");
        printf("====\t==\t==\t==\t====\t====\t=========\n");
        int a=0;
        int b=1;
        pthread_create(&cpu_threads[0],NULL,CPU,&a);
        pthread_create(&cpu_threads[1],NULL,CPU,&b);
    }
    else if(numOfCpu==4){
        printf("Time\tRu\tRe\tWa\tCpu0\tCpu1\tCpu2\tCpu3\tI/O Queue\n");
        printf("====\t==\t==\t==\t====\t====\t====\t====\t=========\n");
        int a=0;
        int b=1;
        int c=2;
        int d=3;
        pthread_create(&cpu_threads[0],NULL,CPU,&a);
        pthread_create(&cpu_threads[1],NULL,CPU,&b);
        pthread_create(&cpu_threads[2],NULL,CPU,&c);
        pthread_create(&cpu_threads[3],NULL,CPU,&d);
    }
    while(checkTerminateProgram()){
        //Our SPARK OS Keeps Running 
    }
    pthread_exit(NULL);
}
