/*
   This file is a dumping ground found code I've trashed but
   that I'd still like to ask questions about 
*/

//  FROM UTILITY  #########################################

// Reads strings from file into a list of char** provided. Requires that the
// num_strings is positive and less than or equal to INPUT_BUFFER_SIZE / (MAX_INPUT + 1).
// Returns total number of strings read or a negative number indicating an
// error has taken place. Behavior is defined only if the list consists only
// of char** and num_strings == number of char** in the list.
int read_strings_from_file(FILE* infile, const int num_strings, ...);
int read_strings_from_file(FILE* infile, const int num_strings, ...){
    static char input_buffer[INPUT_BUFFER_SIZE];

    assert(num_strings <= INPUT_BUFFER_SIZE / (MAX_INPUT + 1));
    assert(num_strings > 0);
    assert(infile);

    int return_val = 0;
    int strings_read = 0;
    char* buffer_write_head = input_buffer;
    char** output_string_ptr;

    va_list arg_ptr;
    va_start(arg_ptr, num_strings);

    for (int i = 0; i < num_strings; ++i){
        return_val = fscanf(infile, "%" STRINGIFY_MACRO(MAX_INPUT) "s", buffer_write_head);

        if (return_val != 1){
            return return_val;
        }
        else {
            strings_read += return_val;
            output_string_ptr = va_arg(arg_ptr, char**);
            *output_string_ptr = buffer_write_head;
            buffer_write_head += (MAX_INPUT + 1);
        }
    }

    return strings_read;
}

// FROM MAIN  #################################################

// Returns a pointer to a Schedule object. There should only be one.
static struct Schedule *const create_schedule(void){
    static struct Schedule schedule = { NULL, NULL, 0 };

    if (!schedule.initialized){
        schedule.rooms_ptr = OC_create_container(&room_comp);
        schedule.people_ptr = OC_create_container(&person_comp);
        schedule.initialized = 1;
    }

    return &schedule;
}

static void destroy_schedule(struct Schedule* schedule_ptr){
    assert(schedule_ptr);
    OC_apply(schedule_ptr->rooms_ptr, &destroy_Room);
    OC_destroy_container(schedule_ptr->rooms_ptr);
    OC_apply(schedule_ptr->people_ptr, &destroy_Person);
    OC_destroy_container(schedule_ptr->people_ptr);
    schedule_ptr->rooms_ptr = NULL;
    schedule_ptr->people_ptr = NULL;
    schedule_ptr->initialized = 0;
}

