#include <iostream>
#include <vector>
#include <algorithm>
#include "kaizen.h"
#include <tuple>
#include "Rec_MatMul.h"
using namespace MatMath;


std::vector<int> multiply(const std::vector<int>& a, const std::vector<int>& b, int n,int m,int p) {
    std::vector<int> result(n * n, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < p; k++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
    return result;
}

std::tuple<int, int,int, int> process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);

    auto row_options = args.get_options("--rows");
    auto col_options = args.get_options("--cols");

    if (row_options.empty() || col_options.empty()) {
        zen::log("Error: --cols and/or --rows arguments are absent, using default 150!");
        return {150, 150,150, 150};
    }

    try {
        int row1 = (row_options.size() > 0) ? std::stoi(row_options[0]) : 150;
        int row2 = (row_options.size() > 1) ? std::stoi(row_options[1]) : 150;

        int col1 = (col_options.size() > 0) ? std::stoi(col_options[0]) : 150;
        int col2 = (col_options.size() > 1) ? std::stoi(col_options[1]) : 150;

        if(row1 != col2) {
            zen::log("Error: The number of columns in the first matrix must be equal to the number of rows in the second matrix using default!");
            return {150, 150, 150, 150};
        }
        return {row1, col1, row2, col2};
    } catch (const std::exception& e) {
        zen::log("Invalid input detected. Using default values (150,150) for both rows and columns!");
        return {150, 150, 150, 150};
    }
}



int main(int argc, char* argv[]) {
    auto [row1, col1, row2, col2] = process_args(argc, argv);   
   
    Mat matrix1(row1, col1);
    Mat matrix2(row2, col2);
    
    // Initialize matrices
    for(int i = 0; i < row1 * col1; i++) {
        matrix1.matrix[i] = zen::random_int(1, row1 * col1);
    }
    for(int i = 0; i < row2 * col2; i++) {
        matrix2.matrix[i] = zen::random_int(1, row2 * col2);
    }   
    
  
    zen::timer timer;
    timer.start();
    MatMath::matMul(matrix1, matrix2);
    timer.stop();
    auto first = timer.duration<zen::timer::usec>().count();
    zen::print("Time taken: ",first,"us\n");
    
    timer.start();
    multiply(matrix1.matrix, matrix2.matrix, row1,col2,col1);   
    timer.stop();
    auto second = timer.duration<zen::timer::usec>().count();

    zen::print("Time taken: ", second,"us\n");

    timer.start();
    MatMath::BlockedMul(matrix1,matrix2);   
    timer.stop();
    auto third = timer.duration<zen::timer::usec>().count();
    zen::print("Time taken: ", third,"us\n");
    

    zen::print("Recursive approach is faster than ierative by : ", second - first,"us\n");
    zen::print("Blocked approach is faster than ierative by : ", third - first,"us\n");
    zen::print("Recursive approach is faster than Blocked by : ", second - third,"us\n");

    
    return 0;
}