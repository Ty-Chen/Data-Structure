#pragma once

#ifndef  SHA1_HPP
#define SHA1_HPP

#include <stdint.h>
typedef uint32_t u32;
typedef uint8_t u8;

struct  SHA_CTX
{
	u32 state[5];
	u32 count[2];
	u8 buffer[64];
};

void SHA1_Init(SHA_CTX* context);
void SHA1_Update(SHA_CTX* context, u8 const* data, u32 len);
void SHA1_Final(u8* digest, SHA_CTX* context);

#endif // !SHA1_HPP 