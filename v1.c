#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 5
#define BILLION 1000000000L;

struct timespec start_time;
struct timespec stop_time;

double calculateExecutionTime()
{

    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    double dSeconds = (stop_time.tv_sec - start_time.tv_sec);

    double dNanoSeconds = (double)(stop_time.tv_nsec - start_time.tv_nsec) / BILLION;

    return dSeconds + dNanoSeconds;
}

void v1(int *adj, int *c3)
{

    int i, j, k;
    printf("\nV1 task...");

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // look for the triangles
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
            {

                if (*(adj + i * N + j) == 1 && *(adj + j * N + k) == 1 && *(adj + k * N + i) == 1)
                {
                    *(c3 + i) += 1;
                    *(c3 + j) += 1;
                    *(c3 + k) += 1;
                }
                else
                    continue;
            }

    printf("\nc3 = ");
    for (i = 0; i < N; i++)
        printf("%d ", *(c3 + i));

    for (int i = 0; i < N; i++)
        *(c3 + i) = *(c3 + i) / 6;

    printf("\nfinal c3 = ");
    for (i = 0; i < N; i++)
        printf("%d ", *(c3 + i));

    printf("\nV1 run time: %f", calculateExecutionTime());
}

int main()
{
    srand(time(NULL));
    int *adj = (int *)malloc(N * N * sizeof(int));
    int i, j, k;

    int *c3 = (int *)malloc(N * sizeof(int));
    for (i = 0; i < N; i++)
        *(c3 + i) = 0;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
        {

            if (i == j)
                *(adj + i * N + j) = *(adj + j * N + i) = 0;
            else
                *(adj + i * N + j) = *(adj + j * N + i) = rand()%2;
        }
    
    //print the adj matrix 
    printf("The adj matrix elements are:\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d ", *(adj + i * N + j));
        }
        printf("\n");
    }

    for (i = 0; i < N; i++)
        *(c3 + i) = 0;

    v1(adj, c3);
    

    free(adj);
    free(c3);
    return 0;
}