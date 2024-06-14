/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Header for module which contains Prefix Trie 
        data structures and functions.
*/
struct prefixTree;

#ifndef PREFIXTREESTRUCT
#define PREFIXTREESTRUCT
#define CHILD_COUNT (1 << 8)
#define UNMARKED (0)
#define MARKED (1)
#define ASCII_CHAR_SIZE 256
#define DELIMITER '\0'
#define CHAR_CONVERTER 'a'
#define ASCII_LETTER_RANGE_FROM 97
#define ASCII_LETTER_RANGE_TO 122
#define TO_LOWER_CASE 32
#define TO_UPPER_CASE 32
#define BLANK_SPACE ' '
#define TRUE 1
#define FALSE 0
#define MAX_WORD_LENGTH 100

struct prefixTree;
struct prefixTrieNode;

struct prefixTree {
    struct prefixTrieNode *root;
    int numOfWord;
};
 
struct prefixTrieNode {
    struct prefixTrieNode* children[ASCII_CHAR_SIZE];
    int endOfWord;
};
#endif

/*
 * This is a function that create and initialise a new Prefix Tries
*/
struct prefixTree *newPrefixTree();

/*
 * This is a function that insert a word into an existing Prefix Tries
*/
struct prefixTree *addWordToTree(struct prefixTree *pt, char *word);

//this is a debug function
void outputAllWordsInTree(struct prefixTrieNode* root, char *word, int level);

/*
 * This is a recursive function that stores all the words in a prefix trie into
  * an array of words
*/
void storeAllWordsInTree(struct prefixTrieNode* root, char *word, int level, char **outputWords, int *wordCount);

/* This is a function that search in a prefix trie to determine whether
 * a word exist in the trie.
*/
int searchWord(struct prefixTrieNode* root, char *word);

//this is a debug function
void findWordForChar(char **board, int **visited, int i, int j, int dimension, char *word, int level, struct prefixTrieNode* root, struct prefixTree *outputTree);

/* This is a function that search simultaneously through the prefix tree and the
 *   boggle graph. store the list of all words found into a prefix trie.
*/
void findWordsInBoggle(char **board, struct prefixTrieNode* root, int dimension, struct prefixTree *outputTree);

//find the node for the last letter in the prompt 
struct prefixTrieNode* findDeepestPrefixNode(struct prefixTrieNode* root, char* prompt);

/* This is a recursive function that use depth first search to search every
 * combination of letters in the boggle board to see whether they forms a 
 * word, while limiting the path of the DFS can go using a prefix trie.
*/
void newFindWordForChar(char **board, int **visited, int i, int j, int dimension, char *word, int level, struct prefixTrieNode* root, struct prefixTree *outputTree);

/* This is a function that take a word and board as input and check whether the word
 * exist in the board by calling a helper function searchWordCharByChar.
*/
void searchWordInBoard(char **board, int dimension, char *word, struct prefixTree *outputTree);

/* This is a recursive function that search through the boggle board
   to determine whether a word exist in the board
*/
int searchWordCharByChar(char **board, int **visited, int i, int j, int dimension,
                         char *word, int level, struct prefixTree *outputTree);

/* This is a function that checks whetehr a word is constructed by unique
   characters.
*/
int isUniqueLetterWord(char *word);
