/*
* File: video.cpp
* ---------------
*/
#include <ctime>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ff/utils.hpp>
#include "pcarving.h"

struct State {
    static const int ver = 20;
    static const int hor = 20;
    Size size;
	std::string output;
    double fps;
    int numFrames;
    int* v_seams;
    int* h_seams;
    bool firstFrame = true;

    State(
            double fps,
            int numFrames,
            Size size,
            std::string output = "out.avi"
    ) :
            fps(fps),
            numFrames(numFrames),
            size(size),
            output(output),
            v_seams(v_seams),
            h_seams(h_seams)
    {
        v_seams = (int *)malloc(ver*size.height*sizeof(int));
        h_seams = (int *)malloc(hor*size.width*sizeof(int));
    }
};

State* s;

void retarget_frame(int i, Mat& image, char orientation = 'v', int num_workers = 1){
    if (orientation == 'h')
        rot90(image, 1);
    int H = image.rows, W = image.cols;

    Mat eimage;
    energy_function(image, eimage, num_workers);

    if(!s->firstFrame){
        int* prev_seam = new int[H];
        for (int r = 0; r < H; r++) {
            if (orientation == 'v')
                prev_seam[r] = s->v_seams[r * s->ver + i];
            else
                prev_seam[r] = s->h_seams[r * s->hor + i];
        }

        coherence_function(eimage, prev_seam, num_workers);
    }

	int* seam = new int[eimage.rows];
	find_seam(eimage, seam, num_workers);

    if (orientation == 'v') {
        for (int r = 0; r < H; r++)
            s->v_seams[r * s->ver + i] = seam[r];
    } else {
        for (int r = 0; r < H; r++)
            s->h_seams[r * s->hor + i] = seam[r];
    }

    remove_pixels(image, seam, num_workers);

	delete[] seam;

    if (orientation == 'h')
        rot90(image, 2);
}

void shrink_image(Mat& image, int ver, int hor, int num_workers = 1){
    for(int i = 0; i < ver; i++){
        retarget_frame(i, image, 'v', num_workers);
    }
    for(int i = 0; i < hor; i++){
        retarget_frame(i, image, 'h', num_workers);
    }
}

void process_video(std::string source, int num_workers = 1)
{
    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        throw source;
    }

    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    s = new State(
            inputVideo.get(CV_CAP_PROP_FPS),
            numFrames,
            Size(
             (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
             (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
            ),
            "out.avi"
    );

	std::cout << "Input frame resolution: Width=" << s->size.width << "  Height=" << s->size.height
         << " of nr#: " << s->numFrames << std::endl;
	std::cout << "Input codec type: " << ".avi" << std::endl;

    VideoWriter outputVideo;
    outputVideo.open(
            s->output,
            CV_FOURCC('D', 'I', 'V', 'X'),
            s->fps,
            Size(
                    s->size.width - s->ver,
                    s->size.height - s->hor
            ),
            true
    );

    if (!outputVideo.isOpened()) {
        throw source;
    }


    for(int i = 0; i < s->numFrames; ++i) {

	    Mat image;

        inputVideo >> image;

	    ff::ffTime(ff::START_TIME);

        shrink_image(image, s->ver, s->hor, num_workers);
        s->firstFrame = false;

		ff::ffTime(ff::STOP_TIME);

//      imshow("mainWin", image);
//      waitKey(5000);

	    std::cout << i << "/" << s->numFrames << " " << ff::ffTime(ff::GET_TIME) << " ms\n" << " " << std::endl;
        outputVideo << image;
    }

    inputVideo.release();
    outputVideo.release();
}