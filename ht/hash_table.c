/*
 Student Name:
 Date:

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file. 

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
  //allocate space in memory to create HTE
 HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
 newEntry->key= key;
 newEntry->value= value;
 newEntry->next = NULL;
 return newEntry; 
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
unsigned int whichBucket= hashTable->hash(key);
HashTableEntry* tempHead = hashTable->buckets[whichBucket];
//tempHead = tempHead + whichBucket;

if(tempHead==NULL){
  //printf("I'm Here\n");
  return NULL;
}

while(tempHead != NULL){
  //run down the list till you reach your key
  if(tempHead->key==key){
    return tempHead;
  }
  tempHead = tempHead->next;
}
return NULL;
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
  //destruction 
  int totalBuckets = 1;
  int out = 1;
  while(out){
    if(hashTable->hash(totalBuckets) == 0){
      out = 0;
    }
    else{
      totalBuckets = totalBuckets + 1;
    }
  }
  //now that I have the total amount of buckets we run down the buckets and free HTE
  for(int i=0; i<totalBuckets; i++){
    if(hashTable->buckets[i]== NULL){
      //empty header
      continue;
    }
    else if(hashTable->buckets[i] != NULL){
      //not empty
      HashTableEntry* ptr = hashTable->buckets[i];
      //check and see if it is the only one in the bucket
      if(ptr->next == NULL){
        //free the value first then the HTE
       //printf("Should be here.\n");
        free(ptr->value);
        free(ptr);
        //next index in bucket 
        continue;
      }
      else if(ptr->next != NULL){
        // the list is greater than 1 then we need to move down it and free
       while(ptr !=NULL){
        HashTableEntry* tempNext = ptr->next;
        printf("never here.\n");
        free(ptr->value);
        free(ptr);
        ptr = tempNext;
        }
        continue;
      }
    }
  }
  //printf("The total Bucket is:%d\n", totalBuckets);
  //free(hashTable->buckets[0]->value);
  //free(hashTable->buckets[0]);
  free(hashTable->buckets);
  free(hashTable);
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  unsigned int bucketNum = hashTable->hash(key);
  HashTableEntry* this_Entry = findItem(hashTable, key);
  if(this_Entry != NULL){
    //replace the value since it is a duplicate
    void* old_Value = this_Entry->value;
    this_Entry->value = value;
    return old_Value;
  }
  else if(this_Entry == NULL){
    //not in the bucket list, allocate for it  
    HashTableEntry* new_Entry = createHashTableEntry(key, value);
    //make sure that the bucket is empty or not
    //if bucket is empty we set new_Entry to the head and update the bucket as well return NULL 
    if(hashTable->buckets[bucketNum] == NULL){
      hashTable->buckets[bucketNum] = new_Entry;
      return NULL;
    }
    //else the new_Entry needs to be added to the list and we have to run down it till we get to NULL and then update the previous->next and then link
    else{
      HashTableEntry* move_list = hashTable->buckets[bucketNum];
      while(move_list != NULL){
        if(move_list->next == NULL){
          //which means we have reached at the end of the list 
          move_list->next = new_Entry;
          return NULL;
        }
        //update to keep moving down till NULL
        move_list = move_list->next;
      }
    }
  }
  //HashTableEntry* new_HTE = createHashTableEntry(key,value);
  //hashTable->buckets[bucketNum]=new_HTE; 
}

void* getItem(HashTable* hashTable, unsigned int key) {
//unsigned int bucketNum = hashTable->hash(key);
HashTableEntry* this_Entry = findItem(hashTable, key);

if(this_Entry == NULL){
  return NULL; //not there
}
return this_Entry->value;
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  //first you would want to find the Item before you delete it
  unsigned int bucketNum= hashTable->hash(key);
  HashTableEntry* this_Entry = findItem(hashTable, key);
  //if it doesn't exist return NULL
  if(this_Entry==NULL){
    return NULL;
  }
  else if (this_Entry != NULL){
  //need to make sure where in the list the item we want to remove is (head, tail or in between)
    void* valueDel = this_Entry->value;
    if(this_Entry == hashTable->buckets[bucketNum]){
      //this is the head of the list
      //check and see if it is the only one in the list
      if(this_Entry->next == NULL){
        //replace bucket index with NULL and then free the HTE and return the value
        hashTable->buckets[bucketNum] = NULL;
        free(this_Entry);
        return valueDel;
      }
      else if(this_Entry->next != NULL){
        //it is the head of a list but not alone in the list
        hashTable->buckets[bucketNum] = this_Entry->next;
        free(this_Entry);
        return valueDel;
      }
    }
    else if (hashTable->buckets[bucketNum] != NULL){
      // run down the list till we are before the key, before to the next 
      HashTableEntry* ptr = hashTable->buckets[bucketNum];
      while(ptr != NULL){
        if(ptr->next->key == key){
          ptr->next = this_Entry->next;
          free(this_Entry);
          return valueDel;
        }
      }
    }
  }
}

void deleteItem(HashTable* hashTable, unsigned int key) {
void* temp = removeItem(hashTable, key);
}

