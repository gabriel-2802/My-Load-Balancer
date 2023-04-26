/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "load_balancer.h"
#include "ll_lists.h"
#include "aux_functions.h"
#include "hashtable.h"


unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer_t *init_load_balancer() {
    load_balancer_t *main = malloc(sizeof(load_balancer_t));
    DIE(!main, "malloc\n");

    main->servers = 0;
    main->max_servers = INITIAL_CAPACITY;

    main->ring = malloc(sizeof(ring_t));
    DIE(!main->ring, "malloc\n");

    main->ring->hash = calloc(INITIAL_CAPACITY, sizeof(unsigned int));
    DIE(!main->ring->hash, "calloc\n");

    main->ring->servers = calloc(INITIAL_CAPACITY, sizeof(server_memory_t *));
    DIE(!main->ring->servers, "calloc\n");

    main->ring->servers_id = calloc(INITIAL_CAPACITY, sizeof(int));
    DIE(!main->ring->servers_id, "calloc\n");

    return main;
}

void loader_add_server(load_balancer_t *main, int server_id)
{   
    server_memory_t *new_server = init_server_memory();

    for (unsigned int i = 0; i < 3; ++i) {
        int id = i * THOUSANDS + server_id;
        unsigned int hash_id = hash_function_servers(&id);

        int pos = search_pos(main->ring->hash, main->servers, hash_id);

        resize_load_balancer(main);
        add_server_ring(main->ring, &main->servers,server_id,
                        hash_id, new_server, pos);

        int next_pos = ((pos + 1) == main->servers ? 0 : (pos + 1));
        if (main->servers > 3 && main->ring->servers_id[pos] != main->ring->servers_id[next_pos]) {
            rebalancing(main->ring->servers[next_pos], pos,
                            main->ring->servers[pos], main->ring, main->servers);
        
        }
    }
}

void loader_remove_server(load_balancer_t *main, int server_id)
{
    server_memory_t *removed_server;
    for (int i = 0; i < 3; ++i) {
        int id = i * THOUSANDS + server_id;
        unsigned int hash_id = hash_function_servers(&id);
        int pos = search_pos(main->ring->hash,main->servers, hash_id);

       if (!i)
            removed_server = main->ring->servers[pos];
        remove_server_ring(main->ring, &main->servers, pos);

    }

    redistribution(main, removed_server);
    free_server_memory(removed_server);
 
}

void loader_store(load_balancer_t *main, char *key, char *value, int *server_id) {
    unsigned int hash_id = hash_function_key(key);
    int add_pos = search_closest(main->ring->hash, hash_id, main->servers);

    server_store(main->ring->servers[add_pos], key, value);
    *server_id = main->ring->servers_id[add_pos];
}

char *loader_retrieve(load_balancer_t *main, char *key, int *server_id)
{
    *server_id = -1;
    unsigned int key_hash = hash_function_key((void *)key);

    int pos = search_closest(main->ring->hash, key_hash, main->servers);
    char *value = server_retrieve(main->ring->servers[pos], key);
    
    if (value)
        *server_id = main->ring->servers_id[pos]; 
    
    return value;
}

void free_load_balancer(load_balancer_t *main)
{
    int *removed_servers = calloc(main->servers, sizeof(int));
    DIE(!removed_servers, "calloc\n");

    for (int i = 0; i < main->servers; ++i)
        if (!removed_servers[i]) {
            mark_removed_server(removed_servers, main->ring->servers_id[i], main->ring->servers_id, main->servers);
            free_server_memory(main->ring->servers[i]);
        }
    
    free(removed_servers);
    free(main->ring->hash);
    free(main->ring->servers_id);
    free(main->ring->servers);
    free(main->ring);
    free(main);
}
