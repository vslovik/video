# video
Video processing with OpenCV
===

Other possible codec codes:

- CV_FOURCC('P','I','M','1')    = MPEG-1 codec
- CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
- CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
- CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
- CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
- CV_FOURCC('U', '2', '6', '3') = H263 codec
- CV_FOURCC('I', '2', '6', '3') = H263I codec
- CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec

A codec code of -1 will open a codec selection window (in windows).

Documentation
---
http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00071000000000000000

To compile
---
g++ -o video video.cpp `pkg-config opencv --cflags --libs`