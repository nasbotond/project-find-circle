#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define _USE_MATH_DEFINES

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cmath>
#include <stdint.h>
#include <functional> 
namespace fs = std::filesystem;

class HoughTransform
{
    private:
    
        std::string sPath;
        std::string outputFileName;
        int d_min;
        int d_max;
        int threshold;

    public:

        HoughTransform(const std::string &sPath, const std::string &outputFileName, const int &d_min, const int &d_max, const int &threshold) : sPath(sPath), outputFileName(outputFileName), 
            d_min(d_min), d_max(d_max), threshold(threshold) {}
        ~HoughTransform() {}

        // Main functions
        void getCircles();
};