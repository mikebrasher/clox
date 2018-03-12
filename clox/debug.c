#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name)
{
	printf("== %s ==\n", name);

	for (int ii = 0; ii < chunk->count;)
	{
		ii = disassembleInstruction(chunk, ii);
	}
}

int disassembleInstruction(Chunk* chunk, int offset)
{
	printf("%04d ", offset);
	
	if (offset > 0)
	{
		int prevLine = getLine(chunk, offset - 1);
		int currLine = getLine(chunk, offset);
		if (prevLine == currLine)
		{
			printf("   | ");
		}
		else
		{
			printf("%4d ", currLine);
		}
	}
	else
	{
		//printf("%4d ", chunk->lines[offset]);
		printf("%4d ", getLine(chunk, offset));
	}

	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
	case OP_CONSTANT:
		return constantInstruction("OP_CONSTANT", chunk, offset);
	case OP_CONSTANT_LONG:
		return constantInstructionLong("OP_CONSTANT_LONG", chunk, offset);
	case OP_RETURN:
		return simpleInstruction("OP_RETURN", offset);
	default:
		printf("Unknown opcode %d\n", instruction);
		return offset + 1;
	}
}

static int constantInstruction(const char* name, Chunk* chunk, int offset)
{
	uint8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 2;
}

static int constantInstructionLong(const char* name, Chunk* chunk, int offset)
{
	int constant = 
		  (chunk->code[offset + 1] << 16)
		+ (chunk->code[offset + 2] << 8)
		+ (chunk->code[offset + 3]);

	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 4;
}

static int simpleInstruction(const char* name, int offset)
{
	printf("%s\n", name);
	return offset + 1;
}
