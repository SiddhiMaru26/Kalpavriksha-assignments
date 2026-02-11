#include<stdio.h>

int sumOfAlternateElement(int *array,int size)
{
    int sum=0;
    for(int *index=array; index<array+size; index++)
    {
        if((index-array)%2==0)
        {
            sum+=*(index);
        }
    }

    return sum;
}

int main()
{
    int size;
    printf("Enter the size of an array  ");
    scanf("%d",&size);

    int array[size];
    for(int *index=array; index<array+size; index++)
    {
        scanf("%d",index);
    }

    int answer=sumOfAlternateElement(array,size);
    printf("Sum of alternate elememts  %d",answer);

    return 0;
}
