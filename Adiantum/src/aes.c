/*
 * AES block cipher (glue code)
 *
 * Copyright (C) 2018 Google LLC
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "aes.h"
#include <errno.h>
/*
 * Notes on chosen AES implementations:
 *
 * We only include implementations that are constant-time, or at least include
 * some hardening measures against cache-timing attacks.  By necessity we
 * tolerate implementations that try to be "constant-time" by prefetching the
 * lookup table(s) into cache, though this isn't guaranteed to be sufficient.
 *
 * For the portable implementation, we use aes_ti.c from Linux.  This is a
 * staightforward AES implementation that uses the 256-byte S-box and 256-byte
 * inverse S-box.  Each is prefetched before use.
 *
 * For 32-bit ARM: for AES-XTS we use the NEON bit-sliced implementation from
 * Linux; this is fastest and also constant-time.  For single blocks, we use the
 * ARM scalar AES cipher from Linux, which uses a 1024-byte table for
 * encryption, and a 1024-byte and a 256-byte table for decryption.  Even after
 * prefetching these tables, this implementation is much faster than aes_ti.c.
 * The 1024-byte tables combine the SubBytes (or InvSubBytes) and MixColumns (or
 * InvMixColumns) steps.  Normally 4096-byte tables would be needed for this,
 * but since rotations are "free" in ARM assembly only the first part is needed.
 */

#ifdef __arm__
void aesbs_convert_key(u8 out[], u32 const rk[], int rounds);
void aesbs_xts_encrypt(u8 out[], u8 const in[], u8 const rk[], int rounds,
		       int blocks, u8 iv[]);
void aesbs_xts_decrypt(u8 out[], u8 const in[], u8 const rk[], int rounds,
		       int blocks, u8 iv[]);
#endif

static void aes_setkey(struct aes_ctx *ctx, const u8 *key, int key_len)
{
	int err;

#ifdef __arm__
	err = aesti_expand_key(&ctx->aes_ctx, key, key_len);
	ASSERT(err == 0);
	ctx->rounds = 6 + key_len / 4;
	aesbs_convert_key(ctx->rk, ctx->aes_ctx.key_enc, ctx->rounds);
#endif
}

void aes128_setkey(struct aes_ctx *ctx, const u8 *key)
{
	aes_setkey(ctx, key, AES_KEYSIZE_128);
}

void aes256_setkey(struct aes_ctx *ctx, const u8 *key)
{
	aes_setkey(ctx, key, AES_KEYSIZE_256);
}

void aes_encrypt(const struct aes_ctx *ctx, u8 *out, const u8 *in)
{
#ifdef __arm__
	__aes_arm_encrypt(ctx->aes_ctx.key_enc, aes_nrounds(&ctx->aes_ctx),
			  in, out);
#else
	//aesti_encrypt(&ctx->aes_ctx, out, in);
#endif
}

void aes_decrypt(const struct aes_ctx *ctx, u8 *out, const u8 *in)
{
#ifdef __arm__
	__aes_arm_decrypt(ctx->aes_ctx.key_dec, aes_nrounds(&ctx->aes_ctx),
			  in, out);
#else
	//aesti_decrypt(&ctx->aes_ctx, out, in);
#endif
}

/*
 * Scalar fixed time AES core transform
 *
 * Copyright (C) 2017 Linaro Ltd <ard.biesheuvel@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * Emit the sbox as volatile const to prevent the compiler from doing
 * constant folding on sbox references involving fixed indexes.
 */
static volatile const u8 __cacheline_aligned __aesti_sbox[] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};


static u32 mul_by_x(u32 w)
{
	u32 x = w & 0x7f7f7f7f;
	u32 y = w & 0x80808080;

	/* multiply by polynomial 'x' (0b10) in GF(2^8) */
	return (x << 1) ^ (y >> 7) * 0x1b;
}

static u32 mul_by_x2(u32 w)
{
	u32 x = w & 0x3f3f3f3f;
	u32 y = w & 0x80808080;
	u32 z = w & 0x40404040;

	/* multiply by polynomial 'x^2' (0b100) in GF(2^8) */
	return (x << 2) ^ (y >> 7) * 0x36 ^ (z >> 6) * 0x1b;
}

static u32 mix_columns(u32 x)
{
	/*
	 * Perform the following matrix multiplication in GF(2^8)
	 *
	 * | 0x2 0x3 0x1 0x1 |   | x[0] |
	 * | 0x1 0x2 0x3 0x1 |   | x[1] |
	 * | 0x1 0x1 0x2 0x3 | x | x[2] |
	 * | 0x3 0x1 0x1 0x2 |   | x[3] |
	 */
	u32 y = mul_by_x(x) ^ ror32(x, 16);

	return y ^ ror32(x ^ y, 8);
}

static u32 inv_mix_columns(u32 x)
{
	/*
	 * Perform the following matrix multiplication in GF(2^8)
	 *
	 * | 0xe 0xb 0xd 0x9 |   | x[0] |
	 * | 0x9 0xe 0xb 0xd |   | x[1] |
	 * | 0xd 0x9 0xe 0xb | x | x[2] |
	 * | 0xb 0xd 0x9 0xe |   | x[3] |
	 *
	 * which can conveniently be reduced to
	 *
	 * | 0x2 0x3 0x1 0x1 |   | 0x5 0x0 0x4 0x0 |   | x[0] |
	 * | 0x1 0x2 0x3 0x1 |   | 0x0 0x5 0x0 0x4 |   | x[1] |
	 * | 0x1 0x1 0x2 0x3 | x | 0x4 0x0 0x5 0x0 | x | x[2] |
	 * | 0x3 0x1 0x1 0x2 |   | 0x0 0x4 0x0 0x5 |   | x[3] |
	 */
	u32 y = mul_by_x2(x);

	return mix_columns(x ^ y ^ ror32(y, 16));
}


static u32 subw(u32 in)
{
	return (__aesti_sbox[in & 0xff]) ^
	       (__aesti_sbox[(in >>  8) & 0xff] <<  8) ^
	       (__aesti_sbox[(in >> 16) & 0xff] << 16) ^
	       (__aesti_sbox[(in >> 24) & 0xff] << 24);
}

/*
 * Generate the AES key schedule:
 * - round keys for forward cipher are written to ->key_enc
 * - round keys for Equivalent Inverse Cipher are written to ->key_dec
 *
 * Warning: the resulting key schedule is not directly usable for
 * aesti_encrypt() and aesti_decrypt(); use aesti_set_key() for those.
 */
int aesti_expand_key(struct crypto_aes_ctx *ctx, const u8 *in_key,
			    unsigned int key_len)
{
	u32 kwords = key_len / sizeof(u32);
	u32 rc, i, j;

	if (key_len != AES_KEYSIZE_128 &&
	    key_len != AES_KEYSIZE_192 &&
	    key_len != AES_KEYSIZE_256)
		return -EINVAL;

	ctx->key_length = key_len;

	for (i = 0; i < kwords; i++)
		ctx->key_enc[i] = get_unaligned_le32(in_key + i * sizeof(u32));

	for (i = 0, rc = 1; i < 10; i++, rc = mul_by_x(rc)) {
		u32 *rki = ctx->key_enc + (i * kwords);
		u32 *rko = rki + kwords;

		rko[0] = ror32(subw(rki[kwords - 1]), 8) ^ rc ^ rki[0];
		rko[1] = rko[0] ^ rki[1];
		rko[2] = rko[1] ^ rki[2];
		rko[3] = rko[2] ^ rki[3];

		if (key_len == 24) {
			if (i >= 7)
				break;
			rko[4] = rko[3] ^ rki[4];
			rko[5] = rko[4] ^ rki[5];
		} else if (key_len == 32) {
			if (i >= 6)
				break;
			rko[4] = subw(rko[3]) ^ rki[4];
			rko[5] = rko[4] ^ rki[5];
			rko[6] = rko[5] ^ rki[6];
			rko[7] = rko[6] ^ rki[7];
		}
	}

	/*
	 * Generate the decryption keys for the Equivalent Inverse Cipher.
	 * This involves reversing the order of the round keys, and applying
	 * the Inverse Mix Columns transformation to all but the first and
	 * the last one.
	 */
	ctx->key_dec[0] = ctx->key_enc[key_len + 24];
	ctx->key_dec[1] = ctx->key_enc[key_len + 25];
	ctx->key_dec[2] = ctx->key_enc[key_len + 26];
	ctx->key_dec[3] = ctx->key_enc[key_len + 27];

	for (i = 4, j = key_len + 20; j > 0; i += 4, j -= 4) {
		ctx->key_dec[i]     = inv_mix_columns(ctx->key_enc[j]);
		ctx->key_dec[i + 1] = inv_mix_columns(ctx->key_enc[j + 1]);
		ctx->key_dec[i + 2] = inv_mix_columns(ctx->key_enc[j + 2]);
		ctx->key_dec[i + 3] = inv_mix_columns(ctx->key_enc[j + 3]);
	}

	ctx->key_dec[i]     = ctx->key_enc[0];
	ctx->key_dec[i + 1] = ctx->key_enc[1];
	ctx->key_dec[i + 2] = ctx->key_enc[2];
	ctx->key_dec[i + 3] = ctx->key_enc[3];

	return 0;
}

