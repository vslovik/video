#include <iostream>

#include "video.hpp"

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

int main(int argc, char **argv)
{
    help();

    if(argc < 2) {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    const string source = argv[1];

    try {
        read_video(source);
    } catch(string e){
        cout  << "Could not open the input video: " << e << endl;
        return -1;
    }

    try {
        process_video();
    } catch(string e){
        cout << e << endl;
        return -1;
    }

    try {
        write_video(source);
    } catch(string e){
        cout  << "Could not open the output video for write: " << source << endl;
        return -1;
    }

    return 0;
}