#include "compileOption.h"

#ifdef TEST_TIME
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <set>

#include "Matrix.h"
#include "query.h"

using namespace std;

//void testAccurateKNN(Matrix& rowData, Matrix& processedData) {
//    const int k = 200;
//    const int TEST_VECTOR = 1000;
//    const int REPEAT_TIME = 5;
//	
//    double sumTimeRaw = 0;
//    double sumTimeHandled = 0;
//
//    for (int i = 0; i != REPEAT_TIME; ++i) {
//        set<size_t> randomList;
//        while (randomList.size() != TEST_VECTOR) {
//            randomList.insert(rand() % rowData.getRow());
//        }
//
//        // 真正KNN（使用原始数据）
//        clock_t start = clock();
//        for each (size_t i in randomList) {
//            Array<size_t> knn = query(k, rowData[i], rowData);
//        }
//        clock_t end = clock();
//        sumTimeRaw += (end - start);
//
//        // 真正KNN（使用处理后数据）
//        sumTimeHandled = 0;
//        for each (size_t i in randomList) {
//            clock_t start = clock();
//            for each (size_t i in randomList) {
//				Array<size_t> knn = query(k, processedData[i], processedData);
//            }
//            clock_t end = clock();
//            sumTimeHandled += (end - start);
//        }
//    }
//    printf("求一个查询向量真正KNN（使用处理后数据）的平均时间:%f秒\n", (sumTimeRaw / (TEST_VECTOR * REPEAT_TIME * CLOCKS_PER_SEC)));
//    printf("求一个查询向量真正KNN（使用原始数据）的平均时间:%f秒\n", (sumTimeHandled / (TEST_VECTOR * REPEAT_TIME * CLOCKS_PER_SEC)));
//}

#define TEST_TIME_PER_QUERY
#ifdef TEST_TIME_PER_QUERY

void testAccurateKNN(Matrix& rowData, Matrix& processedData) {
    const int k = 200;

    size_t i = rand() % rowData.getRow();
    {
        // 真正KNN（使用原始数据）
        double sumTime = 0;
        clock_t start = clock();
        Array<size_t> knn = query(k, Array<float>(rowData.getColumn(), rowData[i]), rowData);
        clock_t end = clock();
        sumTime += (end - start);
        printf("求一个查询向量真正KNN（使用原始数据）的平均时间:%f秒\n", sumTime / CLOCKS_PER_SEC);
    }
    {
        // 真正KNN（使用处理后数据）
        double sumTime = 0;
        clock_t start = clock();
        Array<size_t> knn = query(k, Array<float>(rowData.getColumn(), processedData[i]), processedData);
        clock_t end = clock();
        sumTime += (end - start);
        printf("求一个查询向量真正KNN（使用处理后数据）的平均时间:%f秒\n", (sumTime / CLOCKS_PER_SEC));
    }
}

#endif // TEST_TIME_PRE_QUERY

//#define TEST_TIME_1000_REPEAT_BAT
#ifdef TEST_TIME_1000_REPEAT_BAT
#include <algorithm>
#include <vector>
void testAccurateKNN(Matrix& rawData, Matrix& processedData) {
    const size_t k = 200;
    const size_t TEST_VECTOR = 1000;

    vector<size_t> randomList;
    while (randomList.size() != TEST_VECTOR) {
        size_t randId = rand() % rawData.getRow();
        if (find(randomList.begin(), randomList.end(), randId) == randomList.end()) {
            randomList.push_back(randId);
        }
    }
    sort(randomList.begin(), randomList.end());

    ID_DIST_HEAP heapList[TEST_VECTOR];
    for (int i = 0; i != TEST_VECTOR; ++i) {
        heapList[i] = ID_DIST_HEAP(k);
    }
    {
        // 真正KNN（使用原始数据）
        double sumTime = 0;
        clock_t start = clock();
        size_t count = rawData.getRow();
        for (size_t i = 0; i != count; ++i) {
            Row vec = rawData[i];
            for(size_t index = 0; index != TEST_VECTOR; ++index) {
                heapList[index].insert(ID_DIST_PAIR(i, Row::dist(rawData[randomList[index]], vec)));
            }
            if (i % 1000 == 0) {
                printf("已计算完成%zd行, 耗时%d秒\n", i, (clock() - start) / CLOCKS_PER_SEC);
            }
        }
        clock_t end = clock();
        sumTime += (end - start);
        printf("查询%zd个向量总耗时：%lf秒，求一个查询向量真正KNN（使用原始数据）的平均时间:%lf秒，\n", 
            TEST_VECTOR,
            sumTime / CLOCKS_PER_SEC,
            sumTime / (TEST_VECTOR * CLOCKS_PER_SEC)
        );
    }
    {
        // 真正KNN（使用处理后数据）
        double sumTime = 0;
        clock_t start = clock();
        size_t count = processedData.getRow();
        for (size_t i = 0; i != count; ++i) {
            Row vec = rawData[i];
            for (size_t index = 0; index != TEST_VECTOR; ++index) {
                heapList[index].insert(ID_DIST_PAIR(i, Row::dist(rawData[randomList[index]], vec)));
            }
            if (i % 1000 == 0) {
                printf("已计算完成%zd行, 耗时%d秒\n", i, (clock() - start) / CLOCKS_PER_SEC);
            }
        }
        clock_t end = clock();
        sumTime += (end - start);
        printf("查询%zd个向量总耗时：%f秒，求一个查询向量真正KNN（使用处理后数据）的平均时间:%lf秒，\n",
            TEST_VECTOR,
            sumTime / CLOCKS_PER_SEC,
            sumTime / (TEST_VECTOR * CLOCKS_PER_SEC)
        );
    }
}

#endif // TEST_TIME_1000_REPEAT_BAT


#endif // TEST_TIME