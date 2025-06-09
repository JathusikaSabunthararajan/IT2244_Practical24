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
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2

        float radius, base, height, length, width, side;
        printf("Enter radius of circle: ");
        scanf("%f", &radius);
        printf("Enter base and height of triangle: ");
        scanf("%f %f", &base, &height);
        printf("Enter length and width of rectangle: ");
        scanf("%f %f", &length, &width);
        printf("Enter side of square: ");
        scanf("%f", &side);

        // Send input values to child
        float inputs[6] = {radius, base, height, length, width, side};
        write(pipe1[1], inputs, sizeof(inputs));
        close(pipe1[1]); // Close write end after writing

        // Read results from child
        float results[4];
        read(pipe2[0], results, sizeof(results));
        close(pipe2[0]); // Close read end after reading

        printf("\n--- Area Results ---\n");
        printf("Circle Area   = %.2f\n", results[0]);
        printf("Triangle Area = %.2f\n", results[1]);
        printf("Rectangle Area= %.2f\n", results[2]);
        printf("Square Area   = %.2f\n", results[3]);
    } else {
        // Child process
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        float inputs[6];
        read(pipe1[0], inputs, sizeof(inputs));
        close(pipe1[0]); // Close read end after reading

        float radius = inputs[0];
        float base = inputs[1];
        float height = inputs[2];
        float length = inputs[3];
        float width = inputs[4];
        float side = inputs[5];

        float results[4];
        results[0] = M_PI * radius * radius;      // Circle
        results[1] = 0.5 * base * height;         // Triangle
        results[2] = length * width;              // Rectangle
        results[3] = side * side;                 // Square

        write(pipe2[1], results, sizeof(results));
        close(pipe2[1]); // Close write end after writing

        exit(0); // End child process
    }

    return 0;
}
