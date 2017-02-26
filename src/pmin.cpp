#include "video.hpp"
#include <ff/farm.hpp>

using namespace cv;
using namespace std;
using namespace ff;

vector<Point> points;

int n = 11;
int step = 2;

long *FF(long *task,ff_node*const) {
    ulong left = (ulong) *task - 1;
    ulong right = min((ulong) n - 1, (ulong) *task - 1 + step - 1);

    int m = min(points.at(left).y, points.at(right).y);
    if (m == points.at(right).y) {
        points.at(left) = points.at(right);
        std::cout << task << points.at(left) << points.at(right) << endl;
    } else {
        points.at(right) = points.at(left);
        std::cout << task << points.at(right) << points.at(left) << endl;
    }

    for (std::vector<int>::size_type ii = 0; ii != points.size(); ii++) {
        std::cout << points[ii] << endl;
    }

    return task;
}
struct Sched: ff_node_t<long> {
    Sched(ff_loadbalancer *const lb) : lb(lb) {}

    int svc_init() {
        counter = 1;
        return 0;
    }

    long *svc(long *task) {
        int channel = lb->get_channel_id();
        if (task == nullptr) {
            for (long i = 1; i < n; i += step) {
                ff_send_out(new long(i));
            }
            return GO_ON;
        }

        std::cout << "Task " << *task << " coming from Worker" << channel << "\n";
        delete task;

        ++counter;
        std::cout << counter << "-----" << 1 + (n - 1) / step << std::endl;
        if (step > n) {
            return EOS;
        }
        if (counter == 1 + (n - 1) / step) {
            counter = 0;
            step = step << 1;
            std::cout << "step" << step << std::endl;
            for (long i = 1; i <= n; i += step)
               ff_send_out(new long(i));
        }
        return GO_ON;
    }

    ff_loadbalancer *lb;
    long counter;
};

void pmin(vector<Point> results) {
    int nw = 2;
    ff_Farm<long>  farm(FF, nw);
    farm.remove_collector(); // removes the default collector
    // the scheduler gets in input the internal load-balancer
    Sched S(farm.getlb());
    farm.add_emitter(S);
    // adds feedback channels between each worker and the scheduler
    farm.wrap_around();
    if (farm.run_and_wait_end()<0) error("running farm");
};

int main() {

    int s[11] = {8, 7, 1, 67, 6, 9, 4, 3, 88, 0, 7};
    for (int i = 0; i < n; i++) {
        Point *p = new Point(i, s[i]);
        points.push_back(*p);
    }

    for (std::vector<int>::size_type ii = 0; ii != points.size(); ii++) {
        std::cout << points[ii] << endl;
    }

    pmin(points);

    std::cout << points.at(0).x << points.at(0).y << std::endl;

    return 0;
}