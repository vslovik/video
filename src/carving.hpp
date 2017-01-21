#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc/imgproc.hpp>
#include "graph.h"

using namespace cv;
using namespace std;

Mat reduce_frame(Mat *bunch, int v, int h);
Mat reduce_vertical(Mat *reduced, Mat img);
Mat reduce_horizontal(Mat *reduced, Mat img);
int *find_seam(Mat *reduced);

