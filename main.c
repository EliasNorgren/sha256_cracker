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
#define THREAD_BUFFERT 100000

typedef struct info
{
    Queue *que;  
    char *global_target;
    int target_length;
    
}info ;

int found = 0;
int feeder_done = 0;
int no_threads;
char *answer;
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
    // printf("wait %d\n", id);
    // sem_wait(&target_lock);
    int threads_local = no_threads;
    info *args = (info*) arg;
    int target_length = args->target_length;
    char target[target_length];
    memcpy(target, args->global_target, target_length);
    // printf("Target = %s\n", target);
    // sem_post(&target_lock);
    // printf("post %d\n", id);

    SHA256_CTX ctx;


    while(1){
        // printf("--iterating--%d\n", id);
        sem_wait(&found_lock);
        if(found){
            sem_post(&found_lock);
            printf("%d exited becaause someone else found solution\n", id);
            pthread_exit(NULL);
            
        }
        sem_post(&found_lock);

        printf("worklock %d\n", id);
        sem_wait(&work_lock);
        int no_words;
        sem_getvalue(&words_in_que, &no_words);
        if(no_words % 100 == 0){
            printf("%d ord kvar i kön \\%d\n", no_words, id);
        }

        // BYTE *word = NULL;
        // int respons = no_words/threads_local;
        int respons = THREAD_BUFFERT;
        if(no_words == 0){

            sem_wait(&feeder_lock);
            if(feeder_done){
                sem_post(&feeder_lock);
                printf("%d exited because queue is empty\n",id);
                pthread_exit(NULL);               
            }else{
                // sem_post(&feeder_lock);
                // sem_post(&work_lock);
                // printf("CONTINU\n");
                // continue;
                sem_wait(&words_in_que);
                respons = 1;
            }
            sem_post(&feeder_lock);
        }
        if(respons >= no_words){
            respons = no_words;
        }
        
        unsigned char *words[respons];
        for(int i = 0; i < respons; i++){
            words[i] = queue_dequeue(args->que);
            sem_wait(&words_in_que);
        }
        sem_post(&work_lock);
        printf("work post %d\n", id);
        


        for(int i = 0; i < respons; i++){
            
            if(i % 1000 == 0){
                printf("%d iteration \\%d\n", i, id);
            }
         
            sha256_init(&ctx);
            sha256_update(&ctx, words[i], strlen((char*)words[i]));
            BYTE buf[SHA256_BLOCK_SIZE];
            sha256_final(&ctx, buf);
            char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
            memset(padded_buf, '\0', 65);
            pad_string(buf, padded_buf);
        
            if(memcmp(target, padded_buf, sizeof(unsigned char) * 64) == 0){
        
                sem_wait(&found_lock);
                found = 1;
                answer = words[i];
                printf("%d exited and found word: %s-------------------------------------------------------\n",id ,  words[i]);
                for(int i = 0; i < no_threads+1; i++){
                    sem_post(&found_lock);
                }
                for(int j = i+1; j < respons; j++){
                    free(words[j]);
                }
                pthread_exit(NULL);                    
            }
            // printf("%d tested %s\n", id , words[i]);
            free(words[i]);
        }
     
    }
    printf("%d exited after loop, weird\n",id);
    pthread_exit(NULL);
    return NULL;
}



void sig_handler(int signum){

  sem_wait(&found_lock);
  found = 1;
  sem_post(&found_lock);
  printf("\nInside handler function\n");
  printf("ANSWER WAS %s\n", answer);
  free(answer);
  exit(1);
}

int main(int argc, char **argv)
{   
    const int word_length = atoi(argv[1]);
    // BYTE target[] = {0x94, 0xee, 0x05, 0x93, 0x35, 0xe5, 0x87, 0xe5, 0x01, 0xcc, 0x4b, 0xf9, 0x06, 0x13, 0xe0,0x81,0x4f,0x00,0xa7,0xb0,0x8b,0xc7,
    //                 0xc6,0x48,0xfd,0x86,0x5a,0x2a,0xf6,0xa2,0x2c, 0xc2};
    signal(SIGINT,sig_handler);
    found = 0;
    //char target[] = {"b66d23c42475dff047d7a2538db25533469bd58a644b8da2a8c3c31c39d42ef4"};
    char *target = argv[2];
    no_threads = atoi(argv[3]);

    sem_init(&work_lock, 0, 1);
    sem_init(&found_lock, 0, 1);
    sem_init(&target_lock, 0, 1);
    sem_init(&words_in_que, 0, 0);
    sem_init(&feeder_lock, 0, 1);


    pthread_t cracker_threads[no_threads];
    Queue *work_que = queue_create();
    
    
    info arguments;
    arguments.que = work_que;
    arguments.global_target = target;
    arguments.target_length = strlen(target);

    for(int i = 0; i < no_threads; i++){
        pthread_create(&cracker_threads[i], NULL, thread_pool, &arguments);        
    }

    long ret = generate_words(word_length, work_que);
    printf("%ld words created by generator\n", ret);
    sem_wait(&feeder_lock);
    feeder_done = 1;
    sem_post(&feeder_lock);


    for(int i = 0; i < no_threads; i++){
        pthread_join(cracker_threads[i], NULL);
    }

//    printf("words = %ld\n", ret);
    queue_destroy(work_que);
    sem_destroy(&work_lock);
    sem_destroy(&found_lock);
    sem_destroy(&words_in_que);
    sem_destroy(&feeder_lock);

    printf("ANSWER WAS %s\n", answer);
    free(answer);

    return 69;
}