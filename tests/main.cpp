#include <iostream>
#include <opencv2/highgui/highgui.hpp>

static void help()
{
    std::cout
            << "------------------------------------------------------------------------------" << std::endl
            << "This program just tests openCV installation."                                   << std::endl
            << "Usage:"                                                                         << std::endl
            << "./video inputvideoName"                                                         << std::endl
            << "------------------------------------------------------------------------------" << std::endl
            << std::endl;
}

void process_video(std::string source)
{
    cv::VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        throw source;
    }

    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    cv::Mat inFrame;
    int fps = inputVideo.get(CV_CAP_PROP_FPS);

    cv::Size size = cv::Size(
            (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
            (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
    );

    std::cout << "Input frame resolution: Width=" << size.width << "  Height=" << size.height
              << " of nr#: " << numFrames << std::endl;
    std::cout << "Input codec type: " << ".avi" << std::endl;

    cv::VideoWriter outputVideo;
    outputVideo.open("out.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, size, true);

    if (!outputVideo.isOpened()) {
        throw source;
    }

    for(int i = 0; i < numFrames; ++i) {
        inputVideo >> inFrame;
        outputVideo << inFrame;
    }

    inputVideo.release();
    outputVideo.release();
}

int main(int argc, char **argv)
{
    help();

    if(argc < 2) {
        std::cout << "Not enough parameters" << std::endl;
        return -1;
    }

    const std::string source = argv[1];

    try {
        process_video(source);
    } catch(std::string e){
        std::cout << e << std::endl;
        return -1;
    }

    return 0;
}

