#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    { // Checking if the right number of arguments are supplied
        perror("Error: Incorrect Number of Arguments");
        return 1;
    }

    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1)
    {
        perror("Error: pipe1 couldn't be created");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        perror("Error: pipe2 couldn't be created");
        return 1;
    }

    int pid1;
    if ((pid1 = fork()) < 0)
    {
        perror("Error: First fork failed");
        return 1;
    }

    if (pid1 == 0)
    {
        // Child Process 1 - ls command
        close(fd2[0]);
        close(fd2[1]);
        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        if (execlp("ls", "ls", "-l", argv[1], NULL) == -1)
        {
            perror("Error: Exec Failed");
        }
        exit(0);
    }

    int status;
    waitpid(pid1, &status, 0);
    if (WEXITSTATUS(status) == 1)
    {
        return EXIT_FAILURE;
    }
    close(fd1[1]);

    int pid2;
    if ((pid2 = fork()) < 0)
    {
        perror("Error: Second fork failed");
        return 1;
    }

    if (pid2 == 0)
    {
        close(fd2[0]);
        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);

        if (execlp("grep", "grep", "^d", NULL) == -1)
        {
            perror("Error: Exec failed");
        }
        exit(0);
    }

    int status2;
    waitpid(pid2, &status2, 0);
    if (WEXITSTATUS(status2) == 1)
    {
        return EXIT_FAILURE;
    }
    close(fd1[0]);
    close(fd2[1]);
    dup2(ds2[0], STDIN_FILENO);
    int temp = 0;
    int count = 0; // since there will always be at least 1 \n
    char buf[2048];
    char args[2048];
    while (temp = read(fd2[0], buf, sizeof(buf)) > 0)
    {
        printf("%s", buf);
        strcat(args, buf);
    }
    char *token = strtok(args, "\n");
    while (token != NULL)
    {
        count++;
        token = strtok(NULL, "\n");
    }
    printf("Total amount of subdirectories is: %d\n", count);
    return 0;
}
