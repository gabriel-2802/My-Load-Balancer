#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "ll_lists.h"
#include "utils.h"
#include "server.h"
#include "load_balancer.h"

void resize_server_list(server_memory_t ***v, int size, int *capacity)
{
    if (size == *capacity) {
        server_memory_t **aux = realloc(*v, 2 * (*capacity) * sizeof(server_memory_t *));
        DIE(!aux, "realloc\n");
        *capacity = 2 * (*capacity);
        *v = aux;
    }
}

void resize_array(void **v, int capacity, unsigned int data_size)
{
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

void add_nth_array(void *v, int *size, int pos, void *src, unsigned int data_size)
{
    DIE((pos < 0 || pos > *size), "invalid position");
    for (int i = *size - 1; i >= pos; --i) {
        memcpy(v + (i + 1) * data_size, v + i * data_size, data_size);
    }
    

    memcpy(v + pos * data_size, src, data_size);
    (*size)++;
}

int search_closest(unsigned int *v, unsigned int target, int size)
{
    if (target > v[size - 1] || target < v[0])
        return 0;
    
    if (target == v[size - 1] || (target < v[size - 1] && target > v[size - 2]))
        return size - 1;
    
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
    for (int i = 0; i < size; ++i) {
        if (target < v[i])
            return i;
        else if (target == v[i])
            return i;

    }
    return size;
}

void rebalance_items(hashtable_t *src, int dest_idx, hashtable_t *dest, ring_t *ring, int size)
{
    for (unsigned int i = 0; i < src->hmax; ++i) {
        ll_node_t *item_node = src->buckets[i]->head;
        info_t * item = NULL;

        for (unsigned int j = 0; j < src->buckets[i]->size; ++j) {
            char *key = ((info_t *)item_node->data)->key;
            unsigned int key_hash = hash_function_key(key);

            if (search_closest(ring->hash, key_hash, size) == dest_idx) {
                item = (info_t *)item_node->data;
                ht_put(dest, item->key, strlen(item->key) + 1, item->value, strlen(item->value) + 1);
                item_node = item_node->next;
                //ht_remove_entry(src, key);
            } else {
                item_node = item_node->next;
            }
        }
    }
}

void rebalancing(server_memory_t *src, int dest_idx, server_memory_t *dest, ring_t *ring, int size)
{
    rebalance_items(src->memory, dest_idx, dest->memory, ring, size);
}

void resize_load_balancer(load_balancer_t *main)
{
    if (main->servers == main->max_servers) {
        main->max_servers = main->max_servers * 2;
        resize_array((void **)&main->ring->hash, main->max_servers, sizeof(unsigned int));
        resize_array((void **)&main->ring->servers_id, main->max_servers, sizeof(int));
        resize_double_array((void ***)&main->ring->servers, main->max_servers, sizeof(server_memory_t *));
    }
}

void add_server_ring(ring_t *ring, int *size, int server_id, unsigned int hash_id, server_memory_t *server, int pos)
{
    for (int i = *size - 1; i >= pos; --i) {
        ring->hash[i + 1] = ring->hash[i];
        ring->servers_id[i + 1] = ring->servers_id[i];
        ring->servers[i + 1] = ring->servers[i];
    }

    ring->hash[pos] = hash_id;
    ring->servers_id[pos] = server_id;
    ring->servers[pos] = server;
    (*size)++;
}

void remove_server_ring(ring_t *ring, int *size, int pos)
{
    for (int i = pos; i < *size - 1; ++i) {
        ring->hash[i] = ring->hash[i + 1];
        ring->servers_id[i] = ring->servers_id[i + 1];
        ring->servers[i] = ring->servers[i + 1];
    }
    (*size)--;
}

void redistribute_items(hashtable_t *src, load_balancer_t *main)
{
    for (unsigned int i = 0; i < src->hmax; ++i) {
        ll_node_t *item_node = src->buckets[i]->head;
        info_t *item = NULL;
        
        for (unsigned int j = 0; j < src->buckets[i]->size; ++j) {
            item = (info_t *)item_node->data;

            char key[KEY_LENGTH], value[VALUE_LENGTH];
            strcpy(key, (char *)item->key);
            strcpy(value, (char *)item->value);

            int id;

            loader_store(main, key, value, &id);
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
    for (int i = 0; i < size; ++i)
        if (servers_id[i] == id)
            removed[i] = 1;
}
