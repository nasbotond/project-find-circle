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

            // Get edges
            cv::Mat edgeImage = CannyEdge(image);
            cv::Mat accumulator = cv::Mat::zeros(image.rows, image.cols, CV_8U);
            
            std::vector<cv::Point3i> circle_candidates;
            int r_max = 27; //21/2;
            int r_min = 11; //21/2;

            for(int r = r_min; r <= r_max; ++r)
            {
                for(int t = 0; t <= 360; t=t+5)
                {
                    circle_candidates.push_back(cv::Point3i(r, static_cast<int>(r*std::cos(t*M_PI/180)), static_cast<int>(r*sin(t*M_PI/180))));
                }
            }

            for(int i = 0; i < image.rows; ++i)
            {
                for(int j = 0; j < image.cols; ++j)
                {
                    if(edgeImage.at<uchar>(i, j) > 0)
                    {
                        for(int c = 0; c < circle_candidates.size(); ++c)
                        {
                            int x_c = j - circle_candidates.at(c).y;
                            int y_c = i - circle_candidates.at(c).z;
                            if(accumulator.at<uchar>(y_c, x_c) < 255)
                            {
                                accumulator.at<uchar>(y_c, x_c) += 1;
                            }
                        }
                    }
                }
            }


            /*
            // loop through diameters
            for(int d = 19; d <= 19; d = d+2)
            {
                double radius = (double)d/2;
                double r_2 = radius*radius;

                // calculate circle y's given radius and x's
                int y_s[(int)radius];

                for(int x = 0; x <= radius; ++x)
                {
                    y_s[x] = (int)std::sqrt(r_2-(x*x));
                }
                std::cout << y_s[0] << std::endl;

                for(int i = 0; i < image.rows; ++i)
                {
                    for(int j = 0; j < image.cols; ++j)
                    {
                        if(edgeImage.at<uchar>(i, j)>0)
                        {
                            // increment accumulator
                            int counter = 0;
                            while(counter <= radius)
                            {
                                int x_right = j + counter;
                                int y_bottom = i + y_s[counter];
                                if(x_right < image.cols && y_bottom < image.rows)
                                {
                                    accumulator.at<uchar>(y_bottom, x_right) = accumulator.at<uchar>(y_bottom, x_right) + 1;
                                }
                                // x_right = j + counter;
                                int y_top = i - y_s[counter];
                                if(x_right < image.cols && y_top >= 0)
                                {
                                    accumulator.at<uchar>(y_top, x_right) = accumulator.at<uchar>(y_top, x_right) + 1;
                                }
                                int x_left = j - counter;
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
                                counter++;
                            }
                        }
                    }
                }
            }
            */

            // int optThresh = calculateOptimalThreshold(hist, mean);
            // std::cout << optThresh << std::endl;
            
            // // Apply threshold to output image
            // cv::Mat output = image.clone();

            // for(int i = 0; i < output.rows; ++i)
            // {
            //     for(int j = 0; j < output.cols; ++j)
            //     {
            //         output.at<uchar>(i,j) = output.at<uchar>(i,j) > optThresh ? 255 : 0;
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
            cv::imshow("(1) Original : (2) Thresholded", accumulator);
            cv::waitKey(0);
        }
    }
}

cv::Mat HoughTransform::CannyEdge(cv::Mat& image)
{
    cv::Mat blurredImage;
    cv::Mat cannyImage;

    cv::GaussianBlur(image, blurredImage, cv::Size(3, 3), 0.5);
    cv::Canny(image, cannyImage, 50, 200);

    return cannyImage;
}