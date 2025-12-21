#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_ELEMENTS 100

struct MsgQueue
{
    long type;
    int array[MAX_ELEMENTS];
    int numElements;
};

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

int readArrayFromUser(struct MsgQueue *message)
{
    message->numElements = getIntegerInput();
    if (message->numElements <= 0 || message->numElements > MAX_ELEMENTS)
    {
        return -1;
    }
    for (int index = 0; index < message->numElements; index++)
    {
        message->array[index] = getIntegerInput();
    }
    return message->numElements;
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

void sendArrayToQueue(int queueId, struct MsgQueue *message)
{
    message->type = 1;
    msgsnd(queueId, message, sizeof(struct MsgQueue) - sizeof(long), 0);
}

void receiveArrayFromQueue(int queueId, struct MsgQueue *message)
{
    msgrcv(queueId, message, sizeof(struct MsgQueue) - sizeof(long), 1, 0);
}

void displayArray(int *array, int elementCount)
{
    for (int index = 0; index < elementCount; index++)
    {
        printf("%d ", array[index]);
    }
    printf("\n");
}

void executeMessageQueueSorting()
{
    key_t queueKey = 1234;
    int queueId = msgget(queueKey, 0666 | IPC_CREAT);

    struct MsgQueue message;
    if (readArrayFromUser(&message) == -1)
    {
        return;
    }

    sendArrayToQueue(queueId, &message);

    struct MsgQueue receivedMessage;
    receiveArrayFromQueue(queueId, &receivedMessage);

    sortArray(receivedMessage.array, receivedMessage.numElements);
    displayArray(receivedMessage.array, receivedMessage.numElements);

    msgctl(queueId, IPC_RMID, NULL);
}

int main()
{
    executeMessageQueueSorting();
    return 0;
}
