#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>   // for runtime_error

// A queue stored in a fixed-size array that wraps around.
// NO STL containers are used here (only a plain array).
template <typename T>
class CircularQueue {
private:
    T* data;         // the array holding the records
    int capacity;    // maximum number of records
    int frontIndex;  // index of the first record
    int rearIndex;   // index where the next record will be written
    int count;       // how many records are stored right now

public:
    // Constructor: create an empty queue with room for 'cap' records.
    CircularQueue(int cap = 100) {
        if (cap <= 0) {
            throw std::runtime_error("Capacity must be positive");
        }
        capacity = cap;
        data = new T[capacity];
        frontIndex = 0;
        rearIndex = 0;
        count = 0;
    }

    // Destructor: free the array when the queue is destroyed.
    ~CircularQueue() {
        delete[] data;
    }

    // Copy constructor: DEEP copy.
    // We allocate a NEW array and copy every record, so the two queues
    // never share memory (otherwise both destructors would delete the
    // same array and the program would crash).
    CircularQueue(const CircularQueue& other) {
        capacity = other.capacity;
        frontIndex = other.frontIndex;
        rearIndex = other.rearIndex;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
    }

    // Copy assignment (needed for statements like q1 = q2).
    CircularQueue& operator=(const CircularQueue& other) {
        if (this == &other) {     // copying onto itself: do nothing
            return *this;
        }
        delete[] data;            // free our old array first
        capacity = other.capacity;
        frontIndex = other.frontIndex;
        rearIndex = other.rearIndex;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // Add a record at the back. O(1).
    void enqueue(const T& item) {
        if (isFull()) {
            throw std::runtime_error("Queue is full");
        }
        data[rearIndex] = item;
        rearIndex = (rearIndex + 1) % capacity;  // wrap around
        count++;
    }

    // Remove and return the record at the front. O(1).
    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        T item = data[frontIndex];
        frontIndex = (frontIndex + 1) % capacity; // wrap around
        count--;
        return item;
    }

    // Look at the front record without removing it. O(1).
    T front() const {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[frontIndex];
    }

    bool isEmpty() const { return count == 0; }
    bool isFull()  const { return count == capacity; }
    int  size()    const { return count; }

    // Remove all records (memory is not freed).
    void clear() {
        frontIndex = 0;
        rearIndex = 0;
        count = 0;
    }

    // Read record number i (i = 0 is the front). Used for display
    // and for copying the records into an array for sorting.
    T getAt(int i) const {
        if (i < 0 || i >= count) {
            throw std::runtime_error("Index out of range");
        }
        return data[(frontIndex + i) % capacity];
    }
};

#endif