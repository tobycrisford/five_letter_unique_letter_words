# -*- coding: utf-8 -*-
"""
Created on Fri Aug  5 08:31:38 2022

@author: tobycrisford
"""

from string import ascii_lowercase
from tqdm import tqdm
import time

letter_order_lookup = {ascii_lowercase[i]: i for i in range(26)}

#Tree for child keys, really efficient to read out in sorted order
class WordChildLookup:
    
    def __init__(self):
        
        self.left_side = None
        self.right_side = None
        self.key = None
        self.val = None
    
    def add(self,key):
        if self.key is None:
            self.key = key
            return
        if key <= self.key:
            if self.left_side is None:
                self.left_side = WordChildLookup()
            self.left_side.add(key)
            return
        if self.right_side is None:
            self.right_side = WordChildLookup()
        self.right_side.add(key)
        
    def __iter__(self):
        if not (self.left_side is None):
            for key in self.left_side:
                yield key
        if not (self.key is None):
            yield self.key
        if not (self.right_side is None):
            for key in self.right_side:
                yield key
        
    
    
        

class WordTree:
    
    def __init__(self, depth, layer=-1):
        
        self.layer = layer
        self.children = {}
        self.children_keys = WordChildLookup()
        self.depth = depth
        self.all_words_contain = set(ascii_lowercase)
        
    def add_word(self, word):
        
        if self.layer == self.depth-1:
            self.word = word
        else:
            relevant_letter = word[self.layer+1]
            if not (relevant_letter in self.children):
                self.children[relevant_letter] = WordTree(self.depth, layer=self.layer+1)
                self.children_keys.add(relevant_letter)
            self.children[relevant_letter].add_word(word)
        self.all_words_contain = self.all_words_contain.intersection(set(word[self.layer+1:]))
                
                
    #Assumes unique letters in words
    def solve(self, iteration_number, current_place, used_letters, used_words, answer_store, top_parent, order_constraint, iteration_depth):
        
        if iteration_number == 1 and self.layer == -1:
            print(current_place)
        
        if self.layer == self.depth - 1:
            if iteration_number == iteration_depth - 1:
                answer_store.append(used_words + [self.word])
                
            else:
                top_parent.solve(iteration_number+1,
                                 self.word,
                                 used_letters.union(set(self.word)),
                                 used_words + [self.word],
                                 answer_store,
                                 top_parent,
                                 True,
                                 iteration_depth)
                
        else:
            
            if order_constraint:
                start_place = letter_order_lookup[current_place[self.layer+1]]
            else:
                start_place = -1
            
            for child in self.children_keys:
                if not (child in used_letters):
                    if letter_order_lookup[child] >= start_place:
                        if len(used_letters.intersection(self.children[child].all_words_contain)) == 0:
                            self.children[child].solve(iteration_number,
    	                                                current_place,
    	                                                used_letters,
    	                                                used_words,
    	                                                answer_store,
    	                                                top_parent,
    	                                                order_constraint and (letter_order_lookup[child] == start_place),
    	                                                iteration_depth)
                        
                        

def create_initial_wordtree(filename='words_alpha.txt'):

    word_list = open('words_alpha.txt','r').readlines()
    word_tree = WordTree(5)

    anagram_lookup = {} #For recovering actual words at the end if we want

    for w in tqdm(word_list):
        word = w[:-1] #Remove newline character
        if len(word) == 5:
            word_set = set(word)
            if len(word) == len(word_set):
                sorted_word = sorted(word) #Sorting removes anagrams
                sorted_word_string = ''.join(sorted_word)
                if not (sorted_word_string in anagram_lookup):
                    anagram_lookup[sorted_word_string] = []
                anagram_lookup[sorted_word_string].append(word)
            
                word_tree.add_word(sorted_word)
                
    return word_tree, anagram_lookup


if __name__ == "__main__":
            
    start_time = time.time()
    print("Creating word tree...")
    word_tree, anagram_lookup = create_initial_wordtree()
    print("There are", len(anagram_lookup), " words to work through")
    print("Word tree created")

        
    answer_list = []
    word_tree.solve(0,None,set(),[], answer_list, word_tree, False, 5)
    end_time = time.time()
    
    print("Completed in",end_time-start_time," seconds")
