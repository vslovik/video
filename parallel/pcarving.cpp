#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>

#define CW 1;
#define CCW 0;

bool sortByY(const cv::Point &lhs, const cv::Point &rhs) { return lhs.y < rhs.y; }

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

uchar max_uchar = std::numeric_limits<uchar>::max();
int max_int = std::numeric_limits<int>::max();

void print_traces(int *traces, int W) {
	int j=0;

	for(unsigned int i=0;i<4*W;i++) {
		if (j == W) {
			printf("\n");
			j=0;
		}
		j++;
		printf("%*d", 6, traces[i]);
	}
	printf("\n\n");
}

void print_row(uchar *row, int W) {
	int j=0;

	for(unsigned int i=0;i<W;i++) {
		if (j == W) {
			printf("\n");
			j=0;
		}
		j++;
		printf("%*d", 6, row[i]);
	}
	printf("\n\n");
}

void print_seams(int *seams, int W, int H) {
	int j=0;
	for(unsigned int r=0;r<H;r++) {
		for (unsigned int i = 0; i < W; i++) {
			if (j == W) {
				printf("\n");
				j = 0;
			}
			j++;
			printf("%*d", 6, seams[r*W + i]);
		}
		printf("\n\n");
	}
}

void advance_seams(uchar *row, int r, int W, int H, int *seams, int *traces, int *seam_energies, int *seam_spans, int c) {
	if (r > 0) {
		if (traces[3 * W + c] < W) {
			int seam_index = traces[3 * W + c]; // take seam
			int sc = seams[(r - 1) * W + seam_index]; // take seam position in prev row
			if (sc == c) { // if seam have not been discarded before
				uchar left = c > 0 ? row[c - 1] : max_uchar;
				uchar right = c < W - 1 ? row[c + 1] : max_uchar;
				uchar middle = row[c];
				uchar m = std::min({left, middle, right});

				if (m == left) {
					seams[r * W + seam_index] = c - 1;
					traces[c - 1] = seam_index;
				} else if (m == right) {
					seams[r * W + seam_index] = c + 1;
					traces[2 * W + c + 1] = seam_index;
				} else {
					seams[r * W + seam_index] = c;
					traces[W + c] = seam_index;
				}
				seam_spans[seam_index] += seams[(r - 1) * W + seam_index] != seams[r * W + seam_index] ? 2 : 1;
				seam_energies[seam_index] = seam_energies[seam_index] + m;
			}
		}
	} else {
		seams[r * W + c] = c;
		traces[W + c] = c;
		seam_spans[c] = 0;
		seam_energies[c] = 0;
	}
}

void resolve_seams_conflicts(int r, int W, int H, int *seams, int *traces, int *seam_energies, int *seam_spans, int c) {
	if (traces[3 * W + c] == W) {
		int seam_index;
		int energy;
		int min_energy = max_int;
		int min_span = W + 1;
		int best_seam_index = W;
		for (int i = 0; i < 3; i++) {
			if (traces[i * W + c] == W) {
				continue;
			}
			seam_index = traces[i * W + c];
			energy = seam_energies[seam_index];
			int span = seam_spans[seam_index];

			if (min_energy > energy) {
				min_energy = energy;
				best_seam_index = seam_index;
			} else if (min_energy == energy && min_span > span) {
				min_span = span;
				best_seam_index = seam_index;
			}
		}
		for (int i = 0; i < 3; i++) {
			if (traces[i * W + c] == W) {
				continue;
			}
			seam_index = traces[i * W + c];
			if (seam_index == best_seam_index) {
				traces[i * W + c] = W;
			}
		}
		traces[3 * W + c] = best_seam_index;
	}
};

void move_defeated(uchar* row, int r, int W, int H, int *seams, int *traces, int *seam_energies, int *seam_spans, int c) {
	if (traces[3 * W + c] < W) {
		for (int i = 0; i < 3; i++) {
			if (traces[i * W + c] < W) {
				if(i == 0) {
					if(c - 2 > 0 && traces[3 * W + c - 2] == W && traces[3 * W + c - 1] == W) {
						if(row[c - 2] < row[c - 1]) {
							traces[c - 2] = traces[c];
							seams[r*W + traces[c]] = c - 2;
							seam_energies[traces[c]] += row[c - 2] - row[c];
						} else {
							traces[c - 1] = traces[c];
							seams[r*W + traces[c]] = c - 1;
							seam_energies[traces[c]] += row[c - 1] - row[c];
							seam_spans[traces[c]] -= 1;
						}
						traces[c] = W;
					}
					else if(c - 2 >= 0 && traces[3 * W + c - 2] < W && traces[3 * W + c - 1] == W || c == 1 && traces[3 * W] == W ) {
						traces[c - 1] = traces[c];
						traces[c] = W;
						seams[r*W + traces[c]] = c - 1;
						seam_energies[traces[c]] += row[c - 1] - row[c];
						seam_spans[traces[c]] -= 1;
					}
					else if(c - 2 >= 0 && traces[3 * W + c - 2] == W && traces[3 * W + c - 1] < W) {
						traces[c - 2] = traces[c];
						traces[c] = W;
						seams[r*W + traces[c]] = c - 2;
						seam_energies[traces[c]] += row[c - 2] - row[c];
					}

				} else if(i == 1) {
					if(c - 1 >= 0 && traces[3 * W + c - 1] == W && c + 1 < W && traces[3 * W + c + 1] == W) {
						if(row[c - 1] < row[c + 1]) {
							traces[W + c - 1] = traces[W + c];
							seams[r*W + traces[W + c]] = c - 1;
							seam_energies[traces[W + c]] += row[c - 1] - row[c];
							seam_spans[traces[W + c]] += 1;
						} else {
							traces[W + c + 1] = traces[W + c];
							seams[r*W + traces[W + c]] = c + 1;
							seam_energies[traces[W + c]] += row[c + 1] - row[c];
							seam_spans[traces[W + c]] += 1;
						}
						traces[W + c] = W;
					}
					else if(c - 1 >= 0 && traces[3 * W + c - 1] < W && traces[3 * W + c + 1] == W || c == 0 && traces[3 * W + c + 1] == W) {
						traces[W + c + 1] = traces[W + c];
						seams[r*W + traces[W + c]] = c + 1;
						seam_energies[traces[W + c]] += row[c + 1] - row[c];
						seam_spans[traces[W + c]] += 1;
						traces[W + c] = W;
					}
					else if(c - 1 >= 0 && traces[3 * W + c - 1] == W && c + 1 < W && traces[3 * W + c + 1] < W) {
						traces[W + c - 1] = traces[W + c];
						seams[r*W + traces[W + c]] = c - 1;
						seam_energies[traces[W + c]] += row[c - 1] - row[c];
						seam_spans[traces[W + c]] += 1;
						traces[W + c] = W;
					}
				} else {
					if(c + 2 < W && traces[3 * W + c + 1] == W && traces[3 * W + c + 2] == W) {
						if(row[c + 1] < row[c + 2]) {
							traces[2*W + c + 1] = traces[2*W + c];
							seams[r*W + traces[2*W + c]] = c + 1;
							seam_energies[traces[2*W + c]] += row[c + 1] - row[c];
							seam_spans[traces[2*W + c]] -= 1;
						} else {
							traces[2*W + c + 2] = traces[2*W + c];
							seams[r*W + traces[2*W + c]] = c + 2;
							seam_energies[traces[2*W + c]] += row[c + 2] - row[c];
						}
						traces[2*W + c] = W;
					}
					else if(c + 2 < W && traces[3 * W + c + 1] < W && traces[3 * W + c + 2] == W) {
						traces[2*W + c + 2] = traces[2*W + c];
						seams[r*W + traces[2*W + c]] = c + 2;
						seam_energies[traces[2*W + c]] += row[c + 2] - row[c];
						traces[2*W + c] = W;
					}
					else if(c + 2 < W && traces[3 * W + c + 1] == W && traces[3 * W + c + 2] < W || c + 1 < W && traces[3 * W + c + 1] == W) {
						traces[2*W + c + 1] = traces[2*W + c];
						seams[r*W + traces[2*W + c]] = c + 1;
						seam_energies[traces[2*W + c]] += row[c + 1] - row[c];
						seam_spans[traces[2*W + c]] -= 1;
						traces[2*W + c] = W;
					}
				}
			}
		}
	}
}

int* find_seams(Mat &image, int &num_found){
	int H = image.rows;
	int W = image.cols;

	int seams[image.cols * image.rows];
	int traces[4*image.cols];
	uchar *row = new uchar[W];
	uchar *next_row = new uchar[W];
	int seam_spans[W];
	int seam_energies[W];

	for (unsigned int i = 0; i < 4*W; i++) {
		traces[i] = W;
	};

	for(int r = 0; r < H; r++){

		// calculate row values
		for (unsigned int c = 0; c < W; c++) {
			uchar next = image.at<uchar>(r,c);
			if(r > 0) {
				uchar left = c > 0 ? row[c - 1] : max_uchar;
				uchar right = c < W - 1 ? row[c + 1] : max_uchar;
				next += std::min({left, row[c], right});
			}
			next_row[c] = next;
		}

		std::swap(row, next_row);

		for (unsigned int c = 0; c < W; c++) {
			advance_seams(row, r, W, H, seams, traces, seam_energies, seam_spans, c);
		};

		for (unsigned int c = 0; c < W; c++) {
			traces[3 * W + c] = W;
		}

		for (unsigned int m = 0; m < 3; m++) {
			for (unsigned int c = 0; c < W; c++) {
				resolve_seams_conflicts(r, W, H, seams, traces, seam_energies, seam_spans, c);
			};

			for (unsigned int c = 0; c < W; c++) {
				move_defeated(row, r, W, H, seams, traces, seam_energies, seam_spans, c);
			};
		}

		// clean traces rows
		for (unsigned int c = 0; c < W; c++) {
			for (int i = 0; i < 3; i++) {
				traces[i * W + c] = W;
			}
		};
	}

	cv::Point *points = new cv::Point[W];
	int count = 0;
	for (unsigned int c = 0; c < W; c++) {
		if (traces[3 * W + c] < W) {
			int seam_index = traces[3 * W + c];
			points[count++] = cv::Point(seam_index, seam_energies[seam_index]);
		}
	}

	cv::Point *final_points = new cv::Point[W];
	memcpy(final_points, points, count * sizeof(cv::Point));

	std::sort(final_points,final_points + count, sortByY);
	num_found = cv::min(num_found, count);

	int* minimal_seams = new int[H*num_found];
	for (int i = 0; i < num_found; i++) {
		for (unsigned int r = 0; r < H; r++) {
			int seam_index = final_points[i].x;
			minimal_seams[r * num_found + i] = seams[r * W + seam_index];
		};
	}

	delete[] final_points;
	delete[] points;

	delete[] row;
	delete[] next_row;

	return minimal_seams;
}

void remove_pixels(Mat& image, int *seams, int count){
	int W = image.cols;
	int H = image.rows;

	Mat output(image.rows, image.cols - count, CV_8UC3);

	for (unsigned int r = 0; r < H; r++) {
		int *holes = new int[count];
		for (int k = 0; k < count; k++) {
			holes[k] = seams[r * count + k];
		}
		std::sort(holes, holes + count);
		int i = 0;
		int hole = holes[i];
		for (int c = 0; c < W - count; c++) {
			if (c + i == hole) {
				while (c + i == hole && i < count) {
					i++;
					hole = holes[i];
				}
				output.at<Vec3b>(r, c) = image.at<Vec3b>(r, c + i);
			} else {
				output.at<Vec3b>(r, c) = image.at<Vec3b>(r, c + i);
			}
		}
	}

	image = output;
}

void energy_function(Mat &image, Mat &output, int num_workers = 1){
	sobel(image, output, num_workers);
}

void coherence_function(Mat &image, int* seams, int count) {
	coherence(image, seams, count);
}

void remove_seams(Mat& image, char orientation = 'v', int num_workers = 1){

	if (orientation == 'h') {
		int flag = CW;
		rot90(image, flag);
	}

	Mat eimage;
	energy_function(image, eimage, num_workers);

	int num_found = 1000;
	int* minimal_seams = find_seams(eimage, num_found);

	std::cout << num_found << std::endl;

	for (int r = 0; r < image.rows; r++){
		for (int i = 0; i < num_found; i++) {
			image.at<Vec3b>(r, minimal_seams[r * num_found + i]) = Vec3b(255, 255, 255);
		}
	}

//	print_seams(minimal_seams, num_found, image.rows);

    remove_pixels(image, minimal_seams, num_found);

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
			remove_seams(image, 'v', num_workers);
		}
		else if (key == 'h') {
			remove_seams(image, 'h', num_workers);
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
//		remove_seams(image, 'v', num_workers);
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