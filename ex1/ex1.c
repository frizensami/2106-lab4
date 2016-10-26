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

#define FREE 0      //To represent a free partition
#define OCCUPIED 1  //To represent an occupied partition

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
void createBlock(block buddyArray[], unsigned int powerOfTwo, unsigned int startingAddress) {
    //printf("Creating block for powerOfTwo: %d and starting addr: %d\n", powerOfTwo, startingAddress);
   // Access the first block in the buddyArray level specified 
   block* currentBlock = &buddyArray[powerOfTwo]; 

   // Keep going until we reach an invalid block
   // Assumption - there will always be a last block with a NULL next pointer and -1 address
   while (currentBlock->address != -1) {
       currentBlock = currentBlock->next;
   }

   // We have reached an allocatable block
   // Set the correct starting address
   currentBlock->address = startingAddress;

   // Create a new dummy block
   block* endBlock = (block*) malloc(sizeof(block));
   endBlock->address = -1;
   endBlock->next = NULL;

   // Add it to the end
   currentBlock->next = endBlock;
}

void printBuddyArrayRow(block* buddyRow) {
    // If the first element is invalid, print EMPTY
    if (buddyRow->address == -1) {
        printf("EMPTY");
    } else {
        // Print every block address
        while (buddyRow->address != -1) {
            printf("[%d] ", buddyRow->address);
            buddyRow = buddyRow->next;
        }
    }
}

void printBuddyArray(block buddyArray[], int numElements) {
    for (int i = numElements - 1; i >= 0; i--) {
        printf("A[%d]: ", i);
        printBuddyArrayRow(&buddyArray[i]);
        printf("\n");
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
    block buddyArray[largestAllocSize + 1];

    // Initialize all blocks to sentinel values
    for (int i = 0; i < largestAllocSize + 1; i++) {
        buddyArray[i].address = -1;
        buddyArray[i].next = NULL;
    }

    // Print wasted space
    unsigned int wastedSize = initMemSize;
    unsigned int currentAllocSize = largestAllocSize;
    
    /* INITIAL ALLOCATION ALGORITHM

       Start with the current block to try allocating as the largest block.
       If we can allocate the block (div != 0), add one address block to buddyArray[currentBlock]

       Else, reduce to a lower allocatable size as long as we don't drop below the smallest allocatable size

       Once we can allocate a block: go to parent and pick first block we see. Create a new block halfway between its starting address and its
       starting address + size
    */

    // Keep track of the starting address of the next block we will allocate
    int currentAddress = 0;
    while (currentAllocSize >= smallestAllocSize) {
        if (wastedSize / POWEROF2[currentAllocSize] != 0) {
            // We can allocate one block of this size
            createBlock(buddyArray, currentAllocSize, currentAddress);
            currentAddress += POWEROF2[currentAllocSize];
            wastedSize -= POWEROF2[currentAllocSize];
        } else {
            currentAllocSize--;
        }
    }

    // Wasted size
    printf("%d\n", wastedSize);

    //Print out the arrays
    printBuddyArray(buddyArray, largestAllocSize + 1);

    //Clean up

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
