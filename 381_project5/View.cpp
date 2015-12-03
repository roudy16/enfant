#include "View.h"
#include "Geometry.h"
#include <iostream>
#include <iomanip>

using namespace std;

namespace {
    // Object to save and restore cout settings
    // Settings saved when object is created then restored when object destroyed
    class Cout_settings_saver {
    public:
        // Save settings
        Cout_settings_saver() : m_form_flags(cout.flags()), m_old_precision(cout.precision())
        {}

        // Restore settings
        ~Cout_settings_saver() {
            cout.flags(m_form_flags);
            cout.precision(m_old_precision);
        }

    private:
        ios::fmtflags m_form_flags;
        streamsize m_old_precision;
    };
}

View::View(const string& name)
    : m_name(name)
{
}

View::~View()
{
}

void View::update_location(const string& name, Point location)
{
}

void View::update_health(const string& name, double health)
{
}

void View::update_amount(const string& name, double amount)
{
}

void View::update_remove(const string& name)
{
}

void View::draw_header() {

}

void View::draw() {
    // Save previous output settings
    Cout_settings_saver css;

    // Display doubles with exactly 2 digits after the decimal point
    cout << fixed;
    cout << setprecision(2);

    draw_header();
    draw_body();

    // Restore previous output settings when function ends
}

void View::clear()
{
}

const string& View::get_name() {
    return m_name;
}
