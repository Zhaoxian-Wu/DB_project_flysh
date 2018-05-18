#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

#include "Matrix.h"

#define PI 3.14159265
#define MEAN 100

using namespace std;

// 2.预处理后的数据。
// Preprocess
void preProcess(string dataSet) {
    // source matrix
    DenseMatrix sMatrix(dataSet + "_source");

    size_t row = sMatrix.getRow();
    size_t column = sMatrix.getColumn();

    // preprocess matrix
    DenseMatrix pMatrix(dataSet + "_preProcess", row, column);

    for (size_t i = 0; i < row; ++i) {
        // compute mean
        float rowMean = 0.0;
        for (size_t j = 0; j < column; ++j) {
            rowMean += sMatrix[i][j];
        }
        rowMean /= column;
        // set each row
        Row row(column, i, nullptr);
        for (size_t j = 0; j < column; ++j) {
            row[j] = sMatrix[i][j] * MEAN / rowMean;
        }
        pMatrix.setRow(row);
		cout << "row " << i << endl;
    }
    pMatrix.showPage(1);
}

// 3.高斯投影矩阵和果蝇投影矩阵。
// build a gaussion projection matrix
// compute a gaussion distribution randomly
float GaussionDistribution() {
    return (float)(sqrt(-2.0 * log(rand() / (RAND_MAX + 1.0))) * cos(2.0 * PI * (rand() / (RAND_MAX + 1.0))));
}

void GaussionMatrix(string dataSet, size_t m, size_t dim) {
    srand((unsigned int)time(NULL));
    DenseMatrix gaussMatrix(dataSet + "_gaussMatrix", m, dim);
    for (int i = 0; i < m; ++i) {
        Row row(dim, i, nullptr);
        for (int j = 0; j < dim; ++j) {
            row[j] = GaussionDistribution();
        }
        gaussMatrix.setRow(row);
    }
}

// build a fly projection matrix using a given probability p
void FlyMatrix(string dataSet, size_t m, size_t dim, float p) {
    srand((unsigned int)time(NULL));
    DenseMatrix flyMatrix(dataSet + "_flyMatrix", m, dim);
    // number of "1"
    int d = (int) (dim * p);
    for (size_t i = 0; i < dim; ++i) {
        Row row(dim, i, nullptr);
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
void GaussionProject(string dataSet) {
    DenseMatrix preProcessMatrix(dataSet + "_preProcess");
    DenseMatrix gaussMatrix(dataSet + "_gaussMatrix");
    gaussMatrix.transpose(dataSet + "_gaussMatrixTrans");
    DenseMatrix gaussMatrixTrans(dataSet + "_gaussMatrixTrans");
    dot(dataSet + "_gaussProjectMatrix", preProcessMatrix, gaussMatrixTrans);
}

// 5.使用果蝇投影矩阵哈希后的数据。
// build matrix after fly project
void FlyProject(string dataSet) {
    DenseMatrix preProcessMatrix(dataSet + "_preProcess");
    DenseMatrix flyMatrix(dataSet + "_flyMatrix");
    flyMatrix.transpose(dataSet + "_flyMatrixTrans");
    DenseMatrix flyMatrixTrans(dataSet + "_flyMatrixTrans");
    dot(dataSet + "_flyProjectMatrix", preProcessMatrix, flyMatrixTrans);
}

//this struct must be added too
struct MyStruct {
    double value;
    size_t subscripts;
};

// 6.基于果蝇投影矩阵哈希后，应用了random机制的数据。
void randomMatrix(string dataSet, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(dataSet + "_source");

    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    srand((unsigned)time(NULL));
    size_t* ran = new size_t[k];
    size_t* r = new size_t[COL];

    size_t j = 0, temp = 0;

    //find the random k columns
    for (size_t i = 0; i<COL; i++) {
        r[i] = i;
    }
    for (size_t i = 0; i < k; i++) {
        j = rand() % COL;
        temp = r[i];
        r[i] = r[j];
        r[j] = temp;
    }
    for (size_t i = 0; i < k; i++) {
        ran[i] = r[i];
    }

    //save the corresponding columns
    DenseMatrix*  matrixAfter = new DenseMatrix(dataSet + "_randomMatrix", ROW, k);

    size_t currentCol = 0;
    for (size_t i = 0; i<ROW; i++) {

        Row row(k, i, nullptr);//k维，第i行

        for (size_t x = 0; x < k; x++) {//保留第i行的k个currentCol
            currentCol = ran[x];

            double tt = M[i][currentCol];
            row[x] = M[i][currentCol];
        }
        matrixAfter->setRow(row);
    }

    delete[] ran;
    delete[] r;
    delete matrixAfter;
}

// 7.基于果蝇投影矩阵哈希后，应用了WTA机制的数据。
void WTAMatrix(string dataSet, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(dataSet + "_source");
    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    bool* isDelete = new bool[COL];//record if the certain column shou be delete


    DenseMatrix*  matrixAfter = new DenseMatrix(dataSet + "_WTAMatrix", ROW, COL);

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

        Row row(COL, i, nullptr);

        for (size_t x = 0; x < COL; x++) {
            if (isDelete[x]) {
                row[x] = 0;//row[x]=matrix[i][x]
            } else {
                row[x] = M[i][x];
            }
        }
        matrixAfter->setRow(row);
        delete[] rowStruct;
    }


    delete[] isDelete;
    delete matrixAfter;
}


// 8.基于果蝇投影矩阵哈希后，应用了binary机制的数据。
void binaryMatrix(string dataSet, size_t k) {
    //assum that M is the target matrix
    DenseMatrix M(dataSet + "_source");
    size_t ROW = M.getRow();
    size_t COL = M.getColumn();

    bool* isDelete = new bool[COL];//record if the certain column shou be delete   

    DenseMatrix*  matrixAfter = new DenseMatrix(dataSet + "_binaryMatrix", ROW, COL);
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


        Row row(COL, i, nullptr);
        for (size_t x = 0; x < COL; x++) {
            if (isDelete[x]) {
                row[x] = 0;
            } else {
                row[x] = 1;
            }
        }
        matrixAfter->setRow(row);
        delete[] rowStruct;
    }
    delete[] isDelete;
    delete matrixAfter;
}