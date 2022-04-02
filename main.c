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
    unsigned long total_problem_size;
    unsigned long work_done;
    int i;
}info ;

int found = 0;

sem_t mutex;
sem_t found_lock;
sem_t feeder_lock;
sem_t threads_at_work;
sem_t empty;
sem_t full;
sem_t fuck_mutex;
sem_t memory_mutex;

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
        index += 2;
    }
}

long generate_words(int word_length, Queue *work_que){
    unsigned char tmp[word_length+1]; 
    memset(tmp, CHAR_START, sizeof(char) * word_length);

    int k;
    long problem_size = pow(CHAR_END-CHAR_START+1, word_length);

    for (int i = 0; i < problem_size ;i++) {
        

        double tot_mem = get_phys_pages();
        double avail_mem = get_avphys_pages();
        //printf("%f / %f = %f\n", avail_mem, tot_mem, avail_mem / tot_mem);
        if(avail_mem / tot_mem < 0.1){
            printf("PRODUCER WAITING - MEMORY LOW\n");
            sem_wait(&memory_mutex);
            printf("PRODUCER STARTING\n");
        }

        tmp[word_length] = '\0';
        sem_wait(&mutex);
        queue_enqueue(work_que, tmp);
        sem_post(&mutex);
        sem_post(&full);

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
    pid_t id = pthread_self();

    printf("%u started\n", id);
    
    info *args = (info*) arg;
    int threads_local = args->no_threads;
    int target_length = args->target_length;
    char target[target_length];
    memcpy(target, args->global_target, target_length);
    SHA256_CTX ctx;

    while(1){


        sem_wait(&found_lock);
            if(found){
                sem_post(&found_lock);
                printf("%u exited becaause someone else found solution\n", id);
                return 0;
                
            }
        sem_post(&found_lock);

        sem_wait(&fuck_mutex);
        sem_wait(&full);
        sem_wait(&mutex);


            int no_words;
            sem_getvalue(&full, &no_words);
            no_words ++;     
            if(args->i % 10 == 0){
                long int tot_mem = get_phys_pages() * sysconf(_SC_PAGESIZE);
                long int avail_mem = get_avphys_pages() * sysconf(_SC_PAGESIZE);
                printf("%d ord kvar i kön - %f %% klart - MEM %% = %f - id = %u\n", no_words,(100 * (double)args->work_done / args->total_problem_size), 100* ( 1.0 - ((double)avail_mem/tot_mem)),  id);
            }
            args->i += 1;
            int threads_working;
            sem_getvalue(&threads_at_work, &threads_working);
            
            int respons = args->no_threads >= no_words ? no_words : no_words/ (args->no_threads - threads_working);

            if(respons > 1000000){
                respons = 1000000;
            }

            if(respons >= queue_size(args->que)){
                respons = queue_size(args->que);
            }


            
            if(respons != 1000000){
                int mem_mutex_val;
                sem_getvalue(&memory_mutex, &mem_mutex_val);
                if(mem_mutex_val == 0){
                    sem_post(&memory_mutex);
                }
            }

            unsigned char *words[respons];
            args->work_done += respons;
            for(int i = 0; i < respons; i++){

                words[i] = queue_dequeue(args->que);
                if(i != 0){
                    sem_wait(&full);
                }
            
            }


        sem_post(&fuck_mutex);
        sem_post(&mutex);

        printf("id %u cracking with size %d\n", id, respons);
        
        sem_post(&threads_at_work);

        for(int i = 0; i < respons; i++){

            if(i > 10000 && i % 100 == 0){
                int local_found;
                sem_wait(&found_lock);
                local_found = found;
                sem_post(&found_lock);               

                if(local_found){
                    for(int j = i; j < respons; j++){
                        free(words[j]);
                    } 
                    printf("%u exited while working becaause someone else found solution\n", id);
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
                return 0;                    
            }
            free(words[i]);
        }
        sem_wait(&threads_at_work);
     
    }
    printf("%u exited after loop, weird\n",id);
    return 0;
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

    out = fopen("outputdata", "w");
    gereate_out = fopen("generate_out", "w");

    const int word_length = atoi(argv[1]);
    printf("word length is %d\n", word_length);

    signal(SIGINT,sig_handler);
    found = 0;
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
    sem_init(&memory_mutex, 0, 1);


    pthread_t cracker_threads[no_threads];
    Queue *work_que = queue_create();
    
    int target_lenthg = strlen(argv[2]);
    info arguments = {.que = work_que, .global_target = target, .target_length = target_lenthg, .no_threads = no_threads, 
                                                            .total_problem_size = total_problem_size, .work_done = 0};
    
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

    queue_destroy(work_que);
    sem_destroy(&mutex);
    sem_destroy(&found_lock);
    sem_destroy(&feeder_lock);
    sem_destroy(&threads_at_work);
    sem_destroy(&fuck_mutex);
    sem_destroy(&memory_mutex);

    printf("ANSWER WAS %s\n", arguments.answer);
    printf("Generated in %f seconds\n", ((double) arguments.time) / CLOCKS_PER_SEC);
    free(arguments.answer);
    fclose(out);
    fclose(gereate_out);
    return 69;
}