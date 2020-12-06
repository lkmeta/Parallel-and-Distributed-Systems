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

struct args {
    uint32_t i;
    uint32_t *csc_col;
    uint32_t *csc_row;
    uint32_t *value
};

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

// CSC format for lower half of matrix and v = [1 ... 1]
void matrixVectorMultiply(uint32_t *csc_col, uint32_t *csc_row, uint32_t *val, uint32_t *c3, uint32_t N)
{

    c3[0] = 0;
    for (uint32_t j = csc_col[0]; j < csc_col[1]; ++j)
    {
        c3[0] += val[j];
        // printf("\tj = %d\tval[%d] = %d\tc3[%d] = %d\n", j, j, val[j], 0, c3[0]);
    }

    for (uint32_t i = 1; i < N; ++i)
    {
        c3[i] = 0;
        // printf("i = %d\n", i);
        // printf("cscol[i] = %d\n", csc_col[i]);
        for (uint32_t j = csc_col[i]; j < csc_col[i + 1]; ++j)
        {
            c3[i] += val[j];
            // printf("\tj = %d\tval[%d] = %d\tc3[%d] = %d\n", j, j, val[j], i, c3[i]);
        }

        // printf("\n");
        for (uint32_t j = 0; j < csc_col[i]; ++j)
        {
            if (csc_row[j] == i)
            {
                c3[i] += val[j];
            }
            // printf("\tj = %d\tval[%d] = %d\tc3[%d] = %d\n", j, j, val[j], i, c3[i]);
        }
    }
    
    for (uint32_t i = 0; i < N; ++i)
    {
        c3[i] = c3[i]/2;
    }

}

// CSC format for whole matrix
void matrixVectorMultiply2(uint32_t *csc_col, uint32_t *csc_row, uint32_t *val, uint32_t* v, uint32_t *c3, uint32_t N)
{
    for(uint32_t i = 0; i < N; ++i) {
        c3[i] = 0;
        for(uint32_t j = csc_col[i]; j < csc_col[i+1]; ++j) {
            c3[i] += val[j] * v[csc_row[j]];
        }
    }
}


void calculateTriangles(void *input) {
    uint32_t i = ((struct args*)input)->i;
    uint32_t *csc_col = ((struct args*)input)->csc_col;
    uint32_t *csc_row = ((struct args*)input)->csc_row;
    uint32_t *value = ((struct args*)input)->value;

    printf("Inside function\n");
    printf("i = %u\n", i);

    uint32_t *l1 = NULL;
    uint32_t *l2 = NULL;
    for (uint32_t j = csc_col[i]; j < csc_col[i+1]; ++j) {
        // pthread_mutex_lock(&p);
        // Iindex = csc_row[j];
        // Jindex = i;
        //printf("i = %d, j = %d\n", Iindex, Jindex);

        // Get A(:,Iindex) list
        // printf("l1 = ");
        for (uint32_t k = 0; k < csc_col[csc_row[j]+1] - csc_col[csc_row[j]]; ++k) {
            l1[k] = csc_row[k+csc_col[csc_row[j]]];
            // printf("%d(k=%d) ", l1[k], k);
        }
        
        // printf("\n");
        
        // Get A(:,Jindex) list
        // printf("l2 = ");
        for (uint32_t k = 0; k < csc_col[i+1] - csc_col[i]; ++k) {
            l2[k] = csc_row[k+csc_col[i]];
            // printf("%d(k=%d) ", l1[k], k);
        }
        // printf("\n");

        // Find how many elements do l1 and l2 have in common
        // that is the value of C(i,j)
        uint32_t i1 = 0;
        uint32_t i2 = 0;
        uint32_t count = 0;
        while((i1 < csc_col[csc_row[j]+1]-csc_col[csc_row[j]]) && (i2 < csc_col[i+1]-csc_col[i])) {
            if(l1[i1] == l2[i2]) {
                ++count;
                ++i1;
                ++i2;
            } else if(l1[i1] > l2[i2]) {
                ++i2;
            } else if(l1[i1] < l2[i2]) {
                ++i1;
            }
        }
        value[j] = count;
        // printf("C[%d,%d] = %d\n", Iindex, Jindex, value[j]);
        // printf("\n");
        // pthread_mutex_unlock(&p);
    }
    printf("Leaving\n");
}


uint32_t main(uint32_t argc, char *argv[])
{
    //===================================================================================//
    // readmtx
    // begin
    //===================================================================================//
    uint32_t ret_code;
    MM_typecode matcode;
    FILE *f;
    uint32_t M, N, nz;
    uint32_t i, *I, *J;
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

    I = (uint32_t *)malloc(nz * sizeof(uint32_t));
    J = (uint32_t *)malloc(nz * sizeof(uint32_t));
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

    //===================================================================================//
    // readmtx
    // end
    //===================================================================================//


    
    //===================================================================================//
    // Convert COO of lower-half to CSC of whole matrix
    // begin
    //===================================================================================//
    const uint32_t nnz = nz;
    const uint32_t n = N;

    // printf("%d\n", nnz);
    uint32_t *csc_row = (uint32_t *)malloc(nnz * sizeof(uint32_t));
    uint32_t *csc_col = (uint32_t *)malloc((n + 1) * sizeof(uint32_t));
    uint32_t isOneBased = 0;

    uint32_t* I1 = (uint32_t *)malloc(nz * sizeof(uint32_t));
    uint32_t* J1 = (uint32_t *)malloc(nz * sizeof(uint32_t));

    uint32_t* I2 = (uint32_t *)malloc(nz * sizeof(uint32_t));
    uint32_t* J2 = (uint32_t *)malloc(nz * sizeof(uint32_t));

    for(uint32_t i = 0; i < nz; ++i) {
        I1[i] = I[i];
        J1[i] = J[i];
    }

    coo2csc(csc_row, csc_col, I, J, nz, N, isOneBased);

    for(uint32_t i = 0; i < nz; ++i) {
        J2[i] = csc_row[i];
    }
    for(uint32_t i = 0; i < N; ++i) {
        for(uint32_t j = csc_col[i]; j < csc_col[i+1]; ++j) {
            I2[j] = i;
        }
    }

    I = (uint32_t*)malloc(2*nnz*sizeof(uint32_t));
    J = (uint32_t*)malloc(2*nnz*sizeof(uint32_t));

    // // Merge (I1,J1) and (I2,J2) into (I, J) list
    uint32_t index1 = 0;
    uint32_t index2 = 0;
    uint32_t index = 0;

    while(index1 < nnz && index2 < nnz) {
        if(I1[index1] > I2[index2]) {
            I[index] = I2[index2];
            J[index] = J2[index2];
            ++index2;
        } else if(I1[index1] < I2[index2]) {
            I[index] = I1[index1];
            J[index] = J1[index1];
            ++index1;
        } else {
            if(J1[index1] > J2[index2]) {
                I[index] = I2[index2];
                J[index] = J2[index2];
                ++index2;
            } else if(J1[index1] < J2[index2]) {
                I[index] = I1[index1];
                J[index] = J1[index1];
                ++index1;
            }
        }
        index++;
        // printf("index1 = %d\n", index1);
        // printf("index2 = %d\n", index2);
        // printf("index = %d\n", index);
    }

    // Put remaining elements in (I,J)
    if(index1 < nnz) {
        for(uint32_t k = index1; k < nnz; ++k) {
            I[k+nnz] = I1[k];
            J[k+nnz] = J1[k];
        }
    }
    if(index2 < nnz) {
        for(uint32_t k = index2; k < nnz; ++k) {
            I[k+nnz] = I2[k];
            J[k+nnz] = J2[k];
        }
    }

    // Now create CSC of whole matrix
    // from COO of whole matrix
    uint32_t nnz2 = 2 * nnz;
    csc_row = (uint32_t*)malloc(nnz2*sizeof(uint32_t));
    csc_col = (uint32_t*)malloc((n+1)*sizeof(uint32_t));
    coo2csc(csc_row, csc_col, I, J, nnz2, n, isOneBased);

    printf("\nCSC matrix has been created.\n");

    //===================================================================================//
    // Find number of triangles using c3 = A.*(A*A)*e/2  formula
    // begin
    //===================================================================================//
    uint32_t *value = (uint32_t*)malloc(2*nnz*sizeof(uint32_t));
    uint32_t* l1 = (uint32_t*)malloc(n*sizeof(uint32_t));
    uint32_t* l2 = (uint32_t*)malloc(n*sizeof(uint32_t));
    uint32_t i1 = 0;
    uint32_t i2 = 0;
    uint32_t Iindex = 0;
    uint32_t Jindex = 0;
    uint32_t count = 0;

    uint32_t* c3 = (uint32_t*)malloc(N*sizeof(uint32_t));
    uint32_t* v = (uint32_t*)malloc(N*sizeof(uint32_t));

    for(uint32_t i = 0; i < N; ++i)
        v[i] = 1;
    printf("Start counting the triangles and start the time...\n");

    // Compute C = A.*(A*A) product by finding only non zero
    // C(i,j) elements by searching their indexes in CSC format
    
    struct args *data = (struct args *)malloc(sizeof(struct args));
    data->csc_row = csc_row;
    data->csc_col = csc_col;
    data->value = value;

    // user defined number of threads
    int NUM_THREADS = atoi(argv[2]);

    pthread_t threads[NUM_THREADS];
    printf("Number of threads: %d\n", NUM_THREADS);
    
    int rc;
    long t;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    // Find non zero elements of C=A.*(A*A)
    // Parallelize for each column with NUM_THREADS at a time
    for(int i = 0; i < N; i = i + NUM_THREADS) {
        for(t = 0; t < NUM_THREADS; ++t) {
            data->i = (i+t);
            printf("In main: creating thread %ld\n", t);
            rc = pthread_create(&threads[t], NULL, calculateTriangles, (void *)data);
            if (rc){    
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
            pthread_join(&threads[t], NULL);
        }
     }

    // Find C*e product
    matrixVectorMultiply2(csc_col, csc_row, value, v, c3, N);
    
    double time = calculateExecutionTime();

    // Find c3 = (C*e)/2 and total number of triangles
    uint32_t sum = 0;
    printf("c3 = ");
    for (uint32_t w = 0; w < N; w++)
    {
        printf("%d ", c3[w]/2);
        sum += c3[w]/2;
    }

    printf("\nNum of Triangles = %d ", sum/3);

    printf("\nv4 using pthreads running time: %f\n", time);
    printf("\n===========================================\n");

    //===================================================================================//
    // Find number of triangles using c3 = A.*(A*A)*e/2  formula
    // end
    //===================================================================================//

    // deallocate memory use
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

    /* Last thing that main() should do */
    pthread_exit(NULL);
    return 0;
}
