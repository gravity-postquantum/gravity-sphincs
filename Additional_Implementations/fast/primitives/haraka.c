/*
The MIT License (MIT)

Copyright (c) 2016 Stefan Kölbl
original Haraka implementations

Copyright (c) 2017 Nagravision S.A.
changes by JP Aumasson, Guillaume Endignoux, 2017: improvements, non-ni versions

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include "haraka.h"

#include "wmmintrin.h"
#include "emmintrin.h"
#include "smmintrin.h"



void haraka256_256(unsigned char *out, const unsigned char *in) {
  __m128i s[2], tmp;
  int i;

  s[0] = LOAD(in);
  s[1] = LOAD(in + 16);

  for (i = 0; i < HARAKA_ROUNDS; ++i) {
      AES_MIX2(s[0], s[1], 4*i);
  }

  s[0] = _mm_xor_si128(s[0], LOAD(in));
  s[1] = _mm_xor_si128(s[1], LOAD(in + 16));

  STORE(out, s[0]);
  STORE(out + 16, s[1]);
}

void haraka256_256_chain(unsigned char *out, const unsigned char *in, int chainlen) {
  __m128i s[2], tmp;
  __m128i t[2];
  int i, j;

  s[0] = t[0] = LOAD(in);
  s[1] = t[1] = LOAD(in + 16);

  for (j = 0; j < chainlen; ++j) {
      for (i = 0; i < HARAKA_ROUNDS; ++i) {
          AES_MIX2(s[0], s[1], 4*i);
      }

      s[0] = t[0] = _mm_xor_si128(s[0], t[0]);
      s[1] = t[1] = _mm_xor_si128(s[1], t[1]);
  }

  STORE(out, s[0]);
  STORE(out + 16, s[1]);
}

void haraka256_256_4x(unsigned char *out, const unsigned char *in) {
  __m128i s[4][2], tmp;
  int i;

  s[0][0] = LOAD(in);
  s[0][1] = LOAD(in + 16);
  s[1][0] = LOAD(in + 32);
  s[1][1] = LOAD(in + 48);
  s[2][0] = LOAD(in + 64);
  s[2][1] = LOAD(in + 80);
  s[3][0] = LOAD(in + 96);
  s[3][1] = LOAD(in + 112);

  /* Rounds */
  for (i = 0; i < HARAKA_ROUNDS; ++i) {
      AES_MIX2_4x(s[0], s[1], s[2], s[3], 4*i);
  }

  /* Feed Forward */
  s[0][0] = _mm_xor_si128(s[0][0], LOAD(in));
  s[0][1] = _mm_xor_si128(s[0][1], LOAD(in + 16));
  s[1][0] = _mm_xor_si128(s[1][0], LOAD(in + 32));
  s[1][1] = _mm_xor_si128(s[1][1], LOAD(in + 48));
  s[2][0] = _mm_xor_si128(s[2][0], LOAD(in + 64));
  s[2][1] = _mm_xor_si128(s[2][1], LOAD(in + 80));
  s[3][0] = _mm_xor_si128(s[3][0], LOAD(in + 96));
  s[3][1] = _mm_xor_si128(s[3][1], LOAD(in + 112));

  STORE(out, s[0][0]);
  STORE(out + 16, s[0][1]);
  STORE(out + 32, s[1][0]);
  STORE(out + 48, s[1][1]);
  STORE(out + 64, s[2][0]);
  STORE(out + 80, s[2][1]);
  STORE(out + 96, s[3][0]);
  STORE(out + 112, s[3][1]);
}

void haraka256_256_4x_chain(unsigned char *out, const unsigned char *in, int chainlen) {
  __m128i s[4][2], tmp;
  __m128i t[4][2];
  int i, j;

  s[0][0] = t[0][0] = LOAD(in);
  s[0][1] = t[0][1] = LOAD(in + 16);
  s[1][0] = t[1][0] = LOAD(in + 32);
  s[1][1] = t[1][1] = LOAD(in + 48);
  s[2][0] = t[2][0] = LOAD(in + 64);
  s[2][1] = t[2][1] = LOAD(in + 80);
  s[3][0] = t[3][0] = LOAD(in + 96);
  s[3][1] = t[3][1] = LOAD(in + 112);

  for (j = 0; j < chainlen; ++j) {
      /* Rounds */
      for (i = 0; i < HARAKA_ROUNDS; ++i) {
          AES_MIX2_4x(s[0], s[1], s[2], s[3], 4*i);
      }

      /* Feed Forward */
      s[0][0] = t[0][0] = _mm_xor_si128(s[0][0], t[0][0]);
      s[0][1] = t[0][1] = _mm_xor_si128(s[0][1], t[0][1]);
      s[1][0] = t[1][0] = _mm_xor_si128(s[1][0], t[1][0]);
      s[1][1] = t[1][1] = _mm_xor_si128(s[1][1], t[1][1]);
      s[2][0] = t[2][0] = _mm_xor_si128(s[2][0], t[2][0]);
      s[2][1] = t[2][1] = _mm_xor_si128(s[2][1], t[2][1]);
      s[3][0] = t[3][0] = _mm_xor_si128(s[3][0], t[3][0]);
      s[3][1] = t[3][1] = _mm_xor_si128(s[3][1], t[3][1]);
  }

  STORE(out, s[0][0]);
  STORE(out + 16, s[0][1]);
  STORE(out + 32, s[1][0]);
  STORE(out + 48, s[1][1]);
  STORE(out + 64, s[2][0]);
  STORE(out + 80, s[2][1]);
  STORE(out + 96, s[3][0]);
  STORE(out + 112, s[3][1]);
}

void haraka256_256_8x(unsigned char *out, const unsigned char *in) {
  /* This is faster on Skylake, the code below is faster on Haswell. */
  haraka256_256_4x(out, in);
  haraka256_256_4x(out + 128, in + 128);
  return;
  /*
  __m128i s[8][2], tmp;

  int i;

  s[0][0] = LOAD(in);
  s[0][1] = LOAD(in + 16);
  s[1][0] = LOAD(in + 32);
  s[1][1] = LOAD(in + 48);
  s[2][0] = LOAD(in + 64);
  s[2][1] = LOAD(in + 80);
  s[3][0] = LOAD(in + 96);
  s[3][1] = LOAD(in + 112);
  s[4][0] = LOAD(in + 128);
  s[4][1] = LOAD(in + 144);
  s[5][0] = LOAD(in + 160);
  s[5][1] = LOAD(in + 176);
  s[6][0] = LOAD(in + 192);
  s[6][1] = LOAD(in + 208);
  s[7][0] = LOAD(in + 224);
  s[7][1] = LOAD(in + 240);

  // Rounds
  for (i = 0; i < HARAKA_ROUNDS; ++i) {
    AES_MIX2_8x(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], 4*i);
  }

  // Feed Forward
  s[0][0] = _mm_xor_si128(s[0][0], LOAD(in));
  s[0][1] = _mm_xor_si128(s[0][1], LOAD(in + 16));
  s[1][0] = _mm_xor_si128(s[1][0], LOAD(in + 32));
  s[1][1] = _mm_xor_si128(s[1][1], LOAD(in + 48));
  s[2][0] = _mm_xor_si128(s[2][0], LOAD(in + 64));
  s[2][1] = _mm_xor_si128(s[2][1], LOAD(in + 80));
  s[3][0] = _mm_xor_si128(s[3][0], LOAD(in + 96));
  s[3][1] = _mm_xor_si128(s[3][1], LOAD(in + 112));
  s[4][0] = _mm_xor_si128(s[4][0], LOAD(in + 128));
  s[4][1] = _mm_xor_si128(s[4][1], LOAD(in + 144));
  s[5][0] = _mm_xor_si128(s[5][0], LOAD(in + 160));
  s[5][1] = _mm_xor_si128(s[5][1], LOAD(in + 176));
  s[6][0] = _mm_xor_si128(s[6][0], LOAD(in + 192));
  s[6][1] = _mm_xor_si128(s[6][1], LOAD(in + 208));
  s[7][0] = _mm_xor_si128(s[7][0], LOAD(in + 224));
  s[7][1] = _mm_xor_si128(s[7][1], LOAD(in + 240));

  STORE(out, s[0][0]);
  STORE(out + 16, s[0][1]);
  STORE(out + 32, s[1][0]);
  STORE(out + 48, s[1][1]);
  STORE(out + 64, s[2][0]);
  STORE(out + 80, s[2][1]);
  STORE(out + 96, s[3][0]);
  STORE(out + 112, s[3][1]);
  STORE(out + 128, s[4][0]);
  STORE(out + 144, s[4][1]);
  STORE(out + 160, s[5][0]);
  STORE(out + 176, s[5][1]);
  STORE(out + 192, s[6][0]);
  STORE(out + 208, s[6][1]);
  STORE(out + 224, s[7][0]);
  STORE(out + 240, s[7][1]);
  */
}

void haraka512_256(unsigned char *out, const unsigned char *in) {
  __m128i s[4], tmp;
  int i;

  s[0] = LOAD(in);
  s[1] = LOAD(in + 16);
  s[2] = LOAD(in + 32);
  s[3] = LOAD(in + 48);

  for (i = 0; i < HARAKA_ROUNDS; ++i) {
      AES_MIX4(s[0], s[1], s[2], s[3], 8*i);
  }

  s[0] = _mm_xor_si128(s[0], LOAD(in));
  s[1] = _mm_xor_si128(s[1], LOAD(in + 16));
  s[2] = _mm_xor_si128(s[2], LOAD(in + 32));
  s[3] = _mm_xor_si128(s[3], LOAD(in + 48));

  _mm_storel_epi64 ((__m128i *)(out + 0),
  _mm_shuffle_epi32 (s[0], _MM_SHUFFLE (3, 2, 3, 2)));
_mm_storel_epi64 ((__m128i *)(out + 8),
  _mm_shuffle_epi32 (s[1], _MM_SHUFFLE (3, 2, 3, 2)));
_mm_storel_epi64 ((__m128i *)(out + 16), s[2]);
_mm_storel_epi64 ((__m128i *)(out + 24), s[3]);
}

void haraka512_256_4x(unsigned char *out, const unsigned char *in) {
  __m128i s[4][4], tmp;
  int i, offset;

  s[0][0] = LOAD(in);
  s[0][1] = LOAD(in + 16);
  s[0][2] = LOAD(in + 32);
  s[0][3] = LOAD(in + 48);
  s[1][0] = LOAD(in + 64);
  s[1][1] = LOAD(in + 80);
  s[1][2] = LOAD(in + 96);
  s[1][3] = LOAD(in + 112);
  s[2][0] = LOAD(in + 128);
  s[2][1] = LOAD(in + 144);
  s[2][2] = LOAD(in + 160);
  s[2][3] = LOAD(in + 176);
  s[3][0] = LOAD(in + 192);
  s[3][1] = LOAD(in + 208);
  s[3][2] = LOAD(in + 224);
  s[3][3] = LOAD(in + 240);

  for (i = 0; i < HARAKA_ROUNDS; ++i) {
      AES_MIX4_4x(s[0], s[1], s[2], s[3], 8*i);
  }

  s[0][0] = _mm_xor_si128(s[0][0], LOAD(in));
  s[0][1] = _mm_xor_si128(s[0][1], LOAD(in + 16));
  s[0][2] = _mm_xor_si128(s[0][2], LOAD(in + 32));
  s[0][3] = _mm_xor_si128(s[0][3], LOAD(in + 48));
  s[1][0] = _mm_xor_si128(s[1][0], LOAD(in + 64));
  s[1][1] = _mm_xor_si128(s[1][1], LOAD(in + 80));
  s[1][2] = _mm_xor_si128(s[1][2], LOAD(in + 96));
  s[1][3] = _mm_xor_si128(s[1][3], LOAD(in + 112));
  s[2][0] = _mm_xor_si128(s[2][0], LOAD(in + 128));
  s[2][1] = _mm_xor_si128(s[2][1], LOAD(in + 144));
  s[2][2] = _mm_xor_si128(s[2][2], LOAD(in + 160));
  s[2][3] = _mm_xor_si128(s[2][3], LOAD(in + 176));
  s[3][0] = _mm_xor_si128(s[3][0], LOAD(in + 192));
  s[3][1] = _mm_xor_si128(s[3][1], LOAD(in + 208));
  s[3][2] = _mm_xor_si128(s[3][2], LOAD(in + 224));
  s[3][3] = _mm_xor_si128(s[3][3], LOAD(in + 240));

  for (i = 0; i < 4; i++) {
    offset = i * 32;
    _mm_storel_epi64 ((__m128i *)(out + 0 + offset),
                      _mm_shuffle_epi32 (s[i][0], _MM_SHUFFLE (3, 2, 3, 2)));
    _mm_storel_epi64 ((__m128i *)(out + 8 + offset),
                      _mm_shuffle_epi32 (s[i][1], _MM_SHUFFLE (3, 2, 3, 2)));
    _mm_storel_epi64 ((__m128i *)(out + 16 + offset), s[i][2]);
    _mm_storel_epi64 ((__m128i *)(out + 24 + offset), s[i][3]);
}
}

void haraka512_256_8x(unsigned char *out, const unsigned char *in) {
  /* This is faster on Skylake, the code below is faster on Haswell. */
  haraka512_256_4x(out, in);
  haraka512_256_4x(out + 128, in + 256);

  /*
  __m128i s[8][4], tmp;
  __m128i MSB64 = _mm_set_epi32(0xFFFFFFFF,0xFFFFFFFF,0,0);
  int i;

  s[0][0] = LOAD(in);
  s[0][1] = LOAD(in + 16);
  s[0][2] = LOAD(in + 32);
  s[0][3] = LOAD(in + 48);
  s[1][0] = LOAD(in + 64);
  s[1][1] = LOAD(in + 80);
  s[1][2] = LOAD(in + 96);
  s[1][3] = LOAD(in + 112);
  s[2][0] = LOAD(in + 128);
  s[2][1] = LOAD(in + 144);
  s[2][2] = LOAD(in + 160);
  s[2][3] = LOAD(in + 176);
  s[3][0] = LOAD(in + 192);
  s[3][1] = LOAD(in + 208);
  s[3][2] = LOAD(in + 224);
  s[3][3] = LOAD(in + 240);
  s[4][0] = LOAD(in + 256);
  s[4][1] = LOAD(in + 272);
  s[4][2] = LOAD(in + 288);
  s[4][3] = LOAD(in + 304);
  s[5][0] = LOAD(in + 320);
  s[5][1] = LOAD(in + 336);
  s[5][2] = LOAD(in + 352);
  s[5][3] = LOAD(in + 368);
  s[6][0] = LOAD(in + 384);
  s[6][1] = LOAD(in + 400);
  s[6][2] = LOAD(in + 416);
  s[6][3] = LOAD(in + 432);
  s[7][0] = LOAD(in + 448);
  s[7][1] = LOAD(in + 464);
  s[7][2] = LOAD(in + 480);
  s[7][3] = LOAD(in + 496);

  for (i = 0; i < HARAKA_ROUNDS; ++i) {
    AES_MIX4_8x(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], 8*i);
  }


  s[0][0] = _mm_xor_si128(s[0][0], LOAD(in));
  s[0][1] = _mm_xor_si128(s[0][1], LOAD(in + 16));
  s[0][2] = _mm_xor_si128(s[0][2], LOAD(in + 32));
  s[0][3] = _mm_xor_si128(s[0][3], LOAD(in + 48));
  s[1][0] = _mm_xor_si128(s[1][0], LOAD(in + 64));
  s[1][1] = _mm_xor_si128(s[1][1], LOAD(in + 80));
  s[1][2] = _mm_xor_si128(s[1][2], LOAD(in + 96));
  s[1][3] = _mm_xor_si128(s[1][3], LOAD(in + 112));
  s[2][0] = _mm_xor_si128(s[2][0], LOAD(in + 128));
  s[2][1] = _mm_xor_si128(s[2][1], LOAD(in + 144));
  s[2][2] = _mm_xor_si128(s[2][2], LOAD(in + 160));
  s[2][3] = _mm_xor_si128(s[2][3], LOAD(in + 176));
  s[3][0] = _mm_xor_si128(s[3][0], LOAD(in + 192));
  s[3][1] = _mm_xor_si128(s[3][1], LOAD(in + 208));
  s[3][2] = _mm_xor_si128(s[3][2], LOAD(in + 224));
  s[3][3] = _mm_xor_si128(s[3][3], LOAD(in + 240));
  s[4][0] = _mm_xor_si128(s[4][0], LOAD(in + 256));
  s[4][1] = _mm_xor_si128(s[4][1], LOAD(in + 272));
  s[4][2] = _mm_xor_si128(s[4][2], LOAD(in + 288));
  s[4][3] = _mm_xor_si128(s[4][3], LOAD(in + 304));
  s[5][0] = _mm_xor_si128(s[5][0], LOAD(in + 320));
  s[5][1] = _mm_xor_si128(s[5][1], LOAD(in + 336));
  s[5][2] = _mm_xor_si128(s[5][2], LOAD(in + 352));
  s[5][3] = _mm_xor_si128(s[5][3], LOAD(in + 368));
  s[6][0] = _mm_xor_si128(s[6][0], LOAD(in + 384));
  s[6][1] = _mm_xor_si128(s[6][1], LOAD(in + 400));
  s[6][2] = _mm_xor_si128(s[6][2], LOAD(in + 416));
  s[6][3] = _mm_xor_si128(s[6][3], LOAD(in + 432));
  s[7][0] = _mm_xor_si128(s[7][0], LOAD(in + 448));
  s[7][1] = _mm_xor_si128(s[7][1], LOAD(in + 464));
  s[7][2] = _mm_xor_si128(s[7][2], LOAD(in + 480));
  s[7][3] = _mm_xor_si128(s[7][3], LOAD(in + 496));

  TRUNCSTORE(out, s[0][0], s[0][1], s[0][2], s[0][3]);
  TRUNCSTORE(out + 32, s[1][0], s[1][1], s[1][2], s[1][3]);
  TRUNCSTORE(out + 64, s[2][0], s[2][1], s[2][2], s[2][3]);
  TRUNCSTORE(out + 96, s[3][0], s[3][1], s[3][2], s[3][3]);
  TRUNCSTORE(out + 128, s[4][0], s[4][1], s[4][2], s[4][3]);
  TRUNCSTORE(out + 160, s[5][0], s[5][1], s[5][2], s[5][3]);
  TRUNCSTORE(out + 192, s[6][0], s[6][1], s[6][2], s[6][3]);
  TRUNCSTORE(out + 224, s[7][0], s[7][1], s[7][2], s[7][3]);
  */
}

