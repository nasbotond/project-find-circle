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

            // int numThetas = 30;
            // int r_max = 15; //21/2;
            // int r_min = 15; //21/2;

            // std::vector<cv::Point3i> circle_candidates((90/numThetas)*(r_max-r_min)+1);            

            // for(int r = r_min; r <= r_max; ++r)
            // {
            //     for(int t = 0; t <= 90; t=t+numThetas)
            //     {
            //         circle_candidates.push_back(cv::Point3i(r, static_cast<int>(r*std::cos(t*(M_PI/180))), static_cast<int>(r*std::sin(t*(M_PI/180)))));
            //     }
            // }
            // std::cout << circle_candidates.size() << std::endl;

            // for(int i = 0; i < image.rows; ++i)
            // {
            //     for(int j = 0; j < image.cols; ++j)
            //     {
            //         if(static_cast<int>(edgeImage.at<uchar>(i, j)) != 0)
            //         {
            //             for(int c = 0; c < circle_candidates.size(); ++c)
            //             {
            //                 int x_c = j - circle_candidates.at(c).y;
            //                 int y_c = i - circle_candidates.at(c).z;

            //                 if(static_cast<int>(accumulator.at<uchar>(y_c, x_c)) < 255)
            //                 {
            //                     accumulator.at<uchar>(y_c, x_c) = static_cast<int>(accumulator.at<uchar>(y_c, x_c)) + 1;
            //                 }
            //             }
            //         }
            //     }
            // }

            int numThetas = 30;
            int r_max = 21; //21/2;
            int r_min = 11; //21/2;

            std::vector<cv::Point3i> circle_candidates; //((90/numThetas)*(r_max-r_min)+1);            

            for(int r = r_min; r <= r_max; ++r)
            {
                for(int t = 0; t <= r; ++t)
                {
                    circle_candidates.push_back(cv::Point3i(r, t, (int)std::sqrt((r*r)-(t*t))));
                }
            }

            // std::cout << circle_candidates.size() << std::endl;

            for(int i = 0; i < image.rows; ++i)
            {
                for(int j = 0; j < image.cols; ++j)
                {
                    if(edgeImage.at<uchar>(i, j) > 0)
                    {
                        for(int c = 0; c < circle_candidates.size(); ++c)
                        {
                            int x_c = circle_candidates.at(c).y;
                            int y_c = circle_candidates.at(c).z;
                                                        
                            int x_right = j + x_c;
                            int y_bottom = i + y_c;
                            if(x_right < image.cols && y_bottom < image.rows)
                            {
                                accumulator.at<uchar>(y_bottom, x_right) = accumulator.at<uchar>(y_bottom, x_right) + 1;
                            }
                            // x_right = j + counter;
                            int y_top = i - y_c;
                            if(x_right < image.cols && y_top >= 0)
                            {
                                accumulator.at<uchar>(y_top, x_right) = accumulator.at<uchar>(y_top, x_right) + 1;
                            }
                            int x_left = j - x_c;
                            // y_ = i + y_s[counter];
                            if(x_left >= 0 && y_bottom < image.rows)
                            {
                                accumulator.at<uchar>(y_bottom, x_left) = accumulator.at<uchar>(y_bottom, x_left) + 1;
                            }
                            // x_ = j - counter;
                            // y_ = i - y_s[counter];
                            if(x_left >= 0 && y_top >= 0)
                            {
                                accumulator.at<uchar>(y_top, x_left) = accumulator.at<uchar>(y_top, x_left) + 1;
                            }
                        }
                    }
                }
            }
            
            // for(int i = 0; i < accumulator.rows; ++i)
            // {
            //     for(int j = 0; j < accumulator.cols; ++j)
            //     {
            //         if(static_cast<int>(accumulator.at<uchar>(i, j)) < 60)
            //         {
            //             accumulator.at<uchar>(i, j) = 0;
            //         }
            //     }
            // }

            // cv::Mat outputImage = image.clone();
            // cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR);

            // for(int i = 0; i < accumulator.rows; ++i)
            // {
            //     for(int j = 0; j < accumulator.cols; ++j)
            //     {
            //         double votePercentage = accumulator.at<uchar>(i,j)/360;
            //         if(accumulator.at<uchar>(i,j) >= 50)
            //         {
            //             cv::drawMarker(outputImage, cv::Point(j, i),  cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 10, 1);
            //         }
            //     }
            // }
            

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