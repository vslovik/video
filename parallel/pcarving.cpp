#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"
#include <opencv2/highgui/highgui.hpp>

struct PMinState {
	PMinState(const int n, cv::Point *points):
			n(n), points(points) {};
	const int n;
	int step = 2;
	cv::Point *points;
};

uchar max_int = std::numeric_limits<uchar>::max();

cv::Point parallel_min(cv::Point *points, const int N, int num_workers = 1){

	PMinState* st = new PMinState(N, points);
	ff::ParallelFor pf(num_workers, false);

	while (true) {
		if (st->step > st->n) {
			break;
		}

		pf.parallel_for(1, (long) st->n, (long) st->step, [&st](int i) {
			ulong left = (ulong) i - 1;
			ulong right = (ulong) cv::min(st->n - 1, i - 1 + st->step - 1);

			if (st->points[left].y > st->points[right].y) {
				st->points[left] = st->points[right];
			} else {
				st->points[right] = st->points[left];
			}
		});

		st->step = st->step << 1;
	}

	return st->points[0];
};



int *find_seam(Mat &image, int num_workers = 1){
    int H = image.rows;
    int W = image.cols;
    int *seams;
	uchar *row;
	cv::Point *points;

	points = (cv::Point *) malloc(W * sizeof(cv::Point));
	row = (uchar *) malloc(W * sizeof(uchar));
	seams = (int *) malloc(W * H * sizeof(int));

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

	Point p = parallel_min(points, W, num_workers);

	int *path = new int[H];
    for(int r = 0; r < H; r++)
        path[r] = seams[r * W + p.x];

    return path;
}

void remove_pixels(Mat& image, Mat& output, int *seam, int num_workers = 1){
	int W = image.cols;

    ff::ParallelFor pf(num_workers, false);

    for(int r = 0; r < image.rows; r++ ) {
        pf.parallel_for(0L, W, [r, W, &image, &seam, &output](int c) {
            if (c >= seam[r])
                output.at<Vec3b>(r,c) = image.at<Vec3b>(r, c + 1);
            else
                output.at<Vec3b>(r,c) = image.at<Vec3b>(r, c);
        });
    }
}

void energy_function(Mat &image, Mat &output, int num_workers = 1){
    sobel(image, output, 8);
}


void coherence_function(Mat &image, int* seam, int num_workers = 1) {
    coherence(image, seam, 8);
}

int main(int argc, char **argv)
{
	std::cout << "find seam" << std::endl;
	try {
		Mat image;


        image = imread("../data_/monteverdi_ritratto.jpg", 1);

		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		Mat eimage;
		energy_function(gray, eimage, 4);

		std::time_t t0 = std::time(0);

		for(int k = 0; k < 100; k++) {
			int *seam = find_seam(image, 4);

//		for(int r = 0; r < image.rows; r++) {
//			std::cout << seam[r] << std::endl;
//		}
		}

		std::cout <<  std::time(0) - t0 << std::endl;

	} catch(std::string e){
		std::cout << e << std::endl;
		return -1;
	}

	return 0;
}