#ifndef _STACK_H_
#define _STACK_H_

#include "uint256.h"

#define EmptyTOS -1  

struct Stack {
	int capacity;
	int stackTop;
	struct uint256_t* array;
};

struct Stack* Stack_newStack(struct Stack* s, int maxElements);
//struct Stack* returnStack(struct Stack* s);
struct uint256_t* Stack_Data(struct Stack* s);
void Stack_push(struct Stack* s, struct uint256_t e);
//void Stack_pushN(struct Stack* s, struct uint256_t* e);
struct uint256_t Stack_pop(struct Stack* s);
int Stack_len(struct Stack* s);
void Stack_swap(struct Stack* s, int n);
void Stack_dup(struct Stack* s, int n);
struct uint256_t* Stack_peek(struct Stack* s);
struct uint256_t* Stack_Back(struct Stack* s, int n);
void Stack_Print(struct Stack* s);

#endif