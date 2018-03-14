#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif


int main(int argc, char *argv[])
{
  const unsigned NUM_ITEMS = 10;
  int i;
  int size;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  /*
  array[0] = (int*)MALLOC(5 * sizeof(int));
  print_free();
  FREE(array[0]);
  print_free();
  */
  
  printf("sizeof char: %d\n", sizeof(char));
  printf("sizeof int: %d\n", sizeof(int));
  printf("sizeof block_node: %d\n", sizeof(block_node));
  printf("sizeof size_t: %d\n", sizeof(size_t));
  printf("sizeof size_t *: %d\n", sizeof(size_t *));
  printf("sizeof block_node *: %d\n", sizeof(block_node *));
  printf("sizeof ssize_t: %d\n", sizeof(ssize_t));
  //  printf("sizeof META_DATA_SIZE: %d\n", META_DATA_SIZE);
  

  size = 4;
  expected_sum += size * size;
  array[0] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[0][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[0][i];
  } //for i
  //print_allocated();
  print_free();
  
  printf("----- Done with first malloc -----\n");
  //FREE(array[0]);
  //print_allocated();
  //print_free();
  //printf("----------------------------------\n");
  
  size = 16;
  expected_sum += size * size;
  array[1] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[1][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[1][i];
  } //for i

  //print_allocated();
  //print_free();
  
  printf("----- Done with second malloc -----\n");

  size = 8;
  expected_sum += size * size;
  array[2] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[2][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[2][i];
  } //for i

  //print_allocated();
  //print_free();
  
  printf("----- Done with third malloc -----\n");

  size = 32;
  expected_sum += size * size;
  array[3] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[3][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[3][i];
  } //for i

 
  //  print_allocated();
 
  printf("----- Done with fourth malloc -----\n");


  FREE(array[0]);

  //print_allocated();
  print_free();

  printf("----- Done with first free -----\n");

  FREE(array[2]);
  
  //print_allocated();
  print_free();
  printf("----- Done with second free -----\n");
  printf("Should be no coalescing yet\n");
  
  FREE(array[1]);
  //print_allocated();
  print_free();
  printf("----- Free'd block 3rd block of allocated 4, freed blocks should coalesce -----\n");


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


  printf("Before free array[5]\n");
  //print_allocated();
  print_free();
  FREE(array[5]);
  printf("Post free array[5]\n");
  //print_allocated();
  print_free();
  //FREE(array[1]);
  //printf("Post free array[1]");
  FREE(array[3]);
  // printf("After free array[5]\n");


  size = 23;
  expected_sum += size * size;
  //printf("Pre-malloc\n");
  array[6] = (int *)MALLOC(size * sizeof(int));
  //  printf("Post-malloc\n");

  for (i=0; i < size; i++) {
    array[6][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[6][i];
  } //for i

  //printf("After the two for loops\n");

  size = 4;
  expected_sum += size * size;
  array[7] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[7][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[7][i];
  } //for i
  
  //printf("Here1\n");
  FREE(array[4]);
  //printf("Here2\n");

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

  //print_allocated();
  print_free();

  printf("All done with malloc and free \n");

  FREE(array[6]);
  FREE(array[7]);
  FREE(array[8]);
  FREE(array[9]);
  //print_allocated();
  print_free();
  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else
  //*/

  return 0;
}
