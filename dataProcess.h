#pragma once
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "Matrix.h"

// 2.预处理后的数据。
// Preprocess
void preProcess(string sourceMatrixName);

// 3.高斯投影矩阵和果蝇投影矩阵。
// build a gaussion projection matrix
// compute a gaussion distribution randomly
float GaussionDistribution();

void GaussionProjection(string sourceMatrixName, size_t m, size_t dim);

// build a fly projection matrix using a given probability p
void FlyProjection(string sourceMatrixName, size_t m, size_t dim, float p);


// 4.使用高斯投影矩阵哈希后的数据。
// build matrix after gaussion project
void GaussionProject(string sourceMatrixName);

// 5.使用果蝇投影矩阵哈希后的数据。
// build matrix after fly project
void FlyProject(string sourceMatrixName);

// 6.基于果蝇投影矩阵哈希后，应用了random机制的数据。
Matrix& randomMatrix(string sourceMatrixName, size_t k);

// 7.基于果蝇投影矩阵哈希后，应用了WTA机制的数据。
DenseMatrix& WTAMatrix(string sourceMatrixName, size_t k);

// 8.基于果蝇投影矩阵哈希后，应用了binary机制的数据。
DenseMatrix& binaryMatrix(string sourceMatrixName, size_t k);