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
#include<signal.h>

#include "sha256.h"
#include "queue.h"

#define CHAR_START 33
#define CHAR_END 125
#define UP_TO 7
#define NO_THREADS 1

typedef struct info
{
    Queue *que;  
    char *global_target;
    int target_length;
}info ;

int found = 0;
int feeder_done = 0;
sem_t work_lock;
sem_t found_lock;
sem_t target_lock;
sem_t words_in_que;
sem_t feeder_lock;

void pad_string(BYTE src[], char dst[])
{
    int index = 0;
    for(int i = 0; i < SHA256_BLOCK_SIZE; i++){
        char temp[7];
        sprintf(temp, "%02hhx", src[i]);
        dst[index] = temp[0];
        dst[index+1] = temp[1];
        //printf("%d-%d -> %c%c\n", index, index+1, temp[0], temp[1]);
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
        sem_wait(&work_lock);
        queue_enqueue(work_que, tmp);
        sem_post(&words_in_que);
        sem_post(&work_lock);

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
    pid_t id = pthread_self();

    printf("%d started\n", id);

    sem_wait(&target_lock);
    info *args = (info*) arg;
    int target_length = args->target_length;
    char target[target_length];
    memcpy(target, args->global_target, target_length);
    printf("Target = %s\n", target);
    sem_post(&target_lock);
    SHA256_CTX ctx;


    while(1){
        sem_wait(&found_lock);
        if(found){
            pthread_exit(NULL);
            printf(" exited\n");
        }
        sem_post(&found_lock);

        
        

        sem_wait(&work_lock);
        BYTE *word = NULL;
        if(queue_is_empty(args->que)){

            sem_wait(&feeder_lock);
            if(feeder_done){
                printf("%d exited without finding word\n",id);
                pthread_exit(NULL);               
            }else{
                continue;
            }
            sem_post(&feeder_lock);
        }
        word = queue_dequeue(args->que);
        sem_wait(&words_in_que);
        sem_post(&work_lock);

        //printf("Testing word %s\n", word);
        //printf("\n");
        //printf("\n");
        //printf("wordLenght = %ld \n", strlen((char*)word));
        sha256_init(&ctx);
        sha256_update(&ctx, word, strlen((char*)word));
        BYTE buf[SHA256_BLOCK_SIZE];
        sha256_final(&ctx, buf);
        char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
        memset(padded_buf, '\0', 65);
        pad_string(buf, padded_buf);
        //printf("%s\n", padded_buf);
        if(memcmp(target, padded_buf, sizeof(unsigned char) * 64) == 0){
        
            sem_wait(&found_lock);
            found = 1;
            printf("%d exited\nWord is %s\n",id ,  word);
            free(word);
            pthread_exit(NULL);
            
            sem_post(&found_lock);
        }
        free(word);
        //printf("----\n");
    }

    pthread_exit(NULL);
    return NULL;
}



void sig_handler(int signum){

  sem_wait(&found_lock);
  found = 1;
  sem_post(&found_lock);
  printf("\nInside handler function\n");
  exit(1);
}

int main(int argc, char **argv)
{   

    // BYTE target[] = {0x94, 0xee, 0x05, 0x93, 0x35, 0xe5, 0x87, 0xe5, 0x01, 0xcc, 0x4b, 0xf9, 0x06, 0x13, 0xe0,0x81,0x4f,0x00,0xa7,0xb0,0x8b,0xc7,
    //                 0xc6,0x48,0xfd,0x86,0x5a,0x2a,0xf6,0xa2,0x2c, 0xc2};
    signal(SIGINT,sig_handler);
    found = 0;
    char target[] = {"f85b917399a6d4f8f17d0fb54025eba154969dc5ee101aee76e2b355df89e7c6"};
    sem_init(&work_lock, 0, 1);
    sem_init(&found_lock, 0, 1);
    sem_init(&target_lock, 0, 1);
    sem_init(&words_in_que, 0, 0);
    sem_init(&feeder_lock, 0, 1);


    pthread_t cracker_threads[NO_THREADS];
    Queue *work_que = queue_create();
    
    
    info arguments;
    arguments.que = work_que;
    arguments.global_target = target;
    arguments.target_length = strlen(target);

    for(int i = 0; i < NO_THREADS; i++){
        pthread_create(&cracker_threads[i], NULL, thread_pool, &arguments);        
    }

    long ret = generate_words(1, work_que);
    
    sem_wait(&feeder_lock);
    feeder_done = 1;
    sem_post(&feeder_lock);


    for(int i = 0; i < NO_THREADS; i++){
        pthread_join(cracker_threads[i], NULL);
    }

//    printf("words = %ld\n", ret);
    queue_destroy(work_que);
    sem_destroy(&work_lock);
    sem_destroy(&found_lock);
    sem_destroy(&words_in_que);
    sem_destroy(&feeder_lock);
    return 69;
}