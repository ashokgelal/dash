/*
 * buddy.h
 *
 *      Author: ashokgelal
 */

#ifndef __BUDDY_H_
#define __BUDDY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"

// max memory limit = 2^32
#define MAX_LIMIT 4294967296

#define MAX_SIZE 32

#define MAX_POW 5

// default mem size = 2^16
#define DEFAULT_MEM_SIZE 1 << 16

typedef struct block_header BlockHeader;

struct block_header{
	Boolean is_used;
	uint size_pow;
};

typedef struct mem_block MemBlock;
typedef struct mem_block * MemBlockPtr;

struct mem_block{
	BlockHeader header;
	MemBlockPtr next;
	MemBlockPtr prev;
};

void buddy_init(size_t);
void *buddy_calloc(size_t, size_t);
void *buddy_malloc(size_t);
void buddy_free(void *);
void print_pointer(void *);
void printBuddyLists();

void free(void *);
void *calloc(size_t, size_t);
void *malloc(size_t);

#endif /* __BUDDY_H_ */
