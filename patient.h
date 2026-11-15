#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <iostream>

// One clinic patient record.
struct Patient {
    int id;              // unique patient ID (used for binary search)
    std::string name;
    int priority;        // 1 = emergency, 2 = urgent, 3 = normal
    int arrivalOrder;    // 1 = first to arrive, 2 = second, ...

    // Default constructor: needed because we create arrays of Patient.
    Patient() {
        id = 0;
        name = "";
        priority = 3;
        arrivalOrder = 0;
    }

    // Constructor with values.
    Patient(int i, std::string n, int p, int a) {
        id = i;
        name = n;
        priority = p;
        arrivalOrder = a;
    }
};

// Lets us write: cout << patient;
inline std::ostream& operator<<(std::ostream& out, const Patient& p) {
    out << "ID=" << p.id
        << " Name=" << p.name
        << " Priority=" << p.priority
        << " Arrival=" << p.arrivalOrder;
    return out;
}

#endif