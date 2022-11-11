#include "hough_transform.hpp"

void HoughTransform::getCircles()
{    
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNameAcc = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_acc.png";
            std::string outputNameCircles = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_circles.png";
            std::string outputNameCombined = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_combined.png";
        
            std::cout << "------------------ Parameters -------------------" << std::endl;
            std::cout << "Path = " << sPath << std::endl;
            std::cout << "Output 1 = " << outputNameAcc << std::endl;
            std::cout << "Output 2 = " << outputNameCircles << std::endl;
            std::cout << "Output 3 = " << outputNameCombined << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;

            // Read image into Mat
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
            // std::cout << image.type() << std::endl;

            // Get edges
            cv::Mat edgeImage = CannyEdge(image);
            cv::Mat accumulator = cv::Mat::zeros(image.rows, image.cols, CV_8U);

            cv::Mat outputPrewitt;
            cv::Mat outputPrewittNMS;

            std::vector<double> gradientDirections(image.rows*image.cols);
            prewittEdgeDetectorWithNMS(image, outputPrewitt, outputPrewittNMS, gradientDirections);

            int r_max = 29; //21/2;
            int r_min = 7; //21/2;

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

                            // accumulator.at<uchar>(i+static_cast<int>(r*std::sin(perpendicularDirection)), j+static_cast<int>(r*std::cos(perpendicularDirection))) += 1;
                            // accumulator.at<uchar>(i-static_cast<int>(r*std::sin(perpendicularDirection)), j-static_cast<int>(r*std::cos(perpendicularDirection))) += 1;
                        }
                    }    
                }
            }

            // int numThetas = 30;
            // int r_max = 21; //21/2;
            // int r_min = 11; //21/2;

            // std::vector<cv::Point3i> circle_candidates; //((90/numThetas)*(r_max-r_min)+1);            

            // for(int r = r_min; r <= r_max; ++r)
            // {
            //     for(int t = 0; t <= r; ++t)
            //     {
            //         circle_candidates.push_back(cv::Point3i(r, t, (int)std::sqrt((r*r)-(t*t))));
            //     }
            // }

            // // std::cout << circle_candidates.size() << std::endl;

            // for(int i = 0; i < image.rows; ++i)
            // {
            //     for(int j = 0; j < image.cols; ++j)
            //     {
            //         if(edgeImage.at<uchar>(i, j) > 0)
            //         {
            //             for(int c = 0; c < circle_candidates.size(); ++c)
            //             {
            //                 int x_c = circle_candidates.at(c).y;
            //                 int y_c = circle_candidates.at(c).z;
                                                        
            //                 int x_right = j + x_c;
            //                 int y_bottom = i + y_c;
            //                 if(x_right < image.cols && y_bottom < image.rows)
            //                 {
            //                     accumulator.at<uchar>(y_bottom, x_right) = accumulator.at<uchar>(y_bottom, x_right) + 1;
            //                 }
            //                 // x_right = j + counter;
            //                 int y_top = i - y_c;
            //                 if(x_right < image.cols && y_top >= 0)
            //                 {
            //                     accumulator.at<uchar>(y_top, x_right) = accumulator.at<uchar>(y_top, x_right) + 1;
            //                 }
            //                 int x_left = j - x_c;
            //                 // y_ = i + y_s[counter];
            //                 if(x_left >= 0 && y_bottom < image.rows)
            //                 {
            //                     accumulator.at<uchar>(y_bottom, x_left) = accumulator.at<uchar>(y_bottom, x_left) + 1;
            //                 }
            //                 // x_ = j - counter;
            //                 // y_ = i - y_s[counter];
            //                 if(x_left >= 0 && y_top >= 0)
            //                 {
            //                     accumulator.at<uchar>(y_top, x_left) = accumulator.at<uchar>(y_top, x_left) + 1;
            //                 }
            //             }
            //         }
            //     }
            // }
            
            for(int i = 0; i < accumulator.rows; ++i)
            {
                for(int j = 0; j < accumulator.cols; ++j)
                {
                    if(static_cast<int>(accumulator.at<uchar>(i, j)) < 20)
                    {
                        accumulator.at<uchar>(i, j) = 0;
                    }
                }
            }

            cv::Mat outputImage = image.clone();
            cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR);

            for(int i = 0; i < accumulator.rows; ++i)
            {
                for(int j = 0; j < accumulator.cols; ++j)
                {
                    double votePercentage = accumulator.at<uchar>(i,j)/360;
                    if(accumulator.at<uchar>(i,j) >= 30)
                    {
                        cv::drawMarker(outputImage, cv::Point(j, i),  cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 10, 1);
                    }
                }
            }
            

            // // Create combined image to view results side-by-side
            // cv::Mat combinedImage(image.rows, 2*image.cols, CV_8U);
            // cv::Mat left(combinedImage, cv::Rect(0, 0, image.cols, image.rows));
            // image.copyTo(left);
            // cv::Mat center(combinedImage, cv::Rect(image.cols, 0, image.cols, image.rows));
            // output.copyTo(center);

            // // Save results (write to file)
            // cv::imwrite(outputNameOtsu, output);
            // cv::imwrite(outputNameCombined, combinedImage);

            // Show combined image
            // cv::imshow("(1) Original : (2) Thresholded", edgeImage);
            cv::imshow("(1) Original : (2) Thresholded", accumulator);
            // cv::imshow("(1) Original : (2) Thresholded", outputImage);
            cv::waitKey(0);
        }
    }
}

cv::Mat HoughTransform::CannyEdge(cv::Mat& image)
{
    cv::Mat blurredImage;
    cv::Mat cannyImage;

    cv::GaussianBlur(image, blurredImage, cv::Size(7, 7), 0.5);
    cv::Canny(image, cannyImage, 50, 200, 3);

    return cannyImage;
}

void HoughTransform::prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS, std::vector<double>& gradientDirections)
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