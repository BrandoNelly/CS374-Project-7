#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>


#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - (((x) - 1) & (ALIGNMENT - 1)))

#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))

#define PADDED_SIZE(x) ((x) + GET_PAD(x))



struct block {
    int size;            // Size in bytes
    int in_use;          // Boolean
    struct block *next;
};



void *heap = NULL;
struct block *head = NULL;

void initialize_alloc(){

    heap = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);

    int data_size = 1024 - PADDED_SIZE(sizeof(struct block));

    head = (struct block *)heap;
    head->size = data_size;
    head->in_use = 0;
    head->next = NULL;
}



void *myalloc(int size){

    if (heap == NULL){
        initialize_alloc();
    }

    size_t padded_size = PADDED_SIZE(size);

    struct block *current = head;

    while (current != NULL) {
        if (!current->in_use && (size_t)current->size >= padded_size) {
            current->in_use = 1;

            return PTR_OFFSET(current, sizeof(struct block));
        }

        current = current->next;
    }

    return NULL;

}



void print_data(void)
{
    struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}



int main(void) {
    
    void *p;

    print_data();
    p = myalloc(16);
    print_data();
    p = myalloc(16);
    printf("%p\n", p);

}