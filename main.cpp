#include <ctime>

#include "Matrix.h"

//#define PROCESS
//#define TEST_TIME
//#define DISPLAY_RESULT

#ifdef PROCESS
#include "dataProcess.h"
int main() {
    string dataset[] = {
        "glove",
        "mnist"
    };
    srand((unsigned)time(NULL));
    for (int i = 0; i != 2; ++i) {
        Matrix& M = DenseMatrix(dataset[i]);
        size_t vecCount = M.getRow();
        size_t dimension = M.getColumn();

        // 2.预处理后的数据。
        // Preprocess
        preProcess(dataset[i]);

        // 3.高斯投影矩阵和果蝇投影矩阵。
        GaussionProjection(dataset[i], vecCount, dimension);

        // build a fly projection matrix using a given probability p
        FlyProjection(dataset[i], vecCount, dimension, 0.1);

        // 4.使用高斯投影矩阵哈希后的数据。
        // build matrix after gaussion project
        GaussionProject(dataset[i]);

        // 5.使用果蝇投影矩阵哈希后的数据。
        // build matrix after fly project
        FlyProject(dataset[i]);

        // 6.基于果蝇投影矩阵哈希后，应用了random机制的数据。
        randomMatrix(dataset[i], 32);

        // 7.基于果蝇投影矩阵哈希后，应用了WTA机制的数据。
        WTAMatrix(dataset[i], 32);

        // 8.基于果蝇投影矩阵哈希后，应用了binary机制的数据。
        binaryMatrix(dataset[i], 32);
    }
}
#endif // PROCESS

#ifdef TEST_TIME
#include <iostream>
using namespace std;
void testAccurateKNN(Matrix& rowData, Matrix& handledData);
int main() {
    srand((unsigned)time(NULL));
    string dataset[] = {
        "glove",
        "mnist"
    };
    for (int i = 0; i != 2; ++i) {
        cout << "数据集" << dataset[i] << "测试:" << endl;
        testAccurateKNN(DenseMatrix(dataset[i] + "_source"), DenseMatrix(dataset[i] + "_preProcess"));
    }
}
#endif //TEST_TIME

#ifdef DISPLAY_RESULT
int main() {
    string dataset[] = {
        "glove",
        "mnist"
    };
    string set = dataset[0];
    // 1
    DenseMatrix(set + "_source").showPage(1);
    // 2
    DenseMatrix(set + "_preProcess").showPage(1);
    // 3 
    DenseMatrix(set + "_gaussMatrix").showPage(1);
    DenseMatrix(set + "_flyMatrix").showPage(1);
    // 4
    DenseMatrix(set + "_gaussProjectMatrix").showPage(1);
    // 5
    DenseMatrix(set + "_flyProjectMatrix").showPage(1);
    // 6
    DenseMatrix(set + "_randomMatrix").showPage(1);
    // 7
    DenseMatrix(set + "_WTAMatrix").showPage(1);
    // 8
    DenseMatrix(set + "_binaryMatrix").showPage(1);
}
#endif // DISPLAY_RESULT
