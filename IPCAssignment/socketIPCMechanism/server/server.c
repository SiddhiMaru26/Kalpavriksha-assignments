#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

int accountBalance = 1000;
pthread_mutex_t accountLock;

void* handleClientConnection(void* clientSocketPointer)
{
    int clientSocket = *(int*)clientSocketPointer;
    free(clientSocketPointer);

    int clientChoice;
    int transactionAmount;

    while(1)
    {
        read(clientSocket, &clientChoice, sizeof(int));

        if(clientChoice == 4)
        {
            break;
        }

        if(clientChoice == 1 || clientChoice == 2)
        {
            read(clientSocket, &transactionAmount, sizeof(int));
            pthread_mutex_lock(&accountLock);

            if(clientChoice == 1)
            {
                if(transactionAmount > accountBalance)
                {
                    transactionAmount = -1;
                }
                else
                {
                    accountBalance -= transactionAmount;
                }
            }
            else
            {
                accountBalance += transactionAmount;
            }

            pthread_mutex_unlock(&accountLock);
        }

        int balanceToSend = (clientChoice == 1 && transactionAmount == -1) ? -1 : accountBalance;
        write(clientSocket, &balanceToSend, sizeof(int));
    }

    close(clientSocket);
    return NULL;
}

int main()
{
    pthread_mutex_init(&accountLock, NULL);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = SERVER_PORT;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    while(1)
    {
        int* clientSocketPointer = malloc(sizeof(int));
        *clientSocketPointer = accept(serverSocket, NULL, NULL);

        pthread_t clientThread;
        pthread_create(&clientThread, NULL, handleClientConnection, clientSocketPointer);
        pthread_detach(clientThread);
    }

    close(serverSocket);
    return 0;
}
