//following head files must be added
#include <iostream>
#include <stdlib.h>  
#include <time.h> 
#include <memory.h>
#include <fstream>

#include "Matrix.h"

//this struct must be added too
struct MyStruct{
    double value;
    int subscripts;
};

DenseMatrix& randomMatrix(int k){
    //assum that M is the target matrix
    size_t ROW=M.getRow();
    size_t COL=M.getColumn();

    
    srand((unsigned)time(NULL));
    int* ran = new int[k];
    int* r = new int[COL];    

    //find the random k columns
    for(int i=0;i<COL;i++){
        r[i]=i;
    }
    for (int i = 0; i < k; i++) {
        j = rand()%ROW;
        temp = r[i];
        r[i] = r[j];
        r[j] = temp;
    }
    for (int i = 0; i < k; i++) {
        ran[i] = r[i];
    }

    //save the corresponding columns
    DenseMatrix  matrixAfter(ROW,k);
    int currentCol = 0;
    for(int i=0;i<k;i++){
        currentCol = ran[i];
        for (int x = 0; x < ROW;x++) {
             matrixAfter[x][i]= M[x][currentCol];
        }
    }

    delete[] ran;
    delete[] r;
    return matrixAfter;
}



DenseMatrix& WTAMatrix(int k){
    //assum that M is the target matrix
    size_t ROW=M.getRow();
    size_t COL=M.getColumn();

    bool* isDelete = new bool[COL];//record if the certain column shou be delete
    
    DenseMatrix  matrixAfter(ROW,COL);
    //find the top k in every row
    for (int i = 0; i < ROW; i++) { 
        MyStruct rowStruct[COL];
        MyStruct temp;  
        memset(isDelete, true, sizeof(bool)*COL);//initialize that every col will be delete   
        for (int j = 0; j < COL; j++) { 
            rowStruct[j].value = M[i][j];
            rowStruct[j].subscripts = j;
        }

        for (int x = 0; x < COL; x++) {
            for (int y = x; y < COL; y++) {
                if (rowStruct[x].value < rowStruct[y].value) {//sort from big to small
                    temp = rowStruct[x];
                    rowStruct[x] = rowStruct[y];
                    rowStruct[y] = temp;
                }
            }
        }
        for (int x = 0; x <k; x++) {//make the top k num not delete
            isDelete[rowStruct[x].subscripts] = false;
        }
        for (int x = 0; x < COL; x++) {
            if (isDelete[x]) {
                matrixAfter[i][x] = 0;
            }
            else{
                matrixAfter[i][x]=M[i][x];
            }
        }
    }
    
    delete[] isDelete;
    return matrixAfter;
}
DenseMatrix& binaryMatrix(int k){
    //assum that M is the target matrix
    size_t ROW=M.getRow();
    size_t COL=M.getColumn();
    
    bool* isDelete = new bool[COL];//record if the certain column shou be delete   
    DenseMatrix  matrixAfter(ROW,COL);
    //find the top k in every row
    for (int i = 0; i < ROW; i++) { 
        MyStruct rowStruct[COL];
        MyStruct temp;  
        memset(isDelete, true, sizeof(bool)*COL);//initialize that every col will be delete   
        for (int j = 0; j < COL; j++) { 
            rowStruct[j].value = M[i][j];
            rowStruct[j].subscripts = j;
        }

        for (int x = 0; x < COL; x++) {
            for (int y = x; y < COL; y++) {
                if (rowStruct[x].value < rowStruct[y].value) {//sort from big to small
                    temp = rowStruct[x];
                    rowStruct[x] = rowStruct[y];
                    rowStruct[y] = temp;
                }
            }
        }
        for (int x = 0; x <k; x++) {//make the top k num not delete
            isDelete[rowStruct[x].subscripts] = false;
        }
        for (int x = 0; x < COL; x++) {
            if (isDelete[x]) {
                matrixAfter[i][x] = 0;
            }
            else{
                matrixAfter[i][x]=1;
            }
        }
    }
    
    delete[] isDelete; 
    return matrixAfter;
}