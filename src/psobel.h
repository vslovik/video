/*
* File: psobel.h
* --------------
*/
#ifndef _sobel_h
#define _sobel_h

#include <opencv2/core/core.hpp>

using namespace cv;

/*
* Function: sobel
* Usage: sobel();
* -------------------
*/
void sobel(Mat &image, Mat &output, int num_workers);

#endif