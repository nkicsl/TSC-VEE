#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

struct Stack* Stack_newStack(struct Stack* s, int maxElements) {
	s->array = (struct uint256_t*)malloc(sizeof(struct uint256_t) * maxElements);
	s->capacity = maxElements;
	s->stackTop = EmptyTOS;
	return s;
}
//Stack* returnStack(Stack* s);
struct uint256_t* Stack_Data(struct Stack* s) {
	return s->array;
}
void Stack_push(struct Stack* s, struct uint256_t e) {
	if (s->stackTop != s->capacity - 1) {
		s->stackTop++;
		copy256(&s->array[s->stackTop], &e);
	}
}
//void Stack_pushN(Stack* s, elementType* e);
struct uint256_t Stack_pop(struct Stack* s) {
	struct uint256_t e;
	copy256(&e, &s->array[s->stackTop]);
	clear256(&s->array[s->stackTop]);
	s->stackTop--;
	return e;
}
int Stack_len(struct Stack* s) {
	return s->stackTop + 1;
}
void Stack_swap(struct Stack* s, int n) {
	if (s->stackTop != EmptyTOS) {
		int len = s->stackTop + 1;
		if (n <= len && n >= 2) {
			int m = (n % 2 == 0) ? n / 2 : n / 2 + 1;
			struct uint256_t e;
			for (int i = n; i > m; i--) {
				copy256(&e, &s->array[len - n]);
				copy256(&s->array[len - n], &s->array[len - n + i - 1]);
				copy256(&s->array[len - n + i - 1], &e);
			}
		}
		else {
			printf("N is not fit!\n");
		}
	}
	else {
		printf("Stack is empty!\n");
	}
}
void Stack_dup(struct Stack* s, int n) {
	for (int i = n; i > 0; i--) {
		Stack_push(s, s->array[s->stackTop - n + 1]);
	}
}
struct uint256_t* Stack_peek(struct Stack* s) {
	return &s->array[s->stackTop];
}
struct uint256_t* Stack_Back(struct Stack* s, int n) {
	return &s->array[s->stackTop - n - 1];
}
void Stack_Print(struct Stack* s) {
	uint32_t outLength = 256;
	char* out = (char*)malloc(sizeof(char) * outLength);
	printf("### stack ###\n");
	int len = s->stackTop + 1;
	if (len > 0) {
		for (int i = 0; i < len; i++) {
			uint32_t base = 16;
			if (tostring256(&s->array[i], base, out, outLength)) {
				printf("%-3d %s\n", i, out);
			}
		}
	}
	else {
		printf("-- empty --\n");
	}
	printf("#############\n");
}