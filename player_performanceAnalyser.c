#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "players_data.h"

#define MAX_NAME_LENGTH 50
#define MAX_PLAYERS 1200
#define MAX_TEAMS 50
#define TEAM_PLAYER_LIMIT 60

#define ROLE_BATSMAN 1
#define ROLE_BOWLER 2
#define ROLE_ALL_ROUNDER 3

typedef struct PlayerModel
{
    int id;
    char name[MAX_NAME_LENGTH];
    char teamName[MAX_NAME_LENGTH];
    int role;
    int runs;
    float average;
    float strikeRate;
    int wickets;
    float economy;
    float performance;
} PlayerModel;

typedef struct PlayerNode
{
    int playerIndex;
    struct PlayerNode *next;
} PlayerNode;

typedef struct Team
{
    int id;
    char name[MAX_NAME_LENGTH];
    int totalPlayers;
    float averageBattingStrikeRate;
    PlayerNode *playerHead;
    PlayerNode *batsmanHead;
    PlayerNode *bowlerHead;
    PlayerNode *allrounderHead;
    int batsmanCount;
    int bowlerCount;
    int allrounderCount;
} Team;

PlayerModel playersAll[MAX_PLAYERS];
Team teamsAll[MAX_TEAMS];
int playersCount = 0;
int teamsCount = 0;

bool validateInput(int value, int minValue, int maxValue)
{
    return (value >= minValue && value <= maxValue);
}

bool validateTeamId(int value)
{
    if (value < 1 || value > teamsCount)
    {
        return false;
    }
    return true;
}

float computePerformance(PlayerModel *playerPointer)
{
    if (playerPointer->role == ROLE_BATSMAN)
    {
        return (playerPointer->average * playerPointer->strikeRate) / 100.0f;
    }

    if (playerPointer->role == ROLE_BOWLER)
    {
        return (playerPointer->wickets * 2.0f) + (100.0f - playerPointer->economy);
    }

    return (playerPointer->average * playerPointer->strikeRate) / 100.0f + (playerPointer->wickets * 2.0f);
}

int roleStringToNumber(const char *roleString)
{
    if (strcmp(roleString, "Batsman") == 0)
    {
        return ROLE_BATSMAN;
    }

    if (strcmp(roleString, "Bowler") == 0)
    {
        return ROLE_BOWLER;
    }

    return ROLE_ALL_ROUNDER;
}

PlayerNode *createPlayerNode(int playerIndex)
{
    PlayerNode *node = (PlayerNode *)malloc(sizeof(PlayerNode));
    if (node == NULL)
    {
        return NULL;
    }

    node->playerIndex = playerIndex;
    node->next = NULL;
    return node;
}

void insertPlayer(PlayerNode **headPointer, int playerIndex)
{
    PlayerNode *newNode = createPlayerNode(playerIndex);
    if (newNode == NULL)
    {
        return;
    }

    if (*headPointer == NULL)
    {
        *headPointer = newNode;
        return;
    }

    PlayerNode *node = *headPointer;
    while (node->next != NULL)
    {
        node = node->next;
    }

    node->next = newNode;
}

void SortedByPerformance(PlayerNode **headPointer, int playerIndex)
{
    PlayerNode *newNode = createPlayerNode(playerIndex);
    if (newNode == NULL)
    {
        return;
    }

    if (*headPointer == NULL)
    {
        *headPointer = newNode;
        return;
    }

    float perf = playersAll[playerIndex].performance;
    PlayerNode *node = *headPointer;
    PlayerNode *prev = NULL;

    while (node != NULL && playersAll[node->playerIndex].performance >= perf)
    {
        prev = node;
        node = node->next;
    }

    if (prev == NULL)
    {
        newNode->next = *headPointer;
        *headPointer = newNode;
    }
    else
    {
        prev->next = newNode;
        newNode->next = node;
    }
}

void loadFromHeader()
{
    playersCount = 0;
    int index = 0;

    for (index = 0; index < playerCount && index < MAX_PLAYERS; index++)
    {
        playersAll[index].id = players[index].id;
        strncpy(playersAll[index].name, players[index].name, MAX_NAME_LENGTH - 1);
        playersAll[index].name[MAX_NAME_LENGTH - 1] = '\0';
        strncpy(playersAll[index].teamName, players[index].team, MAX_NAME_LENGTH - 1);
        playersAll[index].teamName[MAX_NAME_LENGTH - 1] = '\0';
        playersAll[index].role = roleStringToNumber(players[index].role);
        playersAll[index].runs = players[index].totalRuns;
        playersAll[index].average = players[index].battingAverage;
        playersAll[index].strikeRate = players[index].strikeRate;
        playersAll[index].wickets = players[index].wickets;
        playersAll[index].economy = players[index].economyRate;
        playersAll[index].performance = computePerformance(&playersAll[index]);
        playersCount++;
    }
}

void buildTeams()
{
    teamsCount = 0;
    int index = 0;

    for (index = 0; index < teamCount && index < MAX_TEAMS; index++)
    {
        teamsAll[index].id = index + 1;
        strncpy(teamsAll[index].name, teams[index], MAX_NAME_LENGTH - 1);
        teamsAll[index].name[MAX_NAME_LENGTH - 1] = '\0';
        teamsAll[index].totalPlayers = 0;
        teamsAll[index].averageBattingStrikeRate = 0.0f;
        teamsAll[index].playerHead = NULL;
        teamsAll[index].batsmanHead = NULL;
        teamsAll[index].bowlerHead = NULL;
        teamsAll[index].allrounderHead = NULL;
        teamsAll[index].batsmanCount = 0;
        teamsAll[index].bowlerCount = 0;
        teamsAll[index].allrounderCount = 0;
        teamsCount++;
    }

    for (index = 0; index < playersCount; index++)
    {
        int index2 = 0;

        for (index2 = 0; index2 < teamsCount; index2++)
        {
            if (strcmp(playersAll[index].teamName, teamsAll[index2].name) == 0)
            {
                insertPlayer(&teamsAll[index2].playerHead, index);
                teamsAll[index2].totalPlayers++;

                if (playersAll[index].role == ROLE_BATSMAN)
                {
                    SortedByPerformance(&teamsAll[index2].batsmanHead, index);
                    teamsAll[index2].batsmanCount++;
                }
                else if (playersAll[index].role == ROLE_BOWLER)
                {
                    SortedByPerformance(&teamsAll[index2].bowlerHead, index);
                    teamsAll[index2].bowlerCount++;
                }
                else
                {
                    SortedByPerformance(&teamsAll[index2].allrounderHead, index);
                    teamsAll[index2].allrounderCount++;
                }

                break;
            }
        }
    }

    for (index = 0; index < teamsCount; index++)
    {
        float strikeRateSum = 0.0f;
        int eligibleCount = 0;
        PlayerNode *node = teamsAll[index].playerHead;

        while (node != NULL)
        {
            int storedPlayerIndex = node->playerIndex;
            bool isBatsman = (playersAll[storedPlayerIndex].role == ROLE_BATSMAN);
            bool isAllRounder = (playersAll[storedPlayerIndex].role == ROLE_ALL_ROUNDER);

            if (isBatsman || isAllRounder)
            {
                strikeRateSum += playersAll[storedPlayerIndex].strikeRate;
                eligibleCount++;
            }

            node = node->next;
        }

        if (eligibleCount > 0)
        {
            teamsAll[index].averageBattingStrikeRate = strikeRateSum / eligibleCount;
        }
        else
        {
            teamsAll[index].averageBattingStrikeRate = 0.0f;
        }
    }
}

void swapValue(int array[], int index1, int index2)
{
    int holder = array[index1];
    array[index1] = array[index2];
    array[index2] = holder;
}

void SortByteam(int orderArray[], int count)
{
    int index = 0;

    for (index = 0; index < count - 1; index++)
    {
        int selectedIndex = index;
        int index2 = 0;

        for (index2 = index + 1; index2 < count; index2++)
        {
            if (teamsAll[orderArray[index2]].averageBattingStrikeRate >
                teamsAll[orderArray[selectedIndex]].averageBattingStrikeRate)
            {
                selectedIndex = index2;
            }
        }

        if (selectedIndex != index)
        {
            swapValue(orderArray, index, selectedIndex);
        }
    }
}

int searchByTeam(int searchTeamId)
{
    int low = 0;
    int high = teamsCount - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;

        if (teamsAll[mid].id == searchTeamId)
        {
            return mid;
        }

        if (teamsAll[mid].id < searchTeamId)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return -1;
}

void DisplayPlayer()
{
    int inputValue = 0;
    int result = 0;

    printf("Enter Team ID (1..%d): ", teamsCount);
    result = scanf("%d", &inputValue);

    if (result != 1)
    {
        printf("Error: Team ID must be an integer\n");
        while (getchar() != '\n') { }
        return;
    }

    if (!validateInput(inputValue, 1, teamsCount))
    {
        printf("Error: Team ID must be between 1 and %d\n", teamsCount);
        return;
    }

    int teamIndex = searchByTeam(inputValue);

    if (teamIndex == -1)
    {
        printf("Team not found.\n");
        return;
    }

    Team *team = &teamsAll[teamIndex];
    printf("\nPlayers of Team %s (ID %d)\n", team->name, team->id);
    printf("====================================================================================\n");
    printf("ID   Name                      Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    PlayerNode *node = team->playerHead;

    while (node != NULL)
    {
        PlayerModel *player = &playersAll[node->playerIndex];
        char *roleName = (player->role == ROLE_BATSMAN) ? "Batsman" : (player->role == ROLE_BOWLER ? "Bowler" : "All-Rounder");

        printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
               player->id,
               player->name,
               roleName,
               player->runs,
               player->average,
               player->strikeRate,
               player->wickets,
               player->economy,
               player->performance);

        node = node->next;
    }

    printf("====================================================================================\n");
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", team->averageBattingStrikeRate);
}

void DisplayBYaverageStirke()
{
    int orderArray[MAX_TEAMS];
    int index = 0;

    for (index = 0; index < teamsCount; index++)
    {
        orderArray[index] = index;
    }

    if (teamsCount > 1)
    {
        SortByteam(orderArray, teamsCount);
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID   Team Name            Avg Bat SR   Total Players\n");
    printf("=========================================================\n");

    for (index = 0; index < teamsCount; index++)
    {
        int teamPrintIndex = orderArray[index];
        printf("%d %s %.1f %d\n",
               teamsAll[teamPrintIndex].id,
               teamsAll[teamPrintIndex].name,
               teamsAll[teamPrintIndex].averageBattingStrikeRate,
               teamsAll[teamPrintIndex].totalPlayers);
    }

    printf("=========================================================\n");
}

void DisplayTopKPlayers()
{
    int teamId = 0;
    int role = 0;
    int k = 0;

    printf("Enter Team ID: ");
    if (scanf("%d", &teamId) != 1)
    {
        printf("Error\n");
        while (getchar() != '\n') { }
        return;
    }

    if (!validateInput(teamId, 1, teamsCount))
    {
        printf("Error: team id out of range\n");
        return;
    }

    printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
    if (scanf("%d", &role) != 1)
    {
        printf("Error\n");
        while (getchar() != '\n') { }
        return;
    }

    printf("Enter number of players: ");
    if (scanf("%d", &k) != 1)
    {
        printf("Error\n");
        while (getchar() != '\n') { }
        return;
    }

    int teamIndex = searchByTeam(teamId);
    if (teamIndex == -1)
    {
        printf("Team not found\n");
        return;
    }

    int tempIndices[MAX_PLAYERS];
    int count = 0;
    int index = 0;

    for (index = 0; index < playersCount; index++)
    {
        if (playersAll[index].role == role && strcmp(playersAll[index].teamName, teamsAll[teamIndex].name) == 0)
        {
            tempIndices[count++] = index;
        }
    }

    if (count == 0)
    {
        printf("No players found\n");
        return;
    }

    for (index = 0; index < count - 1; index++)
    {
        int choice = index;
        int index2 = 0;

        for (index2 = index + 1; index2 < count; index2++)
        {
            if (playersAll[tempIndices[index2]].performance > playersAll[tempIndices[choice]].performance)
            {
                choice = index2;
            }
        }

        if (choice != index)
        {
            swapValue(tempIndices, index, choice);
        }
    }

    if (k > count)
    {
        k = count;
    }

    printf("Top %d players of Team %s:\n", k, teamsAll[teamIndex].name);
    printf("====================================================================================\n");
    printf("ID   Name                      Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    for (index = 0; index < k; index++)
    {
        PlayerModel *player = &playersAll[tempIndices[index]];
        char *roleName = (player->role == ROLE_BATSMAN) ? "Batsman" : (player->role == ROLE_BOWLER ? "Bowler" : "All-Rounder");

      printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
               player->id, player->name, roleName, player->runs, player->average, player->strikeRate, player->wickets, player->economy, player->performance);
    }

    printf("====================================================================================\n");
}

void DisplayAllPlayersByRole()
{
    int role = 0;
    printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
    if (scanf("%d", &role) != 1)
    {
        printf("Error\n");
        while (getchar() != '\n') { }
        return;
    }

    int tempIndices[MAX_PLAYERS];
    int count = 0;
    int index = 0;

    for (index = 0; index < playersCount; index++)
    {
        if (playersAll[index].role == role)
        {
            tempIndices[count++] = index;
        }
    }

    if (count == 0)
    {
        printf("No players found\n");
        return;
    }

    for (index = 0; index < count - 1; index++)
    {
        int temporary = index;
        int index2 = 0;

        for (index2 = index + 1; index2 < count; index2++)
        {
            if (playersAll[tempIndices[index2]].performance > playersAll[tempIndices[temporary]].performance)
            {
                temporary = index2;
            }
        }

        if (temporary != index)
        {
            swapValue(tempIndices, index, temporary);
        }
    }

    printf("====================================================================================\n");
    printf("ID   Name                      Team            Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    for (index = 0; index < count; index++)
    {
        PlayerModel *player = &playersAll[tempIndices[index]];
        char *roleName = (player->role == ROLE_BATSMAN) ? "Batsman" : (player->role == ROLE_BOWLER ? "Bowler" : "All-Rounder");

        printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
               player->id, player->name, player->teamName, roleName, player->runs, player->average, player->strikeRate, player->wickets, player->economy, player->performance);
    }

    printf("====================================================================================\n");
}

void printMenu()
{
    printf("\n1. Display Players of a Specific Team\n");
    printf("2. Display Teams by Average Batting Strike Rate\n");
    printf("3. Display Top K Players of a Specific Team by Role\n");
    printf("4. Display All Players of specific role Across All Teams by performance index\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main()
{
    loadFromHeader();
    buildTeams();

    while (1)
    {
        printMenu();
        int choiceValue = 0;
        int scanfResult = scanf("%d", &choiceValue);

        if (scanfResult != 1)
        {
            printf("Error: Choice must be an integer\n");
            while (getchar() != '\n') { }
            continue;
        }

        if (!validateInput(choiceValue, 1, 5))
        {
            printf("Error: Choice must be between 1 and 5\n");
            continue;
        }

        if (choiceValue == 1)
        {
            DisplayPlayer();
        }
        else if (choiceValue == 2)
        {
            DisplayBYaverageStirke();
        }
        else if (choiceValue == 3)
        {
            DisplayTopKPlayers();
        }
        else if (choiceValue == 4)
        {
            DisplayAllPlayersByRole();
        }
        else if (choiceValue == 5)
        {
            break;
        }
    }

    return 0;
}
