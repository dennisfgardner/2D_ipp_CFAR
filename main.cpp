#include <iostream>

#include "ipp.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


int main(){
    std::cout << "Begin Main Program\n";

    // parameters
    Ipp8u winSize = 5;

    // read image and display image
    cv::Mat img;
    img = cv::imread("hd-wallpaper-1867616.jpg", cv::IMREAD_GRAYSCALE);
    std::string win_name = "Display Window";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, 512, 512);
    cv::imshow(win_name, img);
    int k = cv::waitKey(0);

    // image properties
    Ipp16u rows = img.rows;
    Ipp16u cols = img.cols;
    Ipp16u colors = img.channels();
    std::cout << "rows " << rows << " cols " << cols << " channels "<< colors << "\n";
    // I want to get the type of the MAT programatically
    Ipp32u srcStep = rows * sizeof(Ipp8u);
    Ipp32u total_num_bytes = rows * cols * colors * sizeof(Ipp8u) ;
    std::cout << "The total number of bytes is " << total_num_bytes << "\n";

    // sum window function requires work buffer
    IppiSize roiSize = {rows, cols};
    IppiSize maskSize = {winSize, winSize};
    IppDataType dataType = ipp32f;
    int numChannels = 3;
    int bufferSize = 0;
    IppStatus ippSts = ippStsNoErr;

    ippSts = ippiSumWindowGetBufferSize(roiSize, maskSize, dataType, numChannels, &bufferSize);
    std::cout << "ippSts: " << ippSts << "\n";
    std::cout << "bufferSize: " << bufferSize << "\n";
    Ipp8u *pBuffer = (Ipp8u*) ippMalloc(bufferSize);
    // local background
    Ipp32u dstStep = rows * sizeof(Ipp32s);
    Ipp32s *pLocBkg = (Ipp32s*)ippMalloc(dstStep * rows);
    Ipp8u borderValue = 0;
    ippSts = ippiSumWindow_8u32s_C1R((const Ipp8u*)&img.data[0], srcStep, pLocBkg, dstStep, roiSize, maskSize, ippBorderRepl, &borderValue, pBuffer);
    std::cout << "ippSts: " << ippSts << "\n";

    cv::Mat sumWindow(cv::Size(cols, rows), CV_8UC1, (uint8_t*)pLocBkg);

    std::string win_name1 = "Display Window";
    cv::namedWindow(win_name1, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name1, 512, 512);
    cv::imshow(win_name1, sumWindow);
    int l = cv::waitKey(0);

    ippFree(pBuffer);
    ippFree(pLocBkg);

    cv::destroyAllWindows();
}