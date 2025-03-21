#include <vector>
#include <iostream>

namespace MatMath {
    // Matrix structure inside namespace
    struct Mat {
        int rows;
        int cols;
        std::vector<int> matrix;
        Mat(int rows, int cols) : rows(rows), cols(cols) {
            matrix.resize(rows * cols, 0);
        }
    };

    // Standard matrix multiplication for base case
    Mat MultiplyMat(const Mat& mat1, const Mat& mat2) {
        Mat result(mat1.rows, mat2.cols);
        for (int i = 0; i < mat1.rows; i++) {
            for (int j = 0; j < mat2.cols; j++) {
                int sum = 0;
                for (int k = 0; k < mat1.cols; k++) {
                    sum += mat1.matrix[i * mat1.cols + k] * mat2.matrix[k * mat2.cols + j];
                }
                result.matrix[i * result.cols + j] = sum;
            }
        }
        return result;
    }

    // Copy a submatrix
    Mat copy(const Mat& mat, int start_rows, int end_rows, int start_cols, int end_cols) {
        int rows = end_rows - start_rows;
        int cols = end_cols - start_cols;
        Mat result(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result.matrix[i * cols + j] = mat.matrix[(start_rows + i) * mat.cols + (start_cols + j)];
            }
        }
        return result;
    }

    // Add two matrices 
    Mat add(const Mat& mat1, const Mat& mat2) { //matrices need to be added after multiplication
        Mat result(mat1.rows, mat1.cols); 
        for (int i = 0; i < mat1.rows * mat1.cols; i++) {
            result.matrix[i] = mat1.matrix[i] + mat2.matrix[i];
        }
        return result;
    }

    // Combine four submatrices into one
    Mat combine(const Mat& topLeft, const Mat& topRight, const Mat& bottomLeft, const Mat& bottomRight) {
        int rows = topLeft.rows + bottomLeft.rows;
        int cols = topLeft.cols + topRight.cols;
        Mat result(rows, cols);

        // Top-left quadrant
        for (int i = 0; i < topLeft.rows; i++) {
            for (int j = 0; j < topLeft.cols; j++) {
                result.matrix[i * result.cols + j] = topLeft.matrix[i * topLeft.cols + j];
            }
        }

        // Top-right quadrant
        for (int i = 0; i < topRight.rows; i++) {
            for (int j = 0; j < topRight.cols; j++) {
                result.matrix[i * result.cols + topLeft.cols + j] = topRight.matrix[i * topRight.cols + j];
            }
        }

        // Bottom-left quadrant
        for (int i = 0; i < bottomLeft.rows; i++) {
            for (int j = 0; j < bottomLeft.cols; j++) {
                result.matrix[(topLeft.rows + i) * result.cols + j] = bottomLeft.matrix[i * bottomLeft.cols + j];
            }
        }

        // Bottom-right quadrant
        for (int i = 0; i < bottomRight.rows; i++) {
            for (int j = 0; j < bottomRight.cols; j++) {
                result.matrix[(topLeft.rows + i) * result.cols + topLeft.cols + j] = 
                    bottomRight.matrix[i * bottomRight.cols + j];
            }
        }

        return result;
    }

    // Main divide-and-conquer multiplication
    Mat matMul(const Mat& mat1, const Mat& mat2) {
        if (mat1.cols != mat2.rows) {
            throw std::runtime_error("Matrix dimensions incompatible");
        }

        // Base case
        if (mat1.rows <= 2 || mat1.cols <= 2 || mat2.cols <= 2) {
            return MultiplyMat(mat1, mat2); //division is always done by 4
        }

        int mid1 = mat1.rows / 2;
        int mid2 = mat1.cols / 2;  // Same as mat2.rows / 2
        int mid3 = mat2.cols / 2;

        // Split mat1
        Mat A11 = copy(mat1, 0, mid1, 0, mid2);
        Mat A12 = copy(mat1, 0, mid1, mid2, mat1.cols);
        Mat A21 = copy(mat1, mid1, mat1.rows, 0, mid2);
        Mat A22 = copy(mat1, mid1, mat1.rows, mid2, mat1.cols);

        // Split mat2
        Mat B11 = copy(mat2, 0, mid2, 0, mid3);
        Mat B12 = copy(mat2, 0, mid2, mid3, mat2.cols);
        Mat B21 = copy(mat2, mid2, mat2.rows, 0, mid3);
        Mat B22 = copy(mat2, mid2, mat2.rows, mid3, mat2.cols);

        // Recursive multiplication
        Mat C11 = add(matMul(A11, B11), matMul(A12, B21));  // Top-left
        Mat C12 = add(matMul(A11, B12), matMul(A12, B22));  // Top-right
        Mat C21 = add(matMul(A21, B11), matMul(A22, B21));  // Bottom-left
        Mat C22 = add(matMul(A21, B12), matMul(A22, B22));  // Bottom-right

        // Combine results
        return combine(C11, C12, C21, C22);
    }
} 