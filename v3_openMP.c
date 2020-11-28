#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include <pthread.h> 

#define BILLION 1000000000L;

struct timespec start_time;
struct timespec stop_time;

/**
 *  \brief COO to CSC conversion
 *
 *  Converts a square matrix from COO to CSC format.
 *
 *  Note: The routine assumes the input COO and the output CSC matrix
 *  to be square.
 *
 */
void coo2csc(
    uint32_t *const row,           /*!< CSC row start indices */
    uint32_t *const col,           /*!< CSC column indices */
    uint32_t const *const row_coo, /*!< COO row indices */
    uint32_t const *const col_coo, /*!< COO column indices */
    uint32_t const nnz,            /*!< Number of nonzero elements */
    uint32_t const n,              /*!< Number of rows/columns */
    uint32_t const isOneBased      /*!< Whether COO is 0- or 1-based */
)
{

    // ----- cannot assume that input is already 0!
    for (uint32_t l = 0; l < n + 1; l++)
        col[l] = 0;

    // ----- find the correct column sizes
    for (uint32_t l = 0; l < nnz; l++)
        col[col_coo[l] - isOneBased]++;

    // ----- cumulative sum
    for (uint32_t i = 0, cumsum = 0; i < n; i++)
    {
        uint32_t temp = col[i];
        col[i] = cumsum;
        cumsum += temp;
    }
    col[n] = nnz;
    // ----- copy the row indices to the correct place
    for (uint32_t l = 0; l < nnz; l++)
    {
        uint32_t col_l;
        col_l = col_coo[l] - isOneBased;

        uint32_t dst = col[col_l];
        row[dst] = row_coo[l] - isOneBased;

        col[col_l]++;
    }
    // ----- revert the column pointers
    for (uint32_t i = 0, last = 0; i < n; i++)
    {
        uint32_t temp = col[i];
        col[i] = last;
        last = temp;
    }
}

// time calculate fn
double calculateExecutionTime()
{

    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    double dSeconds = (stop_time.tv_sec - start_time.tv_sec);

    double dNanoSeconds = (double)(stop_time.tv_nsec - start_time.tv_nsec) / BILLION;

    return dSeconds + dNanoSeconds;
}


int main(int argc, char *argv[])
{
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;
    int i, *I, *J;
    double *val;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
        exit(1);
    }
    else
    {
        if ((f = fopen(argv[1], "r")) == NULL)
            exit(1);
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
        mm_is_sparse(matcode))
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
        exit(1);

    /* reseve memory for matrices */

    I = (int *)malloc(nz * sizeof(int));
    J = (int *)malloc(nz * sizeof(int));
    val = (double *)malloc(nz * sizeof(double));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    /* Replace missing val column with 1s and change the fscanf to match patter matrices*/

    if (!mm_is_pattern(matcode))
    {
        for (i = 0; i < nz; i++)
        {
            fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]);
            I[i]--; /* adjust from 1-based to 0-based */
            J[i]--;
        }
    }
    else
    {
        for (i = 0; i < nz; i++)
        {
            fscanf(f, "%d %d\n", &I[i], &J[i]);
            val[i] = 1;
            I[i]--; /* adjust from 1-based to 0-based */
            J[i]--;
        }
    }

    if (f != stdin)
        fclose(f);


    const uint32_t nnz = nz;
    const uint32_t n = N;

    uint32_t *csc_row = (uint32_t *)malloc(nnz * sizeof(uint32_t));
    uint32_t *csc_col = (uint32_t *)malloc((n + 1) * sizeof(uint32_t));
    uint32_t isOneBased = 0;

    coo2csc(csc_row, csc_col, I, J, nnz, n, isOneBased);

    // printf("\nCSC Row: ");
    // for (i = 0; i < nz; i++)
    //     fprintf(stdout, "%d ", csc_row[i]);
    // printf("\nCSC Col: ");
    // for (i = 0; i < n+1; i++)
    //     fprintf(stdout, "%d ", csc_col[i]);


    printf("CSC matrix has been created.\n");

    int counter = 0;
    int c3[(int)N];
    for (int w = 0; w < N; w++)
    {
        c3[w] = 0;
    }
    

    int thread_count = 8;
    printf("Start counting the triangles and start the time...\n");
    pthread_mutex_t m; //define the lock
    pthread_mutex_init(&m,NULL);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #pragma omp parallel for schedule(static, 1) num_threads(thread_count)
    for (uint32_t i = 1; i < N - 1; i++)
    {
        // i pointer in csc_col array
        // printf("i = %d\t", i);
        for (uint32_t j = csc_col[i - 1]; j < csc_col[i]; j++)
        {
            // j pointer in (csc_row[i] - csc_row[i-1]) array
            // printf("j = %d", j);
            for (uint32_t k = csc_col[csc_row[j]]; k < csc_col[csc_row[j] + 1]; k++)
            {
                // k pointer in csc_row array
                if (k > nnz - 1)
                    break;
                //printf("k = %d\n", k);
                for (uint32_t l = j + 1; l < csc_col[i]; l++)
                {
                    if (csc_row[k] == csc_row[l])
                    {   
                        #pragma omp atomic
                        counter += 1;
                        // c3[i - 1] += 1;
                        // c3[csc_row[j]] += 1;
                        // c3[csc_row[k]] += 1;
                        //printf("csc_row[k] = %d\t", csc_row[k]);
                        //printf("csc_row[l] = %d\t", csc_row[l]);
                        //printf("\nFound Triangle with nodes: i j k = %d %d %d", i - 1, csc_row[j], csc_row[k]);
                        //printf("\n\n");
                    }
                }
                
                
                
            }
        }
    }
    double time = calculateExecutionTime();

    // printf("c3 = ");
    // for (int w = 0; w < N; w++)
    // {
    //     printf("%d ", c3[w]);
    // }
    printf("Num of Triangles = %d ", counter);

    printf("\nv3 using OpenMP running time: %f\n", time);

    free(I);
    free(J);
    free(val);
    free(csc_row);
    free(csc_col);
    
    return 0;
}
