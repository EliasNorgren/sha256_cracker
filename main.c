#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <ulimit.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

#include "md5.h"

typedef struct info
{
    char **strings;
    const char *target;
    int start_index;
    int end_index;
} info;

int found = 0;

void generate_words(char **strings, int word_length, FILE *input, int rows)
{
    printf("Producer producing\n");

    for (int i = 0; i < rows; i++)
    {
        strings[i] = malloc(sizeof(char) * (word_length + 2));
        fgets(strings[i], word_length + 2, input);

        if (strings[i][0] == '\n')
        {
            continue;
        }

        strings[i][word_length] = '\0';
    }
}

void *thread_pool(void *arg)
{
    struct timeval t0, t1, dt;
    gettimeofday(&t0, NULL);
    pid_t id = pthread_self();
    info *args = (info *)arg;
    printf("%u started doing %d-%d\n", id, args->start_index, args->end_index);

    for (int i = args->start_index; i <= args->end_index; i++)
    {
        // Optional check
        // if (i > 10000 && i % 100 == 0)
        // {
        //     if (found)
        //     {
        //         printf("%u exited while working, someone else found solution\n", id);
        //         return 0;
        //     }
        // }
        unsigned char *digest = md5String(args->strings[i]);

        char res[33];
        for (int i = 0; i < 16; ++i)
        {
            sprintf(&res[i * 2], "%02x", (int)digest[i]);
        }
        free(digest);

        if (strncmp(args->target, res, 32) == 0)
        {
            found = 1;
            gettimeofday(&t1, NULL);
            timersub(&t1, &t0, &dt);
            printf("%u index = %d ANSWER WAS %s took %ld.%06ld sec\n", id, i, args->strings[i], dt.tv_sec, dt.tv_usec);
            return 0;
        }
    }
    printf("%u exiting\n", id);
    return 0;
}

void sig_handler(int signum)
{
    found = 1;
    printf("\nInside handler function\n");
    exit(1);
}

// ./asd hash1 wordslist2 no_words3 wordlengths4

int main(int argc, char **argv)
{

    const int word_length = atoi(argv[4]);
    printf("word length is %d\n", word_length);

    signal(SIGINT, sig_handler);
    found = 0;
    char *target = argv[1];
    unsigned long total_problem_size = atoi(argv[3]);
    if (total_problem_size == 0)
    {
        fprintf(stderr, "Cant handle size\n");
        exit(0);
    }
    printf("total problem size = %ld\n", total_problem_size);

    char **strings = malloc(sizeof(char *) * total_problem_size);

    FILE *word_file = fopen(argv[2], "r");
    generate_words(strings, word_length, word_file, total_problem_size);
    printf("Read words with length %d\n", word_length);
    printf("---- words created by generator feeder done------\n");

    while (true)
    {


        int no_threads;
        printf("Enter no_threads, -1 to exit\n");
        scanf("%d", &no_threads);
        if(no_threads == -1){
            break;
        }
        info args[no_threads];
        pthread_t cracker_threads[no_threads];
        int steps = total_problem_size / no_threads;
        int index = 0;
        for (int i = 0; i < no_threads; i++)
        {
            args[i].strings = strings;
            args[i].target = target;
            if (i == no_threads - 1)
            {
                args[i].start_index = index;
                args[i].end_index = total_problem_size - 1;
            }
            else
            {
                args[i].start_index = index;
                index += steps;
                args[i].end_index = index - 1;
            }

            pthread_create(&cracker_threads[i], NULL, thread_pool, &args[i]);
        }

        for (int i = 0; i < no_threads; i++)
        {
            pthread_join(cracker_threads[i], NULL);       
        }
    }
    printf("Cleaning up\n");
    for (int i = 0; i < total_problem_size; i++)
    {
        free(strings[i]);
    }
    free(strings);
    fclose(word_file);
    return 69;
}