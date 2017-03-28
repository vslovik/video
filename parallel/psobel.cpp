#include <iostream>
#include <opencv2/core/core.hpp>
#include <ff/parallel_for.hpp>

using namespace cv;
using namespace std;
using namespace ff;

/* ----- utility function ------- */
template<typename T>
T *Mat2uchar(cv::Mat &in) {
    T *out = new T[in.rows * in.cols];
    for (int i = 0; i < in.rows; ++i)
        for (int j = 0; j < in.cols; ++j)
            out[i * (in.cols) + j] = in.at<T>(i, j);
    return out;
}

#define XY2I(Y,X,COLS) (((Y) * (COLS)) + (X))
/* ------------------------------ */

// returns the gradient in the x direction
static inline long xGradient(uchar * image, long cols, long x, long y) {
    return image[XY2I(y-1, x-1, cols)] +
           2*image[XY2I(y, x-1, cols)] +
           image[XY2I(y+1, x-1, cols)] -
           image[XY2I(y-1, x+1, cols)] -
           2*image[XY2I(y, x+1, cols)] -
           image[XY2I(y+1, x+1, cols)];
}

// returns the gradient in the y direction
static inline long yGradient(uchar * image, long cols, long x, long y) {
    return image[XY2I(y-1, x-1, cols)] +
           2*image[XY2I(y-1, x, cols)] +
           image[XY2I(y-1, x+1, cols)] -
           image[XY2I(y+1, x-1, cols)] -
           2*image[XY2I(y+1, x, cols)] -
           image[XY2I(y+1, x+1, cols)];
}

void sobel(Mat &image, Mat &output, int num_workers) {

    int rows = image.rows;
    int cols = image.cols;

    uchar * dst = new uchar[rows * cols];
    uchar * src = Mat2uchar<uchar>(image);

    ff::ParallelFor pf(num_workers, false);
    pf.parallel_for(1, rows - 1,[src, cols, &dst](const long y) {
        for(long x = 1; x < cols - 1; x++){
            const long gx = xGradient(src, cols, x, y);
            const long gy = yGradient(src, cols, x, y);
            // approximation of sqrt(gx*gx+gy*gy)
            long sum = abs(gx) + abs(gy);
            if (sum > 255) sum = 255;
            else if (sum < 0) sum = 0;

            dst[y*cols+x] = (uchar) sum;
        }
    });

    output = Mat(rows, cols, CV_8U, dst, Mat::AUTO_STEP);
}

void coherence(Mat &image, int* seam, int num_workers) {

    int rows = image.rows;
    int cols = image.cols;

    uchar * dst = new uchar[rows * cols];
    uchar * src = Mat2uchar<uchar>(image);

    int Il[cols], Ir[cols];

    ff::ParallelFor pf(num_workers, false);
    pf.parallel_for(0, rows, [src, cols, &dst, &seam, &Il, &Ir](const long r) {
        int sum;
        for (int c = 0; c < cols - 1; c++) {
            Il[c] = 0; Ir[c] = 0;
        }
        for (int c = seam[r]; c < cols; c++) {
            Il[c] = Il[c - 1] + abs(src[r * cols + c - 1] - src[r * cols + c]);
        }
        for (int c = min(cols - 2, seam[r]); c >= 0; c--) {
            Ir[c] = Ir[c + 1] + abs(src[r * cols + c + 1] - src[r * cols + c]);
        }
        for (int c = 1; c < cols; c++) {
            sum = Il[c] + Ir[c] + src[r*cols+c];
            if (sum > 255) sum = 255;
            else if (sum < 0) sum = 0;
            dst[r*cols+c] = (uchar) sum;
        }
    });

    image = Mat(rows, cols, CV_8U, dst, Mat::AUTO_STEP);
}