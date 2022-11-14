#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define _USE_MATH_DEFINES

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cmath>
#include <stdint.h>
#include <functional> 

class OtsuThresholder
{
    private:
    

    public:

        OtsuThresholder() {}
        ~OtsuThresholder() {}

        // Main functions
        static int calculateOptimalThreshold(std::vector<double>& hist, double& mean);
        static std::vector<double> calculateHistogram(cv::Mat& image);
        static double getHistogramMean(std::vector<double>& hist);
        static double calculateWeight1(int t, int& firstNZIndex, std::vector<double>& hist);
        static double calculateMean1(int t, int& firstNZIndex, std::vector<double>& hist);
        static int getFirstNonzeroIndex(std::vector<double>& hist);
};