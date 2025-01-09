#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>

#include "hash_table.h"
#include "prime.h"

static void ht_resize_up(ht_hash_table *ht);
static void ht_resize_down(ht_hash_table *ht);


static ht_item HT_DELETED_ITEM = { NULL , NULL } ;

static ht_hash_table * ht_new_sized( const int base_size ) {
    ht_hash_table * ht = malloc( sizeof( ht_hash_table ) ) ;
    ht -> base_size = base_size ;

    ht -> size = next_prime( ht -> base_size ) ;

    ht -> count = 0 ;
    ht -> items = calloc( ( size_t )ht -> size , sizeof( ht_item * ) ) ;
    return ht ;
}

// allocate memory for a new item, store in our struct
static ht_item * ht_new_item( const char *k, const char *v ) {
    ht_item * i = malloc( sizeof( ht_item ) ) ;
    i -> key = strdup( k ) ;
    i -> value = strdup( v ) ;
    return i ;
}

// allocate memory for a new hash table, default size/count, items
ht_hash_table * ht_new() {
    return ht_new_sized( HT_INITIAL_BASE_SIZE ) ; 
    }




// free memory for item
static void ht_del_item( ht_item * i ) {
    if ( i != NULL && i != &HT_DELETED_ITEM ) {
        free( i -> key ) ;
        free( i -> value ) ;
        free( i ) ;
    }
}

// free memory for a table
void ht_del_hash_table( ht_hash_table * ht ) {
    for( int i = 0 ; i < ht -> size ; i++ ){
        ht_item* item = ht -> items[ i ] ;
        if ( item != NULL ) {
            ht_del_item( item ) ;
        }
    }
    free( ht -> items ) ;
    free( ht ) ;
}

static int modular_pow(int base, int exp, int mod) {
    int result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

// hash function
// choosing a 'perfect' hash function is impossible, create a function that will perform well for the expected data set
static int ht_hash( const char * s , const int a , const int m ) {
    long hash = 0 ;
    const int len_s = strlen( s ) ;
    for ( int i = 0 ; i < len_s; i++ ) {
        hash = ( hash + ( long )modular_pow( a , len_s - (i + 1) , m ) * s[ i ] ) % m ;
    }
    return ( int )hash ;
}

static int ht_get_hash ( const char * s , const int num_buckets , const int attempt ) {
    const int hash_a = ht_hash( s , HT_PRIME_1 , num_buckets ) ;
    const int hash_b = ht_hash( s , HT_PRIME_2 , num_buckets ) ;
    return ( hash_a + ( attempt * ( hash_b + 1 ) ) ) % num_buckets ;
}

void ht_insert( ht_hash_table * ht , const char * key , const char * value ) {
    const int load = ht -> count * 100 / ht -> size ;
    if ( load > 70 ) {
        ht_resize_up( ht ) ;
    }
    ht_item* item = ht_new_item( key , value );
    int index = ht_get_hash( item -> key , ht -> size, 0);
    ht_item * cur_item = ht -> items[ index ];
    int i = 1 ;
    while ( cur_item != NULL ) {
        if ( cur_item != &HT_DELETED_ITEM ) {
            if ( strcmp( cur_item -> key , key ) == 0 ) {
                ht_del_item( cur_item ) ;
                ht -> items[ index ] = item ;
                return ;
            }
        }
        index = ht_get_hash( item -> key , ht -> size , i ) ;
        cur_item = ht -> items[ index ] ;
        i++ ;   
    }
    ht -> items[ index ] = item ;
    ht -> count++ ;
}

char * ht_search( ht_hash_table * ht , const char * key ) {
    int index = ht_get_hash( key , ht -> size , 0 ) ;
    ht_item * item = ht -> items[ index ] ;
    int i = 1 ;
    while ( item != NULL ) {
        if ( item != &HT_DELETED_ITEM ) {
            if ( strcmp( item -> key, key ) == 0 ) {
                return item -> value ;
            }
        }
        index = ht_get_hash( key , ht -> size , i ) ;
        item = ht -> items[ index ] ;
        i++ ;
    }
    return NULL ;
}



void ht_delete( ht_hash_table * ht , const char * key ) {
    const int load = ht -> count * 100 / ht -> size ;
    if ( load < 10 ) {
        ht_resize_down( ht ) ;
    }
    int index = ht_get_hash( key , ht -> size , 0 ) ;
    ht_item * item = ht -> items[ index ] ;
    int i = 1 ;
    while( item != NULL ) {
        if( item != &HT_DELETED_ITEM ) {
            if( strcmp( item -> key, key ) == 0 ) {
                ht_del_item( item ) ;
                ht -> items[ index ] = &HT_DELETED_ITEM ;
                ht -> count-- ;
                return ;
            }
        }
        index = ht_get_hash( key , ht -> size , i );
        item = ht -> items[ index ] ;
        i++ ;
    }
}

static void ht_resize( ht_hash_table * ht , const int base_size ) {
    if( base_size < HT_INITIAL_BASE_SIZE ) {
        return ;
    }
    ht_hash_table * new_ht = ht_new_sized( base_size ) ;
    for( int i = 0; i < ht -> size; i++ ) {
        ht_item * item = ht -> items[ i ] ;
        if ( item != NULL && item != &HT_DELETED_ITEM ) {
            ht_insert( new_ht , item -> key , item -> value ) ;
        }
    }
    ht -> base_size = new_ht -> base_size ;
    ht -> count = new_ht -> count ;

    const int tmp_size = ht -> size ;
    ht -> size = new_ht -> size ;
    new_ht -> size = tmp_size ;

    ht_item ** tmp_items = ht -> items ;
    ht -> items = new_ht -> items ;
    new_ht -> items = tmp_items ;

    ht_del_hash_table( new_ht ) ;
}

static void ht_resize_up( ht_hash_table * ht ) {
    const int new_size = ht -> base_size * 2 ;
    ht_resize( ht, new_size ) ;
}

static void ht_resize_down( ht_hash_table * ht ) {
    const int new_size = ht -> base_size / 2 ;
    ht_resize( ht , new_size ) ;
}

void ht_export_to_json(ht_hash_table *ht, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    json_object *hash_table = json_object_new_array();
    for (int i = 0; i < ht->size; i++) {
        if (ht->items[i] != NULL && ht->items[i] != &HT_DELETED_ITEM) {
            json_object *bucket = json_object_new_object();
            json_object_object_add(bucket, "index", json_object_new_int(i));
            json_object_object_add(bucket, "key", json_object_new_string(ht->items[i]->key));
            json_object_object_add(bucket, "value", json_object_new_string(ht->items[i]->value));
            json_object_array_add(hash_table, bucket);
        } else {
            json_object *bucket = json_object_new_object();
            json_object_object_add(bucket, "index", json_object_new_int(i));
            json_object_object_add(bucket, "key", json_object_new_string("empty"));
            json_object_object_add(bucket, "value", json_object_new_string("empty"));
            json_object_array_add(hash_table, bucket);
        }
    }

    fprintf(file, "%s\n", json_object_to_json_string(hash_table));
    json_object_put(hash_table);
    fclose(file);
}

