#pragma once
#include "ll_lists.h"
#include "utils.h"
#define MAX_STRING_SIZE 256
#define HMAX 200

typedef struct {
	void *key;
	void *value;
} info_t;

typedef struct  {
	ll_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	unsigned int hmax; /* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor.
	 */
	unsigned int (*hash_function)(void *);
	/* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void *, void *);
	/* (Pointer la) Functie pentru a elibera memoria ocupata de cheie si
	 * valoare. */
	void (*key_val_free_function)(void *);
} hashtable_t;

int compare_function_ints(void *a, void *b);

int compare_function_strings(void *a, void *b);

unsigned int hash_function_int(void *a);

unsigned int hash_function_string(void *a);

void key_val_free_function(void *data);

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
		       int (*compare_function)(void *, void *),
		       void (*key_val_free_function)(void *));

int ht_has_key(hashtable_t *ht, void *key);

void *ht_get(hashtable_t *ht, void *key);

void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value,
	    unsigned int value_size);

void ht_remove_entry(hashtable_t *ht, void *key);

void ht_free(hashtable_t *ht);

unsigned int ht_get_size(hashtable_t *ht);

unsigned int ht_get_hmax(hashtable_t *ht);