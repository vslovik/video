#include <iostream>
#include "video.h"

static void help()
{
	std::cout
            << "------------------------------------------------------------------------------" << std::endl
            << "This program reads filters and write video files."                              << std::endl
            << "Usage:"                                                                         << std::endl
            << "./video inputvideoName ver hor num_workers"                                             << std::endl
            << "------------------------------------------------------------------------------" << std::endl
            << std::endl;
}

//int main(int argc, char **argv)
//{
//    help();
//
//    if(argc < 5) {
//	    std::cout << "Not enough parameters" << std::endl;
//        return -1;
//    }
//
//    const std::string source = argv[1];
//
//	int ver = atoi(argv[2]);
//	int hor = atoi(argv[3]);
//	int num_workers = atoi(argv[4]);
//
//    try {
//        process_video(source, ver, hor, num_workers);
//    } catch(string e){
//	    std::cout << e << std::endl;
//        return -1;
//    }
//
//    return 0;
//}