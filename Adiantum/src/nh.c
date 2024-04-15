/*
 * NH hash algorithm, specifically the variant used by Adiantum hashing
 *
 * Copyright (C) 2018 Google LLC
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "nh.h"

void nh_setkey(struct nh_ctx *ctx, const u8 *key)
{
	int i;

	for (i = 0; i < NH_KEY_DWORDS; i++)
		ctx->key[i] = get_unaligned_le32(key + i * sizeof(u32));
}

