#include "Ordered_container.h"
#include "stdio.h"
#include "stdlib.h"

int g_num_things = 0;
/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container
{
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items currently in the array */
};

void delete_thing(void* thing_ptr){
    free(thing_ptr);
    --g_num_things;
}

void* make_thing(int val){
    int* thing_ptr = malloc(sizeof(int));
    *thing_ptr = val;
    ++g_num_things;
    return thing_ptr;
}

int thing_comp_f(const void* lhs, const void* rhs){
    return *(int*)lhs - *(int*)rhs;
}

int alt_thing_comp_f(const void* lhs, const void* rhs){
    return (*(int*)lhs + 10) - (*(int*)rhs + 10);
}

int apply_if_arg_tester(const void* data, const void* arg){
    static int count = 0;
    if (count < 5){
        printf("Apply if arg test: %d\n", *(int*)data);
        ++count;
    }
    else {
        count = 0;
        return *(int*)arg;
    }
    return 0;
}

int main(void)
{
    //OC_comp_fp_t f_ptr;
    struct Ordered_container* container_ptr = OC_create_container(&thing_comp_f);

    OC_insert(container_ptr, make_thing(5));
    OC_insert(container_ptr, make_thing(3));
    OC_insert(container_ptr, make_thing(4));
    OC_insert(container_ptr, make_thing(1));
    OC_insert(container_ptr, make_thing(2));
    OC_insert(container_ptr, make_thing(99));
    OC_insert(container_ptr, make_thing(-2));

    for (int i = 0; i < 7; ++i)
    {
        printf("Element %d: %d\n", i, *(int*)(*(container_ptr->array + i)));
    }

    printf("size = %d\n", OC_get_size(container_ptr));
    printf("Empty = %d\n", OC_empty(container_ptr));

    int search_num = 5;
    int* data_ptr = OC_find_item(container_ptr, &search_num);
    printf("Found data: %d\n", *(int*)OC_get_data_ptr(data_ptr));
    search_num = 99;
    printf("Apply_if_arg returned: %d\n", 
           OC_apply_if_arg(container_ptr, (OC_apply_if_arg_fp_t)apply_if_arg_tester, &search_num));
    data_ptr = OC_find_item_arg(container_ptr, &search_num, alt_thing_comp_f);
    printf("Found data: %d\n", *(int*)OC_get_data_ptr(data_ptr));

    printf("Things: %d\n", g_num_things);
    OC_apply(container_ptr, &delete_thing);
    OC_clear(container_ptr);
    printf("Things: %d\n", g_num_things);
    printf("size = %d\n", OC_get_size(container_ptr));
    printf("Empty = %d\n", OC_empty(container_ptr));

    OC_destroy_container(container_ptr);

    void* test = malloc(8);

    printf("Old way %p   New Way %p\n", *(void**)test, test);

    return 0;
}