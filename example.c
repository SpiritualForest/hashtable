#include <stdio.h>
#include <errno.h>
#include "hashtable.h"

int main(void) {
    /* Make a hash table with 10 entries */
    struct hashtable *ht = hmake(10);
    if (!ht) {
        /* It's NULL, something went wrong */
        return EFAULT;
    }

    /* Add three key-value pairs, one with error checking */
    hadd(ht, "name", "John");
    hadd(ht, "age", "20");
    
    /* It's possible to check for errors with hadd(): */
    int status = hadd(ht, "location", "New York");
    
    if (status != 0) {
        printf("Memory allocation error\n");
        return status;
    }
    
    /* Get values */
    char *name = hget(ht, "name");
    char *age = hget(ht, "age");
    char *location = hget(ht, "location");

    /* Print the information to stdout */
    printf("Name: %s\nAge: %s\nLocation: %s\n", name, age, location);

    /* Delete keys */
    hdel(ht, "name");
    name = hget(ht, "name");

    printf("Name: %s\n", name);

    /* Free the entire table */
    hfree(ht);

    return 0;
}
