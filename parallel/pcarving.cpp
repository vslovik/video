#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"
#include <opencv2/highgui/highgui.hpp>

#define CW 1;
#define CCW 0;
#define THRESHOLD 5;

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

	int *traces = new int[W];
	bool *discarded = new bool[W];
	cv::Point *epoints = new cv::Point[W];
	uchar *row = new uchar[W];
	uchar *next_row = new uchar[W];
	int *seams = new int[W * H];

	for(int c = 0; c < W; c++) {
		discarded[c] = false;
	}

	ff::ParallelFor pf(num_workers, false);
	for(int r = 0; r < H; r++){

		// Calculate row values
		pf.parallel_for(0L, W, [&next_row, row, r, W, image, &traces](int c) {
			uchar next = image.at<uchar>(r,c);
			if(r > 0) {
				uchar left = c > 0 ? row[c - 1] : max_int;
				uchar right = c < W - 1 ? row[c + 1] : max_int;
				next += std::min({left, row[c], right});
			}
			next_row[c] = next;
			traces[c] = 0;
		});

		row = next_row;

		// Advance seams
		pf.parallel_for(0L,W,[row, r, W, H, &seams, &epoints, &traces, &discarded](int c) {
			if(r > 0) {

				int sc = seams[(r-1)*W + c];
				if(!discarded[c]) {
					uchar left = sc > 0 ? row[sc - 1] : max_int;
					uchar right = sc < W - 1 ? row[sc + 1] : max_int;
					uchar middle = row[sc];
					uchar m = std::min({left, middle, right});

					int pos;
					if (m == left)
						pos = sc - 1;
					else if (m == right)
						pos = sc + 1;
					else
						pos = sc;


					//ToDo: move to the next row loop to avoid race cond
					if (traces[pos] == 0) {
						traces[pos] = c;
						epoints[c] = cv::Point(c, m);
						seams[r * W + c] = pos;
					} else {
//						int  thr = THRESHOLD;
//						if(r > H * thr / 100) {
//							int key = 0; int val = max_int + 1;
//							for( int k = sc - 1; k < sc + 2; k++){
//								if(k != pos && traces[k] == 0) {
//									if(val > row[k]) {
//										val = row[k];
//										key = k;
//									}
//								}
//							}
//							if(val < max_int + 1) {
//								traces[pos] = c;
//								epoints[c] = cv::Point(c, val);
//								seams[r * W + c] = key;
//							} else {
//								discarded[c] = true;
//								epoints[c] = cv::Point(c, max_int);
//							}
//						} else {
							int index = traces[pos];
							if (epoints[index].y > m) {
								discarded[c] = true;
								epoints[c] = cv::Point(c, max_int); //ToDo
							} else {
								discarded[index] = true;
								epoints[index] = cv::Point(c, max_int); //ToDo: not needed

								epoints[c] = cv::Point(c, m);
								seams[r * W + c] = pos;
							}
//						}
					}
				}
			} else
				seams[r * W + c] = c;

		});
	}

	delete[] row;

	int step = 2;

	while (true) {

		pf.parallel_for(1, (long) W, (long) step, [W, step, &epoints, &discarded](int i) {
			ulong left = (ulong) i - 1;
			ulong right = (ulong) cv::min(W - 1, i - 1 + step - 1);

			if (epoints[left].y > epoints[right].y) {
				epoints[left] = epoints[right];
			} else {
				epoints[right] = epoints[left];
			}
		});

		if (step > W) {
			break;
		}

		step = step << 1;
	}

	Point p = epoints[0];

	delete[] epoints;

	pf.parallel_for(0, (long)H, [W, &path, p, seams, &image](int r) {
		path[r] = seams[r * W + p.x];
//		image.at<uchar>(r,path[r]) = 255;
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

//	for(int r = 0; r < image.rows; r++) {
//		image.at<Vec3b>(r, seam[r]) = Vec3b(255, 255, 255);
//	}

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

	int num_workers = atoi(argv[2]);

	try {

		Mat image = imread(argv[1], IMREAD_COLOR);

		realTime(image, num_workers);

//		ff::ffTime(ff::START_TIME);
//
//		for(int k = 0; k < 100; k++)
//			remove_seam(image, 'v', num_workers);
//
//		ff::ffTime(ff::STOP_TIME);
//
//		std::cout << "num_workers: " << num_workers << " elapsed time =" ;
//		std::cout << ff::ffTime(ff::GET_TIME) << " ms\n";

	} catch(std::string e){
		std::cout << e << std::endl;
		return -1;
	}

	return 0;
}