/* copyright Carauleanu Valentin Gabriel 311Cab 2023 */
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "hashtable.h"
#include "load_balancer.h"

server_memory_t *init_server_memory(void)
{
	server_memory_t *server = malloc(sizeof(server_memory_t));
	DIE(!server, "malloc\n");

	server->memory = ht_create(MAX_PROD, hash_function_key,
								compare_function_strings,
								key_val_free_function);

	return server;
}

void server_store(server_memory_t *server, char *key, char *value)
{
	ht_put(server->memory, key, strlen((char *)key) + 1, value,
		   strlen((char *)value) + 1);
}

char *server_retrieve(server_memory_t *server, char *key) {
	char *value = NULL;
	value = (char *)ht_get(server->memory, key);
	return value;
}

void server_remove(server_memory_t *server, char *key) {
	ht_remove_entry(server->memory, key);
}

void free_server_memory(server_memory_t *server) {
	ht_free(server->memory);
	free(server);
}
