/*
        A simple implementation of cache mechanism written by Minsu Kang (TA of System Programming 2019 Spring)

                * You can refer this code for your own study, and do not distribute it to any other person.
                * If you cannot understand, please feel free to email me any time.

                        E-Mail: minsu@unist.ac.kr
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

//#include "cachelab.h"

#define HIT 1
#define MISS 0
#define VERBOSE 1
#define MAX_LONG_LONG_INT 0xffffffff

typedef struct ENTRY
{

    // a definition of cache entry

    short valid;
    unsigned long long int tag;
    int *block;
    long long int time;
} entry;

typedef struct CACHE{

    entry ** e;
    
    int S;
    int B;
    int E;
    
    int t;
    int s;
    int b;
    int v;

    char* trace_filename;
    
    int hit;
    int miss;
    int eviction;

}cache;

void get_arguments_from_user(int argc, char** argv, cache* c);
void show_help(char* filename);
void simulate(cache* c);
void cache_init(cache* c);
void hit_miss_calcualte(cache* c, long long* input_address);



int main(int argc, char **argv)
{
    cache c;

    get_arguments_from_user(argc, argv, &c);
    cache_init(&c);
    
    simulate(&c);

    printf("hit: %d, miss: %d, evictions: %d\n", c.hit, c.miss, c.eviction);
    //printSummary(2, 0, 0);

    return 0;
}

void get_arguments_from_user(int argc, char** argv, cache* c){

    char ch = 0;

    while( (ch = getopt(argc, argv, "h:v:s:E:b:t:")) != -1) {

        switch(ch) { 
            case 'h':
                show_help(argv[0]);      
                break; 
            case 'v':
                c->v = 1;

                break;
            case 's':
                c->s = atoi(optarg);
                c->S = (int)pow(atoi(optarg), 2);
                // S = s^2
                break;
            case 'E':
                c->E = (int)atoi(optarg);
                break;
            case 'b':
                c->b = atoi(optarg);
                c->B = (int)pow(atoi(optarg), 2);
                break;
            case 't':
                c->trace_filename = optarg;
                break;

            default:
                printf("get help? -h will help you!\n");
                break;
        }
    }
    c->t = 64 - (c->s + c->b);
}

void show_help(char* filename){

    printf("\n  %s: Missing required command line argument\n\n", filename);
    printf("  Usage: \n      ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
            
    printf("\n  Options: \n");
    printf("      -h         Print this help message.\n");
    printf("      -v         Optional verbose flag.\n");
    printf("      -s <num>   Number of set index bits.\n");
    printf("      -E <num>   Number of lines per set.\n");
    printf("      -t <file>  Trace file.\n\n");
    
    printf("  Examples:   \n");
    printf("       linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("       linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n\n");
}

void simulate(cache* c){

    char instruction[2]={0}, inst = 0;
    long long address = 0x0;

    int size = 0;


    FILE* fp = fopen(c->trace_filename, "rt");

    if((int)fp == -1){
        printf("[ERROR] File open failed!\n\n");
        exit(0);
    }

    while(fscanf(fp, "%s %llx,%d", instruction, &address, &size)!= EOF){

        inst = instruction[0];

        if(c->v)

        switch(inst){

            case 'S':
                hit_miss_calcualte(c, &address);
                break;
            case 'M':
                hit_miss_calcualte(c, &address);
                hit_miss_calcualte(c, &address);
                break;

            case 'L':
                hit_miss_calcualte(c, &address);
                break;

            default:
                printf("[ERROR] Wrong Instruction!");
                exit(0);
                break;
        }        
    }

    fclose(fp);
}

void hit_miss_calcualte(cache* c, long long* input_address){

    long long input_tag = *input_address >> (c->s + c->b);
    long long s_index = (*input_address >> c->b) & (c->S - 1);
    int hit_or_not = MISS;
    int not_valid_index = -1;
    long long temp = MAX_LONG_LONG_INT;
    long long int min_time_index = -1;

    for(int e_index = 0; e_index < c->E; e_index++){

        entry* ptr = &c->e[s_index][e_index];
        

        if (ptr->valid){
            if(input_tag == ptr->tag){
                c->hit++;
                ptr->time = 0;  
                if(c->v)
                    printf(" hit\n");
    
                hit_or_not = HIT;
            }
        }else{
            not_valid_index = e_index;
        }
    }
    // check hit

    if(hit_or_not == MISS){
        
        if(not_valid_index == -1){

            for(int e_index = 0; e_index < c->E; e_index++){
                entry* ptr = &c->e[s_index][e_index];

                if(ptr->time < temp){
                    min_time_index = e_index;
                    temp = ptr->time;
                }
            }

            c->e[s_index][min_time_index].tag = input_tag;
            c->e[s_index][min_time_index].time = 0;
            c->eviction++;

        // all entries are valid
        }else{
            c->e[s_index][not_valid_index].tag = input_tag;
            c->e[s_index][not_valid_index].time = 0;
            c->e[s_index][not_valid_index].valid = 1;   
        }

        c->miss++;

        if (c->v)
            printf(" miss\n");
    }
    // if miss, change entry (policy: LRU)
}

void cache_init(cache* c)
{
    /*
                function "init" 
                        role: allocate memory space for cache entries and give initial conditions for cache mechanism.
    */

   int s_index = 0, e_index = 0, b_index = 0;

    c->hit = c->miss = c->eviction = 0;

    c->e = (entry **)malloc(sizeof(entry *) * c->S);
    
    for (s_index = 0; s_index < c->S; s_index++)
        c->e[s_index] = (entry *)malloc(sizeof(entry) * c->E);
    // dynamically allocate memory space for entries

    if (c->e != NULL)
    {

        for (s_index = 0; s_index < c->S; s_index++)
        {
            for (e_index = 0; e_index < c->E; e_index++)
            {
                entry* temp = &c->e[s_index][e_index];
                temp->valid = 0;
                temp->tag = 0;
                temp->time = 0;
                
                temp->block = (int*) malloc (sizeof(int) * c->B);
 
                for (b_index = 0; b_index < c->B; b_index++)
                    temp->block[b_index] = 0;
            }
        }
    }else{
        printf("[ERROR] memory allocation failed! \n");
        exit(0);
    }
}
