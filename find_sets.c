#include <stdio.h>

static int word_length = 5;
static int word_number = 5;

struct WordTree {
  int layer;
  struct WordTree (*children)[26];
  char word[word_length];
};

void add_word(struct WordTree *word_tree, char word[]) {
  int layer = (*word_tree).layer;
  if (layer == word_length - 1) {
    (*word_tree).word = word;
  }
  else {
    int index = (int)(word[layer + 1]) % 32;
    if ((*word_tree).children[index] == NULL) {
        (*word_tree).children[index] = malloc(sizeof(struct WordTree));
        (*((*word_tree).children[index])).children = {NULL};
    }
    add_word(word_tree.children[index], word);
  }
}

void solve(struct WordTree word_tree, int iteration_number, char[] current_place, char[] *used_letters,
           char[] *answer_store, int *answer_store_index, struct WordTree *top_parent, bool order_constraint) {

  if (iteration_number == 1 && word_tree.layer == -1) {
    printf(current_place);
  }

  if (word_tree.layer == word_length - 1) {
    if (iteration_number == word_number - 1) {
      for (int i = 0;i < word_length*(word_number-1);i++) {
        (*answer_store)[*answer_store_index] = (*used_letters)[i];
        (*answer_store_index) += 1;
      }
      for (int i = 0;i < word_length;i++) {
        (*answer_store)[*answer_store_index] = word_tree.word[i];
        (*answer_store_index) += 1;
      }
    }
    else {
      for (int i = 0;i < word_length;i++) {
        (*used_letters)[iteration_number*word_length + i] = word_tree.word[i];
      }
      solve(*top_parent, iteration_number+1, word_tree.word, used_letters, answer_store, answer_store_index, top_parent, true);
    }
  }
  else {

    int start_place = 0;
    if (order_constraint) {
      start_place = (int)(current_place[word_tree.layer + 1]) % 32;
    }

    bool available[26] = {true};
    for (int i = 0;i < iteration_number*word_length;i++) {
      available[(int) ((*used_letters)[i]) % 32] = false;
    }
    for (int i = start_place;i < 26;i++) {
      if (available[i]) {
        solve(*(word_tree.children[i]), iteration_number, current_place, used_letters, answer_store, answer_store_index, top_parent,
              order_constraint && (i == start_place));
      }
    }
  }
}

int main() {
  printf("Hello World!");
  return 0;
}
