#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <cstdlib>
#include <string>
#include <numeric>
#include <vector>
#include <cmath>
#include <stdint.h>
#include <functional> 

namespace PrewittEdgeDetector
{
    /// @brief Prewitt Edge Detector with NMS
    /// @param image 
    /// @param outputPrewitt 
    /// @param outputPrewittNMS 
    /// @param gradientDirections 
    void prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS, std::vector<double>& gradientDirections)
    {
        std::vector<uint8_t> magnitudes(image.rows*image.cols);
        std::vector<uint8_t> magnitudesAfterNMS(image.rows*image.cols);

        int kernelSize = 3;
        int halfKernelSize = kernelSize / 2;

        for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
        {
            for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
            {
                double magnitudeX = 0;
                double magnitudeY = 0;

                for(int u = -halfKernelSize; u <= halfKernelSize; ++u)
                {
                    for(int v = -halfKernelSize; v <= halfKernelSize; ++v)
                    {
                        magnitudeX += (double)v*(double)image.at<uchar>(i+u,j+v);
                        magnitudeY += (double)u*(double)image.at<uchar>(i+u,j+v);
                    }
                }
                int magnitude = (int)sqrt(pow(magnitudeX, 2) + pow(magnitudeY, 2));
                magnitude = magnitude > 255 ? 255 : magnitude;
                magnitude = magnitude < 0 ? 0 : magnitude;
                magnitudes.at(i*image.cols + j) = magnitude;

                // gradientDirections.at(i*image.cols+j) = magnitudeY != 0 ? atan(magnitudeX/magnitudeY) : atan(magnitudeX/(magnitudeY+0.0000001));
                gradientDirections.at(i*image.cols+j) = std::atan2(magnitudeY, magnitudeX);
            }
        }

        cv::Mat(image.rows, image.cols, CV_8U, magnitudes.data()).copyTo(outputPrewitt); // deep copy

        // Algoritmus: Non-maxima suppression
        // 1 From each position (x,y), step in the two directions perpendicular to edge orientation Θ(x,y)
        // 2 Denote inital pixel (x,y) by C, the two neighbouring pixels in perpendicular directions by A and B
        // 3 If M(A) > M(C) or M(B) > M(C), discard pixel (x,y) by setting M(x, y) = 0

        double angle = 22.5*(M_PI/180.0);

        for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
        {
            for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
            {
                double perpendicularDirection = gradientDirections.at(i*image.cols+j);

                if(abs(perpendicularDirection) <= angle)
                {
                    // horizontal
                    magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i+1)*image.cols+j) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i-1)*image.cols+j) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
                }
                else if(abs(perpendicularDirection) >= (M_PI/2 - angle) && abs(perpendicularDirection) <= (M_PI/2 + angle))
                {
                    // vertical
                    magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at(i*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at(i*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
                }
                else if(abs(perpendicularDirection) > (angle) && abs(perpendicularDirection) < (M_PI/2 - angle))
                {
                    // negative diagonal
                    magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i-1)*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i+1)*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);                
                }
                else
                {
                    // positive diagonal
                    magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i+1)*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i-1)*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
                }
            }
        }

        cv::Mat(image.rows, image.cols, CV_8U, magnitudesAfterNMS.data()).copyTo(outputPrewittNMS); // deep copy
    }
}