#include "Peasant.h"
#include "Utility.h"
#include "Structure.h"
#include "Model.h"
#include <string>
#include <iostream>

using namespace std;

constexpr double kPEASANT_INITIAL_AMOUNT = 0.0;
constexpr double kPEASANT_MAX_AMOUNT = 35.0;

Peasant::Peasant(const string& name_, Point location_)
    : Agent(name_, location_), m_source(nullptr),
    m_destination(nullptr), m_amount(kPEASANT_INITIAL_AMOUNT),
    m_peasant_state(Peasant_State::NOT_WORKING)
{
    cout << "Peasant " << get_name() << " constructed" << endl;
}

Peasant::~Peasant() {
    cout << "Peasant " << get_name() << " destructed" << endl;
}

bool Peasant::is_working() {
    return m_peasant_state != Peasant_State::NOT_WORKING;
}

// implement Peasant behavior
void Peasant::update() {
    Agent::update();

    // Do nothing further if dead or not working
    if (!is_alive() || !is_working()) {
        return;
    }

    // Variables for food withdrawl
    double request_amount;
    double recieved_amount;

    switch (m_peasant_state) {
    case Peasant_State::INBOUND:
        // check if we arrived at source
        if (!is_moving()) {
            m_peasant_state = Peasant_State::COLLECTING;
        }
        break;
    case Peasant_State::COLLECTING:
        // request as much as we can carry
        request_amount = kPEASANT_MAX_AMOUNT - m_amount;
        recieved_amount = m_source->withdraw(request_amount);

        // If we collected some food, report it and then move to deposit
        if (recieved_amount > 0.0) {
            cout << get_name() << ": Collected " << recieved_amount << endl;
            m_amount += recieved_amount;
            Model::get_instance()->notify_amount(get_name(), m_amount);
            m_peasant_state = Peasant_State::OUTBOUND;
            Agent::move_to(m_destination->get_location());
        }
        // Wait for some food otherwise
        else {
            cout << get_name() << ": Waiting " << endl;
        }
        break;
    case Peasant_State::OUTBOUND:
        // check if we arrived at destination
        if (!is_moving()) {
            m_peasant_state = Peasant_State::DEPOSITING;
        }
        break;
    case Peasant_State::DEPOSITING:
        // Deposit what we have at destination and report it
        m_destination->deposit(m_amount);
        cout << get_name() << ": Deposited " << m_amount << endl;
        m_amount = 0.0;
        Model::get_instance()->notify_amount(get_name(), m_amount);

        // Move to the source
        m_peasant_state = Peasant_State::INBOUND;
        Agent::move_to(m_source->get_location());
        break;
    default:
        throw Error("Unrecognized state in Peasant::update");
    }
}

void Peasant::forget_work() {
    m_peasant_state = Peasant_State::NOT_WORKING;
    m_source = nullptr;
    m_destination = nullptr;
}

void Peasant::stop_working() {
    if (is_working()) {
        cout << get_name() << ": I'm stopping work" << endl;
        forget_work();
    }
}

void Peasant::move_to(Point dest) {
    // Set Peasant to stop working
    stop_working();
    Agent::move_to(dest);
}

// stop moving and working
void Peasant::stop() {
    Agent::stop();
    stop_working();
}

// starts the working process
// Throws an exception if the source is the same as the destination.
void Peasant::start_working(shared_ptr<Structure>& source_, shared_ptr<Structure>& destination_) {
    Agent::stop();
    forget_work();

    if (source_ == destination_) {
        throw Error(get_name() + ": I can't move food to and from the same place!");
    }

    m_source = source_;
    m_destination = destination_;

    // Peasant has no food
    if (m_amount == 0.0) {
        // If at food source, collect food
        if (get_location() == m_source->get_location()) {
            m_peasant_state = Peasant_State::COLLECTING;
        }
        // Otherwise, go to food source
        else {
            Agent::move_to(m_source->get_location());
            m_peasant_state = Peasant_State::INBOUND;
        }
    }
    else { // Peasant has food
        // If Peasant at destination, deposit food
        if (get_location() == m_destination->get_location()) {
            m_peasant_state = Peasant_State::DEPOSITING;
        }
        // Otherwise, go to destination for deposit
        else {
            Agent::move_to(m_destination->get_location());
            m_peasant_state = Peasant_State::OUTBOUND;
        }
    }
}

// ask Model to broadcast our current state to all Views
void Peasant::broadcast_current_state() const {
    // Tell View where agents are
    Agent::broadcast_current_state();
    Model::get_instance()->notify_amount(get_name(), m_amount);
}

// output information about the current state
void Peasant::describe() const {
    cout << "Peasant ";
    Agent::describe();
    cout << "   Carrying " << m_amount << endl;

    switch (m_peasant_state) {
    case Peasant_State::INBOUND:
        cout << "   Inbound to source " << m_source->get_name() << endl;
        break;
    case Peasant_State::COLLECTING:
        cout << "   Collecting at source " << m_source->get_name() << endl;
        break;
    case Peasant_State::OUTBOUND:
        cout << "   Outbound to destination " << m_destination->get_name() << endl;
        break;
    case Peasant_State::DEPOSITING:
        cout << "   Depositing at destination " << m_destination->get_name() << endl;
        break;
    case Peasant_State::NOT_WORKING:
        // Say nothing further
        break;
    default:
        throw Error("Unrecognized state in Peasant::describe");
    }
}
