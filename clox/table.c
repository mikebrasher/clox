#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table)
{
	table->count = 0;
	table->capacity = 0;
	table->entries = NULL;
}

void freeTable(Table* table)
{
	FREE_ARRAY(Entry, table->entries, table->capacity);
	initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, ObjString* key)
{
	uint32_t index = key->hash % capacity;
	Entry* tombstone = NULL;

	for (;;)
	{
		Entry* entry = &entries[index];

		if (entry->key == NULL)
		{
			if (IS_NIL(entry->value))
			{
				// empty entry
				return (tombstone != NULL) ? tombstone : entry;
			}
			else
			{
				// we found a tombstone
				if (tombstone == NULL) tombstone = entry;
			}
		}
		else if (entry->key == key)
		{
			// we found the key
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static void adjustCapacity(Table* table, int capacity)
{
	Entry* entries = ALLOCATE(Entry, capacity);
	for (int ii = 0; ii < capacity; ++ii)
	{
		entries[ii].key = NULL;
		entries[ii].value = NIL_VAL;
	}

	table->count = 0;
	for (int ii = 0; ii < table->capacity; ++ii)
	{
		Entry* entry = &table->entries[ii];
		if (entry->key == NULL) continue;

		Entry* dest = findEntry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
		table->count++;
	}

	FREE_ARRAY(Entry, table->entries, table->capacity);
	table->entries = entries;
	table->capacity = capacity;
}

bool tableGet(Table* table, ObjString* key, Value* value)
{
	if (table->entries == NULL) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);
	if (entry->key == NULL) return false;

	*value = entry->value;
	return true;
}

bool tableSet(Table* table, ObjString* key, Value value)
{
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
	{
		int capacity = GROW_CAPACITY(table->capacity);
		adjustCapacity(table, capacity);
	}

	Entry* entry = findEntry(table->entries, table->capacity, key);
	bool isNewKey = entry->key == NULL;
	entry->key = key;
	entry->value = value;

	if (isNewKey) table->count++;
	return isNewKey;
}

bool tableDelete(Table* table, ObjString* key)
{
	if (table->count == 0) return false;

	// Find the entry
	Entry* entry = findEntry(table->entries, table->capacity, key);
	if (entry->key == NULL) return false;

	// Place the tombstone in the entry
	entry->key == NULL;
	entry->value = BOOL_VAL(true);

	return true;
}

void tableAddAll(Table* from, Table* to)
{
	for (int ii = 0; ii < from->capacity; ++ii)
	{
		Entry* entry = &from->entries[ii];
		if (entry->key != NULL)
		{
			tableSet(to, entry->key, entry->value);
		}
	}
}

ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash)
{
	// if the table is empty, we definitely won't find it
	if (table->entries == NULL) return NULL;

	// figure out where to insert it in the table
	// use open addressing and basic linear probing
	uint32_t index = hash % table->capacity;

	for (;;)
	{
		Entry* entry = &table->entries[index];

		if (entry->key == NULL) return NULL;
		if (entry->key->length == length &&
			memcmp(entry->key->chars, chars, length) == 0)
		{
			// we found it
			return entry->key;
		}

		// try the next slot
		index = (index + 1) % table->capacity;
	}
}
