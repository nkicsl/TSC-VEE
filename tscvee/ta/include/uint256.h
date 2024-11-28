#pragma once

#include <stdint.h>
#include <stdbool.h>

struct uint128_t
{
	uint64_t elements[2];
};

struct uint256_t
{
	struct uint128_t elements[2];
};

#define UPPER_P(x) x->elements[0]
#define LOWER_P(x) x->elements[1]
#define UPPER(x) x.elements[0]
#define LOWER(x) x.elements[1]

uint64_t readUint64BE(uint8_t *buffer);
void readu128BE(uint8_t *buffer, struct uint128_t *target);
void readu256BE(uint8_t *buffer, struct uint256_t *target);
bool zero128(struct uint128_t *number);
bool zero256(struct uint256_t *number);
void copy128(struct uint128_t *target, struct uint128_t *number);
void copy256(struct uint256_t *target, struct uint256_t *number);
void clear128(struct uint128_t *target);
void clear256(struct uint256_t *target);
void shiftl128(struct uint128_t *number, uint32_t value, struct uint128_t *target);
void shiftr128(struct uint128_t *number, uint32_t value, struct uint128_t *target);
void shiftl256(struct uint256_t *number, uint32_t value, struct uint256_t *target);
void shiftr256(struct uint256_t *number, uint32_t value, struct uint256_t *target);
uint32_t bits128(struct uint128_t *number);
uint32_t bits256(struct uint256_t *number);
bool equal128(struct uint128_t *number1, struct uint128_t *number2);
bool equal256(struct uint256_t *number1, struct uint256_t *number2);
bool gt128(struct uint128_t *number1, struct uint128_t *number2);
bool gt256(struct uint256_t *number1, struct uint256_t *number2);
bool gte128(struct uint128_t *number1, struct uint128_t *number2);
bool gte256(struct uint256_t *number1, struct uint256_t *number2);
void add128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void add256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void minus128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void minus256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void or128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void or256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void xor128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void xor256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void and128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void and256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void not128(struct uint128_t *number, struct uint128_t *target);
void not256(struct uint256_t *number, struct uint256_t *target);
void mul128(struct uint128_t *number1, struct uint128_t *number2, struct uint128_t *target);
void mul256(struct uint256_t *number1, struct uint256_t *number2, struct uint256_t *target);
void divmod128(struct uint128_t *l, struct uint128_t *r, struct uint128_t *div, struct uint128_t *mod);
void divmod256(struct uint256_t *l, struct uint256_t *r, struct uint256_t *div, struct uint256_t *mod);
bool tostring128(struct uint128_t *number, uint32_t base, char *out,
				 uint32_t outLength);
bool tostring256(struct uint256_t *number, uint32_t base, char *out,
				 uint32_t outLength);