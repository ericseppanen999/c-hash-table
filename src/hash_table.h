#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

// ( key , value ) pairs
typedef struct {
    char * key ;
    char * value ;
} ht_item ;

// table , size , array of pointers to items
typedef struct {
    int size ;
    int count ;
    int base_size;
    ht_item **items ;
} ht_hash_table ;

#define HT_PRIME_1 151
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 50


/**
 * Create a new hash table.
 */
ht_hash_table * ht_new( ) ;

/**
 * Free memory for a hash table.
 */
void ht_del_hash_table( ht_hash_table * ht ) ;

/**
 * Insert a key-value pair into the hash table.
 */
void ht_insert( ht_hash_table * ht , const char * key , const char * value ) ;

/**
 * Search for a key in the hash table and return its value.
 */
char *ht_search( ht_hash_table * ht , const char * key ) ;

/**
 * Delete a key-value pair from the hash table.
 */
void ht_delete( ht_hash_table * ht , const char * key ) ;

#endif // HASH_TABLE_H