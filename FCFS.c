#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_HASHMAP_SIZE 100
#define MAX_NAME_LENGTH 50
#define MAX_INPUT_LENGTH 100
#define MAX_NUMBER_LENGTH 5

typedef enum
{
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED,
    STATE_KILLED
} ProcessState;

typedef struct PCB
{
    int processId;
    char processName[MAX_NAME_LENGTH];
    int arrivalTime;
    int burstTime;
    int remainingBurstTime;
    int currentBurstTime;
    int ioStartTime;
    int ioDurationTime;
    int currentIOTime;
    int remainingIOTime;
    int completionTime;
    ProcessState state;
    int executionTime;
    int ioJustStartedFlag;
    bool isKilled;
    struct PCB *next;
} PCB;

typedef struct QueueNode
{
    PCB *pcb;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

typedef struct KillEvent
{
    int processId;
    int killTime;
    struct KillEvent *next;
} KillEvent;

PCB *HashMap[MAX_HASHMAP_SIZE];
KillEvent *KillHead = NULL;

void InitializeQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

int HashFunction(int processId)
{
    int index = processId % MAX_HASHMAP_SIZE;
    if (index < 0)
    {
        index += MAX_HASHMAP_SIZE;
    }
    return index;
}

void InsertPCBInHash(PCB *pcb)
{
    int index = HashFunction(pcb->processId);
    pcb->next = HashMap[index];
    HashMap[index] = pcb;
}

PCB *GetPCBFromHash(int processId)
{
    int index = HashFunction(processId);
    PCB *currentPCB = HashMap[index];
    while (currentPCB != NULL)
    {
        if (currentPCB->processId == processId)
        {
            return currentPCB;
        }
        currentPCB = currentPCB->next;
    }
    return NULL;
}

void EnqueueProcess(Queue *queue, PCB *pcb)
{
    QueueNode *newNode = malloc(sizeof(QueueNode));
    if (newNode == NULL)
    {
        return;
    }
    newNode->pcb = pcb;
    newNode->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

PCB *DequeueProcess(Queue *queue)
{
    if (queue->front == NULL)
    {
        return NULL;
    }

    QueueNode *nodeToRemove = queue->front;
    PCB *pcb = nodeToRemove->pcb;
    queue->front = nodeToRemove->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(nodeToRemove);
    queue->size--;
    return pcb;
}

int RemovePCBFromQueue(Queue *queue, int processId)
{
    QueueNode *currentNode = queue->front;
    QueueNode *previousNode = NULL;

    while (currentNode != NULL)
    {
        if (currentNode->pcb->processId == processId)
        {
            if (previousNode == NULL)
            {
                queue->front = currentNode->next;
            }
            else
            {
                previousNode->next = currentNode->next;
            }

            if (currentNode == queue->rear)
            {
                queue->rear = previousNode;
            }

            free(currentNode);
            queue->size--;
            return 1;
        }

        previousNode = currentNode;
        currentNode = currentNode->next;
    }

    return 0;
}

PCB *CreateNewPCB(char *processName, int processId, int burstTime, int ioStartTime, int ioDurationTime)
{
    PCB *newPCB = malloc(sizeof(PCB));
    if (newPCB == NULL)
    {
        return NULL;
    }

    newPCB->processId = processId;
    strcpy(newPCB->processName, processName);
    newPCB->arrivalTime = 0;
    newPCB->burstTime = burstTime;
    newPCB->currentBurstTime = 0;
    newPCB->remainingBurstTime = burstTime;
    newPCB->ioStartTime = ioStartTime;
    newPCB->ioDurationTime = ioDurationTime;
    newPCB->remainingIOTime = 0;
    newPCB->currentIOTime = 0;
    newPCB->completionTime = 0;
    newPCB->executionTime = 0;
    newPCB->ioJustStartedFlag = 0;
    newPCB->isKilled = false;
    newPCB->state = STATE_NEW;
    newPCB->next = NULL;

    return newPCB;
}

bool IsValidInteger(char *value)
{
    int index = 0;
    while (value[index] != '\0')
    {
        if (!isdigit(value[index]))
        {
            return false;
        }
        index++;
    }
    return true;
}

bool IsDashOrInteger(char *value)
{
    if (strcmp(value, "-") == 0)
    {
        return true;
    }
    return IsValidInteger(value);
}

bool IsValidProcessName(char *processName)
{
    int index = 0;
    while (processName[index] == ' ')
    {
        index++;
    }
    if (processName[index] == '\0')
    {
        return false;
    }
    return true;
}

bool ValidateProcessInput(char *processName, char *inputProcessId, char *inputBurstTime, char *inputIOStart, char *inputIODuration)
{
    if (!IsValidProcessName(processName))
    {
        return false;
    }
    if (!IsValidInteger(inputProcessId))
    {
        return false;
    }
    if (!IsValidInteger(inputBurstTime))
    {
        return false;
    }
    if (!IsDashOrInteger(inputIOStart))
    {
        return false;
    }
    if (!IsDashOrInteger(inputIODuration))
    {
        return false;
    }
    return true;
}

bool ValidateKillInput(char *inputProcessId, char *inputTime)
{
    if (!IsValidInteger(inputProcessId))
    {
        return false;
    }
    if (!IsValidInteger(inputTime))
    {
        return false;
    }
    return true;
}

bool ValidateInputLine(char *inputLine)
{
    if (inputLine[0] == '\0')
    {
        return false;
    }
    return true;
}

void TakeProcessInput(int totalProcesses, Queue *readyQueue)
{
    int processIndex = 0;

    while (processIndex < totalProcesses)
    {
        char inputLine[MAX_INPUT_LENGTH];
        fgets(inputLine, sizeof(inputLine), stdin);
        inputLine[strcspn(inputLine, "\n")] = '\0';

        if (!ValidateInputLine(inputLine))
        {
            continue;
        }

        char processName[MAX_NAME_LENGTH];
        char inputProcessId[MAX_NUMBER_LENGTH];
        char inputBurstTime[MAX_NUMBER_LENGTH];
        char inputIOStart[MAX_NUMBER_LENGTH];
        char inputIODuration[MAX_NUMBER_LENGTH];

        int count = sscanf(inputLine, "%s %s %s %s %s",
                           processName,
                           inputProcessId,
                           inputBurstTime,
                           inputIOStart,
                           inputIODuration);

        if (count != 5)
        {
            continue;
        }

        if (!ValidateProcessInput(processName, inputProcessId, inputBurstTime, inputIOStart, inputIODuration))
        {
            continue;
        }

        int processId = atoi(inputProcessId);
        int burstTime = atoi(inputBurstTime);
        int ioStart = (strcmp(inputIOStart, "-") == 0) ? -1 : atoi(inputIOStart);
        int ioDuration = (strcmp(inputIODuration, "-") == 0) ? 0 : atoi(inputIODuration);

        PCB *newPCB = CreateNewPCB(processName, processId, burstTime, ioStart, ioDuration);
        newPCB->state = STATE_READY;
        InsertPCBInHash(newPCB);
        EnqueueProcess(readyQueue, newPCB);

        processIndex++;
    }
}

void AddKillEvent(int processId, int killTime)
{
    KillEvent *newNode = malloc(sizeof(KillEvent));
    if (newNode == NULL)
    {
        return;
    }

    newNode->processId = processId;
    newNode->killTime = killTime;
    newNode->next = NULL;

    if (KillHead == NULL || KillHead->killTime > killTime)
    {
        newNode->next = KillHead;
        KillHead = newNode;
        return;
    }

    KillEvent *eventIndex = KillHead;
    while (eventIndex->next != NULL && eventIndex->next->killTime <= killTime)
    {
        eventIndex = eventIndex->next;
    }

    newNode->next = eventIndex->next;
    eventIndex->next = newNode;
}

void TakeKillEventsInput(int totalKillEvents)
{
    int killIndex = 0;

    while (killIndex < totalKillEvents)
    {
        char inputLine[MAX_INPUT_LENGTH];
        fgets(inputLine, sizeof(inputLine), stdin);
        inputLine[strcspn(inputLine, "\n")] = '\0';

        if (!ValidateInputLine(inputLine))
        {
            continue;
        }

        char inputProcessId[MAX_NUMBER_LENGTH];
        char inputTime[MAX_NUMBER_LENGTH];

        int count = sscanf(inputLine, "KILL %s %s", inputProcessId, inputTime);
        if (count != 2)
        {
            continue;
        }

        if (!ValidateKillInput(inputProcessId, inputTime))
        {
            continue;
        }

        int processId = atoi(inputProcessId);
        int killTime = atoi(inputTime);
        AddKillEvent(processId, killTime);
        killIndex++;
    }
}

void ProcessKillEvents(int currentTime, Queue *readyQueue, Queue *waitingQueue, Queue *terminatedQueue, PCB **runningPCB)
{
    KillEvent *eventIndex = KillHead;
    KillEvent *previousEvent = NULL;

    while (eventIndex != NULL)
    {
        if (eventIndex->killTime == currentTime)
        {
            PCB *targetPCB = GetPCBFromHash(eventIndex->processId);
            if (targetPCB != NULL && !targetPCB->isKilled && targetPCB->state != STATE_TERMINATED)
            {
                targetPCB->state = STATE_KILLED;
                targetPCB->isKilled = true;
                targetPCB->completionTime = currentTime;

                RemovePCBFromQueue(readyQueue, targetPCB->processId);
                RemovePCBFromQueue(waitingQueue, targetPCB->processId);

                if (*runningPCB != NULL && (*runningPCB)->processId == targetPCB->processId)
                {
                    *runningPCB = NULL;
                }

                EnqueueProcess(terminatedQueue, targetPCB);
            }

            KillEvent *deleteNode = eventIndex;
            if (previousEvent == NULL)
            {
                KillHead = eventIndex->next;
                eventIndex = KillHead;
            }
            else
            {
                previousEvent->next = eventIndex->next;
                eventIndex = previousEvent->next;
            }

            free(deleteNode);
            continue;
        }

        previousEvent = eventIndex;
        eventIndex = eventIndex->next;
    }
}

void HandleIOTime(Queue *waitingQueue, Queue *readyQueue)
{
    QueueNode *nodeIndex = waitingQueue->front;
    QueueNode *previousNode = NULL;

    while (nodeIndex != NULL)
    {
        PCB *pcb = nodeIndex->pcb;
        QueueNode *nextNode = nodeIndex->next;

        if (pcb->ioJustStartedFlag)
        {
            pcb->ioJustStartedFlag = 0;
        }
        else if (pcb->remainingIOTime > 0)
        {
            pcb->remainingIOTime--;
        }

        if (pcb->remainingIOTime == 0)
        {
            if (!pcb->isKilled)
            {
                pcb->state = STATE_READY;
                EnqueueProcess(readyQueue, pcb);
            }

            if (previousNode == NULL)
            {
                waitingQueue->front = nodeIndex->next;
            }
            else
            {
                previousNode->next = nodeIndex->next;
            }

            if (nodeIndex == waitingQueue->rear)
            {
                waitingQueue->rear = previousNode;
            }

            free(nodeIndex);
            waitingQueue->size--;
            nodeIndex = nextNode;
            continue;
        }

        previousNode = nodeIndex;
        nodeIndex = nodeIndex->next;
    }
}

void ScheduleProcesses(Queue *readyQueue, Queue *waitingQueue, Queue *terminatedQueue)
{
    int currentTime = 0;
    PCB *runningPCB = NULL;

    while (readyQueue->size > 0 || waitingQueue->size > 0 || runningPCB != NULL)
    {
        ProcessKillEvents(currentTime, readyQueue, waitingQueue, terminatedQueue, &runningPCB);

        if (runningPCB == NULL && readyQueue->size > 0)
        {
            PCB *dequeuedPCB = DequeueProcess(readyQueue);
            if (dequeuedPCB != NULL && !dequeuedPCB->isKilled)
            {
                runningPCB = dequeuedPCB;
            }
            else if (dequeuedPCB != NULL)
            {
                EnqueueProcess(terminatedQueue, dequeuedPCB);
            }
        }

        if (runningPCB != NULL)
        {
            runningPCB->currentBurstTime++;
            runningPCB->executionTime++;
            runningPCB->remainingBurstTime--;

            if (runningPCB->currentBurstTime == runningPCB->ioStartTime && runningPCB->ioDurationTime > 0)
            {
                runningPCB->state = STATE_WAITING;
                runningPCB->remainingIOTime = runningPCB->ioDurationTime;
                runningPCB->currentIOTime = 0;
                runningPCB->ioJustStartedFlag = 1;

                EnqueueProcess(waitingQueue, runningPCB);
                runningPCB = NULL;
            }
            else if (runningPCB->remainingBurstTime <= 0)
            {
                runningPCB->completionTime = currentTime + 1;
                runningPCB->state = STATE_TERMINATED;
                EnqueueProcess(terminatedQueue, runningPCB);
                runningPCB = NULL;
            }
        }

        HandleIOTime(waitingQueue, readyQueue);
        currentTime++;
    }
}

void PrintFinalReport(Queue *terminatedQueue)
{
    QueueNode *nodeIndex = terminatedQueue->front;
    int totalPCB = 0;

    while (nodeIndex != NULL)
    {
        totalPCB++;
        nodeIndex = nodeIndex->next;
    }

    if (totalPCB == 0)
    {
        return;
    }

    PCB *pcbArray[totalPCB];
    nodeIndex = terminatedQueue->front;
    int pcbIndex = 0;
    while (nodeIndex != NULL)
    {
        pcbArray[pcbIndex++] = nodeIndex->pcb;
        nodeIndex = nodeIndex->next;
    }

    for (int turn = 0; turn < totalPCB - 1; turn++)
    {
        for (int index = 0; index < totalPCB - turn - 1; index++)
        {
            if (pcbArray[index]->processId > pcbArray[index + 1]->processId)
            {
                PCB *temp = pcbArray[index];
                pcbArray[index] = pcbArray[index + 1];
                pcbArray[index + 1] = temp;
            }
        }
    }

    printf("\n%-5s %-10s %-5s %-5s %-15s %-12s %-8s\n", "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");

    for (int pcbIndex = 0; pcbIndex < totalPCB; pcbIndex++)
    {
        PCB *pcb = pcbArray[pcbIndex];
        int cpuTime = pcb->burstTime;
        int ioTime = pcb->ioDurationTime;

        if (pcb->state == STATE_KILLED)
        {
            printf("%-5d %-10s %-5d %-5d KILLED at %-7d %-12s %-8s\n",
                   pcb->processId,
                   pcb->processName,
                   cpuTime,
                   ioTime,
                   pcb->completionTime,
                   "-",
                   "-");
        }
        else
        {
            int turnaroundTime = pcb->completionTime - pcb->arrivalTime;
            int waitingTime = turnaroundTime - cpuTime;
            printf("%-5d %-10s %-5d %-5d OK%-12s %-12d %-8d\n",
                   pcb->processId,
                   pcb->processName,
                   cpuTime,
                   ioTime,
                   "",
                   turnaroundTime,
                   waitingTime);
        }
    }
}

void FreeQueue(Queue *queue)
{
    QueueNode *nodeIndex = queue->front;
    while (nodeIndex != NULL)
    {
        QueueNode *nextNode = nodeIndex->next;
        free(nodeIndex);
        nodeIndex = nextNode;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

void FreeAllMemory(Queue *readyQueue, Queue *waitingQueue, Queue *terminatedQueue)
{
    for (int index = 0; index < MAX_HASHMAP_SIZE; index++)
    {
        PCB *pcbIndex = HashMap[index];
        while (pcbIndex != NULL)
        {
            PCB *nextPCB = pcbIndex->next;
            free(pcbIndex);
            pcbIndex = nextPCB;
        }
        HashMap[index] = NULL;
    }

    KillEvent *eventIndex = KillHead;
    while (eventIndex != NULL)
    {
        KillEvent *nextEvent = eventIndex->next;
        free(eventIndex);
        eventIndex = nextEvent;
    }

    KillHead = NULL;

    FreeQueue(readyQueue);
    FreeQueue(waitingQueue);
    FreeQueue(terminatedQueue);
}

int main()
{
    int totalProcesses = 0;
    int totalKillEvents = 0;

    Queue readyQueue;
    Queue waitingQueue;
    Queue terminatedQueue;

    InitializeQueue(&readyQueue);
    InitializeQueue(&waitingQueue);
    InitializeQueue(&terminatedQueue);

    for (int index = 0; index < MAX_HASHMAP_SIZE; index++)
    {
        HashMap[index] = NULL;
    }

    printf("Enter number of processes: ");
    scanf("%d", &totalProcesses);
    getchar();

    TakeProcessInput(totalProcesses, &readyQueue);

    printf("Enter number of kill events: ");
    scanf("%d", &totalKillEvents);
    getchar();

    if (totalKillEvents > 0)
    {
        TakeKillEventsInput(totalKillEvents);
    }

    ScheduleProcesses(&readyQueue, &waitingQueue, &terminatedQueue);

    PrintFinalReport(&terminatedQueue);

    FreeAllMemory(&readyQueue, &waitingQueue, &terminatedQueue);

    return 0;
}
