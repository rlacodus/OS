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
    // lock_guard�� ����Ͽ� �� �Լ��� ���� ������ ť�� ���
    std::lock_guard<std::mutex> lock(queue->mtx);

    // ���� �������� �� �޸� �Ҵ��ϰ� ������ ���� 
    Value newValue = new char[item.value_size];
    memcpy(newValue, item.value, item.value_size);

    Node* pred = queue->head;
    Node* curr = queue->head->next;

    // Ű ���� ���� ���ĵ� ��ġ�� ã���ϴ�.
    while (curr && curr->item.key < item.key) {
        pred = curr;
        curr = curr->next;
    }

    // Case 1: ������ Ű�� ���� ��带 ã�� ��� (Update)
    if (curr && curr->item.key == item.key) {
        delete[] static_cast<char*>(curr->item.value); // ���� �� �޸� ����
        curr->item.value = newValue; // �� ������ ��ü
        curr->item.value_size = item.value_size;
    }
    // Case 2: �� ��带 �����ؾ� �ϴ� ���
    else {
        Node* newNode = new Node{ {item.key, newValue, item.value_size}, curr };
        pred->next = newNode;
        // ���� �� ���� ��带 �߰��ߴٸ� tail �����͸� ������Ʈ�մϴ�.
        if (!newNode->next) {
            queue->tail = newNode;
        }
    }

    return { true, {} }; // ���������� �۾��� ��ħ
}

Reply dequeue(Queue* queue) {
    std::lock_guard<std::mutex> lock(queue->mtx);

    Node* sentinel = queue->head;
    Node* nodeToRemove = sentinel->next;

    // ť�� ����ִ� ���
    if (!nodeToRemove) {
        return { false, {} };
    }

    // ť���� ��带 ����
    sentinel->next = nodeToRemove->next;
    // ���� ������ ��带 �����ߴٸ� tail �����͸� sentinel�� �ǵ����ϴ�.
    if (!sentinel->next) {
        queue->tail = sentinel;
    }

    // Reply ����ü�� ��ȯ�� �������� ���� �����մϴ�.
    Item resultItem;
    resultItem.key = nodeToRemove->item.key;
    resultItem.value_size = nodeToRemove->item.value_size;
    resultItem.value = new char[resultItem.value_size];
    memcpy(resultItem.value, nodeToRemove->item.value, resultItem.value_size);

    // ���� ���� �� ���� value �޸𸮸� �����մϴ�.
    delete[] static_cast<char*>(nodeToRemove->item.value);
    delete nodeToRemove;

    return { true, resultItem };
}