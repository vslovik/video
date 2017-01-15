#include "carving.hpp"

using namespace cv;

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
    Mat img;
    Mat *reduced = new Mat[5];

    for(int i = 0; i < 5; i++) {
        cvtColor(bunch[i], img, CV_RGB2GRAY);
        reduced[i] = img;
    }

    cvtColor(bunch[0], img, CV_RGB2GRAY);

    int diff = h > v ? h - v : v - h;
    int min = h > v ? v : h;

    img.t();

    for(int i = 0; i < min; ++i) {
        img = reduce_vertical(reduced, img);
        img = reduce_horizontal(reduced, img.t());
    }

    for(int i = 0; i < diff; ++i) {
        img = reduce_horizontal(reduced, h > v ? img.t() : img);
    }

    return img;
}

/*
 * Remove one vertical seam from image
 *
 * reduced: images used to calculate the seam
 * img: image needed to perform seam-cut
 */
Mat reduce_vertical(Mat *reduced, Mat img) {
    int rows = img.rows;
    int *seam = new int[rows];
//    seam = find_seam(reduced);
//    Mat returnImg = remove_seam(img, seam);
//    for(int i = 0; i < diff; ++i) {
//        reduced[0] = remove_seam_gray(grayImg1, seam);
//    }
//
//    return returnImg;
    return img;
}


/*
 * Remove one horizontal seam from image
 *
 * reduced: images used to calculate the seam
 * img: image needed to perform seam-cut
 */
Mat reduce_horizontal(Mat *reduced, Mat img) {
    int rows = img.rows;
    int *seam = new int[rows];

//    std::for_each(reduced.begin(), reduced.end(), [](int &img){ img.t(); });
//
//    seam = find_seam(reduced);
//    Mat returnImg = remove_seam(img, seam);
//
//    std::for_each(reduced.begin(), reduced.end(), [](int &img){ img.t(); });
//
//    return returnImg.t();

    return img;
}