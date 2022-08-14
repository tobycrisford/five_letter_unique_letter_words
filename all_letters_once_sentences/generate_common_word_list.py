# -*- coding: utf-8 -*-
"""
Created on Sat Aug 13 11:28:59 2022

@author: tobycrisford

This script extracts only common words
from the files found here: http://storage.googleapis.com/books/ngrams/books/datasetsv2.html
and saves them.
"""

import os
from tqdm import tqdm

frequency_threshold = 500000 #Found by trial and error

common_word_list = []
for f in os.listdir('../../wordle_solver/word_frequencies'):
    with open('../../wordle_solver/word_frequencies/' + f + '/' + f, encoding='utf-8') as file:
        
        frequency_dict = {}
        for line in tqdm(file):
            row = line.split("\t")
            word = row[0]
            underscore_location = word.find('_')
            if not underscore_location == -1:
                word = word[:underscore_location]
            word = word.lower()
            if word in frequency_dict:
                frequency_dict[word] += int(row[2])
            else:
                frequency_dict[word] = int(row[2])
        
        new_common_words = [word for word in frequency_dict if frequency_dict[word] > frequency_threshold]
        common_word_list += new_common_words

print("Found ", len(common_word_list), " common words.")

with open('common_words.txt','w') as f:
    for word in common_word_list:
        try:
            f.write(word + "\n")
        except:
            print("Failed on ", word)