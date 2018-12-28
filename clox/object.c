#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
(type*)allocateObject(sizeof(type), objectType)

// fnv 32bit constants
#define FNV_OFFSET_BIAS 2166136261u;
#define FNV_PRIME 16777619;

static Obj* allocateObject(size_t size, ObjType type)
{
	Obj* object = (Obj*)reallocate(NULL, 0, size);
	object->type = type;

	object->next = vm.objects;
	vm.objects = object;
	return object;
}

static ObjString* allocateString(char* chars, int length, uint32_t hash)
{
	ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->chars = chars;

	Value key = OBJ_VAL(string);
	key.hash = hash;
	tableSet(&vm.strings, &key, NIL_VAL);

	return string;
}

uint32_t hashString(const char* key, int length)
{
	uint32_t hash = FNV_OFFSET_BIAS;

	// fnv-1a has the multiplication second
	for (int ii = 0; ii < length; ++ii)
	{
		hash ^= key[ii];
		hash *= FNV_PRIME;
	}

	return hash;
}

uint32_t hashNumber(double number)
{
	// a double has 8 bytes and each char is a byte
	// use a union to neatly break them out
	union
	{
		double val;
		char chars[8];
	} u;

	u.val = number;

	return hashString(u.chars, 8);
}

uint32_t hashBoolean(bool boolean)
{
	// a boolean is a single byte, same as a char
	char single = (char)boolean;
	return hashString(&single, 1);
}

uint32_t hashObject(Obj* obj)
{
	uint32_t hash = 0;
	
	// iterate over the object's fields and hash each byte
	// maybe could loop over bytes with a sizeof?

	// this value gets overwritten for strings in allocateString

	return hash;
}

ObjString* takeString(char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
	if (interned != NULL)
	{
		FREE_ARRAY(char, chars, length);
		return interned;
	}

	return allocateString(chars, length, hash);
}

ObjString* copyString(const char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
	if (interned != NULL) return interned;

	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';

	return allocateString(heapChars, length, hash);
}

void printObject(Value value)
{
	switch (OBJ_TYPE(value))
	{
	case OBJ_STRING:
		printf("%s", AS_CSTRING(value));
		break;
	}
}
