#ifndef HASH_H
#define HASH_H

#define HASH_FUNCTION hashFunction1
#define MAX_TABLE_LOAD 10

typedef struct HashMap HashMap;
typedef struct HashLink HashLink;

struct HashLink
{
	char* key;
	int value;
	HashLink* next;
};

struct HashMap
{
	HashLink** table;
	// Number of links in the table.
	int size;
	// Number of buckets in the table.
	int capacity;
};

HashMap* hashMapNew(int capacity);
void hashMapDelete(HashMap* map);
int* hashMapGet(HashMap* map, const char* key);
void hashMapPut(HashMap* map, const char* key, int value);
void hashMapRemove(HashMap* map, const char* key);
int hashMapContainsKey(HashMap* map, const char* key);

int hashMapSize(HashMap* map);
int hashMapCapacity(HashMap* map);
int hashMapEmptyBuckets(HashMap* map);
float hashMapTableLoad(HashMap* map);
void hashMapPrint(HashMap* map);

#endif
