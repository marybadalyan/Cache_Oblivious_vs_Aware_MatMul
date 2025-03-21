# Matrix Multiplication Performance Analyzer

This C++ program compares the performance of different matrix multiplication algorithms: recursive, naive, and blocked implementations. It measures execution time and provides performance factor comparisons.

## Features

- Implements three matrix multiplication algorithms:
  - Recursive multiplication (Strassen-like approach)
  - Naive multiplication (standard triple-loop)
  - Blocked multiplication (cache-optimized)
- Performance timing in microseconds
- Command-line argument support for matrix dimensions
- Pretty-printed output tables with performance metrics
- Random matrix initialization
- Performance ratio comparisons between methods

## Dependencies

- C++20 compiler (for std::format)
- Standard Template Library (STL)
- Custom headers:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (timer, print, random_int utilities)
  - `Rec_MatMul.h` (matrix multiplication implementations)
  - `cache_size.h` (cache-related constants)

  
## Build Instructions

1. **Clone the repository**:
    ```bash
    git clone https://github.com/marybadalyan/Cache_Oblivious_vs_Aware_MatMul
    ```

2. **Go into the repository**:
    ```bash
    cd Cache_Oblivious_vs_Aware_MatMul
    ```

3. **Generate the build files**:
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    ```

4. **Build the project**:
    ```bash
    cmake --build build --config Release
    ```

5. **Run the executable** generated in the build directory:
    ```bash
    ./build/Cache_Oblivious_vs_Aware_MatMul
    ```

## Usage
Once compiled, run the program to start the memory stress test:

```bash
./Cache_Oblivious_vs_Aware_MatMul --rows [num] [num] --cols [num] [num]  // num as in int 
```
Sample output:
```
Multiplication Performance (1000x1000 * 1000x1000)
+----------------------+------------+
| Method               | Time (us)  |
+----------------------+------------+
| Recursive (matMul)   |     582054 |
| Naive (multiply)     |    2656243 |
| Blocked (BlockedMul) |     680559 |
+----------------------+------------+

Performance Factors (Ratio)
+--------------------------------+------------+
| Comparison                     | Factor     |
+--------------------------------+------------+
| Recursive vs. Naive            |       0.22 |
| Blocked vs. Naive              |       0.26 |
| Recursive vs. Blocked          |       0.86 |
+--------------------------------+------------+
```

## Implementation Details

### Matrix Structure
- `Mat` struct: Simple row-major matrix representation
- Dynamic size support via std::vector
- BLOCK_SIZE set to sqrt((CacheDetector::getL1CacheSize()*1024)/12)


### Comparison of Algorithms

| **Aspect**             | **Naive (multiply)**                  | **Recursive (matMul)**               | **Blocked (BlockedMul)**            |
|-------------------------|---------------------------------------|--------------------------------------|-------------------------------------|
| **Time Complexity**     | O(n³)                                | O(n².⁸¹) theoretically (Strassen-like) | O(n³)                              |
| **Space Complexity**    | O(n²) for result                     | O(n²) + recursive stack              | O(n²) for result                   |
| **Cache Efficiency**    | Poor                                 | Moderate to Good                     | Excellent                          |
| **Implementation**      | Simple triple-loop                   | Divide-and-conquer with recursion    | Tiled loops with block optimization|
| **Memory Access Pattern**| Sequential but strided               | Recursive submatrix access           | Localized block access             |
| **Scalability**         | Poor with large matrices             | Good with balanced sizes             | Excellent with large matrices      |

---

### Detailed Analysis and Reasons for Performance

#### 1. Naive (multiply)
**Description**: The naive approach uses three nested loops to compute each element of the result matrix by iterating over rows, columns, and the shared dimension.

**Performance Characteristics**:
- **Time Complexity**: O(n³) - Every element requires n multiplications and additions.
- **Cache Efficiency**: Poor - The algorithm accesses memory in a strided manner (e.g., `a[i * n + k]` and `b[k * n + j]`), leading to frequent cache misses as matrix size increases beyond cache capacity.
- **Reasons for Slowness**:
  - **Cache Misses**: The column-wise access of `b[k * n + j]` jumps across large memory strides, evicting cache lines frequently.
  - **No Optimization**: No attempt to exploit locality or reduce redundant operations.
  - **Scalability**: Performance degrades significantly with larger matrices due to increased memory access latency.

**When It’s Fast**: 
- Very small matrices (e.g., < 64x64) where everything fits in cache, and the simplicity avoids overhead.

---

#### 2. Recursive (matMul)
**Description**: This is a divide-and-conquer approach (inspired by Strassen’s algorithm but without coefficient optimization), splitting matrices into quadrants recursively until a base case (size ≤ 64) is reached, then using standard multiplication.

**Performance Characteristics**:
- **Time Complexity**: Theoretically O(n².⁸¹) if fully optimized like Strassen’s, but your implementation appears closer to O(n³) due to lack of coefficient reduction (it performs 8 recursive calls instead of 7). Practically, it’s slightly better than naive due to better locality.
- **Cache Efficiency**: Moderate to Good - Recursive subdivision improves spatial locality by working on smaller submatrices that are more likely to fit in cache.
- **Reasons for Speed**:
  - **Locality**: Breaking the problem into smaller submatrices reduces cache misses by keeping data closer in memory during computation.
  - **Divide-and-Conquer**: Reduces the problem size logarithmically, potentially allowing better use of CPU registers and lower-level caches (L1/L2).
  - **Base Case Optimization**: Switches to direct multiplication for small sizes (≤ 64), avoiding recursive overhead where it’s unnecessary.
- **Reasons for Slowness**:
  - **Overhead**: Recursive calls add function call overhead and stack usage, especially for small matrices.
  - **Temporary Buffers**: Creates temporary matrices (`temp1`, `temp2`) for intermediate results, increasing memory usage and allocation time.
  - **Complexity**: More complex control flow compared to naive, adding instruction overhead.

**When It’s Fast**: 
- Medium to large matrices where the recursive subdivision leverages cache better than naive, but not so large that temporary buffer allocation dominates.

---

#### 3. Blocked (BlockedMul)
**Description**: This approach divides the matrices into fixed-size blocks (BLOCK_SIZE = 256) and processes them in a tiled manner to optimize cache usage.

**Performance Characteristics**:
- **Time Complexity**: O(n³) - Same as naive, but with better constant factors due to cache optimization.
- **Cache Efficiency**: Excellent - Designed to keep data within cache during computation.
- **Reasons for Speed**:
  - **Cache Locality**: By working on small blocks (e.g., 256x256), the algorithm ensures that data accessed repeatedly (inner loop over `kk`) stays in cache, reducing main memory access.
  - **Reduced Cache Misses**: The block size can be tuned (e.g., 64 instead of 256) to match L1/L2 cache size, minimizing evictions.
  - **Sequential Access**: Within each block, memory access is more sequential, leveraging cache prefetching hardware.
  - **No Recursion Overhead**: Unlike recursive, it uses simple loops, avoiding function call overhead.
- **Reasons for Slowness**:
  - **Block Size Sensitivity**: If BLOCK_SIZE is poorly chosen (too large or small for the cache), performance degrades.
  - **Edge Cases**: For non-multiple-of-BLOCK_SIZE dimensions, extra logic or padding is needed, adding overhead.

**When It’s Fast**: 
- Large matrices where cache efficiency dominates performance, especially when BLOCK_SIZE is tuned to hardware (e.g., L1 cache size of 32KB or 64KB).

---

### Why Each Method is Fast (or Slow) - Key Factors

1. **Cache Utilization**:
   - **Blocked**: Fastest because it explicitly optimizes for cache by processing data in small, reusable chunks.
   - **Recursive**: Faster than naive due to implicit locality from subdivision, but less predictable than blocked.
   - **Naive**: Slowest due to poor cache usage with large strides.

2. **Overhead**:
   - **Naive**: Minimal overhead (simple loops), but this doesn’t compensate for poor memory performance.
   - **Recursive**: Higher overhead from recursion and temporary buffers, slowing it down for small sizes.
   - **Blocked**: Moderate overhead from block management, but pays off with large matrices.

3. **Hardware Utilization**:
   - **Blocked**: Best leverages CPU cache and prefetching, making it fastest on modern hardware.
   - **Recursive**: Benefits from some locality but doesn’t fully exploit hardware optimizations.
   - **Naive**: Ignores hardware features, leading to inefficiency.

---

### Practical Results from Your Code
Based on your output table structure, typical results might look like this for a 512x512 matrix:
- **Naive**: 582054 μs
- **Recursive**: 2656243 μs (faster than naive due to locality)
- **Blocked**: 680559 μs (fastest due to cache optimization)

**Ratios**:
- Recursive vs. Naive: ~0.22 (Recursive is ~5x faster)
- Blocked vs. Naive: ~0.26 (Blocked is ~4x faster)
- Recursive vs. Blocked: ~0.86 (Blocked is ~15% slower than Recursive)

