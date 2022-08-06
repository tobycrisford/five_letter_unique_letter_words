# -*- coding: utf-8 -*-
"""
Created on Fri Aug  5 08:31:38 2022

@author: tobycrisford
"""

from string import ascii_lowercase
from tqdm import tqdm
import time
import numpy as np

letter_order_lookup = {ascii_lowercase[i]: i for i in range(26)}

class WordTree:
    
    def __init__(self, depth, layer=-1):
        
        self.layer = layer
        self.children = {}
        self.depth = depth
        self.all_words_contain = set(ascii_lowercase)
        
    def add_word(self, word):
        
        if self.layer == self.depth-1:
            self.word = word
        else:
            relevant_letter = word[self.layer+1]
            if not (relevant_letter in self.children):
                self.children[relevant_letter] = WordTree(self.depth, layer=self.layer+1)
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
            
            for child in self.children:
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
                            
                            
class WordTable:
    
    def __init__(self, word_length):
        
        self.letter_buckets = {l: set() for l in ascii_lowercase}
        self.all_words = set()
        self.word_length = word_length
        
    def add_word(self, word):
        word_set = set(word)
        for letter in self.letter_buckets:
            if not (letter in word_set):
                self.letter_buckets[letter].add(word)
        self.all_words.add(word)
                
    def find_words_not_using(self, letters):
        
        letter_sizes = np.array([len(self.letter_buckets[l]) for l in letters])
        size_order = np.argsort(letter_sizes)
        output_words = self.letter_buckets[letters[size_order[0]]]
        for i in size_order[1:]:
            output_words = output_words.intersection(self.letter_buckets[letters[i]])
            
        return output_words
        
    
    def solve(self, iteration_depth, iteration_number=0,letters_so_far=''):
        
        if letters_so_far == '':
            active_word_list = tqdm(list(self.all_words))
        else:
            active_word_list = list(self.find_words_not_using(letters_so_far))
        
        if iteration_number == iteration_depth - 1:
            return list(active_word_list)
        
        output = []
        for word in active_word_list:
            if word > letters_so_far[-self.word_length:]:
                possibles = self.solve(iteration_depth,
                                       iteration_number=iteration_number+1,
                                       letters_so_far=letters_so_far+word)
                output += [word + w for w in possibles]
                
        return output
        
                        

def create_initial_word_structure(structure, filename='words_alpha.txt', as_list=True):

    word_list = open('words_alpha.txt','r').readlines()

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
            
                if as_list:
                    structure.add_word(sorted_word)
                else:
                    structure.add_word(sorted_word_string)
                
    return anagram_lookup


def solve_with_word_tree():
            
    start_time = time.time()
    print("Creating word tree...")
    word_tree = WordTree(5)
    anagram_lookup = create_initial_word_structure(word_tree)
    print("There are", len(anagram_lookup), " words to work through")
    print("Word tree created")

        
    answer_list = []
    word_tree.solve(0,None,set(),[], answer_list, word_tree, False, 5)
    end_time = time.time()
    
    print("Completed in",end_time-start_time," seconds")
    
    return answer_list
    
def solve_with_word_table():
    
    start_time = time.time()
    print("Creating word table...")
    word_table = WordTable(5)
    anagram_lookup = create_initial_word_structure(word_table, as_list=False)
    print("There are", len(anagram_lookup), " words to work through")
    print("Word table created")
    
    answer_list = word_table.solve(5)
    end_time = time.time()
    
    print('Completed in ', end_time-start_time,' seconds')
    
    return answer_list