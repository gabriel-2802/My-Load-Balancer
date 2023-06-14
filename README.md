## Load Balancer - Carauleanu Valentin Gabriel 311CA

A doua mea tema la SDA.

### Descriere:

* Programul prezinta urmatoarele biblioteci:
    * ll_lists.h -> implementara liste simplu inlantuite necirculare
    * hashtable.h -> implementarea hashtable 
    * aux_functions.h -> functii auxiliare programului principal
    * servers.h -> implementarea server
    * load_balancer.h -> implementarea load_balancer, functiile principale ale programului

* Functionarea programului se bazeaza pe structura ring_t, care contine:
    * un vector unsigned int hash*, cu toate hash-urile serverelor
    * un vector int server_id, cu toate id-urile serverelor
    * un vector server_memory_t ** servers servere, ce contine pointeri la servere

* Pentru a salva memorie, 'copiile' unui server vor fi doar reaparitii ale aceluiasi server pe hashring,
    mai precis 3 elemente din vectorul servers vor 'pointa' spre aceeasi structura server_memory_t

* Prin urmare, functia loader_add_server va crea un singur server si un pointer spre acel server care va fi adaugat
    de 3 ori in hashring, conform regulilor prestabilite

* La adaugarea unui nou server, se verifica daca elementele din serverul succesor din hashring au hash-ul mai apropiat
    de noul server. In caz afirmativ, acestea sunt copiate in noul server.

* La eliminarea unui server X(functia loader_remove_server), se vor elimina poztiile cu server-ul X din hashring
    (din cei 3 vectori din structura ring_t), iar toate elementele din X vor fi redistribuite catre serverele
    cu hash-ul cel mai apropiat.

* Loader_store si loader_retrive vor determina serverul cu hash-ul cel mai apropiat de hash-ul cheii si vor
    adauga cheia si valoarea / vor returna valoarea din server.

* Implementarea unui server are la baza structura de hashtable. Astfel, fiecare server ca contine un pointer
    la un hashtable.

### Comentarii asupra temei:

* Din punctul meu de vedere, tema a fost interesanta, deoarece am invatat cum functioneaza un sistem de servere.
* Enuntul a fost concis, iar exemplul powerpoint a explicat foarte bine ceea ce avem de facut. Overall, un enunt
    mult mai clar decat tema 1.
