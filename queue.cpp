#include "qtype.h"
#include "queue.h"
#include <cstring>

Queue* init(void) {
    Queue* q = new Queue();
    
    Node* sentinel = new Node{ {-1, nullptr, 0}, nullptr };
    q->head = sentinel;
    q->tail = sentinel;
    return q;
}

void release(Queue* queue) {
    if (!queue) return;

    Node* curr = queue->head;
    while (curr) {
        Node* temp = curr;
        curr = curr->next;
        if (temp->item.value) {
            delete[] static_cast<char*>(temp->item.value);
        }
        delete temp; 
    }
    delete queue; 
}