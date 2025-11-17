#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <cstddef>

struct Node {
    int data;
    Node* next;
    Node* prev;

    Node(int value) : data(value), next(NULL), prev(NULL) {}
};

class CircularBuffer {
private:
    Node* head;
    int capacity;
    int size;

    void mergeSort(Node** headRef);
    Node* sortedMerge(Node* a, Node* b);
    void split(Node* source, Node** front, Node** back);

public:
    CircularBuffer(int cap);
    ~CircularBuffer();

    bool insert(int value);
    bool isFull();

    int getSize();
    void sort();
    void clear();

    Node* getHead();
};

#endif