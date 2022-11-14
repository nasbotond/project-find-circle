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

    public:

        HoughTransform(const std::string &sPath, const std::string &outputFileName) : sPath(sPath), outputFileName(outputFileName) {}
        ~HoughTransform() {}

        // Main functions
        void getCircles();
        cv::Mat CannyEdge(cv::Mat& image);
        void prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS, std::vector<double>& gradientDirections);
};