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
	int v_seams_found = 0;
	int h_seams_found = 0;

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
            v_seams_found(v_seams_found),
            h_seams_found(h_seams_found)
    {
    }
};

State* s;


void retarget_frame(Mat& image, char orientation = 'v', int num_workers = 1){
	if (orientation == 'h') {
		int flag = CW;
		rot90(image, flag);
	}
    int H = image.rows, W = image.cols;

    Mat eimage;
    energy_function(image, eimage, num_workers);

	int* minimal_seams;
	int num_found;

	if (orientation == 'v') {

		num_found = s->hor - s->v_seams_found; // max num of seams to find

		minimal_seams = find_seams(eimage, num_found);
		remove_pixels(image, minimal_seams, num_found);
//		std::cout << num_found << std::endl;

		s->v_seams_found += num_found;

	} else {

//		num_found = s->ver - s->h_seams_found;

		minimal_seams = find_seams(eimage, num_found);
		remove_pixels(image, minimal_seams, num_found);

//		std::cout << " " << num_found << std::endl;

		s->h_seams_found += num_found;
	}

	delete[] minimal_seams;

	if (orientation == 'h') {
		int flag = CCW;
		rot90(image, flag);
	}
}

void shrink_image(Mat &image, Size out_size, int num_workers = 1) {
	s->v_seams_found = 0;
	s->h_seams_found = 0;
	if (image.cols > out_size.width) {
		while (image.cols > out_size.width) {
			retarget_frame(image, 'v', num_workers);
		}
	}

	if (image.rows > out_size.height) {
		while (image.rows > out_size.height) {
			retarget_frame(image, 'h', num_workers);
		}
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

	Size out_size = Size(
			s->size.width - s->hor,
			s->size.height - s->ver
	);

    VideoWriter outputVideo;
    outputVideo.open(
            s->output,
            CV_FOURCC('D', 'I', 'V', 'X'),
            s->fps,
            out_size,
            true
    );

    if (!outputVideo.isOpened()) {
        throw source;
    }

	Mat image;

	int sum = 0;
	double avg;
    for(int i = 0; i < s->numFrames; ++i) {

        inputVideo >> image;

	    ff::ffTime(ff::START_TIME);

        shrink_image(image, out_size, num_workers);

		ff::ffTime(ff::STOP_TIME);
	    sum += ff::ffTime(ff::GET_TIME);
	    avg = (float) sum / (float) (i + 1);
	    std::cout << i << "/" << s->numFrames << " " << ff::ffTime(ff::GET_TIME) << " ms " << "avg: " << avg << "\n" << std::endl;

//	    imshow("mainWin", image);
//		waitKey(5000);

        outputVideo << image;
    }

    inputVideo.release();
    outputVideo.release();
}