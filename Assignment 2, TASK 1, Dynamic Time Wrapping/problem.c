/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 1-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

#define LDINFINITY (LDBL_MAX / 2.0L)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

void readSequence(FILE *seqFile, int *seqLen, long double **seq);

void readSequence(FILE *seqFile, int *seqLen, long double **seq){
    char *seqText = NULL;
    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&seqText, &allocated, '\0', seqFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int seqTextLength = strlen(seqText);

    int commaCount = 0;
    /* Count how many numbers are present. */
    for(int i = 0; i < seqTextLength; i++){
        if(seqText[i] == ','){
            commaCount++;
        }
    }
    long double *seqLocal = (long double *) malloc(sizeof(long double) * (commaCount + 1));
    assert(seqLocal);

    int seqAdded = 0;
    while(progress < seqTextLength){
        int nextProgress;
        /* Read each value into the sequence. */
        assert(sscanf(seqText + progress, "%Lf , %n", &seqLocal[seqAdded], &nextProgress) == 1);
        assert(nextProgress > 0);
        progress += nextProgress;
        seqAdded++;
    }
    assert(seqAdded == (commaCount + 1));
    *seq = seqLocal;
    *seqLen = seqAdded;
}

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *seqAFile, FILE *seqBFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    int seqALength = 0;
    long double *seqA = NULL;
    readSequence(seqAFile, &seqALength, &seqA);
    int seqBLength = 0;
    long double *seqB = NULL;
    readSequence(seqBFile, &seqBLength, &seqB);

    /* The length of the first sequence. */
    p->seqALength = seqALength;
    /* The first sequence. */
    p->sequenceA = seqA;

    /* The length of the second sequence. */
    p->seqBLength = seqBLength;
    /* The second sequence. */
    p->sequenceB = seqB;

    /* For Part D & F only. */
    p->windowSize = -1;
    p->maximumPathLength = -1;

    p->part = PART_A;

    return p;
}

struct problem *readProblemD(FILE *seqAFile, FILE *seqBFile, int windowSize){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(seqAFile, seqBFile);

    p->part = PART_D;
    p->windowSize = windowSize;

    return p;
}

struct problem *readProblemF(FILE *seqAFile, FILE *seqBFile, 
    int maxPathLength){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(seqAFile, seqBFile);
    
    p->part = PART_F;
    p->maximumPathLength = maxPathLength;

    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    fprintf(outfileName, "%.2Lf\n", solution->optimalValue);
    switch(problem->part){
        case PART_A:
            assert(solution->matrix);
            for(int i = 1; i <= problem->seqALength; i++){
                for(int j = 1; j <= problem->seqBLength; j++){
                    if(solution->matrix[i][j] == LDINFINITY){
                        fprintf(outfileName, "    ");
                    } else {
                        fprintf(outfileName, "%.2Lf", solution->matrix[i][j]);
                    }
                    if(j < (problem->seqBLength)){
                        /* Intercalate with spaces. */
                        fprintf(outfileName, " ");
                    }
                }
                fprintf(outfileName, "\n");
            }
            break;
        case PART_D:
        case PART_F:
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->matrix){
            for(int i = 0; i < problem->seqALength; i++){
                free(solution->matrix[i]);
            }
            free(solution->matrix);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->sequenceA){
            free(problem->sequenceA);
        }
        if(problem->sequenceB){
            free(problem->sequenceB);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    if(problem->part == PART_F){
        s->matrix = NULL;
    } else {
        s->matrix = (long double **) malloc(sizeof(long double *) * 
            (problem->seqALength + 1));
        assert(s->matrix);
        for(int i = 0; i < (problem->seqALength + 1); i++){
            s->matrix[i] = (long double *) malloc(sizeof(long double) * 
                (problem->seqBLength + 1));
            assert(s->matrix[i]);
            for(int j = 0; j < (problem->seqBLength + 1); j++){
                s->matrix[i][j] = 0;
            }
        }
    }

    s->optimalValue = -1;
    
    return s;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part A */

    //store the lengths of sequences
    int n = p->seqALength;
    int m = p->seqBLength;

    //create and initialise the DTW matrix with infinity
    long double **dtwMatrix = (long double**)malloc(sizeof(long double*)*(n+1));
    for (int i = 0; i < n+1; i++){
        dtwMatrix[i] = (long double*)malloc(sizeof(long double)*(m+1));
        for(int j = 0; j < m+1; j++){
            dtwMatrix[i][j] = LDINFINITY;
        }
    }
    //initialise the cell at row 0 and column 0 with value zero, as no cumulative
    // cost for aligning 0th element in seqA and 0th element in seqB
    dtwMatrix[0][0] = 0.0L;

    //populate the DTW matrix
    long double cost;
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <=m; j++){
            /*cell[i][j] represents minimum cumulative cost in aligning ith 
            element in sequence A with the jth element in sequence B
            */
            cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);
            dtwMatrix[i][j] = cost + fminl(fminl(dtwMatrix[i-1][j], dtwMatrix[i][j-1]), dtwMatrix[i-1][j-1]);
        }
    }

    //the optimal value is the bottom right cell of the martix
    s->matrix = dtwMatrix;
    s->optimalValue = dtwMatrix[n][m];
    return s;
}

struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part D */
    //store the lengths of sequences, and the constraint size
    int n = p->seqALength;
    int m = p->seqBLength;
    int constraintSize = p->windowSize;
    int jStart, jEnd;

    //create and initialise the DTW matrix with infinity
    long double **dtwMatrix = (long double**)malloc(sizeof(long double*)*(n+1));
    for (int i = 0; i < n+1; i++){
        dtwMatrix[i] = (long double*)malloc(sizeof(long double)*(m+1));
        for(int j = 0; j < m+1; j++){
            dtwMatrix[i][j] = LDINFINITY;
        }
    }
    //initialise the cell at row 0 and column 0 with value zero, as no cumulative
    // cost for aligning 0th element in seqA and 0th element in seqB
    dtwMatrix[0][0] = 0.0L;

    //populate the DTW matrix
    long double cost;
    for(int i = 1; i <= n; i++){
        //use boundary constraints to limit the alignment to a specified window around the diagonal
        jStart = max(1, i - constraintSize);
        jEnd = min(m, i + constraintSize);
        for(int j = jStart; j <= jEnd; j++){
            cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);
            dtwMatrix[i][j] = cost + fminl(fminl(dtwMatrix[i-1][j], dtwMatrix[i][j-1]), dtwMatrix[i-1][j-1]);
        }
    }

    //the optimal value is the bottom right cell of the martix
    s->matrix = dtwMatrix;
    s->optimalValue = dtwMatrix[n][m];
    return s;
}

struct solution *solveProblemF(struct problem *p){
        struct solution *s = newSolution(p);
    /* Fill in: Part F */

    //store the lengths of sequences, and the Total Path Length Constraint
    int n = p->seqALength;
    int m = p->seqBLength;
    int max_path_length = p->maximumPathLength;

    //create and initialise the DTW matrix with infinity long double value
    long double ***dtwMatrix = (long double***)malloc(sizeof(long double**)*(n+1));
    for (int i = 0; i < n+1; i++){
        dtwMatrix[i] = (long double**)malloc(sizeof(long double*)*(m+1));
        for(int j = 0; j < m+1; j++){
            dtwMatrix[i][j] = (long double*)malloc(sizeof(long double)*(max_path_length+1));
            //the new dimension is the step/path length that restricted by 
            //the total path length constraint
            for (int k = 0; k < max_path_length + 1; k++){
                dtwMatrix[i][j][k] = LDINFINITY;
            }
            
        }
    }
    //the cost of aligning 0th elements for sequence A and B with zero steps is zero
    dtwMatrix[0][0][0] = 0.0L;

    //populate the DTW matrix
    long double cost;
    for(int i = 1; i < n + 1; i++){
        for(int j = 1; j < m + 1; j++){
            cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);
            //calculate the costs for all admissible path lengths
            for (int k = 1; k < max_path_length + 1; k++){
                dtwMatrix[i][j][k] = cost + fminl(fminl(dtwMatrix[i-1][j][k-1], dtwMatrix[i][j-1][k-1]), dtwMatrix[i-1][j-1][k-1]);
            }            
        }
    }

    //extract the optimal cost by find the minimum cost for paths of length up to the maximumPathLength
    long double optimalCost = dtwMatrix[n][m][1];
    for(int k = 2; k <= max_path_length; k++){
        if (dtwMatrix[n][m][k] < optimalCost) {
            optimalCost = dtwMatrix[n][m][k];
        }
    }
    
    s->optimalValue = optimalCost;
    return s;
}

/*This a method that find the returns the input value that is greater than the other input value*/
int max(int a, int b){
    if(a > b){
        return a;
    } else {
        return b;
    }
}

/*This a method that find the returns the input value that is less than the other input value*/
int min(int a, int b){
    if(a < b){
        return a;
    } else {
        return b;
    }
}
