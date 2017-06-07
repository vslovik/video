#include <iostream>
#include <opencv2/core/core.hpp>
#include <ff/parallel_for.hpp>

/* ----- utility function ------- */

#define XY2I(Y,X,COLS) (((Y) * (COLS)) + (X))

/* ------------------------------ */

const float R = 0.3;
const float G = 0.59;
const float B = 0.11;

// returns the gradient in the x direction
static inline long xGradient(uchar * image, long cols, long x, long y) {
    return image[XY2I(y-1, x-1, cols)] +
           2*image[XY2I(y, x-1, cols)] +
           image[XY2I(y+1, x-1, cols)] -
           image[XY2I(y-1, x+1, cols)] -
           2*image[XY2I(y, x+1, cols)] -
           image[XY2I(y+1, x+1, cols)];
}

// returns the gradient in the y direction
static inline long yGradient(uchar * image, long cols, long x, long y) {
    return image[XY2I(y-1, x-1, cols)] +
           2*image[XY2I(y-1, x, cols)] +
           image[XY2I(y-1, x+1, cols)] -
           image[XY2I(y+1, x-1, cols)] -
           2*image[XY2I(y+1, x, cols)] -
           image[XY2I(y+1, x+1, cols)];
}

void sobel(cv::Mat &image, cv::Mat &output, int num_workers) {

	int rows = image.rows;
	int cols = image.cols;

	uchar *src = new uchar[rows * cols];
	uchar *dst = new uchar[rows * cols];

	ff::ffTime(ff::START_TIME);


	ff::ParallelFor pf(num_workers, true);

	ff::ffTime(ff::STOP_TIME);
	std::cout << "cols: " << cols <<" rows: " << rows << std::endl;
	std::cout << "num_workers:::: " << num_workers << " elapsed time =";
	std::cout << ff::ffTime(ff::GET_TIME) << " ms\n";

	ff::ffTime(ff::START_TIME);

	pf.parallel_for(0L, rows, [cols, &src, &image](int r) {
		for (int c = 1; c < cols - 1; c++) {
			cv::Vec3b values = image.at<cv::Vec3b>(r, c);
			int val = (int) (R * values[0] + G * values[1] + B * values[2]);
			if (val > 255)
				val = 255;
			src[r * cols + c] = (uchar) val;
		}
	});

	pf.parallel_for(1, rows - 1, [src, cols, &dst](const long y) {
		for (long x = 1; x < cols - 1; x++) {
			const long gx = xGradient(src, cols, x, y);
			const long gy = yGradient(src, cols, x, y);
			// approximation of sqrt(gx*gx+gy*gy)
			int sum = abs((int) gx) + abs((int) gy);
			if (sum > 255) sum = 255;
			else if (sum < 0) sum = 0;

			dst[y * cols + x] = (uchar) sum;
		}
	});

	pf.threadPause();

	ff::ffTime(ff::STOP_TIME);
	std::cout << "num_workers: " << num_workers << " elapsed time =";
	std::cout << ff::ffTime(ff::GET_TIME) << " ms\n";

	output = cv::Mat(rows, cols, CV_8U, dst, cv::Mat::AUTO_STEP);

//	delete[] dst;
//	delete[] src;
}

void sobel_seq(cv::Mat &image, cv::Mat &output) {

	int rows = image.rows;
	int cols = image.cols;

	uchar * src = new uchar[rows * cols];
	uchar * dst = new uchar[rows * cols];

	ff::ffTime(ff::START_TIME);

	for (int r = 0; r < rows; r++) {
		for (int c = 1; c < cols - 1; c++) {
			cv::Vec3b values = image.at<cv::Vec3b>(r, c);
			int val = (int) (R * values[0] + G * values[1] + B * values[2]);
			if (val > 255)
				val = 255;
			src[r * cols + c] = (uchar) val;
		}
	};

	for (long y = 1; y < rows - 1; y++) {
		for (long x = 1; x < cols - 1; x++) {
			const long gx = xGradient(src, cols, x, y);
			const long gy = yGradient(src, cols, x, y);
			// approximation of sqrt(gx*gx+gy*gy)
			int sum = abs((int) gx) + abs((int) gy);
			if (sum > 255) sum = 255;
			else if (sum < 0) sum = 0;

			dst[y * cols + x] = (uchar) sum;
		}
	};

	ff::ffTime(ff::STOP_TIME);
	std::cout << " elapsed time =";
	std::cout << ff::ffTime(ff::GET_TIME) << " ms\n";

	output = cv::Mat(rows, cols, CV_8U, dst, cv::Mat::AUTO_STEP);

//	delete[] dst;
//	delete[] src;
}

void coherence(cv::Mat &image, int* seams, int num_seams) {

    int rows = image.rows;
    int cols = image.cols;

    uchar * dst = new uchar[rows * cols];
	uchar * src = new uchar[rows * cols];

	int L[cols], R[cols];

	int sum, hole, diff;
	for (int k = 0; k < num_seams; k++) {

		for (unsigned int r = 0; r < rows; r++) {
			std::fill_n(L, cols, 0);
			std::fill_n(R, cols, 0);
			hole = seams[r * num_seams + k];
			if (hole > 0) {
				for (int c = hole - 1; c >= 0; c--) {
					diff = src[r * cols + c] - src[r * cols + c + 1];
					L[c] = L[c + 1] + abs(diff);
				}
			}
			if (hole < cols - 1) {
				for (int c = hole + 1; c < cols; c++) {
					diff = src[r * cols + c] - src[r * cols + c - 1];
					R[c] = R[c - 1] + abs(diff);
				}
			}
			for (int c = 0; c < cols; c++) {
				sum = (uchar) L[c] + R[c];
				dst[r * cols + c] =  (uchar) sum;
			}
		}

		if(k < num_seams - 1)
			std::swap(dst, src);
	}

    image = cv::Mat(rows, cols, CV_8U, dst, cv::Mat::AUTO_STEP);

	delete[] dst;
	delete[] src;
}