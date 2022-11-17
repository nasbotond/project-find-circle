#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include "hough_transform.hpp"

int main(int argc, char* argv[]) 
{
    if (argc < 6) 
    {
        std::cerr << "Usage: " << argv[0] << " <path to directory containing images> <output file prefix> <minimum diameter> <maximum diameter> <threshold>" << std::endl;
        return 1;
    }

    std::string sPath = argv[1];
    std::string outputFileName = argv[2];
    int d_min = std::stoi(argv[3]);
    int d_max = std::stoi(argv[4]);
    int threshold =std::stoi(argv[5]);

    // Check if path is valid
    if(!std::filesystem::exists(sPath)) 
    {
        std::cout << "Path is not valid!" << std::endl;
        return -1;
    }

    // Input
    HoughTransform hough = HoughTransform(sPath, outputFileName, d_min, d_max, threshold);    

    try 
    {
        hough.getCircles();
    } 
    catch(const std::exception &e) 
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}