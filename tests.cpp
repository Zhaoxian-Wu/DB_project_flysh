#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include "Matrix.h"
#include "query.h"

using namespace std;

void testAccurateKNN(Matrix& rowData, Matrix& processedData) {
    const int k = 200;
    const int TEST_VECTOR = 1000;
    const int REPEAT_TIME = 5;

    double sumTimeRaw = 0;
    double sumTimeHandled = 0;

    for (int i = 0; i != REPEAT_TIME; ++i) {
        set<size_t> randomList;
        while (randomList.size() != TEST_VECTOR) {
            randomList.insert(rand() % rowData.getRow());
        }

        // 真正KNN（使用原始数据）
        clock_t start = clock();
        for each (size_t i in randomList) {
            size_t* knn = query(k, rowData[i], rowData);
            delete[] knn;
        }
        clock_t end = clock();
        sumTimeRaw += (end - start);

        // 真正KNN（使用处理后数据）
        sumTimeHandled = 0;
        for (int i = 0; i != REPEAT_TIME; ++i) {
            clock_t start = clock();
            for each (size_t i in randomList) {
                size_t* knn = query(k, processedData[i], processedData);
                delete[] knn;
            }
            clock_t end = clock();
            sumTimeHandled += (end - start);
        }
    }
    printf("求一个查询向量真正KNN（使用处理后数据）的平均时间:%f秒\n", (sumTimeRaw / (TEST_VECTOR * REPEAT_TIME * CLOCKS_PER_SEC)));
    printf("求一个查询向量真正KNN（使用原始数据）的平均时间:%f秒\n", (sumTimeHandled / (TEST_VECTOR * REPEAT_TIME * CLOCKS_PER_SEC)));
}