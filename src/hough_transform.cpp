#include "hough_transform.hpp"
#include "otsu_thresholder.hpp"
#include "prewitt_edge_detector.hpp"

/// @brief Get and mark circle centers on images contained in directory using Hough Circle Detection
void HoughTransform::getCircles()
{    
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNameAcc = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_acc.png";
            std::string outputNameCircles = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_circle_centers.png";
            std::string outputNameCombined = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_combined.png";

            int r_max = this->d_max/2; //29; //21/2;
            int r_min = this->d_min/2; //7; //21/2;
        
            std::cout << "------------------ Parameters -------------------" << std::endl;
            std::cout << "Path = " << sPath << std::endl;
            std::cout << "r_min = " << r_min << std::endl;
            std::cout << "r_max = " << r_max << std::endl;
            std::cout << "Threshold = " << threshold << std::endl;
            std::cout << "Output 1 = " << outputNameAcc << std::endl;
            std::cout << "Output 2 = " << outputNameCircles << std::endl;
            std::cout << "Output 3 = " << outputNameCombined << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;

            // Read image into Mat
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

            cv::Mat accumulator = cv::Mat::zeros(image.rows, image.cols, CV_8U);

            // Get edges and gradient directions
            cv::Mat outputPrewitt;
            cv::Mat outputPrewittNMS;

            std::vector<double> gradientDirections(image.rows*image.cols);
            PrewittEdgeDetector::prewittEdgeDetectorWithNMS(image, outputPrewitt, outputPrewittNMS, gradientDirections);

            // Fill accumulator array
            for(int i = 0; i < image.rows; ++i)
            {
                for(int j = 0; j < image.cols; ++j)
                {
                    double perpendicularDirection = gradientDirections.at(i*image.cols+j);

                    if(outputPrewittNMS.at<uchar>(i, j) > 0)
                    {
                        for(int r = r_min; r <= r_max; ++r)
                        {
                            int x_c = static_cast<int>(r*std::cos(perpendicularDirection));
                            int y_c = static_cast<int>(r*std::sin(perpendicularDirection));

                            int x_right = j + x_c;
                            int y_bottom = i + y_c;
                            int y_top = i - y_c;
                            int x_left = j - x_c;

                            if((x_right > 0 && x_right < image.cols) && (y_bottom > 0 && y_bottom < image.rows))
                            {
                                accumulator.at<uchar>(y_bottom, x_right) = accumulator.at<uchar>(y_bottom, x_right) + 1;
                            }
                            if((x_left > 0 && x_left < image.cols) && (y_top > 0 && y_top < image.rows))
                            {
                                accumulator.at<uchar>(y_top, x_left) = accumulator.at<uchar>(y_top, x_left) + 1;
                            }
                        }
                    }
                }
            }
            
            // Apply threshold to thin accumulator
            for(int i = 0; i < accumulator.rows; ++i)
            {
                for(int j = 0; j < accumulator.cols; ++j)
                {
                    if(static_cast<int>(accumulator.at<uchar>(i, j)) < this->threshold)
                    {
                        accumulator.at<uchar>(i, j) = 0;
                    }
                }
            }

            cv::normalize(accumulator, accumulator, 0, 255, cv::NORM_MINMAX, CV_8U);

            // Otsu Thresholding
            std::vector<double> hist = OtsuThresholder::calculateHistogram(accumulator);
            double mean = OtsuThresholder::getHistogramMean(hist);

            int optThresh = OtsuThresholder::calculateOptimalThreshold(hist, mean);
            
            // Apply threshold to output image
            cv::Mat thresholded = accumulator.clone();

            for(int i = 0; i < thresholded.rows; ++i)
            {
                for(int j = 0; j < thresholded.cols; ++j)
                {
                    thresholded.at<uchar>(i,j) = thresholded.at<uchar>(i,j) > optThresh ? 255 : 0;
                }
            }

            // Mark circle centers
            cv::Mat markedCircles = image.clone();
            cvtColor(markedCircles, markedCircles, cv::COLOR_GRAY2BGR);

            for(int i = 0; i < accumulator.rows; ++i)
            {
                for(int j = 0; j < accumulator.cols; ++j)
                {
                    // double votePercentage = thresholded.at<uchar>(i,j)/360;
                    if(accumulator.at<uchar>(i,j) != 0)
                    {
                        cv::drawMarker(markedCircles, cv::Point(j, i),  cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 10, 1);
                    }
                }
            }

            // Create combined image to view results side-by-side
            cv::Mat combinedImage(image.rows, 2*image.cols, CV_8UC3);
            cv::Mat left(combinedImage, cv::Rect(0, 0, image.cols, image.rows));
            cv::Mat acc_color;
            cv::cvtColor(accumulator, acc_color, cv::COLOR_GRAY2BGR);
            acc_color.copyTo(left);
            cv::Mat center(combinedImage, cv::Rect(image.cols, 0, image.cols, image.rows));
            markedCircles.copyTo(center);

            // // Save results (write to file)
            cv::imwrite(outputNameAcc, accumulator);
            cv::imwrite(outputNameCircles, markedCircles);
            cv::imwrite(outputNameCombined, combinedImage);

            // Show combined image
            cv::imshow("Combined", combinedImage);
            cv::waitKey(0);
        }
    }
}