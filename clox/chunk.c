#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
#include "value.h"

void initChunk(Chunk* chunk)
{
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	
	// add separate cound and capacity for lines array
	chunk->lineCount = 0;
	chunk->lineCapacity = 0;
	chunk->lines = NULL;

	initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk)
{
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line)
{
	if (chunk->capacity < chunk->count + 1)
	{
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(chunk->code, uint8_t, oldCapacity, chunk->capacity);
	}

	chunk->code[chunk->count] = byte;
	chunk->count++;

	// lines array uses run-length encoding in pairs of entries
	// which will reduce memory required to store line numbers
	// assuming several chunks in a row will be on the same line
	// [line#, count], [line#, count], ...

	// grow line capacity if not enough room to add 2 new entries for a new line
	if (chunk->lineCapacity < 2 * chunk->lineCount + 1)
	{
		int oldCapacity = chunk->lineCapacity;
		chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
		chunk->lines = GROW_ARRAY(chunk->lines, int, oldCapacity, chunk->capacity);
	}

	if (chunk->lineCount > 0 && chunk->lines[2 * chunk->lineCount - 2] == line)
	{
		// current chunk on same line as previous, increment counter
		chunk->lines[2 * chunk->lineCount - 1]++;
	}
	else
	{
		// current chunk on new line, add line entry
		chunk->lines[2 * chunk->lineCount] = line;
		chunk->lines[2 * chunk->lineCount + 1] = 1;
		chunk->lineCount++;
	}
}

int getLine(Chunk* chunk, int offset)
{
	int line = 0;

	int ichunk = 0;
	for (int iline = 0; iline < chunk->lineCount; iline++)
	{
		ichunk += chunk->lines[2 * iline + 1];
		if (ichunk > offset)
		{
			line = chunk->lines[2 * iline];
			break;
		}
	}

	return line;
}

int addConstant(Chunk* chunk, Value value)
{
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

// add value to constant array
// and write appropriate instruction to load the constant
void writeConstant(Chunk* chunk, Value value, int line)
{
	int constant = addConstant(chunk, value);
	
	if (constant < 256)
	{
		// default case, single chunk for operand
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, constant, line);
	}
	else
	{
		// use long version of constant operation
		// pass along operand as 24 bit number
		writeChunk(chunk, OP_CONSTANT_LONG, line);

		uint8_t byte00 = (uint8_t)((constant & 0x000000ffUL) >> 0);
		uint8_t byte08 = (uint8_t)((constant & 0x0000ff00UL) >> 8);
		uint8_t byte16 = (uint8_t)((constant & 0x00ff0000UL) >> 16);
		//uint8_t byte24 = (uint8_t)(constant & 0xff000000UL) >> 24; // only need first 24 bits, so don't write this

		// write first 24 bits of offset
		writeChunk(chunk, byte16, line);
		writeChunk(chunk, byte08, line);
		writeChunk(chunk, byte00, line);
	}
}

