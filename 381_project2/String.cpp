#include "String.h"
#include "Utility.h"
#include <utility> // std::swap
#include <iostream>
#include <cstring>
#include <cctype>  // isspace
#include <cassert>

using std::cout;
using std::endl;

// class static variable definitions
char String::a_null_byte = '\0';
int String::number = 0;
int String::total_allocation = 0;
bool String::messages_wanted = false;

void String::init(const char* cstr_) {
    // Calculate length of argument string
    const int len = static_cast<int>(strlen(cstr_));

    // Case for initializing an empty string
    if (len == 0) {
        mp_cstring = &a_null_byte;
        m_allocation = 0;
    }
    else {
        // for a non-empty String we must allocate memory for each character
        // in the argument string plus one byte for the null terminator. Then
        // we copy the argument string into the allocated memory.
        const int allocation_bytes = len + 1;
        mp_cstring = new char[allocation_bytes];
        strncpy(mp_cstring, cstr_, allocation_bytes);
        m_allocation = allocation_bytes;
        total_allocation += allocation_bytes;
    }

    m_length = len;
    ++number;
}

String::String(const char* cstr_) {
    if (messages_wanted) {
        cout << "Ctor: \"" << cstr_ << "\"" << endl;
    }

    init(cstr_);
}

String::String(const String& original) {
    if (messages_wanted) {
        cout << "Copy ctor: \"" << original.mp_cstring << "\"" << endl;
    }

    init(original.mp_cstring);
}

String::String(String&& rhs) noexcept 
    : mp_cstring(rhs.mp_cstring), m_length(rhs.m_length),
      m_allocation(rhs.m_allocation)
{
    if (messages_wanted) {
        cout << "Move ctor: \"" << rhs.mp_cstring << "\"" << endl;
    }

    rhs.mp_cstring = &a_null_byte;
    rhs.m_length = 0;
    rhs.m_allocation = 0;
    ++number;
}

String::~String() noexcept {
    if (messages_wanted) {
        cout << "Dtor: \"" << mp_cstring << "\"" << endl;
    }

    // If the String is non-empty we must delete[] the allocated
    // memory and track this change in allocation
    if (m_length != 0) {
        assert(mp_cstring != &a_null_byte);
        delete[] mp_cstring;
        total_allocation -= m_allocation;
    }

    mp_cstring = nullptr;
    --number;
}

String& String::operator= (const String& rhs) {
    if (messages_wanted) {
        cout << "Copy assign from String:  \"" << rhs.mp_cstring << "\"" << endl;
    }

    String new_string(rhs);
    swap(new_string);

    return *this;
}

String& String::operator= (const char* rhs) {
    if (messages_wanted) {
        cout << "Assign from C-string:  \"" << rhs << "\"" << endl;
    }

    String new_string(rhs);
    swap(new_string);

    return *this;
}

String& String::operator= (String&& rhs) noexcept {
    if (messages_wanted) {
        cout << "Move assign from String:  \"" << rhs.mp_cstring << "\"" << endl;
    }

    swap(rhs);

    return *this;
}

void String::swap(String& other) noexcept {
    if (this != &other){
        std::swap(mp_cstring, other.mp_cstring);
        std::swap(m_length, other.m_length);
        std::swap(m_allocation, other.m_allocation);
    }
}

void String::clear() {
    String new_string;
    swap(new_string);
}

char& String::operator[] (int i) {
    if (i >= m_length || i < 0) {
        throw String_exception("Subscript out of range");
    }

    return mp_cstring[i];
}

const char& String::operator[] (int i) const {
    return const_cast<String&>(*this)[i];
}

const char* String::grow(const int min_new_allocation) {
    assert(min_new_allocation > m_length);

    // We want to grow the string to allow for appending of the string, however
    // we must account for the case where the string appends itself. For this
    // the char array containing the original string cannot be deleted until
    // after the appending is complete. This function returns a pointer that
    // must be deleted after appending is complete.
    const char* deferred_delete_ptr = nullptr;
    const int new_allocation = 2 * min_new_allocation;
    char* temp_cstr = new char[new_allocation];
    strncpy(temp_cstr, mp_cstring, m_length);

    if (mp_cstring != &a_null_byte) {
        deferred_delete_ptr = mp_cstring;
        total_allocation -= m_allocation;
    }

    mp_cstring = temp_cstr;
    m_allocation = new_allocation;
    total_allocation += new_allocation;

    return deferred_delete_ptr;
}

String& String::operator += (char rhs) {
    if (rhs == '\0') {
        return *this;
    }

    // The minimum amount of memory needed to append a new char is the
    // current string length plus one for the new char plus another for
    // the null-byte
    const int required_allocation = m_length + 2;
    if (m_allocation < required_allocation) {
        const char* old_string_data = grow(required_allocation);
        delete[] old_string_data;
    }

    mp_cstring[m_length] = rhs;
    mp_cstring[m_length + 1] = '\0';
    m_length += 1;
    return *this;
}

// TODO ugly solution to case where String appends itself
String& String::operator += (const char* rhs) {
    const int rhs_len = static_cast<const int>(strlen(rhs));
    if (rhs_len == 0) {
        return *this;
    }

    const char* deferred_delete_ptr = nullptr;
    const int required_allocation = m_length + rhs_len + 1;
    if (m_allocation < required_allocation) {
        deferred_delete_ptr = grow(required_allocation);
    }

    // Handles case where String appends itself: str += str.c_str()
    strncpy(mp_cstring + m_length, rhs, rhs_len);
    m_length += rhs_len;
    mp_cstring[m_length] = '\0';

    // Now that we have copied the String it is safe to delete the old
    // char data if we had to grow the appended String.
    delete[] deferred_delete_ptr;

    return *this;
}

String& String::operator += (const String& rhs) {
    return *this += rhs.c_str();
}

bool operator== (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator!= (const String& lhs, const String& rhs) {
    return !(lhs == rhs);
}

bool operator< (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator> (const String& lhs, const String& rhs) {
    return rhs < lhs;
}

String operator+ (const String& lhs, const String& rhs) {
    String new_string(lhs);
    return new_string += rhs;
}

std::ostream& operator<< (std::ostream& os, const String& str) {
    return os << str.c_str();
}

std::istream& operator>> (std::istream& is, String& str) {
    str.clear();

    // Skip initial whitespace
    while (is && isspace(is.peek())) {
        is.ignore();
    }

    // Read characters into str until whitespace is found
    while (is && !isspace(is.peek())) {
        str += is.get();
    }

    return is;
}

