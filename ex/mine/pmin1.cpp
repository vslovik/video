#include <ff/farm.hpp>

using namespace cv;
using namespace std;
using namespace ff;

struct PMinState {
    PMinState(const ulong n, vector<Point> points):
            n(n), points(points) {};
    const ulong n;
    ulong step = 2;
    vector<Point> points;
};

PMinState* st;

long *FF(long *task,ff_node*const) {
    ulong left = (ulong) *task - 1;
    ulong right = min(st->n - 1, (ulong) *task - 1 + st->step - 1);

    if (st->points.at(left).y > st->points.at(right).y) {
        st->points.at(left) = st->points.at(right);
    } else {
        st->points.at(right) = st->points.at(left);
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
            for (long i = 1; i < st->n; i += st->step) {
                ff_send_out(new long(i));
            }
            return GO_ON;
        }

        delete task;

        ++counter;
        if (counter == 1 + (st->n - 1)/st->step) {
            counter = 0;
            st->step = st->step << 1;
            for (long i = 1; i <= st->n; i += st->step)
                ff_send_out(new long(i));
        }

        if (st->step > st->n) {
            return EOS;
        }

        return GO_ON;
    }

    long counter;
};

template <unsigned N, typename T>
Point pmin(T (&s)[N], int nw) { // instead of Point pmin(long* s, int nw)

    vector<Point> points;
    for (int i = 0; i < N; i++) {
        Point *p = new Point(i, s[i]);
        points.push_back(*p);
    }

    st = new PMinState(N, points);

    ff_Farm<long> farm(FF, nw);
    farm.remove_collector();
    Scheduler S;
    farm.add_emitter(S);
    farm.wrap_around();
    if (farm.run_and_wait_end()<0) error("running farm");

    return st->points.at(0);
};

int main(int argc, char *argv[]) {
//    assert(argc>1);
//    int nworkers = atoi(argv[1]);
//    assert(nworkers>=2);

    int nworkers = 4;

    long s[] = {8, 7, 1, 67, 6, 9, 4, 3, 99, 8, 88, 2, 7};

    Point p = pmin(s, nworkers);

    std::cout << "min: " << p.y << std::endl;

    return 0;
}