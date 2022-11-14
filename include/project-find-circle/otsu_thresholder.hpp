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

namespace OtsuThresholder
{
    /// @brief Determines the first greyvalue where P(t) is not zero
    /// @param hist Histogram of greyvalues (normalized)
    /// @return Returns the first greyvalue where P(t) is not zero
    int getFirstNonzeroIndex(std::vector<double>& hist)
    {
        int index = 0;

        while(hist.at(index) == 0)
        {
            index++;
        }

        return index;
    }

    /// @brief Recursively calculates weight of Class 1
    /// @param t Greyvalue
    /// @param firstNZIndex First greyvalue where P(t) is not zero
    /// @param hist Histogram of greyvalues (normalized)
    /// @return Returns the weight of Class 1
    double calculateWeight1(int t, int& firstNZIndex, std::vector<double>& hist)
    {
        if (t == 0) // terminating condition
        {
            return hist.at(firstNZIndex);
        }
        else
        {
            return calculateWeight1(t-1, firstNZIndex, hist) + hist.at(t);
        }
    }

    /// @brief Recursively calculates mean of Class 1
    /// @param t Greyvalue
    /// @param firstNZIndex First greyvalue where P(t) is not zero
    /// @param hist Histogram of greyvalues (normalized)
    /// @return Returns the mean of Class 1
    double calculateMean1(int t, int& firstNZIndex, std::vector<double>& hist)
    {
        if (t == 0) // terminating condition
        {
            return 0;
        }
        else
        {
            return (calculateWeight1(t-1, firstNZIndex, hist)*calculateMean1(t-1, firstNZIndex, hist) + (double)t*hist.at(t))/calculateWeight1(t, firstNZIndex, hist);
        }
    }

    /// @brief Calculates the optimal threshold
    /// @param hist Histogram of greyvalues (normalized)
    /// @param mean Mean of histogram
    /// @return Returns the optimal threshold (between 0-255)
    int calculateOptimalThreshold(std::vector<double>& hist, double& mean)
    {
        double maxVarSquared = 0.0;
        int optThresh = 0;

        // Skip zeroes at the beginning of histogram
        int firstIndex = getFirstNonzeroIndex(hist);

        for(int t = firstIndex; t < 256; ++t)
        {
            double weight1 = calculateWeight1(t, firstIndex, hist);
            double mean1 = calculateMean1(t, firstIndex, hist);
            double mean2 = (mean - weight1*mean1)/(1-weight1);

            double varSquared = weight1*(1.0-weight1)*pow((mean1-mean2), 2);

            if(varSquared > maxVarSquared)
            {
                optThresh = t;
                maxVarSquared = varSquared;
            }
        }

        return optThresh;
    }

    /// @brief Calculates the normalized greyvalue histogram
    /// @param image Input greyscale image
    /// @return A vector of P values
    std::vector<double> calculateHistogram(cv::Mat& image)
    {
        std::vector<double> hist(256);
        int numPixels = image.total();

        for(int i = 0; i < image.rows; ++i)
        {
            for(int j = 0; j < image.cols; ++j)
            {
                hist.at(image.at<uchar>(i,j)) = hist.at(image.at<uchar>(i,j)) + 1;
            }
        }

        std::transform(hist.begin(), hist.end(), hist.begin(), [&numPixels](auto &c){ return c/numPixels; });

        return hist;
    }

    /// @brief Calculate the mean of the histogram
    /// @param hist Histogram of greyvalues (normalized)
    /// @return Returns the mean
    double getHistogramMean(std::vector<double>& hist)
    {
        double mean = 0.0;

        for(int i = 0; i < hist.size(); ++i)
        {
            mean = mean + (i*hist.at(i));
        }

        return mean;
    }
}