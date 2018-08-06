//
//  COMP1927 Assignment 1 - Vlad: the memory allocator
//  allocator.c ... implementation
//
//  Created by Liam O'Connor on 18/07/12.
//  Modified by John Shepherd in August 2014, August 2015
//  Copyright (c) 2012-2015 UNSW. All rights reserved.
//

#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define FREE_HEADER_SIZE  sizeof(struct free_list_header)  
#define ALLOC_HEADER_SIZE sizeof(struct alloc_block_header)  
#define MAGIC_FREE     0xDEADBEEF
#define MAGIC_ALLOC    0xBEEFDEAD

#define BEST_FIT       1
#define WORST_FIT      2
#define RANDOM_FIT     3

typedef unsigned char byte;
typedef u_int32_t vsize_t;
typedef u_int32_t vlink_t;
typedef u_int32_t vaddr_t;

typedef struct free_list_header {
   u_int32_t magic;  // ought to contain MAGIC_FREE
   vsize_t size;     // # bytes in this block (including header)
   vlink_t next;     // memory[] index of next free block
   vlink_t prev;     // memory[] index of previous free block
} free_header_t;

typedef struct alloc_block_header {
   u_int32_t magic;  // ought to contain MAGIC_ALLOC
   vsize_t size;     // # bytes in this block (including header)
} alloc_header_t;

// Global data

static byte *memory = NULL;   // pointer to start of allocator memory
static vaddr_t free_list_ptr; // index in memory[] of first block in free list
static vsize_t memory_size;   // number of bytes malloc'd in memory[]
static u_int32_t strategy;    // allocation strategy (by default BEST_FIT)

// Private functions

static void vlad_merge();
static void checkFreeHeader(free_header_t * Header);
static void checkAllocHeader(alloc_header_t * Header);






// Input: size - number of bytes to make available to the allocator
// Output: none              
// Precondition: Size >= 1024
// Postcondition: `size` bytes are now available to the allocator
// 
// (If the allocator is already initialised, this function does nothing,
//  even if it was initialised with different size)

void vlad_init(u_int32_t size)
{
   free_list_ptr = (vaddr_t)0;
   strategy = BEST_FIT;
   
   //calculate memory_size
   for (memory_size = 1024; memory_size < size; memory_size *= 2){};
   memory = malloc(memory_size);
   
   //make sure malloc succeed
   if (memory == NULL){
      fprintf(stderr, "vlad_init: Insufficient memory\n");
      exit(EXIT_FAILURE);
   } 
   
   //initialize the memory with single free list
   free_header_t * FreeHeader;
   FreeHeader = (free_header_t *) memory;
   FreeHeader->magic = MAGIC_FREE;
   FreeHeader->size = memory_size;
   FreeHeader->next = (vlink_t) 0;
   FreeHeader->prev = (vlink_t) 0;
}








// Input: n - number of bytes requested
// Output: p - a pointer, or NULL
// Precondition: n < size of largest available free block
// Postcondition: If a region of size n or greater cannot be found, p = NULL 
//                Else, p points to a location immediately after a header block
//                      for a newly-allocated region of some size >= 
//                      n + header size.

void *vlad_malloc(u_int32_t n)
{
   if(n < 8){
      n = 8;
   }
   while ( n%4 != 0){
      n++;
   }
   

   vaddr_t Curr = free_list_ptr;
   //find the best free region
   vaddr_t free_region_best = free_list_ptr;
   int isFound = 0;
   vsize_t best_size = 4096;
   free_header_t * Header = (free_header_t *) (free_list_ptr + &memory[0]);
   //check the first free region
   if ((Header->size) >= (ALLOC_HEADER_SIZE + n)){
      free_region_best = free_list_ptr;
      checkFreeHeader(Header);
      best_size = Header->size;
      isFound = 1;
   }
   //printf("%d\n", Curr);
   Curr = Header->next;
   //printf("%d\n", Curr);
   
   while (Curr != free_list_ptr){
      Header = (free_header_t *) (Curr + &memory[0]);
      checkFreeHeader(Header);
      if (  (Header->size) >= (ALLOC_HEADER_SIZE + n) 
         && (Header->size) < best_size){
         free_region_best = Curr;
         best_size = Header->size;
         isFound = 1;
      }
      Curr = Header->next;
   }
   fprintf(stderr,"free_region_best is %d",free_region_best);

   //if no single free region big enough
   if (isFound == 0){
      return NULL;
   }
   
   //begin to allocate that part
   
   //no need to part a new free memory
   Header = (free_header_t *) (free_region_best + &memory[0]);
   checkFreeHeader(Header);
   if (Header->size < ALLOC_HEADER_SIZE + n + 2*FREE_HEADER_SIZE){
      if (Header->next == free_region_best && Header->prev == free_region_best){
         return NULL;
      }
      //if the region free_list_ptr point to was chosen
      if (free_region_best == free_list_ptr){
         free_header_t * ChangeHeader = (free_header_t *) (free_list_ptr + &memory[0]);
         free_list_ptr = ChangeHeader->next;
      }
      
      //make the prev's next to be the curr's next
      free_header_t * PrevHeader = (free_header_t *) (Header->prev + &memory[0]);
      checkFreeHeader(PrevHeader);
      PrevHeader->next = Header->next;
      
      //make the next's prev to be the curr's prev
      free_header_t * NextHeader = (free_header_t *) (Header->next + &memory[0]);
      checkFreeHeader(NextHeader);
      NextHeader->prev = Header->prev;
   
      alloc_header_t * NewHeader = (alloc_header_t *) Header;
      
      NewHeader->magic = MAGIC_ALLOC;
      
      NewHeader->size = Header->size;
      
      checkAllocHeader(NewHeader);
      

      return (void *) (NewHeader + 1);
   }
   
   //need to part a new free memory
   free_header_t * FreeHeader = (free_header_t *) (free_region_best + n + ALLOC_HEADER_SIZE + &memory[0]);
   Header = (free_header_t *) (free_region_best + &memory[0]);
   //if the best region is the first free region
   if (free_list_ptr == free_region_best){
      free_list_ptr = free_region_best + n + ALLOC_HEADER_SIZE;
   }
   checkFreeHeader(Header);
   FreeHeader->magic = MAGIC_FREE;
   FreeHeader->size = Header->size - n - ALLOC_HEADER_SIZE;
   if (Header->next == free_region_best){
      FreeHeader->next = free_region_best + n + ALLOC_HEADER_SIZE;
   } else {
      FreeHeader->next = Header->next;
      free_header_t * NextHeader = (free_header_t *) (Header->next + &memory[0]);
      NextHeader->prev = free_region_best + n + ALLOC_HEADER_SIZE;
   }
   if (Header->prev == free_region_best){
      FreeHeader->prev = free_region_best + n + ALLOC_HEADER_SIZE;
   } else {         
      FreeHeader->prev = Header->prev;
      free_header_t * PrevHeader = (free_header_t *) (Header->prev + &memory[0]);
      PrevHeader->next = free_region_best + n + ALLOC_HEADER_SIZE;
   }
   checkFreeHeader(FreeHeader);
   alloc_header_t * AllocHeader = (alloc_header_t *) (free_region_best + &memory[0]);
   AllocHeader->magic = MAGIC_ALLOC;
   AllocHeader->size = n + ALLOC_HEADER_SIZE;
   checkAllocHeader(AllocHeader);
   return (void *) (AllocHeader + 1);
}








// Input: object, a pointer.
// Output: none
// Precondition: object points to a location immediately after a header block
//               within the allocator's memory.
// Postcondition: The region pointed to by object has been placed in the free
//                list, and merged with any adjacent free blocks; the memory
//                space can be re-allocated by vlad_malloc

void vlad_free(void *object)
{
   if ((byte *)object < &memory[0] || (byte *)object > &memory[memory_size]){
      fprintf(stderr, "vlad_free: Attempt to free via invalid pointer\n");
      exit(EXIT_FAILURE);
   }

   alloc_header_t * AllocHeader = (alloc_header_t *) object;
   AllocHeader -= 1;
   if (AllocHeader->magic !=  MAGIC_ALLOC){
      fprintf(stderr, "vlad_free: Attempt to free non-allocated memory\n");
      exit(EXIT_FAILURE);
   }
   vsize_t region_size = AllocHeader->size;
   vaddr_t position = (byte *)AllocHeader - &memory[0];
   vlink_t Prev;
   vlink_t Next;

   if (free_list_ptr < position){
      free_header_t * CurrHeader = (free_header_t *) (free_list_ptr + &memory[0]);
      checkFreeHeader(CurrHeader);
      Prev = free_list_ptr;
      Next = free_list_ptr;
      vaddr_t Curr = free_list_ptr;
      Curr = CurrHeader->next;
      CurrHeader = (free_header_t *) (Curr + &memory[0]);
      
      while (Curr != free_list_ptr && Prev == Next){
      
         if (Curr < position){
            Prev = Curr;
            Next = Curr;
         }
         if (Curr > position){
            Next = Curr;
         }
         Curr = CurrHeader->next;
         CurrHeader = (free_header_t *) (Curr + &memory[0]);
         checkFreeHeader(CurrHeader);
      }
      //curr == free_lisr_ptr
      free_header_t * PrevHeader = (free_header_t *) (Prev + &memory[0]);
      PrevHeader->next = position;
      free_header_t * NextHeader = (free_header_t *) (Next + &memory[0]);
      NextHeader->prev = position;
      free_header_t * FreeHeader = (free_header_t *) AllocHeader;
      FreeHeader->magic = MAGIC_FREE;
      FreeHeader->size = region_size;
      FreeHeader->prev = Prev;
      FreeHeader->next = Next; 
   }
   
   if (free_list_ptr > position){
      free_header_t * CurrHeader = (free_header_t *) (free_list_ptr + &memory[0]);
      checkFreeHeader(CurrHeader);
      Next = free_list_ptr;
      Prev = CurrHeader->prev;
      CurrHeader->prev = position;
      free_header_t * PrevHeader = (free_header_t *) (Prev + &memory[0]);
      PrevHeader->next = position;
      free_header_t * FreeHeader = (free_header_t *) AllocHeader;
      FreeHeader->magic = MAGIC_FREE;
      FreeHeader->size = region_size;
      FreeHeader->prev = Prev;
      FreeHeader->next = Next;
      //move free_list_ptr to first free region
      free_list_ptr = position;      
   }
   
   
   vlad_merge();
}







// Input: current state of the memory[]
// Output: new state, where any adjacent blocks in the free list
//            have been combined into a single larger block; after this,
//            there should be no region in the free list whose next
//            reference is to a location just past the end of the region

static void vlad_merge()
{

   vaddr_t Curr = free_list_ptr;
   free_header_t * CurrHeader = (free_header_t *) (Curr + &memory[0]);
   vaddr_t Next = CurrHeader->next;
   free_header_t * NextHeader = (free_header_t *) (Next + &memory[0]);
   //if the first and second free region need merge
   while (Curr + CurrHeader->size == CurrHeader->next){
      checkFreeHeader(CurrHeader);
      checkFreeHeader(NextHeader);
      CurrHeader->next = NextHeader->next;
      CurrHeader->size = CurrHeader->size + NextHeader->size;
      if (CurrHeader->prev == Next){
         CurrHeader->prev = CurrHeader->next;
      }
      Next = CurrHeader->next;
      NextHeader = (free_header_t *) (Next + &memory[0]);
   } 
   
   //now the first and second don't need merge
      Curr = Next;
      CurrHeader = NextHeader;
      Next = CurrHeader->next;
      NextHeader = (free_header_t *) (Next + &memory[0]);

   //doing the merge second or more time
   while (Curr != free_list_ptr){//this loop is to shift to next region need merge
      
      while (Curr + CurrHeader->size == CurrHeader->next){//this loop is merge the curr region with next until they are not adjcent
         
         checkFreeHeader(CurrHeader);
         checkFreeHeader(NextHeader);
         CurrHeader->next = NextHeader->next;
         CurrHeader->size = CurrHeader->size + NextHeader->size;
         if (CurrHeader->prev == Next){
            CurrHeader->prev = CurrHeader->next;
         }
         Next = CurrHeader->next;
         NextHeader = (free_header_t *) (Next + &memory[0]);
      }
      //fprintf(stderr, "Stage 2 ");
      Curr = Next;
      CurrHeader = NextHeader;
      Next = CurrHeader->next;
      NextHeader = (free_header_t *) (Next + &memory[0]);
   }
}







// Stop the allocator, so that it can be init'ed again:
// Precondition: allocator memory was once allocated by vlad_init()
// Postcondition: allocator is unusable until vlad_int() executed again

void vlad_end(void)
{
   free(memory);
   memory = NULL;
   free_list_ptr = (vaddr_t)0;
   memory_size = 0;
   strategy = BEST_FIT;
}







// Precondition: allocator has been vlad_init()'d
// Postcondition: allocator stats displayed on stdout

void vlad_stats(void)
{
   // TODO
   // put whatever code you think will help you
   // understand Vlad's current state in this function
   // REMOVE all of these statements when your vlad_malloc() is done
   printf("vlad_stats() won't work until vlad_malloc() works\n");
   return;
}

static void checkFreeHeader(free_header_t * Header)
{
   if (Header->magic !=  MAGIC_FREE){
      fprintf(stderr, "vald_alloc: Memory corruption\n");
      exit(EXIT_FAILURE);
   }
}

static void checkAllocHeader(alloc_header_t * Header)
{
   if (Header->magic !=  MAGIC_ALLOC){
      fprintf(stderr, "vald_alloc: Memory corruption\n");
      exit(EXIT_FAILURE);
   }
}
