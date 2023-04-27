/* copyright Carauleanu Valentin Gabriel 311Cab 2023 */
#ifndef AUX_FUNCTIONS_H_
#define AUX_FUNCTIONS_H_
#include "server.h"

/* functia realoca un orice tip de vector: void * */
void resize_array(void **v, int capacity, unsigned int data_size);

/* functia realoca orice tip de vector : void ** */
void resize_double_array(void  ***v, int capacity, unsigned int data_size);

/* functia gaseste cea mai apropiata pozitie a unui 'target' fata de
un element din vector sortat, folosind binarry search */
int search_closest(unsigned int *v, unsigned int target, int size);

/* functia gaseste pozitia pe care trebuie adaugat un element intr-un
vector sortat */
int search_pos(unsigned int *v, int size, unsigned int target);

/* functia va muta elementele din src care au hash-ul mai apropiat de
hash-ul destinatiei */
void rebalance_items(hashtable_t *src, int dest_idx, hashtable_t *dest,
                     ring_t *ring, int size);

/* functia apeleaza rebalance_items, accesand hashtable-ul din cadrul
unui server */
void rebalancing(server_memory_t *src, int dest_idx, server_memory_t *dest,
                 ring_t *ring, int size);

/* functia va dubla capacitatea load_balancere-ului si va realoca vectorii
din hashring, folosind functiie resize_array si resize_double_array*/
void resize_load_balancer(load_balancer_t *main);

/* functia adauga un nou server sau o copie o unui server in hashring
salvand hash-ul si id-ul */
void add_server_ring(ring_t *ring, int *size, int server_id,
                     unsigned int hash_id, server_memory_t *server, int pos);

/* functia sterge din hashring elementul de pozitia pos */
void remove_server_ring(ring_t *ring, int *size, int pos);

/* functia va muta copia fiecare element din src in server-ul corespunzator */
void redistribute_items(hashtable_t *src, load_balancer_t *main);

/* functia acceseaza hashtable ul din cadrul unui server si apeleaza functia
redistribute_items ; redistribute_items -> functia va muta copia fiecare
element din src in server-ul corespunzator */
void redistribution(load_balancer_t *main, server_memory_t *removed);

/* functia este apelata in cadrul eliberarii finale a memoriei si marcheaza
un server a carui memorie a fost eliberata */
void mark_removed_server(int *removed, int id, int *servers_id, int size);

#endif  // AUX_FUNCTIONS_H_
