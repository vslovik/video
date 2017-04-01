#include <iostream>
#include "video.h"

static void help()
{
	std::cout
            << "------------------------------------------------------------------------------" << std::endl
            << "This program reads filters and write video files."                              << std::endl
            << "Usage:"                                                                         << std::endl
            << "./video inputvideoName"                                                         << std::endl
            << "------------------------------------------------------------------------------" << std::endl
            << std::endl;
}

int main(int argc, char **argv)
{
    help();

    if(argc < 2) {
	    std::cout << "Not enough parameters" << std::endl;
        return -1;
    }

    int num_workers = 1;

    const std::string source = argv[1];

    try {
        process_video(source, num_workers);
    } catch(string e){
	    std::cout << e << std::endl;
        return -1;
    }

    return 0;
}