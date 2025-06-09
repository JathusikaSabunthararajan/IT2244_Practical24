#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;

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
        close(pipe1[0]); // close read end of pipe1
        close(pipe2[1]); // close write end of pipe2

        char choice;
        float input1, input2;
        float area;

        while (1) {
            printf("Area calculation process\n");
            printf("Circle -> C\n");
			printf("Triangle-> T\n") ;  
			printf("Square-> S\n") ;  
			printf("Rectangle-> R\n");
			printf("Exit-> E\n");
            printf("Enter your choice: ");
            scanf(" %c", &choice);
            choice = toupper(choice);

            if (choice == 'E') {
                // Send exit signal to child
                char exit_signal = 'E';
                write(pipe1[1], &exit_signal, sizeof(exit_signal));
                break;
            }

            // Send the choice to child
            write(pipe1[1], &choice, sizeof(choice));

            switch (choice) {
                case 'C':
                    printf("circle area calculation\nEnter the radius: ");
                    scanf("%f", &input1);
                    // send input1
                    write(pipe1[1], &input1, sizeof(input1));
                    break;

                case 'T':
                    printf("triangle area calculation\nEnter base and height: ");
                    scanf("%f %f", &input1, &input2);
                    write(pipe1[1], &input1, sizeof(input1));
                    write(pipe1[1], &input2, sizeof(input2));
                    break;

                case 'R':
                    printf("rectangle area calculation\nEnter length and width: ");
                    scanf("%f %f", &input1, &input2);
                    write(pipe1[1], &input1, sizeof(input1));
                    write(pipe1[1], &input2, sizeof(input2));
                    break;

                case 'S':
                    printf("square area calculation\nEnter the side: ");
                    scanf("%f", &input1);
                    write(pipe1[1], &input1, sizeof(input1));
                    break;

                default:
                    printf("Invalid choice! Try again.\n");
                    continue;  // prompt again
            }

            // Read the area result from child
            read(pipe2[0], &area, sizeof(area));
            printf("Area is: %.2f\n\n", area);
        }

        close(pipe1[1]);
        close(pipe2[0]);

    } else {
        // Child process
        close(pipe1[1]); // close write end of pipe1
        close(pipe2[0]); // close read end of pipe2

        char choice;
        float input1, input2, area;

        while (1) {
            // read choice from parent
            int r = read(pipe1[0], &choice, sizeof(choice));
            if (r <= 0) break;  // pipe closed or error

            if (choice == 'E') {
                break;  // exit loop
            }

            switch (choice) {
                case 'C':
                    read(pipe1[0], &input1, sizeof(input1));
                    area = M_PI * input1 * input1;
                    break;

                case 'T':
                    read(pipe1[0], &input1, sizeof(input1));
                    read(pipe1[0], &input2, sizeof(input2));
                    area = 0.5 * input1 * input2;
                    break;

                case 'R':
                    read(pipe1[0], &input1, sizeof(input1));
                    read(pipe1[0], &input2, sizeof(input2));
                    area = input1 * input2;
                    break;

                case 'S':
                    read(pipe1[0], &input1, sizeof(input1));
                    area = input1 * input1;
                    break;

                default:
                    area = 0;  // Invalid input, return 0
                    break;
            }

            // send the calculated area back to parent
            write(pipe2[1], &area, sizeof(area));
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    return 0;
}
