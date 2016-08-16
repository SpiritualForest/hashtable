#ifndef HASHTABLE_H_DEFINED
#define HASHTABLE_H_DEFINED

#include <stdlib.h> /* for size_t */

struct pair {
    char *key;
    char *value;
    struct pair *next;
};

struct hashtable {
    int size;
    unsigned int entries;
    struct pair **table; /* Pointer to an array of pairs */
};

struct hashtable *hmake(size_t size);
int hadd(struct hashtable *ht, char *key, char *value);
void hdel(struct hashtable *ht, char *key);
char *hget(struct hashtable *ht, char *key);
void hfree(struct hashtable *ht);

#endif /* hashtable.h */
