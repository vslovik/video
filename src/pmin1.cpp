#include "video.hpp"
#include <ff/farm.hpp>

using namespace cv;
using namespace std;
using namespace ff;

struct PMinState {
    PMinState(const ulong n, ulong step, vector<Point> points): n(n), step(step), points(points) {};
    const ulong n;
    ulong step;
    vector<Point> points;
};

PMinState* st;

ulong n = 11;
int step = 2;
vector<Point> points;

long *FF(long *task,ff_node*const) {
    ulong left = (ulong) *task - 1;
    ulong right = min(n - 1, (ulong) *task - 1 + step - 1);

    if (points.at(left).y > points.at(right).y) {
        points.at(left) = points.at(right);
    } else {
        points.at(right) = points.at(left);
    }

    return task;
}

struct Scheduler: ff_node_t<long> {
    int svc_init() {
        counter = 1;
        return 0;
    }

    long *svc(long *task) {
        if (task == nullptr) {
            for (long i = 1; i < st->n; i += step) {
                ff_send_out(new long(i));
            }
            return GO_ON;
        }

        delete task;

        ++counter;
        if (counter == 1 + (st->n - 1)/step) {
            counter = 0;
            step = step << 1;
            for (long i = 1; i <= st->n; i += step)
                ff_send_out(new long(i));
        }

        if (step > st->n) {
            return EOS;
        }

        return GO_ON;
    }

    long counter;
};

void pmin() {
    int nw = 2;
    ff_Farm<long> farm(FF, nw);
    farm.remove_collector();
    Scheduler S;
    farm.add_emitter(S);
    farm.wrap_around();
    if (farm.run_and_wait_end()<0) error("running farm");
};

int main() {

    int s[11] = {8, 7, 1, 67, 6, 9, 4, 3, 88, 0, 7};
    for (int i = 0; i < n; i++) {
        Point *p = new Point(i, s[i]);
        points.push_back(*p);
    }

    st = new PMinState(11, 2, points);

    pmin();

    std::cout << points.at(0).x << points.at(0).y << std::endl;

    return 0;
}