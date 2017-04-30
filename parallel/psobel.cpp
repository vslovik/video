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

	uchar * src = new uchar[rows * cols];
	uchar * dst = new uchar[rows * cols];

	ff::ParallelFor pf(num_workers, false);
	pf.parallel_for(0L, rows, [cols, &src, &image](int r) {
		for(int c = 1; c < cols - 1; c++) {
			cv::Vec3b values = image.at<cv::Vec3b>(r, c);
			int val = (int) (R * values[0] + G * values[1] + B * values[2]);
			if (val > 255)
				val = 255;
			src[r * cols + c] = (uchar) val;
		}
	});

    pf.parallel_for(1, rows - 1,[src, cols, &dst](const long y) {
        for(long x = 1; x < cols - 1; x++){
            const long gx = xGradient(src, cols, x, y);
            const long gy = yGradient(src, cols, x, y);
            // approximation of sqrt(gx*gx+gy*gy)
            int sum = abs((int)gx) + abs((int)gy);
            if (sum > 255) sum = 255;
            else if (sum < 0) sum = 0;

            dst[y*cols+x] = (uchar) sum;
        }
    });

    output = cv::Mat(rows, cols, CV_8U, dst, cv::Mat::AUTO_STEP);

	delete[] dst;
	delete[] src;
}

void coherence(cv::Mat &image, int* seams, int num_seams, int num_workers) {

    int rows = image.rows;
    int cols = image.cols;

    uchar * dst = new uchar[rows * cols];
	uchar * src = new uchar[rows * cols];

	ff::ParallelFor pf(num_workers, false);
	pf.parallel_for(0L, rows*cols, [cols, &src, &image](int i) {
		int r = i / cols;
		int c = i % cols;
		src[r * cols + c] = image.at<uchar>(r, c);
	});

	int Il[cols], Ir[cols];

    pf.parallel_for(0L, rows , [src, cols, &dst, &seams, num_seams, &Il, &Ir](const long r) {
        int sum;
	    for(int k = 0; k < num_seams; k++) {
		    for (int c = 0; c < cols - 1; c++) {
			    Il[c] = 0;
			    Ir[c] = 0;
		    }
		    for (int c = seams[r*num_seams + k]; c < cols; c++) {
			    Il[c] = Il[c - 1] + abs(src[r * cols + c - 1] - src[r * cols + c]);
		    }
		    for (int c = cv::min(cols - 2, seams[r*num_seams + k]); c >= 0; c--) {
			    Ir[c] = Ir[c + 1] + abs(src[r * cols + c + 1] - src[r * cols + c]);
		    }
		    for (int c = 1; c < cols; c++) {
			    sum = Il[c] + Ir[c] + src[r * cols + c];
			    if (sum > 255) sum = 255;
			    else if (sum < 0) sum = 0;
			    dst[r * cols + c] = (uchar) sum;
		    }
	    }
    });

    image = cv::Mat(rows, cols, CV_8U, src, cv::Mat::AUTO_STEP);
}