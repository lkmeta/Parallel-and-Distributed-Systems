#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"
#include <stdint.h>
#include <time.h>

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

    coo2csc(csc_row, csc_col, I, J, nnz, n, isOneBased);

    // printf("\nCSC Row: ");
    // for (i = 0; i < nz; i++)
    //     fprintf(stdout, "%d ", csc_row[i]);
    // printf("\nCSC Col: ");
    // for (i = 0; i < n+1; i++)
    //     fprintf(stdout, "%d ", csc_col[i]);

    printf("CSC matrix has been created.\n");

    /* Initialize numOfTriangles counter and c3 vector */

    int numOfTriangles = 0;
    int c3[(int)N];
    for (int i = 0; i < N; i++)
    {
        c3[i] = 0;
    }

    /* Starting the Algorithm that counts the vector c3 */

    printf("Start counting the triangles with V3 algorithm.\n");
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (int i = 1; i < N - 1; i++)
    {
        for (int j = csc_col[i - 1]; j < csc_col[i]; j++)
        {
            for (int k = csc_col[csc_row[j]]; k < csc_col[csc_row[j] + 1]; k++)
            {
                if (k > nnz - 1)
                    break;
                for (int l = j + 1; l < csc_col[i]; l++)
                {
                    if (csc_row[k] == csc_row[l])
                    {
                        c3[i - 1] += 1;
                        c3[csc_row[j]] += 1;
                        c3[csc_row[k]] += 1;
                    }
                }
            }
        }
    }

    /* Ending the Algorithm that counts the vector c3 */

    /* Calculate the execution time for the algorithm */

    double time = calculateExecutionTime();

    // printf("c3 = ");
    // for (int i = 0; i < N; i++)
    // {
    //     printf("%d ", c3[i]);
    // }

    /* Using c3 to calculate the numOfTriangles */

    for (int i = 0; i < N; i++)
    {
        numOfTriangles += c3[i];
    }
    printf("Num of Triangles = %d ", numOfTriangles / 3);
    printf("\nV3 Sequential running time: %f\n", time);

    /* Deallocate used memory */

    free(I);
    free(J);
    free(val);
    free(csc_row);
    free(csc_col);

    return 0;
}
