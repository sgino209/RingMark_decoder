// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#ifndef DECODE_MARKS_HPP
#define DECODE_MARKS_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "auxiliary.hpp"
#include "possible_marks.hpp"

class PossibleMark;

typedef std::vector<PossibleMark> mark_list_t;

// Marks decoding, based on marks positioning:
std::string decode_marks(mark_list_t marks_list, unsigned int MarksRows, unsigned int MarksCols, cv::Size frame_shape, double rotation_angle_deg, bool debugMode);

// Auxiliary function - Seek for a mark existence in a WxH neighbourhood around (x,y) coordinate:
int seek_for_mark(unsigned int x, unsigned int y, mark_list_t marks, unsigned int w, unsigned int h);

#endif //DECODE_MARKS_HPP

