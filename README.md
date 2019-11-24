# ElectionManager

A program that helps citizens to vote using a composition of data structures. The composition of data structurss helps the curator to know any time 
who has already voted, who cannot vote and also generate statistics depending on the voter's postcodes.

## Tehe data structure composition consists of
* A Bloom-Filter (BF) which rapidly decides whether someone is not in the voting registry.
* A Red-Black-Tree (RBT) which gives fast(logarithmic) access to the voter's record and sets a flag if he has voted.
* A hash table which maps a given postcode to a linked list that contains all the voters that have that postcode.

### Compilation
```
make
```

### Usage
```
./runelection -i inputfile -o outfile -n numofupdates
```
where numofupdates is the required amount of insertions/deletions to reconstruct the Bloom-Filter.

### Instructions
* lbf key (lookup bloom-filter): Checks if the BF possibly contains key. The answer is either not or possibly.
* lrb key (lookup red-black tree): Checks if the RBT contains a record with key. If yes, print it.
* ins record (insert record to red-black-tree): Inserts the record to the data structures specifying it's data in a single line 
  (key firstname lastname age gender postcode). 
* find key: Checks if there is a record with key in the data structures composition and prints it. Otherwise prints the apropriate message.
* delete key: Deletes the record with key from the data structes. The deletion does not affect the BF.
* vote key: Change the hasvoted status of the record with id key to true. If the voter does not exist or has already voted, it prints the apropriate messages.
* load fileofkeys: For all keys that are in fileofkeys, tries to vote for them.
* voted: Prints how many voters have already voted.
* voted postcode: It prints how many people have voted for a specific postcode.
* votedperpc: For each postcode, it prints the percentage of people currently voted.
* exit: Frees all memory and exits the program.

### Clean objects and executables
```
make clean
```

### Note 
This is a project for the operating systems course of the Department of Informatics and Telecommunications at the National and Kapodistrian University
of Athens.