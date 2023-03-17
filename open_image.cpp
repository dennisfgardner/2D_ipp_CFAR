#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

int main(){
    std::cout << "An Example of How to Open an Image File\n";

    cv::Mat img;

    img = cv::imread("hd-wallpaper-1867616.jpg", cv::IMREAD_COLOR);

    std::cout << "the data type is " << img.type() << "\n";

    int rows = img.rows;
    int cols = img.cols;
    std::cout << "rows " << rows << " cols " << cols << "\n";

    std::string win_name = "Display Window";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, 512, 512);
    cv::imshow(win_name, img);
    int k = cv::waitKey(0);
    return 0;


}