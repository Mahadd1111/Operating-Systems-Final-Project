//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "process.c"

void schedule(int);
bool yield(struct processControlBlock*);
bool wake_up(int);
bool terminate(struct processControlBlock*);
void context_switch(struct processControlBlock*,int,int);
bool preEmpt(int,float,struct processControlBlock* );
void force_preEmpt(int,struct processControlBlock*);
bool finished_io(int);
bool checkCPUDone();
void resetCPUStatus(int);

struct Node{
    struct Node* nextNode;
    struct processControlBlock* data; 
};

struct Node* Q1front;
struct Node* Q1rear;
struct Node* Q2front;
struct Node* Q2rear;
struct Node* Q3front;
struct Node* Q3rear;

void enFCFSQueue(struct processControlBlock* ptr){
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));
    temp->data=ptr;
    temp->nextNode=NULL;
    if(Q1rear==NULL){
        Q1front=temp;
        Q1rear=temp;
    }
    else{
        Q1rear->nextNode=temp;
        Q1rear=temp;
    }
}

struct processControlBlock* deFCFSQueue(){
    struct Node* temp=Q1front;
    if(Q1front==NULL){
        printf("Ready Queue1 is empty!\n");
    }
    else{
        struct processControlBlock* returnval=temp->data;
        Q1front=Q1front->nextNode;
        if(Q1front==NULL){Q1rear=NULL;}
        free(temp);
        return returnval;
    }
}

void enIOQueue(struct processControlBlock* ptr){
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));
    temp->data=ptr;
    temp->nextNode=NULL;
    if(Q3rear==NULL){
        Q3front=temp;
        Q3rear=temp;
    }
    else{
        Q3rear->nextNode=temp;
        Q3rear=temp;
    }
}

struct processControlBlock* deIOQueue(){
    struct Node* temp=Q3front;
    if(Q3front==NULL){
        printf("Ready Queue1 is empty!\n");
    }
    else{
        struct processControlBlock* returnval=temp->data;
        Q3front=Q3front->nextNode;
        if(Q3front==NULL){Q3rear=NULL;}
        free(temp);
        return returnval;
    }
}

void enPriorityQueue(struct processControlBlock* ptr){
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));
    temp->data=ptr;
    temp->nextNode=NULL;
    if(Q2rear==NULL){
        Q2front=Q2rear=temp;
    }
    else if(Q2front->data->priority<temp->data->priority){
        temp->nextNode=Q2front;
        Q2front=temp;
    }
    else{
        struct Node* currNode=(struct Node*)malloc(sizeof(struct Node));
        currNode=Q2front;
        while(currNode->nextNode && temp->data->priority<currNode->nextNode->data->priority){
            currNode=currNode->nextNode;
        }
        temp->nextNode=currNode->nextNode;
        currNode->nextNode=temp;
    }
}

struct processControlBlock* dePriorityQueue(){
    struct Node* temp=Q2front;
    if(Q2front==NULL){
        printf("Ready Queue2 is empty!\n");
    }
    else{
        struct processControlBlock* returnval=temp->data;
        Q2front=Q2front->nextNode;
        if(Q2front==NULL){Q2rear=NULL;}
        free(temp);
        return returnval;
    }
}

void displayPriorityQueue(){
    printf("\n\n");
    struct Node* temp=Q2front;
    while(temp){
        printf("%d--%s >>>  ",temp->data->priority,temp->data->name);
        temp=temp->nextNode;
    }
    printf("\n");
}

void QueueTesting(){
    printf("\n\n QUEUE TESTING: \n");
    enPriorityQueue(processes);
    enPriorityQueue(processes->nextProcess->nextProcess);
    enPriorityQueue(processes->nextProcess);
    enPriorityQueue(processes->nextProcess->nextProcess->nextProcess);
    enPriorityQueue(processes->nextProcess->nextProcess->nextProcess->nextProcess);
    displayPriorityQueue();
    dePriorityQueue();
    dePriorityQueue();
    dePriorityQueue();
    displayPriorityQueue();
}
