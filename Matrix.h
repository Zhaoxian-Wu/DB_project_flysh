#pragma once
#include <string>
#define PAGE_NUMBER 50
#define PAGE_SIZE (64 * 1024)

using std::fstream;
using std::string;

class Matrix {
public:
    virtual float* operator[] (size_t) = 0;
    size_t getRow() {
        return row;
    }
    size_t getColumn() {
        return dimension;
    }
protected:
    size_t row;
    size_t dimension;
};

class DenseMatrix : public Matrix {
public:
    DenseMatrix(string matrixName);
    DenseMatrix(string matrixName, size_t _vectorNum, size_t _dimension);
    ~DenseMatrix();

    float* operator[] (size_t);

    // 通知矩阵：某一行元素内容已修改
    void setRow(size_t row);

    void showPage(int);
private:
    // 得到指定页号的page在页管理器中的位置，若页不存在于缓冲区中，则从磁盘中交换出来
    int getPageIndexInBuffer(size_t pageNum);

    // 计算一页可以存多少向量
    static size_t getVectorNumPerPage(size_t dimension);

    // 得到空闲的页面编号
    int getFreePageIndex();

    // 腾出指定位号的缓冲区空间
    void removeSelfFromBuffer(size_t pageIndex);

    static char buffer[PAGE_NUMBER][PAGE_SIZE];		//buffer page
    static size_t page[PAGE_NUMBER];				//page id of matrix
    static DenseMatrix* usedMatrix[PAGE_NUMBER];	//matrix who used this page
    static bool hasChange[PAGE_NUMBER];

    static const size_t FILE_HEAD_SIZE = sizeof(size_t) * 2;
    static const string dir;

    FILE* file;
    bool used[PAGE_NUMBER];
    size_t vectorNumPerPage;
};

float dist(const float* a, const float* b, size_t size);