#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector> 

#define PAGE_NUMBER 50
#define PAGE_SIZE (64 * 1024)

//#define DATA_SET "mnist"
//#define FILE_DIMENSION 784
//#define PAGE_VECTOR_NUM 20

#define DATA_SET "glove"
#define FILE_DIMENSION 300
#define PAGE_VECTOR_NUM 54

using namespace std;

#define GENERATE

#ifdef GENERATE
void savePage (string filePath, size_t currentPage, size_t size, float vectorSet[PAGE_VECTOR_NUM][FILE_DIMENSION], size_t bitMap[PAGE_VECTOR_NUM], size_t id[PAGE_VECTOR_NUM]);

void savePage (string filePath, size_t currentPage, size_t size, float vectorSet[PAGE_VECTOR_NUM][FILE_DIMENSION], size_t bitMap[PAGE_VECTOR_NUM], size_t id[PAGE_VECTOR_NUM]) {
	fstream in;
	in.open("source", ios::out | ios::binary | ios::app);
	char buffer[PAGE_SIZE];
	size_t head = 0;
	size_t tail = PAGE_SIZE - (21 * sizeof(size_t)) - 1;
	for (int i = 0; i < PAGE_VECTOR_NUM; i++) {
		size_t tempi = id[i];
		memcpy(buffer + head, reinterpret_cast<char*>(&tempi), sizeof(size_t));
		head += sizeof(size_t);
		for (int j = 0; j < FILE_DIMENSION; j++) {
			float tempf = vectorSet[i][j];
			memcpy(buffer + head, reinterpret_cast<char*>(&tempf), sizeof(float));
			head += sizeof(float);
		}
		tempi = bitMap[i];
		memcpy(buffer + tail, reinterpret_cast<char*>(&tempi), sizeof(size_t));
		tail += sizeof(size_t);
	}
	memcpy(buffer + tail, reinterpret_cast<char*>(&size), sizeof(size_t));
	in.write(buffer, PAGE_SIZE);
	in.close();
}

int main() {
    fstream in;
    in.open("source", ios::out | ios::binary);
    in.seekp(0, ios::beg);
    char fileHead[sizeof(size_t) * 2];
    in.write(fileHead, sizeof(size_t) * 2);
    in.close();
    size_t currentPage = 0;
    size_t currentVector = 0;
    size_t dimension = FILE_DIMENSION;
    float tempVectorSet[PAGE_VECTOR_NUM][FILE_DIMENSION] = { { 0 } };
    size_t tempVectorNum = 0;
    int tempFloatNum = -1;
    size_t tempBitMap[PAGE_VECTOR_NUM] = { 0 };
    size_t tempId[PAGE_VECTOR_NUM] = { 0 };
    size_t tempSize = 0;

    char tempc;
    float tempf;
    size_t tempi;
    fstream out;
    string tempStr;
    out.open(DATA_SET, ios::in | ios::binary);
    while (getline(out, tempStr, ' ')) {
//    	cout << tempFloatNum << ' ' << tempStr << ' ';
    	if (tempFloatNum == -1) {
    		tempFloatNum++;
		} else if (tempFloatNum != FILE_DIMENSION - 1) {
            tempVectorSet[tempVectorNum][tempFloatNum] = atof(tempStr.c_str());
            tempFloatNum++;
            //			cout << tempStr << ' ';
            //cout << tempf << " floatNum " << tempFloatNum;
        } else {
            //cout << "write page" << ' ' << tempc << " " << tempf << ' ';
            //			cout << "float number : " << tempFloatNum << endl;
            tempVectorSet[tempVectorNum][tempFloatNum] = atof(tempStr.c_str());
            tempFloatNum = -1;
            tempId[tempVectorNum] = currentVector;
            tempBitMap[tempVectorNum] = 1;
            tempVectorNum++;
            currentVector++;
        }
    	
        if (tempVectorNum == PAGE_VECTOR_NUM) {
            currentPage++;
            //cout << " \n current page" << currentPage << endl;
            savePage("", currentPage, tempVectorNum, tempVectorSet, tempBitMap, tempId);
            tempVectorNum = 0;
        }
        if (out.eof()) {
        	break;
		}
    }
    if (tempVectorNum != 0) {
        savePage("", currentPage, tempVectorNum, tempVectorSet, tempBitMap, tempId);
    }
    out.close();
    in.open("source", ios::out | ios::binary | ios::in);
    in.seekp(0, ios::beg);
    memcpy(fileHead, reinterpret_cast<char*>(&currentVector), sizeof(size_t));
    memcpy(fileHead + sizeof(size_t), reinterpret_cast<char*>(&dimension), sizeof(size_t));
    in.write(fileHead, sizeof(size_t) * 2);
    in.close();
}
#endif // GENERATE