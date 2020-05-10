// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#include "possible_marks.hpp"

PossibleMark :: PossibleMark(std::vector<cv::Point> _contour, unsigned int _MinPixelWidth, unsigned int _MaxPixelWidth,
                             unsigned int _MinPixelHeight, unsigned int _MaxPixelHeight, double _MinAspectRatio, double _MaxAspectRatio,
                             unsigned int _MinPixelArea, unsigned int _MaxPixelArea, double _MinExtent, double _MaxExtent) {
  
  contour = _contour;

  boundingRect = cv::boundingRect(contour);

  intBoundingRectArea = boundingRect.width * boundingRect.height;
  dblExtent = (double)(cv::contourArea(contour)) / intBoundingRectArea;

  intCenterX = (boundingRect.x + boundingRect.x + boundingRect.width) / 2;
  intCenterY = (boundingRect.y + boundingRect.y + boundingRect.height) / 2;
        
  intCenterX_r = 0;
  intCenterY_r = 0;

  dblDiagonalSize = sqrt(pow(boundingRect.width, 2) + pow(boundingRect.height, 2));

  dblAspectRatio = (double)boundingRect.width / (double)boundingRect.height;
  
  MinPixelWidth = _MinPixelWidth;
  MaxPixelWidth = _MaxPixelWidth;
  MinPixelHeight = _MinPixelHeight;
  MaxPixelHeight = _MaxPixelHeight;
  MinAspectRatio = _MinAspectRatio;
  MaxAspectRatio = _MaxAspectRatio;
  MinPixelArea = _MinPixelArea;
  MaxPixelArea = _MaxPixelArea;
  MinExtent = _MinExtent;
  MaxExtent = _MaxExtent;
}

// ------------------------------------------------------------------------------------------------------------------------------
bool PossibleMark :: checkIfPossibleMark() {
    
  return (boundingRect.area() > MinPixelArea &&
          boundingRect.area() < MaxPixelArea &&
          boundingRect.width > MinPixelWidth &&
          boundingRect.width < MaxPixelWidth &&
          boundingRect.height > MinPixelHeight &&
          boundingRect.height < MaxPixelHeight &&
          dblAspectRatio > MinAspectRatio &&
          dblAspectRatio < MaxAspectRatio &&
          dblExtent > MinExtent &&
          dblExtent < MaxExtent);
}

// ------------------------------------------------------------------------------------------------------------------------------
double PossibleMark :: operator-(const PossibleMark& other) {
  
  cv::Point2f p1 = cv::Point2f(intCenterX, other.intCenterX);
  cv::Point2f p2 = cv::Point2f(intCenterY, other.intCenterY);

  return distance_mse(p1,p2);
}

// ------------------------------------------------------------------------------------------------------------------------------
std::string PossibleMark::to_string() {
        
  char buffer[1000];

  sprintf(buffer, "(x,y, xR,yR, w,h, BndArea,AspRatio,extent)=(%d,%d, %d,%d, %d,%d, %d,%.2f.%.2f)",
                  intCenterX, intCenterY, intCenterX_r, intCenterY_r, boundingRect.width,
                  boundingRect.height, intBoundingRectArea, dblAspectRatio, dblExtent);

  std::string str(buffer);

  return str;
}

