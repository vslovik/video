/*
* File: video.cpp
* ---------------
*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "pcarving.h"

using namespace cv;
using namespace std;

struct State {
    static const int ver = 1;
    static const int hor = 0;
    Size size;
    string output;
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
            string output = "out.avi"
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

State *process;

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
        cout  << "Unknown rotation flag(" << rotflag << ")" << endl;
    }
}

void remove_seam(int i, Mat& image, char orientation = 'v', int num_workers = 1){
    if (orientation == 'h')
        rot90(image, 1);
    int H = image.rows, W = image.cols;

    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);

    Mat eimage;
    energy_function(gray, eimage);

    if(!process->firstFrame){
        int* prev_seam = new int[H];
        for (int r = 0; r < H; r++) {
            if (orientation == 'v') {
                prev_seam[r] = process->v_seams[r * process->ver + i];
            } else {
                prev_seam[r] = process->h_seams[r * process->hor + i];
            }
        }
        coherence_function(eimage, prev_seam);
    }

    int* seam = find_seam(eimage, num_workers);

    if (orientation == 'v') {
        for (int r = 0; r < H; r++) {
            process->v_seams[r * process->ver + i] = seam[r];
        }
    } else {
        for (int r = 0; r < H; r++) {
            process->h_seams[r * process->hor + i] = seam[r];
        }
    }

    Mat output(H, W-1, CV_8UC3);
    remove_pixels(image, output, seam, num_workers);
    if (orientation == 'h')
        rot90(output,2);
    image = output;
}

void realTime(Mat& image, int num_workers = 1){
    cout << "UP ARROW: Shrink horizontally" << endl;
    cout << "LEFT ARROW: Shrink vertically" << endl;
    cout << "q: Quit" << endl;

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

void shrink_image(Mat& image, int ver, int hor, int num_workers = 1){
    cout << endl << "Processing image..." << endl;
    for(int i = 0; i < ver; i++){
        remove_seam(i, image, 'v', num_workers);
    }
    for(int i = 0; i < hor; i++){
        remove_seam(i, image, 'h', num_workers);
    }
}


void process_video(string source, int num_workers = 1)
{
    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        throw source;
    }

    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    Mat inFrame, outFrame;
    process = new State(
            inputVideo.get(CV_CAP_PROP_FPS),
            numFrames,
            Size(
             (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
             (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
            ),
            inFrame,
            outFrame,
            "out.avi"
    );

    cout << "Input frame resolution: Width=" << process->size.width << "  Height=" << process->size.height
         << " of nr#: " << process->numFrames << endl;
    cout << "Input codec type: " << ".avi" << endl;

    VideoWriter outputVideo;
    outputVideo.open(
            process->output,
            CV_FOURCC('D', 'I', 'V', 'X'),
            process->fps,
            Size(
                    process->size.width - process->ver,
                    process->size.height - process->hor
            ),
            true
    );

    if (!outputVideo.isOpened()) {
        throw source;
    }

    int k = 0;
//    for(int i = 0; i < process->numFrames; ++i) {
    for(int i = 103; i < 106; ++i) {
        inputVideo >> process->inFrame;
        cout << "UP ARROW: Shrink horizontally" << endl;
        cout << "LEFT ARROW: Shrink vertically" << endl;
        cout << "q: Quit" << endl;

        Mat image;
//        image = imread("data/monteverdi_ritratto.jpg", 1);

//        if (k > 0)
//            realTime(process->inFrame, num_workers);
//
//        outputVideo << process->inFrame;

        image = process->inFrame;

        if (k > 0) {

//            cout << image.cols << endl;
            //remove_seam(0, image, 'v', num_workers);

            shrink_image(image, process->ver, process->hor, num_workers);
            process->firstFrame = false;

//            cout << image.cols << endl;
            imshow("mainWin", image);
        }


        waitKey(5000);
        k += 1;
    }

    inputVideo.release();
    outputVideo.release();
}

//http://stackoverflow.com/questions/13494499/why-does-qdebug-work-in-release-builds