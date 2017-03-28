#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

/* ----- utility function ------- */
template<typename T>
T *Mat2uchar(cv::Mat &in) {
    T *out = new T[in.rows * in.cols];
    for (int i = 0; i < in.rows; ++i)
        for (int j = 0; j < in.cols; ++j)
            out[i * (in.cols) + j] = in.at<T>(i, j);
    return out;
}

int *find_seam(Mat &image){

    int max_int = numeric_limits<int>::max();

    int H = image.rows, W = image.cols;

    int dp[H][W];
    for(int c = 0; c < W; c++){
        dp[0][c] = (int)image.at<uchar>(0,c);
    }

    for(int r = 1; r < H; r++){
        for(int c = 0; c < W; c++){
            if (c == 0)
                dp[r][c] = min(dp[r-1][c+1], dp[r-1][c]);
            else if (c == W-1)
                dp[r][c] = min(dp[r-1][c-1], dp[r-1][c]);
            else
                dp[r][c] = min({dp[r-1][c-1], dp[r-1][c], dp[r-1][c+1]});
            dp[r][c] += (int)image.at<uchar>(r,c);
        }
    }

    int min_value = max_int;
    int min_index = -1;
    for(int c = 0; c < W; c++)
        if (dp[H-1][c] < min_value) {
            min_value = dp[H - 1][c];
            min_index = c;
        }

    int *path = new int[H];
    Point pos(H-1,min_index);
    path[pos.x] = pos.y;

    while (pos.x != 0){
        int value = dp[pos.x][pos.y] - (int)image.at<uchar>(pos.x,pos.y);
        int r = pos.x, c = pos.y;
        if (c == 0){
            if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        else if (c == W-1){
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else
                pos = Point(r-1,c);
        }
        else {
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        path[pos.x] = pos.y;
    }

    return path;
}

void remove_pixels(Mat& image, Mat& output, int *seam){
    for(int r = 0; r < image.rows; r++ ) {
        for (int c = 0; c < image.cols; c++){
            if (c >= seam[r])
                output.at<Vec3b>(r, c) = image.at<Vec3b>(r, c+1);
            else
                output.at<Vec3b>(r, c) = image.at<Vec3b>(r, c);
        }
    }
}

void energy_function(Mat &image, Mat &output){
    Mat dx, dy;
    Sobel(image, dx, CV_64F, 1, 0);
    Sobel(image, dy, CV_64F, 0, 1);
    magnitude(dx,dy, output);

    double min_value, max_value, Z;
    minMaxLoc(output, &min_value, &max_value);
    Z = 1/max_value * 255;
    output = output * Z;
    output.convertTo(output, CV_8U);
}

void coherence_function(Mat &image, int* seam) {
    int rows = image.rows;
    int cols = image.cols;

    uchar * dst = new uchar[rows * cols];
    uchar * src = Mat2uchar<uchar>(image);

    int Il[cols], Ir[cols];

    for (int r = 0; r < rows; r++) {
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
    }

    image = Mat(rows, cols, CV_8U, dst, Mat::AUTO_STEP);
}