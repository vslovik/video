#include <iostream>
#include <opencv2/core/core.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"
#include <opencv2/highgui/highgui.hpp>

struct PMinState {
	PMinState(const ulong n, std::vector<cv::Point> points):
			n(n), points(points) {};
	const ulong n;
	ulong step = 2;
	std::vector<cv::Point> points;
};

uchar max_int = std::numeric_limits<uchar>::max();

cv::Point parallel_min(uchar* s, const int N, int num_workers = 1){

	std::vector<cv::Point> points;
	for (int i = 0; i < N; i++) {
		points.push_back(cv::Point(i, s[i]));
	}

	PMinState* st = new PMinState((ulong) N, points);
	ff::ParallelFor pf(num_workers, false);

	while (true) {
		if (st->step > st->n) {
			break;
		}

		pf.parallel_for(1, st->n, st->step, [&st](int i) {
			ulong left = (ulong) i - 1;
			ulong right = (ulong) cv::min(st->n - 1, (ulong) i - 1 + st->step - 1);

			if (st->points.at(left).y > st->points.at(right).y) {
				st->points.at(left) = st->points.at(right);
			} else {
				st->points.at(right) = st->points.at(left);
			}
		});

		st->step = st->step << 1;
	}

	return st->points.at(0);
};

int *find_seam(Mat &image, int num_workers = 1){
    int H = image.rows;
    int W = image.cols;
    int *seams;
	uchar *scores;
	uchar *row;

	row = (uchar *)malloc(W*sizeof(uchar));
    seams = (int *)malloc(W*H*sizeof(int));
    scores = (uchar *)malloc(W*sizeof(uchar));

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
        pf.parallel_for(0L,W,[&row, r, W, H, &seams, &scores](int c) {
            if(r > 0) {
	            uchar left = c > 0 ? row[c - 1] : max_int;
	            uchar right = c < W - 1 ? row[c + 1] : max_int;
	            uchar middle = row[c];
	            uchar m = std::min({left, middle, right});
                scores[c] = m;
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

	Point p = parallel_min(scores, W, num_workers);

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