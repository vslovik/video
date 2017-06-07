#include <iostream>
#include "video.h"

static void help()
{
	std::cout
            << "------------------------------------------------------------------------------" << std::endl
            << "This program reads filters and write video files."                              << std::endl
            << "Usage:"                                                                         << std::endl
            << "./video inputvideoName ver hor num_workers"                                     << std::endl
			<< "hor - number of vertical seams to remove (and shrink horizontally)"             << std::endl
			<< "ver - number of horizontal seams to remove (and shrink vertically)"             << std::endl
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
//	int hor = atoi(argv[2]);
//	int ver = atoi(argv[3]);
//	int num_workers = atoi(argv[4]);
//
//	if(ver <= 0 && hor <= 0) {
//		std::cout << "Specify number of vertical seams to remove and/or number of horizontal seams to remove" << std::endl;
//		return -1;
//	}
//
//	if(num_workers <= 0) {
//		std::cout << "Specify number of workers" << std::endl;
//		return -1;
//	}
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