#include <cassert>
#include <cstring>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "Matrix.h"

using namespace std;

Row::Row(const Row& other) {
    *this = other;
}

Row::Row(size_t _dimension, size_t _id, char* buffer = nullptr) {
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
    return row[index];
}

DenseMatrix::DenseMatrix(string matrixName, size_t _vectorNum, size_t _dimension) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        used[i] = 0;
    }
    string f = dir + matrixName;
    row = _vectorNum;
    dimension = _dimension;
    file.open(f.c_str(), ios::out | ios::binary | ios::app);
    //write the head of file
    char fileHead[FILE_HEAD_SIZE];
    memcpy(fileHead, reinterpret_cast<char*>(&row), sizeof(size_t));
    memcpy(fileHead + sizeof(size_t), reinterpret_cast<char*>(&dimension), sizeof(size_t));
    file.write(fileHead, sizeof(size_t) * 2);
    //
    size_t vectorNumOfOnePage = getVectorNumOfOnePage(dimension);
    size_t pageNum = row / vectorNumOfOnePage;
    size_t needNumOfSize = PAGE_SIZE / sizeof(size_t);
    
    size_t currentVector = 0;
	char buffer[PAGE_SIZE];
	size_t vectorNum = 0;
    size_t tempi = 1;
    float tempf = 0;
    //save page
    for (size_t i = 0; i < pageNum; i++) {
		size_t head = 0;
		size_t tail = PAGE_SIZE - ((vectorNumOfOnePage + 1) * sizeof(size_t)) - 1;
    	//write the vector
        for (size_t j = 0; j < vectorNumOfOnePage; j++) {
        	//write the vector id
        	memcpy(buffer + head, reinterpret_cast<char*>(&currentVector), sizeof(size_t));
        	head += sizeof(size_t);
        	currentVector++;
        	//write the vector
        	for (int k = 0; k < dimension; k++) {
        		memcpy(buffer + head, reinterpret_cast<char*>(&tempf), sizeof(float));
        		head += sizeof(float);
			}
			//write the bit map
			memcpy(buffer + tail, reinterpret_cast<char*>(&tempi), sizeof(size_t));
			tail += sizeof(size_t);
        }
        //write vector number of page
		memcpy(buffer + tail, reinterpret_cast<char*>(&vectorNumOfOnePage), sizeof(size_t));
		file.write(buffer, PAGE_SIZE);
    }
    size_t restVector = row - pageNum * vectorNumOfOnePage;
    if (restVector != 0) {
    	size_t head = 0;
		size_t tail = PAGE_SIZE - ((restVector + 1) * sizeof(size_t)) - 1;
    	//write the vector
        for (size_t j = 0; j < restVector; j++) {
        	//write the vector id
        	memcpy(buffer + head, reinterpret_cast<char*>(&currentVector), sizeof(size_t));
        	head += sizeof(size_t);
        	currentVector++;
        	//write the vector
        	for (int k = 0; k < dimension; k++) {
        		memcpy(buffer + head, reinterpret_cast<char*>(&tempf), sizeof(float));
        		head += sizeof(float);
			}
			//write the bit map
			memcpy(buffer + tail, reinterpret_cast<char*>(&tempi), sizeof(size_t));
			tail += sizeof(size_t);
        }
        //write vector number of page
		memcpy(buffer + tail, reinterpret_cast<char*>(&restVector), sizeof(size_t));
		file.write(buffer, PAGE_SIZE);
	}
    file.close();
    file.open(f.c_str(), ios::in | ios::out | ios::binary);
}

DenseMatrix::DenseMatrix(string matrixName) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        used[i] = 0;
    }
    file.open((dir + matrixName).c_str(), ios::out | ios::in | ios::binary);
    char tempBuffer[sizeof(size_t) * 2];
    file.seekg(0, ios::beg);
    file.read(tempBuffer, sizeof(size_t) * 2);
    memcpy(&row, tempBuffer, sizeof(size_t));
    memcpy(&dimension, tempBuffer + sizeof(size_t), sizeof(size_t));
}

Row DenseMatrix::operator[] (size_t _row) {
    size_t pageIndex = _row / getVectorNumOfOnePage(dimension);
    size_t vectorIndex = _row % getVectorNumOfOnePage(dimension);
    int freeIndex = getPageIndex(pageIndex);
    if (freeIndex == -1) {
        size_t head = pageIndex * PAGE_SIZE + FILE_HEAD_SIZE;
        file.seekg(head, ios::beg);
        freeIndex = getFreePageIndex();
        file.read(buffer[freeIndex], PAGE_SIZE);
    }
    usedMatrix[freeIndex] = this;
    used[freeIndex] = 1;
    page[freeIndex] = pageIndex;
    size_t head = vectorIndex * (sizeof(float) * dimension + sizeof(size_t)) + sizeof(size_t);
    //return *(new Row(dimension, _row, getPageBuffer(freeIndex) + head));
    return Row(dimension, _row, getPageBuffer(freeIndex) + head);
}

size_t DenseMatrix::getVectorNumOfOnePage(size_t dimension) {
    return (PAGE_SIZE - sizeof(size_t)) / (sizeof(float) * dimension + sizeof(size_t) * 2);
}

int DenseMatrix::getPageIndex(size_t pageNum) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (page[i] == pageNum) {
            if (used[i] == 1) {
                return i;
            }
        }
    }
    return -1;
}

char* DenseMatrix::getPageBuffer(size_t pageNum) {
    return buffer[pageNum];
}

int DenseMatrix::getFreePageIndex() {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if(usedMatrix[i] == nullptr) {
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
    size_t head = myPageIndex * PAGE_SIZE + FILE_HEAD_SIZE;
    usedMatrix[pageIndex] = nullptr;
    file.seekp(head, ios::beg);
    file.write(buffer[pageIndex], PAGE_SIZE);
}

void DenseMatrix::setRow(Row& row) {
    assert(row.getColumn() == this->dimension);
    size_t id = row.getID();
    //get pageIndex of Matrix
    size_t myPageIndex = id / getVectorNumOfOnePage(dimension);
    size_t vectorIndex = id % getVectorNumOfOnePage(dimension);
    //get pageIndex of buffer
    int pageIndex = getPageIndex(myPageIndex);
    //if this page not in buffer
    if (pageIndex == -1) {
        size_t head = myPageIndex * PAGE_SIZE + FILE_HEAD_SIZE;
        file.seekg(head, ios::beg);
        pageIndex = getFreePageIndex();
        file.read(buffer[pageIndex], PAGE_SIZE);
        usedMatrix[pageIndex] = this;
        used[pageIndex] = 1;
        page[pageIndex] = myPageIndex;
    }
    size_t head = vectorIndex * (sizeof(float) * dimension + sizeof(size_t)) + sizeof(size_t);
    memcpy(getPageBuffer(pageIndex) + head, row.getBuffer(), dimension * sizeof(float));
}

DenseMatrix::~DenseMatrix() {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (usedMatrix[i]) {
            usedMatrix[i]->removeSelfFromBuffer(i);
        }
    }
    file.close();
}

void DenseMatrix::showPage(int pageNum) {
    size_t head = pageNum * PAGE_SIZE + FILE_HEAD_SIZE;
    size_t tail = 0;
    char tempBuffer[PAGE_SIZE];
    file.seekg(head, ios::beg);
    file.read(tempBuffer, PAGE_SIZE);
    tail = PAGE_SIZE - sizeof(size_t) - 1;
    size_t tempSize;
    memcpy(&tempSize, reinterpret_cast<size_t*>(tempBuffer + tail), sizeof(size_t));
    cout << "page " << pageNum << " has vector: " << tempSize << endl;
    tail -= sizeof(size_t) * getVectorNumOfOnePage(dimension);
    head = 0;
    size_t tempS = 0;
    float tempF = 0;
    for (size_t i = 0; i < tempSize; ) {
        memcpy(&tempS, reinterpret_cast<size_t*>(tempBuffer + tail), sizeof(size_t));
        tail += sizeof(size_t);
        if (tempS == 1) {
            memcpy(&tempS, reinterpret_cast<size_t*>(tempBuffer + head), sizeof(size_t));
            cout << "id: " << tempS << ' ';
            head += sizeof(size_t);
            for (size_t j = 0; j < dimension; j++) {
                memcpy(&tempF, reinterpret_cast<float*>(tempBuffer + head), sizeof(float));
                cout << tempF << ',';
                head += sizeof(float);
            }
            cout << endl;
            i++;
        }
    }
}

DenseMatrix* DenseMatrix::usedMatrix[PAGE_NUMBER] = { nullptr };
const string DenseMatrix::dir = "dataDir/";
char DenseMatrix::buffer[PAGE_NUMBER][PAGE_SIZE] = {};
size_t DenseMatrix::page[PAGE_NUMBER] = {};

Matrix& dot(string newMatrixName, Matrix& A, Matrix& B) {
    assert(A.getColumn() == B.getRow());
    size_t m = A.getRow();
    size_t n = B.getColumn();
    size_t p = A.getColumn();
    B.transpose("temp");
    DenseMatrix temp("temp");
    DenseMatrix C(newMatrixName, m, n);
    for (size_t i = 0; i != m; ++i) {
        Row row(n, i);
        for (size_t j = 0; j != n; ++j) {
            Row aRow = A[i];
            Row bCol = temp[j];
            row[j] = 0;
            for (size_t k = 0; k != p; ++k) {
                row[j] += aRow[k] * bCol[k];
            }
            C.setRow(row);
        }
    }
    return C;
}

float Row::dist(Row& a, Row& b) {
    assert(a.getColumn() == b.getColumn());
    size_t size = a.getColumn();
    float res = 0;
    for (int i = 0; i != size; ++i) {
        float d = a[i] - b[i];
        res += d * d;
    }
    return (float)sqrt(res);
}

void DenseMatrix::transpose(string newMatrixName) {
    size_t m = getRow();
    size_t n = getColumn();
    DenseMatrix C(newMatrixName, n, m);
    for (size_t i = 0; i != n; ++i) {
        Row row(m, i);
        for (size_t j = 0; j != m; ++j) {
            row[j] = (*this)[j][i];
        }
        C.setRow(row);
    }
}
