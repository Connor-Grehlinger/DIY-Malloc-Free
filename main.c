#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"
#include <assert.h>

/*
#ifdef FF
*/
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
/*
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif
*/

int main(int argc, char *argv[])
{
  

  printf("Size of int = %d\n", sizeof(int));
  printf("Size of int * = %d\n", sizeof(int*));
  printf("size of block_node = %d\n", sizeof(block_node));

  unsigned long start_addr = sbrk(0);

  //printf("End of BSS segment address = %p\n", &end);
  //printf("End of BSS segment value = %lu\n", &end);
  printf("sbrk(0) (hex) = %p\n", sbrk(0));
  printf("sbrk(0) = %lu\n", sbrk(0));
 
  
  printf("--- Start, no allocation yet ---\n");
  printf("data seg size = %lu\n", get_data_segment_size());
  printf("free space size = %lu\n", get_data_segment_free_space_size());
  

  int * int_ptr = MALLOC(8 * sizeof(int));
  unsigned long size_of_request = (8 * sizeof(int)) + sizeof(struct block_node_t);

  printf("--- After allocation ---\n");
  printf("data seg size = %lu\n", get_data_segment_size());
  printf("free space size = %lu\n", get_data_segment_free_space_size());
  printf("sbrk(0) = %lu\n", sbrk(0));
 


  printf("--- Post tests --- \n");
  FREE(int_ptr);
  unsigned long heap_growth = (sbrk(0)) - start_addr;
  assert(size_of_request == heap_growth);
  printf("Amount the heap grew = %lu\n", heap_growth);
  
  /*
  const unsigned NUM_ITEMS = 10;
  int i;
  int size;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  size = 4;
  expected_sum += size * size;
  array[0] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[0][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[0][i];
  } //for i

  size = 16;
  expected_sum += size * size;
  array[1] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[1][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[1][i];
  } //for i

  size = 8;
  expected_sum += size * size;
  array[2] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[2][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[2][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  array[3] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[3][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[3][i];
  } //for i

  FREE(array[0]);
  FREE(array[2]);

  size = 7;
  expected_sum += size * size;
  array[4] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[4][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[4][i];
  } //for i

  size = 256;
  expected_sum += size * size;
  array[5] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[5][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[5][i];
  } //for i

  FREE(array[5]);
  FREE(array[1]);
  FREE(array[3]);

  size = 23;
  expected_sum += size * size;
  array[6] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[6][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[6][i];
  } //for i

  size = 4;
  expected_sum += size * size;
  array[7] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[7][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[7][i];
  } //for i

  FREE(array[4]);

  size = 10;
  expected_sum += size * size;
  array[8] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[8][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[8][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  array[9] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[9][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[9][i];
  } //for i

  FREE(array[6]);
  FREE(array[7]);
  FREE(array[8]);
  FREE(array[9]);

  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else
  
  */

  
  return 0;
}
