#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
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

/**
 * Function that calculates Execution time in seconds
 */
double calculateExecutionTime()
{

    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    double dSeconds = (stop_time.tv_sec - start_time.tv_sec);

    double dNanoSeconds = (double)(stop_time.tv_nsec - start_time.tv_nsec) / BILLION;

    return dSeconds + dNanoSeconds;
}

/**
 * Mutiply whole matrix in CSC format with vector
 */
void matrixVectorMultiply2(uint32_t *csc_col, uint32_t *csc_row, uint32_t *val, uint32_t *v, uint32_t *c3, int N)
{
    for (int i = 0; i < N; ++i)
    {
        c3[i] = 0;
        for (int j = csc_col[i]; j < csc_col[i + 1]; ++j)
        {
            c3[i] += val[j] * v[csc_row[j]];
        }
    }
}

int main(int argc, char *argv[])
{

    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;
    int *I, *J;
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

    /* reseve memory for COO matrices */

    I = (int *)malloc(nz * sizeof(int));
    J = (int *)malloc(nz * sizeof(int));
    val = (double *)malloc(nz * sizeof(double));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    /* Replace missing val column with 1s and change the fscanf to match patter matrices*/

    if (!mm_is_pattern(matcode))
    {
        for (int i = 0; i < nz; i++)
        {
            fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]);
            I[i]--; /* adjust from 1-based to 0-based */
            J[i]--;
        }
    }
    else
    {
        for (int i = 0; i < nz; i++)
        {
            fscanf(f, "%d %d\n", &I[i], &J[i]);
            val[i] = 1;
            I[i]--; /* adjust from 1-based to 0-based */
            J[i]--;
        }
    }

    if (f != stdin)
        fclose(f);

    /* COO matrix has been created */

    /* reserve memory for CSC matrices */

    const uint32_t nnz = nz;
    const uint32_t n = N;

    uint32_t *csc_row = (uint32_t *)malloc(nnz * sizeof(uint32_t));
    uint32_t *csc_col = (uint32_t *)malloc((n + 1) * sizeof(uint32_t));
    uint32_t isOneBased = 0;

    /**
     * Use I1 and J1 to save COO lower triangular matrix 
     */

    int *I1 = (int *)malloc(nz * sizeof(int));
    int *J1 = (int *)malloc(nz * sizeof(int));

    for (int i = 0; i < nz; ++i)
    {
        I1[i] = I[i];
        J1[i] = J[i];
    }

    coo2csc(csc_row, csc_col, I, J, nz, N, isOneBased);

    /**
     * Use I2 and J2 to save CSC lower triangular matrix 
     */

    int *I2 = (int *)malloc(nz * sizeof(int));
    int *J2 = (int *)malloc(nz * sizeof(int));

    for (int i = 0; i < nz; ++i)
    {
        J2[i] = csc_row[i];
    }
    for (int i = 0; i < N; ++i)
    {
        for (int j = csc_col[i]; j < csc_col[i + 1]; ++j)
        {
            I2[j] = i;
        }
    }

    I = (int *)malloc(2 * nnz * sizeof(int));
    J = (int *)malloc(2 * nnz * sizeof(int));

    /**
     * Merge I1 with I2 and J1 with J2 to create the whole COO matrix
     */

    int index1 = 0;
    int index2 = 0;
    int index = 0;

    while (index1 < nnz && index2 < nnz)
    {
        if (I1[index1] > I2[index2])
        {
            I[index] = I2[index2];
            J[index] = J2[index2];
            ++index2;
        }
        else if (I1[index1] < I2[index2])
        {
            I[index] = I1[index1];
            J[index] = J1[index1];
            ++index1;
        }
        else
        {
            if (J1[index1] > J2[index2])
            {
                I[index] = I2[index2];
                J[index] = J2[index2];
                ++index2;
            }
            else if (J1[index1] < J2[index2])
            {
                I[index] = I1[index1];
                J[index] = J1[index1];
                ++index1;
            }
        }
        index++;
    }

    // Put remaining elements in (I,J)
    if (index1 < nnz)
    {
        for (int k = index1; k < nnz; ++k)
        {
            I[k + nnz] = I1[k];
            J[k + nnz] = J1[k];
        }
    }
    if (index2 < nnz)
    {
        for (int k = index2; k < nnz; ++k)
        {
            I[k + nnz] = I2[k];
            J[k + nnz] = J2[k];
        }
    }

    /**
     * Create CSC of whole matrix from I and J
     */

    int nnz2 = 2 * nnz;
    csc_row = (int *)malloc(nnz2 * sizeof(int));
    csc_col = (int *)malloc((n + 1) * sizeof(int));
    coo2csc(csc_row, csc_col, I, J, nnz2, n, isOneBased);

    printf("CSC matrix has been created.\n");

    /**
     * Find number of triangles using c3 = A.*(A*A)*e/2 formula
     */
    int *value = (int *)malloc(2 * nnz * sizeof(int));
    int *l1 = (int *)malloc(n * sizeof(int));
    int *l2 = (int *)malloc(n * sizeof(int));
    int i1 = 0;
    int i2 = 0;
    int Iindex = 0;
    int Jindex = 0;
    int count = 0;

    int numOfTriangles = 0;
    uint32_t *c3 = (uint32_t *)malloc(N * sizeof(uint32_t));
    uint32_t *v = (uint32_t *)malloc(N * sizeof(uint32_t));

    for (int i = 0; i < N; ++i)
    {
        c3[i] = 0;
        v[i] = 1;
    }

    /* Create mutex to avoid data racing */

    pthread_mutex_t m;
    pthread_mutex_init(&m, NULL);

    /* Change number of workers */

    int numWorkers = atoi(argv[2]);
    //printf("There are %d workers by default.\n",numWorkers);
    __cilkrts_set_param("nworkers", argv[2]);

    numWorkers = __cilkrts_get_nworkers();
    printf("There are %d workers running this algorithm.\n",numWorkers);

    /**
     * Starting the Algorithm that counts the vector c3 
     * Compute C = A.*(A*A) product by finding only non zero
     * C(i,j) elements by searching their indexes in CSC format
    */

    printf("Start counting the triangles with V4 Cilk algorithm.\n");
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    cilk_for (int i = 0; i < N; ++i)
    {   
        pthread_mutex_lock(&m);
        for (int j = csc_col[i]; j < csc_col[i + 1]; ++j)
        {   
            for (int k = 0; k < csc_col[csc_row[j] + 1] - csc_col[csc_row[j]]; ++k)
            {
                l1[k] = csc_row[k + csc_col[csc_row[j]]];
            }

            for (int k = 0; k < csc_col[i + 1] - csc_col[i]; ++k)
            {
                l2[k] = csc_row[k + csc_col[i]];
            }

            /**
             * Find how many elements do l1 and l2 have in common
             * that is the value of C(i,j)
            */

            i1 = 0;
            i2 = 0;
            count = 0;
            while ((i1 < csc_col[csc_row[j] + 1] - csc_col[csc_row[j]]) && (i2 < csc_col[i + 1] - csc_col[i]))
            {   
                if (l1[i1] == l2[i2])
                {   
                    ++count;
                    ++i1;
                    ++i2;
                }
                else if (l1[i1] > l2[i2])
                {
                    ++i2;
                }
                else if (l1[i1] < l2[i2])
                {
                    ++i1;
                }
            }
            value[j] = count;
        }
        pthread_mutex_unlock(&m);
    }

    /* Calculate C * e product*/

    matrixVectorMultiply2(csc_col, csc_row, value, v, c3, N);

    /* Ending the Algorithm that counts the vector c3 */

    /* Calculate the execution time for the algorithm */

    double time = calculateExecutionTime();

    /* Print c3 = (C*e)/2 and total number of triangles */

    // printf("c3 = ");
    // for (int i = 0; i < N; i++)
    // {
    //     printf("%d ", c3[i]/2);
    // }

    /* Using c3 to calculate the numOfTriangles */

    for (int i = 0; i < N; i++)
    {
        numOfTriangles += c3[i] / 2;
    }
    printf("Num of Triangles = %d ", numOfTriangles / 3);
    printf("\nV4 using Cilk running time: %f\n", time);

    /* Deallocate used memory */

    free(I);
    free(J);
    free(I1);
    free(J1);
    free(I2);
    free(J2);
    free(val);
    free(csc_row);
    free(csc_col);
    free(value);
    free(c3);
    free(l1);
    free(l2);
    free(v);

    return 0;
}
