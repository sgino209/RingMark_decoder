// RingMark-Decoder algo app
// (c) Shahar Gino, May-2020, sgino209@gmail.com

#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP

#include <list>
#include <math.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef struct {
  unsigned int x1;
  unsigned int x2;
  unsigned int x3;
  unsigned int x4;
  unsigned int len;
} uint_x4_t;

typedef struct {    
  std::string ImageFile;
  std::string TemplateFile;
  unsigned int MarksNumExternal;
  unsigned int MarksNumInternal;
  double TemplateMatchingThr;
  double TemplateMatchingMinScale;
  double TemplateMatchingMaxScale;
  double TemplateToMarkXScale;
  double TemplateToMarkYScale;
  double CropMarginR;
  unsigned int CropMarginX;
  unsigned int CropMarginY;
  double RoiR1Scale;
  double RoiR2Scale;
  unsigned int MarkDecodeThr;
  uint_x4_t ROI;
  bool debugMode;
} args_t;

std::string decode_frame(args_t args);

class PossibleMark;

// Resizing values:
const cv::Size resizingVec1(1600,1200);
const cv::Size resizingVec2(1200,1600);

// Common color values:
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 255.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

// Crop a given ROI from a given Image; ROI=(startX,startY,W,H)
cv::Mat crop_roi_from_image(cv::Mat &frame, uint_x4_t roi, bool verbose);

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

// Auxiliary function - mimics Python's numpy linspace method:
std::vector<double> linspace(double start_in, double end_in, int num_in, bool flip);

// Auxiliary function - converts binary string to hex string
std::string bin2hex(std::string binstr);

#endif //AUXILIARY_HPP

