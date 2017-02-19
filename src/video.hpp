#include <iostream>
#include <string>   // for strings
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <opencv2/imgproc/imgproc.hpp>

#include "carving.hpp"

struct State {
    static const int ver = 10;
    static const int hor = 0;
    Size size;
    string output;
    double fps;
    int numFrames;
    Mat inFrame, outFrame;

    State(
            double fps,
            int numFrames,
            Size size,
            Mat inFrame,
            Mat outFrame,
            string output = "out.avi"
    ) :
            fps(fps),
            numFrames(numFrames),
            size(size),
            inFrame(inFrame),
            outFrame(outFrame),
            output(output)
    {}
};

void process_video(string source);
