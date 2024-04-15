#include <stddef.h>
#include <stdint.h>
#include "align.h"

void deckFunction(const uint8_t *input, const uint8_t *key, const uint8_t *output, const uint8_t *savestate, size_t outputLen, size_t inputLen, const uint8_t *inputV); 
void testWBCFunc(void);
void Multimixer128field(const uint8_t *input, const uint8_t *key, const uint8_t *output, size_t inputLen, const uint8_t *inputT);
void keyFunction(const uint8_t *input, const uint8_t *output, size_t outputLen);
