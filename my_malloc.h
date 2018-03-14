#include <stdlib.h>

// Meta data for alloaced blocks

typedef struct block_node_t{

  size_t size;
  struct block_node_t * next;
  struct block_node_t * prev;

} block_node;



// First Fit malloc/free

void * ff_malloc(size_t size);

void ff_free(void * ptr);




// Best Fit malloc/free

void *bf_malloc(size_t size);

void bf_free(void * ptr);



// Performance (fragmentation) functions 

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space();




// Helper functions 

void add_to_free_list(block_node * to_add);

// Tries to split a block with size greater than the needed size
void attempt_split(block_node * to_split, size_t size_needed);

// Tries to re-use free'd blocks instead of growing heap
block_node * try_block_reuse(size_t size);

void remove_from_free_list(block_node * to_remove);

// Extends the process heap segment
block_node * grow_heap(size_t size);

// Coalesces free'd blocks if they exist around free_block
void coalesce(block_node * free_block);

