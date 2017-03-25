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


    int i, j; int sum;
    for (int r = 0; r < rows; r++) {
        cout << seam[r] << endl;
        for (int c = 0; c < cols; c++) {
            sum = 0;


            if(c != seam[r]) {
                int start = min(c, seam[r]);
                int end = max(c, seam[r]);
                i = start;
                j = start;
                for (int k = start; k <= end; k++) {


                    if (i == c) i++;
                    if (j == seam[r]) j++;
                    if (i != j)
                        sum += abs(src[r * cols + i] - src[r * cols + j]);
                    i++;
                    j++;
                }
            }

            sum += src[r*cols+c];
            if (sum > 255) sum = 255;
            else if (sum < 0) sum = 0;

            dst[r*cols+c] = (uchar) sum;

        }
    }

    image = Mat(rows, cols, CV_8U, dst, Mat::AUTO_STEP);
}