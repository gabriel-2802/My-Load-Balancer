#pragma once
#include "server.h"

void resize_server_list(server_memory_t ***v, int size, int *capacity);

void resize_array(void **v, int capacity, unsigned int data_size);

void resize_double_array(void  ***v, int capacity, unsigned int data_size);

/*functia adauga un element pe pozitia pos dintr-un vector*/
void add_nth_array(void *v, int *size, int pos, void *src, unsigned int data_size);

/*functia gaseste cea mai apropiata pozitie a unui 'target' fata de un element din vector*/
int search_closest(unsigned int *v, unsigned int target, int size);

/*functia gaseste pozitia pe care trebuie adaugat un element intr-un vector sortat*/
// int search_pos(void *v, int size, void *target, unsigned data_size);

int search_pos(unsigned int *v, int size, unsigned int target);

void rebalance_items(hashtable_t *src, int dest_idx, hashtable_t *dest, ring_t *ring, int size);

void rebalancing(server_memory_t *src, int dest_idx, server_memory_t *dest, ring_t *ring, int size);

void resize_load_balancer(load_balancer_t *main);

void add_server_ring(ring_t *ring, int *size, int server_id, unsigned int hash_id, server_memory_t *server, int pos);

void remove_server_ring(ring_t *ring, int *size, int pos);

void redistribute_items(hashtable_t *src, load_balancer_t *main);

void redistribution(load_balancer_t *main, server_memory_t *removed);

void mark_removed_server(int *removed, int id, int *servers_id, int size);
