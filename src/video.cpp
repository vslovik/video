#include "video.hpp"

using namespace cv;

pthread_mutex_t mutexLock;
State *process;

void read_video(string source)
{
    VideoCapture inputVideo(source);
    if (!inputVideo.isOpened())
    {
        throw source;
    }

    int numFrames = (int) inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    process = new State(
                inputVideo.get(CV_CAP_PROP_FPS),
                numFrames,
                Size(
                        (int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                        (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)
                ),
                0,
                new Mat[numFrames],
                new Mat[numFrames],
                "out.avi"
                );

    cout << "Input frame resolution: Width=" << process->size.width << "  Height=" << process->size.height
         << " of nr#: " << process->numFrames << endl;
    cout << "Input codec type: " << ".avi" << endl;

    for(int i = 0; i < process->numFrames; ++i) {
        inputVideo >> process->inFrames[i];
    }

    inputVideo.release();

}

void write_video(string source)
{
    VideoWriter outputVideo;
    outputVideo.open(process->output, CV_FOURCC('D', 'I', 'V', 'X'), process->fps, process->size, true);

    if (!outputVideo.isOpened())
    {
        throw source;
    }

    for(int i = 0; i < process->numFrames; ++i){
        outputVideo << process->inFrames[i];
//        imshow("mainWin", process->inFrames[i]);
//        waitKey(20);
    }

    outputVideo.release();
    cout << "Finished writing" << endl;
}

void *process_frame(void *)
{
    int id, index;
    while(true) {

        pthread_mutex_lock(&mutexLock);

        if(process->frameIter > process->numFrames - 1) {
            pthread_mutex_unlock(&mutexLock);
            return NULL;
        }

        id = process->frameIter++;
        pthread_mutex_unlock(&mutexLock);

        Mat *bunch = new Mat[5];
        ostringstream ss;

        ss << id; cout << ("frame: " + ss.str() + "\n");

        for(int i = 0; i < 5; i++) {
            index = id + i < process->numFrames ? id + i : process->numFrames - 1;
            bunch[i] = process->inFrames[index];
        }

        process->outFrames[id] = reduce_frame(bunch, process->ver, process->hor);
    }
}

void process_video()
{
    int numWorkers = 4;
    cout << "Using " << numWorkers << " workers." << endl;

    pthread_t *workers = (pthread_t *)malloc(sizeof(pthread_t)*numWorkers);
    for(int i = 0; i < numWorkers; ++i) {
        if(pthread_create(&workers[i], NULL, process_frame, NULL)) {
            throw "Error creating thread\n";
        }
    }

    for(int i = 0; i < numWorkers; ++i) {
        if(pthread_join(workers[i], NULL)) {
            throw "Error joining thread\n";
        }
    }
}