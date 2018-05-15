#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

#include "Matrix.h"

using namespace std;

void savePage (string filePath, size_t currentPage, size_t size, float vectorSet[20][784], size_t bitMap[20], size_t id[20]);

void savePage (string filePath, size_t currentPage, size_t size, float vectorSet[20][784], size_t bitMap[20], size_t id[20]) {
	fstream in;
	in.open("source.txt", ios::in | ios::binary | ios::app);
	char buffer[PAGE_SIZE];
	int head = 0;
	int tail = PAGE_SIZE - (21 * sizeof(size_t));
	for (int i = 0; i < 20; i++) {
		int tempi = id[i];
		memcpy(buffer + head, reinterpret_cast<char*>(&tempi), sizeof(size_t));
		head += sizeof(int);
		for (int j = 0; j < 784; j++) {
			float tempf = vectorSet[i][j];
			memcpy(buffer + head, reinterpret_cast<char*>(&tempf), sizeof(float));
			head += sizeof(float);
		}
		tempi = bitMap[i];
		memcpy(buffer + tail, reinterpret_cast<char*>(&tempi), sizeof(size_t));
		tail += sizeof(int);
	}
	memcpy(buffer + tail, reinterpret_cast<char*>(&size), sizeof(size_t));
	in.write(buffer, PAGE_SIZE);
	in.close();
}

//#define generate
#ifdef generate
int main() {
    fstream in;
    in.open("mnist10k", ios::in | ios::binary);
    in.seekp(0, ios::beg);
    char fileHead[sizeof(size_t) * 2];
    in.write(fileHead, sizeof(size_t) * 2);
    in.close();
    size_t currentPage = 0;
    int currentVector = 0;
    int dimension = 784;
    float tempVectorSet[20][784] = { { 0 } };
    int tempVectorNum = 0;
    int tempFloatNum = 0;
    size_t tempBitMap[20] = { 0 };
    size_t tempId[20] = { 0 };
    int tempSize = 0;

    char tempc;
    float tempf;
    int tempi;
    fstream out;
    string tempStr;
    out.open("mnist10k.txt", ios::in | ios::binary);
    while (getline(out, tempStr, ',')) {
        if (tempFloatNum != 783) {
            tempVectorSet[tempVectorNum][tempFloatNum] = atof(tempStr.c_str());
            tempFloatNum++;
            //			cout << tempStr << ' ';
            //cout << tempf << " floatNum " << tempFloatNum;
        } else {
            //cout << "write page" << ' ' << tempc << " " << tempf << ' ';
            //			cout << "float number : " << tempFloatNum << endl;
            tempVectorSet[tempVectorNum][tempFloatNum] = atof(tempStr.c_str());
            tempFloatNum = 0;
            tempId[tempVectorNum] = currentVector;
            tempBitMap[tempVectorNum] = 1;
            tempVectorNum++;
            currentVector++;
        }
        if (tempVectorNum == 20) {
            currentPage++;
            cout << " \n current page" << currentPage << endl;
            savePage("", currentPage, tempVectorNum, tempVectorSet, tempBitMap, tempId);
            tempVectorNum = 0;
        }
    }
    if (tempVectorNum != 0) {
        savePage("", currentPage, tempVectorNum, tempVectorSet, tempBitMap, tempId);
    }
    out.close();
    in.open("source.txt", ios::in | ios::binary);
    in.seekp(0, ios::beg);
    memcpy(fileHead, reinterpret_cast<char*>(&currentVector), sizeof(size_t));
    memcpy(fileHead + sizeof(size_t), reinterpret_cast<char*>(&dimension), sizeof(size_t));
    in.write(fileHead, sizeof(size_t) * 2);
    in.close();
}
#endif // generate

//int main() {
//    DenseMatrix M("source.txt");
//    for (int i = 0; i != 1; ++i) {
//        for (int j = 0; j != M.getColumn(); ++j) {
//            cout << M[i][j] << endl;
//        }
//    }
//    cout << M[0][0] << endl;
//}