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
#define THRESHOLD 20;
#define LIMIT 10;

struct State {
    int ver;
    int hor;
    Size size;
	std::string output;
    double fps;
    int numFrames;
	int* prev_frame_v_seams;
	int* prev_frame_h_seams;
    int* v_seams;
    int* h_seams;
	int v_seams_found = 0;
	int h_seams_found = 0;
    bool firstFrame = true;

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
            prev_frame_v_seams(prev_frame_v_seams),
            prev_frame_h_seams(prev_frame_v_seams),
            v_seams(v_seams),
            h_seams(h_seams),
            v_seams_found(v_seams_found),
            h_seams_found(h_seams_found)
    {
	    prev_frame_v_seams = (int *)malloc(hor*size.height*sizeof(int));
	    prev_frame_h_seams = (int *)malloc(ver*size.width*sizeof(int));
        v_seams = (int *)malloc(hor*size.height*sizeof(int));
        h_seams = (int *)malloc(ver*size.width*sizeof(int));
    }
};

State* s;

void retarget_frame(Mat& image, int limit, char orientation = 'v', int num_workers = 1){
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

		num_found = cv::min(s->hor - s->v_seams_found, limit); // max num of seams to find

		if (!s->firstFrame) {
			if(limit != s->hor) {
				int seams[H*limit];
				for (int i = 0; i < num_found; i++) {
					for (int r = 0; r < H; r++) {
						seams[r * limit + i] = s->prev_frame_v_seams[r * s->hor  + s->v_seams_found + i];
					}
				}
				coherence_function(eimage, seams, num_found, num_workers);
			} else {
				coherence_function(eimage, s->prev_frame_v_seams, s->hor, num_workers);
			}
		}

		minimal_seams = find_seams(eimage, num_found, num_workers);
		remove_pixels(image, minimal_seams, num_found, num_workers);

		for (int r = 0; r < H; r++) {
			for (int i = 0; i < num_found; i++) {
				s->v_seams[r * s->hor + s->v_seams_found + i] = minimal_seams[r * num_found + i];
			}
		}

		s->v_seams_found += num_found;

	} else {

		num_found = cv::min(s->ver - s->h_seams_found, limit);

		if (!s->firstFrame) {
			if(limit != s->ver) {
				int seams[H*limit];
				for (int i = 0; i < num_found; i++) {
					for (int r = 0; r < H; r++) {
						seams[r * limit + i] = s->prev_frame_h_seams[r * s->ver  + s->h_seams_found + i];
					}
				}
				coherence_function(eimage, seams, num_found, num_workers);
			} else {
				coherence_function(eimage, s->prev_frame_h_seams, s->ver, num_workers);
			}
		}

		minimal_seams = find_seams(eimage, num_found, num_workers);
		remove_pixels(image, minimal_seams, num_found, num_workers);

		for (int r = 0; r < H; r++) {
			for (int i = 0; i < num_found; i++) {
				s->h_seams[r * s->ver + s->h_seams_found + i] = minimal_seams[r * num_found + i];
			}
		}

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

	int threshold = THRESHOLD;
	int limit_param = LIMIT;
	int limit;
	if (image.cols > out_size.width) {
		if(s->hor > threshold)
			limit = limit_param;
		else
			limit = s->hor;
		while (image.cols > out_size.width) {
			retarget_frame(image, limit, 'v', num_workers);
		}
		std::swap(s->v_seams, s->prev_frame_v_seams);
	}

	if (image.rows > out_size.height) {
		while (image.rows > out_size.height) {
			if(s->ver > threshold)
				limit = limit_param;
			else
				limit = s->ver;
			retarget_frame(image, limit, 'h', num_workers);
		}

		std::swap(s->h_seams, s->prev_frame_h_seams);
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

	    if (s->firstFrame)
            s->firstFrame = false;

		ff::ffTime(ff::STOP_TIME);

//      imshow("mainWin", image);
//      waitKey(5000);

	    sum += ff::ffTime(ff::GET_TIME);
	    avg = (float) sum / (float) (i + 1);
	    std::cout << i << "/" << s->numFrames << " " << ff::ffTime(ff::GET_TIME) << " ms " << "avg: " << avg << "\n" << std::endl;
        outputVideo << image;
    }

    inputVideo.release();
    outputVideo.release();
}