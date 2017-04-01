#include <iostream>
#include <opencv2/core/core.hpp>
#include <ff/parallel_for.hpp>
#include <opencv2/highgui/highgui.hpp>

struct PMinState {
    PMinState(const ulong n, std::vector<cv::Point> points):
            n(n), points(points) {};
    const ulong n;
    ulong step = 2;
	std::vector<cv::Point> points;
};

template <unsigned N, typename T>
cv::Point pmin(T (&s)[N], int nw) { // instead of Point pmin(long* s, int nw)

	std::vector<cv::Point> points;
    for (int i = 0; i < N; i++) {
	    points.push_back(cv::Point(i, s[i]));
    }

	PMinState* st = new PMinState(N, points);
	ff::ParallelFor pf(nw, false);

    while (true) {
        if (st->step > st->n) {
            break;
        }

	    pf.parallel_for(1, st->n, st->step, [&st](int i) {
            ulong left = (ulong) i - 1;
            ulong right = cv::min(st->n - 1, (ulong) i - 1 + st->step - 1);

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

int main(int argc, char *argv[]) {
//    assert(argc>1);
//    int nworkers = atoi(argv[1]);
//    assert(nworkers>=2);

    int nworkers = 4;

    long s[] = {8, 7, 1, 67, 6, 9, 4, 3, 99, 8, 88, 2, 7};

    cv::Point p = pmin(s, nworkers);

    std::cout << "min: " << p.y << std::endl;

    return 0;
}