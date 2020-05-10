// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "detect_marks.hpp"
#include "decode_marks.hpp"

typedef struct {
  unsigned int x;
  unsigned int y;
} uint_xy_t;

typedef struct {
  unsigned int x1;
  unsigned int x2;
  unsigned int x3;
  unsigned int x4;
  unsigned int len;
} uint_x4_t;

typedef struct {
  unsigned int x1;
  unsigned int x2;
  unsigned int x3;
  unsigned int x4;
  unsigned int x5;
  unsigned int x6;
  unsigned int len;
} uint_x6_t;

typedef struct {    
  std::string ImageFile;
  cv::Mat ImageMat;
  std::string PreprocessCvcSel;
  std::string PreprocessMode;
  uint_xy_t PreprocessGaussKernel;
  unsigned int PreprocessThreshBlockSize;
  unsigned int PreprocessThreshweight;
  uint_xy_t PreprocessMorphKernel;
  unsigned int PreprocessMedianBlurKernel;
  unsigned int PreprocessCannyThr;
  unsigned int imgEnhancementEn;
  unsigned int MinPixelWidth;
  unsigned int MaxPixelWidth;
  unsigned int MinPixelHeight;
  unsigned int MaxPixelHeight;
  double MinAspectRatio;
  double MaxAspectRatio;
  unsigned int MinPixelArea;
  unsigned int MaxPixelArea;
  double MinExtent;
  double MaxExtent;
  double MaxDrift;
  unsigned int MarksRows;
  unsigned int MarksCols;
  uint_x4_t ROI;
  unsigned int PerspectiveMode;
  std::string FindContoursMode;
  uint_x6_t HoughParams;
  bool debugMode;
} args_t;

std::string decode_frame(args_t args);

class PossibleMark;

// Resizing values:
const cv::Size resizingVec1(1600,1200);
const cv::Size resizingVec2(1200,1600);
const cv::Size resizingVec3(1120,840);

// Common color values:
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 255.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

// Crop a given ROI from a given Image; ROI=(startX,startY,W,H)
cv::Mat crop_roi_from_image(cv::Mat &frame, uint_x4_t roi);

// Auxiliary function for drawing a given ROI on a given frame
cv::Mat draw_roi(cv::Mat &frame, uint_x4_t roi);

// Auxiliary function for a info printout
void generic_message(std::string message, std::string severity);
void info(std::string message);
void debug(std::string message);
void error(std::string message);

// Auxiliary function for a MSE calculation
double distance_mse(cv::Point2f p1, cv::Point2f p2);

// Auxiliary function - loads default parameters:
args_t load_default_args();
void print_args(args_t args);

#endif //AUXILIARY_HPP

