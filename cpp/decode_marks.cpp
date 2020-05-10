// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#include "decode_marks.hpp"

std::string decode_marks(mark_list_t marks_list, unsigned int MarksRows, unsigned int MarksCols, cv::Size frame_shape, double rotation_angle_deg, bool debugMode) {

  char buffer[1000];
  std::string code_hex_str = "N/A";
  mark_list_t::iterator it;

  if (marks_list.size() < MarksRows) {
    return code_hex_str;
  }

  // Extract XY min/max for the given marks:
  unsigned int min_x=2*frame_shape.width, max_x=0, min_y=2*frame_shape.height, max_y=0;
  for(it=marks_list.begin(); it<marks_list.end(); it++) {
    
    PossibleMark mark = *it;

    if (mark.intCenterX_r < min_x) { min_x = mark.intCenterX_r; }
    if (mark.intCenterX_r > max_x) { max_x = mark.intCenterX_r; }
    if (mark.intCenterY_r < min_y) { min_y = mark.intCenterY_r; }
    if (mark.intCenterY_r > max_y) { max_y = mark.intCenterY_r; }
  }
  min_y += 5;
  if (max_y > 310) {
    max_y -= 5;
  }
  
  // Prepare KNN origins:
  if ((std::abs(rotation_angle_deg) > 80) && (std::abs(rotation_angle_deg) < 100)) {   // Adaptive scale fix
    min_x -= 1;
    max_x += 1;
  }

  unsigned int radius_y = (unsigned int)((max_y - min_y) / (MarksRows - 1));
  unsigned int radius_x = (unsigned int)((max_x - min_x) / (MarksCols - 1));

  if ((radius_y == 0) || (radius_x == 0) || (radius_y > 100) || (radius_x > 100)) {
    return code_hex_str;
  }

  std::vector<unsigned int> y_scale, x_scale;
  for (unsigned int k=min_y; k<max_y; k+=radius_y) { y_scale.push_back(k); }
  for (unsigned int k=min_x; k<max_x; k+=radius_x) { x_scale.push_back(k); }

  if (y_scale.size() < MarksRows) {
    y_scale.push_back(max_y);
  }

  if (x_scale.size() < MarksCols) {
    x_scale.push_back(max_x);
  }

  // For each KNN origin, seek for closest mark:
  std::vector<unsigned int>::iterator it_x, it_y;

  if (debugMode) {
    
    std::cout << "DEBUG: x_scale = [" << std::flush;
    for(it_x=x_scale.begin(); it_x<x_scale.end(); it_x++) {
      std::cout << *it_x << ", " << std::flush;
    }
    std::cout << "]" << std::endl << std::flush;
    
    std::cout << "DEBUG: y_scale = [" << std::flush;
    for(it_y=y_scale.begin(); it_y<y_scale.end(); it_y++) {
      std::cout << *it_y << ", " << std::flush;
    }
    std::cout << "]" << std::endl << std::flush;
  }

  std::string code_str = "";

  for(it_y=y_scale.begin(); it_y<y_scale.end(); it_y++) {
    for(it_x=x_scale.begin(); it_x<x_scale.end(); it_x++) {
      int ret = seek_for_mark(*it_x, *it_y, marks_list, (unsigned int)(radius_x), (unsigned int)(radius_y));
      code_str += std::to_string(ret);
    }
  }

  if (debugMode) {
    sprintf(buffer, "code before flip: %s", code_str.c_str());
    info(buffer);
  }

  // Code must start with '1' and end with '0', flip if not:
  int N = code_str.length();
  if ((code_str[0] == '0') && (code_str[N-1] == '1')) {

    // Swap character starting from two corners
    for (int i=0; i<N/2; i++)
       std::swap(code_str[i], code_str[N-i-1]);
    if (debugMode) {
      sprintf(buffer, "code after flip: %s", code_str.c_str());
      info(buffer);
    }
  }
  else if (code_str[0] == code_str[N-1]) {
    error("Invalid code detected! (first mark equals to last mark)");
  }

  // Bin to Hex:
  const unsigned g_unMaxBits = 32;
  std::bitset<g_unMaxBits> bs(code_str);
  unsigned int n = bs.to_ulong();
  std::stringstream ss;
  ss << std::hex << n;

  code_hex_str = "0x" + ss.str();

  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- ..

  // Debug:
  if (debugMode) {
  
    int height = frame_shape.height;
    int width = frame_shape.width;
    cv::Mat aligned_frame = cv::Mat::zeros(height, width, CV_8UC3);
  
    for(it_y=y_scale.begin(); it_y<y_scale.end(); it_y++) {
      for(it_x=x_scale.begin(); it_x<x_scale.end(); it_x++) {
        for (int ky=-2; ky<=2; ky++) {
          for (int kx=-2; kx<=2; kx++) {
            aligned_frame.at<cv::Vec3b>((*it_y)+ky,(*it_x)+kx)[1] = 255;
          }
        }
      }
    }
    for(it=marks_list.begin(); it<marks_list.end(); it++) {
      
      unsigned int mark_cy = (*it).intCenterY;
      unsigned int mark_cx = (*it).intCenterX;

      unsigned int mark_cy_r = (*it).intCenterY_r;
      unsigned int mark_cx_r = (*it).intCenterX_r;

      for (int ky=-3; ky<=3; ky++) {
        for (int kx=-3; kx<=3; kx++) {
          aligned_frame.at<cv::Vec3b>(mark_cy+ky,mark_cx+kx)[2] = 255;
          aligned_frame.at<cv::Vec3b>(mark_cy_r+ky,mark_cx_r+kx)[0] = 255;
        }
      }
    }
    
    cv::putText(aligned_frame, "Original", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_RED, 2);
    cv::putText(aligned_frame, "Rotation fix (SVD)", cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_BLUE, 2);
    cv::putText(aligned_frame, "KNN origins", cv::Point(10, 110), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_GREEN, 2);
    cv::imwrite("img_possible_marks_aligned.jpg", aligned_frame);
  }

  return code_hex_str;
}

// ------------------------------------------------------------------------------------------------------------------------------
int seek_for_mark(unsigned int x, unsigned int y, mark_list_t marks, unsigned int w, unsigned int h) {
  
  unsigned int xx, yy;
  mark_list_t::iterator it;

  for(it=marks.begin(); it<marks.end(); it++) {

    PossibleMark mark = *it;

    for (yy=y-int(h/2); yy<=y+int(h/2); yy++) {
      for (xx=x-int(w/2); xx<=x+int(w/2); xx++) {
        if ((mark.intCenterX_r == xx) && (mark.intCenterY_r == yy)) {
          return 1;
        }
      }
    }
  }
  
  return 0;
}

