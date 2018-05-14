#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "Matrix.h"

using namespace std;

DenseRow::DenseRow(size_t _dimension, size_t _id, char* _buffer) {
    id = _id;
    dimension = _dimension;
    row = new float[dimension];
    buffer = _buffer;
    memcpy(row, buffer, dimension * sizeof(float));
}

DenseRow::~DenseRow() {
    delete[] row;
}

float DenseRow::operator[] (size_t index) {
    return row[index];
}

void DenseRow::set(size_t index) {
    size_t head = sizeof(float) * index;
    float element = row[index];
    memcpy(buffer + head, reinterpret_cast<char*>(&element), sizeof(float));
}

DenseMatrix::DenseMatrix(char* fileName, size_t _vectorNum, size_t _dimension) : vectorNum(_vectorNum), dimension(_dimension) {
    file.open(fileName, ios::out | ios::out | ios::binary);
    file << vectorNum << dimension;
    int vectorNumOfOnePage = getVectorNumOfOnePage(dimension);
    int pageNum = vectorNum / vectorNumOfOnePage + 1;
    size_t needNumOfSize = PAGE_SIZE / sizeof(size_t);
    size_t full = 0;
    for (int i = 0; i < pageNum; i++) {
        for (int j = 0; j < needNumOfSize; j++) {
            file << full;
        }
    }
}


DenseMatrix::DenseMatrix(char* matrixName) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        used[i] = 0;
    }
    file.open(matrixName, ios::out | ios::in | ios::binary);
    file >> vectorNum >> dimension;
}

Row& DenseMatrix::operator[] (size_t _row) {
    size_t pageIndex = _row / getVectorNumOfOnePage(dimension);
    size_t vectorIndex = _row % getVectorNumOfOnePage(dimension);
    if (getPageIndex(pageIndex) != -1) {
        size_t head = vectorIndex * (sizeof(float) * dimension + sizeof(size_t)) + sizeof(size_t);
        return *(new DenseRow(dimension, _row, getPage(pageIndex) + head));
    } else {
        size_t head = pageIndex * PAGE_SIZE;
        head += 8;
        file.seekp(head, ios::beg);
        int freeIndex = getFreePageIndex();
        file.read(buffer[freeIndex], PAGE_SIZE);
        bufferUsed[freeIndex] = 1;
        usedMatrix[freeIndex] = this;
        used[freeIndex] = 1;
        size_t head = vectorIndex * sizeof(float) * dimension + sizeof(size_t);
        page[freeIndex] = pageIndex;
        return *(new DenseRow(dimension, _row, getPage(freeIndex) + head));
    }
}

size_t DenseMatrix::getVectorNumOfOnePage(size_t dimension) {
    return (PAGE_SIZE - sizeof(size_t)) / (sizeof(float) * dimension + sizeof(size_t) * 2);
}
int DenseMatrix::getPageIndex(int pageNum) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (page[i] == pageNum) {
            if (used[i] == 1) {
                return i;
            }
        }
    }
    return -1;
}
char* DenseMatrix::getPage(size_t pageNum) {
    return buffer[pageNum];
}
int DenseMatrix::getFreePageIndex() {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (bufferUsed[i] == 0) {
            return i;
        }
    }
    int tempIndex = rand() % PAGE_NUMBER;
    usedMatrix[tempIndex]->removeSelfFromBuffer(tempIndex);
    return tempIndex;
}
void DenseMatrix::removeSelfFromBuffer(size_t pageIndex) {
    used[pageIndex] = 0;
    size_t myPageIndex = page[pageIndex];
    size_t head = myPageIndex * PAGE_SIZE + 8;
    file.seekp(head, ios::beg);
    file.write(buffer[pageIndex], PAGE_SIZE);
}

DenseMatrix::~DenseMatrix() {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        usedMatrix[i]->removeSelfFromBuffer(i);
    }
    file.close();
}

int DenseMatrix::bufferUsed[PAGE_NUMBER] = { 0 };

Matrix& dot(Matrix& A, Matrix& B) {
    assert(A.getColumn() == B.getRow());
    size_t m = A.getRow();
    size_t n = B.getColumn();
    size_t p = A.getColumn();
    SparesMatrix* CPtr = new SparesMatrix(m, n);
    for (size_t i = 0; i != m; ++i) {
        for (size_t j = 0; j != n; ++j) {
            (*CPtr)[i][j] = 0;
            for (size_t k = 0; k != p; ++k) {
                (*CPtr)[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return *CPtr;
}

float Row::dist(Row& a, Row& b) {
    assert(a.getSize() == b.getSize());
    size_t size = a.getSize();
    float res = 0;
    for (int i = 0; i != size; ++i) {
        float d = a[i] - b[i];
        res += d * d;
    }
}

Matrix& DenseMatrix::transpose() {
    size_t m = getRow();
    size_t n = getColumn();
    SparesMatrix* CPtr = new SparesMatrix(m, n);
    for (size_t i = 0; i != m; ++i) {
        for (size_t j = 0; j != n; ++j) {
            (*CPtr)[i][j] = (*this)[j][i];
        }
    }
    return *CPtr;
}