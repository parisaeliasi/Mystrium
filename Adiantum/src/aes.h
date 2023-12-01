/*
 * Copyright (C) 2018 Google LLC
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#pragma once

#include "util.h"

#define AES_MIN_KEY_SIZE	16
#define AES_MAX_KEY_SIZE	32
#define AES_KEYSIZE_128		16
#define AES_KEYSIZE_192		24
#define AES_KEYSIZE_256		32
#define AES_BLOCK_SIZE		16
#define AES_MAX_KEYLENGTH	(15 * 16)
#define AES_MAX_KEYLENGTH_U32	(AES_MAX_KEYLENGTH / sizeof(u32))

/*
 * Please ensure that the first two fields are 16-byte aligned
 * relative to the start of the structure, i.e., don't move them!
 */
struct crypto_aes_ctx {
	u32 key_enc[AES_MAX_KEYLENGTH_U32];
	u32 key_dec[AES_MAX_KEYLENGTH_U32];
	u32 key_length;
};

struct aes_ctx {
#ifdef __arm__	/* for aes-neonbs */
	int rounds;
	u8 pad[12];
	u8 rk[13 * (8 * AES_BLOCK_SIZE) + 32];
#endif
	struct crypto_aes_ctx aes_ctx;
} __attribute__((aligned(32)));

void aes128_setkey(struct aes_ctx *ctx, const u8 *key);
void aes256_setkey(struct aes_ctx *ctx, const u8 *key);
void aes_encrypt(const struct aes_ctx *ctx, u8 *out, const u8 *in);
void aes_decrypt(const struct aes_ctx *ctx, u8 *out, const u8 *in);

static inline int aes_nrounds(const struct crypto_aes_ctx *ctx)
{
	/*
	 * AES-128: 6 + 16 / 4 = 10 rounds
	 * AES-192: 6 + 24 / 6 = 12 rounds
	 * AES-256: 6 + 32 / 8 = 14 rounds
	 */
	return 6 + ctx->key_length / 4;
}

int aesti_expand_key(struct crypto_aes_ctx *ctx, const u8 *in_key,
		     unsigned int key_len);

int aesti_set_key(struct crypto_aes_ctx *ctx, const u8 *in_key,
		  unsigned int key_len);
void aesti_encrypt(const struct crypto_aes_ctx *ctx, u8 *out, const u8 *in);
void aesti_decrypt(const struct crypto_aes_ctx *ctx, u8 *out, const u8 *in);

#ifdef __arm__
void __aes_arm_encrypt(const u32 *rk, int rounds, const u8 *in, u8 *out);
void __aes_arm_decrypt(const u32 *rk, int rounds, const u8 *in, u8 *out);
#endif
