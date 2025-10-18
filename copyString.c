#include<stdio.h>

#define MAX_SIZE 100

void copyString(char *string,char *copiedString)
{
    while(*string!='\0')
    {
        *copiedString=*string;
        string++;
        copiedString++;

    }

    *copiedString='\0';
}

int main()
{
    char string[MAX_SIZE];
    int size=MAX_SIZE;
    printf("Enter the String : ");
    fgets(string,size,stdin);

    char copiedString[size];

    copyString(string,copiedString);
    printf("Copied String :");
    puts(copiedString);

    return 0;
}
