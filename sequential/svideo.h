/*
* File: video.h
* --------------
* This interface provides several
* functions for video retargeting
*
*/
#ifndef _video_h
#define _video_h

#include <iostream>

using namespace std;

/*
* Function: find_seam
* Usage: find_seam();
* -------------------
* Retargets a video sample
*/
void process_video(string source);

#endif