#include "include/bin_utils.h"


int execute_binary(const char* filename, const char* binary_name)
{
    //int X = open("./output_file.txt", O_WRONLY);

    pid_t fk = fork();
    if (!fk) { /* in child */
            //dup2(X, 1); /* fd 1 is standard output,
             //           so this makes standard out refer to the same file as X  */
   //             close(X);

                /* I'm using execl here rather than exec because
                     *        it's easier to type the arguments. */
                    execl(filename, binary_name, NULL);
                        _exit(127); /* should not get here */
    } else if (fk == -1) {
            /* An error happened and you should do something about it. */
            perror("fork"); /* print an error message */
    }
    //close(X); /* The parent doesn't need this anymore */
    int status;
    wait(&status); //simplest one, man wait for others
    printf("child pid was %d, it exited with %d\n", fk, status);
    exit(0);
}
