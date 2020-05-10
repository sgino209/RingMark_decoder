// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#ifndef DETECT_MARKS_HPP
#define DETECT_MARKS_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "auxiliary.hpp"
#include "possible_marks.hpp"
#include "decode_marks.hpp"

class PossibleMark;

typedef std::vector<PossibleMark> mark_list_t;

typedef struct {
  double rotation_angle_deg;
  unsigned int centroid_x;
  unsigned int centroid_y;
} rotation_align_t;

// Find all possible bracelet marks (finds all contours that could be representing marks):
double find_possible_marks(mark_list_t &possible_marks_final, cv::Mat &frame_thresh, unsigned int MinPixelWidth, unsigned int MaxPixelWidth, 
                           unsigned int MinPixelHeight, unsigned int MaxPixelHeight, double MinAspectRatio, double MaxAspectRatio, 
                           unsigned int MinPixelArea, unsigned int MaxPixelArea, double MinExtent, double MaxExtent, double MaxDrift,
                           unsigned int PerspectiveMode, std::string FindContoursMode, unsigned int HoughParams1, unsigned int HoughParams2,
                           unsigned int HoughParams3,unsigned int HoughParams4,unsigned int HoughParams5,unsigned int HoughParams6, bool debugMode);

// Remove outliers in a given 2D data-set of possible-marks
mark_list_t remove_outliers(mark_list_t possible_marks_list, double MaxDrift, bool debugMode);

  // SVD decomposition for rotation alignment, adds intCenterX/Y_r attributes and returns angle and centroid estimation:
rotation_align_t rotation_alignment(mark_list_t &possible_marks_list, bool debugMode);

// Wrapper function for Perspective Alignment:
void perspective_alignment(mark_list_t &possible_marks_list, unsigned int PerspectiveMode, double rotation_angle_deg, bool debugMode);
void perspective_alignment_opt0(mark_list_t &possible_marks_list, double rotation_angle_deg, bool debugMode);
void perspective_alignment_opt1(mark_list_t &possible_marks_list, bool debugMode);

// Auxiliary function - Gets a Circle (x,y,radius) and returns the corresponding contour (array of x,y points)
std::vector<cv::Point> circle_to_contour(cv::Vec3f circle, unsigned int points_per_contour, float resize_factor);

// Auxiliary function - calculates median for a given vector:
template <class T>
double median_calc(std::vector<T> data_vec);

// Auxiliary function - prints a point vector:
template <class T>
void print_point_vec(std::vector<T> data_vec);

// Auxiliary function - prints a mark vector:
template <class T>
void print_mark_vec(std::vector<T> data_vec);

#endif //DETECT_MARKS_HPP

