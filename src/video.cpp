/*
* File: video.cpp
* ---------------
*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ff/parallel_for.hpp>
#include "pcarving.h"

using namespace cv;
using namespace std;

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

State *process;

void energy_function(Mat &image, Mat &output){
    Mat dx, dy;
    Sobel(image, dx, CV_64F, 1, 0);
    Sobel(image, dy, CV_64F, 0, 1);
    magnitude(dx,dy, output);

    double min_value, max_value, Z;
    minMaxLoc(output, &min_value, &max_value);
    Z = 1/max_value * 255;
    output = output * Z;
    output.convertTo(output, CV_8U);
}

void rot90(Mat &matImage, int rotflag){
    //1=CW, 2=CCW, 3=180
    if (rotflag == 1){
        transpose(matImage, matImage);
        flip(matImage, matImage,1); //transpose+flip(1)=CW
    } else if (rotflag == 2) {
        transpose(matImage, matImage);
        flip(matImage, matImage,0); //transpose+flip(0)=CCW
    } else if (rotflag ==3){
        flip(matImage, matImage,-1);    //flip(-1)=180
    } else if (rotflag != 0){ //if not 0,1,2,3:
        cout  << "Unknown rotation flag(" << rotflag << ")" << endl;
    }
}

void remove_seam(Mat& image, char orientation = 'v', int nw = 1){
    if (orientation == 'h')
        rot90(image,1);
    int H = image.rows, W = image.cols;

    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);

    Mat eimage;
    energy_function(gray, eimage);

    int* seam = find_seam(eimage, nw);

    Mat output(H,W-1, CV_8UC3);
    remove_pixels(image, output, seam, nw);
    if (orientation == 'h')
        rot90(output,2);
    image = output;
}

void realTime(Mat& image, int num_workers = 1){
    cout << "UP ARROW: Shrink horizontally" << endl;
    cout << "LEFT ARROW: Shrink vertically" << endl;
    cout << "q: Quit" << endl;

    int key;
    while(1) {
        namedWindow("Display window", WINDOW_AUTOSIZE);
        imshow("Display window", image);
        key = waitKey(0);
        if (key == 'q')
            break;
        else if (key == 'v')
            remove_seam(image, 'v', num_workers);
        else if (key == 'h')
            remove_seam(image, 'h', num_workers);
    }
}

void shrink_image(Mat& image, int new_cols, int new_rows, int width, int height, int num_workers = 1){
    cout << endl << "Processing image..." << endl;
    for(int i = 0; i < width - new_cols; i++){
        remove_seam(image, 'v', num_workers);
    }
    for(int i = 0; i < height - new_rows; i++){
        remove_seam(image, 'h', num_workers);
    }
}


void process_video(string source, int num_workers = 1)
{
//    VideoCapture inputVideo(source);
//    if (!inputVideo.isOpened())
//    {
//        throw source;
//    }
//
//    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
//    Mat inFrame, outFrame;
//    process = new State(
//            inputVideo.get(CV_CAP_PROP_FPS),
//            numFrames,
//            Size(
//             (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
//             (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
//            ),
//            inFrame,
//            outFrame,
//            "out.avi"
//    );
//
//    cout << "Input frame resolution: Width=" << process->size.width << "  Height=" << process->size.height
//         << " of nr#: " << process->numFrames << endl;
//    cout << "Input codec type: " << ".avi" << endl;
//
//    VideoWriter outputVideo;
//    outputVideo.open(
//            process->output,
//            CV_FOURCC('D', 'I', 'V', 'X'),
//            process->fps,
//            Size(
//                    process->size.width - process->ver,
//                    process->size.height - process->hor
//            ),
//            true
//    );
//
//    if (!outputVideo.isOpened()) {
//        throw source;
//    }

//    for(int i = 0; i < process->numFrames; ++i) {
//        inputVideo >> process->inFrame;

        cout << "UP ARROW: Shrink horizontally" << endl;
        cout << "LEFT ARROW: Shrink vertically" << endl;
        cout << "q: Quit" << endl;

        Mat image;
        image = imread("data/monteverdi_ritratto.jpg", 1);
        realTime(image, num_workers);


//        outputVideo << process->inFrame;
//        imshow("mainWin", process->inFrame);
//        waitKey(20);

//        break;
//    }

//    inputVideo.release();
//    outputVideo.release();
}