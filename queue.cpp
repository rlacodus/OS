#include "qtype.h"
#include "queue.h"
#include <cstring>

// nalloc/nfree/nclone 최소구현 (혹시 호출될 경우 대비)
Node* nalloc(Item item) { return nullptr; }
void nfree(Node* node) {}
Node* nclone(Node* node) { return nullptr; }

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

Reply enqueue(Queue* queue, Item item) {
    std::lock_guard<std::mutex> lock(queue->mtx);

    Value newValue = new char[item.value_size];
    memcpy(newValue, item.value, item.value_size);

    Node* pred = queue->head;
    Node* curr = queue->head->next;

    while (curr && curr->item.key < item.key) {
        pred = curr;
        curr = curr->next;
    }

    if (curr && curr->item.key == item.key) {
        delete[] static_cast<char*>(curr->item.value);
        curr->item.value = newValue;
        curr->item.value_size = item.value_size;
    }
    else {
        Node* newNode = new Node{ {item.key, newValue, item.value_size}, curr };
        pred->next = newNode;
        if (!newNode->next) {
            queue->tail = newNode;
        }
    }

    return { true, {} };
}

Reply dequeue(Queue* queue) {
    std::lock_guard<std::mutex> lock(queue->mtx);

    Node* sentinel = queue->head;
    Node* nodeToRemove = sentinel->next;

    if (!nodeToRemove) {
        return { false, {} };
    }

    sentinel->next = nodeToRemove->next;
    if (!sentinel->next) {
        queue->tail = sentinel;
    }

    Item resultItem;
    resultItem.key = nodeToRemove->item.key;
    resultItem.value_size = nodeToRemove->item.value_size;
    resultItem.value = new char[resultItem.value_size];
    memcpy(resultItem.value, nodeToRemove->item.value, resultItem.value_size);

    delete[] static_cast<char*>(nodeToRemove->item.value);
    delete nodeToRemove;

    return { true, resultItem };
}

Queue* range(Queue* queue, Key start, Key end) {
    Queue* resultQueue = init();

    std::lock_guard<std::mutex> lock(queue->mtx);

    Node* curr = queue->head->next;
    while (curr && curr->item.key < start) {
        curr = curr->next;
    }

    while (curr && curr->item.key <= end) {  // end 포함으로 보강
        enqueue(resultQueue, curr->item);
        curr = curr->next;
    }

    return resultQueue;
}
