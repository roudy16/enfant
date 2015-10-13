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
    const int len = static_cast<int>(strlen(cstr_));
    if (len == 0) {
        mp_cstring = &a_null_byte;
        m_allocation = 0;
    }
    else {
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
    init(cstr_);

    if (messages_wanted) {
        cout << "Ctor: \"" << mp_cstring << "\"" << endl;
    }
}

String::String(const String& original) {
    init(original.mp_cstring);

    if (messages_wanted) {
        cout << "Copy ctor: \"" << mp_cstring << "\"" << endl;
    }
}

String::String(String&& rhs) noexcept {
    mp_cstring = rhs.mp_cstring;
    m_length = rhs.m_length;
    m_allocation = rhs.m_allocation;
    rhs.mp_cstring = &a_null_byte;
    rhs.m_length = 0;
    rhs.m_allocation = 0;
    ++number;

    if (messages_wanted) {
        cout << "Move ctor: \"" << mp_cstring << "\"" << endl;
    }
}

String::~String() noexcept {
    if (m_length != 0) {
        assert(mp_cstring != &a_null_byte);
        delete[] mp_cstring;
        total_allocation -= m_allocation;
    }

    mp_cstring = nullptr;
    --number;
}

String& String::operator= (const String& rhs) {
    if (this != &rhs) {
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

void String::grow(const int min_new_allocation) {
    assert(min_new_allocation > m_length);

    const int new_allocation = 2 * min_new_allocation;
    char* temp_cstr = new char[new_allocation];
    strncpy(temp_cstr, mp_cstring, m_length);

    if (mp_cstring != &a_null_byte) {
        delete[] mp_cstring;
        total_allocation -= m_allocation;
    }

    mp_cstring = temp_cstr;
    m_allocation = new_allocation;
    total_allocation += new_allocation;
}

String& String::operator += (char rhs) {
    if (rhs == '\0') {
        return *this;
    }

    const int required_allocation = m_length + 2;
    if (m_allocation < required_allocation) {
        grow(required_allocation);
    }

    mp_cstring[m_length] = rhs;
    mp_cstring[m_length + 1] = '\0';
    m_length += 1;
    return *this;
}

String& String::operator += (const char* rhs) {
    const int rhs_len = static_cast<const int>(strlen(rhs));
    if (rhs_len == 0) {
        return *this;
    }

    const int required_allocation = m_length + rhs_len + 1;
    if (m_allocation < required_allocation) {
        grow(required_allocation);
    }

    strncpy(mp_cstring + m_length, rhs, rhs_len + 1);
    m_length += rhs_len;
    return *this;
}

String& String::operator += (const String& rhs) {
    if (rhs.m_length == 0) {
        return *this;
    }

    const int required_allocation = m_length + rhs.m_length + 1;
    if (m_allocation < required_allocation) {
        grow(required_allocation);
    }

    strncpy(mp_cstring + m_length, rhs.mp_cstring, rhs.m_length + 1);
    m_length += rhs.m_length;
    return *this;
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
    while (isspace(is.peek())) {
        is.ignore();
    }

    // Read characters into str until whitespace is found
    while (!isspace(is.peek())) {
        str += is.get();
    }

    return is;
}

