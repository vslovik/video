#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

using namespace cv;
using namespace std;

Mat reduce_frame(vector<Mat> bunch, int v, int h);
Mat reduce_vertical(vector<Mat> &reduced, Mat img);
Mat reduce_horizontal(vector<Mat> &reduced, Mat img);

