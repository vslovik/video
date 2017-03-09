/*
* File: carving.h
* --------------
* This interface provides several functions
* for image seam sequential carving.
*
*/
#ifndef _carving_h
#define _carving_h

#include <opencv2/core/core.hpp>

using namespace cv;

/*
* Function: find_seam
* Usage: find_seam();
* -------------------
* Finds minimal seam to remove
* with minimal image distortion
*/
int *find_seam(Mat &image);

#endif
