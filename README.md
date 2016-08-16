Simple hash tables implementation in C.
This is my first serious C program beyond K&R exercises.

Compilation:
gcc -Wall -c hashtable.c

Compiling with your code:
gcc -Wall -o executable hashtable.o executable.c

API functions:

### Make a new table:
struct hashtable *hmake(size_t size);

### Add a key-value pair to the table:
int hadd(struct hashtable *ht, char *key, char *value);

### Delete a key-value pair from the table:
void hdel(struct hashtable *ht, char *key);

### Get a value from the table:
char *hget(struct hashtable *ht, char *key);

### Delete (free) the entire table:
void hfree(struct hashtable *ht);
