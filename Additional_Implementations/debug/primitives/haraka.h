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
#pragma once

#include <stdint.h>

static const uint32_t rc[48 * 4] = {
    0x75817b9d, 0xb2c5fef0, 0xe620c00a, 0x0684704c, 0x2f08f717, 0x640f6ba4,
    0x88f3a06b, 0x8b66b4e1, 0x9f029114, 0xcf029d60, 0x53f28498, 0x3402de2d,
    0xfd5b4f79, 0xbbf3bcaf, 0x2e7b4f08, 0x0ed6eae6, 0xbe397044, 0x79eecd1c,
    0x4872448b, 0xcbcfb0cb, 0x2b8a057b, 0x8d5335ed, 0x6e9032b7, 0x7eeacdee,
    0xda4fef1b, 0xe2412761, 0x5e2e7cd0, 0x67c28f43, 0x1fc70b3b, 0x675ffde2,
    0xafcacc07, 0x2924d9b0, 0xb9d465ee, 0xecdb8fca, 0xe6867fe9, 0xab4d63f1,
    0xad037e33, 0x5b2a404f, 0xd4b7cd64, 0x1c30bf84, 0x8df69800, 0x69028b2e,
    0x941723bf, 0xb2cc0bb9, 0x5c9d2d8a, 0x4aaa9ec8, 0xde6f5572, 0xfa0478a6,
    0x29129fd4, 0x0efa4f2e, 0x6b772a12, 0xdfb49f2b, 0xbb6a12ee, 0x32d611ae,
    0xf449a236, 0x1ea10344, 0x9ca8eca6, 0x5f9600c9, 0x4b050084, 0xaf044988,
    0x27e593ec, 0x78a2c7e3, 0x9d199c4f, 0x21025ed8, 0x82d40173, 0xb9282ecd,
    0xa759c9b7, 0xbf3aaaf8, 0x10307d6b, 0x37f2efd9, 0x6186b017, 0x6260700d,
    0xf6fc9ac6, 0x81c29153, 0x21300443, 0x5aca45c2, 0x36d1943a, 0x2caf92e8,
    0x226b68bb, 0x9223973c, 0xe51071b4, 0x6cbab958, 0x225886eb, 0xd3bf9238,
    0x24e1128d, 0x933dfddd, 0xaef0c677, 0xdb863ce5, 0xcb2212b1, 0x83e48de3,
    0xffeba09c, 0xbb606268, 0xc72bf77d, 0x2db91a4e, 0xe2e4d19c, 0x734bd3dc,
    0x2cb3924e, 0x4b1415c4, 0x61301b43, 0x43bb47c3, 0x16eb6899, 0x03b231dd,
    0xe707eff6, 0xdba775a8, 0x7eca472c, 0x8e5e2302, 0x3c755977, 0x6df3614b,
    0xb88617f9, 0x6d1be5b9, 0xd6de7d77, 0xcda75a17, 0xa946ee5d, 0x9d6c069d,
    0x6ba8e9aa, 0xec6b43f0, 0x3bf327c1, 0xa2531159, 0xf957332b, 0xcb1e6950,
    0x600ed0d9, 0xe4ed0353, 0x00da619c, 0x2cee0c75, 0x63a4a350, 0x80bbbabc,
    0x96e90cab, 0xf0b1a5a1, 0x938dca39, 0xab0dde30, 0x5e962988, 0xae3db102,
    0x2e75b442, 0x8814f3a8, 0xd554a40b, 0x17bb8f38, 0x360a16f6, 0xaeb6b779,
    0x5f427fd7, 0x34bb8a5b, 0xffbaafde, 0x43ce5918, 0xcbe55438, 0x26f65241,
    0x839ec978, 0xa2ca9cf7, 0xb9f3026a, 0x4ce99a54, 0x22901235, 0x40c06e28,
    0x1bdff7be, 0xae51a51a, 0x48a659cf, 0xc173bc0f, 0xba7ed22b, 0xa0c1613c,
    0xe9c59da1, 0x4ad6bdfd, 0x02288288, 0x756acc03, 0x848f2ad2, 0x367e4778,
    0x0de7d31e, 0x2ff37238, 0xb73bd58f, 0xee36b135, 0xcf74be8b, 0x08d95c6a,
    0xa3743e4a, 0x66ae1838, 0xc9d6ee98, 0x5880f434, 0x9a9369bd, 0xd0fdf4c7,
    0xaefabd99, 0x593023f0, 0x6f1ecb2a, 0xa5cc637b, 0xeb606e6f, 0x329ae3d1,
    0xcb7594ab, 0xa4dc93d6, 0x49e01594, 0xe00207eb, 0x65208ef8, 0x942366a6,
    0xf751c880, 0x1caa0c4f, 0xe3e67e4a, 0xbd03239f, 0xdb2dc1dd, 0x02f7f57f,
};

#define HARAKA_ROUNDS 6

#define LOAD(src) _mm_load_si128((__m128i *)(src))
#define STORE(dest,src) _mm_storeu_si128((__m128i *)(dest),src)

#define AES2(s0, s1, rci) \
  s0 = _mm_aesenc_si128 (s0, LOAD (&rc[4 * (rci + 0)]));                     \
  s1 = _mm_aesenc_si128 (s1, LOAD (&rc[4 * (rci + 1)]));                     \
  s0 = _mm_aesenc_si128 (s0, LOAD (&rc[4 * (rci + 2)]));                     \
  s1 = _mm_aesenc_si128 (s1, LOAD (&rc[4 * (rci + 3)]));

#define AES2_4x(s0, s1, s2, s3, rci) \
  AES2(s0[0], s0[1], rci); \
  AES2(s1[0], s1[1], rci); \
  AES2(s2[0], s2[1], rci); \
  AES2(s3[0], s3[1], rci);

#define AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES2_4x(s0, s1, s2, s3, rci); \
  AES2_4x(s4, s5, s6, s7, rci);

#define AES4(s0, s1, s2, s3, rci) \
    s0 = _mm_aesenc_si128 (s0, LOAD (&rc[4 * (rci + 0)]));                     \
    s1 = _mm_aesenc_si128 (s1, LOAD (&rc[4 * (rci + 1)]));                     \
    s2 = _mm_aesenc_si128 (s2, LOAD (&rc[4 * (rci + 2)]));                     \
    s3 = _mm_aesenc_si128 (s3, LOAD (&rc[4 * (rci + 3)]));                     \
    s0 = _mm_aesenc_si128 (s0, LOAD (&rc[4 * (rci + 4)]));                     \
    s1 = _mm_aesenc_si128 (s1, LOAD (&rc[4 * (rci + 5)]));                     \
    s2 = _mm_aesenc_si128 (s2, LOAD (&rc[4 * (rci + 6)]));                     \
    s3 = _mm_aesenc_si128 (s3, LOAD (&rc[4 * (rci + 7)]));

#define AES4_4x(s0, s1, s2, s3, rci) \
  AES4(s0[0], s0[1], s0[2], s0[3], rci); \
  AES4(s1[0], s1[1], s1[2], s1[3], rci); \
  AES4(s2[0], s2[1], s2[2], s2[3], rci); \
  AES4(s3[0], s3[1], s3[2], s3[3], rci);

#define AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES4_4x(s0, s1, s2, s3, rci); \
  AES4_4x(s4, s5, s6, s7, rci);

#define MIX2(s0, s1) \
  tmp = _mm_unpacklo_epi32(s0, s1); \
  s1 = _mm_unpackhi_epi32(s0, s1); \
  s0 = tmp;

#define MIX2_4x(s0, s1, s2, s3) \
  MIX2(s0[0], s0[1]); \
  MIX2(s1[0], s1[1]); \
  MIX2(s2[0], s2[1]); \
  MIX2(s3[0], s3[1]);

#define MIX2_8x(s0, s1, s2, s3, s4, s5, s6, s7) \
  MIX2_4x(s0, s1, s2, s3); \
  MIX2_4x(s4, s5, s6, s7);

#define MIX4(s0, s1, s2, s3) \
  tmp  = _mm_unpacklo_epi32(s0, s1); \
  s0 = _mm_unpackhi_epi32(s0, s1); \
  s1 = _mm_unpacklo_epi32(s2, s3); \
  s2 = _mm_unpackhi_epi32(s2, s3); \
  s3 = _mm_unpacklo_epi32(s0, s2); \
  s0 = _mm_unpackhi_epi32(s0, s2); \
  s2 = _mm_unpackhi_epi32(s1, tmp); \
  s1 = _mm_unpacklo_epi32(s1, tmp);

#define MIX4_4x(s0, s1, s2, s3) \
  MIX4(s0[0], s0[1], s0[2], s0[3]); \
  MIX4(s1[0], s1[1], s1[2], s1[3]); \
  MIX4(s2[0], s2[1], s2[2], s2[3]); \
  MIX4(s3[0], s3[1], s3[2], s3[3]);

#define MIX4_8x(s0, s1, s2, s3, s4, s5, s6, s7) \
  MIX4_4x(s0, s1, s2, s3); \
  MIX4_4x(s4, s5, s6, s7);

#define AES_MIX2(s0, s1, rci) \
  AES2(s0, s1, rci); \
  MIX2(s0, s1);

#define AES_MIX2_4x(s0, s1, s2, s3, rci) \
  AES2_4x(s0, s1, s2, s3, rci); \
  MIX2_4x(s0, s1, s2, s3);

#define AES_MIX2_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci); \
  MIX2_8x(s0, s1, s2, s3, s4, s5, s6, s7);

#define AES_MIX4(s0, s1, s2, s3, rci) \
  AES4(s0, s1, s2, s3, rci); \
  MIX4(s0, s1, s2, s3);

#define AES_MIX4_4x(s0, s1, s2, s3, rci) \
  AES4_4x(s0, s1, s2, s3, rci); \
  MIX4_4x(s0, s1, s2, s3);

#define AES_MIX4_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci); \
  MIX4_8x(s0, s1, s2, s3, s4, s5, s6, s7);



void haraka256_256(unsigned char *out, const unsigned char *in);
void haraka256_256_4x(unsigned char *out, const unsigned char *in);
void haraka256_256_8x(unsigned char *out, const unsigned char *in);

void haraka256_256_chain(unsigned char *out, const unsigned char *in, int chainlen);
void haraka256_256_4x_chain(unsigned char *out, const unsigned char *in, int chainlen);

void haraka512_256(unsigned char *out, const unsigned char *in);
void haraka512_256_4x(unsigned char *out, const unsigned char *in);
void haraka512_256_8x(unsigned char *out, const unsigned char *in);

