#include <stdlib.h>
#include <errno.h>
#include "hashtable.h"

/********************************************************************
 * These functions are internal and not meant to be used as an API. *
 *******************************************************************/

static unsigned int hash(char *key, size_t hashtable_size) {
    /* Basic hashing function:
     * Loop on the string, shift each letter's ASCII value by <shift>
     * and store the result. */
    int shift = 2, sum = 0;
    int i = 0;
    while(key[i] != '\0') {
        sum += key[i] << shift;
        i++;
        shift += 3;
    }
    return sum % hashtable_size;
}

static int get_load_factor(struct hashtable *ht) {
    /* Not unsigned int because it can be negative as well */
    return ht->entries / ht->size;
}

static void _hadd_(struct hashtable *ht, struct pair *kvpair, unsigned int hashvalue) {
    struct pair *pairptr = ht->table[hashvalue];
    if (!pairptr) {
        /* First pair on this index */
        ht->table[hashvalue] = kvpair;
    }
    else {
        /* Link */
        kvpair->next = ht->table[hashvalue];
        ht->table[hashvalue] = kvpair;
    }
}

static void _resize(struct hashtable *ht) {
    /* We try to allocate extra memory for our hashtable.
     * We create an array of ht->entries on the stack.
     * We copy all our entries from ht to that array.
     * We reset all buckets in the newly allocated table to NULL. */
    struct pair **newtable = realloc(ht->table, sizeof(ht->table) * (ht->size * 2.5));
    if (newtable == NULL) {
        /* Memory allocation error */
        return;
    }
    ht->size *= 2.5;
    struct pair *entries[ht->entries];
    struct pair *kvpair;
    unsigned int entryindex = 0;
    for(unsigned int i = 0; i < ht->size; i++) {
        struct pair *kvpair = newtable[i];
        if (kvpair == NULL) { continue; }
        while(kvpair != NULL) {
            /* Traverse the list at the given index (i),
             * and add its element to the entires array */
            entries[entryindex] = kvpair;
            struct pair *previous = kvpair;
            entryindex++;
            kvpair = kvpair->next;
            /* We have to set each element's "next" pointer to NULL, otherwise it might still be linked to other structs */
            previous->next = NULL;
        }
        /* We've copied all the list entries in this bucket. Now we set its head to NULL */
        newtable[i] = NULL;
    }
    /* We've copied all pair pointers into our temporary array,
     * and at the same time we've initialized
     * the newly allocated memory blocks to NULL.
     * Now it's time to rehash the entries */
    ht->table = newtable;
    for(unsigned int i = 0; i < ht->entries; i++) {
        kvpair = entries[i];
        unsigned int hashvalue = hash(kvpair->key, ht->size);
        _hadd_(ht, kvpair, hashvalue);
    }
}

/**********************************************
 * The following functions constitute the API *
 * ********************************************/

struct hashtable *hmake(size_t size) {
    struct hashtable *ht = malloc(sizeof(*ht));
    if (ht == NULL) { return NULL; }
    if (size < 1) { size = 1; }
    /* Initialize the table itself */
    ht->size = size;
    ht->entries = 0;
    ht->table = malloc(sizeof(ht->table) * size);
    for(unsigned int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

int hadd(struct hashtable *ht, char *key, char *value) {
    if (ht == NULL) { return EFAULT; }
    if (get_load_factor(ht) >= 15) {
        // Hash table is full. Need to resize it and rehash all the elements.
        _resize(ht);
    }
    unsigned int hashvalue = hash(key, ht->size);
    struct pair *pairptr = ht->table[hashvalue];
    /* We traverse the list until we reach its last element */
    while(pairptr != NULL) {
        if (pairptr->key == key) { 
            /* Duplicate. Modify value and return */
            pairptr->value = value;
            return 0;
        }
        pairptr = pairptr->next;
    }
    /* Initialize and link the new key-value pair */
    struct pair *kvpair = malloc(sizeof(*kvpair));
    if (!kvpair) { return ENOMEM; }
    kvpair->key = key;
    kvpair->value = value;
    kvpair->next = NULL;
    ht->entries++;
    _hadd_(ht, kvpair, hashvalue);
    return 0;
}

char *hget(struct hashtable *ht, char *key) {
    if (ht == NULL) { return NULL; }
    
    /* Get the hash value, and the first key-value pair at the given index */
    unsigned int hashvalue = hash(key, ht->size);
    struct pair *kvpair = ht->table[hashvalue];
    if (kvpair == NULL) { return NULL; }
    
    while(kvpair != NULL) {
        if (kvpair->key == key) {
            return kvpair->value;
        }
        else { 
            kvpair = kvpair->next;
        }
    }
    /* If execution reached here, no matching key was found. */
    return NULL;
}

void hdel(struct hashtable *ht, char *key) {
    /* Remove a key-value pair from the table */
    if (ht == NULL) { return; }

    unsigned int hashvalue = hash(key, ht->size);
    struct pair *kvpair = ht->table[hashvalue];
    if (kvpair == NULL) { return; }
    if (kvpair->key == key) {
       /* First element.
        * We set the head of this index to kvpair's next element.
        * This can be either another struct, or NULL. */
       ht->table[hashvalue] = kvpair->next;
       free(kvpair);
       ht->entries--;
    }
    else {
        while(kvpair != NULL) {
            struct pair *nextpair = kvpair->next;
            if (nextpair->key == key) {
                kvpair->next = nextpair->next;
                free(nextpair);
                ht->entries--;
                return;
            }
            kvpair = nextpair;
        }
    }
}

void hfree(struct hashtable *ht) {
    /* Clear out the hash table and free its pointer.
     * **********************************************
     * We go through all the buckets,
     * free all their element pointers,
     * and then free the hashtable pointer itself */

    if (ht == NULL) { return; }
    for(unsigned int i = 0; i < ht->size; i++) {
        struct pair *head = ht->table[i];
        if (head == NULL) { continue; }
        while(head != NULL) {
            struct pair *nextpair = head->next;
            free(head);
            head = nextpair;
        }
    }
    /* We now have an empty array */
    free(ht->table);
    free(ht);
}
