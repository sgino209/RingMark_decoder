// RingMark-Decoder algo app
// (c) Shahar Gino, May-2020, sgino209@gmail.com

#ifndef MAINSCAN_HPP
#define MAINSCAN_HPP

#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <dirent.h>
#include <unistd.h>
#include "auxiliary.hpp"

#define no_argument 0
#define required_argument 1
#define optional_argument 2

void set_x4(uint_x4_t *d, std::string str) {
  std::size_t found1 = str.find("(");
  std::size_t found2 = str.find(",");
  std::size_t found3 = 0;
  std::size_t found4 = 0;
  std::size_t found5 = str.find(")");
  std::string x_str;
  
  d->len = 2;

  if (found2 != std::string::npos) {
    found3 = str.find(",", found2 + 1);
  }
  if (found3 != std::string::npos) {
    found4 = str.find(",", found3 + 1);
  }
   
  if ((found1 != std::string::npos) && (found2 != std::string::npos)) {
    std::string x_str = str.substr(found1+1, found2-found1-1);
    d->x1 = atoi(x_str.c_str());
  }
  if (found2 != std::string::npos) {
    std::string x_str = str.substr(found2+1, found2-found1-1);
    d->x2 = atoi(x_str.c_str());
    d->len = 2;
  }
  if ((found2 != std::string::npos) && (found3 != std::string::npos)) {
    std::string x_str = str.substr(found2+1, found3-found2-1);
    d->x2 = atoi(x_str.c_str());
  }
  if (found3 != std::string::npos) {
    std::string x_str = str.substr(found3+1, found4-found3-1);
    d->x3 = atoi(x_str.c_str());
    d->len = 3;
  }
  if ((found3 != std::string::npos) && (found4 != std::string::npos)) {
    std::string x_str = str.substr(found4+1, found5-found4-1);
    d->x4 = atoi(x_str.c_str());
    d->len = 4;
  }
}

typedef enum {
  ARG_TEMPLATE_FILE = 0,
  ARG_MARKS_NUM_EXTERNAL,
  ARG_MARKS_NUM_INTERNAL,
  ARG_TEMPLATE_MATCHING_THR,
  ARG_TEMPLATE_MATCHING_MIN_SCALE,
  ARG_TEMPLATE_MATCHING_MAX_SCALE,
  ARG_TEMPLATE_TO_MARK_X_SCALE,
  ARG_TEMPLATE_TO_MARK_Y_SCALE,
  ARG_CROP_MARGIN_R,
  ARG_CROP_MARGIN_X,
  ARG_CROP_MARGIN_Y,
  ARG_ROI_R1_SCALE,
  ARG_ROI_R2_SCALE,
  ARG_MARK_DECODE_THR,
  ARG_ROI,
  ARG_DEBUG,
  ARGS_NUM
} ringmark_decoder_arg_t;

const struct option longopts[] =
{
  {"version",                    no_argument,       0, 'v' },
  {"help",                       no_argument,       0, 'h' },
  {"image",                      required_argument, 0, 'i' },
  {"TemplateFile",               required_argument, 0, ARG_TEMPLATE_FILE},
  {"MarksNumExternal",           required_argument, 0, ARG_MARKS_NUM_EXTERNAL},
  {"MarksNumInternal",           required_argument, 0, ARG_MARKS_NUM_INTERNAL},
  {"TemplateMatchingThr",        required_argument, 0, ARG_TEMPLATE_MATCHING_THR},
  {"TemplateMatchingMinScale",   required_argument, 0, ARG_TEMPLATE_MATCHING_MIN_SCALE},
  {"TemplateMatchingMaxScale",   required_argument, 0, ARG_TEMPLATE_MATCHING_MAX_SCALE},
  {"TemplateToMarkXScale",       required_argument, 0, ARG_TEMPLATE_TO_MARK_X_SCALE},
  {"TemplateToMarkYScale",       required_argument, 0, ARG_TEMPLATE_TO_MARK_Y_SCALE},
  {"CropMarginR",                required_argument, 0, ARG_CROP_MARGIN_R},
  {"CropMarginX",                required_argument, 0, ARG_CROP_MARGIN_X},
  {"CropMarginY",                required_argument, 0, ARG_CROP_MARGIN_Y},
  {"RoiR1Scale",                 required_argument, 0, ARG_ROI_R1_SCALE},
  {"RoiR2Scale",                 required_argument, 0, ARG_ROI_R2_SCALE},
  {"MarkDecodeThr",              required_argument, 0, ARG_MARK_DECODE_THR},
  {"ROI",                        required_argument, 0, ARG_ROI},
  {"debug",                      no_argument,       0, ARG_DEBUG},
  { NULL, 0, NULL, 0 }
};

// Main function:
int main(int argc, char** argv);

// Auxiliary function - returns folder name for results to be stored at results_%d%m%y_%H%M%S_imgFile:
std::string get_envpath(std::string ImageFile);

// Auxiliary function - prints usage information:
void usage(char *script_name);

#endif	// MAINSCAN_HPP

