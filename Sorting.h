#ifndef SORTING_H
#define SORTING_H

#include <cstdlib>   // for rand()
#include <utility>   // for std::swap

// ---------------------------------------------------------------
// INSERTION SORT (stable)
// T = record type, Compare = a function like
//     bool byId(const Patient& a, const Patient& b)
// Loop invariant: at the start of iteration i, arr[0..i-1] is sorted.
// ---------------------------------------------------------------
template <typename T, typename Compare>
void insertionSort(T* arr, int n, Compare comp) {
    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;
        // Move bigger records one step to the right.
        while (j >= 0 && comp(key, arr[j])) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---------------------------------------------------------------
// QUICKSORT WITH A RANDOM PIVOT  (required because my reg
// number ends in 5, which is odd)
// ---------------------------------------------------------------

// Pick a random pivot, move it to the end, then partition.
template <typename T, typename Compare>
int partitionArray(T* arr, int low, int high, Compare comp) {
    // Random index between low and high.
    // The formula below is safe on every compiler, even Windows where
    // RAND_MAX is only 32767.
    double fraction = (double)rand() / ((double)RAND_MAX + 1.0);
    int randomIndex = low + (int)(fraction * (high - low + 1));

    std::swap(arr[randomIndex], arr[high]);
    T pivot = arr[high];

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (comp(arr[j], pivot)) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

template <typename T, typename Compare>
void quickSortHelper(T* arr, int low, int high, Compare comp) {
    if (low < high) {
        int p = partitionArray(arr, low, high, comp);
        quickSortHelper(arr, low, p - 1, comp);
        quickSortHelper(arr, p + 1, high, comp);
    }
}

template <typename T, typename Compare>
void quickSort(T* arr, int n, Compare comp) {
    if (n > 1) {
        quickSortHelper(arr, 0, n - 1, comp);
    }
}

// ---------------------------------------------------------------
// ITERATIVE BINARY SEARCH by id.
// Returns the index where the id is found, or -1 if it is absent.
// The array MUST already be sorted by id.
// Works for any record type that has an int member called "id".
// ---------------------------------------------------------------
template <typename T>
int binarySearchById(const T* arr, int n, int key) {
    int low = 0;
    int high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].id == key) {
            return mid;
        } else if (arr[mid].id < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

#endif