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

#include "sha256.h"
#include "queue.h"

#define CHAR_START 33
#define CHAR_END 125
#define UP_TO 7
#define NO_THREADS 1

typedef struct info
{
    Queue *que;
    int *found;
    char *global_target;
    int target_length;
}info ;


sem_t work_lock;
sem_t found_lock;
sem_t target_lock;

void pad_string(BYTE src[], char dst[])
{
    int index = 0;
    for(int i = 0; i < SHA256_BLOCK_SIZE; i++){
        char temp[7];
        sprintf(temp, "%02hhx", src[i]);
        dst[index] = temp[0];
        dst[index+1] = temp[1];
        printf("%d-%d -> %c%c\n", index, index+1, temp[0], temp[1]);
        index += 2;
    }
}

long generate_words(int word_length, Queue *work_que){
    unsigned char tmp[word_length+1]; 
    memset(tmp, CHAR_START, sizeof(char) * word_length);

    int k;
    long problem_size = pow(CHAR_END-CHAR_START+1, word_length);

    for (int i = 0; i < problem_size ;i++) {
        
        tmp[word_length] = '\0';
        queue_enqueue(work_que, tmp);

        tmp[word_length-1]++;
        k = word_length-1;
        while (k > 0 && tmp[k] == CHAR_END) {
            tmp[k] = CHAR_START; 
            k--; 
            tmp[k]++; 
        }
    }
    return problem_size;
}

void *thread_pool(void * arg)
{   
    sem_wait(&target_lock);
    info *args = (info*) arg;
    int target_length = args->target_length;
    char target[target_length];
    memcpy(target, args->global_target, target_length);
    sem_post(&target_lock);

    SHA256_CTX ctx;
    sha256_init(&ctx);

    while(1){
        
        sem_wait(&found_lock);
        if(args->found){
         pthread_exit(NULL);
        }
        sem_post(&found_lock);



        sem_wait(&work_lock);

        BYTE *word = NULL;
        queue_enqueue(args->que, word);

        sem_post(&work_lock);

        sha256_update(&ctx, word, strlen((char*)word));
        BYTE buf[SHA256_BLOCK_SIZE];
        sha256_final(&ctx, buf);
        char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
        memset(padded_buf, '\0', 65);
        pad_string(buf, padded_buf);
        printf("%s", buf);
    
    }

    
    return NULL;
}

int create_threads(int no_threads, pthread_t *threads, void *arg)
{
    for(int i = 0; i < no_threads; i++){
        if(pthread_create(&threads[i], NULL, thread_pool, arg) != 0){
            fprintf(stderr, "Error pthread_create\n");
            return -1;
        }
    }
    return 1;
}



int main(int argc, char **argv)
{   

    // BYTE target[] = {0x94, 0xee, 0x05, 0x93, 0x35, 0xe5, 0x87, 0xe5, 0x01, 0xcc, 0x4b, 0xf9, 0x06, 0x13, 0xe0,0x81,0x4f,0x00,0xa7,0xb0,0x8b,0xc7,
    //                 0xc6,0x48,0xfd,0x86,0x5a,0x2a,0xf6,0xa2,0x2c, 0xc2};

    char target[] = {"94ee059335e587e501cc4bf90613e0814f00a7b08bc7c648fd865a2af6a22cc2"};

    // SHA256_CTX ctx;
    // sha256_init(&ctx);
    // BYTE name[] = {"TEST"};
    // sha256_update(&ctx, name, 4);
    // BYTE buf[SHA256_BLOCK_SIZE];
    // sha256_final(&ctx, buf);

    // char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
    // memset(padded_buf, '\0', 65);
    // pad_string(buf, padded_buf);

    // printf("%s\n", padded_buf);
    // printf("%s\n", target);


    // printf("\n%s\n", !memcmp(target, padded_buf, 32) ? "SAME\n" : "FAIL\n");
    sem_init(&work_lock, 0, 1);
    sem_init(&found_lock, 0, 1);
    sem_init(&target_lock, 0, 1);


    pthread_t cracker_threads[NO_THREADS];
    Queue *work_que = queue_create();
    
    
    info arguments;
    int found = 0;
    arguments.que = work_que;
    arguments.found = &found;
    arguments.global_target = target;
    arguments.target_length = strlen(target);

    for(int i = 0; i < NO_THREADS; i++){
        


    }

    long ret = generate_words(1, work_que);
    
    while(!queue_is_empty(work_que)){
        free(queue_dequeue(work_que));
    }

    sem_wait(&found_lock);
    found = 1;
    sem_post(&found_lock);

    printf("words = %ld\n", ret);
    queue_destroy(work_que);
    return 69;
}