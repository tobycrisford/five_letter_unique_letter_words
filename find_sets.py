# -*- coding: utf-8 -*-
"""
Created on Fri Aug  5 08:31:38 2022

@author: tobycrisford
"""

from string import ascii_lowercase

letter_order_lookup = {ascii_lowercase[i]: i for i in range(26)}

class WordTree:
    
    def __init__(self, layer=-1):
        
        self.layer = layer
        self.children = [None for i in range(26)]
        
    def add_word(self, word):
        
        if self.layer == 4:
            self.word = word
        else:
            relevant_place = letter_order_lookup[word[self.layer+1]]
            if self.children[relevant_place] is None:
                self.children[relevant_place] = WordTree(layer=self.layer+1)
            self.children[relevant_place].add_word(word)
                
                
    #Assumes unique letters in words
    def solve(self, iteration_number, current_place, used_letters, used_words, answer_store, top_parent, order_constraint):
        
        if self.layer == 4:
            if iteration_number == 4:
                answer_store.append(used_words + [self.word])
                
            else:
                top_parent.solve(iteration_number+1,
                                 self.word,
                                 used_letters.union(set(self.word)),
                                 used_words + [self.word],
                                 answer_store,
                                 top_parent,
                                 True)
                
        else:
            
            if order_constraint:
                start_place = letter_order_lookup[current_place[self.layer+1]]
                if not (self.children[start_place] is None):
                    if not (ascii_lowercase[start_place] in used_letters):
                        self.children[start_place].solve(iteration_number,
                                                         current_place,
                                                         used_letters,
                                                         used_words,
                                                         answer_store,
                                                         top_parent,
                                                         True)
                start_place += 1
                
            else:
                start_place = 0
                    
            for child in range(start_place, 26):
                if not self.children[child] is None:
                    if not (ascii_lowercase[child] in used_letters):
                        self.children[child].solve(iteration_number,
                                                   current_place,
                                                   used_letters,
                                                   used_words,
                                                   answer_store,
                                                   top_parent,
                                                   False)
                        
                        
word_list = open('words_alpha.txt','r').readlines()
word_tree = WordTree()

anagram_lookup = {} #For recovering actual words at the end if we want

for w in word_list:
    word = w[:-1] #Remove newline character
    if len(word) == 5:
        if len(word) == len(set(word)):
            sorted_word = sorted(word) #Sorting removes anagrams
            sorted_word_string = ''.join(sorted_word)
            if not (sorted_word_string in anagram_lookup):
                anagram_lookup[sorted_word_string] = []
            anagram_lookup[sorted_word_string].append(word)
            
            word_tree.add_word(sorted_word)
            
print("Word tree created")
            
answer_store = []
word_tree.solve(0,None,set(),[], answer_store, word_tree, False)