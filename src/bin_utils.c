#include "include/bin_utils.h"


int execute_binary(const char* filename, const char* binary_name)
{
    pid_t fk = fork();
    if (!fk)
    {
        execl(filename, binary_name, NULL);
    }
    else if (fk == -1)
    {
        perror("fork"); 
    }

    int status;
    wait(&status);
    printf("child pid was %d, it exited with %d\n", fk, status);

    return status;
}
