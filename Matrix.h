#pragma once
#include <fstream>

#define PAGE_NUMBER 50
#define PAGE_SIZE (64 * 1024)

using std::fstream;

class Row {
public:
    static float dist(Row&, Row&);
    virtual float operator[] (size_t) = 0;
    virtual void set(size_t index) = 0;
    size_t getID() {
        return id;
    };
    size_t getSize() {
        return dimension;
    }
protected:
    size_t dimension;
    size_t id; // id其实就是矩阵的行数
};

class Matrix {
public:
    virtual Row& operator[] (size_t) = 0;
    virtual Matrix& transpose() = 0;
    size_t getRow() {
        return row;
    }
    size_t getColumn() {
        return column;
    }
private:
    size_t row;
    size_t column;
};

class SparseRow : Row {

};

class SparesMatrix : public Matrix{
public:
    SparesMatrix(int row, int col);
    SparesMatrix(char* matrixName);
    Row& operator[] (size_t);
    Matrix& transpose();
};

class DenseRow : public Row {
public:
    ~DenseRow();
    DenseRow(size_t _dimension, size_t _id, char* _buffer);
    float operator[] (size_t);
    void set(size_t index);
private:
    float* row;
    size_t dimension;
    char* buffer;
};

class DenseMatrix : public Matrix {
public:
    DenseMatrix(char* matrixName);
    DenseMatrix(char* fileName, size_t _vectorNum, size_t _dimension);
    Row& operator[] (size_t);
    Matrix& transpose();

    Row& operator[] (size_t _row);
    int getPageIndex(int pageNum);

    void setRow(size_t rid, size_t index, float value);

    ~DenseMatrix();
private:
    static size_t getVectorNumOfOnePage(size_t dimension);

    char* getPage(size_t pageNum);
    int getFreePageIndex();
    void removeSelfFromBuffer(size_t pageIndex);

    static char buffer[PAGE_NUMBER][PAGE_SIZE];		//buffer page
    static size_t page[PAGE_NUMBER];				//page id of matrix
    static int bufferUsed[PAGE_NUMBER];				//page states of buffer
    static DenseMatrix* usedMatrix[PAGE_NUMBER];	//matrix who used this page
    fstream file;
    size_t vectorNum;
    size_t dimension;
    int used[PAGE_NUMBER];
};

Matrix& dot(Matrix&, Matrix&);