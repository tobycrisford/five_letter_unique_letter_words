#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define word_length 5
#define word_number 5

struct WordTree {
  int layer;
  struct WordTree* children[26];
  char word[word_length];
  int all_words_contain[26];
};

void add_word(struct WordTree *word_tree, char word[]) {
  int layer = (*word_tree).layer;
  if (layer == word_length - 1) {
    strcpy((*word_tree).word, word);
  }
  else {
    int index = (int)(word[layer + 1]) % 32 - 1;
    if ((*word_tree).children[index] == NULL) {
        (*word_tree).children[index] = malloc(sizeof(struct WordTree));
        for (int i = 0;i < 26;i++) {
          (*((*word_tree).children[index])).children[i] = NULL;
          (*((*word_tree).children[index])).all_words_contain[i] = 1;
        }
        (*((*word_tree).children[index])).layer = layer + 1;
    }
    add_word((*word_tree).children[index], word);
    int working_containment_check[26];
    for (int i = 0;i < 26;i++) {
      working_containment_check[i] = 0;
    }
    for (int i = layer + 1;i < word_length;i++) {
      working_containment_check[(int) (word[i]) % 32 - 1] = 1;
    }
    for (int i = 0;i < 26;i++) {
      (*word_tree).all_words_contain[i] = (*word_tree).all_words_contain[i] && working_containment_check[i];
    }
  }
}

void solve(struct WordTree *word_tree, int iteration_number, char current_place[], char used_letters[],
           char answer_store[], int *answer_store_index, struct WordTree *top_parent, int order_constraint) {


  /*
  if (iteration_number == 1 && word_tree.layer == -1) {
    printf("%s\n", current_place);

  }
  */

  if ((*word_tree).layer == word_length - 1) {
    if (iteration_number == word_number - 1) {
      for (int i = 0;i < word_length * (word_number-1);i++) {
        answer_store[*answer_store_index] = used_letters[i];
        (*answer_store_index) += 1;
      }
      for (int i = 0;i < word_length;i++) {
        answer_store[*answer_store_index] = (*word_tree).word[i];
        (*answer_store_index) += 1;
      }
    }
    else {
      for (int i = 0;i < word_length;i++) {
        used_letters[iteration_number*word_length + i] = (*word_tree).word[i];
      }
      solve(top_parent, iteration_number+1, (*word_tree).word, used_letters, answer_store, answer_store_index, top_parent, 1);
    }
  }
  else {

    int start_place = 0;
    if (order_constraint) {
      start_place = (int)(current_place[(*word_tree).layer + 1]) % 32 - 1;
    }

    int available[26];
    for (int i = 0;i < 26;i++) {
      available[i] = 1;
    }
    for (int i = 0;i < iteration_number*word_length;i++) {
      available[(int) (used_letters[i]) % 32 - 1] = 0;
    }
    int useable;
    for (int i = start_place;i < 26;i++) {
      if (available[i]) {
        if ((*word_tree).children[i] != NULL) {
            useable = 1;
            if ((*word_tree).layer < word_length - 2) {
                for (int j = 0;j < iteration_number*word_length;j++) {
                  if ((*((*word_tree).children[i])).all_words_contain[(int) (used_letters[j]) % 32 - 1]) {
                    useable = 0;
                    break;
                  }
                }
            }
            if (useable) {
                solve((*word_tree).children[i], iteration_number, current_place, used_letters, answer_store, answer_store_index, top_parent,
                      order_constraint && (i == start_place));
            }
        }
      }
    }
  }
}

void sort_word(char input_array[], char output_array[], int length) {
  if (length == 1) {
    output_array[0] = input_array[0];
    return;
  }
  char small_half[length-1];
  char big_half[length-1];
  int small_counter = 0;
  int big_counter = 0;
  for (int i = 1;i < length;i++) {
    if (input_array[i] < input_array[0]) {
      small_half[small_counter] = input_array[i];
      small_counter++;
    }
    else {
      big_half[big_counter] = input_array[i];
      big_counter++;
    }
  }
  char sorted_small[small_counter+1];
  char sorted_big[big_counter+1];
  if (small_counter > 0) {
    sort_word(small_half, sorted_small, small_counter);
  }
  if (big_counter > 0) {
    sort_word(big_half, sorted_big, big_counter);
  }
  for (int i = 0;i < small_counter;i++) {
    output_array[i] = sorted_small[i];
  }
  output_array[small_counter] = input_array[0];
  for (int i = 0;i < big_counter;i++) {
    output_array[small_counter + 1 + i] = sorted_big[i];
  }
}


int main() {

  printf("Building word tree...\n");
  FILE *file_pointer;
  char buff[100];
  struct WordTree word_tree;
  word_tree.layer = -1;
  for (int i = 0;i < 26;i++) {
    word_tree.children[i] = NULL;
  }
  printf("Parent node initialized\n");
  file_pointer = fopen("words_alpha.txt","r");
  printf("File opened\n");
  char sorted_word[word_length];
  int no_repeats;
  if (file_pointer == NULL) {
    printf("Error opening file");
    return -1;
  }
  else {
    while (1==1) {
      if (fgets(buff, 100, file_pointer) == NULL) {
        break;
      }
      if (strlen(buff) == word_length+1) {
        sort_word(buff, sorted_word, word_length);
        no_repeats = 1;
        for (int i = 1;i < word_length;i++) {
          if (sorted_word[i-1] == sorted_word[i]) {
            no_repeats = 0;
            break;
          }
        }
        if (no_repeats == 1) {
          add_word(&word_tree, sorted_word);
        }
      }
    }
  }

  printf("Tree built.\n");
  printf("Allocating answer store memory...\n");
  char answer_store[250000]; //Space for up to 10,000 solutions if needed
  int answer_store_index = 0;
  char used_letters[25];
  printf("Solving tree...\n");
  solve(&word_tree, 0, "aaaaa", used_letters, answer_store, &answer_store_index, &word_tree, 0);

  printf("Number of sets found: %d", answer_store_index / 25);

  return 0;
}

