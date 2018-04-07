#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
	initVM();

	Chunk chunk;
	initChunk(&chunk);

	int constant = 0;

	int challenge = 22;
	switch (challenge)
	{
	case 11:
		// challenge 1.1
		// 1 * 2 + 3 = 5
		// (1*2)+3
		constant = addConstant(&chunk, 1.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_MULTIPLY, challenge);

		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_ADD, challenge);
		break;

	case 12:
		// challenge 1.2
		// 1 + 2 * 3 = 7
		// 1+(2*3)
		constant = addConstant(&chunk, 1.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_MULTIPLY, challenge);
		writeChunk(&chunk, OP_ADD, challenge);
		break;

	case 13:
		// challenge 1.3
		// 3 - 2 - 1 = 0
		// (3-2)-1
		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_SUBTRACT, challenge);

		constant = addConstant(&chunk, 1.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_SUBTRACT, challenge);
		break;

	case 14:
		// challenge 1.4
		// 1 + 2 * 3 - 4 / -5 = 7.8
		// (1+(2*3))-(4/(-5))
		constant = addConstant(&chunk, 1.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_MULTIPLY, challenge);

		writeChunk(&chunk, OP_ADD, challenge);

		constant = addConstant(&chunk, 4.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);
		
		constant = addConstant(&chunk, 5.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);		
		
		writeChunk(&chunk, OP_NEGATE, challenge);

		writeChunk(&chunk, OP_DIVIDE, challenge);

		writeChunk(&chunk, OP_SUBTRACT, challenge);
		break;

	case 21:
		// challenge 2.1
		// 4 - 3 * -2 = 10 without negate
		// 4 - 3 * (0 - 2)
		constant = addConstant(&chunk, 4.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 0.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_SUBTRACT, challenge);

		writeChunk(&chunk, OP_MULTIPLY, challenge);

		writeChunk(&chunk, OP_SUBTRACT, challenge);
		break;

	case 22:
		// challenge 2.1
		// 4 - 3 * -2 = 10 without subtract
		// 4 + -(3 * -2)
		constant = addConstant(&chunk, 4.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 3.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		constant = addConstant(&chunk, 2.0);
		writeChunk(&chunk, OP_CONSTANT, challenge);
		writeChunk(&chunk, constant, challenge);

		writeChunk(&chunk, OP_NEGATE, challenge);

		writeChunk(&chunk, OP_MULTIPLY, challenge);

		writeChunk(&chunk, OP_NEGATE, challenge);

		writeChunk(&chunk, OP_ADD, challenge);
		break;

	default:
		// main chapter
		constant = addConstant(&chunk, 1.2);
		writeChunk(&chunk, OP_CONSTANT, 123);
		writeChunk(&chunk, constant, 123);

		constant = addConstant(&chunk, 3.4);
		writeChunk(&chunk, OP_CONSTANT, 123);
		writeChunk(&chunk, constant, 123);

		writeChunk(&chunk, OP_ADD, 123);

		constant = addConstant(&chunk, 5.6);
		writeChunk(&chunk, OP_CONSTANT, 123);
		writeChunk(&chunk, constant, 123);

		writeChunk(&chunk, OP_DIVIDE, 123);
		writeChunk(&chunk, OP_NEGATE, 123);
		break;
	}

	writeChunk(&chunk, OP_RETURN, 123);

	disassembleChunk(&chunk, "test chunk");

	interpret(&chunk);
	freeVM();
	freeChunk(&chunk);

    return 0;
}

