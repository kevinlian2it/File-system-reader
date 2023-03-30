#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

// Function to count the number of newline characters in a buffer
int count_newlines(char *buffer, int size) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (buffer[i] == '\n') {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    int pfind_to_sort_pipe[2];
    int sort_to_parent_pipe[2];
    int line_count = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s -d <directory> -p <permissions string> [-h]\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create pipes
    if (pipe(pfind_to_sort_pipe) < 0 || pipe(sort_to_parent_pipe) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Child process 1: execute pfind and write results to pipe
        close(pfind_to_sort_pipe[0]); // Close unused read end
        dup2(pfind_to_sort_pipe[1], STDOUT_FILENO); // Connect pipe to stdout
        close(pfind_to_sort_pipe[1]); // Close original write end
        
	// Execute pfind with the remaining arguments
        char *args[argc];
        args[0] = "./pfind";
        for (int i = 1; i < argc; i++) {
            args[i] = argv[i + 1];
        }
        args[argc] = NULL;
	if (dup2(STDERR_FILENO, STDERR_FILENO) < 0) {
    perror("dup2");
    exit(EXIT_FAILURE);
}

// Execute pfind
if (execvp("./pfind", args) == -1) {
    perror("execvp");
    exit(EXIT_FAILURE);
}
    }
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Child process 2: sort the output of pfind and write to pipe
        close(pfind_to_sort_pipe[1]); // Close unused write end
        dup2(pfind_to_sort_pipe[0], STDIN_FILENO); // Connect pipe to stdin
        close(pfind_to_sort_pipe[0]); // Close original read end

        close(sort_to_parent_pipe[0]); // Close unused read end
        dup2(sort_to_parent_pipe[1], STDOUT_FILENO); // Connect pipe to stdout
        close(sort_to_parent_pipe[1]); // Close original write end

        execlp("sort", "sort", NULL);

        // If execlp returns, an error occurred
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Close unused pipe ends
    close(pfind_to_sort_pipe[0]);
    close(pfind_to_sort_pipe[1]);
    close(sort_to_parent_pipe[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    // Check if child processes terminated normally
    if (!WIFEXITED(status1)) {
        fprintf(stderr, "Error: pfind did not terminate normally\n");
        exit(EXIT_FAILURE);
    }

    // Check if pfind returned non-zero exit status
    if (WEXITSTATUS(status1) != EXIT_SUCCESS) {
        fprintf(stderr, "Error: pfind exited with non-zero status\n");
        exit(EXIT_FAILURE);
    }

    close(pfind_to_sort_pipe[1]);
    close(sort_to_parent_pipe[0]);

    // Read output from sort process and count lines
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(sort_to_parent_pipe[0], buffer, BUFFER_SIZE)) > 0) {
        line_count += count_newlines(buffer, bytes_read);
    }

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", line_count);

    return EXIT_SUCCESS;
}

