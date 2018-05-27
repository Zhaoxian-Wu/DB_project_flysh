#include <cassert>
#include <cstring>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "Matrix.h"

using namespace std;

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

    size_t vectorNumOfOnePage = getVectorNumPerPage(dimension);
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
    file.read(tempBuffer, sizeof(size_t) * 2);
    row = *reinterpret_cast<size_t*>(tempBuffer);
    dimension = *reinterpret_cast<size_t*>(tempBuffer + sizeof(size_t));
}

float* DenseMatrix::operator[] (size_t _row) {
	size_t vectorNumPerPage = getVectorNumPerPage(dimension);
    size_t PageInDisk = _row / vectorNumPerPage;
    size_t vectorIndex = _row % vectorNumPerPage;
    size_t head = vectorIndex * (sizeof(float) * dimension + sizeof(size_t)) + sizeof(size_t);
    int pageInBuffer = getPageIndexInBuffer(PageInDisk);
    return reinterpret_cast<float*>(getPageBuffer(pageInBuffer) + head);
}

size_t DenseMatrix::getVectorNumPerPage(size_t dimension) {
    return (PAGE_SIZE - sizeof(size_t)) / (sizeof(float) * dimension + sizeof(size_t) * 2);
}

int DenseMatrix::getPageIndexInBuffer(size_t pageInDisk) {
	// 如果缓冲区有对应页，返回页号
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (page[i] == pageInDisk) {
            if (used[i] == 1) {
                return i;
            }
        }
    }
	// 如果缓冲区没有对应页，从磁盘里交换出一页
	int pageInBuffer = getFreePageIndex();
	size_t head = pageInDisk * PAGE_SIZE + FILE_HEAD_SIZE;
	file.seekg(head, ios::beg);
	file.read(buffer[pageInBuffer], PAGE_SIZE);
	usedMatrix[pageInBuffer] = this;
	used[pageInBuffer] = true;
	page[pageInBuffer] = pageInDisk;

	return pageInBuffer;
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

void DenseMatrix::removeSelfFromBuffer(size_t pageInBuffer) {
    used[pageInBuffer] = false;
    usedMatrix[pageInBuffer] = nullptr;
    size_t head = page[pageInBuffer] * PAGE_SIZE + FILE_HEAD_SIZE;
    file.seekp(head, ios::beg);
    file.write(buffer[pageInBuffer], PAGE_SIZE);
}

DenseMatrix::~DenseMatrix() {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (usedMatrix[i] == this) {
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
    tail -= sizeof(size_t) * getVectorNumPerPage(dimension);
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

float dist(const float* a, const float* b, size_t size) {
    float res = 0;
    for (int i = 0; i != size; ++i) {
        float d = a[i] - b[i];
        res += d * d;
    }
    return (float)sqrt(res);
}