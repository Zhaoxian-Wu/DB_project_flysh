#include <cassert>

#include "Array.h"
#include "Matrix.h"
#include "query.h"

#include <vector>
using namespace std;

ID_DIST_PAIR::ID_DIST_PAIR() : id(0), dist(0) {}
ID_DIST_PAIR::ID_DIST_PAIR(size_t k, float distance) : id(k), dist(distance) {}
ID_DIST_PAIR::ID_DIST_PAIR(const ID_DIST_PAIR& pair) : id(pair.id), dist(pair.dist) {}

ID_DIST_HEAP::ID_DIST_HEAP() = default;

ID_DIST_HEAP::ID_DIST_HEAP(size_t k) : capacity(k), size(0) {
    buffer = new ID_DIST_PAIR[capacity];
	hasSort = false;
}
ID_DIST_HEAP::~ID_DIST_HEAP() {
    delete[] buffer;
}

ID_DIST_HEAP& ID_DIST_HEAP::operator=(const ID_DIST_HEAP& other) {
    capacity = other.capacity;
    size = other.size;
    buffer = new ID_DIST_PAIR[capacity];
    memcpy(buffer, other.buffer, sizeof(ID_DIST_PAIR) * capacity);
    hasSort = other.hasSort;
    return *this;
}

// 返回一个指向存有k个id的数组
Array<size_t> ID_DIST_HEAP::getList() {
	if (!hasSort) {
		for (size_t last = size - 1; last != 0; --last) {
			ID_DIST_PAIR temp = buffer[last];
			buffer[last] = buffer[0];
			buffer[0] = temp;
            adjust(0, last);
		}
		hasSort = true;
	}
	Array<size_t> res(size);
	for (int i = 0; i != size; ++i) {
		res[i] = buffer[i].id;
	}
	return res;
}
// 往堆中插入id-距离对
void ID_DIST_HEAP::insert(ID_DIST_PAIR pair) {
	assert(!hasSort);
	if (size < capacity) {
		buffer[size] = pair;
		// index一直指向插入的pair
		size_t index = size;
		size_t parent = (index + 1) / 2 - 1;
		while (index > 0 && buffer[parent].dist < pair.dist) {
			buffer[index] = buffer[parent];
			buffer[parent] = pair;
			index = parent;
			parent = (index + 1) / 2 - 1;
		}
		++size;
	} else if(pair.dist < buffer[0].dist) {
		// 这个堆事实上是大根堆，如果新距离小于堆顶，则替换掉堆顶元素
		buffer[0] = pair;
		adjust(0, capacity);
	}
}

// 自上而下调整堆
void ID_DIST_HEAP::adjust(size_t index, size_t last) {
	ID_DIST_PAIR pair = buffer[index];
	size_t child = 2 * index + 1;
	if (child + 1 < last && buffer[child + 1].dist > buffer[child].dist) {
		child = child + 1;
	}
	while (child < last && buffer[child].dist > pair.dist) {
		buffer[index] = buffer[child];
		index = child;
		child = 2 * child + 1;
		if (child + 1 < last && buffer[child + 1].dist > buffer[child].dist) {
			child = child + 1;
		}
	}
	buffer[index] = pair;
}

Array<size_t> query(size_t k, Array<float>& src, Matrix& data) {
    ID_DIST_HEAP heap(k);
    size_t dimension = data.getColumn();
    for (size_t i = 0; i != data.getRow(); ++i) {
        heap.insert(ID_DIST_PAIR(i, dist(src.getBuffer(), data[i], dimension)));
    }
    return heap.getList();
}