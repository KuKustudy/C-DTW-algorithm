/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Implementation for module which contains Prefix Trie 
        data structures and functions.
*/
#include "prefixTree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <stdbool.h>

/*
 * This is a function that create and initialise a new Prefix Tries
*/
struct prefixTree *newPrefixTree(){
    /* Structure of the prefix tree includes a root node that contains no info. */
    struct prefixTree *retTree = NULL;
    retTree = (struct prefixTree*)malloc(sizeof(struct prefixTree));
    assert(retTree);

    retTree->numOfWord = 0;
    //initialise the rootNode for the new Prefix tree
    struct prefixTrieNode *rootNode;
    rootNode = (struct prefixTrieNode*)malloc(sizeof(struct prefixTrieNode));
    assert(rootNode);
    for (int i = 0; i < ASCII_CHAR_SIZE; i++){
        rootNode->children[i] = NULL;
    }
    rootNode->endOfWord = FALSE;

    retTree->root = rootNode;
    return retTree;
}

/*
 * This is a function that insert a word into an existing Prefix Tries
*/
struct prefixTree *addWordToTree(struct prefixTree *pt, char *word){
    //create a new tree node
    struct prefixTrieNode* currNode = pt->root;

    //iterate through every char in the input word
    for (int i = 0; i < (int)strlen(word); i++){
        int asciiIndex = word[i] - CHAR_CONVERTER;

        if (currNode->children[asciiIndex] == NULL){
            /*if the char is not in the currNode's children, which means it does
            not follow an existing word, it should have its own new path
            */
            struct prefixTrieNode *newNode = (struct prefixTrieNode*)malloc(sizeof(struct prefixTrieNode));
            assert(newNode);
     
            for (int i = 0; i < ASCII_CHAR_SIZE; i++){
                 newNode->children[i] = NULL;
            }

            newNode->endOfWord = FALSE;
            //we don't need to store the char, as we implicitly stored the
            // asciiIndex by setting children[index] not null
            currNode->children[asciiIndex] = newNode;
        }

        currNode = currNode->children[asciiIndex];
    }

    //after added last char of new word, mark that it is the end of this word
    if (currNode->endOfWord != TRUE){
        //only increase num of word if this word had not been previously added
        pt->numOfWord++;
        currNode->endOfWord = TRUE;
    }
    
    
    return pt;
}

/*
 * This is a recursive function that prints all the words in a prefix trie.
 * This is used for debug purpose
*/
void outputAllWordsInTree(struct prefixTrieNode* root, char *word, int level) {
    if (root == NULL){
        return;
    }

    //reached the end char of a word in the trie, print the word
    if (root->endOfWord == TRUE){
        word[level] = DELIMITER;
        printf("%s\n", word);
    }

    /* scan every children of the node, if not null child is found, add child
     char to the word, and call the function recursively for child node
    */
    for (int i = 0; i < ASCII_CHAR_SIZE; i++){
        if (root->children[i] != NULL){
            //convert key back to char
            word[level] = (char) (i + CHAR_CONVERTER);
            outputAllWordsInTree(root->children[i], word, level+1);
        }
    }

}

/*
 * This is a recursive function that stores all the words in a prefix trie into
  * an array of words
*/
void storeAllWordsInTree(struct prefixTrieNode* root, char *word, int level, char **outputWords, int *wordCount) {
    if (root == NULL){
        return;
    }

    //reached the end char of a word in the trie, store the word in an array
    if (root->endOfWord == TRUE){
        word[level] = DELIMITER;
        outputWords[*wordCount] = (char *)malloc(sizeof(char) * (level+1));
    
        //copy word without passing pointer to avoid overwrite the word
        for (int k = 0; k < level+1; k++){
            outputWords[*wordCount][k] = word[k];
        }

        //after stored the word, increase the word count
        *wordCount += 1;
    }

    /* scan every children of the node, if not null child is found, add child
     char to the word, and call the function recursively for child node
    */
    for (int i = 0; i < ASCII_CHAR_SIZE; i++){
        if (root->children[i] != NULL){
            //convert key back to char
            word[level] = (char) (i + CHAR_CONVERTER);
            storeAllWordsInTree(root->children[i], word, level+1, outputWords, wordCount);
        }
    }

}

/* This is a function that search in a prefix trie to determine whether
 * a word exist in the trie.
*/
int searchWord(struct prefixTrieNode* root, char *word){
    /* scan every char in the word, if it does not exist in the corresponding
     * level in the trie tree, then we know it does not exist in the trie.    
    */
    struct prefixTrieNode* currNode = root;
    for (int i = 0; word[i] != DELIMITER; i++){
        int asciiIndex = word[i] - CHAR_CONVERTER;
        if (currNode->children[asciiIndex] == NULL){
            //the word does not exist
            return FALSE;
        }

        //the word may exist, search the next level of the trie
        currNode = currNode->children[asciiIndex];
    }

    return (currNode->endOfWord == TRUE);
}

/* This is a recursive function that use depth first search to search every
 * combination of letters in the boggle board, and check whether it forms
 * a word by checking against with a dictionary (a prefix trie)
 * This function does not limit the path using the prefix trie, this is used
 * for debug and comparison, and is not used in solving part 2 problems.
*/
void findWordForChar(char **board, int **visited, int i, int j, int dimension, char *word,
                     int level, struct prefixTrieNode* root, struct prefixTree *outputTree){
    //mark current char as visited, char's position is located at board[i][j]
    visited[i][j] = TRUE;

    //convert char in the board to lower case letter
    word[level] = board[i][j] + TO_LOWER_CASE;
    printf("%s\n", word);

    //check whether current formed word is in the dictionary (prefix trie)
    word[level + 1] = DELIMITER;
    if (searchWord(root, word) == TRUE){
        // the word is in the trie, add the found word into the output
        // tree if it has not been added yet
        if(searchWord(outputTree->root, word) != TRUE){
            outputTree->numOfWord++;
            outputTree = addWordToTree(outputTree, word);
        }
        
    }

    //explore the possible 8 adjacent cells for every char in the board
    for(int row = i - 1; row <= i + 1 && row < dimension; row++){
        for (int col = j - 1; col <= j + 1 && col < dimension; col++){
            /* if the adjacent cell for the current char is within the board
             * and has not been visited yet, call the function to explore its
             * 8 adjacent cells 
            */
            if (row >= 0 && col >= 0 && !visited[row][col]){
                findWordForChar(board, visited, row, col, dimension, word, level+1, root, outputTree);
            }
        }
    }

    //at the end of the search, backtrack, and mark visited cell as unvisited
    word[level] = DELIMITER;
    visited[i][j] = FALSE;
}

/* This is a function that search simultaneously through the prefix tree and the
 *   boggle graph. store the list of all words found into a prefix trie.
*/
void findWordsInBoggle(char **board, struct prefixTrieNode* root,
                       int dimension, struct prefixTree *outputTree){

    //This is a matrix that used to mark whether a cell had been visited
    int **visited = (int **)malloc(sizeof(int*) * dimension);
    assert(visited);

    //initialise every cell in the visited array to false(unvisited)
    for(int a = 0; a < dimension; a++){
        visited[a] = (int *)malloc(sizeof(int) * dimension);
        assert(visited[a]);
        for (int b = 0; b < dimension; b++){
            visited[a][b] = FALSE;
        }
    }

    char *word = (char *)malloc(sizeof(char) * MAX_WORD_LENGTH);
    int level = 0;

    /* consider every char in the boggle board as the starting letter
     * of a word.
    */
    for (int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            newFindWordForChar(board, visited, i, j, dimension,
                                 word, level, root, outputTree);
        }
    }

    //free allocated memory
    for(int a = 0; a < dimension; a++){
        free(visited[a]);
    }
    free(word);

}

/* This is a recursive function that use depth first search to search every
 * combination of letters in the boggle board to see whether they forms a 
 * word, while limiting the path of the DFS can go using a prefix trie.
*/
void newFindWordForChar(char **board, int **visited, int i, int j, int dimension,
                         char *word, int level, struct prefixTrieNode* root, struct prefixTree *outputTree){

    //mark current char as visited
    visited[i][j] = TRUE;
    //convert to lower case letter by adding 32
    word[level] = board[i][j] + TO_LOWER_CASE;

    /*this is a static variable that used for analysing the time complexity
     *of this function call
    */
    // static int count = 0; 
    // count++;
    //printf("Function has been called %d times\n", count);

    //check whether current formed word in the dictionary (prefix tree)
    word[level + 1] = DELIMITER;
    if (searchWord(root, word) == TRUE) {
         //add the found word into the output tree
        outputTree = addWordToTree(outputTree, word);
    }

    //explore the possible 8 adjacent cells of the current char
    for(int row = i - 1; row <= i + 1 && row < dimension; row++){
        for (int col = j - 1; col <= j + 1 && col < dimension; col++){
            if (row >= 0 && col >= 0 && !visited[row][col]){ 
                newFindWordForChar(board, visited, row, col, dimension,
                                     word, level+1, root, outputTree);
            }
        }
    }

    //at the end of the search, backtrack, and mark visited cell as unvisited
    word[level] = DELIMITER;
    visited[i][j] = FALSE;
}



/* This is a function that takes a prefix (prompt) and a prefix trie as input, and 
 * return the deepest node that the prompt can reach to, for example, if the prompt
 * is app, and we have the word apple stored in the trie, then we return p as the 
 * deepest node. If the prompt does not exist in the trie, we return null
*/
struct prefixTrieNode* findDeepestPrefixNode(struct prefixTrieNode* root, char* prompt){

    struct prefixTrieNode* currNode = root;
    for (int k = 0; k < (int)strlen(prompt); k++){
        int asciiIndex = prompt[k] - CHAR_CONVERTER;
        if (currNode->children[asciiIndex] == NULL){
            //no such prefix in the tree
            return NULL;
        }
        currNode = currNode->children[asciiIndex];
    }

    return currNode;
}



/* This is a function that take a word and board as input and check whether the word
 * exist in the board by calling a helper function searchWordCharByChar.
*/
void searchWordInBoard(char **board, int dimension, char *word, struct prefixTree *outputTrie) {

    //This is a matrix that used to mark whether a cell had been visited
    int **visited = (int **)malloc(sizeof(int*) * dimension);
    assert(visited);

    //initialise every cell in the visited array to false(unvisited)
    for(int a = 0; a < dimension; a++){
        visited[a] = (int *)malloc(sizeof(int) * dimension);
        assert(visited[a]);
        for (int b = 0; b < dimension; b++){
            visited[a][b] = FALSE;
        }
    }

    int level = 0;
    for (int i = 0; i < dimension; i++){
        for (int j = 0; j < dimension; j++){
            if (searchWordCharByChar(board, visited, i, j, dimension, word, level, outputTrie)){
                //had successfully found the word
                return;
            }
        }
    }


    
}

/* This is a recursive function that search through the boggle board
   to determine whether a word exist in the board
*/
int searchWordCharByChar(char **board, int **visited, int i, int j, int dimension,
                         char *word, int level, struct prefixTree *outputTree){

    char tempLowerCaseBoardChar = board[i][j] + TO_LOWER_CASE;
    if (tempLowerCaseBoardChar != word[level]){
        /* if the neighbor cell of the focus cell does not match the next char in the
           word, this path will not give us the word we are searching for, hence return
        */
        return FALSE;
    }
    
    if (level == (int)strlen(word) - 1){
        //we had found the word in the board

        //store it in the output tree
        addWordToTree(outputTree, word);
        return TRUE;
    }

    //This seems to be a possible path, let's continute the search

    //mark current char as visited
    visited[i][j] = TRUE;

    //explore the possible 8 adjacent cells of the current char
    for(int row = i - 1; row <= i + 1 && row < dimension; row++){
        for (int col = j - 1; col <= j + 1 && col < dimension; col++){
            if (row >= 0 && col >= 0 && !visited[row][col]){ 
                if (searchWordCharByChar(board, visited, row, col, dimension,
                         word, level+1, outputTree)){
                    //we had found the word, return an indicator to the function
                    //that first called this recursive function.
                    return TRUE;
                }
                
            }
        }
    }

    //at the end of the search, backtrack, and mark visited cell as unvisited
    visited[i][j] = FALSE;
    return FALSE;
}

/* This is a function that checks whetehr a word is constructed by unique
   characters.
*/
int isUniqueLetterWord(char *word){
    /* to find word with unique letters, we need an int array with size 256
       to indicates whether a letter had been appeared in the word.
       every element had been implicitly set to zero (FALSE)
    */
    int* letterAppeared = (int*)calloc(ASCII_CHAR_SIZE, sizeof(int));

    /* scan every character in the word, if it appeared for the first time,
       mark it using the ascii index as its position index in the array.
       else, return FALSE indicate the word is not unique
    */
    for (int i = 0; i < strlen(word); i++){
        int asciiIndex = word[i] - CHAR_CONVERTER;
        if (letterAppeared[asciiIndex] != TRUE){
            letterAppeared[asciiIndex] = TRUE;
        } else {
            //the letter had appeared before in the word
            return FALSE;
        }
    }

    free(letterAppeared);
    return TRUE;
}


