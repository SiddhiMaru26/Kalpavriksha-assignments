#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct MessageQueue
{
    long type;
    int array[100];
    int numElements;
};

int main()
{
    key_t queueKey = 1234;
    int queueId = msgget(queueKey, 0666 | IPC_CREAT);
    struct MessageQueue sendMsg;

    printf("Enter number of elements: ");
    scanf("%d", &sendMsg.numElements);

    for (int index = 0; index < sendMsg.numElements; index++)
    {
        printf("Enter element %d: ", index + 1);
        scanf("%d", &sendMsg.array[index]);
    }

    sendMsg.type = 1;
    msgsnd(queueId, &sendMsg, sizeof(sendMsg) - sizeof(long), 0);

    struct MessageQueue receiveMsg;
    msgrcv(queueId, &receiveMsg, sizeof(receiveMsg) - sizeof(long), 1, 0);

    for (int pass = 0; pass < receiveMsg.numElements - 1; pass++)
    {
        for (int index = 0; index < receiveMsg.numElements - 1 - pass; index++)
        {
            if (receiveMsg.array[index] > receiveMsg.array[index + 1])
            {
                int tempValue = receiveMsg.array[index];
                receiveMsg.array[index] = receiveMsg.array[index + 1];
                receiveMsg.array[index + 1] = tempValue;
            }
        }
    }

    printf("Sorted Array: ");
    for (int index = 0; index < receiveMsg.numElements; index++)
    {
        printf("%d ", receiveMsg.array[index]);
    }
    printf("\n");

    msgctl(queueId, IPC_RMID, NULL);
    return 0;
}
