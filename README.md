# What is it?
  This is a Deterministic Turing Machine Emulation coded using cpp. Feed it a transition table text file that describes a language, and type some string into the program, it will tell you if the input string is in the language or not or it will loop forever. 

  If your transition table describes a Recursively Enumerable Language, then for input strings that are in the language, the program will accept and halt. But for strings not in the Language, there's no gurantee that the program will halt and reject. It may run forever in a loop calculating.

  If your transition table describes a Recursive Language, then for strings that are in the Language, the program will accept and halt. And for strings not in the language, it will reject and halt. In this case, the program ALWAYS halt!

# How to run
```cpp
g++ -o test TuringMachine.cpp
./test input.txt
```

# Transition input text file format
* each line must contain exactly 5 non-space char
    - Line 1: "a b c d e"
    - Line 2: "a b c d e"
    - etc...
* a is the current state
    - initial state must be '0'
* b is read_symbol  (Any non-space character)
* c is next state   
* d is write_symbol (Any non-space character)
* e is direction
    - must be 'L' or 'R'
* Important tape symbols
    - 'B' is the blank symbol
* Important state 
    - '0' is the initial state
    = 'f' is the final accepting state