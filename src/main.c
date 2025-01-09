#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"
#include <json.h/json-c>


int main() {
    // Create a new hash table
    ht_hash_table *ht = ht_new();

    // Test insertions
    ht_insert(ht, "name", "Alice");
    ht_insert(ht, "age", "25");
    ht_insert(ht, "city", "Vancouver");

    // Test searching
    printf("Search for 'name': %s\n", ht_search(ht, "name")); // Should print "Alice"
    printf("Search for 'age': %s\n", ht_search(ht, "age"));   // Should print "25"
    printf("Search for 'city': %s\n", ht_search(ht, "city")); // Should print "Vancouver"
    printf("Search for 'country': %s\n", ht_search(ht, "country")); // Should print "(null)"

    // Test deletion
    ht_delete(ht, "age");
    printf("Search for 'age' after deletion: %s\n", ht_search(ht, "age")); // Should print "(null)"

    // Test collisions
    ht_insert(ht, "abc", "123");
    ht_insert(ht, "acb", "321"); // Likely to collide with "abc" depending on your hash function
    printf("Search for 'abc': %s\n", ht_search(ht, "abc")); // Should print "123"
    printf("Search for 'acb': %s\n", ht_search(ht, "acb")); // Should print "321"

    // Test resizing (Insert many elements)
    for (int i = 0; i < 100; i++) {
        char key[10];
        char value[10];
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "val%d", i);
        ht_insert(ht, key, value);
    }

    // Verify some keys after resizing
    printf("Search for 'key0': %s\n", ht_search(ht, "key0"));  // Should print "val0"
    printf("Search for 'key99': %s\n", ht_search(ht, "key99")); // Should print "val99"

    ht_export_to_json(ht, "hash_table_data.json");

    // Free the hash table
    ht_del_hash_table(ht);

    return 0;
}
