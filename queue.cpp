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

Reply enqueue(Queue* queue, Item item) {
    // lock_guard를 사용하여 이 함수가 끝날 때까지 큐를 잠금
    std::lock_guard<std::mutex> lock(queue->mtx);

    // 깊은 복사위해 새 메모리 할당하고 데이터 복사 
    Value newValue = new char[item.value_size];
    memcpy(newValue, item.value, item.value_size);

    Node* pred = queue->head;
    Node* curr = queue->head->next;

    // 키 값에 따라 정렬될 위치를 찾습니다.
    while (curr && curr->item.key < item.key) {
        pred = curr;
        curr = curr->next;
    }

    // Case 1: 동일한 키를 가진 노드를 찾은 경우 (Update)
    if (curr && curr->item.key == item.key) {
        delete[] static_cast<char*>(curr->item.value); // 기존 값 메모리 해제
        curr->item.value = newValue; // 새 값으로 교체
        curr->item.value_size = item.value_size;
    }
    // Case 2: 새 노드를 삽입해야 하는 경우
    else {
        Node* newNode = new Node{ {item.key, newValue, item.value_size}, curr };
        pred->next = newNode;
        // 만약 맨 끝에 노드를 추가했다면 tail 포인터를 업데이트합니다.
        if (!newNode->next) {
            queue->tail = newNode;
        }
    }

    return { true, {} }; // 성공적으로 작업을 마침
}