#include "video.hpp"
#include <ff/parallel_for.hpp>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using namespace ff;

const long N=10;
const long streamlength=20;

State *process;



void energy_function(Mat &image, Mat &output){
    Mat dx, dy;
    Sobel(image, dx, CV_64F, 1, 0);
    Sobel(image, dy, CV_64F, 0, 1);
    magnitude(dx,dy, output);

    double min_value, max_value, Z;
    minMaxLoc(output, &min_value, &max_value);
    Z = 1/max_value * 255;
    output = output * Z;                    //normalize
    output.convertTo(output, CV_8U);
}

int *find_seam(Mat &image){
    int H = image.rows;
    int W = image.cols;
    int *seams;
    int *scores;
    int *buffer;
    int* prev = new int[W];
    long nworkers = 1;
    int* row = new int[W];

    seams = (int *)malloc(W*H*sizeof(int));
    scores = (int *)malloc(W*sizeof(int));



    // Calculate row values
    ff::ParallelFor pf(nworkers, false);
//    for(int r = 0; r < H; r++){
//
//
//        pf.parallel_for(0L,W,[row, r, W, prev, &image](int c) {
//            row[c] = (int)image.at<uchar>(r,c);
//            if(r > 0) {
//                int left = c > 0 ? prev[c - 1] : 0;
//                int right = c < W - 1 ? prev[c + 1] : 0;
//                row[c] += min({left, prev[c], right});
//            }
//        });
//
//        // Advance seams
//        pf.parallel_for(0L,W,[&row, r, W, H, &seams, &scores](int c) {
//            if(r > 0) {
//                int left = c > 0 ? row[c - 1] : 0;
//                int right = c < W - 1 ? row[c + 1] : 0;
//                int middle = row[c];
//                int m = min({left, middle, right});
//                scores[c] = m;
//                if(m == left)
//                    seams[r * H + c] = c - 1;
//                else if(m == right)
//                    seams[r * H + c] = c + 1;
//                else
//                    seams[r*H + c] = c;
//            } else
//                seams[r*H + c] = row[c];
//        });
//
//        prev = row;
//    }



//    int s[8] = {8, 7, 1, 67, 6, 9, 4, 3};
//    int N = sizeof(s)/sizeof(int);
//    vector<Point> b;
//    for(int i = 0; i < N; i++){
//        Point* p = new Point(i, s[i]);
//        b.push_back(*p);
//    }
//
//
//    int step = 2;
//    while(1) {
//        ff::ParallelFor pfi(nworkers, false);
//        pfi.parallel_for(0L, N, [N, &b, &step](int c) {
//            if (c % step == 0) {
//                std::cout << c << "-" << step << endl;
//                if (c + step - 1 < N ) {
//                    int m = min(b[c].y, b[c + step - 1].y);
//                    if(m == b[c + step - 1].y) {
//                        b[c] = b[c + step - 1];
//                    } else {
//                        b[c + step - 1] = b[c];
//                    }
//                    for(int j=0; j < N; j++) {
//                        std::cout << b[j].x << b[j].y << std::endl;
//                    }
//                }
//            }
//        });
//
//        std::cout << b[0].x << b[0].y << std::endl;
//
//        step = step << 1;
//        if(step > N)
//            break;
//    }
//
//    std::cout << b[0].x << b[0].y << std::endl;
//
//    step = step >> 1;
//    std::cout << "step" << step << endl;
//    int res = (step < N && b[0].y > b[step].y) ? b[step].x : b[0].x;
//    std::cout << res << endl;

    //==================================================================================================================

//    int s[8] = {8, 7, 1, 67, 6, 9, 4, 3};
//    int N = sizeof(s)/sizeof(int);
//
//    vector<Point> from;
//
//    for(int i = 0; i < N; i++){
//        Point* p = new Point(i, s[i]);
//        from.push_back(*p);
//    }
//    for (int j = 0; j < N; j++) {
//        std::cout << "++++" << from[j].x << "-" <<  from[j].y << std::endl;
//    }
//
//    vector<Point> to;
//    while(N) {
//        ff::ParallelFor pfi(nworkers, false);
//        pfi.parallel_for(0L, N, [N, &from, &to](int c) {
//            if (c % 2 == 0) {
//                if (c + 1 < N) {
//                    from[c + 1].y <= from[c].y ? to.push_back(from[c + 1]) : to.push_back(from[c]);
//                }
//            }
//        });
//        N = N >> 1;
//        from = to;
//        to.clear();
//    }
//
//    std::cout << to[0].x << to[0].y << endl;


    int s[8] = {8, 7, 1, 67, 6, 9, 4, 3};
    int N = sizeof(s)/sizeof(int);
    vector<Point> results;

    for(int i = 0; i < N; i++){
        Point* p = new Point(i, s[i]);
        results.push_back(*p);
    }

    using namespace ff;

    struct Stage0 : ff_minode_t<long> {
        Stage0(vector<Point> points, int n): points(points), n(n) {}

        int svc_init() {
            counter = 0;
            return 0;
        }

        long *svc(long *task) {
            std::cout << n << endl;
            if (task == nullptr) {
                for (long i = 1; i <= n; ++i)
                    ff_send_out((void *) i);
                return GO_ON;
            }
            printf("Stage0 has got task %ld\n", (long) task);
            if ((long) task + step - 1 < n) {
                std::cout << points.at(0).x << endl;
//                int m = min(points.at((ulong)*task).y, points.at((ulong)*task + step - 1).y);
//                if (m == points.at((ulong)*task + step - 1).y) {
//                    points.at((ulong)*task) = points.at((ulong)*task + step - 1);
//                } else {
//                    points.at((ulong)*task + step - 1) = points.at((ulong)*task);
//                }
            }
            ++counter;
            if(n < 2)
                return EOS;
            if (counter == n) {
                counter = 0;
                n = n >> 1;
                for (long i = 1; i <= n; ++i)
                    ff_send_out((void *) i);
            }
            return GO_ON;
        }

        long counter;
        int n;
        int step = 2;
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

    Stage0 s0(results, N);
    Stage1 s1;

    ff_Pipe<long> pipe1(s0, s1);
    pipe1.wrap_around();

    if (pipe1.run_and_wait_end() < 0) error("running pipe");

    std::cout << results.at(0).x << results.at(0).y << std::endl;

    int dp[H][W];
    for(int c = 0; c < W; c++){
        dp[0][c] = (int)image.at<uchar>(0,c);
    }

    for(int r = 1; r < H; r++){
        for(int c = 0; c < W; c++){
            if (c == 0)
                dp[r][c] = min(dp[r-1][c+1], dp[r-1][c]);
            else if (c == W-1)
                dp[r][c] = min(dp[r-1][c-1], dp[r-1][c]);
            else
                dp[r][c] = min({dp[r-1][c-1], dp[r-1][c], dp[r-1][c+1]});
            dp[r][c] += (int)image.at<uchar>(r,c);
        }
    }

    int min_value = 2147483647; //infinity
    int min_index = -1;
    for(int c = 0; c < W; c++)
        if (dp[H-1][c] < min_value) {
            min_value = dp[H - 1][c];
            min_index = c;
        }

    int *path = new int[H];
    Point pos(H-1,min_index);
    path[pos.x] = pos.y;

    while (pos.x != 0){
        int value = dp[pos.x][pos.y] - (int)image.at<uchar>(pos.x,pos.y);
        int r = pos.x, c = pos.y;
        if (c == 0){
            if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        else if (c == W-1){
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else
                pos = Point(r-1,c);
        }
        else{
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        path[pos.x] = pos.y;
    }

    return path;
}

int *find_seam_(Mat &image){
    int H = image.rows, W = image.cols;

    int dp[H][W];
    for(int c = 0; c < W; c++){
        dp[0][c] = (int)image.at<uchar>(0,c);
    }

    for(int r = 1; r < H; r++){
        for(int c = 0; c < W; c++){
            if (c == 0)
                dp[r][c] = min(dp[r-1][c+1], dp[r-1][c]);
            else if (c == W-1)
                dp[r][c] = min(dp[r-1][c-1], dp[r-1][c]);
            else
                dp[r][c] = min({dp[r-1][c-1], dp[r-1][c], dp[r-1][c+1]});
            dp[r][c] += (int)image.at<uchar>(r,c);
        }
    }

    int min_value = 2147483647; //infinity
    int min_index = -1;
    for(int c = 0; c < W; c++)
        if (dp[H-1][c] < min_value) {
            min_value = dp[H - 1][c];
            min_index = c;
        }

    int *path = new int[H];
    Point pos(H-1,min_index);
    path[pos.x] = pos.y;

    while (pos.x != 0){
        int value = dp[pos.x][pos.y] - (int)image.at<uchar>(pos.x,pos.y);
        int r = pos.x, c = pos.y;
        if (c == 0){
            if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        else if (c == W-1){
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else
                pos = Point(r-1,c);
        }
        else{
            if (value == dp[r-1][c-1])
                pos = Point(r-1,c-1);
            else if (value == dp[r-1][c+1])
                pos = Point(r-1,c+1);
            else
                pos = Point(r-1,c);
        }
        path[pos.x] = pos.y;
    }

    return path;
}


void remove_pixels(Mat& image, Mat& output, int *seam){
    for(int r = 0; r < image.rows; r++ ) {
        for (int c = 0; c < image.cols; c++){
            if (c >= seam[r])
                output.at<Vec3b>(r,c) = image.at<Vec3b>(r,c+1);
            else
                output.at<Vec3b>(r,c) = image.at<Vec3b>(r,c);
        }
    }
}

void rot90(Mat &matImage, int rotflag){
    //1=CW, 2=CCW, 3=180
    if (rotflag == 1){
        transpose(matImage, matImage);
        flip(matImage, matImage,1); //transpose+flip(1)=CW
    } else if (rotflag == 2) {
        transpose(matImage, matImage);
        flip(matImage, matImage,0); //transpose+flip(0)=CCW
    } else if (rotflag ==3){
        flip(matImage, matImage,-1);    //flip(-1)=180
    } else if (rotflag != 0){ //if not 0,1,2,3:
        cout  << "Unknown rotation flag(" << rotflag << ")" << endl;
    }
}

void remove_seam(Mat& image, char orientation = 'v'){
    if (orientation == 'h')
        rot90(image,1);
    int H = image.rows, W = image.cols;

    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);

    Mat eimage;
    energy_function(gray, eimage);

    int* seam = find_seam(eimage);

    Mat output(H,W-1, CV_8UC3);
    remove_pixels(image, output, seam);
    if (orientation == 'h')
        rot90(output,2);
    image = output;
}

void realTime(Mat& image){
    cout << "UP ARROW: Shrink horizontally" << endl;
    cout << "LEFT ARROW: Shrink vertically" << endl;
    cout << "q: Quit" << endl;

    int key;
    while(1) {
        namedWindow("Display window", WINDOW_AUTOSIZE);
        imshow("Display window", image);
        key = waitKey(0);
        if (key == 'q')
            break;
        else if (key == 'v')
            remove_seam(image, 'v');
        else if (key == 'h')
            remove_seam(image, 'h');
    }
}

void shrink_image(Mat& image, int new_cols, int new_rows, int width, int height){
    cout << endl << "Processing image..." << endl;
    for(int i = 0; i < width - new_cols; i++){
        remove_seam(image, 'v');
    }
    for(int i = 0; i < height - new_rows; i++){
        remove_seam(image, 'h');
    }
}


void process_video(string source)
{
//    VideoCapture inputVideo(source);
//    if (!inputVideo.isOpened())
//    {
//        throw source;
//    }
//
//    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
//    Mat inFrame, outFrame;
//    process = new State(
//            inputVideo.get(CV_CAP_PROP_FPS),
//            numFrames,
//            Size(
//             (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
//             (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
//            ),
//            inFrame,
//            outFrame,
//            "out.avi"
//    );
//
//    cout << "Input frame resolution: Width=" << process->size.width << "  Height=" << process->size.height
//         << " of nr#: " << process->numFrames << endl;
//    cout << "Input codec type: " << ".avi" << endl;
//
//    VideoWriter outputVideo;
//    outputVideo.open(
//            process->output,
//            CV_FOURCC('D', 'I', 'V', 'X'),
//            process->fps,
//            Size(
//                    process->size.width - process->ver,
//                    process->size.height - process->hor
//            ),
//            true
//    );
//
//    if (!outputVideo.isOpened()) {
//        throw source;
//    }

//    for(int i = 0; i < process->numFrames; ++i) {
//        inputVideo >> process->inFrame;

        cout << "UP ARROW: Shrink horizontally" << endl;
        cout << "LEFT ARROW: Shrink vertically" << endl;
        cout << "q: Quit" << endl;

        Mat image;
        image = imread("data/monteverdi_ritratto.jpg", 1);
        realTime(image);


//        outputVideo << process->inFrame;
//        imshow("mainWin", process->inFrame);
//        waitKey(20);

//        break;
//    }

//    inputVideo.release();
//    outputVideo.release();
}