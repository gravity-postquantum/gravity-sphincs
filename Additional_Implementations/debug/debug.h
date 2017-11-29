/*
 * Copyright (C) 2017 Nagravision S.A.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PBYTES(s, x, xlen)                                                     \
    do {                                                                       \
        printf ("%s (%llu):\n", s, (unsigned long long)xlen);                  \
        printbytes (x, xlen);                                                  \
        printf ("\n");                                                         \
    } while (0)

#define PINTS(s, x, xlen)                                                      \
    do {                                                                       \
        printf ("%s (%llu):\n", s, (unsigned long long)xlen);                  \
        printints (x, xlen);                                                   \
        printf ("\n");                                                         \
    } while (0)

#define PINT(s, x)                                                             \
    do {                                                                       \
        printf ("%s: %llu\n", s, (unsigned long long)x);                       \
        printf ("\n");                                                         \
    } while (0)

static void printints (int *x, size_t xlen) {
    size_t i;
    for (i = 0; i < xlen; ++i) {
        printf ("%d ", x[i]);
    }
    printf ("\n");
}

static void printbytes (uint8_t *x, size_t xlen) {
    size_t i;
    for (i = 0; i < xlen; ++i) {
        if (i && !(i % 32)) printf ("\n");
        printf ("%02x", x[i]);
    }
    printf ("\n");
}
