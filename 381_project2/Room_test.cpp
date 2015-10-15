#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include "Person.h"
#include "Meeting.h"
#include "Room.h"
#include <iostream>
#include <fstream>

using namespace std;

template <typename T>
void print_ol_ref(const T& data, ostream& os){
    os << data << endl;
}

template <typename T>
void print_ol_p(const T* const data, ostream& os){
    os << *data << endl;
}

int main() {
    return 0;
}
