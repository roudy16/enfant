/* skeleton file for Ordered_container_array.c
The struct declaration below must be used for the Ordered_container object.
Remove this comment and complete this file with all necessary code.
*/

/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container {
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items currently in the array */
};

struct Ordered_container cont;
cont.a
