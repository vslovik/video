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

/*
* Function: remove_pixels
* Usage: remove_pixels();
* -------------------
* Removes found seam
*/
void remove_pixels(Mat& image, Mat& output, int *seam);

/*
* Function: energy_function
* Usage: energy_function();
* -------------------
* Calculates energy map
*/
void energy_function(Mat &image, Mat &output);

/*
* Function: coherence_function
* Usage: coherence_function();
* -------------------
* Calculates coherence-aware energy map
*/
void coherence_function(Mat &image, int* seam);

#endif