/*
 * Copyright (C) 2017 Nagravision S.A.
 */
#include "gravity.h"
#include "randombytes.h"

#ifdef DEBUG
#include "debug.h"
#endif

int crypto_sign_keypair (unsigned char *pk, unsigned char *sk) {

    struct gravity_sk sk_str;
    struct gravity_pk pk_str;

    if (!pk || !sk) return -1;

    randombytes (sk_str.seed.h, HASH_SIZE);
    randombytes (sk_str.salt.h, HASH_SIZE);

#ifdef DEBUG
    PBYTES ("crypto_sign_keypair: sk seed", (uint8_t *)(sk_str.seed.h), HASH_SIZE);
    PBYTES ("crypto_sign_keypair: sk salt", (uint8_t *)(sk_str.salt.h), HASH_SIZE);
#endif

    gravity_gensk (&sk_str);

    memcpy (sk, (void *)&sk_str, sizeof (struct gravity_sk));

#ifdef DEBUG
    PBYTES ("crypto_sign_keypair: sk", (uint8_t *)sk, sizeof (struct gravity_sk));
#endif

    gravity_genpk (&sk_str, &pk_str);

    memcpy (pk, pk_str.k.h, HASH_SIZE);

#ifdef DEBUG
    PBYTES ("crypto_sign_keypair: pk", (uint8_t *)pk, sizeof (struct gravity_pk));
#endif

    return 0;
}

int crypto_sign (unsigned char *sm,
                 unsigned long long *smlen,
                 const unsigned char *m,
                 unsigned long long mlen,
                 const unsigned char *sk) {

    struct gravity_sk sk_str;
    struct hash msg;
    struct gravity_sign sig;
    int ret;

    if (!sm || !smlen || !m || !sk) return -1;

#ifdef DEBUG
    PINT ("crypto_sign: mlen", mlen);
    PBYTES ("crypto_sign: m", (uint8_t *)m, mlen);
    PBYTES ("crypto_sign: sk seed", (uint8_t *)sk, HASH_SIZE);
    PBYTES ("crypto_sign: sk salt", (uint8_t *)(sk + HASH_SIZE), HASH_SIZE);
#endif

    hash_to_N (&msg, m, mlen);

#ifdef DEBUG
    PBYTES ("crypto_sign: H(m)", (uint8_t *)msg.h, HASH_SIZE);
#endif

    memcpy ((void *)&sk_str, sk, sizeof (struct gravity_sk));

    memset ((uint8_t *)(&sig), 0, sizeof (struct gravity_sign));

    ret = gravity_sign (&sk_str, &sig, &msg);

    if (ret != GRAVITY_OK) return ret;

    memcpy (sm + mlen, (uint8_t *)(&sig), sizeof (struct gravity_sign));

    memmove (sm, m, mlen);
    *smlen = mlen + sizeof (struct gravity_sign);

#ifdef DEBUG
    PINT ("crypto_sign: smlen", *smlen);
#endif

    return 0;
}

int crypto_sign_open (unsigned char *m,
                      unsigned long long *mlen,
                      const unsigned char *sm,
                      unsigned long long smlen,
                      const unsigned char *pk) {

    struct gravity_pk pk_str;
    struct hash msg;
    struct gravity_sign sig;

    if (!m || !mlen || !sm || !pk) return -1;

    if (smlen < sizeof (struct gravity_sign)) return -2;

#ifdef DEBUG
    PINT ("crypto_sign_open: smlen", smlen);
    PBYTES ("crypto_sign_open: sm", (uint8_t *)sm, smlen);
    PBYTES ("crypto_sign_open: pk", (uint8_t *)pk, sizeof (struct gravity_pk));
#endif

    *mlen = smlen - sizeof (struct gravity_sign);

#ifdef DEBUG
    PINT ("crypto_sign_open: mlen", *mlen);
#endif

    memcpy ((void *)(&pk_str), pk, HASH_SIZE);

    memcpy ((void *)&sig, sm + *mlen, sizeof (struct gravity_sign));

    memcpy (m, sm, *mlen);

    hash_to_N (&msg, m, *mlen);

#ifdef DEBUG
    PBYTES ("crypto_sign: H(m)", (uint8_t *)msg.h, HASH_SIZE);
#endif

    return gravity_verify (&pk_str, &sig, &msg);
}
