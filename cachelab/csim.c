/*
        A simple implementation of cache mechanism written by Minsu Kang (TA of System Programming 2019 Spring)

                * You can refer this code for your own study, and do not distribute it to any other person.
                * If you cannot understand, please feel free to email me any time.

                        E-Mail: minsu@unist.ac.kr
*/

#include <stdio.h>
#include <stdlib.h>

#include "cachelab.h"

typedef struct ENTRY
{

    // a definition of cache entry

    short valid;
    int tag;
    long long *block;

} entry;

typedef struct CACHE{

    entry ** e;
    int hit;
    int miss;

}cache;


void get_arguments_from_user(int argc, char** argv);
void init(cache* c, int S, int E, int B);


int main(int argc, char **argv)
{
    cache c;
    char parameter[9];

    get_arguments_from_user(argc, argv);
    init(&c, 2, 4, 8);

    printSummary(2, 0, 0);
    return 0;
}

void get_arguments_from_user(int argc, char** argv){

    char ch = 0;

    while ((ch = getopt(argc, argv, "bf:sEbt")) != -1)
    {
        switch (ch)
        {
            case 's':
                
            case 'E':
            
            case 'b':

            case 't':

            case '? ':
                break;
            default:
                break;
        }
    }
}

void init(cache* c, int S, int E, int B)
{
    /*
                function "init" 
                        role: allocate memory space for cache entries and give initial conditions for cache mechanism.

                parameters
                        S: the number of set in cache (i.e., 2^s)
                        E: the number of entry in 'one set'.
                        B: the number of byte per cache block in 'one entry'.
    */

   int s_index = 0, e_index = 0, b_index = 0;

    c->e = (entry **)malloc(sizeof(entry *) * S);

    for (int s_index = 0; s_index < S; s_index++)
        c->e[s_index] = (entry *)malloc(sizeof(entry) * E);
    // dynamically allocate memory space for entries

    if (c->e != NULL)
    {

        for (s_index = 0; s_index < S; s_index++)
        {
            for (e_index = 0; e_index < E; e_index++)
            {

                c->e[s_index][e_index].valid = 0;
                c->e[s_index][e_index].tag = 0;

                for (b_index = 0; b_index < B; b_index++)
                    c->e[s_index][e_index].block[b_index] = 0;
            }
        }
    }
}