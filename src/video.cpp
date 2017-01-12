#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    if(argc < 2) {
        cout << "./video [video.avi]" << endl;
        return 0;
    }

    // Initializing capture source
    CvCapture* capture = cvCaptureFromAVI(argv[1]);
    if(!cvGrabFrame(capture)){ // capture a frame
        printf("Could not grab a frame\n\7");
        exit(0);
    }

    // Getting frame information
    cvQueryFrame(capture); // this call is necessary to get correct capture properties
    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    int numFrames = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);

    cout << "frameH: " << frameH << endl;
    cout << "frameW: " << frameW << endl;
    cout << "fps: " << fps << endl;
    cout << "numFrames: " << numFrames << endl;

    // Initializing a video writer:
    CvVideoWriter *writer = 0;
    int isColor = 1;
    writer=cvCreateVideoWriter("out.avi", CV_FOURCC('D', 'I', 'V', 'X'),
                               fps,cvSize(frameW,frameH),isColor);

    // Capturing and Writing the video file:
    IplImage* img = 0;
    for(int i=0;i < numFrames-2;i++){
        cvGrabFrame(capture);          // capture a frame
        img=cvRetrieveFrame(capture);  // retrieve the captured frame
        cvWriteFrame(writer,img);      // add the frame to the file
        // To view the captured frames during capture, add the following in the loop:
        cvShowImage("mainWin", img);
        int key=cvWaitKey(20); // wait 20 ms
//        cout << i << endl;
    }

    cvReleaseVideoWriter(&writer); // Releasing the video writer
    cvReleaseCapture(&capture); // Releasing the capture source:

    return 0;
}