#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ELEMENTS 100

int getIntegerInput()
{
    int value;
    while (1)
    {
        if (scanf("%d", &value) != 1)
        {
            while (getchar() != '\n');
        }
        else
        {
            if (getchar() != '\n')
            {
                while (getchar() != '\n');
            }
            else
            {
                return value;
            }
        }
    }
}

int readArrayFromUser(int *array)
{
    int elementCount;
    elementCount = getIntegerInput();
    if (elementCount <= 0 || elementCount > MAX_ELEMENTS)
    {
        return -1;
    }
    for (int index = 0; index < elementCount; index++)
    {
        array[index] = getIntegerInput();
    }
    return elementCount;
}

void sortArray(int *array, int elementCount)
{
    for (int pass = 0; pass < elementCount - 1; pass++)
    {
        for (int index = 0; index < elementCount - 1 - pass; index++)
        {
            if (array[index] > array[index + 1])
            {
                int temp = array[index];
                array[index] = array[index + 1];
                array[index + 1] = temp;
            }
        }
    }
}

void executeSharedMemorySorting()
{
    key_t sharedKey = 5678;
    int elementCount;
    elementCount = getIntegerInput();
    if (elementCount <= 0 || elementCount > MAX_ELEMENTS)
    {
        return;
    }

    int sharedId = shmget(sharedKey, sizeof(int) * elementCount, 0666 | IPC_CREAT);
    int *sharedArray = (int*) shmat(sharedId, NULL, 0);

    for (int index = 0; index < elementCount; index++)
    {
        sharedArray[index] = getIntegerInput();
    }

    pid_t processId = fork();

    if (processId < 0)
    {
        return;
    }
    else if (processId == 0)
    {
        sortArray(sharedArray, elementCount);
        shmdt(sharedArray);
    }
    else
    {
        wait(NULL);
        for (int index = 0; index < elementCount; index++)
        {
            printf("%d ", sharedArray[index]);
        }
        printf("\n");
        shmdt(sharedArray);
        shmctl(sharedId, IPC_RMID, NULL);
    }
}

int main()
{
    executeSharedMemorySorting();
    return 0;
}
