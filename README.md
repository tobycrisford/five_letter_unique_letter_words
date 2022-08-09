# Can you find five five-letter words with twenty-five unique letters?

The problem is to find all sets of five five-letter words containing twenty-five unique letters, while using an unhelpfully large word-list ('words_alpha.txt'). This word list contains 5,977 five letter words with no repeated letters, after removing anagrams. That leads to (5,977 choose 5) possible combinations for a naive brute force approach to check. That's too many to search through in a reasonable time.

This problem is taken from [Matt Parker's video](https://www.youtube.com/watch?v=_-AfhLQfb6w). In the video, he improves the naive brute force search by first finding pairs of words with distinct letters (of which there are just over 3 million), and then looking for pairs of pairs with distinct letters (which then 'only' requires 9 trillion checks). For each of those sets of four you can then search the word list for possible fifth words. This is a big improvement on the naive brute force approach, but his python script still took over 31 days to run. At the end of the video, he shows a faster solution that was sent in by a podcast listener. Their approach is able to find the solution in just 15 minutes on a normal laptop, demonstrating that a relatively fast solution to this problem is possible. They first create a graph where a vertex is created for each of the words, and words are joined by an edge if and only if they share no letters in common. The problem of finding five five-letter words sharing no letters in common is apparently then reduced to a classic problem in graph theory, for which good algorithms already exist.

I was interested in how I would have approached the problem. I don't think I would have created the graph theory solution, but I wondered if I could still make a significant improvement on 31 days. In my approach, we still do a search through all of the possible combinations, but we store the words in a data structure that allows us to efficiently rule out words that have no chance of working, without needing to check every single one. The tree data structure I've used is taken from my solution to the [Countdown Letters round](https://github.com/tobycrisford/countdown) (from 15 years ago).

**On my ordinary laptop, my python script finishes in 22-26 minutes.**

## High level summary of the approach

You can think of this approach as a brute force search through the 5,977^5 possible arrangements of five five-letter words, but with the following improvements:

- We ensure that in each arrangement we check, the words appear in alphabetical order, and that no words repeat. We can do this without loss of generality since the order of the words is not important. This reduces the search space from 5,977^5, to (5,977 choose 5). It's trivial to see that we should find some way of reducing our search space like this, and I implicitly assumed it had been done in the opening paragraph of this readme. But making this reduction in this particular approach is not completely straightforward, so it gets its own bullet point here. The order_constraint argument in the recursive 'solve' function takes care of the requirement that the words appear in alphabetical order.

- The words are stored in a tree structure, in which each node has 26 branches, one for each letter. For example, 'paint' would be located at .->'p'->'a'->'i'->'n'->'t'. This means when we are searching through the possible arrangements, if we have already used the letter 'a', we simply never look down any of the 'a' branches of the tree any more when trying subsequent words. This is where the biggest speedup comes in.

- For a final small speedup, during tree creation, we get each node to remember any letters which all the words that have been sent through it have in common. Then, during the search phase, if we've already used one of these letters, we can also skip that node, even if the letter it corresponds to hasn't yet been used.

## C version

I also implemented an essentially identical algorithm in C, out of curiosity (and as an excuse to learn some C!). **That completes in just over 5 minutes.**
