#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"
#include "memory.h"

VM vm;

static void resetStack()
{
	//vm.stackTop = vm.stack;
	vm.count = 0;
}

void initVM()
{
	vm.stack = NULL;
	vm.capacity = 0;
	resetStack();
}

void freeVM()
{
	FREE_ARRAY(Value, vm.stack, vm.capacity);
	initVM();
}

void push(Value value)
{
	if (vm.capacity < vm.count + 1)
	{
		int oldCapacity = vm.capacity;
		vm.capacity = GROW_CAPACITY(oldCapacity);
		vm.stack = GROW_ARRAY(vm.stack, Value, oldCapacity, vm.capacity);
		//vm.stackTop = &vm.stack[vm.count];
	}

	//*vm.stackTop = value;
	//vm.stackTop++;
	vm.stack[vm.count] = value;
	vm.count++;
}

Value pop()
{
	//vm.stackTop--;
	//return  *vm.stackTop;
	vm.count--;
	return vm.stack[vm.count];
}

static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
	do { \
		double b = pop(); \
		double a = pop(); \
		push(a op b); \
	} while (false)

	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		//for (Value* slot = vm.stack; slot < vm.stackTop; slot++)
		for (int islot = 0; islot < vm.count; islot++)
		{
			printf("[ ");
			printValue(vm.stack[islot]);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{
		case OP_CONSTANT:
		{
			Value constant = READ_CONSTANT();
			push(constant);
			break;
		}
		case OP_ADD:      BINARY_OP(+); break;
		case OP_SUBTRACT: BINARY_OP(-); break;
		case OP_MULTIPLY: BINARY_OP(*); break;
		case OP_DIVIDE:   BINARY_OP(/); break;
		case OP_NEGATE:   push(-pop()); break;
		case OP_RETURN:
		{
			printValue(pop());
			printf("\n");
			return INTERPRET_OK;
		}
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk)
{
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();
	return result;
}
