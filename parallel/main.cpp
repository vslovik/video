#include <iostream>
#include "video.h"

using namespace std;

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

    int num_workers = 8;

    const string source = argv[1];

    try {
        process_video(source, num_workers);
    } catch(string e){
        cout << e << endl;
        return -1;
    }

    return 0;
}