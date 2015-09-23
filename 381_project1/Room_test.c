#include "Utility.h"
#include "Ordered_container.h"
#include "Room.h"
#include "Person.h"
#include "Meeting.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int g_number_Person_structs = 0;
int g_number_Room_structs = 0;
const char* firstname1 = "Rhonda";  // 7 bytes
const char* firstname2 = "Chelsea"; // 8 bytes
const char* firstname3 = "Ryan";    // 5 bytes
const char* firstname4 = "Sarah";   // 6 bytes
const char* lastname1 = "Rousey";   // 7 bytes
const char* lastname2 = "Clinton";  // 8 bytes
const char* lastname3 = "Seacrest"; // 9 bytes
const char* lastname4 = "Murphy";   // 7 bytes
const char* phoneno1 = "xXx";       // 4 bytes
const char* phoneno2 = "411";       // 4 bytes
const char* phoneno3 = "666";       // 4 bytes
const char* phoneno4 = "312";       // 4 bytes
const int time1 = 12;
const int time2 = 9;
const int time3 = 4;
const char* topic1 = "Cartoons";
const char* topic2 = "Movies";
const char* topic3 = "Firing_Ryan";
const int room_num1 = 98799;
const int room_num2 = 1234;
const int room_num3 = 6666;

static void create_and_add_person(struct Ordered_container* const c_ptr,
                                  const char* const firstname,
                                  const char* const lastname,
                                  const char* const phoneno)
{
    assert(c_ptr);
    struct Person* new_person = create_Person(firstname, lastname, phoneno);
    assert(new_person);

    OC_insert(c_ptr, new_person);
}

static int person_to_name_comp(const char* const name_ptr,
                               const struct Person *const person_ptr)
{
    assert(person_ptr);
    assert(name_ptr);

    return strcmp(name_ptr, get_Person_lastname(person_ptr));
}

int main(void){
    FILE* file1 = fopen("room_custom1", "w");
    fprintf(file1, "77 1\n9 Killing_Ryan 3\nRousey\nMurphy\nClinton");
    file1 = freopen("room_custom1", "r", file1);
    FILE* file2 = fopen("room_save1", "w");

    struct Ordered_container* const people = OC_create_container(&person_comp);
    create_and_add_person(people, firstname1, lastname1, phoneno1);
    create_and_add_person(people, firstname2, lastname2, phoneno2);
    create_and_add_person(people, firstname3, lastname3, phoneno3);
    create_and_add_person(people, firstname4, lastname4, phoneno4);

    struct Room* room1 = create_Room(room_num1);
    struct Room* room2 = create_Room(room_num2);
    struct Room* room3 = create_Room(room_num3);

    printf("Num container: %d Num items: %d\n", g_Container_count, g_Container_items_in_use);

    struct Meeting* meeting1 = create_Meeting(time1, topic1);
    struct Meeting* meeting2 = create_Meeting(time2, topic2);
    struct Meeting* meeting3 = create_Meeting(time3, topic3);

    add_Meeting_participant(meeting1, OC_get_data_ptr(OC_find_item_arg(people, lastname1, &person_to_name_comp)));
    add_Meeting_participant(meeting1, OC_get_data_ptr(OC_find_item_arg(people, lastname2, &person_to_name_comp)));
    add_Meeting_participant(meeting1, OC_get_data_ptr(OC_find_item_arg(people, lastname3, &person_to_name_comp)));
    add_Meeting_participant(meeting1, OC_get_data_ptr(OC_find_item_arg(people, lastname4, &person_to_name_comp)));
    add_Room_Meeting(room1, meeting1);
    print_Room(room1);
    remove_Meeting_participant(meeting1, OC_get_data_ptr(OC_find_item_arg(people, lastname1, &person_to_name_comp)));
    print_Meeting(find_Room_Meeting(room1, time1));
    printf("String Bytes: %d  Allocated items: %d\n", g_string_memory, g_Container_items_allocated);

    clear_Room(room1);
    print_Room(room1);
    save_Room(room1, file2);
    struct Room* room4 = load_Room(file1, people);
    print_Room(room4);

    FILE* file5 = fopen("meeting_custom1.txt", "w");
    fprintf(file5, "10 Chocolate 2\nClinton Rousey");
    fflush(file5);
    fclose(file5);
    FILE* file3 = fopen("meeting_custom1.txt", "r");
    meeting3 = load_Meeting(file3, people);
    printf("String Bytes: %d  Allocated items: %d\n", g_string_memory, g_Container_items_allocated);
    fclose(file2);

    add_Room_Meeting(room1, meeting2);
    add_Room_Meeting(room1, meeting3);

    print_Room(room1);
    FILE* file6 = fopen("room_save2.txt", "w");
    save_Room(room1, file6);
    fclose(file6);
    return 0;
}