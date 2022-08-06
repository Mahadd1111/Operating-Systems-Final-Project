//MAHAD AHMED 20i0426 ---- ALISHBA ASIF 20i0582 ---- OPERATING SYSTEMS PROJECT
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>
#include<math.h>

int argcount=0;
int numOfCpu=0;
char inputFile[256];
char outputFile[256];
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int numOfProcesses=0;
char schedulingType[1];
float timeQuantum=0;
float timeInterval=0.0;
char resolveProcess[256];
bool cpuStatus[4];
int numContextSwitches;

struct processControlBlock{
    char name[256]; //Name of the Process
    int priority; //Priority of the Process
    float arrivalTime; //Arrival time of the Process
    char type[1]; //Whether this is an I/O bound or CPU bound Process
    float cpuTime; //Total Cpu burst time of the process left
    float cpuDone;//Time the Process has completed of its Cpu burst time
    int ioTime; //The number of times the process will go for I/O in its lifetime
    float* ioSendTimes; //The Cpu burst Times on which the Process will go for I/O
    float ioBurst; //The time Process will spend in the I/O each time
    float lastIoSentTime; // The time process was sent for I/O so that we can see if its I/O burst is complete
    char state[256]; //Current State of the Process
    float turnAroundTime; //Arrival Time - Terminating Time;
    float waitingTime; // time spent in the ready Queue;
    struct processControlBlock* nextProcess; //Linked list implementation of next processes
};

struct processControlBlock* processes;
void displayProcesses();
void removeLastProcess();
void setProcess();
void seperateFunction();
void readProcessesData();
void QueueTesting();
void setRemainingValues();
bool checkTerminateProgram();
bool compareFloat(float,float);
int start(char**);
char* findIOQueue(char*,int);
int findWaitingNum();
int findRunningNum();
int findReadyNum();
void setProcess(int,char[],int,struct processControlBlock*);
void seperateFunction(int,char[],struct processControlBlock*);
int removeDuplicateRandom(int ,struct processControlBlock*);
float findAvgTurnAround();
float findAvgWaitTime();

