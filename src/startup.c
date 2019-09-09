#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


static int DIRS_N = 2;

static const char* DIRS[] = {
    "scripts",
    "sprites"
};

void startup()
{
    printf("Creating directories...\n");
    startup_create_directories();
}

void startup_create_directories()
{
    for (int i = 0; i < DIRS_N; i++)
    {
        const char* dirname = DIRS[i];

        struct stat st = {0};

        if (stat(dirname, &st) == -1)
        {
            printf("Creating directory `%s`.\n", dirname);
            mkdir(dirname, 0700);
        }
    }
}
