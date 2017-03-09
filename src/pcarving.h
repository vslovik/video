/*
* File: pcarving.h
* --------------
* This interface provides several functions
* for image seam carving in parallel
*/
#ifndef _pcarving_h
#define _pcarving_h

#include <opencv2/core/core.hpp>

using namespace cv;

/*
* Function: find_seam
* Usage: find_seam();
* -------------------
* Finds minimal seam to remove
* with minimal image distortion
* as parallel computing task
*/
int *find_seam(Mat &image);

#endif