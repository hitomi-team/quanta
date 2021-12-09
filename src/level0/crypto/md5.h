#ifndef LEVEL0_CRYPTO_MD5_H
#define LEVEL0_CRYPTO_MD5_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint64_t size;        // Size of input in bytes
	uint32_t buffer[4];   // Current accumulation of hash
	uint8_t input[64];    // Input to be used in the next step
} MD5Context;

void md5Init(MD5Context *ctx);
void md5Update(MD5Context *ctx, const uint8_t *input, size_t input_len);
void md5Finalize(MD5Context *ctx, uint8_t *result);
void md5Step(uint32_t *buffer, const uint32_t *input);

void md5String(uint8_t *result, const char *input);
void md5File(uint8_t *result, FILE *file);

#ifdef __cplusplus
}
#endif

#endif
