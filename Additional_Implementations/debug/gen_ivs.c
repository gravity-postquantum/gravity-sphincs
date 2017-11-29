/*
 * Copyright (C) 2017 Nagravision S.A.
 */

#include "gravity.h"
#include "sign.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N HASH_SIZE
#define SKLEN sizeof (struct gravity_sk)
#define PKLEN sizeof (struct gravity_pk)
#define SIGLEN sizeof (struct gravity_sign)

/* sign CLI-provided message, default message otherwise */
int main (int ac, char **av) {
    uint8_t pk[PKLEN];
    uint8_t sk[SKLEN];
    struct gravity_pk pk_str;
    struct gravity_sk sk_str;
    uint8_t *m = NULL;
    int i;
    uint8_t *sm = NULL;
    unsigned long long mlen;
    unsigned long long smlen;
    int ret = 0;

#define CLEANUP                                                                \
    memset (sk, 0x00, SKLEN);                                                  \
    memset (pk, 0x00, PKLEN);                                                  \
    memset (sm, 0x00, mlen + SIGLEN);

#define SIGN                                                                   \
    if (crypto_sign (sm, &smlen, m, mlen, sk)) {                               \
        fprintf (stderr, "error: crypto_sign failed\n");                       \
        ret = 1;                                                               \
        goto label_exit_2;                                                     \
    }

#define VERIFY                                                                 \
    if (crypto_sign_open (m, &mlen, sm, smlen, pk)) {                          \
        fprintf (stderr, "error: crypto_sign_open failed\n");                  \
        ret = 1;                                                               \
        goto label_exit_2;                                                     \
    }

#define SK2PK(b)                                                               \
    memset (sk_str.seed.h, (unsigned char)b, HASH_SIZE);                       \
    memset (sk_str.salt.h, (unsigned char)b, HASH_SIZE);                       \
    gravity_gensk (&sk_str);                                                   \
    gravity_genpk (&sk_str, &pk_str);                                          \
    memcpy (sk, (void *)&sk_str, SKLEN);                                       \
    memcpy (pk, pk_str.k.h, HASH_SIZE);

    if (ac == 2) {
        m = (uint8_t *)av[1];
        mlen = strlen (av[1]);
    } else {
        m = malloc (N);
        if (m == NULL) {
            ret = 1;
            goto label_exit_0;
        }
        for (i = 0; i < N; ++i) m[i] = i;
        mlen = N;
    }

    sm = malloc (mlen + SIGLEN);
    if (!sm) {
        ret = 1;
        goto label_exit_1;
    }

    SK2PK (0x00);
    SIGN;
    VERIFY;
    CLEANUP;

    SK2PK (0x01);
    SIGN;
    VERIFY;
    CLEANUP;

    SK2PK (0xff);
    SIGN;
    VERIFY;

label_exit_2:
    free (sm);
label_exit_1:
    if (ac != 2) free (m);
label_exit_0:
    return ret;
}
