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
* Function: rot90
* Usage: rot90();
* -------------------
* Image transposition
*/
void rot90(Mat &matImage, int flag);

/*
* Function: find_seam
* Usage: find_seam();
* -------------------
* Finds minimal seam to remove
* with minimal image distortion
* as parallel computing task
*/
int* find_seams(Mat &image, int &num_found, int num_workers = 1);

/*
* Function: remove_pixels
* Usage: remove_pixels();
* -------------------
* Removes found seam
*/
void remove_pixels(Mat& image, int *seam, int num_workers = 1);

/*
* Function: energy_function
* Usage: energy_function();
* -------------------
* Calculates energy map
*/
void energy_function(Mat &image, Mat &output, int num_workers = 1);

/*
* Function: coherence_function
* Usage: coherence_function();
* -------------------
* Calculates coherence-aware energy map
*/
void coherence_function(Mat &image, int* seam, int num_workers = 1);

/*
* Function: remove_seam
* Usage: remove_seam();
* -------------------
* Removes seam
*/
void remove_seam(Mat& image, char orientation = 'v', int num_workers = 1);

/*
* Function: realTime
* Usage: realTime();
* -------------------
* Real time seams removal
*/
void realTime(Mat& image, int num_workers);

#endif