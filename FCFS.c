#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_MAX 64
#define HASHMAP_SIZE 1024

typedef enum
{
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED,
    STATE_KILLED
} ProcessState;

typedef struct ProcessControlBlock
{
    char processName[NAME_MAX];
    int pid;
    int totalCpuBurst;
    int remainingCpuBurst;
    int ioStartAfterCpu;
    int ioDuration;
    int remainingIo;
    int cpuExecuted;
    int actualIoTime;
    int arrivalTime;
    int completionTime;
    int waitingTime;
    ProcessState state;
    int killedAtTime;
    struct ProcessControlBlock *nextInHash;
} ProcessControlBlock;

typedef struct Node
{
    ProcessControlBlock *pcb;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *head;
    Node *tail;
    int length;
} Queue;

typedef struct KillEvent
{
    int pid;
    int killTime;
} KillEvent;

ProcessControlBlock *hashmap[HASHMAP_SIZE];

unsigned int hashPid(int pid)
{
    return (unsigned int)pid % HASHMAP_SIZE;
}

void hashmapPut(ProcessControlBlock *pcb)
{
    unsigned int index = hashPid(pcb->pid);
    pcb->nextInHash = hashmap[index];
    hashmap[index] = pcb;
}

ProcessControlBlock *hashmapGet(int pid)
{
    unsigned int index = hashPid(pid);
    ProcessControlBlock *cursor = hashmap[index];
    while (cursor != NULL)
    {
        if (cursor->pid == pid)
        {
            return cursor;
        }
        cursor = cursor->nextInHash;
    }
    return NULL;
}

void initQueue(Queue *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;
}

int enqueue(Queue *queue, ProcessControlBlock *pcb)
{
    Node *node = malloc(sizeof(Node));
    if (!node)
    {
        return 0;
    }
    node->pcb = pcb;
    node->next = NULL;
    if (!queue->tail)
    {
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->length++;
    return 1;
}

ProcessControlBlock *dequeue(Queue *queue)
{
    if (!queue->head)
    {
        return NULL;
    }
    Node *node = queue->head;
    ProcessControlBlock *pcb = node->pcb;
    queue->head = node->next;
    if (!queue->head)
    {
        queue->tail = NULL;
    }
    free(node);
    queue->length--;
    return pcb;
}

int removeFromQueueByPid(Queue *queue, int pid)
{
    Node *previous = NULL;
    Node *cursor = queue->head;
    while (cursor)
    {
        if (cursor->pcb->pid == pid)
        {
            if (!previous)
            {
                queue->head = cursor->next;
            }
            else
            {
                previous->next = cursor->next;
            }
            if (cursor == queue->tail)
            {
                queue->tail = previous;
            }
            free(cursor);
            queue->length--;
            return 1;
        }
        previous = cursor;
        cursor = cursor->next;
    }
    return 0;
}

bool isValidInteger(const char *token)
{
    if (!token || token[0] == '\0')
    {
        return false;
    }
    int index = 0;
    if (token[0] == '-' && token[1] != '\0')
    {
        index = 1;
    }
    for (; token[index]; index++)
    {
        if (token[index] < '0' || token[index] > '9')
        {
            return false;
        }
    }
    return true;
}

bool parseIoTokenBool(const char *token, int *outValue, bool *isDash)
{
    if (!token)
    {
        return false;
    }
    if (strcmp(token, "-") == 0)
    {
        *outValue = 0;
        *isDash = true;
        return true;
    }
    if (!isValidInteger(token))
    {
        return false;
    }
    *outValue = atoi(token);
    *isDash = false;
    return true;
}

bool isValidName(const char *name)
{
    if (!name || name[0] == '\0')
    {
        return false;
    }
    for (int index = 0; name[index]; index++)
    {
        unsigned char ch = (unsigned char)name[index];
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
        {
            return false;
        }
    }
    return true;
}

int comparePids(const void *a, const void *b)
{
    const ProcessControlBlock *processA = *(const ProcessControlBlock **)a;
    const ProcessControlBlock *processB = *(const ProcessControlBlock **)b;
    return processA->pid - processB->pid;
}

void freeAllPcbsFromHashmap()
{
    int index = 0;
    while (index < HASHMAP_SIZE)
    {
        ProcessControlBlock *cursor = hashmap[index];
        while (cursor)
        {
            ProcessControlBlock *next = cursor->nextInHash;
            free(cursor);
            cursor = next;
        }
        hashmap[index] = NULL;
        index++;
    }
}

int main()
{
    int numberOfProcesses = 0;
    printf("Enter number of processes: ");
    if (scanf("%d", &numberOfProcesses) != 1 || numberOfProcesses <= 0)
    {
        printf("Invalid number of processes\n");
        return 1;
    }

    for (int index = 0; index < HASHMAP_SIZE; index++)
        hashmap[index] = NULL;

    Queue readyQueue, waitingQueue, terminatedQueue;
    initQueue(&readyQueue);
    initQueue(&waitingQueue);
    initQueue(&terminatedQueue);

    printf("Enter each process: <name> <pid> <cpu> <io_start or -> <io_duration or ->\n");

    for (int index = 0; index < numberOfProcesses; index++)
    {
        char name[NAME_MAX], pidToken[32], cpuToken[32], ioStartToken[32], ioDurToken[32];
        if (scanf("%s %s %s %s %s", name, pidToken, cpuToken, ioStartToken, ioDurToken) != 5)
        {
            printf("Invalid process input\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        if (!isValidName(name))
        {
            printf("Invalid process name\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        if (!isValidInteger(pidToken) || !isValidInteger(cpuToken))
        {
            printf("Invalid numeric values for PID or CPU burst\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        int pidValue = atoi(pidToken);
        int cpuBurst = atoi(cpuToken);

        int ioStart = 0, ioDur = 0;
        bool ioStartIsDash = false, ioDurIsDash = false;

        if (!parseIoTokenBool(ioStartToken, &ioStart, &ioStartIsDash) ||
            !parseIoTokenBool(ioDurToken, &ioDur, &ioDurIsDash))
        {
            printf("Invalid IO values\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        ProcessControlBlock *pcb = malloc(sizeof(ProcessControlBlock));
        if (!pcb)
        {
            printf("Memory allocation failed\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        strncpy(pcb->processName, name, NAME_MAX - 1);
        pcb->processName[NAME_MAX - 1] = '\0';
        pcb->pid = pidValue;
        pcb->totalCpuBurst = cpuBurst;
        pcb->remainingCpuBurst = cpuBurst;
        pcb->ioStartAfterCpu = (ioStartIsDash || ioDurIsDash) ? 0 : ioStart;
        pcb->ioDuration = (ioStartIsDash || ioDurIsDash) ? 0 : ioDur;
        pcb->remainingIo = 0;
        pcb->cpuExecuted = 0;
        pcb->actualIoTime = 0;
        pcb->arrivalTime = 0;
        pcb->completionTime = -1;
        pcb->waitingTime = 0;
        pcb->state = STATE_READY;
        pcb->killedAtTime = -1;
        pcb->nextInHash = NULL;

        hashmapPut(pcb);
        enqueue(&readyQueue, pcb);
    }

    int numberOfKillEvents = 0;
    printf("Enter number of KILL events: ");
    if (scanf("%d", &numberOfKillEvents) != 1 || numberOfKillEvents < 0)
    {
        printf("Invalid number of kill events\n");
        freeAllPcbsFromHashmap();
        return 1;
    }

    KillEvent *killEvents = NULL;
    if (numberOfKillEvents > 0)
    {
        killEvents = malloc(sizeof(KillEvent) * numberOfKillEvents);
        if (!killEvents)
        {
            printf("Memory allocation failed\n");
            freeAllPcbsFromHashmap();
            return 1;
        }

        for (int index = 0; index < numberOfKillEvents; index++)
        {
            char killWord[8], pidToken[32], timeToken[32];
            if (scanf("%s %s %s", killWord, pidToken, timeToken) != 3 ||
                strcmp(killWord, "KILL") != 0)
            {
                printf("Invalid KILL input\n");
                free(killEvents);
                freeAllPcbsFromHashmap();
                return 1;
            }

            if (!isValidInteger(pidToken) || !isValidInteger(timeToken))
            {
                printf("Invalid KILL numeric values\n");
                free(killEvents);
                freeAllPcbsFromHashmap();
                return 1;
            }

            killEvents[index].pid = atoi(pidToken);
            killEvents[index].killTime = atoi(timeToken);
        }
    }

    int currentTime = 0;
    ProcessControlBlock *runningProcess = NULL;
    int terminatedCount = 0;

    while (terminatedCount < numberOfProcesses)
    {
        for (int index = 0; index < numberOfKillEvents; index++)
        {
            if (killEvents[index].killTime == currentTime)
            {
                ProcessControlBlock *target = hashmapGet(killEvents[index].pid);
                if (target && target->state != STATE_TERMINATED && target->state != STATE_KILLED)
                {
                    if (runningProcess == target)
                        runningProcess = NULL;
                    removeFromQueueByPid(&readyQueue, target->pid);
                    removeFromQueueByPid(&waitingQueue, target->pid);
                    target->state = STATE_KILLED;
                    target->killedAtTime = currentTime;
                    target->completionTime = currentTime;
                    enqueue(&terminatedQueue, target);
                    terminatedCount++;
                }
            }
        }

        if (!runningProcess)
        {
            runningProcess = dequeue(&readyQueue);
            if (runningProcess)
                runningProcess->state = STATE_RUNNING;
        }

        if (runningProcess)
        {
            runningProcess->cpuExecuted++;
            runningProcess->remainingCpuBurst--;
            if (runningProcess->remainingCpuBurst == 0)
            {
                runningProcess->state = STATE_TERMINATED;
                runningProcess->completionTime = currentTime + 1;
                enqueue(&terminatedQueue, runningProcess);
                runningProcess = NULL;
                terminatedCount++;
            }
            else if (runningProcess->ioDuration > 0 && runningProcess->cpuExecuted == runningProcess->ioStartAfterCpu)
            {
                runningProcess->state = STATE_WAITING;
                runningProcess->remainingIo = runningProcess->ioDuration;
                enqueue(&waitingQueue, runningProcess);
                runningProcess = NULL;
            }
        }

        Node *waitCursor = waitingQueue.head;
        while (waitCursor)
        {
            ProcessControlBlock *pcb = waitCursor->pcb;
            if (pcb->remainingIo > 0)
            {
                pcb->remainingIo--;
                pcb->actualIoTime++;
            }
            waitCursor = waitCursor->next;
        }

        int waitCount = waitingQueue.length;
        for (int index = 0; index < waitCount; index++)
        {
            ProcessControlBlock *pcb = dequeue(&waitingQueue);
            if (pcb->remainingIo <= 0 && pcb->state != STATE_TERMINATED && pcb->state != STATE_KILLED)
            {
                pcb->state = STATE_READY;
                enqueue(&readyQueue, pcb);
            }
            else
            {
                enqueue(&waitingQueue, pcb);
            }
        }

        if (!runningProcess && readyQueue.length == 0 && waitingQueue.length == 0)
            break;

        currentTime++;
    }

    ProcessControlBlock **allList = malloc(sizeof(ProcessControlBlock *) * numberOfProcesses);
    int collected = 0;

    for (int index = 0; index < HASHMAP_SIZE; index++)
    {
        ProcessControlBlock *cursor = hashmap[index];
        while (cursor)
        {
            allList[collected++] = cursor;
            cursor = cursor->nextInHash;
        }
    }

    qsort(allList, collected, sizeof(ProcessControlBlock *), comparePids);

    // --- FIXED WAITING TIME CALCULATION ---
    for (int index = 0; index < collected; index++)
    {
        ProcessControlBlock *pcb = allList[index];
        int tat = 0, wt = 0;

        if (pcb->state == STATE_KILLED)
        {
            tat = pcb->killedAtTime - pcb->arrivalTime;
            wt = tat - pcb->cpuExecuted;
            if (wt < 0)
                wt = 0;
        }
        else
        {
            tat = pcb->completionTime - pcb->arrivalTime;
            wt = tat - pcb->totalCpuBurst;
        }

        pcb->waitingTime = wt;
    }

    int showStatus = (numberOfKillEvents > 0);

    if (showStatus)
        printf("\nFinal process table:\nPID   Name                 CPU   IO    Status          Turnaround   Waiting\n");
    else
        printf("\nFinal process table:\nPID   Name                 CPU   IO    Turnaround   Waiting\n");

    for (int index = 0; index < collected; index++)
    {
        ProcessControlBlock *pcb = allList[index];
        int tat = (pcb->completionTime >= 0) ? ((pcb->state == STATE_KILLED) ? pcb->killedAtTime - pcb->arrivalTime : pcb->completionTime - pcb->arrivalTime) : 0;

        if (showStatus)
        {
            char statusText[32];
            snprintf(statusText, sizeof(statusText), (pcb->state == STATE_KILLED) ? "KILLED at %d" : "OK", pcb->killedAtTime);
            printf("%-5d %-20s %-5d %-5d %-15s %-11d %-7d\n", pcb->pid, pcb->processName, pcb->totalCpuBurst, pcb->actualIoTime, statusText, tat, pcb->waitingTime);
        }
        else
        {
            printf("%-5d %-20s %-5d %-5d %-11d %-7d\n", pcb->pid, pcb->processName, pcb->totalCpuBurst, pcb->actualIoTime, tat, pcb->waitingTime);
        }
    }

    free(allList);
    free(killEvents);
    freeAllPcbsFromHashmap();

    return 0;
}
