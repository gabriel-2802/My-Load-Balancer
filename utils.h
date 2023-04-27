/* Copyright 2023 <> */
#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 15
#define COPIES 3
#define THOUSANDS 100000

#define REQUEST_LENGTH 1024
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

/* useful macro for handling error codes */
#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

#endif /* UTILS_H_ */
