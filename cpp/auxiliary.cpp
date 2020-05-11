// RingMark-Decoder algo app
// (c) Shahar Gino, May-2020, sgino209@gmail.com

#include "auxiliary.hpp"

extern void set_x4(uint_x4_t *d, std::string str);

std::string decode_frame(args_t args) {

  std::string code;
  cv::Mat frame_orig, img_gray, crop_img, crop_img_gray, crop_img_bw;
  cv::Mat template_orig, template_gray, resized;
  cv::Mat frame_thresh, imgCropped, imgResized;
  cv::Point minLoc, maxLoc, matchLoc;
  double minVal, maxVal, x, y, r;
  int w, h, tw, th, res_cols, res_rows;
  std::string mark_ext_val_list("deadbeef");
  std::string mark_int_val_list("cafecafe");
  std::string mark_ext_str, mark_int_str;
  bool found=false;
  char buffer[1000];
  
  if (args.debugMode) {
    info("Starting C++ code");
  }
 
  // ---------------------------------------------------------------------------------
  // Load input image:
  frame_orig = cv::imread(args.ImageFile);
  
  // ---------------------------------------------------------------------------------
  // Load template image:
  template_orig = cv::imread(args.TemplateFile);

  // ---------------------------------------------------------------------------------
  // Resizing preparations:
  cv::Size s = frame_orig.size();
  cv::Size resizingVec;
  resizingVec = resizingVec1;
  if (s.width < s.height) {
      resizingVec = resizingVec2;
  }
  
  // ---------------------------------------------------------------------------------
  // Resizing and ROI cropping:
  cv::resize(frame_orig.clone(), imgResized, resizingVec);
  imgCropped = crop_roi_from_image(imgResized, args.ROI, true);

  // ---------------------------------------------------------------------------------
  // Color space conversion:
  cv::cvtColor(imgCropped, img_gray, cv::COLOR_BGR2GRAY);
  cv::cvtColor(template_orig, template_gray, cv::COLOR_BGR2GRAY);

  // ---------------------------------------------------------------------------------
  // Dimensions extraction:
  w = imgCropped.size().width;
  h = imgCropped.size().height;
  tw = template_orig.size().width;
  th = template_orig.size().height;

  // ---------------------------------------------------------------------------------
  // Template Matching, multi-scale, end up with ROI extraction (cropped):
  img_gray.copyTo(crop_img);

  std::vector<double> scale_vec = linspace(args.TemplateMatchingMinScale, args.TemplateMatchingMaxScale, 20, true);
  for (const auto& scale: scale_vec) {
   
    cv::resize(template_gray, resized, cv::Size(int(tw*scale), int(th*scale)));
    
    r = scale;

    /// Create the result matrix
    cv::Mat res;
    res_cols =  img_gray.cols - resized.cols + 1;
    res_rows = img_gray.rows - resized.rows + 1;
    res.create(res_rows, res_cols, CV_32FC1);
    
    // Apply Template Matching
    int match_method = cv::TM_CCOEFF_NORMED; 
    cv::matchTemplate(img_gray, resized, res, match_method);

    // Localizing the best match with minMaxLoc
    cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    matchLoc = (match_method  == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED) ? minLoc : maxLoc;

    if (maxVal > args.TemplateMatchingThr) {
 
        x = matchLoc.x + std::ceil(args.TemplateToMarkXScale*tw*scale);
        y = matchLoc.y + std::ceil(args.TemplateToMarkYScale*th*scale);
        r = std::ceil(tw*scale/2);

        uint_x4_t template_roi;
        template_roi.x1 = x - std::ceil(args.CropMarginR*r) - args.CropMarginX;
        template_roi.x2 = y - std::ceil(args.CropMarginR*r) - args.CropMarginY;
        template_roi.x3 = 2*std::ceil(args.CropMarginR*r);
        template_roi.x4 = 2*std::ceil(args.CropMarginR*r);
        template_roi.len = 4;
        crop_img_gray = crop_roi_from_image(img_gray, template_roi, false);
   
        cv::threshold(crop_img_gray, crop_img_bw, 127, 255, cv::THRESH_BINARY_INV);

        found = true;
        break;
    }
  }

  if (found) {
      
    // ---------------------------------------------------------------------------------
    // Ring Marks extraction:
    int roi_x = std::ceil(crop_img_bw.size().width/2);
    int roi_y = std::ceil(crop_img_bw.size().height/2);
    int roi_r1 = std::ceil(args.RoiR1Scale * r);
    int roi_r2 = std::ceil(args.RoiR2Scale * r);
  
    std::list<uint_x4_t> mark_ext_list, mark_int_list;

    std::vector<double> teta_vec_ext = linspace(0, 2*M_PI, args.MarksNumExternal+1, false);
    for (const auto& teta: teta_vec_ext) {
      uint_x4_t el;
      el.x1 = std::ceil(roi_x + roi_r1 * std::cos(teta));
      el.x2 = std::ceil(roi_y + roi_r1 * std::sin(teta));
      el.x3 = std::ceil(roi_x + roi_r1 * std::cos(teta)*1.15);
      el.x4 = std::ceil(roi_y + roi_r1 * std::sin(teta)*1.15);
      el.len = 4;
      mark_ext_list.push_back(el);
    }
    mark_ext_list.pop_back();
    
    std::vector<double> teta_vec_int = linspace(0, 2*M_PI, args.MarksNumInternal+1, false);
    for (const auto& teta: teta_vec_int) {
      uint_x4_t el;
      el.x1 = std::ceil(roi_x + roi_r2 * std::cos(teta));
      el.x2 = std::ceil(roi_y + roi_r2 * std::sin(teta));
      el.x3 = std::ceil(roi_x + roi_r2 * std::cos(teta)*0.85);
      el.x4 = std::ceil(roi_y + roi_r2 * std::sin(teta)*0.85);
      el.len = 4;
      mark_int_list.push_back(el);
    }
    mark_int_list.pop_back();

    // ---------------------------------------------------------------------------------
    // Ring code decoding:
    std::list<int> mark_ext_val_list;
    for (auto const& mark: mark_ext_list) {
      uint_x4_t mask_roi;
      mask_roi.x1 = mark.x1;
      mask_roi.x2 = mark.x2;
      mask_roi.x3 = 4;
      mask_roi.x4 = 4;
      mask_roi.len = 4;
      cv::Mat mask_roi_cropped = crop_roi_from_image(crop_img_bw, mask_roi, false);
      double mark_mean = double(cv::mean(mask_roi_cropped).val[0]);
      mark_ext_val_list.push_back(int(mark_mean) < args.MarkDecodeThr);
    }
    
    std::list<int> mark_int_val_list;
    for (auto const& mark: mark_int_list) {
      uint_x4_t mask_roi;
      mask_roi.x1 = mark.x1;
      mask_roi.x2 = mark.x2;
      mask_roi.x3 = 4;
      mask_roi.x4 = 4;
      mask_roi.len = 4;
      cv::Mat mask_roi_cropped = crop_roi_from_image(crop_img_bw, mask_roi, false);
      double mark_mean = double(cv::mean(mask_roi_cropped).val[0]);
      mark_int_val_list.push_back(int(mark_mean) < args.MarkDecodeThr);
    }
        
    for (auto const& k: mark_ext_val_list) {
        mark_ext_str += std::to_string(k);
    }
    
    for (auto const& k: mark_int_val_list) {
        mark_int_str += std::to_string(k);
    }
  
    std::string code_str = mark_ext_str + mark_int_str;
  
    code = "0x" + bin2hex(code_str);
  
    // ---------------------------------------------------------------------------------
    // Debug
    if (args.debugMode) {
    
        sprintf(buffer, "External Ring: %s", mark_ext_str.c_str());
        debug(buffer);
        sprintf(buffer, "Internal Ring: %s", mark_int_str.c_str());
        debug(buffer);
        sprintf(buffer, "Merged Code (Int+Ext): %s", code_str.c_str());
        debug(buffer);
        
        cv::Mat tm_result;
        imgCropped.copyTo(tm_result);
        cv::rectangle(tm_result, matchLoc, cv::Point(matchLoc.x + resized.cols , matchLoc.y + resized.rows), cv::Scalar(0,255,0), 2, 8, 0);
        cv::rectangle(tm_result, cv::Point(x-5, y-5), cv::Point(x+5, y+5), cv::Scalar(0,128,255), -1);
        cv::circle(tm_result, cv::Point(x, y), r, cv::Scalar(255, 255, 0), 4);
        cv::imwrite("tm_result.jpg", tm_result);
        
        cv::Mat cropped_bw_result;
        crop_img_bw.copyTo(cropped_bw_result);
        cv::rectangle(cropped_bw_result, cv::Point(roi_x-2, roi_y-2), cv::Point(roi_x+2, roi_y+2), 128, -1);
        cv::circle(cropped_bw_result, cv::Point(roi_x, roi_y), roi_r1, 128, 1);
        cv::circle(cropped_bw_result, cv::Point(roi_x, roi_y), roi_r2, 128, 1);
        int k=0;
        for (auto const& mark: mark_ext_list) {
          cv::rectangle(cropped_bw_result, cv::Rect(mark.x1, mark.x2, 4, 4), 128, -1);
          cv::putText(cropped_bw_result, std::to_string(k), cv::Point(mark.x3-4, mark.x4+2), cv::FONT_HERSHEY_SIMPLEX, 0.2, 128, 1);
          k++;
        }
        k=0;
        for (auto const& mark: mark_int_list) {
          cv::rectangle(cropped_bw_result, cv::Rect(mark.x1, mark.x2, 4, 4), 128, -1);
          cv::putText(cropped_bw_result, std::to_string(k), cv::Point(mark.x3-4, mark.x4+2), cv::FONT_HERSHEY_SIMPLEX, 0.2, 128, 1);
          k++;
        }
        cv::imwrite("tm_cropped_bw.jpg", cropped_bw_result);
    }
  }

  return code.c_str();
}

// ------------------------------------------------------------------------------------------------------------------------------
void generic_message(std::string message, std::string severity) {
    
    char buffer[1000];
    sprintf(buffer, "%s: %s", severity.c_str(), message.c_str());
    
    std::cout << buffer << std::endl << std::flush;
}

void info(std::string message)  { generic_message(message, "INFO");  }
void debug(std::string message) { generic_message(message, "DEBUG"); }
void error(std::string message) { generic_message(message, "ERROR"); }

// ------------------------------------------------------------------------------------------------------------------------------
cv::Mat draw_roi(cv::Mat &frame, uint_x4_t roi) {
    
  cv::Size s = frame.size();
  int imgH = s.height;
  int imgW = s.width;
  unsigned int x1=0, y1=0, x2=0, y2=0;

  if (roi.len >= 3) {
    x1 = roi.x1;
    y1 = roi.x2;
    x2 = roi.x3;
    y2 = (roi.len == 4) ? roi.x4 : roi.x3;
  }
  else if (roi.x1 == 0) {
      x2 = imgW;
      y2 = imgH;
  }

  cv::rectangle(frame, cv::Rect(x1, y1, x2, y2), SCALAR_RED, 2);
  cv::putText(frame, "ROI", cv::Point(x1 + (int)(0.45 * x2), y1 - 10) , cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_RED, 2);

  return frame;
}

// ------------------------------------------------------------------------------------------------------------------------------
cv::Mat crop_roi_from_image(cv::Mat &frame, uint_x4_t roi, bool verbose) {
    
    cv::Mat imgCropped;
    
    if (roi.len == 2 && roi.x1 == 0 && roi.x2 == 0) {

      imgCropped = frame;
    }
    else {
        
      unsigned int roiW = roi.x3;
      unsigned int roiH = (roi.len == 4) ? roi.x4 : roi.x3;

      double roiCx = roi.x1 + roiW / 2.0;
      double roiCy = roi.x2 + roiH / 2.0;
   
      //if (frame.dims == 2) {
      //  cv::cvtColor(frame.clone(), frame, cv::COLOR_RGB2BGR);    
      //}
      cv::getRectSubPix(frame, 
                        cv::Size2f((float)roiW, (float)roiH),
                        cv::Point2f((float)roiCx, (float)roiCy),
                        imgCropped);

      if (verbose) {
        char buffer[1000];
        sprintf(buffer, "ROI size: (Cx,Cy)=(%.2f,%.2f), WxH=%dx%d", roiCx, roiCy, roiW, roiH);
        info(buffer);
      }
    }
     
    return imgCropped;
}

// ------------------------------------------------------------------------------------------------------------------------------
// Auxiliary function for a MSE calculation
double distance_mse(cv::Point2f p1, cv::Point2f p2) {

    return (double)(sqrt(pow((p1.x - p2.x),2) + pow((p1.y - p2.y),2)));
}

// ------------------------------------------------------------------------------------------------------------------------------
args_t load_default_args() {

  args_t args;

  args.ImageFile = "../data/image0.jpg";
  args.TemplateFile = "../data/template_m.png";
  args.MarksNumExternal = 40;
  args.MarksNumInternal = 34;
  args.TemplateMatchingThr = 0.83;
  args.TemplateMatchingMinScale = 0.4;
  args.TemplateMatchingMaxScale = 1.2;
  args.TemplateToMarkXScale = 1.69;
  args.TemplateToMarkYScale = 0.50;
  args.CropMarginR = 1.2;
  args.CropMarginX = 2;
  args.CropMarginY = 0;
  args.RoiR1Scale = 0.88;
  args.RoiR2Scale = 0.75;
  args.MarkDecodeThr = 128;
  set_x4(&args.ROI, "(0,0)");
  args.debugMode = false;

  return args;
}

// ------------------------------------------------------------------------------------------------------------------------------
void print_args(args_t args) {

  printf("args.ImageFile = %s\n", args.ImageFile.c_str());
  printf("args.TemplateFile = %s\n", args.TemplateFile.c_str());
  printf("args.MarksNumExternal = %d\n", args.MarksNumExternal);
  printf("args.MarksNumInternal = %d\n", args.MarksNumInternal);
  printf("args.TemplateMatchingThr = %.2f\n", args.TemplateMatchingThr);
  printf("args.TemplateMatchingMinScale = %.2f\n", args.TemplateMatchingMinScale);
  printf("args.TemplateMatchingMaxScale = %.2f\n", args.TemplateMatchingMaxScale);
  printf("args.TemplateToMarkXScale = %.2f\n", args.TemplateToMarkXScale);
  printf("args.TemplateToMarkYScale = %.2f\n", args.TemplateToMarkYScale);
  printf("args.CropMarginR = %.2f\n", args.CropMarginR);
  printf("args.CropMarginX = %d\n", args.CropMarginX);
  printf("args.CropMarginY = %d\n", args.CropMarginY);
  printf("args.RoiR1Scale = %.2f\n", args.RoiR1Scale);
  printf("args.RoiR2Scale = %.2f\n", args.RoiR2Scale);
  printf("args.MarkDecodeThr = %d\n", args.MarkDecodeThr);
  printf("args.ROI = (%d,%d,%d,%d,%d)\n", args.ROI.x1, args.ROI.x2, args.ROI.x3, args.ROI.x4, args.ROI.len);
  printf("args.debugMode = %d\n", args.debugMode);
}

// ------------------------------------------------------------------------------------------------------------------------------

std::vector<double> linspace(double start_in, double end_in, int num_in, bool flip) {

  std::vector<double> linspaced;

  double start = static_cast<double>(start_in);
  double end = static_cast<double>(end_in);
  double num = static_cast<double>(num_in);

  if (num == 0) { return linspaced; }
  if (num == 1) {
    linspaced.push_back(start);
    return linspaced;
  }

  double delta = (end - start) / (num - 1);

  for (int i=0; i < num-1; ++i) {
    linspaced.push_back(start + delta * i);
  }
  linspaced.push_back(end);

  if (flip) {
      std::reverse(linspaced.begin(), linspaced.end());
  }
  
  return linspaced;
}

// ------------------------------------------------------------------------------------------------------------------------------


std::string bin2hex(std::string binstr) {
    
  std::string hexstr;

  for (uint i = 0; i < binstr.size(); i += 4) {
    
    int8_t n = 0;
    for (uint j = i; j < i + 4; ++j) {
      n <<= 1;
      if (binstr[j] == '1') {
        n |= 1;
      }
    }

    if (n<=9) {
      hexstr.push_back('0' + n);
    }
    else {
      hexstr.push_back('A' + n - 10);
    }
  }

  return hexstr;
}
