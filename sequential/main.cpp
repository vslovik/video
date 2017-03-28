#include <iostream>
#include "svideo.h"

using namespace std;

static void help()
{
    cout
            << "------------------------------------------------------------------------------" << endl
            << "This program retargets video files."                                            << endl
            << "Usage:"                                                                         << endl
            << "./svideo inputvideoName"                                                         << endl
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
        process_video(source);
    } catch(string e){
        cout << e << endl;
        return -1;
    }

    return 0;
}