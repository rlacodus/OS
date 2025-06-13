#pragma once
#include <mutex>

typedef int Key;
typedef void* Value;

typedef struct {
    Key key;
    Value value;
    int value_size; 
} Item;

typedef struct {
    bool success; 
    Item item;   
} Reply;

typedef struct node_t {
    Item item;
    struct node_t* next;
} Node;

typedef struct {
    Node* head, tail;
    std::mutex mtx; 
} Queue;