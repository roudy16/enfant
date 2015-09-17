#include "Ordered_container.h"
#include "stdio.h"
#include "stdlib.h"


/* struct LL_Node structure declaration. This declaration is local to this file.
This is a two-way or doubly-linked list. Each node has a pointer to the previous
node and another pointer to the next node in the list. This means insertions or
removals of a node can be made in constant time, once the location has been
determined. */
struct LL_Node {
    struct LL_Node* prev;      /* pointer to the previous node */
	struct LL_Node* next;		/* pointer to the next node */
	void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to this file.
A pointer is maintained to the last node in the list as well as the first,
meaning that additions to the end of the list can be made in constant time.
The number of nodes in the list is kept up-to-date in the size member
variable, so that the size of the list can be accessed in constant time. */
struct Ordered_container {
	OC_comp_fp_t comp_func;
	struct LL_Node* first;
	struct LL_Node* last;
	int size;
};

typedef struct Compare_result_pair {
    struct LL_Node* node_ptr;
    int             comp_result;
} Comp_pair_t;


int g_Container_count = 0;
int g_Container_items_in_use = 0;
int g_Container_items_allocated = 0;

inline static void Init_container_helper(struct Ordered_container *const c_ptr){
    c_ptr->first = NULL;
    c_ptr->last = NULL;
    c_ptr->size = 0;
}

// Deletes the nodes in a container but does not reset container size
static void Clear_container_helper(struct Ordered_container *const c_ptr){
    struct LL_Node* node_ptr = c_ptr->first;
    struct LL_Node* node_to_delete_ptr = NULL;

    while ( (node_to_delete_ptr = node_ptr) ){
        node_ptr = node_ptr->next;
        free(node_to_delete_ptr);
    }

    g_Container_items_allocated -= c_ptr->size;
    g_Container_items_in_use -= c_ptr->size;
}

// Finds first node with data that is evaluated as greater than or equal to the
// node passed in.
// Requires that c_ptr points to a container
static const Comp_pair_t Find_first_node_greater_equal(const void* const data_ptr,
                                                       const struct Ordered_container *const c_ptr,
                                                       const OC_comp_fp_t comp_func)
{
    Comp_pair_t comp_pair = { c_ptr->first, 0 };
    while (comp_pair.node_ptr){
        comp_pair.comp_result = comp_func(data_ptr, comp_pair.node_ptr->data_ptr);
        if ( comp_pair.comp_result > 0){
            comp_pair.node_ptr = comp_pair.node_ptr->next;
        } else {
            break;
        }
    }
    return comp_pair;
}

static void Insert_before(struct LL_Node *const insert_node_ptr,
                          struct LL_Node *const insert_before_node_ptr,
                          struct Ordered_container *const c_ptr)
{
    insert_node_ptr->next = insert_before_node_ptr;
    insert_node_ptr->prev = insert_before_node_ptr->prev;
    insert_before_node_ptr->prev = insert_node_ptr;

    // The invariant that the first node must have a NULL prev is implicit here
    // Perhaps I should assert that invariant
    if (insert_before_node_ptr == c_ptr->first){
         c_ptr->first = insert_node_ptr;
    } else {
        insert_node_ptr->prev->next = insert_node_ptr;
    }
}

static void* Find_helper(const struct Ordered_container* c_ptr, const void* data_ptr,
                         const OC_comp_fp_t comp_func_ptr)
{
    const Comp_pair_t comp_pair = Find_first_node_greater_equal(data_ptr,
                                                                c_ptr,
                                                                comp_func_ptr);

    if (!comp_pair.node_ptr){
        return comp_pair.node_ptr;
    }

    if (comp_pair.comp_result == 0){
        return comp_pair.node_ptr;
    } else {
        return (void*)NULL;
    }
}

/* DEBUGGING FUNCTIONS */
#ifdef DEBUG

void Print_node(const struct LL_Node *const node_ptr, const int data_size){
    printf("My address: %p, ", (void*)node_ptr);
    printf("Prev address: %p, ", (void*)node_ptr->prev);
    printf("Next address: %p, ", (void*)node_ptr->next);
    printf("Data address: %p\n", (void*)node_ptr->data_ptr);
    printf("Data: 0x");
    for (int i = data_size - 1; i >= 0; --i){
        printf("%x%x", (((char*)node_ptr->data_ptr)[i] & 0xF0) >> 4,
                       ((char*)node_ptr->data_ptr)[i] & 0xF);
    }
    printf("\n");
}

void Print_container(const struct Ordered_container *const c_ptr, const int data_size){
    printf("Container address: %p, ", (void*)&*c_ptr);
    printf("First addres: %p, ", (void*)c_ptr->first);
    printf("Last address: %p, ", (void*)c_ptr->last);
    printf("Container size: %d\n", c_ptr->size);

    struct LL_Node* node_ptr = c_ptr->first;
    while(node_ptr){
        Print_node(node_ptr, data_size);
        node_ptr = node_ptr->next;
    }
}

#endif // DEBUGGING FUNCTIONS

struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr) {
    struct Ordered_container* new_container_ptr = malloc(sizeof(struct Ordered_container));

    if (!new_container_ptr) {
        printf("Could not allocate memory for new Ordered_container\n");
    }
    else {
        ++g_Container_count;
        new_container_ptr->comp_func = f_ptr;
        Init_container_helper(new_container_ptr);
    }

    return new_container_ptr;
}

void OC_destroy_container(struct Ordered_container* c_ptr){
    Clear_container_helper(c_ptr);
    --g_Container_count;
    free(c_ptr);
}

void OC_clear(struct Ordered_container* c_ptr){
    Clear_container_helper(c_ptr);
    Init_container_helper(c_ptr);
}

int OC_get_size(const struct Ordered_container* c_ptr){
    return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr){
    return c_ptr->size == 0;
}

void* OC_get_data_ptr(const void* item_ptr){
    return ((struct LL_Node*)item_ptr)->data_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr){
    if (!item_ptr || !c_ptr){
        return;
    }

    struct LL_Node *const node_ptr = item_ptr;
    if (node_ptr != c_ptr->first){
        node_ptr->prev->next = node_ptr->next;
    } else {
        c_ptr->first = node_ptr->next;
    }

    if (node_ptr != c_ptr->last){
        node_ptr->next->prev = node_ptr->prev;
    } else {
        c_ptr->last = node_ptr->prev;
    }

    --c_ptr->size;
    free(node_ptr);
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr){
    struct LL_Node *const node_ptr = malloc(sizeof(struct LL_Node));
    node_ptr->data_ptr = data_ptr;

    if (OC_empty(c_ptr)){
        c_ptr->first = node_ptr;
        c_ptr->last = node_ptr;
    } else {
        Comp_pair_t comp_pair = Find_first_node_greater_equal(data_ptr, c_ptr, c_ptr->comp_func);
        struct LL_Node* insert_before_node_ptr = comp_pair.node_ptr;

        if (insert_before_node_ptr){
            Insert_before(node_ptr, insert_before_node_ptr, c_ptr);
        } else {
            c_ptr->last->next = node_ptr;
            node_ptr->prev = c_ptr->last;
            node_ptr->next = NULL;
            c_ptr->last = node_ptr;
        }
    }
    ++c_ptr->size;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr){
    return Find_helper(c_ptr, data_ptr, c_ptr->comp_func);
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr,
                       const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
    return Find_helper(c_ptr, arg_ptr, fafp);
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp){
    struct LL_Node* node_ptr = c_ptr->first;

    while (node_ptr){
        afp(node_ptr->data_ptr);
        node_ptr = node_ptr->next;
    }
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
    struct LL_Node* node_ptr = c_ptr->first;

    while (node_ptr){
        const int ret_val = afp(node_ptr->data_ptr);
        if (ret_val != 0){
            return ret_val;
        }
        node_ptr = node_ptr->next;
    }

    return 0;
}

void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr){
    struct LL_Node* node_ptr = c_ptr->first;

    while (node_ptr){
        afp(node_ptr->data_ptr, arg_ptr);
        node_ptr = node_ptr->next; // Could make this inline function for one point of maint
    }
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
    struct LL_Node* node_ptr = c_ptr->first;

    while (node_ptr){
        const int ret_val = afp(node_ptr->data_ptr, arg_ptr);
        if (ret_val != 0){
            return ret_val;
        }
        node_ptr = node_ptr->next;
    }

    return 0;
}

