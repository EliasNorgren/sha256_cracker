/*

V!3

Good links fow workking

SHA256 Online
https://emn178.github.io/online-tools/sha256.html

Text comparatOr
https://text-compare.com/

-------------

mutex = 1 
Full = 0 // Initially, all slots are empty. Thus full slots are 0 
Empty = n // All slots are empty initially 

do{

//produce an item

wait(empty);
wait(mutex);

//place in buffer

signal(mutex);
signal(full);

}while(true)

When producer produces an item then the value of “empty” is reduced by 1 because one slot will be filled now. The value of mutex is also reduced to prevent consumer to access the buffer. Now, the producer has placed the item and thus the value of “full” is increased by 1. The value of mutex is also increased by 1 because the task of producer has been completed and consumer can access the buffer. 

Solution for Consumer – 

do{

wait(full);
wait(mutex);

// remove item from buffer

signal(mutex);
signal(empty);

// consumes item

}while(true)



---------


Semaphore algorithm

Initialization of semaphores – 
mutex = 1 
Full = 0 // Initially, all slots are empty. Thus full slots are 0 

Producer{
    do{
    //produce an item

    wait(mutex);
    //place in buffer
    signal(mutex);

    signal(full);

    }while(none left)

    wait(feeder_done)
    feeder_done = true
    signal(feeder_done)

}

Consumer{
    do{
    
    wait(feeder_done);
    if(feeder_doner){
        signal(feeder_done)
        exit
    }
    signal(feeder_done)

    
    wait(fuck_sem)
    wait(full);
    wai(mutex)
    
    X = full + 1

    // remove X item from buffer
    wait(full) X times
    signal(fuck_sem)
    signal(mutex);

    // consumes item

    }while(true)  
}


*/

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
#include <ulimit.h>
#include <time.h>

#include "sha256.h"
#include "queue.h"

#define CHAR_START 33
#define CHAR_END 125
#define THREAD_BUFFERT 100

typedef struct info
{
    Queue *que;  
    const char *global_target;
    const int target_length;
    const int no_threads;
    unsigned char *answer;
    int feeder_done;
    time_t time;
}info ;

int found = 0;
// int feeder_done = 0;
// int no_threads;
// unsigned char *answer;
sem_t mutex;
sem_t found_lock;
sem_t feeder_lock;
sem_t threads_at_work;
sem_t empty;
sem_t full;
sem_t fuck_mutex;


FILE *out;
FILE *gereate_out;

void pad_string(BYTE src[], unsigned char dst[])
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
        //printf("PRODUCER LOCK");
        sem_wait(&mutex);
        queue_enqueue(work_que, tmp);
        sem_post(&mutex);
        sem_post(&full);
       // printf("PRODUCER POST");

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

unsigned long calc_total_problem_size(int word_length){
    unsigned long sum = 0;
    for(int i = 1; i <= word_length; i++){
        sum += pow(CHAR_END-CHAR_START+1, i);
    }
    return sum;
}

void *thread_pool(void * arg)
{   
    //sleep(3);
    pid_t id = pthread_self();

    printf("%u started\n", id);

    
    info *args = (info*) arg;
    int threads_local = args->no_threads;
    int target_length = args->target_length;
    char target[target_length];
    memcpy(target, args->global_target, target_length);
    SHA256_CTX ctx;


    while(1){
        printf("--iterating--%d\n", id);
        sem_wait(&found_lock);
            if(found){
                sem_post(&found_lock);
                printf("%u exited becaause someone else found solution\n", id);
                // pthread_exit(NULL);
                return 0;
                
            }
        sem_post(&found_lock);

        printf("fuck lock %u\n", id);
        sem_wait(&fuck_mutex);
        printf("full lock %u\n", id);
        sem_wait(&full);
        printf("mutex lock %u\n", id);
        sem_wait(&mutex);
            int no_words;
            sem_getvalue(&full, &no_words);
            no_words ++;
            //printf("%u no_words = %d\n", id,no_words);       
            if(no_words % 100 == 0){
                printf("%d ord kvar i kön \\%u\n", no_words, id);
            }
            // int respons = no_words;
            // BYTE *word = NULL;
            int threads_working;
            sem_getvalue(&threads_at_work, &threads_working);
            
            int respons = args->no_threads >= no_words ? no_words : no_words/ (args->no_threads - threads_working);
            //printf("id %u: %d / (%d - %d) = %d\n", id, no_words, args->no_threads, threads_working, respons);
            // //int respons = THREAD_BUFFERT;
            // if(no_words <= 0){
            //     //printf("feeder_lock %u\n", id);  
            //     sem_wait(&feeder_lock);
            //     if(args->feeder_done){
            //         sem_post(&feeder_lock);
            //         sem_post(&mutex);
            //         //printf("feeder unlocked %u\n", id);  
            //         printf("%u exited because feeder is done empty\n",id);
            //         // pthread_exit(NULL);  
            //         return 0;             
            //     }else{
            //         // sem_post(&feeder_lock);
            //         // sem_post(&mutex);
            //         // printf("CONTINU\n");
            //         // continue;
            //         sem_post(&feeder_lock);
            //         respons = 1;
            //     }
                
            // }
            // if(respons >= no_words){
            //     respons = no_words;
            // }
            if(respons > 1000000){
                respons = 1000000;
            }
            //printf("%u Queueu size = %d\n", id, queue_size(args->que));
            if(respons >= queue_size(args->que)){
                respons = queue_size(args->que);
            }
            unsigned char *words[respons];

            //int val;
            //int items = queue_size(args->que);
           // sem_getvalue(&full, &val);
          //  printf("-----%u----que = %d\tfull = %d\n", id, items, val);

            for(int i = 0; i < respons; i++){
                
                //int items = queue_size(args->que);
              //  int val;
                //sem_getvalue(&full, &val);
                //printf("que = %d\tfull = %d\n", items, val);

                words[i] = queue_dequeue(args->que);
                if(i != 0){
                    sem_wait(&full);
                }
            
            }
            //printf("Ending loop\n");

        sem_post(&fuck_mutex);
        sem_post(&mutex);

        printf("id %u cracking with size %d\n", id, respons);
        
        sem_post(&threads_at_work);

        for(int i = 0; i < respons; i++){

            if(i > 10000 && i % 100 == 0){
                //printf("%u found wait\n");
                int local_found;
                sem_wait(&found_lock);
                local_found = found;
                sem_post(&found_lock);               

                if(local_found){
                    for(int j = i; j < respons; j++){
                        free(words[j]);
                    } 
                    printf("%u exited while working becaause someone else found solution\n", id);
                    // pthread_exit(NULL);
                    return 0;
                   
                }
            }
            
            sha256_init(&ctx);
            sha256_update(&ctx, words[i], strlen((char*)words[i]));
            BYTE buf[SHA256_BLOCK_SIZE];
            sha256_final(&ctx, buf);
            unsigned char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
            memset(padded_buf, '\0', sizeof(unsigned char) * 65);
            pad_string(buf, padded_buf);
        
            if(memcmp(target, padded_buf, sizeof(unsigned char) * 64) == 0){
                args->time = clock() - args->time;
                sem_wait(&found_lock);
                args->answer = words[i];
                found = 1;  
                for(int j = i+1; j < respons; j++){
                    free(words[j]);
                } 
                printf("%u exited and found word: %s-------------------------------------------------------\n",id ,  words[i]);
                for(int i = 0; i < threads_local+1; i++){
                    sem_post(&found_lock);
                    sem_post(&fuck_mutex);
                    sem_post(&full);

                }

                // pthread_exit(NULL);
                return 0;                    
            }
            //printf("%u tested %s\n", id , words[i]);
            //fprintf(out, "%u tested %s\n", id , words[i]);
            free(words[i]);
        }
      //  printf("threads at work lock");
        sem_wait(&threads_at_work);
     
    }
    printf("%u exited after loop, weird\n",id);
    // pthread_exit(NULL);
    return 0;
}



void sig_handler(int signum){

  sem_wait(&found_lock);
    found = 1;
  sem_post(&found_lock);
  printf("\nInside handler function\n");
  //printf("ANSWER WAS %s\n", answer);
  
//   for(int i = 0; i < 3; i++){
//       sem_post(&mutex);
//   }
    
  //free(answer);
  exit(1);
}



int main(int argc, char **argv)
{   
    out = fopen("outputdata", "w");
    gereate_out = fopen("generate_out", "w");

    const int word_length = atoi(argv[1]);
    printf("word length is %d\n", word_length);
    // BYTE target[] = {0x94, 0xee, 0x05, 0x93, 0x35, 0xe5, 0x87, 0xe5, 0x01, 0xcc, 0x4b, 0xf9, 0x06, 0x13, 0xe0,0x81,0x4f,0x00,0xa7,0xb0,0x8b,0xc7,
    //                 0xc6,0x48,0xfd,0x86,0x5a,0x2a,0xf6,0xa2,0x2c, 0xc2};
    signal(SIGINT,sig_handler);
    found = 0;
    //char target[] = {"b66d23c42475dff047d7a2538db25533469bd58a644b8da2a8c3c31c39d42ef4"};
    char *target = argv[2];
    unsigned long total_problem_size = calc_total_problem_size(word_length);
    if(total_problem_size == 0){
        fprintf(stderr, "Cant handle size\n");
        exit(0);
    }
    printf("total problem size = %ld\n", total_problem_size);
    int no_threads = atoi(argv[3]);
    
    sem_init(&mutex, 0, 1);
    sem_init(&found_lock, 0, 1);
    sem_init(&feeder_lock, 0, 1);
    sem_init(&threads_at_work, 0, 0);
    sem_init(&empty, 0, total_problem_size);
    sem_init(&full, 0, 0);
    sem_init(&fuck_mutex, 0, 1);


    pthread_t cracker_threads[no_threads];
    Queue *work_que = queue_create();
    
    int target_lenthg = strlen(argv[2]);
    info arguments = {.que = work_que, .global_target = target, .target_length = target_lenthg, .no_threads = no_threads};
    // arguments.que = work_que;
    // arguments.global_target = target;
    // arguments.target_length = strlen(argv[2]);
    
    arguments.time = clock();

    for(int i = 0; i < no_threads; i++){
        pthread_create(&cracker_threads[i], NULL, thread_pool, &arguments);        
    }

    long sum = 0;
    for(int i = 1; i <= word_length; i++){        
        long temp = generate_words(i, work_que);
        printf("Generated %ld words with length %d\n",temp , i);
        sum += temp;
    }
    sem_wait(&feeder_lock);
        arguments.feeder_done = 1;
    sem_post(&feeder_lock);
    printf("----%ld words created by generator feeder done------------\n", sum);

    for(int i = 0; i < no_threads; i++){
        pthread_join(cracker_threads[i], NULL);
    }

//    printf("words = %ld\n", ret);
    queue_destroy(work_que);
    sem_destroy(&mutex);
    sem_destroy(&found_lock);
    sem_destroy(&feeder_lock);
    sem_destroy(&threads_at_work);
    sem_destroy(&fuck_mutex);

    printf("ANSWER WAS %s\n", arguments.answer);
    printf("Generated in %f seconds\n", ((double) arguments.time) / CLOCKS_PER_SEC);
    free(arguments.answer);
    fclose(out);
    fclose(gereate_out);
    return 69;
}