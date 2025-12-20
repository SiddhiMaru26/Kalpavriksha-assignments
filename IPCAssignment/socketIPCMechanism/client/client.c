#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = SERVER_PORT;
    serverAddress.sin_addr.s_addr = 0x7f000001;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    int clientChoice;
    int transactionAmount;
    int currentBalance;

    while(1)
    {
        printf("\n1.Withdraw 2.Deposit 3.Display Balance 4.Exit\n");
        printf("Enter choice: ");
        scanf("%d", &clientChoice);

        write(clientSocket, &clientChoice, sizeof(int));

        if(clientChoice == 4)
        {
            break;
        }

        if(clientChoice == 1 || clientChoice == 2)
        {
            printf("Enter amount: ");
            scanf("%d", &transactionAmount);
            write(clientSocket, &transactionAmount, sizeof(int));
        }

        read(clientSocket, &currentBalance, sizeof(int));

        if(clientChoice == 1 && currentBalance == -1)
        {
            printf("Insufficient balance\n");
        }
        else
        {
            printf("Current balance: %d\n", currentBalance);
        }
    }

    close(clientSocket);
    return 0;
}
