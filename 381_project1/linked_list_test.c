#include "Ordered_container.h"
#include "stdio.h"
#include "stdlib.h"
#include "Ordered_container_list.c"

static int g_num_things = 0;

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

    printf("size = %d\n", OC_get_size(container_ptr));
    printf("Empty = %d\n", OC_empty(container_ptr));
    Print_container(container_ptr, sizeof(int));

    int search_num = 5;
    int* data_ptr = OC_find_item(container_ptr, &search_num);
    printf("Found data: %d\n", *data_ptr);

    printf("Things: %d", g_num_things);
    OC_apply(container_ptr, &delete_thing);
    OC_clear(container_ptr);
    printf("Things: %d", g_num_things);
    printf("size = %d\n", OC_get_size(container_ptr));
    printf("Empty = %d\n", OC_empty(container_ptr));
    Print_container(container_ptr, sizeof(int));

    OC_destroy_container(container_ptr);

    return 0;
}
