#include <stdio.h>
#include <omp.h>
#include "time.h"

int main()
{

    srand(time(NULL));
    clock_t start, stop;

    double arr[2001];
    omp_set_num_threads(4);
    double max_val = 0.0;

    int i;
    start = clock();
    #pragma omp parallel for
    for( i=0; i<2000; i++)
        arr[i] = rand() % 100000;

    #pragma omp parallel for reduction(max : max_val)
    for ( i=0;i<2000; i++)
    {
        printf("thread id = %d and i = %d\n", omp_get_thread_num() + 1, i);
        if(arr[i] > max_val)
        {
            max_val = arr[i];
        }
    }
    stop = clock();

    printf("\nmax_val = %f\n", max_val);
    printf("time = %fs\n", (double)(stop - start) / CLOCKS_PER_SEC);
}
