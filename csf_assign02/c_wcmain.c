#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  FILE *fp;
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  unsigned char *best_word = (unsigned char *) "";
  uint32_t best_word_count = 0;

  // TODO: implement
  struct WordEntry *bucket[HASHTABLE_SIZE] = {NULL};
  struct WordEntry *temp;
  fp = fopen(argv[1],"r");
  unsigned char *w = malloc(MAX_WORDLEN + 1);
  int uniqueWords = 0;
  while(wc_readnext(fp, w)){
    total_words++;
    temp = wc_dict_find_or_insert(bucket,HASHTABLE_SIZE,w);
    if(temp->count == 0){
      uniqueWords++;
    }
    if(temp->count > best_word_count){
      best_word_count = temp->count;
      best_word = temp->word;
  }

  for (int i = 0; i < HASHTABLE_SIZE; i++) { 
    if (bucket[i] != NULL) {
      temp = bucket[i];
      unique_words++;
      if (best_word_count < temp->count) {
        best_word_count = temp->count;
        wc_str_copy(best_word,temp->word);
      }
       while(temp != NULL) {
        unique_words++;
        if (best_word_count < temp->count) {
          best_word_count = temp->count;
          wc_str_copy(best_word,temp->word);
        }
        temp = temp->next;
      }
    }
  }

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
