#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace cv;
using namespace std;

pthread_t *workers;
pthread_mutex_t mutexLock;

int ver = 1;
int hor = 0;
int frameIter= 0;
int numFrames = 0;
Mat *inFrames;
Mat *outFrames;
//vector<Mat> inFrames;
//vector<Mat> *outFrames;

void *reduce(void *);

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

/*
 * Entrance function of the worker thread.
 */
void *reduce(void *)
{
    int frameId;
    cout << "Frame " << frameIter << "/" << numFrames << endl;
    Mat frame1, frame2, frame3, frame4, frame5;
    while(1) {

        //pthread_mutex_lock(&mutexLock);
        if(frameIter > numFrames - 1) {
            //pthread_mutex_unlock(&mutexLock);
            cout << "Frame-- " << frameIter << "/" << numFrames << endl;
            break;
        }
        frameId = frameIter++;
        cout << "Frame++ " << frameIter << "/" << numFrames << endl;

        //pthread_mutex_unlock(&mutexLock);

//
//
//        frame1 = inFrames[frameId];
//
//        // check if we are close to the end of the video and
//        // select frames appropriately
//        if(frameId < numFrames - 4) {
//            frame2 = inFrames[frameId+1];
//            frame3 = inFrames[frameId+2];
//            frame4 = inFrames[frameId+3];
//            frame5 = inFrames[frameId+4];
//        }
//        else if(frameId < numFrames - 3) {
//            frame2 = inFrames[frameId+1];
//            frame3 = inFrames[frameId+2];
//            frame4 = inFrames[frameId+3];
//            frame5 = frame4;
//        }
//        else if(frameId < numFrames - 2) {
//            frame2 = inFrames[frameId+1];
//            frame3 = inFrames[frameId+2];
//            frame4 = frame3;
//            frame5 = frame3;
//        }
//        else if(frameId < numFrames - 1) {
//            frame2 = inFrames[frameId+1];
//            frame3 = frame2;
//            frame4 = frame2;
//            frame5 = frame2;
//        }
//        else {
//            frame2 = frame1;
//            frame3 = frame1;
//            frame4 = frame1;
//            frame5 = frame1;
//        }
        // outFrames[frameId] = reduce_frame(frame1, frame2, frame3, frame4, frame5, ver, hor);
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
    int numFrames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);

    cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << numFrames << endl;
    cout << "Input codec type: " << ".avi" << endl;

    inFrames = new Mat[numFrames];
    for(int i = 0; i < numFrames; ++i) {
        inputVideo >> inFrames[i];
    }
    inputVideo.release();

    //--------------------------------------------------------------------------------

    // Process video
    outFrames = new Mat[numFrames];
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
        outputVideo << inFrames[i];
//        imshow("mainWin", inFrames[i]);
//        waitKey(20);
    }

    outputVideo.release();
    cout << "Finished writing" << endl;

    return 0;
}