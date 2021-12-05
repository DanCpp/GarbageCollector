#include <stdlib.h>
#include <stdio.h>

#define STACK_MAX_SIZE 256
#define IGET 8;


typedef enum
{
	INT,
	TWIN
}oType;



typedef struct sObject
{
	oType type;
	unsigned int marked;

	struct sObject* next;

	union
	{
		int value;
		struct
		{
			struct sObject* head;
			struct sObject* tail;
		};
	};
}Object;

typedef struct
{
	Object* stack[STACK_MAX_SIZE];
	int stackSize;

	Object* firstObject;
	int numObjects;
	int maxObjects;
}vm;

void push(vm* vm, Object* value)
{
	vm->stack[vm->stackSize] = value;
}

void mark(Object* object)
{
	if (object->marked) return;

	object->marked = 1;

	if (object->type == TWIN)
	{
		mark(object->head);
		mark(object->tail);
	}
}

Object* pop(vm* vm)
{
	return vm->stack[vm->stackSize];
}

vm* newVm()
{
	vm* mainVm = (vm*)malloc(sizeof(vm));
	mainVm->stackSize = 0;
	mainVm->firstObject = NULL;
	mainVm->numObjects = 0;
	mainVm->maxObjects = IGET;
	return mainVm;
}

Object* newObject(vm* vm, oType type)
{
	if (vm->numObjects == vm->maxObjects) gc(vm);
	Object* object = (Object*)malloc(sizeof(Object));
	object->type = type;

	vm->numObjects++;
	return object;
}

void pushInt(vm* vm, int intV)
{
	Object* object = newObject(vm, INT);
	object->value = intV;
	push(vm, object);
}

Object* pushTwin(vm* vm)
{
	Object* object = newObject(vm, TWIN);
	object->tail = pop(vm);
	object->head = pop(vm);
	push(vm, object);
	return object;

}

void markAll(vm* vm)
{
	for (int i = 0; i < vm->stackSize; i++)
	{
		mark(vm->stack[i]);
	}
}



void marksweep(vm* vm)
{
	Object** object = &vm->firstObject;
	while (*object)
	{
		if (!(*object)->marked)
		{
			Object* unreached = *object;
			*object = unreached->next;
			free(unreached);
			vm->numObjects-- ;
		}
		else
		{
			(*object)->marked = 0;
			object = &(*object)->next;
		}
	}
}

void gc(vm* vm)
{
	int numObjects = vm->numObjects;

	markAll(vm);
	marksweep(vm);

	vm->maxObjects = vm->numObjects * 2;
}

void freeVm(vm* vm)
{
	vm->stackSize = 0;
	gc(vm);
	free(vm);
}

void printObj(Object* object)
{
	switch (object->type)
	{
	case INT:
		printf_s("%d", object->value);
		break;
	case TWIN:
		printf_s("(");
		printObj(object->head);
		printf_s(", ");
		printObj(object->tail);
		printf_s(")");
		break;
	}
}

void first_test()
{
	printf_s("1: ОбЪекты на стэке сохранены\n");
	vm* mainVm = newVm();
	pushInt(mainVm, 1);
	pushInt(mainVm, 2);

	gc(mainVm);
	freeVm(mainVm);
}

int main(int argc, const char** argv)
{
	first_test();
	return(0);
}
