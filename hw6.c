#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    { // Checking if the right number of arguments are supplied
        perror("Error: Incorrect Number of Arguments");
        return 1;
    }

    chdir(argv[1]); // Changing directory to user specified location from argv
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
        close(fd1[1]);

        if (execlp("ls", "ls", "-l", argv[1], NULL) == -1)
        {
            perror("Error: Exec Failed");
        }
    }

    int pid2;
    if ((pid2 = fork()) < 0)
    {
        perror("Error: Second fork failed");
        return 1;
    }

    if (pid2 == 0)
    {
        close(fd1[1]);
        dup2(fd1[0], STDIN_FILENO);
        close(fd1[0]);
        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[1]);
        if (execlp("grep", "grep", "^d", NULL) == -1)
        {
            perror("Error: Exec failed");
        }
    }

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Working in the main parent process
    char buf[2048];
    int lineCount = 0;
    int temp;

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);

    dup2(fd2[0], STDIN_FILENO);

    char args[2048];
    while (temp = read(fd2[0], buf, sizeof(buf)) > 0)
    {
        printf("%s", buf);
        strcat(args, buf);
    }
    char *token = strtok(args, "\n");
    while (token != NULL)
    {
        lineCount++;
        token = strtok(NULL, "\n");
    }
    printf("Total amount of subdirectories is: %d\n", lineCount);

    return 0;
}
