/*************************************
* Lab 4 Exercise 1
* Name: Sriram Sami
* Student No: A0138978E
* Lab Group: 7
*************************************
*Warning: Make sure your code works on
lab machine (Linux on x86)
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define FREE 0      //To represent a free partition
#define OCCUPIED 1  //To represent an occupied partition

#define ALLOCATE 1
#define DEALLOCATE 2

#define INVALID_ADDRESS UINT_MAX

//Declaration of a Linked List Node
//  for handling partition information

//Note: You are free to rewrite / redesign in anyway

typedef struct BLOCKSTRUCT{
    unsigned int address;       //starting address
    struct BLOCKSTRUCT* next;
} block;

//For easy access of power of 2s. POWEROF2[IDX] gives 2^IDX
// Upto 4096 only :-)
int POWEROF2[] = {1, 2, 4, 8, 16, 32, 64,
                    128, 256, 512, 1024, 2048, 4096};

//Helper Functions written for you
unsigned int pOf2Ceiling( unsigned int N );
// Find the smallest S, such that 2&S >= N
//   S is returned

unsigned int buddyOf( unsigned int , unsigned int );
//Return the address of the buddy block, at a particular power of 2


// Given the buddy allocation array - allocates a block of the power of two given the starting address
void createBlock(block freeBlockArray[], unsigned int powerOfTwo, unsigned int startingAddress) {
    //printf("Creating block for powerOfTwo: %d and starting addr: %d\n", powerOfTwo, startingAddress);
   // Access the first block in the freeBlockArray level specified
   block* currentBlock = &freeBlockArray[powerOfTwo];

   // Keep going until we reach an invalid block
   // Assumption - there will always be a last block with a NULL next pointer and INVALID address
   while (currentBlock->address != INVALID_ADDRESS) {
       currentBlock = currentBlock->next;
   }

   // We have reached an allocatable block
   // Set the correct starting address
   currentBlock->address = startingAddress;

   // Create a new dummy block
   block* endBlock = (block*) malloc(sizeof(block));
   endBlock->address = INVALID_ADDRESS;
   endBlock->next = NULL;

   // Add it to the end
   currentBlock->next = endBlock;
}

void printBuddyArrayRow(block* buddyRow) {
    // If the first element is invalid, print EMPTY
    if (buddyRow->address == INVALID_ADDRESS) {
        printf("EMPTY");
    } else {
        // Print every block address
        while (buddyRow->address != INVALID_ADDRESS) {
            printf("[%d] ", buddyRow->address);
            buddyRow = buddyRow->next;
        }
    }
}

void printBuddyArray(block freeBlockArray[], int numElements) {
    for (int i = numElements - 1; i >= 0; i--) {
        printf("A[%d]: ", i);
        printBuddyArrayRow(&freeBlockArray[i]);
        printf("\n");
    }

}

// Checks if the block address is invalid
int isValidBlock(block* blockToCheck) {
    return blockToCheck->address != INVALID_ADDRESS;
}


// Checks if there is a free block at the level requested
int isThereFreeBlockAt(int powerOfTwo, block freeBlockArray[]) {
    return isValidBlock(&freeBlockArray[powerOfTwo]);
}

// Assumes there is a block here and it is splittable
void splitBlockAt(unsigned powerOfTwoToSplit, block freeBlockArray[]) {
    printf("Splitting block at %d, to move down to location %d\n", powerOfTwoToSplit, powerOfTwoToSplit - 1);

    // Get the first block at that power of two
    block* blockToSplit = &freeBlockArray[powerOfTwoToSplit];

    // Print the buddy's value, it's -1 because we're finding the buddy address at a lower power
    unsigned int buddyAddress = buddyOf(blockToSplit->address, powerOfTwoToSplit - 1);
    printf("Buddy address for new buddy block = %d\n", buddyAddress);

}

int splitBlocksFor(unsigned int powerOfTwoToAllocate, block freeBlockArray[], unsigned int largestAllocSize) {
    // If we've exceeded the largest allocatable size, return -1;
    if (powerOfTwoToAllocate >= largestAllocSize) {
        return -1;
    }

    // Otherwise, try to split the next highest level
    if (!isThereFreeBlockAt(powerOfTwoToAllocate + 1, freeBlockArray)) {
        // No free block directly above.
        // We need to try to get a free block at the next highest level
        int success = splitBlocksFor(powerOfTwoToAllocate + 1, freeBlockArray, largestAllocSize);

        if (!success) {
            // We couldn't split a block at the higher level - we can't recover from this
            printf("Cannot allocate block at: %d - allocation failure!\n", powerOfTwoToAllocate + 1);

            return -1;
        }

    }

    // If we have reached here - we definitely have a block at the next higher level we can split
    printf("We can split block at level: %d\n", powerOfTwoToAllocate + 1);
    // Actually split the block
    splitBlockAt(powerOfTwoToAllocate+1, freeBlockArray);

    return -1; //for now
}


int doAllocate(int requestType, int size, block freeBlockArray[], block* allocatedBlockList,
        unsigned int smallestAllocSize, unsigned int largestAllocSize ) {
    if (size <= 0) return -1;
    /*
       ALLOCATION ALGORITHM
       1. Find the power of two that is closest to size
       2. Check if there is a free block at that power
       2a. If there IS A FREE BLOCK (allocateBlock)
        - Remove it from the free array
        - Mark it as allocated
       2b. If there is NO FREE BLOCK
         - Attempt to split a block from level above this one into this level (splitBlockAbove)
         - Do 2a
    */

    unsigned int powerOfTwoToAllocate = pOf2Ceiling(size);


    // Check if the block request is larger than possible
    if (powerOfTwoToAllocate > largestAllocSize) {
        printf("No valid block of size 2^%d, too large to accomodate request!\n", powerOfTwoToAllocate);
        return -1;
    }

    // See if we need to split blocks to get a free block at this level
    if (!isThereFreeBlockAt(powerOfTwoToAllocate, freeBlockArray)) {
        printf("No valid block of size 2^%d yet - splitting blocks above!\n", powerOfTwoToAllocate);

        // Recursively split higher blocks.
        int success = splitBlocksFor(powerOfTwoToAllocate, freeBlockArray, largestAllocSize);

        if (!success) {
            return -1;
        }
    }

    // Otherwise: can allocate
    printf("Valid block of size 2^%d\n", powerOfTwoToAllocate);

    return 0;



    // Checks if the block address is invalid
}


int dispatchRequest(int requestType, int size, block freeBlockArray[], block* allocatedBlockList,
        unsigned int smallestAllocSize, unsigned int largestAllocSize) {
    if (requestType == ALLOCATE) {
        return doAllocate(requestType, size, freeBlockArray, allocatedBlockList, smallestAllocSize, largestAllocSize);
    } /*else if (requestType == DEALLOCATE) {
        return doDeallocate(requestType, size, freeBlockArray);
    } */else {
        printf ("Incorrect requestType %d received\n", requestType);
        return -1;
    }
}


int main(int argc, char** argv)
{

    unsigned int initMemSize, smallestAllocSize, largestAllocSize;

    //printf("Initial Free Size (MS): ");
    scanf("%d", &initMemSize); // 0 < MS <= 4096
    //printf("Smallest Allocatable size in log_2: ");
    scanf("%d", &smallestAllocSize);
    //printf("Largest Allocatable size in log_2: ");
    scanf("%d", &largestAllocSize);

    // Array of blocks - each one can be null
    block freeBlockArray[largestAllocSize + 1];

    // Array of allocated blocks
    block* allocatedBlockList = NULL;

    // Initialize all blocks to sentinel values
    for (int i = 0; i < largestAllocSize + 1; i++) {
        freeBlockArray[i].address = INVALID_ADDRESS;
        freeBlockArray[i].next = NULL;
    }

    // Print wasted space
    unsigned int wastedSize = initMemSize;
    unsigned int currentAllocSize = largestAllocSize;

    /* INITIAL ALLOCATION ALGORITHM

       Start with the current block to try allocating as the largest block.
       If we can allocate the block (div != 0), add one address block to freeBlockArray[currentBlock]

       Else, reduce to a lower allocatable size as long as we don't drop below the smallest allocatable size

       Once we can allocate a block: go to parent and pick first block we see. Create a new block halfway between its starting address and its
       starting address + size
    */

    // Keep track of the starting address of the next block we will allocate
    int currentAddress = 0;
    while (currentAllocSize >= smallestAllocSize) {
        if (wastedSize / POWEROF2[currentAllocSize] != 0) {
            // We can allocate one block of this size
            createBlock(freeBlockArray, currentAllocSize, currentAddress);
            currentAddress += POWEROF2[currentAllocSize];
            wastedSize -= POWEROF2[currentAllocSize];
        } else {
            currentAllocSize--;
        }
    }

    // Wasted size
    printf("%d\n", wastedSize);

    //Print out the arrays
    printBuddyArray(freeBlockArray, largestAllocSize + 1);

    // Ask for total number of requests
    int totalRequests;
    scanf("%d", &totalRequests);

    // Read in every request and dispatch
    for (int i = 0; i < totalRequests; i++) {
        int requestType, size, returnVal;
        scanf("%d %d", &requestType, &size);
        returnVal = dispatchRequest(requestType, size, freeBlockArray, allocatedBlockList, smallestAllocSize, largestAllocSize);
        printf("Return value: %d\n", returnVal);
    }

    return 0;
}

unsigned int pOf2Ceiling( unsigned int N )
{
    unsigned int s = 0, pOf2 = 1;

    while( pOf2 < N){
        pOf2 <<= 1;
        s++;
    }

    return s;
}

unsigned int buddyOf( unsigned int addr, unsigned int S )
{
    unsigned int mask = 0xFFFFFFFF << S;
    unsigned int buddyBit = 0x0001 << S;

    return (addr & mask) ^ buddyBit;
}
