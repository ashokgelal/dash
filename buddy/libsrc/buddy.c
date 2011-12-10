/*
 ============================================================================
 Name        : buddy.c
 Author      : Ashok Gelal
 Version     :
 Copyright   : All Wrongs Reserved
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "buddy.h"

void * raw_mem;
size_t raw_mem_size;
size_t initial_best_fit_size;
MemBlockPtr mem_blocks[MAX_SIZE];
Boolean initialized = FALSE;

static size_t find_best_fit_two(size_t);
static size_t validate_size(size_t);
static void * divide_mem_block(uint, uint);
static void * create_mem_block(uint, uint);
static MemBlockPtr get_buddy(MemBlockPtr, uint);
static Boolean are_buddies(MemBlockPtr, MemBlockPtr, uint);
static void	merge_with_buddy(MemBlockPtr, uint);
static void merge(MemBlockPtr, MemBlockPtr);
static void print_memerror();
static void rearrange(MemBlockPtr, uint);
static void remove_buddy(MemBlockPtr, uint);

// initializes buddy memory
// n is the size of memory
void buddy_init(size_t n){
	initial_best_fit_size = find_best_fit_two(validate_size(n));
	initialized = TRUE;
	raw_mem = sbrk(1 << initial_best_fit_size);

	raw_mem_size = (size_t) raw_mem;
	mem_blocks[initial_best_fit_size] = (MemBlockPtr) raw_mem;
	mem_blocks[initial_best_fit_size]->next = NULL;
	mem_blocks[initial_best_fit_size]->prev = NULL;
	mem_blocks[initial_best_fit_size]->header.is_used = FALSE;
	mem_blocks[initial_best_fit_size]->header.size_pow = initial_best_fit_size;
}

// overrides calloc to support library-interposing
void * calloc(size_t blocks, size_t size){
	return calloc(blocks, size);
}

// allocates blocks of memory each of given size
void * buddy_calloc(size_t blocks, size_t size){
	size_t best_fit_pow;
	uint i;

	best_fit_pow = find_best_fit_two(blocks * size);
	char *ret = (char *)buddy_malloc(best_fit_pow);
	char *sanitized = (char *)0;

	for(i=0; i < best_fit_pow; i++){
		ret = sanitized;
		ret++;
	}

	return ret;
}

// faciliates malloc to support library-interposing
void * malloc(size_t size){
	return buddy_malloc(size);
}

// mallocs memory of given size. If memory is not being
// initialized, it transparently initializes it.
void * buddy_malloc(size_t size){
	uint best_fit_pow, current_pow;

	if(initialized == FALSE)
		buddy_init((size_t)MAX_LIMIT);

	// add 'overhead' to size
	size += sizeof(BlockHeader);
	if(size > MAX_SIZE)
		best_fit_pow = find_best_fit_two(size);
	else
		best_fit_pow = MAX_POW;
	current_pow = best_fit_pow;
	while (current_pow <= initial_best_fit_size){
		if(mem_blocks[current_pow] != NULL){

			void * new_block;
			new_block = create_mem_block(current_pow, best_fit_pow);

			if(new_block == NULL)
				print_memerror();
			else
				return new_block;
		}
		current_pow++;
	}
	// we should never reach this point
	return NULL;
}

/// prints the buddy lists
void printBuddyLists(){
	uint i;
	for(i = 0; i<= initial_best_fit_size; i++){
		printf("Memory Block[%d] = ", i);
		MemBlockPtr ptr = mem_blocks[i];
		if(ptr != NULL) {
			while (ptr != NULL){
				printf("0x%d :: initialized = %s :: k = %d :: prev = %p :: next = %p ",
						(void *)ptr - raw_mem,
						ptr->header.is_used ? "true" : "false",
						ptr->header.size_pow,
						(void *)ptr->prev,
						(void *)ptr->next
				);
				ptr = ptr->next;
			}
		}
		else
			printf("NULL\n");
	}
	printf("\n");
}

// A helper method for printing out of memory error
static void print_memerror() {
	fprintf(stderr, "Out of Memory!");
	errno = ENOMEM;
	exit(EXIT_FAILURE);
}

// validates the given size
static size_t validate_size(size_t n){
	if(n > MAX_LIMIT) {
		print_memerror();
	}
	return (n == 0 ? DEFAULT_MEM_SIZE : n);
}

// returns the nearest best fit power of two
static size_t find_best_fit_two(size_t num){
	if(num < 2)
		return 0;
	uint next, nearest;
	next = 1;
	nearest = 0;

	while (next < num){
		next <<= 1;
		nearest++;
	}
	return nearest;
}

// a helper method for creating a memory blocks.
static void * create_mem_block(uint curr_pow, uint best_fit_pow){
	if(curr_pow == best_fit_pow){
		MemBlockPtr block = mem_blocks[curr_pow];

		if(block->next == NULL)
			mem_blocks[curr_pow] = NULL;
		else{
			mem_blocks[curr_pow] = block->next;
			mem_blocks[curr_pow]->prev = NULL;
		}
		block->header.is_used = TRUE;
		block->header.size_pow = curr_pow;

		char * temp = (char *) block;
		size_t temp_size = (uint) temp+sizeof(BlockHeader);
		return (void *)(temp_size);
	}
	else
		return divide_mem_block(curr_pow, best_fit_pow);
}

// a helper method for dividing a big chunk of memory to small chunks
static void *divide_mem_block(uint curr_pow, uint best_fit_pow){
	if(curr_pow == best_fit_pow)
		return create_mem_block(curr_pow, best_fit_pow);

	// start dividing
	MemBlockPtr big_block = mem_blocks[curr_pow];
	if(big_block->next == NULL)
		mem_blocks[curr_pow] = NULL;
	else{
		mem_blocks[curr_pow] = (MemBlockPtr)big_block->next;
		mem_blocks[curr_pow]->prev = NULL;
	}

	// next block
	curr_pow--;
	MemBlockPtr new_block = (MemBlockPtr)((void *) big_block + ( 1 << curr_pow));
	new_block->header.is_used = FALSE;
	new_block->header.size_pow = curr_pow;
	new_block->prev = big_block;
	new_block->next = NULL;

	mem_blocks[curr_pow] = big_block;
	big_block->header.size_pow--;
	big_block->next = new_block;
	big_block->prev = NULL;

	// tail-recursively start dividing big blocks
	return divide_mem_block(curr_pow, best_fit_pow);
}

// returns the buddy memory block pointer for given block pointer
static MemBlockPtr get_buddy(MemBlockPtr ptr, uint best_fit_pow){
	MemBlockPtr next = mem_blocks[best_fit_pow];
	while(next != NULL){
		if(are_buddies(ptr, next, best_fit_pow))
			return next;
		next = next->next;
	}
	return NULL;
}

// check if two memory block pointers are buddies
static Boolean are_buddies(MemBlockPtr client, MemBlockPtr anon, uint best_fit_pow){
	size_t client_add;
	client_add = (size_t)client - raw_mem_size;
	return (MemBlockPtr)(raw_mem_size + ((client_add ^ 1) << client->header.size_pow)) == anon;
}

// facilitates free call for library interposing
void free(void *obj){
	buddy_free(obj);
}

// frees memory for given pointer
void buddy_free(void *obj){
	MemBlockPtr ptr = (MemBlockPtr) ((char *)obj - sizeof(BlockHeader));
	ptr->header.is_used = FALSE;
	ptr->prev = NULL;
	ptr->next = NULL;
	uint curr_pow = ptr->header.size_pow;
	mem_blocks[curr_pow] == NULL ? (mem_blocks[curr_pow] = ptr) : merge_with_buddy(ptr, curr_pow);
}

// removes buddy from a tree
static void remove_buddy(MemBlockPtr buddy, uint best_fit_pow){
	MemBlockPtr prev_block, next_block;
	prev_block = buddy->prev;
	next_block = buddy->next;

	if(prev_block == NULL){
		if(next_block == NULL)
			mem_blocks[best_fit_pow] = NULL;
		else{
			mem_blocks[best_fit_pow] = next_block;
			next_block->prev = NULL;
		}
	}
	else{
		if(next_block == NULL)
			prev_block->next = NULL;
		else{
			prev_block->next = next_block;
			next_block->prev = prev_block;
		}
	}
}

// merges two buddies
static void merge(MemBlockPtr client, MemBlockPtr buddy){
	MemBlockPtr main_block, sec_block;
	uint curr_pow;

	if(buddy < client){
		main_block = buddy;
		sec_block = client;
	}else {
		main_block = client;
		sec_block = buddy;
	}

	curr_pow = main_block->header.size_pow;

	main_block->header.size_pow++;
	sec_block->header.size_pow++;

	remove_buddy(buddy, curr_pow);
	merge_with_buddy(main_block, main_block->header.size_pow);
}

// finds and either rearranges two memory blocks or merges them if they are buddies
// and both are free
static void	merge_with_buddy(MemBlockPtr ptr, uint best_fit_pow){
	MemBlockPtr buddy = get_buddy(ptr, best_fit_pow);
	buddy == NULL ? rearrange(ptr, best_fit_pow) : merge(ptr, buddy);
}

// rearranges two memory blocks
static void rearrange(MemBlockPtr client, uint i){
	MemBlockPtr parent_block, curr_block;
	curr_block = mem_blocks[i];
	if(curr_block == NULL){
		curr_block = client;
		client->next = NULL;
		client->prev = NULL;
		return;
	}

	while(curr_block != NULL){
		if(client < curr_block){
			client->prev = curr_block->prev;
			client->next = curr_block;
			if(curr_block->prev != NULL){
				curr_block->prev->next = client;
			}else{
				mem_blocks[i] = client;
			}
			curr_block->prev = client;
			return;
		}
		parent_block = curr_block;
		curr_block = curr_block->next;
	}

	parent_block->next = client;
	client->prev = parent_block;
	client->next = NULL;
}
