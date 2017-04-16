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

void find_seams(Mat &image, int* seams, int* traces, int num_workers = 1){
	int H = image.rows;
	int W = image.cols;

	uchar *row = new uchar[W];
	uchar *next_row = new uchar[W];
	int *seam_spans = new int[W];
	int *seam_energies = new int[W];

	cv::Point* points = new cv::Point[W];

	for(unsigned int i = 0; i < 4*W; i++){
		traces[i] = W;
	}

	int count = 0;
	//print_traces(traces, W);

	std::cout << H << std::endl;

	ff::ParallelFor pf(num_workers, false);
	for(int r = 0; r < H; r++){

		// Calculate row values
		pf.parallel_for(0L, W, [&next_row, row, r, W, image](int c) {
			uchar next = image.at<uchar>(r,c);
			if(r > 0) {
				uchar left = c > 0 ? row[c - 1] : max_uchar;
				uchar right = c < W - 1 ? row[c + 1] : max_uchar;
				next += std::min({left, row[c], right});
			}
			next_row[c] = next;
		});

		row = next_row;

		// Advance seams
		pf.parallel_for(0L,W,[row, r, W, H, &seams, &points, &traces, &seam_energies, &seam_spans](int c) {
			if(r > 0) {
				if(traces[3 * W + c] < W) {
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
						seam_spans[seam_index] = cv::max(seam_spans[seam_index],
						                                 abs(seams[seam_index] - seams[r * W + seam_index]));
						seam_energies[seam_index] = seam_energies[seam_index] + m;

						points[seam_index] = cv::Point(seam_index, seam_energies[seam_index]);
//						if (r == H - 3)
//							std::cout << r << "---" << seam_index << "+++++" << seam_energies[seam_index] << "===" << (int) m << std::endl;
//						if (r == H - 2)
//							std::cout << r << "---" << seam_index << "+++++" << seam_energies[seam_index] << "===" << (int) m << std::endl;
//						if (r == H - 1) {
//							std::cout << r << "---" << seam_index <<  "+++++" << seam_energies[seam_index] << "===" << (int) m << std::endl;
//						}
					}
				}
			} else {
				seams[r * W + c] = c;
				traces[W + c] = c;
				seam_spans[c] = 0;
				seam_energies[c] = 0;
			}
		});

		for(unsigned int c = 0; c < W; c++){
			traces[3 * W + c] = W;
		}

		// Resolve seams conflicts
		pf.parallel_for(0L, W, [row, r, W, H, &seams, &traces, &seam_energies, &seam_spans](int c) {
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
				if (min_energy > span) {
					min_energy = energy;
					best_seam_index = seam_index;
				} else if (min_span > span) {
					min_span = span;
					best_seam_index = seam_index;
				}
			}
			for (int i = 0; i < 3; i++) {
				if (traces[i * W + c] == W) {
					continue;
				}
				seam_index = traces[i * W + c];
				if(seam_index != best_seam_index) {
					seams[r*W + seam_index] = W;
				}
			}
			traces[3 * W + c] = best_seam_index;
		});

//		// seam splitting: do not trache but deviate some seams
//		// 1. first: move trashed
		if(r > 950) {//ToDo treshold
			for(int c = 0; c < W; c++) {
				if (traces[c] != W) {
					int seam_index = traces[c];
					if (c > 0 && traces[3 * W + c - 1] == W && traces[3 * W + c - 2] != W) {
						traces[W + c - 1] = seam_index;
					} else if (c > 1 && traces[3 * W + c - 1] != W && traces[3 * W + c - 2] == W) {
						traces[2 * W + c - 2] = seam_index;
					} else if (c > 1 && traces[3 * W + c - 1] == W && traces[3 * W + c - 2] == W) {
						if (row[c - 1] > row[c - 2]) {
							seams[r * W + seam_index] = c - 2;
							traces[2 * W + c - 2] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c - 2];
						} else {
							seams[r * W + seam_index] = c - 1;
							traces[W + c - 1] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c - 1];
						}
						seam_spans[seam_index] = cv::max(seam_spans[seam_index],
						                                 abs(seams[seam_index] - seams[r * W + seam_index]));
					}
				}
			}
			for(int c = 0; c < W; c++) {
				if (traces[W + c] != W) {
					int seam_index = traces[c];
					if (c > 0 && traces[3 * W + c - 1] == W && traces[3 * W + c + 1] != W) {
						traces[2 * W + c - 1] = seam_index;
					} else if (c > 1 && traces[3 * W + c - 1] != W && traces[3 * W + c + 1] == W) {
						traces[c + 1] = seam_index;
					} else if (c > 1 && traces[3 * W + c - 1] == W && traces[3 * W + c + 1] == W) {
						if (row[c - 1] > row[c + 1]) {
							seams[r * W + seam_index] = c + 1;
							traces[c + 1] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c + 1];
						} else {
							seams[r * W + seam_index] = c - 1;
							traces[2 * W + c - 1] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c - 1];
						}
						seam_spans[seam_index] = cv::max(seam_spans[seam_index],
						                                 abs(seams[seam_index] - seams[r * W + seam_index]));
					}
				}
			}
			for(int c = 0; c < W; c++) {
				if (traces[2*W + c] != W) {
					int seam_index = traces[c];
					if(c > 0 && traces[3*W + c + 1] == W && traces[3*W + c + 2] != W) {
						traces[W + c + 1] = seam_index;
					} else if(c > 1 && traces[3*W + c + 1] != W && traces[3*W + c + 2] == W) {
						traces[c + 2] = seam_index;
					} else if(c > 1 && traces[3*W + c + 1] == W && traces[3*W + c + 2] == W) {
						if(row[c + 1] > row[c + 2]) {
							seams[r * W + seam_index] = c + 2;
							traces[c + 2] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c + 2];
						} else {
							seams[r * W + seam_index] = c + 1;
							traces[W + c + 1] = seam_index;
							seam_energies[seam_index] = seam_energies[seam_index] + row[c + 1];
						}
						seam_spans[seam_index] = cv::max(seam_spans[seam_index],
						                                 abs(seams[seam_index] - seams[r * W + seam_index]));
					}
				}
			}

			// Resolve seams conflicts
			pf.parallel_for(0L, W, [row, r, W, H, &seams, &traces, &seam_energies, &seam_spans](int c) {
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
					if (min_energy > span) {
						min_energy = energy;
						best_seam_index = seam_index;
					} else if (min_span > span) {
						min_span = span;
						best_seam_index = seam_index;
					}
				}
				for (int i = 0; i < 3; i++) {
					if (traces[i * W + c] == W) {
						continue;
					}
					seam_index = traces[i * W + c];
					if(seam_index != best_seam_index) {
						seams[r*W + seam_index] = W;
					}
				}
				traces[3 * W + c] = best_seam_index;
			});
		}

		//print_traces(traces, W);
		// clean traces rows
		count = 0;
		for(unsigned int c = 0; c < W; c++){
			for (int i = 0; i < 4; i++) {
				if (traces[i * W + c] == W) {
					continue;
				}
				if(i == 3)
					count++;
				if(i < 3)
					traces[i * W + c] = W;
			}
		}

		std::cout << r << "===" << count << "===" << W << std::endl;
	}

	delete[] row;

	Point p = points[0];

	cv::Point *final_points = new cv::Point[count];
	int i = 0;
	for (unsigned int c = 0; c < W; c++) {

		if (traces[3 * W + c] == W) {
			continue;
		}

		int seam_index = traces[3 * W + c];
		final_points[i] = points[seam_index];
		i++;
	}

//	for (unsigned int j = 0; j < count; j++) {
//		std::cout << final_points[j].x << "===" << final_points[j].y << std::endl;
//	}

	std::sort(final_points,final_points + count,sortByY); //ToDo: in parallel if more than 30 elements

	for (unsigned int j = 0; j < count; j++) {
		std::cout << final_points[j].x << "---" << final_points[j].y << std::endl;
	}

	seams = new int[count];

	delete[] points;

//	delete[] seams;
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


	int *seams = new int[eimage.cols * eimage.rows];

	int *traces = new int[4*eimage.cols];
	find_seams(eimage, seams, traces, num_workers);

	int W = image.rows;

	for (int c = 0; c < image.rows; c++) {
		if (traces[3 * W + c] == W) {
			continue;
		}
		int seam_index = traces[3 * W + c];
		for (int r = 0; r < image.rows; r++) {
			image.at<Vec3b>(r, seams[r * W + seam_index]) = Vec3b(255, 255, 255);
		}
	}

	//remove_pixels(image, seams, traces, num_workers);

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