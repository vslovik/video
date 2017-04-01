/*
* File: video.cpp
* ---------------
*/
#include <ctime>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "pcarving.h"

struct State {
    static const int ver = 20;
    static const int hor = 20;
    Size size;
	std::string output;
    double fps;
    int numFrames;
    Mat inFrame, outFrame;
    int* v_seams;
    int* h_seams;
    bool firstFrame = true;

    State(
            double fps,
            int numFrames,
            Size size,
            Mat inFrame,
            Mat outFrame,
            std::string output = "out.avi"
    ) :
            fps(fps),
            numFrames(numFrames),
            size(size),
            inFrame(inFrame),
            outFrame(outFrame),
            output(output),
            v_seams(v_seams),
            h_seams(h_seams)
    {
        v_seams = (int *)malloc(ver*size.height*sizeof(int));
        h_seams = (int *)malloc(hor*size.width*sizeof(int));
    }
};

State* s;

//http://stackoverflow.com/questions/16865730/how-to-to-parallelize-the-matrix-transpose
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
	    std::cout  << "Unknown rotation flag(" << rotflag << ")" << std::endl;
    }
}

void remove_seam(int i, Mat& image, char orientation = 'v', int num_workers = 1){
    if (orientation == 'h')
        rot90(image, 1);
    int H = image.rows, W = image.cols;

    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);

    Mat eimage;
    energy_function(gray, eimage, num_workers);

//    if(!s->firstFrame){
//        int* prev_seam = new int[H];
//        for (int r = 0; r < H; r++) {
//            if (orientation == 'v') {
//                prev_seam[r] = s->v_seams[r * s->ver + i];
//            } else {
//                prev_seam[r] = s->h_seams[r * s->hor + i];
//            }
//        }
//        coherence_function(eimage, prev_seam, num_workers);
//    }

    int* seam = find_seam(eimage, num_workers);

//    if (orientation == 'v') {
//        for (int r = 0; r < H; r++) {
//            s->v_seams[r * s->ver + i] = seam[r];
//        }
//    } else {vvvvvvvvvvvvvvvvv
//        for (int r = 0; r < H; r++) {
//            s->h_seams[r * s->hor + i] = seam[r];
//        }
//    }

    Mat output(H, W-1, CV_8UC3);
    remove_pixels(image, output, seam, num_workers);
    if (orientation == 'h')
        rot90(output,2);
    image = output;
}

void shrink_image(Mat& image, int ver, int hor, int num_workers = 1){
//    cout << endl << "Processing image..." << endl;
    for(int i = 0; i < ver; i++){
        remove_seam(i, image, 'v', num_workers);
    }
    for(int i = 0; i < hor; i++){
        remove_seam(i, image, 'h', num_workers);
    }
}

void realTime(Mat& image, int num_workers){
    std::cout << "UP ARROW: Shrink horizontally" << std::endl;
    std::cout << "LEFT ARROW: Shrink vertically" << std::endl;
    std::cout << "q: Quit" << std::endl;

    int key;
    int iv = 0, ih = 0;
    while(1) {
        namedWindow("Display window", WINDOW_AUTOSIZE);
        imshow("Display window", image);
        key = waitKey(0);
        if (key == 'q')
            break;
        else if (key == 'v') {
            remove_seam(iv++, image, 'v', num_workers);
        }
        else if (key == 'h') {
            remove_seam(ih++, image, 'h', num_workers);
        }
    }
}

void process_video(std::string source, int num_workers = 1)
{
//    VideoCapture inputVideo(source);
//    if (!inputVideo.isOpened())
//    {
//        throw source;
//    }
//
//    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
//    Mat inFrame, outFrame;
//    s = new State(
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
//	std::cout << "Input frame resolution: Width=" << s->size.width << "  Height=" << s->size.height
//         << " of nr#: " << s->numFrames << std::endl;
//	std::cout << "Input codec type: " << ".avi" << std::endl;
//
//    VideoWriter outputVideo;
//    outputVideo.open(
//            s->output,
//            CV_FOURCC('D', 'I', 'V', 'X'),
//            s->fps,
//            Size(
//                    s->size.width - s->ver,
//                    s->size.height - s->hor
//            ),
//            true
//    );
//
//    if (!outputVideo.isOpened()) {
//        throw source;
//    }

//
//    for(int i = 0; i < s->numFrames; ++i) {

//        inputVideo >> s->inFrame;
//	    std::time_t t = std::time(0);

//        cout << "UP ARROW: Shrink horizontally" << endl;
//        cout << "LEFT ARROW: Shrink vertically" << endl;
//        cout << "q: Quit" << endl;

        Mat image;

        image = imread("../data_/monteverdi_ritratto.jpg", 1);
        realTime(image, num_workers);
//        remove_seam(0, image, 'v', num_workers);


//        image = s->inFrame;
//        shrink_image(image, s->ver, s->hor, num_workers);
//        s->firstFrame = false;

//        cout << image.rows << "---" << image.cols << endl;
//        imshow("mainWin", image);
//        waitKey(5000);

//	    std::cout << i << "/" << s->numFrames << " " << std::time(0) - t << std::endl;
//        outputVideo << image;
//    }

//    inputVideo.release();
//    outputVideo.release();
}