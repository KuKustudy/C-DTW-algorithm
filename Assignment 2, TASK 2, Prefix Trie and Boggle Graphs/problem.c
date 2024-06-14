/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 2-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick

    //			gcc -g -Wall problem.c -o problem

*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"
#include "prefixTree.h"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    /* Part B onwards so set as empty. */
    p->partialString = NULL;

    int wordCount = 0;
    int wordAllocated = 0;
    char *dictText = NULL;
    char **words = NULL;

    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&dictText, &allocated, '\0', dictFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    char *boardText = NULL;
    /* Reset allocated marker. */
    allocated = 0;
    success = getdelim(&boardText, &allocated, '\0', boardFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading board file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int dictTextLength = strlen(dictText);

    /* Count words present. */
    while(progress < dictTextLength){
        char *word = NULL;
        int nextProgress;
        /* Read each value into the dictionary. */
        if(progress == 0){
            /* First line. */
            int wordNumberGuess;
            assert(sscanf(dictText + progress, "%d %n", &wordNumberGuess, &nextProgress) == 1);
            /* Will fail if integer missing from the start of the words. */
            assert(nextProgress > 0);
            if(wordNumberGuess > 0){
                wordAllocated = wordNumberGuess;
                words = (char **) malloc(sizeof(char *) * wordAllocated);
                assert(words);
            }
            progress += nextProgress;
            continue;
        } else {
            assert(sscanf(dictText + progress, "%m[^\n] %n", &word, &nextProgress) == 1);
            assert(nextProgress > 0);
            progress += nextProgress;
        }

        /* Check if more space is needed to store the word. */
        if(wordAllocated <= 0){
            words = (char **) malloc(sizeof(char *) * INITIALWORDSALLOCATION);
            assert(words);
            wordAllocated = INITIALWORDSALLOCATION;
        } else if(wordCount >= wordAllocated){
            words = (char **) realloc(words, sizeof(char *) * 
                wordAllocated * 2);
            assert(words);
            wordAllocated = wordAllocated * 2;
            /* Something has gone wrong if there's not sufficient word 
                space for another word. */
            assert(wordAllocated > wordCount);
        }

        words[wordCount] = word;
        wordCount++;
    }
    
    /* Done with dictText */
    if(dictText){
        free(dictText);
    }
    
    /* Now read in board */
    progress = 0;
    int dimension = 0;
    int boardTextLength = strlen(boardText);
    /* Count dimension with first line */
    while(progress < boardTextLength){
        /* Count how many non-space characters appear in line. */
        if(boardText[progress] == '\n' || boardText[progress] == '\0'){
            /* Reached end of line. */
            break;
        }
        if(isalpha(boardText[progress])){
            dimension++;
        }
        progress++;
    }

    assert(dimension > 0);

    /* Check each line has the correct dimension. */
    for(int i = 1; i < dimension; i++){
        int rowDim = 0;
        if(boardText[progress] == '\n'){
            progress++;
        }
        while(progress < boardTextLength){
            /* Count how many non-space characters appear in line. */
            if(boardText[progress] == '\n' || boardText[progress] == '\0'){
                /* Reached end of line. */
                break;
            }
            if(isalpha(boardText[progress])){
                rowDim++;
            }
            progress++;
        }
        if(rowDim != dimension){
            fprintf(stderr, "Row #%d had %d letters, different to Row #1's %d letters.\n", i + 1, rowDim, dimension);
            assert(rowDim == dimension);
        }
    }

    /* Define board. */
    char *boardFlat = (char *) malloc(sizeof(char) * dimension * dimension);
    assert(boardFlat);
    
    /* Reset progress. */
    progress = 0;
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            int nextProgress;
            assert(sscanf(boardText + progress, "%c %n", &boardFlat[i * dimension + j], &nextProgress) == 1);
            progress += nextProgress;
        }
    }

    char **board = (char **) malloc(sizeof(char **) * dimension);
    assert(board);
    for(int i = 0; i < dimension; i++){
        board[i] = &boardFlat[i * dimension];
    }

    // fprintf(stderr, "\n");

    /* The number of words in the text. */
    p->wordCount = wordCount;
    /* The list of words in the dictionary. */
    p->words = words;

    /* The dimension of the board (number of rows) */
    p->dimension = dimension;

    /* The board, represented both as a 1-D list and a 2-D list */
    p->boardFlat = boardFlat;
    p->board = board;

    /* For Part B only. */
    p->partialString = NULL;

    p->part = PART_A;

    return p;
}

struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(dictFile, boardFile);

    char *partialString = NULL;

    /* Part B has a string that is partially given - we assume it follows 
        word conventions for the %s specifier. */
    assert(fscanf(partialStringFile, "%ms", &partialString) == 1);
    
    p->part = PART_B;
    p->partialString = partialString;

    return p;
}

struct problem *readProblemD(FILE *dictFile, FILE *boardFile){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(dictFile, boardFile);
    
    p->part = PART_D;
    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    switch(problem->part){
        case PART_A:
        case PART_D:
            assert(solution->foundWordCount == 0 || solution->words);
            for(int i = 0; i < solution->foundWordCount; i++){
                fprintf(outfileName, "%s\n", solution->words[i]);
            }
            break;
        case PART_B:
            assert(solution->foundLetterCount == 0 || solution->followLetters);
            for(int i = 0; i < solution->foundLetterCount; i++){
                if(isalpha(solution->followLetters[i])){
                    fprintf(outfileName, "%c\n", solution->followLetters[i]);
                } else {
                    fprintf(outfileName, " \n");
                }
            }
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->followLetters){
            free(solution->followLetters);
        }
        if(solution->words){
            free(solution->words);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->words){
            for(int i = 0; i < problem->wordCount; i++){
                if(problem->words[i]){
                    free(problem->words[i]);
                }
            }
            free(problem->words);
        }
        if(problem->board){
            free(problem->board);
        }
        if(problem->boardFlat){
            free(problem->boardFlat);
        }
        if(problem->partialString){
            free(problem->partialString);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    s->foundWordCount = 0;
    s->words = NULL;
    s->foundLetterCount = 0;
    s->followLetters = NULL;
    
    return s;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part A */
    //construct the prefix trie for the dictionary
    struct prefixTree *tree = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++){
        tree = addWordToTree(tree, p->words[i]);
    }

    /* use a tree to store all words in the board that are in the dictionary
     * so that we don't need to sort them by alphabet order later
    */
    struct prefixTree *outputTree = newPrefixTree();
    findWordsInBoggle(p->board, tree->root, p->dimension, outputTree);


    //store the words in the outputTree into an array of words

    int level = 0;
    //this is a temporary array of char to store the current prefix during the search
    char *word = (char *)malloc(sizeof(char) * MAX_WORD_LENGTH);    
    char **outputWords = (char **)malloc(sizeof(char*) * outputTree->numOfWord);
    int *wordCount = (int *)malloc(sizeof(int));
    *wordCount = 0;

    storeAllWordsInTree(outputTree->root, word, level, outputWords, wordCount);

    free(word);
    free(wordCount);

    s->foundWordCount = outputTree->numOfWord;
    s->words = outputWords;
    return s;
}

struct solution *solveProblemB(struct problem *p){
    struct solution *s = newSolution(p);

    /*adapted from part A*/
    struct prefixTree *tree = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++){
        tree = addWordToTree(tree, p->words[i]);
    }
    struct prefixTree *outputTree = newPrefixTree();
    findWordsInBoggle(p->board, tree->root, p->dimension, outputTree);
    /*from part A*/

    /* Fill in: Part B */
    char *prompt = p->partialString;

    int numOfFoundChar = 0;
    int allocatedMemory = 10;
    char *outputChars = (char *)malloc(sizeof(char) * allocatedMemory);

    //store the deepest node the prompt can reach, its children forms the following characters
    //of the prompt we received
    struct prefixTrieNode *deepestNode = findDeepestPrefixNode(outputTree->root, prompt);

    /*
     If the word can be terminated, a blank space ( ) character should also be printed
     on its own line
    */
    if (deepestNode->endOfWord == TRUE){
        outputChars[numOfFoundChar] = BLANK_SPACE;
        numOfFoundChar++;
    }

    for (int i = 0; i < ASCII_CHAR_SIZE; i++){
        if (deepestNode->children[i] != NULL){
            //if there is a char that will continue this prefix, store it

            //allocate more memory for the string if needed
            if (numOfFoundChar >= allocatedMemory - 1){
                allocatedMemory *= 2;
                outputChars = (char *)realloc(outputChars, sizeof(char) * allocatedMemory);
            }

            //any non-letter character must be printed as a space
            if (i + CHAR_CONVERTER < ASCII_LETTER_RANGE_FROM 
                || i + CHAR_CONVERTER > ASCII_LETTER_RANGE_TO){
                outputChars[numOfFoundChar] = BLANK_SPACE;
            } else {
                outputChars[numOfFoundChar] = i + CHAR_CONVERTER;   
            }
            numOfFoundChar++;  
                      
        }
    }
    
    outputChars[numOfFoundChar] = DELIMITER;
    
    s->foundLetterCount = numOfFoundChar;
    s->followLetters = outputChars;
    return s;
}

struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part D */  

    //search through the words in the dictionary
    //and check whether it is in the board
    struct prefixTree *outputTree = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++){
        if (isUniqueLetterWord(p->words[i])){
            //only search word that is contsurcted with unique letters
            //For example: bear, leak are unique, bee, book are not
            searchWordInBoard(p->board, p->dimension, p->words[i], outputTree);
        }
    }

    int level = 0;
    char *word = (char *)malloc(sizeof(char) * MAX_WORD_LENGTH);    
    char **outputWords = (char **)malloc(sizeof(char*) * outputTree->numOfWord);
    int *wordCount = (int *)malloc(sizeof(int));
    *wordCount = 0;

    storeAllWordsInTree(outputTree->root, word, level, outputWords, wordCount);

    free(word);
    free(wordCount);
    
    s->foundWordCount = outputTree->numOfWord;
    s->words = outputWords;

    return s;
}



