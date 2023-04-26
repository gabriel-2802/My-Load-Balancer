#include <string.h>
#include <stdlib.h>
#include "ll_lists.h"
#include "utils.h"
#include "hashtable.h"

int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

void key_val_free_function(void *data)
{
	info_t *data_info = (info_t *)data;
	free(data_info->key);
	free(data_info->value);
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
		       int (*compare_function)(void *, void *),
		       void (*key_val_free_function)(void *))
{
	if (!hash_function || !compare_function) {
		return NULL;
	}

	hashtable_t *map = malloc(sizeof(hashtable_t));
    DIE(!map, "malloc\n");

	map->size = 0;
	map->hmax = hmax;
	map->hash_function = hash_function;
	map->compare_function = compare_function;
	map->key_val_free_function = key_val_free_function;

	map->buckets = malloc(map->hmax * sizeof(ll_list_t *));
	for (unsigned int i = 0; i < map->hmax; ++i) {
		map->buckets[i] = ll_create(sizeof(info_t));
        DIE(!map->buckets[i], "Malloc\n");

	}

	return map;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable
 * folosind functia put 0, altfel.
 */
int ht_has_key(hashtable_t *ht, void *key)
{
	if (!ht || !key) {
		return -1;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;
		if (!ht->compare_function(data_info->key, key)) {
			return 1;
		}
		node = node->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1) {
		return NULL;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;
		if (!ht->compare_function(data_info->key, key)) {
			return data_info->value;
		}
		node = node->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value,
	    unsigned int value_size)
{
	if (!ht || !key || !value) {
		return;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;

	if (ht_has_key(ht, key) == 1) {
		ll_node_t *node = ht->buckets[hash_index]->head;
		while (node != NULL) {
			info_t *data_info = node->data;

			if (!ht->compare_function(data_info->key, key)) {
				free(data_info->value);

				data_info->value = malloc(value_size);

				memcpy(data_info->value, value, value_size);
				return;
			}

			node = node->next;
		}
	}

	info_t *data_info = malloc(sizeof(info_t));

	data_info->key = malloc(key_size);
	data_info->value = malloc(value_size);

	memcpy(data_info->key, key, key_size);
	memcpy(data_info->value, value, value_size);

	ll_add_nth_node(
	    ht->buckets[hash_index], 0,
	    data_info); // adaugam fiecare nod nou pe prima pozitie a listei
	ht->size++;

	free(data_info);
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1) {
		return;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	unsigned int node_nr = 0;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;

		if (!ht->compare_function(data_info->key, key)) {
			ht->key_val_free_function(data_info);
			free(data_info); // eliberam memoria la care pointeaza
					 // membrul data din nod

			ll_node_t *deleted_node = ll_remove_nth_node(
			    ht->buckets[hash_index],
			    node_nr); // captam nodul care trebuie eliberat
			free(
			    deleted_node); // eliberam memoria la care pointeaza
					   // nodul ce a fost exclus din lista

			ht->size--;
			return;
		}

		node = node->next;
		node_nr++;
	}
}

void ht_free(hashtable_t *ht)
{
	if (!ht) {
		return;
	}

	for (unsigned int i = 0; i < ht->hmax; ++i) {
		ll_node_t *node = ht->buckets[i]->head;

		while (node != NULL) {
			ht->key_val_free_function(node->data);
			node = node->next;
		}

		ll_free(&ht->buckets[i]); // eliberam memoria la care pointeaza
					  // lista impreuna cu memoria la care
					  // pointeaza nodurile
	}

	free(ht->buckets); // eliberam memoria la care pointeaza dublul pointer
			   // buckets, deci avem 0 liste in array
	free(ht); // eliberam memoria la care pointeaza ht, deci stergem
		  // hashtable-ul
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}