#include <vector>
#include <iostream>
#include <format> // C++20
#include "kaizen.h" // Assuming this provides timer, print, etc.
#include "Rec_MatMul.h" // For matMul
// Assuming BlockedMul and multiply are defined elsewhere

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

std::tuple<int, int, int, int> process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    auto row_options = args.get_options("--rows");
    auto col_options = args.get_options("--cols");

    if (row_options.empty() || col_options.empty()) {
        zen::log("Error: --cols and/or --rows arguments are absent, using default 150!");
        return {150, 150, 150, 150};
    }

    try {
        int row1 = (row_options.size() > 0) ? std::stoi(row_options[0]) : 150;
        int row2 = (row_options.size() > 1) ? std::stoi(row_options[1]) : 150;
        int col1 = (col_options.size() > 0) ? std::stoi(col_options[0]) : 150;
        int col2 = (col_options.size() > 1) ? std::stoi(col_options[1]) : 150;

        if (row1 != col2) {
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
    for (int i = 0; i < row1 * col1; i++) {
        matrix1.matrix[i] = zen::random_int(1, row1 * col1);
    }
    for (int i = 0; i < row2 * col2; i++) {
        matrix2.matrix[i] = zen::random_int(1, row2 * col2);
    }

    zen::timer timer;
    long long recursive_time, naive_time, blocked_time;

    // Recursive matMul
    timer.start();
    MatMath::matMul(matrix1, matrix2);
    timer.stop();
    recursive_time = timer.duration<zen::timer::usec>().count();

    // Naive multiply
    timer.start();
    multiply(matrix1.matrix, matrix2.matrix, row1, col2, col1);
    timer.stop();
    naive_time = timer.duration<zen::timer::usec>().count();

    // BlockedMul
    timer.start();
    MatMath::BlockedMul(matrix1, matrix2);
    timer.stop();
    blocked_time = timer.duration<zen::timer::usec>().count();

    // Table header for timings
    zen::print(std::format("\nMatrix Multiplication Performance ({}x{} * {}x{})\n", row1, col1, row2, col2));
    zen::print("+----------------------+------------+\n");
    zen::print("| Method               | Time (us)  |\n");
    zen::print("+----------------------+------------+\n");

    // Table rows for timings
    zen::print(std::format("| {:<20} | {:>10} |\n", "Recursive (matMul)", recursive_time));
    zen::print(std::format("| {:<20} | {:>10} |\n", "Naive (multiply)", naive_time));
    zen::print(std::format("| {:<20} | {:>10} |\n", "Blocked (BlockedMul)", blocked_time));
    zen::print("+----------------------+------------+\n");

    // Comparisons table with time factors
    zen::print("\nPerformance Factors (Ratio)\n");
    zen::print("+--------------------------------+------------+\n");
    zen::print("| Comparison                     | Factor     |\n");
    zen::print("+--------------------------------+------------+\n");

    // Compute factors as floating-point ratios
    double rec_vs_naive = static_cast<double>(recursive_time) / naive_time;
    double blocked_vs_naive = static_cast<double>(blocked_time) / naive_time;
    double rec_vs_blocked = static_cast<double>(recursive_time) / blocked_time;

    // Table rows for factors
    zen::print(std::format("| {:<30} | {:>10.2f} |\n", "Recursive vs. Naive", rec_vs_naive));
    zen::print(std::format("| {:<30} | {:>10.2f} |\n", "Blocked vs. Naive", blocked_vs_naive));
    zen::print(std::format("| {:<30} | {:>10.2f} |\n", "Recursive vs. Blocked", rec_vs_blocked));
    zen::print("+--------------------------------+------------+\n");

    return 0;
}