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

#define CW 1;
#define CCW 0;

struct State {
    int ver;
    int hor;
    Size size;
	std::string output;
    double fps;
    int numFrames;
    int* v_seams;
    int* h_seams;
    bool firstFrame = true;
	int num_found_v_seams = 0;
	int num_found_h_seams = 0;

    State(
		    int ver,
		    int hor,
            double fps,
            int numFrames,
            Size size,
            std::string output = "out.avi"
    ) :
            ver(ver),
            hor(hor),
            fps(fps),
            numFrames(numFrames),
            size(size),
            output(output),
            v_seams(v_seams),
            h_seams(h_seams),
            num_found_v_seams(num_found_v_seams),
			num_found_h_seams(num_found_h_seams)
    {
        v_seams = (int *)malloc(ver*size.height*sizeof(int));
        h_seams = (int *)malloc(hor*size.width*sizeof(int));
    }
};

State* s;

void retarget_frame(Mat& image, char orientation = 'v', int num_workers = 1){
	if (orientation == 'h') {
		int flag = CW;
		rot90(image, flag);
	}

    Mat eimage;
    energy_function(image, eimage, num_workers);

	int end_seam_index;
	if (orientation == 'v') {
		if (!s->firstFrame)
			coherence_function(eimage, s->v_seams, s->hor, num_workers);
		end_seam_index = s->hor;
		find_seams(eimage, s->v_seams, s->hor, s->num_found_v_seams, end_seam_index, num_workers);
		remove_pixels(image, s->v_seams, s->hor, s->num_found_v_seams, end_seam_index, num_workers);
	} else {
		if (!s->firstFrame)
			coherence_function(eimage, s->h_seams, s->ver, num_workers);
		end_seam_index = s->ver;
		find_seams(eimage, s->h_seams, s->ver, s->num_found_h_seams, end_seam_index, num_workers);
		remove_pixels(image, s->h_seams, s->ver, s->num_found_v_seams, end_seam_index, num_workers);
	}

	s->num_found_h_seams = end_seam_index;

	if (orientation == 'h') {
		int flag = CCW;
		rot90(image, flag);
	}
}

void shrink_image(Mat& image, int num_workers = 1){
	while(image.cols > s->size.width - s->hor){
        retarget_frame(image, 'v', num_workers);
    }
    while(image.rows > s->size.height - s->ver){
        retarget_frame(image, 'h', num_workers);
    }
}

void process_video(std::string source, int ver, int hor, int num_workers = 1)
{
    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        throw source;
    }

    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    s = new State(
		    ver,
		    hor,
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

    VideoWriter outputVideo;
    outputVideo.open(
            s->output,
            CV_FOURCC('D', 'I', 'V', 'X'),
            s->fps,
            Size(
                    s->size.width - s->hor,
                    s->size.height - s->ver
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

        shrink_image(image, num_workers);

	    if (s->firstFrame)
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