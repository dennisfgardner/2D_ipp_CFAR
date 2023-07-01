#include <iostream>

#include "ipp.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using std::cout, std::endl, std::string;

#define DEBUG_PLOTS
// #define VERBOSE

#ifdef DEBUG_PLOTS 
void DEBUG_PLOTS_helper(cv::Mat img, string description){
    cv::Mat temp = img.clone();
    temp.convertTo(temp, CV_32FC1);
    // display the img in a window
    cout << "examining " << description << endl;
    double maxV, minV;
    cv::minMaxIdx(temp, &minV, &maxV);
    cout << "min " << minV << " maxV " << maxV << endl;
    // rescale for DEBUG_PLOTSting (must be between 0 and 1)
    temp = temp - minV;
    temp = temp/maxV;
    cv::namedWindow(description, cv::WINDOW_NORMAL);
    cv::resizeWindow(description, 512, 512);
    cv::imshow(description, temp);
    int k = cv::waitKey(0);
}
#endif

int calcSumBufferSize(cv::Mat img, IppiSize roiSize, IppiSize maskSize){
    // return window sum buffer size
    int bufferSize;
    IppStatus ippSts = ippiSumWindowGetBufferSize(roiSize, maskSize, ipp32f, 1, &bufferSize);
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the work butter size"  << "\n";
    #endif
    return bufferSize;
}

cv::Mat calcBoxSum(cv::Mat src, int winSize){
    // return box sum
    Ipp32u step = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_32F);
    IppiSize roiSize = {src.cols, src.rows};
    IppiSize maskSize = {winSize, winSize};
    Ipp32f borderValue = 0;
    int bufferSize = calcSumBufferSize(src, roiSize, maskSize);
    Ipp8u *pBuffer = (Ipp8u*) ippMalloc(bufferSize);
    IppStatus ippSts = ippiSumWindow_32f_C1R(
        (const Ipp32f*)&src.data[0], step,
        (Ipp32f*)&dst.data[0], step,
        roiSize, maskSize,
        ippBorderRepl, &borderValue,
        pBuffer
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the box sum"  << "\n";
    #endif
    ippFree(pBuffer);
    return dst;
}

cv::Mat calcSubtraction(cv::Mat src1, cv::Mat src2){
    // subtract src1 from src2, i.e. dst = src2 - src1
    Ipp32u step = src1.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src1.cols, src1.rows), CV_32F);;
    IppiSize roiSize = {src1.cols, src1.rows};
    IppStatus ippSts = ippiSub_32f_C1R(
        (const Ipp32f*)&src1.data[0], step, 
        (const Ipp32f*)&src2.data[0], step,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the difference"  << "\n";
    #endif
    return dst;
}

cv::Mat calcDivideScalar(cv::Mat src, int scalar){
    Ipp32u step = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_32F);;
    IppiSize roiSize = {src.cols, src.rows};
    IppStatus ippSts = ippiDivC_32f_C1R(
        (const Ipp32f*)&src.data[0], step,
        scalar,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the scalar division"  << "\n";
    #endif
    return dst;
}

cv::Mat calcDonutAvg(cv::Mat img, int innerSize, int outerSize){
    cv::Mat outerSum = calcBoxSum(img, outerSize);
    cv::Mat innerSum = calcBoxSum(img, innerSize);
    cv::Mat donutSum = calcSubtraction(innerSum, outerSum);
    int numPixels = outerSize*outerSize - innerSize*innerSize;
    cv::Mat donutAvg = calcDivideScalar(donutSum, numPixels);
    return donutAvg;
}

cv::Mat calcSquare(cv::Mat src){
    Ipp32u step = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_32F);;
    IppiSize roiSize = {src.cols, src.rows};
    IppStatus ippSts = ippiSqr_32f_C1R(
        (const Ipp32f*)&src.data[0], step,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the square"  << "\n";
    #endif
    return dst;
}

cv::Mat calcSqrt(cv::Mat src){
    Ipp32u step = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_32F);;
    IppiSize roiSize = {src.cols, src.rows};
    IppStatus ippSts = ippiSqrt_32f_C1R(
        (const Ipp32f*)&src.data[0], step,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the square root"  << "\n";
    #endif
    return dst;
}

cv::Mat calcMultiplyScalar(cv::Mat src, float scalar){
    Ipp32u step = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_32F);;
    IppiSize roiSize = {src.cols, src.rows};
    IppStatus ippSts = ippiMulC_32f_C1R(
        (const Ipp32f*)&src.data[0], step,
        scalar,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating the scalar multiplication"  << "\n";
    #endif
    return dst;
}

cv::Mat calcAddition(cv::Mat src1, cv::Mat src2){
    Ipp32u step = src1.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src1.cols, src1.rows), CV_32F);;
    IppiSize roiSize = {src1.cols, src1.rows};
    IppStatus ippSts = ippiAdd_32f_C1R(
        (const Ipp32f*)&src1.data[0], step,
        (const Ipp32f*)&src2.data[0], step,
        (Ipp32f*)&dst.data[0], step,
        roiSize
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating scalar addition"  << "\n";
    #endif
    return dst;
}

cv::Mat calcOutliers(cv::Mat src, cv::Mat threshold_values){
    Ipp32u step_src = src.cols * sizeof(Ipp32f);
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_8UC1);
    Ipp32u step_dst = src.cols * sizeof(Ipp8u);
    IppiSize roiSize = {src.cols, src.rows};
    IppStatus ippSts = ippiCompare_32f_C1R(
        (const Ipp32f*)&src.data[0], step_src,
        (const Ipp32f*)&threshold_values.data[0], step_src,
        (Ipp8u*)&dst.data[0], step_dst,
        roiSize,
        ippCmpGreater
    );
    #ifdef VERBOSE 
    cout << ippGetStatusString(ippSts) << " when calculating outliers"  << "\n";
    #endif
    return dst;
}


int main(){
    cout << "Begin Main Program\n";

    // read image and display image
    cv::Mat img;
    img = cv::imread("hd-wallpaper-1867616.jpg", cv::IMREAD_GRAYSCALE);
    img.convertTo(img, CV_32FC1);

    #ifdef DEBUG_PLOTS
    DEBUG_PLOTS_helper(img, "input image");
    #endif

    cv::Mat E_of_x = calcDonutAvg(img, 41, 101);
    cv::Mat E_squared = calcSquare(E_of_x);
    cv::Mat img_squared = calcSquare(img);
    cv::Mat E_of_x_squared = calcDonutAvg(img_squared, 41, 101);
    cv::Mat var = calcSubtraction(E_squared, E_of_x_squared);
    cv::Mat std = calcSqrt(var);

    #ifdef DEBUG_PLOTS
    DEBUG_PLOTS_helper(E_of_x, "E[x]");
    DEBUG_PLOTS_helper(E_squared, "E[x]^2");
    DEBUG_PLOTS_helper(img_squared, "x^2");
    DEBUG_PLOTS_helper(E_of_x_squared, "E[x^2]");
    DEBUG_PLOTS_helper(var, "E[x^2] - E[x]^2");
    DEBUG_PLOTS_helper(std, "std = sqrt{E[x^2] - E[x]^2}");
    #endif


    // threshold to find pixels of interest
    float deviation_from_std = 2.5;
    cv::Mat deviations = calcMultiplyScalar(std, deviation_from_std);
    cv::Mat threshold_values = calcAddition(E_of_x, deviations);
    cv::Mat outliers = calcOutliers(img, threshold_values);

    #ifdef DEBUG_PLOTS
    DEBUG_PLOTS_helper(threshold_values, "threshold values");
    DEBUG_PLOTS_helper(outliers, "outliers");
    #endif

    cv::destroyAllWindows();
    return 0;
}