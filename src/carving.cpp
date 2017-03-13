#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

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

void energy_function_(Mat &image, Mat &output){
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