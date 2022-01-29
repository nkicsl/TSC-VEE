#ifndef _OPCODE_H
#define _OPCODE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char OpCode;

bool OpCode_IsPush(OpCode op);

bool OpCode_IsStaticJump(OpCode op);

char* OpCode_String(OpCode op);

OpCode OpCode_StringToOp(char* str);

#endif 