//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include "process.h"
#include<time.h>

char* findIOQueue(char* buf,int len){
    struct processControlBlock* currProcess=processes;
    char val[256]="<<";
    while(currProcess){
        if(strcmp(currProcess->state,"WAITING")==0){
            strcat(val,currProcess->name);
            strcat(val,"<<");
        }
        currProcess=currProcess->nextProcess;
    }
    strncpy(buf,val,len);
    return buf;
}

int findWaitingNum(){
    struct processControlBlock* currProcess=processes;
    int count=0;
    while(currProcess){
        if(strcmp(currProcess->state,"WAITING")==0){
            count++;
        }
        currProcess=currProcess->nextProcess;
    }
    return count;
}

int findRunningNum(){
    struct processControlBlock* currProcess=processes;
    int count=0;
    while(currProcess){
        if(strcmp(currProcess->state,"RUNNING")==0){
            count++;
        }
        currProcess=currProcess->nextProcess;
    }
    return count;
}

int findReadyNum(){
    struct processControlBlock* currProcess=processes;
    int count=0;
    while(currProcess){
        if(strcmp(currProcess->state,"READY")==0){
            currProcess->waitingTime=currProcess->waitingTime+0.1;
            count++;
        }
        currProcess=currProcess->nextProcess;
    }
    return count;
}

void setProcess(int pnum,char a[],int num,struct processControlBlock* p){
    if(num==0){strcpy(p->name,a);}
    else if(num==1){p->priority=atoi(a);}
    else if(num==2){p->arrivalTime=(float)atoi(a);}
    else if(num==3){strcpy(p->type,a);}
    else if(num==4){
        if(strcmp(inputFile,"Processes1.txt")==0){
            p->cpuTime=(float)atoi(a);
        }
        else{
            int val=(rand()%10)+1;
            p->cpuTime=(float)val;
        }
    }
    else if(num==5){p->ioTime=atoi(a);}
}

void seperateFunction(int pnum,char a[],struct processControlBlock* p){
    int parameter=0;int j=0;
    for(int i=0;a[i]!='\0',i<strlen(a);i++){
        char word[256]="";
        while(1){
            if(a[i]=='\0'){
                break;
            }
            else if(a[i]!='\t'){
                word[j]=a[i];
                j++;
                i++;
            }
            else{
                break;
            }
        }
        while(a[i+1]==' '){
        i++;
        }   
        if(strlen(word)==0){
            continue;
        }
        else{
            setProcess(pnum,word,parameter,p);
        }
        parameter++;
        j=0;
    }
}

void removeLastProcess(){
    struct processControlBlock* c=processes;
    while(c->nextProcess->nextProcess){
        c=c->nextProcess;
    }
    struct processControlBlock* lastnode=c->nextProcess;
    c->nextProcess=NULL;
    free(lastnode);
}

bool checkExists(int n,struct processControlBlock* p){
    for(int i=0;i<p->ioTime;i++){
        if(p->ioSendTimes[i]==n){
            return true;
        }
    }
    return false;
}

int removeDuplicateRandom(int n,struct processControlBlock* p){
    bool flag=checkExists(n,p);
    while(flag){
        n=(rand()%((int)p->cpuTime))+1;
        flag=checkExists(n,p);
    }
    return n;
}

void setRemainingValues(){
    struct processControlBlock* currProcess=processes;
    srand((unsigned)time(0));
    while(currProcess){
        //For all processes set New state, if I/O process then set send times + I/O burst
        strcpy(currProcess->state,"NEW");
        if(strcmp(inputFile,"Processes2.txt")==0){
            int val1=rand()%10;
            currProcess->cpuTime=(float)val1;
            if(strcmp(currProcess->type,"I")==0){
                int val2=(rand()%2)+1;
                currProcess->ioTime=val2;
            }
            else{
                currProcess->ioTime=-1;
            }
        }
        if(strcmp(currProcess->type,"I")==0){
            currProcess->ioSendTimes=(float*)malloc(currProcess->ioTime * sizeof(float));
            for(int j=0;j<currProcess->ioTime;j++){
                int val3=(rand()%((int)currProcess->cpuTime))+1;
                val3=removeDuplicateRandom(val3,currProcess);
                currProcess->ioSendTimes[j]=(float)val3;
            }
            currProcess->ioBurst=2.0;
        }
        currProcess=currProcess->nextProcess;
    }
}

void readProcessesData(){
    processes=(struct processControlBlock*)malloc(sizeof(struct processControlBlock));
    struct processControlBlock* currProcess=processes;
    FILE* fptr;
    int pnum=0;
    fptr=fopen(inputFile,"r");
    char line[256];
    if(!fptr){printf("unable to open file\n");return;}
    fgets(line,sizeof(line),fptr);
    while(fgets(line,sizeof(line),fptr)){
        seperateFunction(pnum,line,currProcess);
        pnum++;
        currProcess->nextProcess=(struct processControlBlock*)malloc(sizeof(struct processControlBlock));
        currProcess=currProcess->nextProcess;
    }
    removeLastProcess();
    numOfProcesses=pnum;
    setRemainingValues();
}

void displayProcesses(){
    struct processControlBlock* currentProcess=processes;
    int i=0;
    while(currentProcess){
        printf("Process # %d :\n",i);
        printf("Process Name: %s\n",currentProcess->name);
        printf("Process State: %s\n",currentProcess->state);
        printf("Process Priority: %d\n",currentProcess->priority);
        printf("Process Arrival time: %f\n",currentProcess->arrivalTime);
        printf("Process type: %s\n",currentProcess->type);
        printf("Process cpu Time: %f\n",currentProcess->cpuTime);
        printf("Process IO time: %d\n",currentProcess->ioTime);
        printf("Process completed cpu time: %f\n",currentProcess->cpuDone);
        printf("Process IO burst time: %f\n",currentProcess->ioBurst);
        printf("Process TurnAround Time is: %f\n",currentProcess->turnAroundTime);
        printf("Process Waiting Time is: %f\n",currentProcess->waitingTime);
        printf("Process will go to I/O at times: \n");
        if(strcmp(currentProcess->type,"I")==0){
            for(int j=0;j<currentProcess->ioTime;j++){
                printf("Time : %f\n",currentProcess->ioSendTimes[j]);
            }
        }
        printf("------------------------------------------------------------------------------------------\n");
        currentProcess=currentProcess->nextProcess;
        i++;
    }
}

int start(char** a){
    for(int i=0;i<argcount;i++){
        if(i==2){strcpy(inputFile,a[i]);}
        else if(i==3){numOfCpu=atoi(a[i]);}
        else if(i==4){strcpy(schedulingType,a[i]);}
        else if(i==5){
            if(strcmp(schedulingType,"r")==0){
                timeQuantum=atoi(a[i]);
                printf("TIME QUANTUM IS %f\n",timeQuantum);
                strcpy(outputFile,a[i+1]);
            }
            else{
                strcpy(outputFile,a[i]);
            }
        }
    }
    readProcessesData();
    printf("Number of processes: %d\n",numOfProcesses);
}

bool checkTerminateProgram(){
    struct processControlBlock* currProcess=processes;
    while(currProcess){
        //If any Process has a state which is not terminated, return true
        if(strcmp(currProcess->state,"TERMINATED")!=0){
            return true; 
        }
        currProcess=currProcess->nextProcess;
    }
    return false;
}

bool compareFloat(float a, float b){
    if(fabs(a-b)<1e-2){
        return true;
    }
    else{
        return false;
    }
}

float findAvgTurnAround(){
    struct processControlBlock* currProcess=processes;
    float sum=0;
    while(currProcess){
        sum=sum+currProcess->turnAroundTime;
        currProcess=currProcess->nextProcess;
    }
    return (sum/(float)numOfProcesses);
}

float findAvgWaitTime(){
    struct processControlBlock* currProcess=processes;
    float sum=0;
    while(currProcess){
        sum=sum+currProcess->waitingTime;
        currProcess=currProcess->nextProcess;
    }
    return (sum/(float)numOfProcesses);
}