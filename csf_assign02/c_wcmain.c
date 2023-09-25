#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
    FILE *fp = NULL;
    // stats (to be printed at end)
    uint32_t total_words = 0;
    uint32_t unique_words = 0;
    unsigned char best_word[MAX_WORDLEN +1] = {'\0'};
    uint32_t best_word_count = 0;

    unsigned char *w = malloc(MAX_WORDLEN + 1);

    struct WordEntry *bucket[HASHTABLE_SIZE] = {NULL};
    struct WordEntry *temp;
    if(argc > 1) {
        fp = fopen(argv[1],"r");
        if(fp == NULL) {
            perror("no file\n");
            return 1;
        }
        while(wc_readnext(fp, w)){
            wc_trim_non_alpha(w);
            wc_tolower(w);
            total_words++;
            temp = wc_dict_find_or_insert(bucket,HASHTABLE_SIZE,w);
            temp->count++;
        }
    } else {
        //TODO


    }




    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (bucket[i] != NULL) {
            temp = bucket[i];
            unique_words++;
            // if (wc_str_compare(temp->word,(const unsigned char *)"dominates") == 0) {
            //   printf("this\n");
            // }
            if (best_word_count < temp->count) {
                best_word_count = temp->count;
                wc_str_copy(best_word,temp->word);
            }
            while(temp->next != NULL) {
                temp = temp->next;
                unique_words++;
                // if (wc_str_compare(temp->word,(const unsigned char *)"dominates") == 0) {
                //   printf("this\n");
                // }
                if (best_word_count < temp->count) {
                    best_word_count = temp->count;
                    wc_str_copy(best_word,temp->word);
                }
            }
        }
    }
    //printf("%d\n\n\n",count);
    printf("Total words read: %u\n", (unsigned int) total_words);
    printf("Unique words read: %u\n", (unsigned int) unique_words);
    printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

    // TODO: make sure file is closed (if one was opened)
    // TODO: make sure memory is freed
    free(w);
    fclose(fp);
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        wc_free_chain(bucket[i]);
    }
    return 0;
}

// TODO: definitions of helper functions