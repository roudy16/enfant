#include "String.h"
#include "Utility.h"
#include <cstring>
#include <cassert>

using namespace std;

char String::a_null_byte = '\0';
int String::number = 0;
int String::total_allocation = 0;
bool String::messages_wanted = false;

String::String(const char* cstr_) {
    const size_t len = strlen(cstr_);
    if (len == 0){
        m_length = 0;
        mp_cstring = &a_null_byte;
    }
    else {
        mp_cstring = new char[len + 1];
        strncpy(mp_cstring, cstr_, len + 1);
        m_length = static_cast<int>(len);
        total_allocation += (m_length + 1);
    }

    ++number;
}

String::String(const String& original) : String(original.mp_cstring) {}

String::String(String&& rhs) noexcept {
    mp_cstring = rhs.mp_cstring;
    m_length = rhs.m_length;
    rhs.mp_cstring = &a_null_byte;
    rhs.m_length = 0;
    ++number;
}

String::~String() noexcept {
    if (m_length != 0) {
        assert(mp_cstring != &a_null_byte);
        delete[] mp_cstring;
        total_allocation -= (m_length + 1);
    }

    --number;
}

String& String::operator= (const String& rhs) {
    if (this != &rhs){
        String new_string(rhs);
        swap(new_string);
    }

    return *this;
}

String& String::operator= (const char* rhs) {
    if (this->mp_cstring != rhs){
        String new_string(rhs);
        swap(new_string);
    }

    return *this;
}

String& String::operator= (String&& rhs) noexcept {
    swap(rhs);
    return *this;
}

void String::swap(String& other) noexcept {
    if (this != &other){
        char* temp_cstring = mp_cstring;
        mp_cstring = other.mp_cstring;
        other.mp_cstring = temp_cstring;

        m_length ^= other.m_length;
        other.m_length ^= m_length;
        m_length ^= other.m_length;
    }
}

void String::clear() {
    String new_string;
    swap(new_string);
}

