#include <iostream>
#include <opencv2/core/core.hpp>
#include <ff/parallel_for.hpp>
#include "psobel.h"

struct PMinState {
    PMinState(const int n, std::vector<Point> points):
            n(n), points(points) {};
    const int n;
    int step = 2;
    std::vector<Point> points;
};

int max_int = std::numeric_limits<int>::max();

PMinState* st;

int *FF(int *task,ff::ff_node*const) {
    ulong left = (ulong) *task - 1;
    ulong right = (ulong) min(st->n - 1, *task - 1 + st->step - 1);

    if (st->points.at(left).y > st->points.at(right).y) {
        st->points.at(left) = st->points.at(right);
    } else {
        st->points.at(right) = st->points.at(left);
    }

    return task;
}

struct Scheduler: ff::ff_node_t<int> {
    int svc_init() {
        counter = 1;
        return 0;
    }

    int *svc(int *task) {
        if (task == nullptr) {
            for (int i = 1; i < st->n; i += st->step) {
                ff_send_out(new int(i));
            }
            return GO_ON;
        }

        delete task;

        ++counter;
        if (counter == 1 + (st->n - 1)/st->step) {
            counter = 0;
            st->step = st->step << 1;
            for (int i = 1; i <= st->n; i += st->step)
                ff_send_out(new int(i));
        }

        if (st->step > st->n) {
            return EOS;
        }

        return GO_ON;
    }

    int counter;
};

Point parallel_min(int* s, const int N, int nw){

    std::vector<Point> points;
    for (int i = 0; i < N; i++) {
        Point *p = new Point(i, s[i]);
        points.push_back(*p);
    }

    st = new PMinState(N, points);

    ff::ff_Farm<int> farm(FF, nw);
    farm.remove_collector();
    Scheduler S;
    farm.add_emitter(S);
    farm.wrap_around();
    if (farm.run_and_wait_end()<0) ff::error("running farm");

    return st->points.at(0);
};

int *find_seam(Mat &image, int num_workers = 1){
    int H = image.rows;
    int W = image.cols;
    int *seams;
    int *scores;
    int* row = new int[W];

    seams = (int *)malloc(W*H*sizeof(int));
    scores = (int *)malloc(W*sizeof(int));

    ff::ParallelFor pf(num_workers, false);
    for(int r = 0; r < H; r++){

        // Calculate row values
        pf.parallel_for(0L, W, [&row, r, W, &image](int c) {
            int next = (int)image.at<uchar>(r,c);
            if(r > 0) {
                int left = c > 0 ? row[c - 1] : max_int;
                int right = c < W - 1 ? row[c + 1] : max_int;
                next += min({left, row[c], right});
            }
            row[c] = next;
        });

        // Advance seams
        pf.parallel_for(0L,W,[&row, r, W, H, &seams, &scores](int c) {
            if(r > 0) {
                int left = c > 0 ? row[c - 1] : max_int;
                int right = c < W - 1 ? row[c + 1] : max_int;
                int middle = row[c];
                int m = min({left, middle, right});
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

void energy_function(Mat &image, Mat &output){

    int num_workers = 6;
    sobel(image, output, num_workers);
}


void coherence_function(Mat &image, int* seam) {
    int num_workers = 6;
    coherence(image, seam, num_workers);
}