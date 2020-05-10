// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#ifndef PREPROCESS_HPP
#define PREPROCESS_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include "auxiliary.hpp"

const cv::Size GAUSSIAN_SMOOTH_FILTER_SIZE = cv::Size(5, 5);
const int ADAPTIVE_THRESH_BLOCK_SIZE = 19;
const int ADAPTIVE_THRESH_WEIGHT = 9;

// Pre-processing (CSC --> contrast --> blur --> threshold):
void preprocess(cv::Mat &imgOriginal, 
                cv::Mat &imgGrayscale, 
                cv::Mat &imgThresh, 
                std::string PreprocessCvcSel, 
                std::string PreprocessMode, 
                uint_xy_t PreprocessGaussKernel, 
                unsigned int PreprocessThreshBlockSize, 
                unsigned int PreprocessThreshweight, 
                uint_xy_t PreprocessMorphKernel,
                unsigned int PreprocessMedianBlurKernel, 
                unsigned int PreprocessCannyThr); 

// Morphological filtering for increasing contrast: OutputImage = InputImage + TopHat - BlackHat
cv::Mat maximizeContrast(cv::Mat &imgGrayscale, 
                         uint_xy_t PreprocessMorphKernel);

// Image enhancement, applies Warming effect (+CLAHE) and Saturation effect (+Gamma)
cv::Mat imageEnhancement(cv::Mat &imgOriginal, 
                         double clahe_clipLimit, 
                         int clahe_tileGridSize, 
                         double gamma, 
                         bool debugMode);

#endif //PREPROCESS_HPP

