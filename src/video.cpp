#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace cv;
using namespace std;

static void help()
{
    cout
            << "------------------------------------------------------------------------------" << endl
            << "This program reads filters and write video files."                              << endl
            << "Usage:"                                                                         << endl
            << "./video inputvideoName"                                                         << endl
            << "------------------------------------------------------------------------------" << endl
            << endl;
}

int main(int argc, char **argv)
{
    help();

    if(argc < 2) {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    const string source = argv[1];

    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        cout  << "Could not open the input video: " << source << endl;
        return -1;
    }

    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    VideoWriter outputVideo;
    outputVideo.open("out.avi", CV_FOURCC('D', 'I', 'V', 'X'), inputVideo.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << source << endl;
        return -1;
    }

    cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
    cout << "Input codec type: " << ".avi" << endl;

    int channel = 2;
    Mat src, res;
    vector<Mat> frames;

    for(;;)
    {
        inputVideo >> src;
        if (src.empty()) break;

        frames.push_back(src);

        imshow("mainWin", src);
        waitKey(20);

        //outputVideo.write(res); //save or
        outputVideo << src;
    }

    cout << "Finished writing" << endl;

    inputVideo.release();
    outputVideo.release();
    return 0;
}