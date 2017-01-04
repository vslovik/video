#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    if(argc < 2) {
        cout << "./video [video.avi]" << endl;
        return 0;
    }

    // http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00071000000000000000
    // To compile: g++ -o video video.cpp `pkg-config opencv --cflags --libs`
    // Capturing a frame from a video sequence

    CvCapture* capture = cvCaptureFromAVI(argv[1]);

    IplImage* img0 = 0;
    if(!cvGrabFrame(capture)){ // capture a frame
        printf("Could not grab a frame\n\7");
        exit(0);
    }

    img0=cvRetrieveFrame(capture); // retrieve the captured frame


    // Getting/setting frame information

    cvQueryFrame(capture); // this call is necessary to get correct capture properties
    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    int numFrames = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);

    cout << "frameH: " << frameH << endl;
    cout << "frameW: " << frameW << endl;
    cout << "fps: " << fps << endl;
    cout << "numFrames: " << numFrames << endl;

    // Get frame information:
    // Get the position of the captured frame in [msec] with respect to the first frame,
    // or get its index where the first frame starts with an index of 0. The relative
    // position (ratio) is 0 in the first frame and 1 in the last frame. This ratio is
    // valid only for capturing images from a file.

    float posMsec   =       cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC);
    int posFrames   = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
    float posRatio  =       cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);

    cout << "posMsec: " << posMsec << endl;
    cout << "posFrames: " << posFrames << endl;
    cout << "posRatio: " << posRatio << endl;


    // Set the index of the first frame to capture:
    // start capturing from a relative position of 0.9 of a video file
    //cvSetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO, (double)0.9);

    posMsec   =       cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC);
    posFrames   = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
    posRatio  =       cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);

    cout << "posMsec: " << posMsec << endl;
    cout << "posFrames: " << posFrames << endl;
    cout << "posRatio: " << posRatio << endl;

    // cvReleaseCapture(&capture); // Releasing the capture source:

    // Saving a video file

    // Initializing a video writer:

    CvVideoWriter *writer = 0;
    int isColor_ = 1;
    int fps_     = 29;  // or 30
    int frameW_  = 640; // 744 for firewire cameras
    int frameH_  = 480; // 480 for firewire cameras
    writer=cvCreateVideoWriter("out.avi", CV_FOURCC('D', 'I', 'V', '2'),
                               fps_,cvSize(frameW_,frameH_),isColor_);

    // Other possible codec codes:
    //    CV_FOURCC('P','I','M','1')    = MPEG-1 codec
    //    CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
    //    CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
    //    CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
    //    CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
    //    CV_FOURCC('U', '2', '6', '3') = H263 codec
    //    CV_FOURCC('I', '2', '6', '3') = H263I codec
    //    CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec

    // A codec code of -1 will open a codec selection window (in windows).

    // Writing the video file:

    IplImage* img = 0;
    int nFrames_ = 5000;
    for(int i=0;i<nFrames_;i++){
        cvGrabFrame(capture);          // capture a frame
        img=cvRetrieveFrame(capture);  // retrieve the captured frame
        cvWriteFrame(writer,img);      // add the frame to the file
        // To view the captured frames during capture, add the following in the loop:
//        cvShowImage("mainWin", img);
//        int key=cvWaitKey(20);           // wait 20 ms
    }

    cvReleaseVideoWriter(&writer); // Releasing the video writer

    return 0;
}