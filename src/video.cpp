#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <opencv2/imgproc/imgproc.hpp>

#include "carving.hpp"

using namespace cv;
using namespace std;

pthread_t *workers;
pthread_mutex_t mutexLock;

//vector<Mat> inFrames;
//vector<Mat> *outFrames;

void *reduce(void *);

struct State {
    static const int ver = 1;
    static const int hor = 0;
    int frameIter, numFrames;
    Mat *inFrames, *outFrames;
};

State *process;

static void help()
{
    cout
            << "------------------------------------------------------------------------------" << endl
            << "This program reads filters and write video files."                              << endl
            << "Usage:"                                                                         << endl
            << "./video inputvideoName"                                                         << endl
            << "------------------------------------------------------------------------------" << endl
            << endl;
}

int get_frame_id(){

    pthread_mutex_lock(&mutexLock);

    if(process->frameIter > process->numFrames - 1) {
        pthread_mutex_unlock(&mutexLock);
        return 0;
    }

    cout << "Frame++ " << process->frameIter + 1 << "/" << process->numFrames << endl;

    pthread_mutex_unlock(&mutexLock);

    return process->frameIter++;
}

/*
 * Entrance function of the worker thread.
 */
void *reduce(void *)
{
    int id;
    Mat img;

    vector<Mat> bunch;

    while(1) {

        id = get_frame_id();
        if(0 == id) {
            break;
        }

        for(int i = 0; i < 5; i++) {
            if(id + i < process->numFrames) {
                bunch.push_back(process->inFrames[id + i]);
            } else {
                bunch.push_back(process->inFrames[process->numFrames - 1]);
            }
        }

        cvtColor(bunch.at(0), img, CV_RGB2GRAY);
        process->outFrames[id] = reduce_frame(bunch, process->ver, process->hor);
    }
}

int main(int argc, char **argv)
{
    help();

    if(argc < 2) {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    const string source = argv[1];

    // Read video
    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        cout  << "Could not open the input video: " << source << endl;
        return -1;
    }

    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    int fps = inputVideo.get(CV_CAP_PROP_FPS);
    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);

    cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << numFrames << endl;
    cout << "Input codec type: " << ".avi" << endl;

    process = new State();
    process->frameIter= 0;
    process->numFrames = numFrames;

    process->inFrames = new Mat[numFrames];
    for(int i = 0; i < numFrames; ++i) {
        inputVideo >> process->inFrames[i];
    }
    inputVideo.release();

    //--------------------------------------------------------------------------------

    // Process video

    process->outFrames = new Mat[numFrames];
    int numWorkers = 4;
    cout << "Using " << numWorkers << " workers." << endl;

    workers = (pthread_t *)malloc(sizeof(pthread_t)*numWorkers);
    for(int i = 0; i < numWorkers; ++i) {
        cout << i << endl;
        if(pthread_create(&workers[i], NULL, reduce, NULL)) {
            cout << "Error creating thread\n" << endl;
            return -1;
        }
    }

    for(int i = 0; i < numWorkers; ++i) {
        if(pthread_join(workers[i], NULL)) {
            cout << "Error joining thread\n" << endl;
            return -1;
        }
    }

    //----------------------------------------------------------------------------------

    // Write video
    VideoWriter outputVideo;
    outputVideo.open("out.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, S, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << source << endl;
        return -1;
    }

    for(int i = 0; i < numFrames; ++i){
        outputVideo << process->inFrames[i];
//        imshow("mainWin", process->inFrames[i]);
//        waitKey(20);
    }

    outputVideo.release();
    cout << "Finished writing" << endl;

    return 0;
}