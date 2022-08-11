#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define letter_total 26

struct WordTree {
  int layer;
  struct WordTree* children[26];
  int contains_word;
  int all_words_contain[26];
};

//This pair of functions used to map lower-case characters to array indices
int char_to_number(char c) {
    return (int) (c) % 32 - 1;
}

char number_to_char(int n) {
    return (char) (97 + n);
}

void update_containment_info(struct WordTree *word_tree, char new_word[], int new_word_length) {
    int working_containment_check[26];
    for (int i = 0;i < 26;i++) {
      working_containment_check[i] = 0;
    }
    for (int i = 0;i < new_word_length;i++) {
      working_containment_check[char_to_number(new_word[i])] = 1;
    }
    for (int i = (*word_tree).layer;i < 26;i++) {
      (*word_tree).all_words_contain[i] = (*word_tree).all_words_contain[i] && working_containment_check[i];
    }
}

void add_word(struct WordTree *word_tree, char word[], int word_length) {
  int layer = (*word_tree).layer;
  if (layer == word_length - 1) {
    (*word_tree).contains_word = 1;
  }
  else {
    int index = char_to_number(word[layer+1]);
    if ((*word_tree).children[index] == NULL) {
        (*word_tree).children[index] = malloc(sizeof(struct WordTree));
        for (int i = 0;i < 26;i++) {
          (*((*word_tree).children[index])).children[i] = NULL;
          (*((*word_tree).children[index])).all_words_contain[i] = 1;
        }
        (*((*word_tree).children[index])).layer = layer + 1;
        (*((*word_tree).children[index])).contains_word = 0;
    }
    add_word((*word_tree).children[index], word, word_length);
  }
  update_containment_info(word_tree, word, word_length);
}

void solve(struct WordTree *word_tree, int letter_number, int space_number, char current_place[], int current_place_length, char used_letters[],
           char answer_store[], int *answer_store_index, struct WordTree *top_parent, int order_constraint) {

  
  int layer = (*word_tree).layer;
  
  if (space_number == 2 && layer == -1) {
    char print_output[current_place_length + 1];
    memcpy(print_output, current_place, current_place_length);
    print_output[current_place_length] = '\0';
    printf("%s\n", print_output);
  }

  if (letter_number + 1 == letter_total) {
    if ((*word_tree).contains_word) {
        for (int i = 0;i < letter_total + space_number;i++) {
            answer_store[*answer_store_index] = used_letters[i];
            (*answer_store_index) += 1;
        }
        answer_store[*answer_store_index] = ' ';
        answer_store[*answer_store_index + 1] = ' ';
        (*answer_store_index) += 2;
        return;
    }
  }

  else {

    if ((*word_tree).contains_word) {
        char new_current_place[layer + 1];
        for (int i = 0;i < layer + 1;i++) {
            new_current_place[i] = used_letters[(letter_number + space_number - layer) + i];
            used_letters[letter_number + space_number + 1] = ' ';
        }
        solve(top_parent, letter_number, space_number + 1, new_current_place, layer + 1, used_letters, answer_store, answer_store_index, top_parent, 1);
    }

    int start_place = 0;
    if (order_constraint) {
      if (layer + 1 < current_place_length) {
        start_place = char_to_number(current_place[layer + 1]);
      }
    }

    int available[26];
    for (int i = 0;i < 26;i++) {
      available[i] = 1;
    }
    for (int i = 0;i < letter_number + space_number + 1;i++) {
      if (used_letters[i] != ' ') {
        available[char_to_number(used_letters[i])] = 0;
      }
    }
    int useable;
    for (int i = start_place;i < 26;i++) {
      if (available[i]) {
        if ((*word_tree).children[i] != NULL) {
            useable = 1;
            for (int j = 0;j < letter_number + space_number + 1;j++) {
                if (used_letters[j] != ' ') {
                    if ((*((*word_tree).children[i])).all_words_contain[char_to_number(used_letters[j])]) {
                        useable = 0;
                        break;
                    }
                }
            }
            if (useable) {
                used_letters[letter_number + space_number + 1] = number_to_char(i);
                solve((*word_tree).children[i], letter_number + 1, space_number, current_place, current_place_length, used_letters, answer_store, answer_store_index, top_parent,
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
    word_tree.contains_word = 0;
    printf("Parent node initialized\n");
    file_pointer = fopen("words_alpha.txt","r");
    printf("File opened\n");
    char sorted_word[100];
    int no_repeats;
    int word_counter = 0;
    int word_length;
    if (file_pointer == NULL) {
        printf("Error opening file");
        return -1;
    }
    while (1==1) {
        if (fgets(buff, 100, file_pointer) == NULL) {
            break;
        }
        word_length = strlen(buff) - 1;
        sort_word(buff, sorted_word, word_length);
        no_repeats = 1;
        for (int i = 1;i < word_length;i++) {
            if (sorted_word[i-1] == sorted_word[i]) {
                no_repeats = 0;
                break;
            }
        }
        if (no_repeats == 1) {
            add_word(&word_tree, sorted_word, word_length);
            word_counter++;
        }
    }

    printf("Tree built with %d words.\n", word_counter);
    printf("Allocating answer store memory...\n");
    char *answer_store = malloc(27000000 * sizeof(char)); //Space for up to a million solutions if needed (slightly less now spaces added)
    int answer_store_index = 0;
    char used_letters[2 * letter_total];
    printf("Solving tree...\n");
    solve(&word_tree, -1, 0, "", 0, used_letters, answer_store, &answer_store_index, &word_tree, 0);

    char output[answer_store_index];
    memcpy(output, answer_store, answer_store_index);
    printf("%s\n", output);
    

    return 0;
}

