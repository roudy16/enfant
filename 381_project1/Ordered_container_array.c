#include "Ordered_container.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INITIAL_ARRAY_SIZE 3

/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container
{
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items currently in the array */
};

// Used to store information about the result of a search within container
struct Search_result
{
    void** item_ptr;
    int item_found;
};

int g_Container_count = 0;
int g_Container_items_in_use = 0;
int g_Container_items_allocated = 0;


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// Performs binary search of the container for the item that compares equal
// to the passed in data_ptr using the passed in compare function. Returns
// NULL if none found.
static void* Find_helper(const struct Ordered_container* const c_ptr,
                         const void* data_ptr,
                         OC_comp_fp_t comp_func);

// Shifts elements in the array starting at the space_ptr and working to the end.
// This will overwrite the void* currently at space_ptr.
// Requires space_ptr points inside the active area of the array.
static void Shift_array_left(const struct Ordered_container *const c_ptr, void** space_ptr);

// Moves all data one space to the right starting at the location of
// the spot_to_be_empty pointer. When this function return the area pointed to
// by spot_to_be_empty can be treated as empty and data can be safely written there.
// Requires enough memory allocated to hold all active elements plus one.
static void Shift_array_right(const struct Ordered_container *const c_ptr,
                              void** const spot_to_be_empty_ptr);

// Perform binary search in container to find item that matches data_ptr.
// Returns item found and pointer to that item if it is found, otherwise
// returns item not found and pointer to place where data_ptr would be
// inserted into container.
static struct Search_result Find_element(const void* const data_ptr,
                                         const struct Ordered_container *const c_ptr,
                                         OC_comp_fp_t comp_func);

// Allocates initial array, sets appropriate values to container
static void Init_container_helper(struct Ordered_container *const c_ptr);

// Deallocate container array
static void Clear_container_helper(struct Ordered_container *const c_ptr);

// Copies old container array into a new one of size = 2 * (old_size + 1)
static void Grow_array(struct Ordered_container *const c_ptr);


/* #################### */
/* FUNCTION DEFINITIONS */
/* #################### */


static void Shift_array_left(const struct Ordered_container *const c_ptr, void** space_ptr)
{
    assert(space_ptr >= c_ptr->array);

    void** const array_last_element = c_ptr->array + c_ptr->size - 1;
    assert(space_ptr <= array_last_element);

    while (space_ptr < array_last_element)
    {
        *space_ptr = *(space_ptr + 1);
        ++space_ptr;
    }
}

static void Shift_array_right(const struct Ordered_container *const c_ptr,
                              void** const spot_to_be_empty_ptr)
{
    assert(c_ptr->size < c_ptr->allocation);
    assert(spot_to_be_empty_ptr >= c_ptr->array);

    void** shift_to_ptr = c_ptr->array + c_ptr->size;
    assert(spot_to_be_empty_ptr <= shift_to_ptr);

    while (shift_to_ptr > spot_to_be_empty_ptr)
    {
        *shift_to_ptr = *(shift_to_ptr - 1);
        --shift_to_ptr;
    }
}

static void Init_container_helper(struct Ordered_container *const c_ptr)
{
    c_ptr->array = malloc(sizeof(void*) * INITIAL_ARRAY_SIZE);

    if (!c_ptr->array)
    {
        perror("Failed to allocate memory in Init_container_helper\n");
        exit(EXIT_FAILURE);
    }

    c_ptr->allocation = INITIAL_ARRAY_SIZE;
    c_ptr->size = 0;
    g_Container_items_allocated += INITIAL_ARRAY_SIZE;
}

static void Clear_container_helper(struct Ordered_container *const c_ptr)
{
    free(c_ptr->array);
    g_Container_items_allocated -= c_ptr->allocation;
    g_Container_items_in_use -= c_ptr->size;
}

static void Grow_array(struct Ordered_container *const c_ptr)
{
    assert(c_ptr->size == c_ptr->allocation);

    const int old_size = c_ptr->size;
    void** const old_array = c_ptr->array;
    const int number_new_elements = 2 * (old_size + 1);
    void** const new_array = malloc(sizeof(void*) * number_new_elements);

    if (!new_array)
    {
        perror("Could not allocate memory to grow array\n");
        exit(EXIT_FAILURE);
    }

    g_Container_items_allocated += number_new_elements;

    for (int i = 0; i < old_size; ++i)
    {
        new_array[i] = old_array[i];
    }

    free(old_array);
    g_Container_items_allocated -= old_size;
    c_ptr->array = new_array;
    c_ptr->allocation = number_new_elements;
}

static struct Search_result Find_element(const void* const data_ptr,
                                         const struct Ordered_container *const c_ptr,
                                         OC_comp_fp_t comp_func)
{
    assert(c_ptr);

    struct Search_result result = { NULL, 0 };

    void** const array_base = c_ptr->array;
    int low = 0;
    int high = c_ptr->size - 1;

    while (low <= high)
    {
        const int mid = (low + high) / 2;
        const int comp_return = comp_func(data_ptr, array_base[mid]);
        if (comp_return < 0)
        {
            high = mid - 1;
        }
        else if (comp_return > 0)
        {
            low = mid + 1;
        }
        else
        {
            result.item_ptr = array_base + mid;
            result.item_found = 1;
            return result;
        }
    }

    result.item_ptr = array_base + high + 1;
    return result;
}

struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
    assert(f_ptr);
    struct Ordered_container* new_container_ptr = malloc(sizeof(struct Ordered_container));

    if (!new_container_ptr)
    {
        perror("Could not allocate memory in OC_create_container\n");
        exit(EXIT_FAILURE);
    }

    new_container_ptr->comp_fun = f_ptr;
    Init_container_helper(new_container_ptr);
    ++g_Container_count;

    return new_container_ptr;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
    assert(c_ptr);
    Clear_container_helper(c_ptr);
    --g_Container_count;
    free(c_ptr);
}

void OC_clear(struct Ordered_container* c_ptr)
{
    assert(c_ptr);
    Clear_container_helper(c_ptr);
    Init_container_helper(c_ptr);
}

int OC_get_size(const struct Ordered_container* c_ptr)
{
    assert(c_ptr);
    return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr)
{
    assert(c_ptr);
    return c_ptr->size == 0;
}

void* OC_get_data_ptr(const void* item_ptr)
{
    assert(item_ptr);
    return *(void**)item_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
    assert(c_ptr);
    assert(item_ptr);

    Shift_array_left(c_ptr, (void**)item_ptr);
    --c_ptr->size;
    --g_Container_items_in_use;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
    if (c_ptr->size == c_ptr->allocation)
    {
        Grow_array(c_ptr);
    }

    // Find pointer to item that data_ptr should be inserted before
    struct Search_result search_result = Find_element(data_ptr, c_ptr, c_ptr->comp_fun);
    void** const insert_spot = search_result.item_ptr;

    // Move elements from insert_spot after to right one in array
    Shift_array_right(c_ptr, insert_spot);

    *insert_spot = data_ptr;
    ++c_ptr->size;
    ++g_Container_items_in_use;
}

static void* Find_helper(const struct Ordered_container* const c_ptr,
                         const void* data_ptr,
                         OC_comp_fp_t comp_func)
{
    struct Search_result search_result = Find_element(data_ptr, c_ptr, comp_func);

    if (search_result.item_found)
    {
        return search_result.item_ptr;
    }
    else
    {
        return NULL;
    }
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
    return Find_helper(c_ptr, data_ptr, c_ptr->comp_fun);
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr, const void* arg_ptr,
                       OC_find_item_arg_fp_t fafp)
{
    return Find_helper(c_ptr, arg_ptr, fafp);
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
    void** const array_base = c_ptr->array;
    const int array_size = c_ptr->size;

    // Call function with every item in array
    for (int i = 0; i < array_size; ++i)
    {
        afp(array_base[i]);
    }
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
    void** const array_base = c_ptr->array;
    const int array_size = c_ptr->size;

    // Call function with every item in array until return non-zero
    for (int i = 0; i < array_size; ++i)
    {
        const int ret_val = afp(array_base[i]);
        if (ret_val != 0)
        {
            return ret_val;
        }
    }

    return 0;
}

void OC_apply_arg(const struct Ordered_container* c_ptr,
                  OC_apply_arg_fp_t afp,
                  void* arg_ptr)
{
    void** const array_base = c_ptr->array;
    const int array_size = c_ptr->size;

    // Call function with every item in array
    for (int i = 0; i < array_size; ++i)
    {
        afp(array_base[i], arg_ptr);
    }
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp,
                    void* arg_ptr)
{
    void** const array_base = c_ptr->array;
    const int array_size = c_ptr->size;

    // Call function with every item in array until return non-zero
    for (int i = 0; i < array_size; ++i)
    {
        const int ret_val = afp(array_base[i], arg_ptr);
        if (ret_val != 0)
        {
            return ret_val;
        }
    }

    return 0;
}
