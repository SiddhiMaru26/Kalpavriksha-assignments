#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    key_t sharedKey = 5678;
    int numElements;
    printf("Enter number of elements: ");
    scanf("%d", &numElements);

    int sharedId = shmget(sharedKey, sizeof(int) * numElements, 0666 | IPC_CREAT);
    int *sharedArray = (int*) shmat(sharedId, NULL, 0);

    for (int index = 0; index < numElements; index++)
    {
        printf("Enter element %d: ", index + 1);
        scanf("%d", &sharedArray[index]);
    }

    int processId = fork();

    if (processId == 0)
    {
        for (int pass = 0; pass < numElements - 1; pass++)
        {
            for (int index = 0; index < numElements - 1 - pass; index++)
            {
                if (sharedArray[index] > sharedArray[index + 1])
                {
                    int tempValue = sharedArray[index];
                    sharedArray[index] = sharedArray[index + 1];
                    sharedArray[index + 1] = tempValue;
                }
            }
        }
        shmdt(sharedArray);
    }
    else
    {
        wait(NULL);
        printf("Sorted Array: ");
        for (int index = 0; index < numElements; index++)
        {
            printf("%d ", sharedArray[index]);
        }
        printf("\n");
        shmdt(sharedArray);
        shmctl(sharedId, IPC_RMID, NULL);
    }

    return 0;
}
