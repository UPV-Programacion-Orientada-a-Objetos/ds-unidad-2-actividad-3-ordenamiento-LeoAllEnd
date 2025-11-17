#include "CircularBuffer.h"
#include <cstddef>

CircularBuffer::CircularBuffer(int cap) {
    capacity = cap;
    size = 0;
    head = NULL;
}

CircularBuffer::~CircularBuffer() {
    clear();
}

bool CircularBuffer::insert(int value) {
    if (size >= capacity) {
        return false;
    }
    
    Node* newNode = new Node(value);
    
    if (head == NULL) {
        head = newNode;
        head->next = head;
        head->prev = head;
    } else {
        Node* tail = head->prev;
        
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
    }
    
    size++;
    return true;
}

bool CircularBuffer::isFull() {
    return size >= capacity;
}

int CircularBuffer::getSize() {
    return size;
}

void CircularBuffer::clear() {
    if (head == NULL) {
        return;
    }
    
    Node* current = head;
    Node* nextNode;
    
    // Romper el círculo
    head->prev->next = NULL;
    
    while (current != NULL) {
        nextNode = current->next;
        delete current;
        current = nextNode;
    }
    
    head = NULL;
    size = 0;
}

// Ordenamiento usando MergeSort
void CircularBuffer::sort() {
    if (head == NULL || size <= 1) {
        return;
    }
    
    // Romper la estructura circular temporalmente
    Node* tail = head->prev;
    tail->next = NULL;
    head->prev = NULL;
    
    // Aplicar mergesort
    mergeSort(&head);
    
    // Restaurar la estructura circular
    Node* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    tail = current;
    
    tail->next = head;
    head->prev = tail;
}

void CircularBuffer::split(Node* source, Node** front, Node** back) {
    Node* slow = source;
    Node* fast = source->next;
    
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

Node* CircularBuffer::sortedMerge(Node* a, Node* b) {
    if (a == NULL) {
        return b;
    }
    if (b == NULL) {
        return a;
    }
    
    Node* result = NULL;
    
    if (a->data <= b->data) {
        result = a;
        result->next = sortedMerge(a->next, b);
        if (result->next != NULL) {
            result->next->prev = result;
        }
        result->prev = NULL;
    } else {
        result = b;
        result->next = sortedMerge(a, b->next);
        if (result->next != NULL) {
            result->next->prev = result;
        }
        result->prev = NULL;
    }
    
    return result;
}

void CircularBuffer::mergeSort(Node** headRef) {
    Node* head_ptr = *headRef;
    
    if (head_ptr == NULL || head_ptr->next == NULL) {
        return;
    }
    
    Node* a;
    Node* b;
    
    split(head_ptr, &a, &b);
    
    mergeSort(&a);
    mergeSort(&b);
    
    *headRef = sortedMerge(a, b);
}

Node* CircularBuffer::getHead() {
    return head;
}