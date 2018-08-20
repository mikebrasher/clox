#include <stdio.h>

#include "memory.h"
#include "value.h"

bool valuesEqual(Value a, Value b)
{
	bool ret = false;

	if (a.type == b.type)
	{
		switch (a.type)
		{
		case VAL_BOOL:   ret = AS_BOOL(a) == AS_BOOL(b); break;
		case VAL_NIL:    ret = true; break;
		case VAL_NUMBER: ret = AS_NUMBER(a) == AS_NUMBER(b); break;
		default:         ret = false; break;
		}
	}

	return ret;
}

void initValueArray(ValueArray* array)
{
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void writeValueArray(ValueArray* array, Value value)
{
	if (array->capacity < array->count + 1)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values = GROW_ARRAY(array->values, Value, oldCapacity, array->capacity);
	}

	array->values[array->count] = value;
	array->count++;
}

void freeValueArray(ValueArray* array)
{
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

void printValue(Value value)
{
	switch (value.type)
	{
	case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
	case VAL_NIL:  printf("nil"); break;
	case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
	}
}
