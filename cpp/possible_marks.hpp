// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#ifndef POSSIBLE_MARKS_HPP
#define POSSIBLE_MARKS_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "auxiliary.hpp"

// Class for representing a contour which might be representing a possible bracelet mark (for a later analysis)
class PossibleMark {
public:
  
  // Member variables
  std::vector<cv::Point> contour;
  cv::Rect boundingRect;
  unsigned int intBoundingRectArea;
  int intCenterX;
  int intCenterY;
  int intCenterX_r;
  int intCenterY_r;
  double dblDiagonalSize;
  double dblAspectRatio;
  unsigned int MinPixelWidth;
  unsigned int MaxPixelWidth;
  unsigned int MinPixelHeight;
  unsigned int MaxPixelHeight;
  double MinAspectRatio;
  double MaxAspectRatio;
  unsigned int MinPixelArea;
  unsigned int MaxPixelArea;
  double dblExtent;
  double MinExtent;
  double MaxExtent;

  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. --
  // Constructor
  PossibleMark(std::vector<cv::Point> _contour, unsigned int _MinPixelWidth, unsigned int _MaxPixelWidth,
               unsigned int _MinPixelHeight, unsigned int _MaxPixelHeight, double _MinAspectRatio, double _MaxAspectRatio,
               unsigned int _MinPixelArea, unsigned int _MaxPixelArea, double _MinExtent, double _MaxExtent);
    
  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. --
  // A 'first pass' over the contour, to see if it could be representing a character
  bool checkIfPossibleMark();
    
  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. --
  // Substraction operator overloading:
  double operator-(const PossibleMark& other);

  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. --
  // Print mark method:
  std::string to_string();
};

#endif //POSSIBLE_MARKS_HPP

