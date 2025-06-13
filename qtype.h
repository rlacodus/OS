#pragma once
#include <mutex>

typedef int Key;
typedef void* Value;

typedef struct {
    Key key;
    Value value;
    int value_size; // value가 가리키는 메모리의 바이트 크기
} Item;

typedef struct {
    bool success; // 작업 성공 여부 (true: 성공, false: 실패) 
    Item item;    // 작업 결과로 얻은 아이템
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