/*
 * Copyright (C) 2017 Nagravision S.A.
 */

#include "../aes.h"
#include <smmintrin.h>
#include <stdio.h>
#include <string.h>
#include <wmmintrin.h>


static __m128i assist256_1 (__m128i a, __m128i b) {
    __m128i c = _mm_setzero_si128 ();
    b = _mm_shuffle_epi32 (b, _MM_SHUFFLE (3, 3, 3, 3));
    c = _mm_castps_si128 (_mm_shuffle_ps (_mm_castsi128_ps (c), _mm_castsi128_ps (a),
                                          _MM_SHUFFLE (0, 1, 0, 0)));
    a = _mm_xor_si128 (a, c);
    c = _mm_castps_si128 (_mm_shuffle_ps (_mm_castsi128_ps (c), _mm_castsi128_ps (a),
                                          _MM_SHUFFLE (2, 0, 3, 0)));
    a = _mm_xor_si128 (a, c);
    a = _mm_xor_si128 (a, b);
    return a;
}

static __m128i assist256_2 (__m128i a, __m128i b) {
    __m128i c = _mm_setzero_si128 ();
    b = _mm_shuffle_epi32 (b, _MM_SHUFFLE (2, 2, 2, 2));
    c = _mm_castps_si128 (_mm_shuffle_ps (_mm_castsi128_ps (c), _mm_castsi128_ps (a),
                                          _MM_SHUFFLE (0, 1, 0, 0)));
    a = _mm_xor_si128 (a, c);
    c = _mm_castps_si128 (_mm_shuffle_ps (_mm_castsi128_ps (c), _mm_castsi128_ps (a),
                                          _MM_SHUFFLE (2, 0, 3, 0)));
    a = _mm_xor_si128 (a, c);
    a = _mm_xor_si128 (a, b);
    return a;
}

static void expand256 (__m128i *rkeys, const __m128i *key) {
    __m128i s, t;

    rkeys[0] = s = _mm_loadu_si128 (&key[0]);
    rkeys[1] = t = _mm_loadu_si128 (&key[1]);
    rkeys[2] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x01));
    rkeys[3] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x01));
    rkeys[4] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x02));
    rkeys[5] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x02));
    rkeys[6] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x04));
    rkeys[7] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x04));
    rkeys[8] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x08));
    rkeys[9] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x08));
    rkeys[10] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x10));
    rkeys[11] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x10));
    rkeys[12] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x20));
    rkeys[13] = t = assist256_2 (t, _mm_aeskeygenassist_si128 (s, 0x20));
    rkeys[14] = s = assist256_1 (s, _mm_aeskeygenassist_si128 (t, 0x40));
}

static __m128i increment_be (__m128i x) {
    __m128i swap;
    swap = _mm_setr_epi8 (15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    x = _mm_shuffle_epi8 (x, swap);
    x = _mm_add_epi64 (x, _mm_set_epi32 (0, 0, 0, 1));
    x = _mm_shuffle_epi8 (x, swap);
    return x;
}

void aesctr256_direct_x4 (uint8_t *out, const __m128i *rkeys, const void *counter, size_t bytes) {
    __m128i s1, s2, s3, s4;
    __m128i ctr, *bo;
    /* bytes will always be a multiple of 16 */
    int blocks = bytes / 16;
    int blocks_parallel = 4 * (blocks / 4);
    int blocks_left = blocks - blocks_parallel;
    int i;

    ctr = _mm_load_si128 ((__m128i *)counter);
    bo = (__m128i *)out;

    for (i = 0; i < blocks_parallel; i += 4) {
        s1 = _mm_xor_si128 (ctr, rkeys[0]);
        ctr = increment_be (ctr);
        s2 = _mm_xor_si128 (ctr, rkeys[0]);
        ctr = increment_be (ctr);
        s3 = _mm_xor_si128 (ctr, rkeys[0]);
        ctr = increment_be (ctr);
        s4 = _mm_xor_si128 (ctr, rkeys[0]);
        ctr = increment_be (ctr);

        s1 = _mm_aesenc_si128 (s1, rkeys[1]);
        s2 = _mm_aesenc_si128 (s2, rkeys[1]);
        s3 = _mm_aesenc_si128 (s3, rkeys[1]);
        s4 = _mm_aesenc_si128 (s4, rkeys[1]);
        s1 = _mm_aesenc_si128 (s1, rkeys[2]);
        s2 = _mm_aesenc_si128 (s2, rkeys[2]);
        s3 = _mm_aesenc_si128 (s3, rkeys[2]);
        s4 = _mm_aesenc_si128 (s4, rkeys[2]);
        s1 = _mm_aesenc_si128 (s1, rkeys[3]);
        s2 = _mm_aesenc_si128 (s2, rkeys[3]);
        s3 = _mm_aesenc_si128 (s3, rkeys[3]);
        s4 = _mm_aesenc_si128 (s4, rkeys[3]);
        s1 = _mm_aesenc_si128 (s1, rkeys[4]);
        s2 = _mm_aesenc_si128 (s2, rkeys[4]);
        s3 = _mm_aesenc_si128 (s3, rkeys[4]);
        s4 = _mm_aesenc_si128 (s4, rkeys[4]);
        s1 = _mm_aesenc_si128 (s1, rkeys[5]);
        s2 = _mm_aesenc_si128 (s2, rkeys[5]);
        s3 = _mm_aesenc_si128 (s3, rkeys[5]);
        s4 = _mm_aesenc_si128 (s4, rkeys[5]);
        s1 = _mm_aesenc_si128 (s1, rkeys[6]);
        s2 = _mm_aesenc_si128 (s2, rkeys[6]);
        s3 = _mm_aesenc_si128 (s3, rkeys[6]);
        s4 = _mm_aesenc_si128 (s4, rkeys[6]);
        s1 = _mm_aesenc_si128 (s1, rkeys[7]);
        s2 = _mm_aesenc_si128 (s2, rkeys[7]);
        s3 = _mm_aesenc_si128 (s3, rkeys[7]);
        s4 = _mm_aesenc_si128 (s4, rkeys[7]);
        s1 = _mm_aesenc_si128 (s1, rkeys[8]);
        s2 = _mm_aesenc_si128 (s2, rkeys[8]);
        s3 = _mm_aesenc_si128 (s3, rkeys[8]);
        s4 = _mm_aesenc_si128 (s4, rkeys[8]);
        s1 = _mm_aesenc_si128 (s1, rkeys[9]);
        s2 = _mm_aesenc_si128 (s2, rkeys[9]);
        s3 = _mm_aesenc_si128 (s3, rkeys[9]);
        s4 = _mm_aesenc_si128 (s4, rkeys[9]);
        s1 = _mm_aesenc_si128 (s1, rkeys[10]);
        s2 = _mm_aesenc_si128 (s2, rkeys[10]);
        s3 = _mm_aesenc_si128 (s3, rkeys[10]);
        s4 = _mm_aesenc_si128 (s4, rkeys[10]);
        s1 = _mm_aesenc_si128 (s1, rkeys[11]);
        s2 = _mm_aesenc_si128 (s2, rkeys[11]);
        s3 = _mm_aesenc_si128 (s3, rkeys[11]);
        s4 = _mm_aesenc_si128 (s4, rkeys[11]);
        s1 = _mm_aesenc_si128 (s1, rkeys[12]);
        s2 = _mm_aesenc_si128 (s2, rkeys[12]);
        s3 = _mm_aesenc_si128 (s3, rkeys[12]);
        s4 = _mm_aesenc_si128 (s4, rkeys[12]);
        s1 = _mm_aesenc_si128 (s1, rkeys[13]);
        s2 = _mm_aesenc_si128 (s2, rkeys[13]);
        s3 = _mm_aesenc_si128 (s3, rkeys[13]);
        s4 = _mm_aesenc_si128 (s4, rkeys[13]);
        s1 = _mm_aesenclast_si128 (s1, rkeys[14]);
        s2 = _mm_aesenclast_si128 (s2, rkeys[14]);
        s3 = _mm_aesenclast_si128 (s3, rkeys[14]);
        s4 = _mm_aesenclast_si128 (s4, rkeys[14]);

        _mm_storeu_si128 (bo + i, s1);
        _mm_storeu_si128 (bo + i + 1, s2);
        _mm_storeu_si128 (bo + i + 2, s3);
        _mm_storeu_si128 (bo + i + 3, s4);
    }
    for (i = 0; i < blocks_left; i++) {
        s1 = _mm_xor_si128 (ctr, rkeys[0]);
        ctr = increment_be (ctr);
        s1 = _mm_aesenc_si128 (s1, rkeys[1]);
        s1 = _mm_aesenc_si128 (s1, rkeys[2]);
        s1 = _mm_aesenc_si128 (s1, rkeys[3]);
        s1 = _mm_aesenc_si128 (s1, rkeys[4]);
        s1 = _mm_aesenc_si128 (s1, rkeys[5]);
        s1 = _mm_aesenc_si128 (s1, rkeys[6]);
        s1 = _mm_aesenc_si128 (s1, rkeys[7]);
        s1 = _mm_aesenc_si128 (s1, rkeys[8]);
        s1 = _mm_aesenc_si128 (s1, rkeys[9]);
        s1 = _mm_aesenc_si128 (s1, rkeys[10]);
        s1 = _mm_aesenc_si128 (s1, rkeys[11]);
        s1 = _mm_aesenc_si128 (s1, rkeys[12]);
        s1 = _mm_aesenc_si128 (s1, rkeys[13]);
        s1 = _mm_aesenclast_si128 (s1, rkeys[14]);
        _mm_storeu_si128 (bo + blocks_parallel + i, s1);
    }
}

int aesctr256_zeroiv (uint8_t *out, const uint8_t *sk, int bytes) {
    uint8_t counter[16] = {0};
    return aesctr256(out, sk, counter, bytes);
}

int aesctr256 (uint8_t *out, const uint8_t *k, const void *counter, int bytes) {
    __m128i rkeys[15];
    expand256 (rkeys, (__m128i *)k);
    aesctr256_direct_x4 (out, rkeys, counter, bytes);
    return 0;
}
