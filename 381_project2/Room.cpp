#include "Meeting.h"
#include "Room.h"
#include "Utility.h"
#include <fstream>
#include <ostream>

using std::flush;
using std::endl;
using std::ostream;
using std::ifstream;

using People_list_t = const Ordered_list < const Person*, Less_than_ptr<const Person*> > ;

Room::Room(std::ifstream& is, People_list_t& people_list) {

}

void Room::add_Meeting(const Meeting& m) {
    if (is_Meeting_present(m.get_time())) {
        // TODO throw error
    }
    else {
        meetings.insert(m);
    }
}

bool Room::is_Meeting_present(int time) const {
    auto iter = meetings.find(Meeting(time));
    return iter != meetings.end();
}
