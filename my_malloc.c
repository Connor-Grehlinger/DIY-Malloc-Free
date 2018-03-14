#include "my_malloc.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

/***************************************************** 
 * ECE650 Homework Assignment 1: Implementing Malloc *
 * Developed by Connor Grehlinger (cmg88)            *
 * January 22, 2018                                  *
 *****************************************************/

/* NOTE: -Alignment macros not used in final implementation.
         -Global list of allocated blocks also not used in final implementation 
*/

/* For alignment purposes */
#define ALIGNMENT 8

/* Macro for finding the nearest multiple of 8 for alignment */
#define ALIGN(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT-1))

/* Size of meta data struct (offset to payload in memory) */
#define META_DATA_SIZE sizeof(block_node)  // 24 bytes

/* Minimum size threshold that determines if a block is split. 
 * This parameter can be set based on the target workload. */
#define MIN_SIZE META_DATA_SIZE + 128

/* Global list head */
//block_node * head = NULL;

/* Free list head */
block_node * free_list_head = NULL;

/* Global list tail */
//block_node * tail = NULL;

/* Free list tail */
block_node * free_list_tail = NULL;

/* Sizes for linked lists */
//unsigned long size = 0;

unsigned long free_size = 0;

/* Global for determining size of entire data segment */
unsigned long data_segment_size = 0;

/* First address returned by sbrk, used for error checking */
block_node * original_break;

unsigned long long num_cos = 0;
unsigned long long num_splits = 0;
unsigned long long num_mallocs = 0;
unsigned long long sum = 0;
unsigned long long num_reuse = 0;
unsigned long long num_frees = 0;

/* Print allocated blocks for debugging */
/*
void print_allocated(){
  printf("********** Printing allocated blocks... **********\n");
  block_node * current = head;
  printf("Original break location = %lu\n", original_break);
  printf("Current program break = %lu\n", sbrk(0));
  while (current){
    printf("----------\n");
    if (current == head){
      printf("Head:\n");
    }
    if (current == tail){
      printf("Tail:\n");
    }
    printf("address of previous block = %lu\n", current->prev);
    printf("address of current block  = %lu, block size = %lu\n", current, current->size);
    printf("address of next block     = %lu\n", current->next);
    printf("address of block + size   = %lu\n", (char*)current + current->size);
    current = current->next;
    printf("----------\n");
  }
  printf("**************************************************\n");
}
*/

/* Print free blocks for debugging */
void print_free(){
  printf("*********** Printing free blocks... ***********\n");
  block_node * current = free_list_head;

  printf("Current program break = %lu\n", sbrk(0));
 
  while (current){
    printf("----------\n");
    if (current == free_list_head){
      printf("Free list head:\n");
    }
    if (current == free_list_tail){
      printf("Free list tail:\n");
    }
    printf("address of previous block = %lu\n", current->prev);
    printf("address of current block  = %lu, block size = %lu\n", current, current->size);
    printf("address of next block     = %lu\n", current->next);
    printf("address of block + size   = %lu\n", (char*)current + current->size);
    
    if (current == current->next){
      printf("Inf loop error, free_size = %lu\n", free_size);
      break;
    }
    current = current->next;
    printf("----------\n");
  }
  printf("***********************************************\n");
}


/* Adds to free list in sorted order. 
 * Sorted insert is used to ensure that free blocks the form a contiguous 
 * segment of the heap are neighbors in the list and can be coalesced. */
void add_to_free_list(block_node * to_add){ 
  if (free_list_head == NULL){
    free_list_head = free_list_tail = to_add;
    free_list_head->next = NULL;
    free_list_head->prev = NULL;
  }
  else if (to_add < free_list_head){ // if added block has lower address than the list head
    to_add->next = free_list_head;  // add to front
    to_add->prev = NULL;
    to_add->next->prev = to_add;
    free_list_head = to_add;
  }
  else{
    block_node * current = free_list_head;
    while ((current->next) && (to_add > current->next)){ 
      // traverse list until <= next block or at end of free list 
      current = current->next;
    }
    to_add->next = current->next;
    to_add->prev = current;
    if (current->next){
      current->next->prev = to_add;
    }
    else{
      free_list_tail = to_add;
    }
    current->next = to_add;
  } 
  free_size++;
}



/* Adds to list at head (may not use in final implementation but helpful for development) */
/*
void add_to_list(block_node * to_add){
  to_add->next = NULL;
  to_add->prev = tail;
  tail = to_add;
  if (head == NULL){
    head = tail;
  }
  else{
    tail->prev->next = tail;
  }
  size++;
}
*/


/* Only split the block if it can hold the minimum set size (MIN_SIZE). 
 * Size_needed is the whole block size (for the malloc request AND the meta data).
 * This function essentially creates a block within another block and
 * adds it to the list of free blocks.
 */
void attempt_split(block_node * to_split, size_t size_needed){  
  if (to_split->size - size_needed >= MIN_SIZE){
    num_splits++; //
    block_node * new_block = (block_node *) ((char *) to_split + size_needed);
    new_block->size = to_split->size - size_needed; // set new block's size to remaining size
    
    //add_to_free_list(new_block); // add new block to the free list in sorted order  
    // no need to do sorted add, you have all the data you need to add here

    new_block->next = to_split->next;
    new_block->prev = to_split;
    if (to_split->next == NULL){
      free_list_tail = new_block;
    }
    else{
      to_split->next->prev = new_block;
    }
    to_split->size = size_needed; // update the size for the split block
    to_split->next = new_block;
    free_size++;
  }
}  



/* Search for free'd block to use, only search free list for performance */
block_node * try_block_reuse(size_t size){
  block_node * current_block = free_list_head; // get the head of the free list
  while (current_block){ 
    if (current_block->size >= size){ // if the current block can accomodate request
      num_reuse++; //
      attempt_split(current_block, size); 
      return current_block;
    }
    current_block = current_block->next; 
  }
  return NULL; // return NULL if no block found
}


/* For removing from list of allocated blocks, useful for debug and development */
/*
void remove_from_list(block_node * to_remove){
  block_node * current = head;
  while (current){ // while nodes exist in the list
    if (current == to_remove){
      if (current == head){ // if the head is removed, update global pointer
	head = current->next;
	if (head == NULL){
	  tail = head;
      	  return;
	}
	head->prev = NULL;
	size--;
	if (size == 1){
	  tail = head;
	}
      }
      else if (current == tail){
	tail = current->prev;
	tail->next = NULL;
	size--;
	if (size == 1){
	  head = tail;
	}
      }
      else{
	current->prev->next = current->next;
	current->next->prev = current->prev;
	size--;
      }
    }
    current = current->next;
  }  
}
*/

/* Removes from free list */
void remove_from_free_list(block_node * to_remove){
  if (free_list_head == NULL){
    fprintf(stderr,"Error: remove from free list called on empty list\n");
    return;
  }
  if (to_remove == free_list_head){
    free_size--;
    free_list_head = free_list_head->next;
    if (free_list_head){
      free_list_head->prev = NULL;
    }
    if (free_size == 1){
      free_list_tail = free_list_head;
      free_list_head->next = NULL;
    }   
  }
  else if (to_remove == free_list_tail){
    free_list_tail = free_list_tail->prev;
    if (free_list_tail){
      free_list_tail->next = NULL;
    }
    free_size--;
    if (free_size == 1){
      free_list_head = free_list_tail;
    }
  }
  else{
    to_remove->prev->next = to_remove->next;
    to_remove->next->prev = to_remove->prev;
    free_size--;
  }
  to_remove->next = NULL;
  to_remove->prev = NULL;
}


/* Extends the heap by size bytes, returns pointer to old break (for address of added block)*/   
block_node * grow_heap(size_t size){
  block_node * new_block = NULL;
  if ((new_block = sbrk(size)) == (void *) -1){ // check if sbrk failed, return NULL if true
    fprintf(stderr, "Error, sbrk call with size %lu failed\n", size);
    return NULL;
  }
  new_block->size = size; // set size of the block
  data_segment_size += size; // keep track of data segment size   
  return new_block;
}
  

/* First fit malloc */
void * ff_malloc(size_t size){
  size_t block_size = size + META_DATA_SIZE;
  num_mallocs++;
  sum += block_size;
  block_node * target_block = NULL;
  if (original_break){ // if blocks have been allocated
    target_block = try_block_reuse(block_size);
    if (target_block){ // if block found for re-use 
      remove_from_free_list(target_block); 
    }
    else{ // extend the heap if no block found 
      if ((target_block = grow_heap(block_size)) == NULL){ // check grow_heap function
	return NULL;
      }
    }
  }
  else{ // first allocation
    target_block = grow_heap(block_size);
    original_break = target_block;
  }
  return (char*)target_block + META_DATA_SIZE;
}


void coalesce(block_node * free_block){
  if(free_size > 1){
    if (free_block->next){
      block_node * next_location = (block_node *)((char*) free_block + free_block->size); 
      // next location in heap memory
      if (next_location == free_block->next){ 
	num_cos++; //

	// if the next free block in mem is adjacent to block being free'd
	free_block->size += free_block->next->size;
	remove_from_free_list(free_block->next);
	if (free_size == 1){
	  return; // no blocks left to coalesce
	}
      }
    }
    if (free_block->prev){
      block_node * current_location = 
	(block_node *)((char *) free_block->prev + free_block->prev->size); 
      if (current_location == free_block){
	num_cos++; //

	// check if current block is adjacent to previous block in mem
	free_block->prev->size += free_block->size;
	remove_from_free_list(free_block);
      }
    }
  }
}

/* For debugging and data collection */
void printAvg(){
  printf("Average request size = %lu\n", sum/num_mallocs);
}

void print_coalesces(){
  printf("Number of coalesces = %lu\n", num_cos);
}


void print_splits(){
  printf("Number of splits = %lu\n", num_splits);
}

void print_num_mallocs(){
  printf("Number of mallocs = %lu\n", num_mallocs);
}

void print_num_frees(){
  printf("Number of frees = %lu\n", num_frees);
}

void print_num_reuse(){
  printf("Number of block reuses = %lu\n", num_reuse);
}

 
/* For debugging */
/* Checks to ensure the address is one allocated by custom malloc */
char is_valid_address(block_node * to_check){	
  if (original_break){ // make sure malloc has at least been called once
    if ((original_break < to_check) && (to_check < (block_node*)sbrk(0))){ // range of heap
      return 1;
    }
  }
  return 0;
}
 


/* Free is required to coalesce adjacent free blocks. 
 * This is done in the called "coalesce" function. */
void ff_free(void * ptr){
  num_frees++;
  if (ptr == NULL){ // freeing NULL does nothing 
    return;
  }
  // get address of meta data
  block_node * to_free = (block_node *)((char *)ptr - META_DATA_SIZE);  
  add_to_free_list(to_free);
  coalesce(to_free);
}


/* Search for free'd block to use, only search free list for performance */
block_node * try_block_reuse_bf(size_t size){
  block_node * current_block = free_list_head; // get the head of the free list
  size_t smallest_diff = ULONG_MAX; 
  size_t current_diff;
  block_node * result = NULL;
  while (current_block){ // while current block_node != NULL
    if (current_block->size >= size){ // if the current block can accomodate request
      current_diff = current_block->size - size;
      if (current_diff < smallest_diff){
	smallest_diff = current_diff; // update mimimum
	result = current_block;
      }
    }
    current_block = current_block->next; // check next block
  }
  if (result){ // if block found, attempt to split it
    attempt_split(result, size);
  }
  return result;
}


void * bf_malloc(size_t size){
  size_t block_size = size + META_DATA_SIZE;
  num_mallocs++;
  sum += block_size; 
  block_node * target_block = NULL;
  if (original_break == NULL){ // if first malloc call
    target_block = grow_heap(block_size);
    original_break = target_block;
  }
  else{ // try to re-use any free'd blocks
    target_block = try_block_reuse_bf(block_size);
    if (target_block){ // if block found for re-use 
      remove_from_free_list(target_block);
    }
    else{ // extend the heap
      if ((target_block = grow_heap(block_size)) == NULL){
	return NULL;
      }
    }
  }
  return (char*)target_block + META_DATA_SIZE;
}


void bf_free(void * ptr){
  block_node * to_free = (block_node *)((char *)ptr - META_DATA_SIZE);
  add_to_free_list(to_free);
  coalesce(to_free);
}


unsigned long get_data_segment_size(){
  return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
  unsigned long free_space = 0;
  block_node * current = free_list_head;
  while (current){
    free_space += current->size;
    current = current->next;
  }
  return free_space;
}
