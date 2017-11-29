/*
 * Copyright (C) 2017 Nagravision S.A.
 */

#include "gravity.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include "debug.h"
#endif

int gravity_gensk (struct gravity_sk *sk) {
    int n = GRAVITY_ccc;
    struct hash *src;
    struct hash *dst = sk->cache;
    struct merkle_pk mpk;
    struct address address;
    int i;
    int res;

    /* Create sub Merkle trees */
    address.layer = 0;
    for (i = 0; i < n; ++i) {
        address.index = i * MERKLE_hhh;
        res = merkle_genpk (&sk->seed, &address, &mpk);
        if (res != GRAVITY_OK) return res;

        hashcpy (&dst[i], &mpk.k);

#ifdef DEBUG
        PINT ("gravity_gensk", i);
        PBYTES ("gravity_gensk: mpk ", (uint8_t *)&mpk.k, HASH_SIZE);
#endif
    }

    /* Cache layers of Merkle tree */
    for (i = 0; i < GRAVITY_c; ++i) {
        src = dst;
        dst += n;
        n >>= 1;

        hash_compress_pairs (dst, src, n);
    }

    return GRAVITY_OK;
}

int gravity_genpk (const struct gravity_sk *sk, struct gravity_pk *pk) {
    hashcpy (&pk->k, &sk->cache[2 * GRAVITY_ccc - 2]);

    return GRAVITY_OK;
}

int gravity_sign (const struct gravity_sk *sk, struct gravity_sign *sign, const struct hash *msg) {
    struct hash buf[2];
    struct address address;
    struct pors_subset subset;
    struct pors_sk *psk;
    struct porst_pk ppk;
    struct merkle_pk mpk;
    struct hash h;
    int layer;
    int res;
#if GRAVITY_c > 0
    int offset;
    int n = GRAVITY_ccc;
    int i, sibling;
#endif

    /* Generate "randomness" from message and secret salt */
    hashcpy (&buf[0], &sk->salt);
    hashcpy (&buf[1], msg);
    hash_2N_to_N (&sign->rand, buf);

#ifdef DEBUG
    PBYTES ("gravity_sign: H(salt, m)", (uint8_t *)&sign->rand.h, HASH_SIZE);
#endif

    /* Generate address and PORST indices */
    pors_randsubset (&sign->rand, msg, &address.index, &subset);

#ifdef DEBUG
    PINTS ("gravity_sign: subset", (int *)&subset, PORS_k);
#endif

    /* PORST */
    psk = malloc (sizeof (struct pors_sk));
    if (psk == NULL) return GRAVITY_ERR_ALLOC;

    address.layer = GRAVITY_d;
    pors_gensk (&sk->seed, &address, psk);

#ifdef DEBUG
    PBYTES ("gravity_sign: PORS sk[0]", (uint8_t *)psk->k[0].h, HASH_SIZE);
    PBYTES ("gravity_sign: PORS sk[1]", (uint8_t *)psk->k[1].h, HASH_SIZE);
    PBYTES ("gravity_sign: PORS sk[2]", (uint8_t *)psk->k[2].h, HASH_SIZE);
#endif

    res = octoporst_sign (psk, &sign->op_sign, &ppk, &subset);
    /* TODO: wipe key */
    free (psk);

#ifdef DEBUG
    PINTS ("gravity_sign: octopus size", &sign->op_sign.octolen, 1);
    PBYTES ("gravity_sign: PORS sig[0]", (uint8_t *)&sign->op_sign.s.s[0].h, HASH_SIZE);
    PBYTES ("gravity_sign: PORS sig[1]", (uint8_t *)&sign->op_sign.s.s[1].h, HASH_SIZE);
    PBYTES ("gravity_sign: PORS sig[2]", (uint8_t *)&sign->op_sign.s.s[2].h, HASH_SIZE);
#endif

    if (res != GRAVITY_OK) return res;

    /* Store PORST pubkey into h */
    hashcpy (&h, &ppk.k);

#ifdef DEBUG
    PBYTES ("gravity_sign: PORST pubkey", (uint8_t *)&ppk.k.h, HASH_SIZE);
#endif

    /* Hyper tree */
    for (layer = 0; layer < GRAVITY_d; ++layer) {
        /* Sign h with Merkle tree and obtain public key */
        --address.layer;
        res = merkle_sign (&sk->seed, &address, &sign->merkle[layer], &h, &mpk);
        if (res != GRAVITY_OK) return res;
        hashcpy (&h, &mpk.k);
#ifdef DEBUG
        PINT ("gravity_sign: layer", layer);
        PBYTES ("gravity_sign: Merkle pubkey", (uint8_t *)&mpk.k.h, HASH_SIZE);
#endif

        /* Reduce address for next layer */
        address.index >>= MERKLE_h;
    }

#if GRAVITY_c > 0
    /* Cached Merkle tree */
    offset = 0;
    for (i = 0; i < GRAVITY_c; ++i) {
        sibling = address.index ^ 1;
        hashcpy (&sign->auth[i], &sk->cache[offset + sibling]);

#ifdef DEBUG
        PINT ("gravity_sign: cache layer", i);
        PBYTES ("gravity_sign: cached hash", (uint8_t *)&sign->auth[i].h, HASH_SIZE);
#endif

        address.index >>= 1;
        offset += n;
        n >>= 1;
    }
#endif

    return GRAVITY_OK;
}

int gravity_verify (const struct gravity_pk *pk,
                    const struct gravity_sign *sign,
                    const struct hash *msg) {
    struct address address;
    struct pors_subset subset;
    struct porst_pk ppk;
    struct merkle_pk mpk;
    struct hash h;
    int layer;
    int res;

    /* Generate address and PORST indices */
    pors_randsubset (&sign->rand, msg, &address.index, &subset);

#ifdef DEBUG
    PINTS ("gravity_verify: subset", (int *)&subset, PORS_k);
#endif

    /* PORST */
    res = octoporst_extract (&ppk, &sign->op_sign, &subset);
    if (res != GRAVITY_OK) return res;

#ifdef DEBUG
    PBYTES ("gravity_sign: PORST pk", (uint8_t *)ppk.k.h, HASH_SIZE);
#endif

    /* Store PORST pubkey into h */
    hashcpy (&h, &ppk.k);

    /* Hyper tree */
    address.layer = GRAVITY_d;
    for (layer = 0; layer < GRAVITY_d; ++layer) {
        /* Obtain Merkle tree root */
        --address.layer;
        merkle_extract (&mpk, &address, &sign->merkle[layer], &h);
        hashcpy (&h, &mpk.k);

#ifdef DEBUG
        PINT ("gravity_verify: layer", layer);
        PBYTES ("gravity_verify: Merkle pubkey", (uint8_t *)&mpk.k.h, HASH_SIZE);
#endif

        /* Reduce address for next layer */
        address.index >>= MERKLE_h;
    }

#if GRAVITY_c > 0
    /* Cached Merkle tree */
    merkle_compress_auth (&h, address.index, sign->auth, GRAVITY_c);
#endif

#ifdef DEBUG
    PBYTES ("gravity_verify: Merkle root", (uint8_t *)&h.h, HASH_SIZE);
#endif

    if (hashcmp (&h, &pk->k)) return GRAVITY_ERR_VERIF;

    return GRAVITY_OK;
}

int gravity_loadsign (struct gravity_sign *sign, const uint8_t *_sign, size_t _len) {
    size_t baselen = HASH_SIZE + GRAVITY_d * sizeof (struct merkle_sign)
#if GRAVITY_c > 0
                     + GRAVITY_c * HASH_SIZE
#endif
    ;

    if (_len < baselen) return GRAVITY_ERR_VERIF;
    _len -= baselen;

    memcpy (&sign->rand, _sign, HASH_SIZE);
    _sign += HASH_SIZE;

    if (octoporst_loadsign (&sign->op_sign, _sign, _len) != GRAVITY_OK)
        return GRAVITY_ERR_VERIF;
    _sign += _len;

    memcpy (sign->merkle, _sign, GRAVITY_d * sizeof (struct merkle_sign));

#if GRAVITY_c > 0
    _sign += GRAVITY_d * sizeof (struct merkle_sign);
    memcpy (sign->auth, _sign, GRAVITY_c * HASH_SIZE);
#endif

    return GRAVITY_OK;
}

int gravity_signcmp (const struct gravity_sign *sign1, const struct gravity_sign *sign2) {
    if (hashcmp (&sign1->rand, &sign2->rand)) return 1;
    if (octoporst_signcmp (&sign1->op_sign, &sign2->op_sign)) return 1;
    if (memcmp (&sign1->merkle, &sign2->merkle, GRAVITY_d * sizeof (struct merkle_sign)))
        return 1;
#if GRAVITY_c > 0
    if (hashcmpN (sign1->auth, sign2->auth, GRAVITY_c)) return 1;
#endif

    return 0;
}
