#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define HASH_MAP_SIZE 1009
#define MAX_NAME_LEN 64
#define MAX_PROCESSES 1000
#define MAX_KILL_EVENTS 1000
#define MAX_INTEGER_LENGTH 16

typedef enum
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} State;

typedef struct PCB
{
    char name[MAX_NAME_LEN];
    int pid;
    int cpu_burst;
    int cpu_remaining;
    int io_start;
    int io_duration;
    int io_remaining;
    int executed_time;
    int completion_time;
    int io_time_total;
    int killed;
    int killed_time;
    State state;
    struct PCB *next;
} PCB;

typedef struct Queue
{
    PCB *front;
    PCB *rear;
} Queue;

typedef struct HashMapNode
{
    int key;
    PCB *process;
    struct HashMapNode *next;
} HashMapNode;

typedef struct KillEvent
{
    int pid;
    int time;
} KillEvent;

HashMapNode *hashMap[HASH_MAP_SIZE];
Queue readyQueue;
Queue waitingQueue;
Queue terminatedQueue;
PCB *processList[MAX_PROCESSES];
KillEvent killEvents[MAX_KILL_EVENTS];
int processCount = 0;
int killEventCount = 0;
int totalProcesses = 0;

int hashFunction(int key)
{
    int hashValue = key % HASH_MAP_SIZE;
    return (hashValue < 0 ? hashValue + HASH_MAP_SIZE : hashValue);
}

void hashPut(int key, PCB *process)
{
    int index = hashFunction(key);
    HashMapNode *current = hashMap[index];

    while (current)
    {
        if (current->key == key)
        {
            return;
        }

        current = current->next;
    }

    HashMapNode *node = (HashMapNode *)malloc(sizeof(HashMapNode));
    node->key = key;
    node->process = process;
    node->next = hashMap[index];
    hashMap[index] = node;
}

PCB *hashGet(int key)
{
    int index = hashFunction(key);
    HashMapNode *current = hashMap[index];

    while (current)
    {
        if (current->key == key)
        {
            return current->process;
        }

        current = current->next;
    }

    return NULL;
}

void initQueue(Queue *queue)
{
    queue->front = queue->rear = NULL;
}

void enqueue(Queue *queue, PCB *process)
{
    process->next = NULL;

    if (queue->rear)
    {
        queue->rear->next = process;
        queue->rear = process;
    }
    else
    {
        queue->front = queue->rear = process;
    }
}

PCB *dequeue(Queue *queue)
{
    if (!queue->front)
    {
        return NULL;
    }

    PCB *process = queue->front;
    queue->front = process->next;

    if (!queue->front)
    {
        queue->rear = NULL;
    }

    return process;
}

PCB *removeFromQueue(Queue *queue, int pid)
{
    PCB *current = queue->front;
    PCB *previous = NULL;

    while (current)
    {
        if (current->pid == pid)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                queue->front = current->next;
            }

            if (current == queue->rear)
            {
                queue->rear = previous;
            }

            current->next = NULL;
            return current;
        }

        previous = current;
        current = current->next;
    }

    return NULL;
}

void forEachQueue(Queue *queue, void (*callback)(PCB *))
{
    PCB *current = queue->front;

    while (current)
    {
        PCB *nextProcess = current->next;
        callback(current);
        current = nextProcess;
    }
}

PCB *createPCB(const char *name, int pid, int burst, int io_start, int io_duration)
{
    PCB *process = (PCB *)malloc(sizeof(PCB));
    strcpy(process->name, name);
    process->pid = pid;
    process->cpu_burst = burst;
    process->cpu_remaining = burst;
    process->io_start = io_start;
    process->io_duration = io_duration;
    process->io_remaining = 0;
    process->io_time_total = io_duration;
    process->executed_time = 0;
    process->completion_time = -1;
    process->killed = 0;
    process->killed_time = -1;
    process->state = READY;
    process->next = NULL;
    return process;
}

void trim(char *line)
{
    int startIndex = 0;
    int endIndex = 0;

    while (isspace(line[startIndex]))
    {
        startIndex++;
    }

    while (line[startIndex])
    {
        line[endIndex++] = line[startIndex++];
    }

    line[endIndex] = '\0';

    while (endIndex > 0 && isspace(line[endIndex - 1]))
    {
        line[--endIndex] = '\0';
    }
}

bool isValidProcessName(char processName[MAX_NAME_LEN])
{
    int index = 0;

    while (processName[index] == ' ')
    {
        index++;
    }

    if (processName[index] == '\0')
    {
        printf("Process name can't be empty.\n");
        return false;
    }

    return true;
}

bool isValidInteger(char integerValue[MAX_INTEGER_LENGTH])
{
    int index = 0;

    if (integerValue[0] == '\0')
    {
        return false;
    }

    while (integerValue[index] != '\0')
    {
        if (!isdigit(integerValue[index]))
        {
            return false;
        }

        index++;
    }

    return true;
}

bool isDashOrInteger(char value[MAX_INTEGER_LENGTH])
{
    if (strcmp(value, "-") == 0)
    {
        return true;
    }

    return isValidInteger(value);
}

bool validateProcessInputValues(char processName[MAX_NAME_LEN], char inputedProcessId[MAX_INTEGER_LENGTH], char inputedBurstTime[MAX_INTEGER_LENGTH], char inputedIOStart[MAX_INTEGER_LENGTH], char inputedIODuration[MAX_INTEGER_LENGTH])
{
    if (!isValidProcessName(processName))
    {
        return false;
    }

    if (!isValidInteger(inputedProcessId))
    {
        printf("Invalid input. Process ID must be an integer.\n");
        return false;
    }

    if (!isValidInteger(inputedBurstTime))
    {
        printf("Invalid input. Burst must be an integer.\n");
        return false;
    }

    if (!isDashOrInteger(inputedIOStart))
    {
        printf("Invalid input. I/O start must be integer or '-'.\n");
        return false;
    }

    if (!isDashOrInteger(inputedIODuration))
    {
        printf("Invalid input. I/O duration must be integer or '-'.\n");
        return false;
    }

    int pid = atoi(inputedProcessId);

    if (hashGet(pid))
    {
        printf("Duplicate PID %d detected.\n", pid);
        return false;
    }

    return true;
}

bool validateKillEventInputValues(char inputedProcessId[MAX_INTEGER_LENGTH], char inputedTime[MAX_INTEGER_LENGTH])
{
    if (!isValidInteger(inputedProcessId))
    {
        printf("Invalid input. Kill PID must be an integer.\n");
        return false;
    }

    if (!isValidInteger(inputedTime))
    {
        printf("Invalid input. Kill time must be an integer.\n");
        return false;
    }

    return true;
}

void parseLine(char *line)
{
    char firstWord[16];
    sscanf(line, "%s", firstWord);

    if (strcasecmp(firstWord, "KILL") == 0)
    {
        char killPidStr[MAX_INTEGER_LENGTH];
        char killTimeStr[MAX_INTEGER_LENGTH];
        sscanf(line + strlen(firstWord), "%s %s", killPidStr, killTimeStr);

        if (!validateKillEventInputValues(killPidStr, killTimeStr))
        {
            return;
        }

        int pid = atoi(killPidStr);
        int timeValue = atoi(killTimeStr);
        killEvents[killEventCount].pid = pid;
        killEvents[killEventCount].time = timeValue;
        killEventCount++;
        return;
    }

    char processName[MAX_NAME_LEN];
    char ioStartStr[MAX_INTEGER_LENGTH];
    char ioDurationStr[MAX_INTEGER_LENGTH];
    char pidStr[MAX_INTEGER_LENGTH];
    char burstStr[MAX_INTEGER_LENGTH];

    int parsedItems = sscanf(line, "%s %s %s %s %s", processName, pidStr, burstStr, ioStartStr, ioDurationStr);

    if (parsedItems < 3)
    {
        return;
    }

    if (!validateProcessInputValues(processName, pidStr, burstStr, (parsedItems >= 4) ? ioStartStr : "-", (parsedItems >= 5) ? ioDurationStr : "-"))
    {
        return;
    }

    int pid = atoi(pidStr);
    int burst = atoi(burstStr);
    int ioStart = (parsedItems >= 4 && strcmp(ioStartStr, "-") != 0) ? atoi(ioStartStr) : -1;
    int ioDuration = (parsedItems >= 5 && strcmp(ioDurationStr, "-") != 0) ? atoi(ioDurationStr) : 0;

    PCB *process = createPCB(processName, pid, burst, ioStart, ioDuration);
    processList[processCount++] = process;
    totalProcesses++;
    hashPut(pid, process);
    enqueue(&readyQueue, process);
}

void decrementIO(PCB *process)
{
    if (process->io_remaining > 0)
    {
        process->io_remaining--;
    }
}

void checkIOCompletion()
{
    PCB *previous = NULL;
    PCB *current = waitingQueue.front;

    while (current)
    {
        PCB *nextProcess = current->next;

        if (current->io_remaining == 0)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                waitingQueue.front = current->next;
            }

            if (current == waitingQueue.rear)
            {
                waitingQueue.rear = previous;
            }

            current->state = READY;
            enqueue(&readyQueue, current);
        }
        else
        {
            previous = current;
        }

        current = nextProcess;
    }
}

void terminatePCB(PCB *process, int current_time)
{
    process->state = TERMINATED;
    process->completion_time = current_time;
    enqueue(&terminatedQueue, process);
}

void applyKillEvents(int current_time, PCB **running_process, int *terminated_count)
{
    for (int index = 0; index < killEventCount; index++)
    {
        if (killEvents[index].time == current_time)
        {
            int pid = killEvents[index].pid;
            PCB *target = hashGet(pid);

            if (!target || target->state == TERMINATED)
            {
                continue;
            }

            if (*running_process && (*running_process)->pid == pid)
            {
                PCB *process = *running_process;
                *running_process = NULL;
                process->killed = 1;
                process->killed_time = current_time;
                terminatePCB(process, current_time);
                (*terminated_count)++;
            }
            else
            {
                PCB *process = removeFromQueue(&readyQueue, pid);

                if (!process)
                {
                    process = removeFromQueue(&waitingQueue, pid);
                }

                if (process)
                {
                    process->killed = 1;
                    process->killed_time = current_time;
                    terminatePCB(process, current_time);
                    (*terminated_count)++;
                }
            }
        }
    }
}

void scheduleProcesses()
{
    int current_time = 0;
    int terminated_count = 0;
    PCB *running_process = NULL;

    while (terminated_count < totalProcesses)
    {
        applyKillEvents(current_time, &running_process, &terminated_count);

        if (!running_process)
        {
            running_process = dequeue(&readyQueue);

            if (running_process)
            {
                running_process->state = RUNNING;
            }
        }

        if (running_process)
        {
            running_process->executed_time++;
            running_process->cpu_remaining--;
        }

        forEachQueue(&waitingQueue, decrementIO);

        if (running_process)
        {
            if (running_process->cpu_remaining > 0 && running_process->io_start >= 0 && running_process->executed_time == running_process->io_start && running_process->io_duration > 0)
            {
                running_process->io_remaining = running_process->io_duration;
                running_process->state = WAITING;
                enqueue(&waitingQueue, running_process);
                running_process = NULL;
            }
            else if (running_process->cpu_remaining == 0)
            {
                terminatePCB(running_process, current_time + 1);
                running_process = NULL;
                terminated_count++;
            }
        }

        checkIOCompletion();
        current_time++;
    }
}

int comparePID(const void *a, const void *b)
{
    PCB *p1 = *(PCB **)a;
    PCB *p2 = *(PCB **)b;
    return p1->pid - p2->pid;
}

void printResults()
{
    PCB *sortedList[MAX_PROCESSES];
    int killedExist = 0;

    for (int index = 0; index < processCount; index++)
    {
        sortedList[index] = processList[index];

        if (sortedList[index]->killed)
        {
            killedExist = 1;
        }
    }

    qsort(sortedList, processCount, sizeof(PCB *), comparePID);

    if (!killedExist)
    {
        printf("PID\tName\tCPU\tIO\tTurnaround\tWaiting\n");

        for (int index = 0; index < processCount; index++)
        {
            PCB *process = sortedList[index];
            int turnaround = process->completion_time;
            int waiting = turnaround - process->cpu_burst;
            printf("%d\t%s\t%d\t%d\t%d\t\t%d\n", process->pid, process->name, process->cpu_burst, process->io_time_total, turnaround, waiting);
        }
    }
    else
    {
        printf("PID\tName\tCPU\tIO\tStatus\t\tTurnaround\tWaiting\n");

        for (int index = 0; index < processCount; index++)
        {
            PCB *process = sortedList[index];

            if (process->killed)
            {
                printf("%d\t%s\t%d\t%d\tKILLED at %d\t-\t\t-\n", process->pid, process->name, process->cpu_burst, process->io_time_total, process->killed_time);
            }
            else
            {
                int turnaround = process->completion_time;
                int waiting = turnaround - process->cpu_burst;
                printf("%d\t%s\t%d\t%d\tOK\t\t%d\t\t%d\n", process->pid, process->name, process->cpu_burst, process->io_time_total, turnaround, waiting);
            }
        }
    }
}

int main()
{
    initQueue(&readyQueue);
    initQueue(&waitingQueue);
    initQueue(&terminatedQueue);

    char inputLine[256];

    while (fgets(inputLine, sizeof(inputLine), stdin))
    {
        trim(inputLine);

        if (strlen(inputLine) == 0)
        {
            break;
        }

        parseLine(inputLine);
    }

    if (totalProcesses == 0)
    {
        printf("No valid processes entered.\n");
        return 0;
    }

    scheduleProcesses();
    printResults();
    return 0;
}
