#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"
#include <opencv2/highgui/highgui.hpp>

#define CW 1;
#define CCW 0;

static void help()
{
	std::cout
			<< "------------------------------------------------------------------------------" << std::endl
			<< "Image seam carving"                                                             << std::endl
			<< "Usage:"                                                                         << std::endl
			<< "./seam image num_workers"                                                       << std::endl
			<< "Ex: ./seam /home/valeriya/project/video/data_/monteverdi_ritratto.jpg 4"        << std::endl
			<< "------------------------------------------------------------------------------" << std::endl
			<< std::endl;
}

void rot90(Mat &matImage, int flag) {
	transpose(matImage, matImage);
	flip(matImage, matImage, flag);
}

uchar max_int = std::numeric_limits<uchar>::max();

void find_seam(Mat &image, int *path, int num_workers = 1){
    int H = image.rows;
    int W = image.cols;

	cv::Point *points = new cv::Point[W];
	uchar *row = new uchar[W];
	int *seams = new int[W * H];


	int minimum = 256;
	int argmin = W + 1;

	ff::ParallelFor pf(num_workers, false);
    for(int r = 0; r < H; r++){

        // Calculate row values
        pf.parallel_for(0L, W, [&row, r, W, &image](int c) {
	        uchar next = image.at<uchar>(r,c);
            if(r > 0) {
                uchar left = c > 0 ? row[c - 1] : max_int;
                uchar right = c < W - 1 ? row[c + 1] : max_int;
                next += std::min({left, row[c], right});
            }
            row[c] = next;
        });

        // Advance seams
        pf.parallel_for(0L,W,[&row, r, W, H, &seams, &points](int c) {
            if(r > 0) {
	            uchar left = c > 0 ? row[c - 1] : max_int;
	            uchar right = c < W - 1 ? row[c + 1] : max_int;
	            uchar middle = row[c];
	            uchar m = std::min({left, middle, right});
                if(r == H - 1) {
	                points[c] = cv::Point(c, m);
                }
                if(m == left)
                    seams[r * W + c] = c - 1;
                else if(m == right)
                    seams[r * W + c] = c + 1;
                else
                    seams[r*W + c] = c;
            } else
                seams[r*W + c] = row[c];
        });
    }

	delete[] row;

	int step = 2;

	while (true) {
		if (step > W) {
			break;
		}

		pf.parallel_for(1, (long) W, (long) step, [W, step, &points](int i) {
			ulong left = (ulong) i - 1;
			ulong right = (ulong) cv::min(W - 1, i - 1 + step - 1);

			if (points[left].y > points[right].y) {
				points[left] = points[right];
			} else {
				points[right] = points[left];
			}
		});

		step = step << 1;
	}

	Point p = points[0];

	delete[] points;

	pf.parallel_for(0, (long)H, [W, &path, &p, &seams](int r) {
		path[r] = seams[r * W + p.x];
	});

	delete[] seams;
}

void remove_pixels(Mat& image, int *seam, int num_workers = 1){
	int W = image.cols;
	int H = image.rows;

	Mat output(image.rows, image.cols - 1, CV_8UC3);

    ff::ParallelFor pf(num_workers, false);
	pf.parallel_for(0L, H*W, [W, &image, seam, &output](int i) {
		int r = i / W;
		int c = i % W;

		if (c >= seam[r])
			output.at<Vec3b>(r,c) = image.at<Vec3b>(r, c + 1);
		else
			output.at<Vec3b>(r,c) = image.at<Vec3b>(r, c);
	});

	image = output;
}

void energy_function(Mat &image, Mat &output, int num_workers = 1){
    sobel(image, output, num_workers);
}

void coherence_function(Mat &image, int* seam, int num_workers = 1) {
    coherence(image, seam, num_workers);
}

void remove_seam(Mat& image, char orientation = 'v', int num_workers = 1){

	if (orientation == 'h') {
		int flag = CW;
		rot90(image, flag);
	}

	Mat eimage;
	energy_function(image, eimage, num_workers);

	int* seam = new int[eimage.rows];
	find_seam(eimage, seam, num_workers);

	remove_pixels(image, seam, num_workers);

	delete[] seam;

	if (orientation == 'h') {
		int flag = CCW;
		rot90(image, flag);
	}
}

void realTime(Mat& image, int num_workers){
	std::cout << "UP ARROW: Shrink horizontally" << std::endl;
	std::cout << "LEFT ARROW: Shrink vertically" << std::endl;
	std::cout << "q: Quit" << std::endl;

	int key;
	while(1) {
		namedWindow("Display window", WINDOW_AUTOSIZE);
		imshow("Display window", image);
		key = waitKey(0);
		if (key == 'q')
			break;
		else if (key == 'v') {
			remove_seam(image, 'v', num_workers);
		}
		else if (key == 'h') {
			remove_seam(image, 'h', num_workers);
		}
	}
}

int main(int argc, char **argv)
{
	if(argc < 3) {
		std::cout << "Not enough parameters" << std::endl;
		return -1;
	}

//    int num_workers = atoi(argv[2]);

	try {

		Mat image = imread(argv[1], IMREAD_COLOR);

//		realTime(image, num_workers);

		for(int num_workers = 1; num_workers <= 20; num_workers++) {
			ff::ffTime(ff::START_TIME);

			Mat eimage;
			energy_function(image, eimage, num_workers);

			int* seam = new int[eimage.rows];
			find_seam(eimage, seam, num_workers);

			remove_pixels(image, seam, num_workers);

			ff::ffTime(ff::STOP_TIME);

			std::cout << "num_workers: " << num_workers << " elapsed time =";
			std::cout << ff::ffTime(ff::GET_TIME) << " ms\n";
		}

	} catch(std::string e){
		std::cout << e << std::endl;
		return -1;
	}

	return 0;
}