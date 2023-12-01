/*
 * Poly1305 ε-almost-∆-universal hash function
 *
 * Note: this isn't the full Poly1305 MAC, i.e. it skips the final addition!
 *
 * Copyright (C) 2018 Google LLC
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "poly1305.h"

static void poly1305_key_powers(struct poly1305_key *key)
{
	const u32 r0 = key->r[0], r1 = key->r[1], r2 = key->r[2],
		  r3 = key->r[3], r4 = key->r[4];
	const u32 s1 = r1 * 5, s2 = r2 * 5, s3 = r3 * 5, s4 = r4 * 5;
	u32 h0 = r0, h1 = r1, h2 = r2, h3 = r3, h4 = r4;
	int i = 0;

	for (;;) {
		u64 d0, d1, d2, d3, d4;

		key->powers[i][0] = h0;
		key->powers[i][1] = h1;
		key->powers[i][2] = h1 * 5;
		key->powers[i][3] = h2;
		key->powers[i][4] = h2 * 5;
		key->powers[i][5] = h3;
		key->powers[i][6] = h3 * 5;
		key->powers[i][7] = h4;
		key->powers[i][8] = h4 * 5;

		if (++i == ARRAY_SIZE(key->powers))
			break;

		d0 = ((u64)h0 * r0) + ((u64)h1 * s4) + ((u64)h2 * s3) +
		     ((u64)h3 * s2) + ((u64)h4 * s1);
		d1 = ((u64)h0 * r1) + ((u64)h1 * r0) + ((u64)h2 * s4) +
		     ((u64)h3 * s3) + ((u64)h4 * s2);
		d2 = ((u64)h0 * r2) + ((u64)h1 * r1) + ((u64)h2 * r0) +
		     ((u64)h3 * s4) + ((u64)h4 * s3);
		d3 = ((u64)h0 * r3) + ((u64)h1 * r2) + ((u64)h2 * r1) +
		     ((u64)h3 * r0) + ((u64)h4 * s4);
		d4 = ((u64)h0 * r4) + ((u64)h1 * r3) + ((u64)h2 * r2) +
		     ((u64)h3 * r1) + ((u64)h4 * r0);

		d1 += (u32)(d0 >> 26);
		h0 = d0 & 0x3ffffff;
		d2 += (u32)(d1 >> 26);
		h1 = d1 & 0x3ffffff;
		d3 += (u32)(d2 >> 26);
		h2 = d2 & 0x3ffffff;
		d4 += (u32)(d3 >> 26);
		h3 = d3 & 0x3ffffff;
		h0 += (u32)(d4 >> 26) * 5;
		h4 = d4 & 0x3ffffff;
		h1 += h0 >> 26;
		h0 &= 0x3ffffff;
	}
}

void poly1305_setkey(struct poly1305_key *key, const u8 *raw_key)
{
	/* Clamp the Poly1305 key and split it into five 26-bit limbs */
	key->r[0] = (get_unaligned_le32(raw_key +  0) >> 0) & 0x3ffffff;
	key->r[1] = (get_unaligned_le32(raw_key +  3) >> 2) & 0x3ffff03;
	key->r[2] = (get_unaligned_le32(raw_key +  6) >> 4) & 0x3ffc0ff;
	key->r[3] = (get_unaligned_le32(raw_key +  9) >> 6) & 0x3f03fff;
	key->r[4] = (get_unaligned_le32(raw_key + 12) >> 8) & 0x00fffff;

	/* Precompute key powers */
	poly1305_key_powers(key);
}

