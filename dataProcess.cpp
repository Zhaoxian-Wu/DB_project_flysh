#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "Matrix.h"

#define PI 3.14159265
#define MEAN 100

// 2.预处理后的数据。
// Preprocess
void preProcess(string sourceMatrixName) {
    // source matrix
    DenseMatrix sMatrix(sourceMatrixName + "_source");
    // preprocess matrix
    DenseMatrix pMatrix(sourceMatrixName + "_preProcess", sMatrix.getRow(), sMatrix.getColumn());
    for (size_t i = 0; i < sMatrix.getRow(); ++i) {
        // compute mean
        int rowMean = 0;
        for (size_t j = 0; j < sMatrix.getColumn(); ++j) {
            rowMean += sMatrix[i][j];
        }
        // set each row
        DenseRow row(sMatrix.getColumn(), i, nullptr);
        for (int j = 0; j < sMatrix.getColumn(); ++j) {
            row[j] = sMatrix[i][j] * MEAN / rowMean;
        }
        pMatrix.setRow(row);
    }
}

// 3.高斯投影矩阵和果蝇投影矩阵。
// build a gaussion projection matrix
// compute a gaussion distribution randomly
float GaussionDistribution() {
    return sqrt(-2.0 * log(rand() / (RAND_MAX + 1.0))) * cos(2.0 * PI * (rand() / (RAND_MAX + 1.0)));
}

void GaussionProjection(string sourceMatrixName, size_t m, size_t dim) {
    DenseMatrix gaussMatrix(sourceMatrixName + "_gaussMatrix", m, dim);
    for (int i = 0; i < m; ++i) {
        DenseRow row(dim, i, nullptr);
        for (int j = 0; j < dim; ++j) {
            row[j] = GaussionDistribution();
        }
        gaussMatrix.setRow(row);
    }
}

// build a fly projection matrix using a given probability p
void FlyProjection(string sourceMatrixName, size_t m, size_t dim, float p) {
    DenseMatrix flyMatrix(sourceMatrixName + "_flyMatrix", m, dim);
    // number of "1"
    int d = dim * p;
    for (size_t i = 0; i < dim; ++i) {
        DenseRow row(dim, i, nullptr);
        for (int count = 0; count < d; ) {
            size_t j = rand() % dim;
            if (flyMatrix[i][j] == 0.0f) {
                row[j] = 1.0f;
                ++count;
            }
        }
        flyMatrix.setRow(row);
    }
}


// 4.使用高斯投影矩阵哈希后的数据。
// build matrix after gaussion project
void GaussionProject(string sourceMatrixName) {
    DenseMatrix preProcessMatrix(sourceMatrixName + "_preProcess");
    DenseMatrix gaussMatrix(sourceMatrixName + "_gaussMatrix", preProcessMatrix.getRow(), preProcessMatrix.getColumn());
    dot(sourceMatrixName + "_gaussProjectMatrix", preProcessMatrix, gaussMatrix.transpose(sourceMatrixName + "_gaussProjectMatrixTrans"));
}

// 5.使用果蝇投影矩阵哈希后的数据。
// build matrix after fly project
void FlyProject(string sourceMatrixName) {
    DenseMatrix preProcessMatrix(sourceMatrixName + "_preProcess");
    DenseMatrix flyMatrix(sourceMatrixName + "_flyMatrix", preProcessMatrix.getRow(), preProcessMatrix.getColumn());
    dot(sourceMatrixName + "_flyProjectMatrix", preProcessMatrix, flyMatrix.transpose(sourceMatrixName + "_flyProjectMatrixTrans"));
}

//this struct must be added too
struct MyStruct {
    double value;
    size_t subscripts;
};

// 6.基于果蝇投影矩阵哈希后，应用了random机制的数据。
Matrix& randomMatrix(string sourceMatrixName, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(sourceMatrixName + "_flyProjectMatrix");

    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    size_t* ran = new size_t[k];
    size_t* r = new size_t[COL];

    size_t j = 0, temp = 0;

    //find the random k columns
    for (size_t i = 0; i<COL; i++) {
        r[i] = i;
    }
    for (size_t i = 0; i < k; i++) {
        j = rand() % ROW;
        temp = r[i];
        r[i] = r[j];
        r[j] = temp;
    }
    for (size_t i = 0; i < k; i++) {
        ran[i] = r[i];
    }

    //save the corresponding columns
    DenseMatrix matrixAfter(sourceMatrixName + "_randomMatrix", ROW, k);

    size_t currentCol = 0;
    for (size_t i = 0; i<k; i++) {

        DenseRow row(k, i, nullptr);
        //DenseRow row((size_t)k, (size_t)i);

        currentCol = ran[i];
        for (size_t x = 0; x < ROW; x++) {
            row[x] = M[x][currentCol];
        }
        matrixAfter.setRow(row);
    }


    delete[] ran;
    delete[] r;
    return matrixAfter;
}

// 7.基于果蝇投影矩阵哈希后，应用了WTA机制的数据。
DenseMatrix& WTAMatrix(string sourceMatrixName, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(sourceMatrixName + "_flyProjectMatrix");
    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    bool* isDelete = new bool[COL];//record if the certain column shou be delete

    DenseMatrix matrixAfter(sourceMatrixName + "_WTAMatrix", ROW, COL);

    //find the top k in every row
    for (size_t i = 0; i < ROW; i++) {
        MyStruct* rowStruct = new MyStruct[COL];
        MyStruct temp;
        memset(isDelete, true, sizeof(bool)*COL);//initialize that every col will be delete   
        for (size_t j = 0; j < COL; j++) {
            rowStruct[j].value = M[i][j];
            rowStruct[j].subscripts = j;
        }

        for (size_t x = 0; x < COL; x++) {
            for (size_t y = x; y < COL; y++) {
                if (rowStruct[x].value < rowStruct[y].value) {//sort from big to small
                    temp = rowStruct[x];
                    rowStruct[x] = rowStruct[y];
                    rowStruct[y] = temp;
                }
            }
        }
        for (size_t x = 0; x <k; x++) {//make the top k num not delete
            isDelete[rowStruct[x].subscripts] = false;
        }

        DenseRow row(COL, i, nullptr);

        for (size_t x = 0; x < COL; x++) {
            if (isDelete[x]) {
                row[x] = 0;
            } else {
                row[x] = M[i][x];
            }
        }
        matrixAfter.setRow(row);
    }
    delete[] isDelete;
    return matrixAfter;
}

// 8.基于果蝇投影矩阵哈希后，应用了binary机制的数据。
DenseMatrix& binaryMatrix(string sourceMatrixName, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(sourceMatrixName + "_flyProjectMatrix");
    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    bool* isDelete = new bool[COL];//record if the certain column shou be delete   

    DenseMatrix matrixAfter(sourceMatrixName + "_binaryMatrix", ROW, COL);
    //find the top k in every row

    for (size_t i = 0; i < ROW; i++) {
        MyStruct* rowStruct = new MyStruct[COL];
        MyStruct temp;
        memset(isDelete, true, sizeof(bool)*COL);//initialize that every col will be delete   
        for (size_t j = 0; j < COL; j++) {
            rowStruct[j].value = M[i][j];
            rowStruct[j].subscripts = j;
        }

        for (size_t x = 0; x < COL; x++) {
            for (size_t y = x; y < COL; y++) {
                if (rowStruct[x].value < rowStruct[y].value) {//sort from big to small
                    temp = rowStruct[x];
                    rowStruct[x] = rowStruct[y];
                    rowStruct[y] = temp;
                }
            }
        }
        for (size_t x = 0; x <k; x++) {//make the top k num not delete
            isDelete[rowStruct[x].subscripts] = false;
        }


        DenseRow row(COL, i, nullptr);

        for (size_t x = 0; x < COL; x++) {
            if (isDelete[x]) {
                row[x] = 0;
            } else {
                row[x] = 1;
            }
        }
        matrixAfter.setRow(row);
    }

    delete[] isDelete;
    return matrixAfter;
}