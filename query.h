#pragma once
#include "Array.h"
#include "Matrix.h"

Array<size_t> query(size_t k, Array<float>& src, Matrix& data);

class ID_DIST_PAIR {
public:
    size_t id;
    float dist;
    ID_DIST_PAIR();
    ID_DIST_PAIR(size_t k, float distance);
    ID_DIST_PAIR(const ID_DIST_PAIR& pair);
};

class ID_DIST_HEAP {
private:
    size_t capacity;
    size_t size;
    ID_DIST_PAIR* buffer;
    bool hasSort;
public:
    ID_DIST_HEAP();
    ID_DIST_HEAP(size_t k);
    ~ID_DIST_HEAP();
    ID_DIST_HEAP& operator=(const ID_DIST_HEAP&);
    // 返回一个指向存有k个id的数组
    Array<size_t> getList();
    // 往堆中插入id-距离对
    void insert(ID_DIST_PAIR pair);
private:
    // 自上而下调整堆
    void adjust(size_t index);
};