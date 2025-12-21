#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

#define WITHDRAW 1
#define DEPOSIT 2
#define DISPLAY 3
#define EXIT 4

void displayMenu() {
    printf("\n============================================\n");
    printf("ATM Transaction Menu\n");
    printf("============================================\n");
    printf("1. Withdraw Amount\n");
    printf("2. Deposit Amount\n");
    printf("3. Display Balance\n");
    printf("4. Exit\n");
    printf("============================================\n");
    printf("Enter your choice: ");
}

int getChoice() {
    int choice;
    while (1) {
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Enter again: ");
            while (getchar() != '\n');
        } else if (choice < 1 || choice > 4) {
            printf("Choice must be 1-4. Enter again: ");
        } else {
            return choice;
        }
    }
}

int getAmount() {
    int amount;
    while (1) {
        if (scanf("%d", &amount) != 1) {
            printf("Invalid input. Enter again: ");
            while (getchar() != '\n');
        } else if (amount <= 0) {
            printf("Amount must be positive. Enter again: ");
        } else {
            return amount;
        }
    }
}

void handleTransaction(int clientSocket) {
    int choice, amount, response;
    while (1) {
        displayMenu();
        choice = getChoice();
        if (choice == EXIT) {
            write(clientSocket, &choice, sizeof(int));
            printf("Thank you for using ATM service!\n");
            break;
        }
        if (choice == WITHDRAW || choice == DEPOSIT) {
            printf("Enter amount: ");
            amount = getAmount();
        } else {
            amount = 0;
        }
        write(clientSocket, &choice, sizeof(int));
        if (choice == WITHDRAW || choice == DEPOSIT)
            write(clientSocket, &amount, sizeof(int));
        read(clientSocket, &response, sizeof(int));
        if (choice == WITHDRAW && response == -1)
            printf("Withdrawal failed: Insufficient balance.\n");
        else
            printf("Transaction successful. Current balance: %d\n", response);
    }
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        exit(1);
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(clientSocket);
        exit(1);
    }
    printf("Connected to ATM server.\n");
    handleTransaction(clientSocket);
    close(clientSocket);
    return 0;
}
