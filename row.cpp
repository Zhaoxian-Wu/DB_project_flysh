#include <cassert>
#include "Matrix.h"

Row::Row(const Row& other) {
    id = other.id;
    dimension = other.dimension;
    row = new float[dimension];
    memcpy(row, other.row, dimension * sizeof(float));
}

Row::Row(size_t _dimension, size_t _id, char* buffer) {
    id = _id;
    dimension = _dimension;
    row = new float[dimension];
    if (buffer) {
        memcpy(row, buffer, dimension * sizeof(float));
    } else {
        memset(row, 0, dimension * sizeof(float));
    }
}

Row::~Row() {
    delete[] row;
}

Row& Row::operator=(const Row&other) {
    dimension = other.dimension;
    if (row) {
        delete[] row;
    }
    row = new float[dimension];
    memcpy(row, other.row, dimension * sizeof(float));
    return *this;
}

float& Row::operator[] (size_t index) {
    assert(0 <= index && index < dimension);
    return row[index];
}

float Row::operator[] (size_t index) const {
    assert(0 <= index && index < dimension);
    return row[index];
}

float Row::dist(const Row& a, const Row& b) {
    assert(a.getColumn() == b.getColumn());
    size_t size = a.getColumn();
    float res = 0;
    for (int i = 0; i != size; ++i) {
        float d = a[i] - b[i];
        res += d * d;
    }
    return (float)sqrt(res);
}