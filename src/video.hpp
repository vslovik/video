#include <iostream>
#include <string>   // for strings
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <opencv2/imgproc/imgproc.hpp>

#include "carving.hpp"

struct State {
    static const int ver = 100;
    static const int hor = 50;
    Size size;
    string output;
    double fps;
    int frameIter, numFrames;
    Mat *inFrames, *outFrames;

    State(
            double fps,
            int numFrames,
            Size size,
            int frameIter = 0,
            Mat *inFrames = new Mat[0],
            Mat *outFrames = new Mat[0],
            string output = "out.avi"
    ) :
            fps(fps),
            numFrames(numFrames),
            size(size),
            frameIter(frameIter),
            inFrames(inFrames),
            outFrames(outFrames),
            output(output)
    {}
};

void read_video(string source);
void write_video(string source);
void *process_frame(void *);
void process_video();
