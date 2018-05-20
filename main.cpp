#include <ctime>
#include <cstdlib>

#include "compileOption.h"
#include "Matrix.h"

#ifdef PROCESS
#include <iostream>
#include "dataProcess.h"

using namespace std;

int main() {
    string dataset[] = {
        "mnist",
        //"glove"
    };
    srand((unsigned)time(NULL));
    for (int i = 0; i != 1; ++i) {
        DenseMatrix M(dataset[i] + "_source");

        // 2.预处理后的数据。
		cout << "[开始预处理]" << endl;
        preProcess(dataset[i]);

        // 3.高斯投影矩阵和果蝇投影矩阵。
        size_t vecCount = M.getRow();
        size_t dimension = M.getColumn();
		cout << "[开始创建高斯投影矩阵]" << endl;
		GaussionMatrix(dataset[i], 32, dimension);
		cout << "[开始创建果蝇投影矩阵]" << endl;
		FlyMatrix(dataset[i], 32, dimension, 0.1);

        // 4.使用高斯投影矩阵哈希后的数据。
		cout << "[开始使用高斯投影矩阵哈希数据]" << endl;
		GaussionProject(dataset[i]);

        // 5.使用果蝇投影矩阵哈希后的数据。
		cout << "[开始使用果蝇投影矩阵哈希数据]" << endl;
		FlyProject(dataset[i]);

        // 6.基于果蝇投影矩阵哈希后，应用了random机制的数据。
		cout << "[开始应用random机制]" << endl;
		randomMatrix(dataset[i], 32);

        // 7.基于果蝇投影矩阵哈希后，应用了WTA机制的数据。
		cout << "[开始应用WTA机制]" << endl;
		WTAMatrix(dataset[i], 32);

        // 8.基于果蝇投影矩阵哈希后，应用了binary机制的数据。
		cout << "[开始应用binary机制]" << endl;
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
        //"glove",
        "mnist"
    };
    for (int i = 0; i != 1; ++i) {
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


#ifdef OTHER
#include <iostream>
using namespace std;
int main() {
    DenseMatrix M("mnist_preProcess");
    //M.showPage(10);
	for (int i = 0; i != M.getRow(); ++i) {
		float sum = 0;
		for (int j = 0; j != M.getColumn(); ++j) {
			sum += M[i][j];
		}
		cout << (sum /= M.getColumn()) << endl;
	}
}
#endif // OTHER

//int main() {
//    {
//        fstream fs("mnist");
//        string str;
//        clock_t start = clock();
//        while (getline(fs, str)) {
//        }
//        clock_t end = clock();
//        printf("读取文件时间:%d秒\n", (end - start) / CLOCKS_PER_SEC);
//    }
//    {
//        clock_t start = clock();
//        DenseMatrix M("mnist_source");
//        for (int i = 0; i != M.getRow(); ++i) {
//            Row r = M[i];
//        }
//        clock_t end = clock();
//        printf("遍历矩阵时间:%d秒\n", (end - start) / CLOCKS_PER_SEC);
//    }
//}