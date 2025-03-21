#include <vector>
#include <iostream>
#include "kaizen.h" // Assuming this provides timer, etc.

namespace MatMath {
    // Matrix structure with raw data access
    struct Mat {
        int rows, cols;
        std::vector<int> matrix;
        Mat(int r, int c) : rows(r), cols(c), matrix(r * c, 0) {}
        
        // Access element at (i, j) without copying
        int& at(int i, int j) { return matrix[i * cols + j]; }
        const int& at(int i, int j) const { return matrix[i * cols + j]; }
    };

    // Standard multiplication with index bounds
    void MultiplyMat(Mat& result, const Mat& mat1, const Mat& mat2,
                     int r1_start, int r1_end, int c1_start, int c1_end,
                     int r2_start, int r2_end, int c2_start, int c2_end,
                     int r_res_start, int c_res_start) {
        for (int i = r1_start; i < r1_end; i++) {
            for (int j = c2_start; j < c2_end; j++) {
                int sum = 0;
                for (int k = c1_start; k < c1_end; k++) {
                    sum += mat1.at(i, k) * mat2.at(k, j);
                }
                result.at(r_res_start + (i - r1_start), c_res_start + (j - c2_start)) = sum;
            }
        }
    }

    // Add matrices in-place
    void add(Mat& result, const Mat& mat1, const Mat& mat2,
             int r_start, int r_end, int c_start, int c_end) {
        for (int i = r_start; i < r_end; i++) {
            for (int j = c_start; j < c_end; j++) {
                result.at(i, j) += mat1.at(i, j) + mat2.at(i, j);
            }
        }
    }

    // Recursive multiplication without copying
    void matMul(Mat& result, const Mat& mat1, const Mat& mat2,
                int r1_start, int r1_end, int c1_start, int c1_end,
                int r2_start, int r2_end, int c2_start, int c2_end,
                int r_res_start, int c_res_start) {
        int r1_size = r1_end - r1_start;
        int c1_size = c1_end - c1_start;
        int c2_size = c2_end - c2_start;

        // Base case
        if (r1_size <= 16 || c1_size <= 16 || c2_size <= 16) {
            MultiplyMat(result, mat1, mat2,
                        r1_start, r1_end, c1_start, c1_end,
                        r2_start, r2_end, c2_start, c2_end,
                        r_res_start, c_res_start);
            return;
        }

        int mid1 = r1_start + r1_size / 2;
        int mid2 = c1_start + c1_size / 2; // Also r2_size / 2
        int mid3 = c2_start + c2_size / 2;

        // Recursive calls without copying
        Mat temp1(r1_size / 2, c2_size / 2); // Temporary for intermediate results
        Mat temp2(r1_size / 2, c2_size / 2);

        // Top-left: C11 = A11*B11 + A12*B21
        matMul(temp1, mat1, mat2, r1_start, mid1, c1_start, mid2, r2_start, mid2, c2_start, mid3, 0, 0);
        matMul(temp2, mat1, mat2, r1_start, mid1, mid2, c1_end, mid2, r2_end, c2_start, mid3, 0, 0);
        add(result, temp1, temp2, r_res_start, r_res_start + r1_size / 2, c_res_start, c_res_start + c2_size / 2);

        // Top-right: C12 = A11*B12 + A12*B22
        matMul(temp1, mat1, mat2, r1_start, mid1, c1_start, mid2, r2_start, mid2, mid3, c2_end, 0, 0);
        matMul(temp2, mat1, mat2, r1_start, mid1, mid2, c1_end, mid2, r2_end, mid3, c2_end, 0, 0);
        add(result, temp1, temp2, r_res_start, r_res_start + r1_size / 2, c_res_start + c2_size / 2, c_res_start + c2_size);

        // Bottom-left: C21 = A21*B11 + A22*B21
        matMul(temp1, mat1, mat2, mid1, r1_end, c1_start, mid2, r2_start, mid2, c2_start, mid3, 0, 0);
        matMul(temp2, mat1, mat2, mid1, r1_end, mid2, c1_end, mid2, r2_end, c2_start, mid3, 0, 0);
        add(result, temp1, temp2, r_res_start + r1_size / 2, r_res_start + r1_size, c_res_start, c_res_start + c2_size / 2);

        // Bottom-right: C22 = A21*B12 + A22*B22
        matMul(temp1, mat1, mat2, mid1, r1_end, c1_start, mid2, r2_start, mid2, mid3, c2_end, 0, 0);
        matMul(temp2, mat1, mat2, mid1, r1_end, mid2, c1_end, mid2, r2_end, mid3, c2_end, 0, 0);
        add(result, temp1, temp2, r_res_start + r1_size / 2, r_res_start + r1_size, c_res_start + c2_size / 2, c_res_start + c2_size);
    }

    // Wrapper for ease of use
    Mat matMul(const Mat& mat1, const Mat& mat2) {
        if (mat1.cols != mat2.rows) throw std::runtime_error("Matrix dimensions incompatible");
        Mat result(mat1.rows, mat2.cols);
        matMul(result, mat1, mat2, 0, mat1.rows, 0, mat1.cols, 0, mat2.rows, 0, mat2.cols, 0, 0);
        return result;
    }
}
