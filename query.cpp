#include "Matrix.h"

class ID_DIST_PAIR {
public:
    size_t id;
    float dist;
    ID_DIST_PAIR() : id(0), dist(0) {}
    ID_DIST_PAIR(size_t k, float distance) : id(k), dist(distance) {}
    ID_DIST_PAIR(const ID_DIST_PAIR& pair) : id(pair.id), dist(pair.dist) {}
};

class ID_DIST_HEAP {
private:
    size_t capacity;
    size_t size;
    ID_DIST_PAIR* buffer;
public:
    ID_DIST_HEAP(size_t k) : capacity(k), size(0) {
        buffer = new ID_DIST_PAIR[capacity];
    }
    ~ID_DIST_HEAP() {
        delete[] buffer;
    }
    // 返回一个指向存有k个id的数组
    size_t* getList() {
        size_t* res = new size_t[capacity];
        for (size_t i = 0; i != capacity; ++i) {
            res[i] = buffer[i].id;
        }
        return res;
    }
    // 往堆中插入id-距离对
    void insert(ID_DIST_PAIR pair) {
        size_t index = 0;
        while (index != size && pair.dist >= buffer[index].dist) {
            ++index;
        }
        if (index == size) {
            if (index != capacity) {
                buffer[index] = pair;
            }
        } else {
            for (size_t i = size; i != index; --i) {
                if (i == capacity) {
                    continue;
                }
                buffer[i] = buffer[i - 1];
            }
            buffer[index] = pair;
        }
    }
};

size_t* query(size_t k, Row& src, Matrix& data) {
    ID_DIST_HEAP heap(k);
    for (size_t i = 0; i != data.getRow(); ++i) {
        heap.insert(ID_DIST_PAIR(i, Row::dist(src, data[i])));
    }
    return heap.getList();
}