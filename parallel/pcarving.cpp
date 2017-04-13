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

cv::Point get_pos(int c, int W, uchar *row) {
	uchar left = c > 0 ? row[c - 1] : max_int;
	uchar right = c < W - 1 ? row[c + 1] : max_int;
	uchar middle = row[c];
	uchar m = std::min({left, middle, right});

	int pos;
	if (m == left)
		pos = c - 1;
	else if (m == right)
		pos = c + 1;
	else
		pos = c;

	return cv::Point(pos, m);
}

void update(int *seams, int *next_traces, int *energy, int *traces, uchar *row, int &count, int next, int prev, int r, int W) {
	int seam_index = traces[prev];
	cv::Point p = get_pos(prev, W, row);

	seams[r * W + seam_index] = next;
	next_traces[next] = seam_index;
	energy[seam_index] = p.y;
	traces[prev] = 0;
	count++;
}

int keep_one_out_of_two(int one, int two, int *traces, int *energy, uchar *row, int W, int c)
{
	if(get_pos(one, W, row).x == c && get_pos(two, W, row).x != c)
		return one;
	else if (get_pos(two, W, row).x == c && get_pos(one, W, row).x != c)
		return two;
	else {
		if(energy[traces[one]] < energy[traces[two]]) {
			return one;
		}
		else {
			return two;
		}
	}
}

int keep_one_out_of_three(int *traces, int *energy, uchar *row, int W, int c)
{
	if(get_pos(c - 1, W, row).x != c)
		return keep_one_out_of_two(c, c + 1, traces, energy, row, W, c);
	else if(get_pos(c, W, row).x != c)
		return keep_one_out_of_two(c - 1, c + 1, traces, energy, row, W, c);
	else if(get_pos(c + 1, W, row).x != c)
		return keep_one_out_of_two(c - 1, c, traces, energy, row, W, c);
	else {
		int m = std::min({energy[traces[c - 1]], energy[traces[c]], energy[traces[c + 1]]});
		if(m == energy[traces[c - 1]])
			return c - 1;
		else if(m == energy[traces[c]])
			return c;
		else
			return c + 1;
	}
}

void find_seam(Mat &image, int *path, int num_workers = 1){
	int H = image.rows;
	int W = image.cols;

	int *energy = new int[W];
	int *traces = new int[W];
	int *next_traces = new int[W];

	int pos;
	uchar *row = new uchar[W];
	uchar *next_row = new uchar[W];
	int *seams = new int[W * H];

	int count = 0;
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
			next_traces[c] = 0;
			count = 0;
		});

		row = next_row;

		// Advance seams
		pf.parallel_for(0L, W,[row, r, W, H, &seams, &traces](int c) {
			if(r > 0) {
				if (c == 0)
					pos = keep_one_out_of_two(c, c + 1, traces, energy, row, W, c);
				else if (c == W - 1)
					pos = keep_one_out_of_two(c - 1, c, traces, energy, row, W, c);
				else
					pos = keep_one_out_of_three(traces, energy, row, W, c);

				update(seams, next_traces, energy, traces, row, count, c, pos, r, W);

			} else {
				seams[r * W + c] = c;
				traces[c] = c;
				energy[c] = row[c];
				count++;
			}
		});

		int thr = THRESHOLD // keep discarded seams (splitting)
		if(r > H * thr / 100) {

			pf.parallel_for(0L, W, [row, r, W, H, &seams, &traces, &i](int c) {
				if(next_traces[c] == 0) {
					int seam_index = 0;
					if(traces[c - 1] != 0)
						seam_index = traces[c - 1];
					else if (traces[c] != 0)
						seam_index = traces[c];
					else if (traces[c + 1] != 0)
						seam_index = traces[c + 1];
					if(seam_index) {
						seams[r * W + seam_index] = c;
						next_traces[c] = seam_index;
						energy[seam_index] = row[c];
						count++;
					}
				}
			});
		}

		traces = next_traces;
	}

	cv::Point *epoints = new cv::Point[count];
	count = 0;
	for(int c = 0; c < W; c++) {
		if(traces[c] != 0) {
			epoints[count++] = Point(traces[c], energy[traces[c]]);
		}
	}

	delete[] row;
	delete[] traces;
	delete[] next_traces;
	delete[] energy;

	int step = 2;

	while (true) {

		pf.parallel_for(1, (long) W, (long) step, [W, step, &epoints](int i) {
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