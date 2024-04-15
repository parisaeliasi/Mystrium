/*
 * Copyright (C) 2018 Google LLC
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#pragma once

void test_adiantum(void);

struct cipherbench_params {
	int bufsize;
	int ntries;
};

extern struct cipherbench_params g_params;
