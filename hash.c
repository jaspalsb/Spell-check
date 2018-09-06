/*
* Name:Jaspal Bainiwal
* Date:06/4/2018
*/
//#define  _CRT_SECURE_NO_WARNINGS
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
	int r = 0;
	for (int i = 0; key[i] != '\0'; i++)
	{
		r += key[i];
	}
	return r;
}

int hashFunction2(const char* key)
{
	int r = 0;
	for (int i = 0; key[i] != '\0'; i++)
	{
		r += (i + 1) * key[i];
	}
	return r;
}

/**
* Creates a new hash table link with a copy of the key string.
* @param key Key string to copy in the link.
* @param value Value to set in the link.
* @param next Pointer to set as the link's next.
* @return Hash table link allocated on the heap.
*/
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
	HashLink* link = malloc(sizeof(HashLink));
	link->key = malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(link->key, key);
	link->value = value;
	link->next = next;
	return link;
}

/**
* Free the allocated memory for a hash table link created with hashLinkNew.
* @param link
*/
static void hashLinkDelete(HashLink* link)
{
	free(link->key);
	free(link);
}

/**
* Initializes a hash table map, allocating memory for a link pointer table with
* the given number of buckets.
* @param map
* @param capacity The number of table buckets.
*/
void hashMapInit(HashMap* map, int capacity)
{
	map->capacity = capacity;
	map->size = 0;
	map->table = malloc(sizeof(HashLink*) * capacity);
	for (int i = 0; i < capacity; i++)
	{
		map->table[i] = NULL;
	}
}

/**
* Removes all links in the map and frees all allocated memory. You can use
* hashLinkDelete to free the links.
* @param map
*/
void hashMapCleanUp(HashMap* map)
{
	//For cleaning up and removing all the links in the map I have to look at the map like this
	/*[idx] ->[hash1->key->value] -> [hash1->key->value]
	  [idx] ->[hash2->key->value]
	  [idx] ->[hash3->key->value]
	  [idx] ->[hash4->key->value] ->[hash4->key->value] */
	//so to loop through the multiple hash keys at each index is just like a linked list according to the
	//lectures. So I will delete the same way I have done when we implemented linkedlists by looping until
	//link reachs null.
	
	HashLink* temp;
	//hashlink next temp is used to store the next link because if I delete the temp link then 
	//it would not be possible to traverse to the next link of the deleted one so I have to use two links
	HashLink* nextTemp;
	//I need the size of the capacity (buckets) and not the number of links because I will be using the
	//buckets and indexing them then traversing through the links within the buckets.
	int cap = map->capacity;
	for (int idx = 0; idx < cap; idx++)
	{
		//first set the link to the current indexes link
		temp = map->table[idx];
		//now that we have the link for the hashtable there is a possibility there is a linked list of hash values
		//so I will traverse through each link untill I reach the end(null) then move on.
		while (temp != 0)
		{
			//I first have to save the next link or else if I delete the current temp link then I will not be able to
			//traverse to the next one.
			nextTemp = temp->next;
			//now I delete temp using the function that is said in the directions above
			hashLinkDelete(temp);
			temp = nextTemp;
		}
	}

	free(map->table);
}

/**
* Creates a hash table map, allocating memory for a link pointer table with
* the given number of buckets.
* @param capacity The number of buckets.
* @return The allocated map.
*/
HashMap* hashMapNew(int capacity)
{
	HashMap* map = malloc(sizeof(HashMap));
	hashMapInit(map, capacity);
	return map;
}

/**
* Removes all links in the map and frees all allocated memory, including the
* map itself.
* @param map
*/
void hashMapDelete(HashMap* map)
{
	hashMapCleanUp(map);
	free(map);
}

/**
* Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
* if no link with that key is in the table.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @return Link value or NULL if no matching link.
*/
int* hashMapGet(HashMap* map, const char* key)
{
	//create the new link that will be used to compare key values with the key value from the hash map
	HashLink *map2;
	//from the lectures on hashtable intro it talks about valid index so I am using that example to get the hash value
	//to keep the hashMapget function reusuable from the hashMap.h file the HASH_FUNCTION is defined as the hashFunction1.
	int idx = HASH_FUNCTION(key) % map->capacity;

	map2 = map->table[idx];
	//I have to loop through each key that has the same hash index value to find the correct key because of collisions
	while (map2 != 0)
	{
		//I am using the example that is provided in the worksheet 36 answer to find the value
		//since the strcmp function returns 0 if the str1 matches str2 so I will return the pointer to
		//the link with the given key
		if (strcmp((map2->key), key) == 0) {
			//now that the two keys have matched i return the pointer to the links value and not the key
			return &(map2->value);
		}
		//if if did not match the move on to the next key with the same hash index
		map2 = map2->next;
	}

	//if the keys with the same hash index have all been searched and there isnt a match then return null.
	return NULL;
}

/**
* Resizes the hash table to have a number of buckets equal to the given
* capacity. After allocating the new table, all of the links need to be
* rehashed into it because the capacity has changed.
*
* Remember to free the old table and any old links if you use hashMapPut to
* rehash them.
*
* @param map
* @param capacity The new number of buckets.
*/
void resizeTable(HashMap* map, int capacity)
{
	HashMap *map2;
	//First since we are resizing I will need to create a new hashmap with the capacity that is passed in
	//luckily there is a function that creates a new hash table with just the capacity parameters.
	map2 = hashMapNew(capacity);
	//now I need to add the links at each bucket to the new hashmap
	HashLink *temp;
	int buckets = map->capacity;
	for (int idx = 0; idx < buckets; idx++) 
	{
		//now make the temp link the first link in the current bucket
		temp = map->table[idx];
		//then add the link to the resized map and then traverse to the next link in the same bucket and add that as well.
		//since this is a linked list it is how we have always been implementing the traverse using while loop
		while (temp != 0)
		{
			//put the new link into the hashmap
			hashMapPut(map2, temp->key, temp->value);
			//now once that is added move to the next link in the same bucket and
			temp = temp->next;
		}
	}
	//first remove the links from the old links from the hash map
	hashMapCleanUp(map);
	//set hashmap to the new bucket and link size and the new hash table.
	map->capacity = map2->capacity;
	map->size = map2->size;
	map->table = map2->table;
	free(map2);
}

/**
* Updates the given key-value pair in the hash table. If a link with the given
* key already exists, this will just update the value and skip traversing. Otherwise, it will
* create a new link with the given key and value and add it to the table
* bucket's linked list. You can use hashLinkNew to create the link.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @param value
*/

void hashMapPut(HashMap* map, const char* key, int value)
{
	//first thing I ge the index using the formula from our lectures and worksheet 37
	int idx = HASH_FUNCTION(key) % map->capacity;
	//using the same example that was in our lecture video
	if (idx < 0)
	{
		idx += map->size;
	}
	//now that the index has been assigned I create a new link and set it to the the link in the hash table by indexing
	//from the lecture it talked about the buckets having null values if it was empty so I first use an if statement
	//if the bucket is not null that means there is  links so I need add the new link in by traversing until a reach the end of the list
	HashLink *new = map->table[idx];
	//now if that bucket is not empty then that means there already is a link there so I need to traverse until
	//I reach a next->null where I then can add the new link.

	if (new != NULL)
	{
		while (new != 0)
		{
			//testing for special circumstance from the tests if the key matches a key in the list then I update the value and return
			//otherwise I continue to march through the buckets links until I reach the end at which point I break out of loop
			if (strcmp(new->key, key) != 0)
			{
				new = new->next;
			}
			else
			{
				new->value = value;
				return;
			}
		}
		//now that we reach a null as the next link I am now able to add the new link
		//now this time I dont use NULL as argument to the hashlinkNew because this time link has next 
		HashLink *temp = hashLinkNew(key, value, map->table[idx]);
		map->table[idx] = temp;
		map->size++;
	}
	else 
	{
		HashLink *temp = hashLinkNew(key, value, NULL);
		map->table[idx] = temp;
		map->size++;
	}
		float loadTest = hashMapTableLoad(map);
	int loadCap = map->capacity;
		if (loadTest > MAX_TABLE_LOAD)
	{
		resizeTable(map, loadCap * 2);
	}	
}

/**
* Removes and frees the link with the given key from the table. If no such link
* exists, this does nothing. Remember to search the entire linked list at the
* bucket. You can use hashLinkDelete to free the link.
* @param map
* @param key
*/
void hashMapRemove(HashMap* map, const char* key)
{
	int idx = HASH_FUNCTION(key) % map->capacity;
	HashLink *temp = map->table[idx];
	//now that I have indexed to the bucket using the hash function I first test if that bucket even has values
	//if it has values in it that means the bucket is not null
	if (temp != NULL)
	{
		//now I test if the key value matches the key provided for the first link
		//if it does match that makes it easy I set the bucket to the links next which will be null so
		//it will make the bucket also null.
		if (strcmp(temp->key, key) == 0)
		{
			map->table[idx] = temp->next;
		}
		//now the issue would be if the first one does not match then I have to traverse the list until it
		//is found. The issue is I have to track the previos value as well because I want the previos value
		//to then point to the removed links next.
		else
		{
			HashLink *prev = NULL;
			while (strcmp(temp->key, key) != 0)
			{
				prev = temp;
				temp = temp->next;
			}
			//now if the prev has links that are next I need to link them skipping over the removal key
			/*[prev hash] -> [remove hash] ->[next hash]
					|                           ^
					----------------------------|
			*/
			if (prev != 0)
			{
				prev->next = temp->next;
			}

		}
	}
	//now finally remove the hash 
	hashLinkDelete(temp);
	map->size--;
}

/**
* Returns 1 if a link with the given key is in the table and 0 otherwise.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @return 1 if the key is found, 0 otherwise.
*/
int hashMapContainsKey(HashMap* map, const char* key)
{
	//now the contains function is the simiplest one. first I calculate the index by using the hash function
	//once the key has been hashed I use the strcmp function to find the matching key. If the matching key is found
	//return 1 else return 0.

	int idx = HASH_FUNCTION(key) % map->capacity;
	if (idx < 0)
	{
		idx += map->capacity;
	}
	HashLink *temp = map->table[idx];
	while (temp != 0)
	{
		if (strcmp(temp->key, key) == 0)
		{
			return 1;
		}
		else
		{
			temp = temp->next;
		}
	}
	return 0;
}

/**
* Returns the number of links in the table.
* @param map
* @return Number of links in the table.
*/
int hashMapSize(HashMap* map)
{
	int links = map->size;
	return links;
}

/**
* Returns the number of buckets in the table.
* @param map
* @return Number of buckets in the table.
*/
int hashMapCapacity(HashMap* map)
{

	//super simple the hashmap struct contains the buckets which is the capacity.
	int buckets = map->capacity;

	return buckets;
}

/**
* Returns the number of table buckets without any links.
* @param map
* @return Number of empty buckets.
*/
int hashMapEmptyBuckets(HashMap* map)
{
	//this is super simple just need to find buckets without links in otherwords.
	//buckets that have null value.
	int bucketcounter = 0;
	for (int idx = 0; idx < map->capacity; idx++)
	{
		if (map->table[idx] == NULL)
		{
			bucketcounter += 1;
		}
	}
	return bucketcounter;
}

/**
* Returns the ratio of (number of links) / (number of buckets) in the table.
* Remember that the buckets are linked lists, so this ratio tells you nothing
* about the number of empty buckets. Remember also that the load is a floating
* point number, so don't do integer division.
* @param map
* @return Table load.
*/
float hashMapTableLoad(HashMap* map)
{
	float links = hashMapSize(map);
	float  buckets = hashMapCapacity(map);
	float tableLoad;

	tableLoad = links / buckets;
	return tableLoad;
}

/**
* Prints all the links in each of the buckets in the table.
* @param map
*/
void hashMapPrint(HashMap* map)
{
	for (int i = 0; i < map->capacity; i++)
	{
		HashLink* link = map->table[i];
		if (link != NULL)
		{
			printf("\nBucket %i ->", i);
			while (link != NULL)
			{
				printf(" (%s, %d) ->", link->key, link->value);
				link = link->next;
			}
		}
	}
	printf("\n");
}
