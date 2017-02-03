#include "carving.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

/*
 * Seam-cut to the frame1 according to the next 4 frames and itself.
 *
 * bunch: bunch of frames
 * 1st frame: the frame to be seam-cut
 * 2nd, 3rd, 4th, 5th frames: used to calculate the look ahead energy
 * v: number of vertical seams to be removed
 * h: number of horizontal seams to be removed
 */
Mat reduce_frame(Mat *bunch, int v, int h) {
    Mat img, tmp;
    img = bunch[0];

    Mat *history = new Mat[5];

    for(int i = 0; i < 5; i++) {
        cvtColor(bunch[i], tmp, CV_RGB2GRAY);
        history[i] = tmp;
    }

    int diff = h > v ? h - v : v - h;
    int min = h > v ? v : h;

    for(int i = 0; i < min; ++i) {
        img = reduce_vertical(history, img);
        img = reduce_horizontal(history, img.t());
    }

    for(int i = 0; i < diff; ++i) {
        img = h > v ? reduce_horizontal(history, img.t()) : reduce_vertical(history, img);
    }

    return img;
}

/*
 * Remove one vertical seam from image
 *
 * history: images used to calculate the seam
 * img: image needed to perform seam-cut
 */
Mat reduce_vertical(Mat *history, Mat img) {
    Mat reduced;
    int *seam;
    seam = find_seam(history);

    reduced = remove_seam(img, seam, CV_8UC3);
    for(int i = 0; i < 5; ++i) {
        history[i] = remove_seam(history[i], seam, CV_8UC1);
    }

    return reduced;
}

/*
 * Remove one horizontal seam from image
 *
 * history: images used to calculate the seam
 * img: image needed to perform seam-cut
 */
Mat reduce_horizontal(Mat *history, Mat img) {
    Mat reduced;
    int *seam;

    for(int i = 0; i < 5; ++i) {
        history[i] = history[i].t();
    }

    seam = find_seam(history);
    reduced = remove_seam(img, seam, CV_8UC3);
    for(int i = 0; i < 5; ++i) {
        history[i] = remove_seam(history[i], seam, CV_8UC1);
    }

    for(int i = 0; i < 5; ++i) {
        history[i] = history[i].t();
    }

    return reduced.t();
}

/*
 * Find the seam of minimum energy according to our improved graph cut algorithm
 *
 * history.at(0): current image needed to be seam-removed
 * history: used to calculate the look ahead energy
 */
int *find_seam(Mat *history)
{
    int rows = history[0].rows;
    int cols = history[0].cols;
    int *seam = new int[rows];

    Graph<int,int,int> *g = build_graph(history);

    for(int i=0; i<rows; i++) {
        for(int j=0;j<cols; j++) {
            if(g->what_segment(i*cols+j) == Graph<int,int,int>::SINK) {
                seam[i] = j-1;
                break;
            }
            if(j==cols-1 && g->what_segment(i*cols+j) == Graph<int,int,int>::SOURCE) {
                seam[i] = cols-1;
            }
        }
    }

    delete g;
    return seam;
}

Graph<int,int,int> *build_graph(Mat *history)
{
    int inf = 100000;
    float a[] = {0.2, 0.2, 0.2, 0.2, 0.2};

    int rows = history[0].rows;
    int cols = history[0].cols;

    int num_nodes = rows*cols;
    int num_edges = (rows - 1)*cols + (cols - 1)*rows + 2*(rows - 1)*(cols - 1);

    Graph<int,int,int> *g = new Graph<int,int,int>(num_nodes, num_edges);

    for (int i = 1; i <= num_nodes; i++) {
        g->add_node();
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(j == 0) {
                g->add_tweights(i*cols, inf, 0);
            }
            else if(j == cols - 1 && i + 1 <= rows) {
                g->add_tweights((i + 1)*cols - 1, 0, inf );
            }

            if(j == 0) {
                int lr = 0;
                for(int k = 0; k < 5; k++) {
                    lr += a[k]*(history[k].at<char>(i, j + 1));
                }
                g->add_edge(i*cols, i*cols + 1, lr, inf );
            }
            else if(j != cols - 1) {
                int lr = 0;
                for(int k = 0; k < 5; k++) {
                    lr += a[k]*abs(history[k].at<char>(i, j + 1) - history[k].at<char>(i, j - 1));
                }
                g->add_edge(i*cols + j, i*cols + j + 1, lr, inf);
            }

            if(i != rows - 1) {
                if(j == 0) {
                    // positive LU
                    int posLU = 0;
                    for(int k = 0; k < 5; k++) {
                        posLU += a[k]*(history[k].at<char>(i,j));
                    }

                    // negative LU
                    int negLU = 0;
                    for(int k = 0; k < 5; k++) {
                        negLU += a[k]*(history[k].at<char>(i+1,j));
                    }
                    g->add_edge(i*cols + j, i*cols + j + 1, negLU, posLU);
                }
                else {
                    // positive LU
                    int posLU = 0;
                    for(int k = 0; k < 5; k++) {
                        posLU += a[k]*abs(history[k].at<char>(i,j) - history[k].at<char>(i + 1,j - 1));
                    }
                    // negative LU
                    int negLU = 0;
                    for(int k=0; k < 5; k++) {
                        negLU += a[k]*abs(history[k].at<char>(i+1,j) - history[k].at<char>(i, j - 1));
                    }
                    g->add_edge( i*cols + j, i*cols + j + 1, negLU, posLU );
                }
            }
            if(i != 0 && j != 0)
            {
                g->add_edge(i*cols + j, (i - 1)*cols + j - 1, inf, 0);
            }
            if(i != rows - 1 && j != 0)
            {
                g->add_edge(i*cols + j, (i + 1)*cols + j - 1, inf, 0);
            }
        }
    }

    g->maxflow();

    return g;
}

/*
 * Remove seam from the gray image.
 *
 * img: gray image to be seam-removed
 * seam: column index of each row
 */
Mat remove_seam(Mat img, int *seam, int type)
{
    Mat reduced(img.rows, img.cols - 1, type);
    for(int i = 0; i < img.rows; i++){

        if(seam[i] != 0) {
            img.row(i).colRange(Range(0, seam[i])).copyTo(reduced.row(i).colRange(Range(0, seam[i])));
        }

        if(seam[i] != img.cols - 1) {
            img.row(i).colRange(Range(seam[i] + 1, img.cols)).copyTo(reduced.row(i).colRange(Range(seam[i], img.cols - 1)));
        }
    }

    return reduced;
}