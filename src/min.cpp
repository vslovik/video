#include "video.hpp"
#include <ff/parallel_for.hpp>

using namespace cv;
using namespace std;
using namespace ff;


struct Stage0 : ff_minode_t<long> {
    Stage0(vector<Point> points) : points(points) {}

    int svc_init() {
        counter = 0;
        step = 2;
        n = sizeof(points) / sizeof(int);
        return 0;
    }

    long *svc(long *task) {
        std::cout << n << endl;
        if (task == nullptr) {
            for (long i = 1; i < n; i += step) {
                ff_send_out((void *) i);
            }
            return GO_ON;
        }

        printf("Stage0 has got task %ld\n", (ulong) task);

        ulong left = (ulong) task - 1;
        ulong right = min((ulong) n - 1, (ulong) task - 1 + step - 1);
        int m = min(points.at(left).y, points.at(right).y);
        if (m == points.at(right).y) {
            points.at(left) = points.at(right);
            std::cout << task << points.at(left) << points.at(right) << endl;
        } else {
            points.at(right) = points.at(left);
            std::cout << task << points.at(right) << points.at(left) << endl;
        }

        std::cout << "CCCC" << std::endl;
        for (std::vector<int>::size_type ii = 0; ii != points.size(); ii++) {
            std::cout << points[ii] << endl;
        }

        ++counter;
        std::cout << counter << std::endl;
        if (step > n) {
            std::cout << points.at(0).x << points.at(0).y << std::endl;
            return EOS;
        }

        if (counter == 1 + (n - 1) / step) {
            counter = 0;
            step = step << 1;
            std::cout << step << std::endl;
            for (long i = 1; i <= n; i += step)
                ff_send_out((void *) i);
        }
        return GO_ON;
    }

    long counter;
    int n;
    int step;
    vector<Point> points;
};

struct Stage1 : ff_monode_t<long> {
    long *svc(long *task) {
        //if ((long) task & 0x1) // sends odd tasks back
        ff_send_out_to(task, 0);
        //else ff_send_out_to(task, 1);
        return GO_ON;
    }
};

Point min(vector<Point> results) {
    std::cout << "BBBB" << std::endl;
    for (std::vector<int>::size_type ii = 0; ii != results.size(); ii++) {
        std::cout << results[ii] << endl;
    }

    Stage0 s0(results);
    Stage1 s1;

    ff_Pipe<long> pipe1(s0, s1);
    pipe1.wrap_around();

    if (pipe1.run_and_wait_end() < 0) error("running pipe");

    std::cout << s0.points.at(0).x << s0.points.at(0).y << std::endl;

    return s0.points.at(0);
};

//int main(int argc, char **argv)
//{
//    int s[11] = {8, 7, 1, 67, 6, 9, 4, 3, 88, 0, 7};
//    vector<Point> points;
//    int N = sizeof(s) / sizeof(int);
//    for (int i = 0; i < N; i++) {
//        Point *p = new Point(i, s[i]);
//        points.push_back(*p);
//    }
//
//    std::cout << "AAAA" << std::endl;
//    for (std::vector<int>::size_type ii = 0; ii != points.size(); ii++) {
//        std::cout << points[ii] << endl;
//    }
//    Point p = min(points);
//
//    std::cout << p.x << p.y << std::endl;
//
//    return 0;
//}