#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0) {
        // Parent process
        close(pipe1[0]);
        close(pipe2[1]);

        int shape_flags[4] = {0}; // circle, triangle, rectangle, square
        float inputs[6] = {0};    // radius, base, height, length, width, side

        printf("Select shapes to calculate area (enter 1 to select):\n");
        printf("Circle   (1/0): ");
        scanf("%d", &shape_flags[0]);
        if (shape_flags[0]) {
            printf("Enter radius of circle: ");
            scanf("%f", &inputs[0]);
        }

        printf("Triangle (1/0): ");
        scanf("%d", &shape_flags[1]);
        if (shape_flags[1]) {
            printf("Enter base and height of triangle: ");
            scanf("%f %f", &inputs[1], &inputs[2]);
        }

        printf("Rectangle (1/0): ");
        scanf("%d", &shape_flags[2]);
        if (shape_flags[2]) {
            printf("Enter length and width of rectangle: ");
            scanf("%f %f", &inputs[3], &inputs[4]);
        }

        printf("Square   (1/0): ");
        scanf("%d", &shape_flags[3]);
        if (shape_flags[3]) {
            printf("Enter side of square: ");
            scanf("%f", &inputs[5]);
        }

        // Send flags and inputs to child
        write(pipe1[1], shape_flags, sizeof(shape_flags));
        write(pipe1[1], inputs, sizeof(inputs));
        close(pipe1[1]);

        float results[4] = {0}; // circle, triangle, rectangle, square
        read(pipe2[0], results, sizeof(results));
        close(pipe2[0]);

        printf("\n--- Area Results ---\n");
        if (shape_flags[0])
            printf("Circle Area   = %.2f\n", results[0]);
        if (shape_flags[1])
            printf("Triangle Area = %.2f\n", results[1]);
        if (shape_flags[2])
            printf("Rectangle Area= %.2f\n", results[2]);
        if (shape_flags[3])
            printf("Square Area   = %.2f\n", results[3]);

    } else {
        // Child process
        close(pipe1[1]);
        close(pipe2[0]);

        int shape_flags[4];
        float inputs[6];

        read(pipe1[0], shape_flags, sizeof(shape_flags));
        read(pipe1[0], inputs, sizeof(inputs));
        close(pipe1[0]);

        float results[4] = {0};

        if (shape_flags[0]) // Circle
            results[0] = M_PI * inputs[0] * inputs[0];
        if (shape_flags[1]) // Triangle
            results[1] = 0.5 * inputs[1] * inputs[2];
        if (shape_flags[2]) // Rectangle
            results[2] = inputs[3] * inputs[4];
        if (shape_flags[3]) // Square
            results[3] = inputs[5] * inputs[5];

        write(pipe2[1], results, sizeof(results));
        close(pipe2[1]);

        exit(0);
    }

    return 0;
}
