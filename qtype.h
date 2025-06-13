#pragma once
#include <mutex>

typedef int Key;
typedef void* Value;

typedef struct {
    Key key;
    Value value;
    int value_size; // value�� ����Ű�� �޸��� ����Ʈ ũ��
} Item;

typedef struct {
    bool success; // �۾� ���� ���� (true: ����, false: ����) 
    Item item;    // �۾� ����� ���� ������
} Reply;


typedef struct node_t {
    Item item;
    struct node_t* next;
} Node;

typedef struct {
    Node* head; 
    Node* tail;
    std::mutex mtx; 
} Queue;