/* copyright Carauleanu Valentin Gabriel 311Cab 2023 */
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "ll_lists.h"
#include "utils.h"
#include "server.h"
#include "load_balancer.h"

void resize_array(void **v, int capacity, unsigned int data_size)
{
	/* realocam vectorul intr-o var auxiliara din considerente defensive */
	void *aux = realloc(*v, capacity * data_size);
	DIE(!aux, "realloc\n");
	*v = aux;
}

void resize_double_array(void  ***v, int capacity, unsigned int data_size)
{
	void **aux = realloc(*v, capacity * data_size);
	DIE(!aux, "realloc\n");
	*v = aux;
}

int search_closest(unsigned int *v, unsigned int target, int size)
{
	/* verificam daca target trebuie adaugat la final sau inceput*/
	if (target > v[size - 1] || target < v[0])
		return 0;

	/* verificam daca target trebuie adaugat in penultima pozitie */
	if (target == v[size - 1] || (target < v[size - 1] &&
		target > v[size - 2]))
		return size - 1;

	/* aplicam binarry search pentru restul pozitiilor */
	int left = 0, right = size - 1, mid;
	while (left < right) {
		mid = left + (right - left) / 2;

		if (v[mid] == target)
			return mid;

		if (mid != 0 && target > v[mid - 1] && target < v[mid])
			return mid;

		if (v[mid] > target)
			right = mid;
		else
			left = mid + 1;
	}
	return 0;
}

int search_pos(unsigned int *v, int size, unsigned int target)
{
	int left = 0, right = size - 1;

	while (left <= right) {
		int mid = left + (right - left) / 2;

		if (v[mid] == target)
			return mid;
		else if (v[mid] < target)
			left = mid + 1;
		else
			right = mid - 1;
	}

	return left;
}

void rebalance_items(hashtable_t *src, int dest_idx, hashtable_t *dest,
					 ring_t *ring, int size)
{
	/* parcurgem fiecare bucket din hashtable-ul sursa */
	for (unsigned int i = 0; i < src->hmax; ++i) {
		ll_node_t *item_node = src->buckets[i]->head;
		info_t * item = NULL;

		/* parcugem fiecare element din bucket */
		while (item_node) {
			char *key = ((info_t *)item_node->data)->key;
			/* obtinem hash-ul cheii */
			unsigned int key_hash = hash_function_key(key);

			/* daca hash-ul cheii este mai aprope de hash-ul noul server,
			mutam cheia si valoarea */
			if (search_closest(ring->hash, key_hash, size) == dest_idx) {
				item = (info_t *)item_node->data;
				ht_put(dest, item->key, strlen(item->key) + 1, item->value,
						strlen(item->value) + 1);
			}

			item_node = item_node->next;
		}
	}
}

void rebalancing(server_memory_t *src, int dest_idx, server_memory_t *dest,
				 ring_t *ring, int size)
{
	rebalance_items(src->memory, dest_idx, dest->memory, ring, size);
}

void resize_load_balancer(load_balancer_t *main)
{
	/* daca nr de server este egal cu capacotatea maxima a load_balancer-ului
	dublam capacitatea si realocam vectorii din hashring */
	if (main->servers == main->max_servers) {
		main->max_servers = main->max_servers * 2;
		resize_array((void **)&main->ring->hash, main->max_servers,
					 sizeof(unsigned int));
		resize_array((void **)&main->ring->servers_id, main->max_servers,
					 sizeof(int));
		resize_double_array((void ***)&main->ring->servers, main->max_servers,
							sizeof(server_memory_t *));
	}
}

void add_server_ring(ring_t *ring, int *size, int server_id,
					 unsigned int hash_id, server_memory_t *server, int pos)
{
	/* pentru a adauga un nou element vom shifta la dreapata vectorul */
	for (int i = *size - 1; i >= pos; --i) {
		ring->hash[i + 1] = ring->hash[i];
		ring->servers_id[i + 1] = ring->servers_id[i];
		ring->servers[i + 1] = ring->servers[i];
	}

	/* adaugam noul element */
	ring->hash[pos] = hash_id;
	ring->servers_id[pos] = server_id;
	ring->servers[pos] = server;
	(*size)++;
}

void remove_server_ring(ring_t *ring, int *size, int pos)
{
	/* pentru a sterge un server, vom shifta la stanga elementele
	vectorlui */
	for (int i = pos; i < *size - 1; ++i) {
		ring->hash[i] = ring->hash[i + 1];
		ring->servers_id[i] = ring->servers_id[i + 1];
		ring->servers[i] = ring->servers[i + 1];
	}
	(*size)--;
}

void redistribute_items(hashtable_t *src, load_balancer_t *main)
{
	/* parcurgem fiecare bucket */
	for (unsigned int i = 0; i < src->hmax; ++i) {
		ll_node_t *item_node = src->buckets[i]->head;
		info_t *item = NULL;

		/* parcurgem fiecare element din bucket */
		while (item_node) {
			item = (info_t *)item_node->data;

			/* vom copia fiecare pereche din src si o vom adauga in
			load_balancer */
			int id;
			loader_store(main, item->key, item->value, &id);
			item_node = item_node->next;
		}
	}
}

void redistribution(load_balancer_t *main, server_memory_t *removed)
{
	hashtable_t *memory = removed->memory;
	redistribute_items(memory, main);
}

void mark_removed_server(int *removed, int id, int *servers_id, int size)
{
	/* vom marca ca eliberat serverul cu id-ul primit ca parametru si
	copiile sale */
	for (int i = 0; i < size; ++i)
		if (servers_id[i] == id)
			removed[i] = 1;
}
