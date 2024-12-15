/*In this lab I am providing you with a code that performs the dot product on two 
matrices.
 I want you to edit this code so that we use multiprocessing (fork) where each 
child calculates a rows of the resulting matrix.
 Additionally, please get the process ID of each child process that we create*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <assert.h>

// Matrix Multiplication function 
void matrix_multiply(int rows_a, int cols_a, int rows_b, int cols_b, int **a, int **b) {
    if (cols_a != rows_b) {
        printf("Dimensions mismatch.\n");
        return;
    }

    // Shared memory for result matrix
    int *shared_result = mmap(NULL, rows_a * cols_b * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_result == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows_a; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            printf("Child process with PID %d is calculating row %d\n", getpid(), i);
            for (int j = 0; j < cols_b; j++) {
                shared_result[i * cols_b + j] = 0;
                for (int k = 0; k < cols_a; k++) {
                    shared_result[i * cols_b + j] += a[i][k] * b[k][j];
                }
            }
            exit(0); // Child exits after finishing its row
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < rows_a; i++) {
        wait(NULL);
    }

    // Print the result matrix
    printf("Result Matrix:\n");
    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < cols_b; j++) {
            printf("%d ", shared_result[i * cols_b + j]);
        }
        printf("\n");
    }

    // Unmap shared memory
    munmap(shared_result, rows_a * cols_b * sizeof(int));
}

int main() {
    int rows_a = 3, cols_a = 4;
    int rows_b = 4, cols_b = 3;

    // Allocate and initialize matrix A
    int **a = (int **)malloc(rows_a * sizeof(int *));
    for (int i = 0; i < rows_a; i++) {
        a[i] = (int *)malloc(cols_a * sizeof(int));
        assert(a[i] != NULL);
    }
    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < cols_a; j++) {
            a[i][j] = i + 1 + j; // Example values
        }
    }
    //Prints Matrix A
    printf("Matrix A:\n");
    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < cols_a; j++) {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }

    // Allocate and initialize matrix B
    int **b = (int **)malloc(rows_b * sizeof(int *));
    for (int i = 0; i < rows_b; i++) {
        b[i] = (int *)malloc(cols_b * sizeof(int));
        assert(b[i] != NULL);
    }
    for (int i = 0; i < rows_b; i++) {
        for (int j = 0; j < cols_b; j++) {
            b[i][j] = i + 10 + j; // Example values
        }
    }
    // Prints Matrix B
    printf("Matrix B:\n");
    for (int i = 0; i < rows_b; i++) {
        for (int j = 0; j < cols_b; j++) {
            printf("%d ", b[i][j]);
        }
        printf("\n");
    }

    // Perform matrix multiplication
    matrix_multiply(rows_a, cols_a, rows_b, cols_b, a, b);

    // Free allocated memory
    for (int i = 0; i < rows_a; i++) {
        free(a[i]);
    }
    free(a);

    for (int i = 0; i < rows_b; i++) {
        free(b[i]);
    }
    free(b);

    return 0;
}
