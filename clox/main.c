#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[])
{
	Chunk chunk;
	initChunk(&chunk);

	// wrap these calls in writeConstant
	//int constant = addConstant(&chunk, 1.2);
	//writeChunk(&chunk, OP_CONSTANT, 123);
	//writeChunk(&chunk, constant, 123);

	double val = 0.0;
	int count = 0;
	int line = 1;
	for (int ii = 0; ii < 300; ii++)
	{
		writeConstant(&chunk, val, line);
		val += 0.1;

		count++;
		if (count > 9)
		{
			count = 0;
			line++;
		}
	}

	writeChunk(&chunk, OP_RETURN, 123);

	disassembleChunk(&chunk, "test chunk");
	freeChunk(&chunk);

    return 0;
}

