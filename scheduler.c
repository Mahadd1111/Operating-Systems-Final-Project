//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "scheduler.h"

void schedule(int cpuNum){
    if(strcmp("f",schedulingType)==0){
        if(Q1front!=NULL){
            pthread_mutex_trylock(&mutex1);
            struct processControlBlock* runProcess=deFCFSQueue();
            strcpy(runProcess->state,"RUNNING");
            pthread_mutex_unlock(&mutex1);
            context_switch(runProcess,-1,cpuNum);
        }
        else{
            context_switch(NULL,1,cpuNum);
        }
    }
    else if(strcmp("r",schedulingType)==0){
        if(Q1front!=NULL){
            pthread_mutex_trylock(&mutex1);
            struct processControlBlock* runProcess=deFCFSQueue();
            strcpy(runProcess->state,"RUNNING");
            pthread_mutex_unlock(&mutex1);
            context_switch(runProcess,timeQuantum,cpuNum);
        }
        else{
            context_switch(NULL,1,cpuNum);
        }
    }
    else if(strcmp("p",schedulingType)==0){
        if(Q2front!=NULL){
            pthread_mutex_trylock(&mutex1);
            struct processControlBlock* runProcess=dePriorityQueue();
            strcpy(runProcess->state,"RUNNING");
            pthread_mutex_unlock(&mutex1);
            context_switch(runProcess,-1,cpuNum);
        }
        else{
            context_switch(NULL,1,cpuNum);
        }
    }
}

bool preEmpt(int timeSlice,float startTime,struct processControlBlock* p){
    if(timeSlice==-1 && (strcmp("f",schedulingType)==0 || strcmp("p",schedulingType)==0)){
        //if it is FCFS then there is no need to pre-empt
        return true;
    }
    else{
        if(compareFloat(timeInterval-startTime,(float)timeSlice)==1 && strcmp("r",schedulingType)==0 && p!=NULL){
            //If the time slice for this process is complete then pre-empt
            pthread_mutex_trylock(&mutex1);
            strcpy(p->state,"READY");
            enFCFSQueue(p);
            pthread_mutex_unlock(&mutex1);
            return false;
        }
        else{
            return true;
        }
    }
}

bool terminate(struct processControlBlock* p){
    if(compareFloat(p->cpuTime,(float)0)==1){
        //if there is no remaining cpuTime needed then terminate this process
        if(strcmp(p->state,"TERMINATED")!=0){
            p->turnAroundTime=timeInterval-p->arrivalTime;
        }
        strcpy(p->state,"TERMINATED");
        return true;
    }
    return false;
}

bool yield(struct processControlBlock* p){
    for(int i=0;i<p->ioTime;i++){
        if(compareFloat(p->cpuDone,p->ioSendTimes[i])==1 && strcmp(p->type,"I")==0){
            terminate(p);
            if(strcmp(p->state,"TERMINATED")!=0){
                //it is time for this process to go to I/O
                strcpy(p->state,"WAITING");
                p->ioSendTimes[i]=0;
                p->lastIoSentTime=timeInterval;
                return true;
            }
        }
    }
    return false;
}


void force_preEmpt(int cpuNum,struct processControlBlock* p){
    bool flag=false;
    struct processControlBlock* currProcess=processes;
    while(currProcess){
        if(currProcess->priority>p->priority){
            flag=true;
        }
        currProcess=currProcess->nextProcess;    
    }
    if(flag==true){
        // we must pre-empt it by moving this to ready Queue and bringing in the new process from the ready Queue
        pthread_mutex_trylock(&mutex1);
        strcpy(p->state,"READY");
        enPriorityQueue(p);
        schedule(cpuNum);
        pthread_mutex_unlock(&mutex1);
    }
}

bool wake_up(int cpuNum){
    bool flag=false;
    struct processControlBlock* currProcess=processes;
    while(currProcess){
        if(compareFloat(currProcess->arrivalTime,timeInterval) && strcmp(currProcess->state,"NEW")==0){
            //A processes arrival time has come
            if(strcmp("f",schedulingType)==0 || strcmp("r",schedulingType)==0){
                strcpy(currProcess->state,"READY");
                pthread_mutex_trylock(&mutex1);
                enFCFSQueue(currProcess);
                flag=true;
                pthread_mutex_unlock(&mutex1);
            }
            else if(strcmp("p",schedulingType)==0){
                strcpy(currProcess->state,"READY");
                pthread_mutex_trylock(&mutex1);
                enPriorityQueue(currProcess);
                flag=true;
                pthread_mutex_unlock(&mutex1);
            }
        }
        currProcess=currProcess->nextProcess;
    }
    return flag;
}

bool finished_io(int cpuNum){
    bool flag=false;
    struct processControlBlock* currProcess=processes;
    while(currProcess){
        if(compareFloat(timeInterval,currProcess->lastIoSentTime+2)==1 && strcmp(currProcess->state,"WAITING")==0 && strcmp(currProcess->type,"I")==0){
            //Process has completed its I/O and must be sent back to ready queue
            terminate(currProcess);
            if(strcmp(currProcess->state,"TERMINATED")!=0 && (strcmp("r",schedulingType)==0 || strcmp("f",schedulingType)==0)){
                strcpy(currProcess->state,"READY");
                pthread_mutex_trylock(&mutex1);
                enFCFSQueue(currProcess);
                flag=true;
                pthread_mutex_unlock(&mutex1);
            }
            else if(strcmp(currProcess->state,"TERMINATED")!=0 && strcmp("p",schedulingType)==0){
                strcpy(currProcess->state,"READY");
                pthread_mutex_trylock(&mutex1);
                enPriorityQueue(currProcess);
                flag=true;
                pthread_mutex_unlock(&mutex1);
            }
        }
        currProcess=currProcess->nextProcess;
    }
    return flag;
}

bool checkCPUDone(){
    for(int j=0;j<numOfCpu;j++){
        if(cpuStatus[j]==0){
            // if there is a single cpu that has not finished execution
            return false;
        }
    }
    return true;
}

void resetCPUStatus(int cpuNum){
    for(int k=0;k<numOfCpu;k++){
        cpuStatus[k]=false;
    }
}