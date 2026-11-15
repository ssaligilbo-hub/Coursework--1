#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#include "Patient.h"
#include "Queue.h"
#include "Sorting.h"

using namespace std;

// ---------------- comparators (passed to the sorts) ----------------

// Clinic order: emergency (1) first, then urgent (2), then normal (3).
// If two patients have the same priority, the one who arrived first
// goes first (arrivalOrder). This tie-break matters because
// quicksort itself is NOT stable.
bool byPriorityThenArrival(const Patient& a, const Patient& b) {
    if (a.priority != b.priority) {
        return a.priority < b.priority;
    }
    return a.arrivalOrder < b.arrivalOrder;
}

// Used before binary search.
bool byId(const Patient& a, const Patient& b) {
    return a.id < b.id;
}

// ---------------- file input / output ----------------

// Load records from a text file into the queue.
bool loadPatients(const string& fileName, CircularQueue<Patient>& q) {
    ifstream in(fileName.c_str());
    if (!in) {
        cout << "ERROR: cannot open " << fileName << "\n";
        return false;
    }

    int id, priority, arrivalOrder;
    string name;
    int loaded = 0;

    while (in >> id >> name >> priority >> arrivalOrder) {
        try {
            q.enqueue(Patient(id, name, priority, arrivalOrder));
            loaded++;
        } catch (runtime_error& e) {
            cout << "Stopped loading: " << e.what() << "\n";
            break;
        }
    }

    in.close();
    return loaded > 0;
}

// Save every record in the queue to a text file.
void savePatients(const string& fileName, const CircularQueue<Patient>& q) {
    ofstream out(fileName.c_str());
    if (!out) {
        cout << "ERROR: cannot write " << fileName << "\n";
        return;
    }
    for (int i = 0; i < q.size(); i++) {
        Patient p = q.getAt(i);
        out << p.id << " " << p.name << " "
            << p.priority << " " << p.arrivalOrder << "\n";
    }
    out.close();
}

// Copy the queue into a plain array so it can be sorted.
int queueToArray(const CircularQueue<Patient>& q, Patient* arr, int maxSize) {
    int n = q.size();
    if (n > maxSize) {
        n = maxSize;
    }
    for (int i = 0; i < n; i++) {
        arr[i] = q.getAt(i);
    }
    return n;
}

// ---------------- demonstrations ----------------

void demoQueue() {
    cout << "\n=== A1: Queue demo ===\n";

    CircularQueue<Patient> q(50);
    if (!loadPatients("patients.txt", q)) {
        return;
    }

    cout << "Loaded " << q.size() << " patients.\n";
    cout << "Next patient (front): " << q.front() << "\n";

    Patient served = q.dequeue();
    cout << "Served: " << served << "\n";
    cout << "Remaining: " << q.size() << "\n";

    q.enqueue(Patient(2001, "NewArrival", 1, 13));
    cout << "Added NewArrival. Size now: " << q.size() << "\n";

    // Invalid operation, rejected safely.
    try {
        CircularQueue<Patient> empty(5);
        empty.dequeue();
    } catch (runtime_error& e) {
        cout << "[Safe rejection] " << e.what() << "\n";
    }

    // Deep copy test.
    CircularQueue<Patient> copy = q;
    cout << "Original size: " << q.size()
         << ", copy size: " << copy.size() << "\n";

    // Save the updated records. (The original file is kept unchanged so
    // the sample data stays reproducible. Change the file name to
    // "patients.txt" if you want to save back into the original file.)
    savePatients("patients_updated.txt", q);
    cout << "Saved updated records to patients_updated.txt\n";
}

void demoSorts() {
    cout << "\n=== A2: Sorting and binary search ===\n";

    CircularQueue<Patient> q(50);
    if (!loadPatients("patients.txt", q)) {
        return;
    }

    Patient arr[100];
    int n = queueToArray(q, arr, 100);

    insertionSort(arr, n, byPriorityThenArrival);
    cout << "After insertionSort by priority then arrival:\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << arr[i] << "\n";
    }

    quickSort(arr, n, byPriorityThenArrival);
    cout << "After quickSort (random pivot) by priority then arrival:\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << arr[i] << "\n";
    }

    // Binary search needs sorting by id first.
    quickSort(arr, n, byId);

    int found = binarySearchById(arr, n, 1004);
    if (found >= 0) {
        cout << "Binary search: ID 1004 found at index " << found
             << " -> " << arr[found] << "\n";
    } else {
        cout << "Binary search: ID 1004 not found\n";
    }

    found = binarySearchById(arr, n, 7777);
    cout << "Binary search for absent ID 7777 returned " << found
         << " (expected -1)\n";
}

// ---------------- A3: boundary tests ----------------

void runTests() {
    cout << "\n=== A3: Boundary tests ===\n";
    int passed = 0;
    int failed = 0;

    // T1: dequeue from empty queue
    try {
        CircularQueue<Patient> q(3);
        q.dequeue();
        cout << "[FAIL] T1 dequeue from empty queue did not throw\n";
        failed++;
    } catch (runtime_error&) {
        cout << "[PASS] T1 dequeue from empty queue throws safely\n";
        passed++;
    }

    // T2: front on empty queue
    try {
        CircularQueue<Patient> q(3);
        q.front();
        cout << "[FAIL] T2 front on empty queue did not throw\n";
        failed++;
    } catch (runtime_error&) {
        cout << "[PASS] T2 front on empty queue throws safely\n";
        passed++;
    }

    // T3: one record
    {
        CircularQueue<Patient> q(3);
        q.enqueue(Patient(1, "Solo", 2, 1));
        bool ok = (q.size() == 1);
        Patient p = q.dequeue();
        ok = ok && (p.id == 1) && q.isEmpty();
        if (ok) {
            cout << "[PASS] T3 one record: enqueue/dequeue works\n";
            passed++;
        } else {
            cout << "[FAIL] T3 one record test failed\n";
            failed++;
        }
    }

    // T4: enqueue into a full queue
    {
        CircularQueue<Patient> q(2);
        q.enqueue(Patient(1, "A", 1, 1));
        q.enqueue(Patient(2, "B", 2, 2));
        try {
            q.enqueue(Patient(3, "C", 3, 3));
            cout << "[FAIL] T4 enqueue into full queue did not throw\n";
            failed++;
        } catch (runtime_error&) {
            if (q.size() == 2) {
                cout << "[PASS] T4 enqueue into full queue throws, size stays 2\n";
                passed++;
            } else {
                cout << "[FAIL] T4 size changed after failed enqueue\n";
                failed++;
            }
        }
    }

    // T5: remove the first record
    {
        CircularQueue<Patient> q(3);
        q.enqueue(Patient(10, "First", 1, 1));
        q.enqueue(Patient(20, "Second", 1, 2));
        Patient p = q.dequeue();
        bool ok = (p.id == 10) && (q.front().id == 20) && (q.size() == 1);
        if (ok) {
            cout << "[PASS] T5 removal of first record leaves second at front\n";
            passed++;
        } else {
            cout << "[FAIL] T5 removal of first record failed\n";
            failed++;
        }
    }

    // T6: remove the last record
    {
        CircularQueue<Patient> q(3);
        q.enqueue(Patient(10, "First", 1, 1));
        q.enqueue(Patient(20, "Last", 1, 2));
        q.dequeue();
        Patient p = q.dequeue();
        bool ok = (p.id == 20) && q.isEmpty();
        if (ok) {
            cout << "[PASS] T6 removal of last record leaves queue empty\n";
            passed++;
        } else {
            cout << "[FAIL] T6 removal of last record failed\n";
            failed++;
        }
    }

    // T7: deep copy independence
    {
        CircularQueue<Patient> q(5);
        q.enqueue(Patient(1, "A", 1, 1));
        q.enqueue(Patient(2, "B", 2, 2));
        CircularQueue<Patient> copy = q;
        copy.dequeue();
        bool ok = (q.size() == 2) && (copy.size() == 1);
        if (ok) {
            cout << "[PASS] T7 deep copy: copy change does not affect original\n";
            passed++;
        } else {
            cout << "[FAIL] T7 deep copy test failed\n";
            failed++;
        }
    }

    // Test data with DUPLICATE priorities (1, 2, 3 appear twice each)
    Patient base[6] = {
        Patient(105, "F", 3, 6),
        Patient(103, "D", 1, 4),
        Patient(101, "B", 1, 2),
        Patient(106, "G", 2, 7),
        Patient(102, "C", 3, 3),
        Patient(104, "E", 2, 5)
    };

    // T8: insertion sort by id
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[i];
        insertionSort(a, 6, byId);
        bool ok = true;
        for (int i = 1; i < 6; i++) {
            if (a[i - 1].id > a[i].id) ok = false;
        }
        ok = ok && (a[0].id == 101) && (a[5].id == 106);
        if (ok) {
            cout << "[PASS] T8 insertion sort by id\n";
            passed++;
        } else {
            cout << "[FAIL] T8 insertion sort by id failed\n";
            failed++;
        }
    }

    // T9: quicksort by id
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[i];
        quickSort(a, 6, byId);
        bool ok = true;
        for (int i = 1; i < 6; i++) {
            if (a[i - 1].id > a[i].id) ok = false;
        }
        ok = ok && (a[0].id == 101) && (a[5].id == 106);
        if (ok) {
            cout << "[PASS] T9 quicksort by id\n";
            passed++;
        } else {
            cout << "[FAIL] T9 quicksort by id failed\n";
            failed++;
        }
    }

    // T10: duplicate priority -> arrival order must be kept
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[i];
        quickSort(a, 6, byPriorityThenArrival);
        bool ok = true;
        for (int i = 1; i < 6; i++) {
            if (a[i - 1].priority > a[i].priority) ok = false;
            if (a[i - 1].priority == a[i].priority &&
                a[i - 1].arrivalOrder > a[i].arrivalOrder) ok = false;
        }
        if (ok) {
            cout << "[PASS] T10 duplicate priority keeps arrival order\n";
            passed++;
        } else {
            cout << "[FAIL] T10 duplicate priority test failed\n";
            failed++;
        }
    }

    // T11: already sorted input stays sorted
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[i];
        insertionSort(a, 6, byId);
        Patient before[6];
        for (int i = 0; i < 6; i++) before[i] = a[i];
        insertionSort(a, 6, byId);
        bool ok = true;
        for (int i = 0; i < 6; i++) {
            if (a[i].id != before[i].id) ok = false;
        }
        if (ok) {
            cout << "[PASS] T11 sorting already-sorted input keeps order\n";
            passed++;
        } else {
            cout << "[FAIL] T11 already-sorted test failed\n";
            failed++;
        }
    }

    // T12: reversed input
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[5 - i];
        quickSort(a, 6, byId);
        bool ok = true;
        for (int i = 1; i < 6; i++) {
            if (a[i - 1].id > a[i].id) ok = false;
        }
        if (ok) {
            cout << "[PASS] T12 sorting reversed input gives ascending order\n";
            passed++;
        } else {
            cout << "[FAIL] T12 reversed input test failed\n";
            failed++;
        }
    }

    // T13: binary search on first, last and absent key
    {
        Patient a[6];
        for (int i = 0; i < 6; i++) a[i] = base[i];
        insertionSort(a, 6, byId);
        int first  = binarySearchById(a, 6, 101);
        int last   = binarySearchById(a, 6, 106);
        int absent = binarySearchById(a, 6, 999);
        if (first == 0 && last == 5 && absent == -1) {
            cout << "[PASS] T13 binary search first, last, absent\n";
            passed++;
        } else {
            cout << "[FAIL] T13 binary search test failed\n";
            failed++;
        }
    }

    cout << "\nTests passed: " << passed << ", failed: " << failed << "\n";
}

// ---------------- A4: timing experiment ----------------

void timingExperiment() {
    cout << "\n=== A4: Timing insertion sort vs quicksort ===\n";
    cout << "n\tinsertion(s)\tquick(s)\tins ratio\tquick ratio\n";
    cout << fixed << setprecision(6);

    ofstream csv("timing.csv");
    csv << "n,insertion_s,quicksort_s\n";

    double previousIns = 0.0;
    double previousQuick = 0.0;

    for (int n = 1000; n <= 64000; n = n * 2) {
        double totalIns = 0.0;
        double totalQuick = 0.0;

        for (int trial = 0; trial < 3; trial++) {
            // Build random data for this trial.
            Patient* base = new Patient[n];
            for (int i = 0; i < n; i++) {
                int randomPriority = (rand() % 3) + 1;
                base[i] = Patient(100000 + i, "P", randomPriority, i);
            }

            // Time insertion sort.
            Patient* copy1 = new Patient[n];
            for (int i = 0; i < n; i++) copy1[i] = base[i];
            chrono::high_resolution_clock::time_point start =
                chrono::high_resolution_clock::now();
            insertionSort(copy1, n, byPriorityThenArrival);
            chrono::high_resolution_clock::time_point finish =
                chrono::high_resolution_clock::now();
            chrono::duration<double> diff = finish - start;
            totalIns = totalIns + diff.count();
            delete[] copy1;

            // Time quicksort (same input).
            Patient* copy2 = new Patient[n];
            for (int i = 0; i < n; i++) copy2[i] = base[i];
            start = chrono::high_resolution_clock::now();
            quickSort(copy2, n, byPriorityThenArrival);
            finish = chrono::high_resolution_clock::now();
            diff = finish - start;
            totalQuick = totalQuick + diff.count();
            delete[] copy2;

            delete[] base;
        }

        double averageIns   = totalIns / 3.0;
        double averageQuick = totalQuick / 3.0;

        double insRatio = 0.0;
        double quickRatio = 0.0;
        if (previousIns > 0.0) {
            insRatio   = averageIns / previousIns;
            quickRatio = averageQuick / previousQuick;
        }

        cout << n << "\t" << averageIns << "\t" << averageQuick
             << "\t" << setprecision(2) << insRatio
             << "\t" << quickRatio << setprecision(6) << "\n";

        csv << n << "," << averageIns << "," << averageQuick << "\n";

        previousIns = averageIns;
        previousQuick = averageQuick;
    }

    csv.close();
    cout << "Timing data saved to timing.csv\n";
}

// ---------------- main ----------------

int main() {
    srand((unsigned)time(0));   // seed rand() for the random pivot

    demoQueue();
    demoSorts();
    runTests();
    timingExperiment();

    return 0;
}