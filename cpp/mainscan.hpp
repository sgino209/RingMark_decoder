// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

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

void set_xy(uint_xy_t *d, std::string str) {
  std::size_t found1 = str.find("(");
  std::size_t found2 = str.find(",");
  std::size_t found3 = str.find(")");
  if ((found1 != std::string::npos) && (found2 != std::string::npos)) {
    std::string x_str = str.substr(found1+1, found2-found1-1);
    d->x = atoi(x_str.c_str());
  }
  if ((found2 != std::string::npos) && (found3 != std::string::npos)) {
    std::string y_str = str.substr(found2+1, found3-found2-1);
    d->y = atoi(y_str.c_str());
  }
}

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

void set_x6(uint_x6_t *d, std::string str) {
  std::size_t found1 = str.find("(");
  std::size_t found2 = str.find(",", found1+1);
  std::size_t found3 = str.find(",", found2+1);
  std::size_t found4 = str.find(",", found3+1);
  std::size_t found5 = str.find(",", found4+1);
  std::size_t found6 = str.find(",", found5+1);
  std::size_t found7 = str.find(")");
  d->x1 = atoi(str.substr(found1+1, found2-found1-1).c_str());
  d->x2 = atoi(str.substr(found2+1, found3-found2-1).c_str());
  d->x3 = atoi(str.substr(found3+1, found4-found3-1).c_str());
  d->x4 = atoi(str.substr(found4+1, found5-found4-1).c_str());
  d->x5 = atoi(str.substr(found5+1, found6-found5-1).c_str());
  d->x6 = atoi(str.substr(found6+1, found7-found6-1).c_str());
  d->len = 6;
}

typedef enum {
  ARG_PREPROCESSCVCSEL = 0,
  ARG_PREPROCESSMODE,
  ARG_PREPROCESSGAUSSKERNEL,
  ARG_PREPROCESSTHRESHBLOCKSIZE,
  ARG_PREPROCESSTHRESHWEIGHT,
  ARG_PREPROCESSMORPHKERNEL,
  ARG_PREPROCESSMEDIANBLURKERNEL,
  ARG_PREPROCESSCANNYTHR,
  ARG_IMGENHANCEMENTEN,
  ARG_MINPIXELWIDTH,
  ARG_MAXPIXELWIDTH,
  ARG_MINPIXELHEIGHT,
  ARG_MAXPIXELHEIGHT,
  ARG_MINASPECTRATIO,
  ARG_MAXASPECTRATIO,
  ARG_MINPIXELAREA,
  ARG_MAXPIXELAREA,
  ARG_MAXDRIFT,
  ARG_MARKROWS,
  ARG_MARKCOLS,
  ARG_ROI,
  ARG_FINDCONTOURSMODE,
  ARG_HOUGHPARAMS,
  ARG_PERSPECTIVEMODE,
  ARG_MINEXTENT,
  ARG_MAXEXTENT,
  ARG_DEBUG,
  ARGS_NUM
} bracelet_decoder_arg_t;

const struct option longopts[] =
{
  {"version",                    no_argument,       0, 'v' },
  {"help",                       no_argument,       0, 'h' },
  {"image",                      required_argument, 0, 'i' },
  {"PreprocessCvcSel",           required_argument, 0, ARG_PREPROCESSCVCSEL },
  {"PreprocessMode",             required_argument, 0, ARG_PREPROCESSMODE },
  {"PreprocessGaussKernel",      required_argument, 0, ARG_PREPROCESSGAUSSKERNEL },
  {"PreprocessThreshBlockSize",  required_argument, 0, ARG_PREPROCESSTHRESHBLOCKSIZE },
  {"PreprocessThreshweight",     required_argument, 0, ARG_PREPROCESSTHRESHWEIGHT },
  {"PreprocessMorphKernel",      required_argument, 0, ARG_PREPROCESSMORPHKERNEL },
  {"PreprocessMedianBlurKernel", required_argument, 0, ARG_PREPROCESSMEDIANBLURKERNEL },
  {"PreprocessCannyThr",         required_argument, 0, ARG_PREPROCESSCANNYTHR },
  {"imgEnhancementEn",           no_argument,       0, ARG_IMGENHANCEMENTEN },
  {"MinPixelWidth",              required_argument, 0, ARG_MINPIXELWIDTH },
  {"MaxPixelWidth",              required_argument, 0, ARG_MAXPIXELWIDTH },
  {"MinPixelHeight",             required_argument, 0, ARG_MINPIXELHEIGHT },
  {"MaxPixelHeight",             required_argument, 0, ARG_MAXPIXELHEIGHT },
  {"MinAspectRatio",             required_argument, 0, ARG_MINASPECTRATIO },
  {"MaxAspectRatio",             required_argument, 0, ARG_MAXASPECTRATIO },
  {"MinPixelArea",               required_argument, 0, ARG_MINPIXELAREA },
  {"MaxPixelArea",               required_argument, 0, ARG_MAXPIXELAREA },
  {"MinExtent",                  required_argument, 0, ARG_MINEXTENT },
  {"MaxExtent",                  required_argument, 0, ARG_MAXEXTENT },
  {"MaxDrift",                   required_argument, 0, ARG_MAXDRIFT },
  {"MarksRows",                  required_argument, 0, ARG_MARKROWS },
  {"MarksCols",                  required_argument, 0, ARG_MARKCOLS },
  {"ROI",                        required_argument, 0, ARG_ROI },
  {"FindContoursMode ",          required_argument, 0, ARG_FINDCONTOURSMODE },
  {"HoughParams",                required_argument, 0, ARG_HOUGHPARAMS },
  {"PerspectiveMode",            required_argument, 0, ARG_PERSPECTIVEMODE },
  {"debug",                      no_argument,       0, ARG_DEBUG },
  { NULL, 0, NULL, 0 }
};

// Main function:
int main(int argc, char** argv);

// Auxiliary function - returns folder name for results to be stored at results_%d%m%y_%H%M%S_imgFile:
std::string get_envpath(std::string ImageFile);

// Auxiliary function - prints usage information:
void usage(char *script_name);

#endif	// MAINSCAN_HPP

