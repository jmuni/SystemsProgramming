Assignment 0: Pointer Sorter

At first I wanted to scanf the input, and store it in a large array.
Then I wanted to read through the entire input until the program reached '\0'
search for separators (non-alphabet characters), and store each word into its own array. 

The problem with this approach was that I didnt know how big the input was 
so I had to arbitrarily initialize the input array with a large number like 100
Also, I didnt know how big each words would be so I would set each word array
to be some arbitrary number such as 10. Lastly when storing these word chars 
into word arrays, printing them out providing some strange symbols.

So I knew I had to take a different approach in storing inputs safely, without
comprising my memory. Instead of using scanf, I ran input as an argument, which 
allows me to check its size then store it accordingly. To seperate the input into
words, I decided to use a struct that recursively calls itself similar to a Node
in a linked list. This way, I use exactly how much the word needs. 

To sort, I use quicksort and partition to correctly sort my seperated words. 
The partition uses a swap method, which basically swaps the value of 2 variables.
It compares the word by using the library function strcmp,
which returns a value that signifies which string is greater. 
Quicksort recursively calls partition to sort all the word nodes in descending order

