/*
	Hieu Pham
	UTA ID 1001631515
	CSE 3318
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "spell.h"

/*  Suggestions
- When you work with 2D arrays, be careful. Either manage the memory yourself, or
work with local 2D arrays. Note C99 allows parameters as array sizes as long as 
they are declared before the array in the parameter list. See: 
https://www.geeksforgeeks.org/pass-2d-array-parameter-c/

Worst case time complexity to compute the edit distance from T test words
 to D dictionary words where all words have length MAX_LEN:
Student answer:  Theta(D*T)


Worst case to do an unsuccessful binary search in a dictionary with D words, when 
all dictionary words and the searched word have length MAX_LEN 
Student answer:  Theta(lgD)
*/


/* You can write helper functions here */
#define MAX_LINE 100000
#define MAX_LEN 100
#define MAX_NAME 101

void print_line(int size){
    for (int i = 0; i < size; i++)
    {
        printf("-----");
    }
    printf("\n");
}
int minimum(int up, int left, int diag){
	int min = diag;
	if (min > left)
		min = left;
	if(min > up)
		min = up;
	return min;
	
}
char **getWords(FILE *fp, int *count){
    char line[MAX_LINE];
    int wordCount = 0;
    char **words;
    
    char newNord[MAX_LEN] = "NULL";

    words = malloc(wordCount*sizeof(char *));
    char w[2];
    w[1] = '\0';
    bool flag = false;
    memset(newNord,0,strlen(newNord));
	
    while (fgets(line,MAX_LINE,fp))
    {
		int lenOfLine =strlen(line);
        for (int i = 0; i < lenOfLine; i++)
        {

            if(line[i] == ' ' || line[i] == ',' 
            || line[i]=='?'|| line[i] == '!' 
            || line[i] == '.'){
                if (flag)
                {
                    wordCount++;
                    words = realloc(words,wordCount*sizeof(char *));
                    words[wordCount-1] = malloc(MAX_LINE*sizeof(char));
                    strcpy(words[wordCount-1],newNord);
                    memset(newNord,0,strlen(newNord));
                    flag = false;
                }
                
                w[0] = line[i];
                strcat(newNord,w);

                wordCount++;
                words = realloc(words,wordCount*sizeof(char *));
                words[wordCount-1] = malloc(MAX_LINE*sizeof(char));
                strcpy(words[wordCount-1],newNord);

                memset(newNord,0,strlen(newNord));
                flag = false;
            }
            else{
                w[0] = line[i];
                strcat(newNord,w);
                flag = true;
            }
        }
        if (flag)
        {
            wordCount++;
            words = realloc(words,wordCount*sizeof(char *));
            words[wordCount-1] = malloc(MAX_LINE*sizeof(char));
            strcpy(words[wordCount-1],newNord);
        }
        
        
        
    }

    *count = wordCount;
    return words;
}

struct Dictionary{
    char word[MAX_LEN];
};
int cmpfunc( const void *a, const void *b){
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    return strcmp(*ia , *ib);
}

int binarySearch(struct Dictionary* book, int bookSize, char *word,int *searchPath, int Isverbose){
    int left = 0;
    int right = bookSize-1;
    int pivot,i;
	
	char *wordlwr = malloc(MAX_LEN*sizeof(char));
	int len = strlen(word);
	for ( i = 0; i < len; i++)
	{
		wordlwr[i] = tolower(word[i]); 
	}
	wordlwr[i] = '\0'; 
	
	if(Isverbose == 1)
		printf("Binary sarch for: %s\n",word);
    while (left <= right){
        pivot = left + (right - left)/2;
		*searchPath = *searchPath + 1;
		if(Isverbose == 1)
			printf("dict[%d] = %s\n",pivot,book[pivot].word);
        if (strcmp(wordlwr,book[pivot].word) == 0){
			free(wordlwr);
			return pivot;
		}    
        if (strcmp(wordlwr,book[pivot].word) < 0)
            right = pivot -1;
        else
			left = pivot + 1;
        
    }
	if(Isverbose == 1)
		printf("Not found\n");
	free(wordlwr);
    return -1;
    
}
char** makeDictArrayBook(FILE *fp, int *sizeBook,int printOn){
    int dictLen;

    fscanf(fp,"%d",&dictLen);
    char **words;
    words = malloc(dictLen*sizeof(char *));
    for (int i = 0; i < dictLen; i++)
    {
        words[i] = malloc(MAX_LEN*sizeof(char));
        char newWord[MAX_LEN];
        fscanf(fp,"%s",newWord);
        strcpy(words[i] , newWord);
    }

    *sizeBook = dictLen;
	printf("\nDictionary has size: %d\n", dictLen);
	if (printOn == 1){
		printf(" Original  dictionary:\n");
		for (int i = 0; i < dictLen; i++)
			printf("%d. %s\n",i,words[i]);
	}
	
    return words;
}
struct Dictionary* sortBook(char **arrayBook, int sizeBook, int printOn){

    struct Dictionary *book = malloc(sizeBook*sizeof(struct Dictionary )); 

    qsort(arrayBook,sizeBook,sizeof(char *),cmpfunc);
    
    for (int i = 0; i < sizeBook; i++)
        strcpy(book[i].word , arrayBook[i]);

	
	if (printOn ==1){
		printf(" Sorted dictionary (alphabetical order) :\n");
		for (int i = 0; i < sizeBook; i++)
			printf("%d. %s\n",i,book[i].word);
	}
	
    return book;
}
void print_editDist_table(char * first_string, char * second_string,int fs,int ss,int **distance){

	printf("  |    |");
	for (int i = 0; i < ss; i++)
	{
		printf("%4c|",second_string[i]);
	}
	printf("\n");
	printf("---");
	print_line(ss+1);
	for (int i = 0; i < fs+1; i++)
	{
		if(i == 0)
			printf("  |");
		else
			printf("%2c|",first_string[i-1]);
		for (int j = 0 ;j < ss+1; j++)
			printf("%4d|",distance[i][j]);
		printf("\n");
		printf("---");
		print_line(ss+1);
	}
}

void * recorrect_word(struct Dictionary *book, char *aword,int bookSize){
	char wordlwr[MAX_LEN];
	int i = 0;
	int minimum = MAX_LEN;
	int numMinDistance = 1;
	int *minDistance = (int *)malloc(numMinDistance*sizeof(int));
	int len = strlen(aword);
	for ( i = 0; i < len; i++)
	{
		wordlwr[i] = tolower(aword[i]); 
	}
	wordlwr[i] = '\0'; 
	
 
	for (i = 0; i < bookSize; i++){
		int distance = edit_distance(book[i].word,aword,0);
		

		if (distance == minimum){
			minDistance = (int *)realloc(minDistance,numMinDistance*sizeof(int));
			minDistance[numMinDistance-1] = i;
			numMinDistance++;
			continue;
		}

		if (distance < minimum){
			minimum = distance;
			free(minDistance);
			numMinDistance = 1;
			minDistance = (int *)calloc(numMinDistance,sizeof(int));
			minDistance[numMinDistance-1] = i;
			numMinDistance++;
		}

	}
	int option;
	char c;

	printf("-1 - type correction\n");
	printf(" 0 - leave word as is (do not fix spelling)\n");
	printf("	Minimum distance: %d (computed edit distances: %d)\n",minimum,bookSize);
	printf("	Words that give minimum distance:\n");
	for (int i = 0; i < numMinDistance-1; i++)
		printf(" %d. %s\n",i+1,book[minDistance[i]].word);
	printf("Enter your choice (from the above option): ");
	scanf("%d",&option);
	if(option == -1){
		memset(aword,0,strlen(aword));
		printf("Enter correct word: ");
  		scanf("%s%c", aword,&c);
	}
	if(option < numMinDistance+1 && option > 0)
		strcpy(aword,book[minDistance[option-1]].word);
	free(minDistance);
}
/*
Parameters:
first_string - pointer to the first string (displayed vertical in the table)
second_string - pointer to the second string (displayed horizontal in the table)
print_table - If 1, the table with the calculations for the edit distance will be printed.
              If 0 (or any value other than 1)it will not print the table
(See sample run)
Return:  the value of the edit distance (e.g. 3)
*/
int edit_distance(char * first_string, char * second_string, int print_table){
	int value = 0;
	int i,j;
	int vertical = 0;
	int horizontal = 0;
	int length_firstStr = strlen(first_string);
	int length_secondStr = strlen(second_string);

	int **distance;
	distance = malloc((length_firstStr+1)*sizeof(int*));
	for (i = 0; i < length_firstStr+1; i++)
	{
		distance[i] = malloc((length_secondStr+1)*sizeof(int));
	}
	
	for (i = 0; i < length_firstStr+1; i++)
	{
		for (j = 0 ;j < length_secondStr+1; j++)
		{
			if(i == 0){
				value = vertical;
				vertical++;
			}
			if(j ==0 ){
				value = horizontal;
				horizontal++;
			}
			if(j > 0 && i > 0){
				if(first_string[i-1] == second_string[j-1])
					value = distance[i-1][j-1];
				else
					value = 1 + minimum(distance[i-1][j],distance[i][j-1],distance[i-1][j-1]);
			}


			distance[i][j] = value;
		}
	}
	if(print_table == 1)
		print_editDist_table(first_string,second_string,length_firstStr,length_secondStr,distance);
	for (int i = 0; i < (length_firstStr+1); i++)
	{
		free(distance[i]);
	}
	
	free(distance);

	return value;  // replace this line with your code
}
	  

/*
Parameters:
testname - string containing the name of the file with the paragraph to spell check, includes .txt e.g. "text1.txt" 
dictname - name of file with dictionary words. Includes .txt, e.g. "dsmall.txt"
printOn - If 1 it will print EXTRA debugging/tracing information (in addition to what it prints when 0):
			 dictionary as read from the file	
			 dictionary AFTER sorting in lexicographical order
			 for every word searched in the dictionary using BINARY SEARCH shows all the "probe" words and their indices indices
			 See sample run.
	      If 0 (or anything other than 1) does not print the above information, but still prints the number of probes.
		     See sample run.
*/

void spell_check(char * testname, char * dictname, int printOn){
	char corrected[MAX_NAME] = "out_";
	int bookSize;
	char **words ;
    int count;
	int isFound; 
	strcat(corrected,testname);

	printf("Corrected output filename: %s\n",corrected);


	FILE *fp;
	fp = fopen(dictname,"r");
	if (fp == NULL){
        printf("\nUnable to open file.\n");
        exit(EXIT_FAILURE);
    }
	printf("Loading the dictionary file: %s\n", dictname);

	//Load dictionary to array
	char **arrayBook = makeDictArrayBook(fp,&bookSize,printOn);
	fclose(fp);
	//Load array of dictionary to dictionary book.
    struct Dictionary *book = sortBook(arrayBook,bookSize,printOn);

	//Load word from testname file to 2D array,
	FILE *fpt;
    fpt = fopen(testname,"r");
    if (fpt == NULL){
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }
    words = getWords(fpt,&count);
	fclose(fpt);

	for (int i = 0; i < count; i++)
	{
		if (strcmp(words[i],",") == 0
        || strcmp(words[i],".") == 0
        || strcmp(words[i],"?") == 0
        || strcmp(words[i],"!") == 0
        || strcmp(words[i]," ") == 0)
        {
            continue;
        }
		
		int searchPath = 0;
		isFound = binarySearch(book, bookSize,words[i],&searchPath,printOn);
		printf("---> |%s| (words compared when searching: %d)\n",words[i],searchPath);
		if (isFound != -1){
			printf(" - OK\n");
			
		}
		else{
			recorrect_word(book,words[i],bookSize);
		}
	}

	FILE *fpOut;
	fpOut = fopen(corrected,"w");

	for (int i = 0;i < count; i++){
		fputs(words[i],fpOut);
		free(words[i]);
	}
	fclose(fpOut);
	
	free(words);

	for ( int i = 0; i < bookSize; i++)
		free(arrayBook[i]);
		
	free(book);
	free(arrayBook);
	

}

