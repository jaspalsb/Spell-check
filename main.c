#include "hash.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int distance(const char * word1, int len1, const char * word2, int len2)
{
	int matrix[len1 + 1][len2 + 1];
	int i;
	for (i = 0; i <= len1; i++) {
		matrix[i][0] = i;
	}
	for (i = 0; i <= len2; i++) {
		matrix[0][i] = i;
	}
	for (i = 1; i <= len1; i++) {
		int j;
		char c1;

		c1 = word1[i - 1];
		for (j = 1; j <= len2; j++) {
			char c2;

			c2 = word2[j - 1];
			if (c1 == c2) {
				matrix[i][j] = matrix[i - 1][j - 1];
			}
			else {
				int delete;
				int insert;
				int substitute;
				int minimum;

				delete = matrix[i - 1][j] + 1;
				insert = matrix[i][j - 1] + 1;
				substitute = matrix[i - 1][j - 1] + 1;
				minimum = delete;
				if (insert < minimum) {
					minimum = insert;
				}
				if (substitute < minimum) {
					minimum = substitute;
				}
				matrix[i][j] = minimum;
			}
		}
	}
	return matrix[len1][len2];
}
/**
* Allocates a string for the next word in the file and returns it. This string
* is null terminated. Returns NULL after reaching the end of the file.
* @param file
* @return Allocated string or NULL.
*/
char* nextWord(FILE* file)
{
	int maxLength = 16;
	int length = 0;
	char* word = malloc(sizeof(char) * maxLength);
	while (1)
	{
		char c = fgetc(file);
		if ((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '\'')
		{
			if (length + 1 >= maxLength)
			{
				maxLength *= 2;
				word = realloc(word, maxLength);
			}
			word[length] = c;
			length++;
		}
		else if (length > 0 || c == EOF)
		{
			break;
		}
	}
	if (length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}

/**
* Loads the contents of the file into the hash map.
* @param file
* @param map
*/
void loadDictionary(FILE* file, HashMap* map)
{
	char *getWord = nextWord(file);
	//now loading the dictiionary is a little different from the concordance but very similar. 
	//for loading I only have to add it to the hashtable once so I need to just use the hashmapput function
	//with key value of 1 for each word.
	while (getWord != NULL)
	{
		hashMapPut(map, getWord, 1);
		getWord = nextWord(file);
	}
}

/**
* Prints the concordance of the given file and performance information. Uses
* the file input1.txt by default or a file name specified as a command line
* argument.
* @param argc
* @param argv
* @return
*/
int main(int argc, const char** argv)
{
	HashMap* map = hashMapNew(1000);

	FILE* file = fopen("wordBank.txt", "r");
	clock_t timer = clock();
	loadDictionary(file, map);
	timer = clock() - timer;
	printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
	fclose(file);

	char inputBuffer[256];
	int quit = 0;
	while (!quit)
	{
		printf("Enter a word or \"quit\" to quit: ");
		scanf("%s", inputBuffer);
    		for (int i = 0, n = strlen(inputBuffer); i < n; i++)
			{
				inputBuffer[i] = tolower(inputBuffer[i]);
			}
		if (strcmp(inputBuffer, "quit") == 0)
		{
			quit = 1;
		}
    		else
    		{
		// Implement the spell checker code here..
		HashLink *spellchecker;
		int bucketSize = hashMapCapacity(map);
		//now I loop through the buckets and the links to find a match with the dictionary.
		//needed a break from the for loop because I was getting an infinite loop since there was no way to
		//exit the loop once a word had been checked and matched.
		int search = 0;

		for (int idx = 0; idx < bucketSize; idx++)
		{
			if (!search) 
			{
				spellchecker = map->table[idx];
				while (spellchecker != 0)
				{
					if (strcmp(spellchecker->key, inputBuffer) == 0)
					{
						printf("The word %s%s\n", spellchecker->key, ", was spelled correctly!" );
						search = 1;
					}
					spellchecker = spellchecker->next;
				}
			}
		}
		char *suggestions[5];
		if (search == 0)
		{
			//now if the word has been checked against the dictionary map and still has not been found that means it is spelled incorrectly
			printf("The word was spelled incorrectly \n");
			int len1 = strlen(inputBuffer);
			int counter = 0;
			int len2;
			int d;
			for (int idx2 = 0; idx2 < bucketSize; idx2++)
			{
				if (counter < 5)
				{
					spellchecker = map->table[idx2];
					while (spellchecker != 0)
					{
						len2 = strlen(spellchecker->key);
						d = distance(inputBuffer, len1, spellchecker->key, len2);
						if (d >= 1 && d < 3)
						{
							suggestions[counter] = spellchecker->key;
							counter +=1;
						}
						spellchecker = spellchecker->next;

					}
				}
			}
			for (int index = 0; index < 5; index++)
			{
				printf("Similar words...%s \n", suggestions[index]);
			}
		}
	}
  }

	hashMapDelete(map);
	return 0;
}
