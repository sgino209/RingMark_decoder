// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#include "detect_marks.hpp"

double find_possible_marks(mark_list_t &possible_marks_final, cv::Mat &frame_thresh, unsigned int MinPixelWidth, unsigned int MaxPixelWidth, 
                           unsigned int MinPixelHeight, unsigned int MaxPixelHeight, double MinAspectRatio, double MaxAspectRatio, 
                           unsigned int MinPixelArea, unsigned int MaxPixelArea, double MinExtent, double MaxExtent, double MaxDrift,
                           unsigned int PerspectiveMode, std::string FindContoursMode, unsigned int HoughParams1, unsigned int HoughParams2,
                           unsigned int HoughParams3,unsigned int HoughParams4,unsigned int HoughParams5,unsigned int HoughParams6, bool debugMode) {

  char buffer[1000];
  double rotation_angle_deg;
  rotation_align_t rotation_align;
  unsigned int possible_marks_cntr=0;
  mark_list_t possible_marks_list, possible_marks_wo_outliers;

  // Find all contours in the image: 
  std::vector<std::vector<cv::Point>> contours;

  if (FindContoursMode == "Legacy") {
    cv::findContours(frame_thresh.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  }
  else if (FindContoursMode == "Hough") {
    
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(
        frame_thresh.clone(),                    // 8-bit, single channel image
        circles,                                 // detected circles (results)
        cv::HOUGH_GRADIENT,                      // Defines the method to detect circles in images
        (HoughParams1 > 0 ? HoughParams1 :  1),  // Large dp values -->  smaller accumulator array
        (HoughParams2 > 0 ? HoughParams2 : 60),  // Min distance between the detected circles centers
        (HoughParams3 > 0 ? HoughParams3 : 50),  // Gradient value used to handle edge detection
        (HoughParams4 > 0 ? HoughParams4 : 18),  // Accumulator thresh val (smaller = more circles)
        (HoughParams5 > 0 ? HoughParams5 : 20),  // Minimum size of the radius (in pixels)
        (HoughParams6 > 0 ? HoughParams6 : 50)   // Maximum size of the radius (in pixels)
    );

    if (!circles.empty()) {
      for (auto it = circles.begin(); it != circles.end(); it++) {
        contours.push_back(circle_to_contour(*it, 50, 0.7));
      }
    }
  }

  else {
    error("Unsupported FindContoursMode mode: " + FindContoursMode);
  }

  // Foreach contour, check if it describes a possible character:
  cv::Mat frame_contours = cv::Mat::zeros(frame_thresh.size(), CV_8UC3);
  for (unsigned int i = 0; i < contours.size(); i++) {

    // Register the contour as a possible character (+calculate intrinsic metrics):
    PossibleMark possible_mark = PossibleMark(contours[i],
                                              MinPixelWidth, MaxPixelWidth,
                                              MinPixelHeight, MaxPixelHeight,
                                              MinAspectRatio, MaxAspectRatio,
                                              MinPixelArea, MaxPixelArea,
                                              MinExtent, MaxExtent);

    // If contour is a possible char, increment count of possible chars and add to list of possible chars:
    if (possible_mark.checkIfPossibleMark()) {
      possible_marks_cntr++;
      possible_marks_list.push_back(possible_mark);
    }

    if (debugMode) {
      cv::drawContours(frame_contours, contours, i, SCALAR_WHITE);
    }
  }

  if (possible_marks_list.size() == 0) {
    possible_marks_final = possible_marks_list;
    return 0;
  }

  // Remove outliers in a PCA scheme, i.e. possible marks which are too faraway from the group or interiors:
  possible_marks_wo_outliers = remove_outliers(possible_marks_list, MaxDrift, debugMode);

  // Rotation and Perspective alignments:
  rotation_angle_deg = 0;
  if (possible_marks_wo_outliers.size() > 0) {

    // Rotation Alignment (SVD decomposition):
    rotation_align = rotation_alignment(possible_marks_wo_outliers, debugMode);

    rotation_angle_deg = rotation_align.rotation_angle_deg;

    // Perspective Alignment (Homography+PerspectiveWarp):
    possible_marks_final = possible_marks_wo_outliers;
    perspective_alignment(possible_marks_final, PerspectiveMode, rotation_angle_deg, debugMode);
  }

  // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- ..

  if (debugMode) {

    int kx, ky;
    unsigned int X_xc, X_yc;
    mark_list_t::iterator it;
    
    cv::Mat frame_possible_marks = cv::Mat::zeros(frame_thresh.size(), CV_8UC3);
  
    for(it=possible_marks_wo_outliers.begin(); it<possible_marks_wo_outliers.end(); it++) {
      
      std::vector<std::vector<cv::Point>> contours;
      contours.push_back(it->contour);
      cv::drawContours(frame_possible_marks, contours, 0, SCALAR_WHITE);
      for (ky=-3; ky<=3; ky++) {
        for (kx=-3; kx<=3; kx++) {
          frame_possible_marks.at<cv::Vec3b>(it->intCenterY+ky,it->intCenterX+kx)[2] = 255;
          frame_possible_marks.at<cv::Vec3b>(it->intCenterY_r+ky,it->intCenterX_r+kx)[0] = 255;
        }
      }
    }

    for(it=possible_marks_final.begin(); it<possible_marks_final.end(); it++) {
      
      for (ky=-3; ky<=3; ky++) {
        for (kx=-3; kx<=3; kx++) {
          frame_possible_marks.at<cv::Vec3b>(it->intCenterY_r+ky,it->intCenterX_r+kx)[1] = 255;
          frame_possible_marks.at<cv::Vec3b>(it->intCenterY_r+ky,it->intCenterX_r+kx)[2] = 255;
        }
      }
    }

    cv::putText(frame_possible_marks, "Original", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_RED, 2);
    cv::putText(frame_possible_marks, "Rotation fix (SVD)", cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_BLUE, 2);
    cv::putText(frame_possible_marks, "Centroid", cv::Point(10, 110), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_GREEN, 2);
    cv::putText(frame_possible_marks, "Perspective fix", cv::Point(10, 150), cv::FONT_HERSHEY_SIMPLEX, 1, SCALAR_YELLOW, 2);
       
    if (PerspectiveMode == 1) {
        // place-holder (TBD: translate from Python):
        //frame_possible_marks = polylines(frame_possible_marks, array([rect_src]), True, (255, 0, 0), 1, LINE_AA)
        //frame_possible_marks = polylines(frame_possible_marks, array([rect_dst]), True, (0, 255, 255), 1, LINE_AA)
    }

    X_xc = std::max(3,int(rotation_align.centroid_x));
    X_yc = std::max(3,int(rotation_align.centroid_y));
    for (ky=-3; ky<=3; ky++) {
      for (kx=-3; kx<=3; kx++) {
        frame_possible_marks.at<cv::Vec3b>(X_yc+ky,X_xc+kx)[1] = 255;
      }
    }

    sprintf(buffer, "Amount of detected contours: %d", contours.size());
    debug(buffer);
    sprintf(buffer, "Amount of possible marks: %d", possible_marks_cntr);
    debug(buffer);
    sprintf(buffer, "Amount of possible marks w/o outliers: %d", possible_marks_wo_outliers.size());
    debug(buffer);
    sprintf(buffer, "Rotation: %.2f", rotation_angle_deg);
    debug(buffer);
    cv::imwrite("img_contours_all.jpg", frame_contours);
    cv::imwrite("img_possible_marks_.jpg", frame_possible_marks);
  } 

  return rotation_angle_deg;
}

// ------------------------------------------------------------------------------------------------------------------------------
mark_list_t remove_outliers(mark_list_t possible_marks_list, double MaxDrift, bool debugMode) {
  
  std::string dbg;
  bool is_interior;
  char buffer[1000];
  cv::Point2f p1, p2;
  std::vector<double> dist;
  mark_list_t::iterator it;
  mark_list_t possible_marks_list_final;
  double median_x, median_y, median_dist, d, dr;
  std::vector<unsigned int> median_x_vec, median_y_vec;

  // Extract marks relevant data (Median calculation):
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {
    median_x_vec.push_back(it->intCenterX);
    median_y_vec.push_back(it->intCenterY);
  }
  median_x = median_calc<unsigned int>(median_x_vec);
  median_y = median_calc<unsigned int>(median_y_vec);
  
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {
    p1 = cv::Point2f(it->intCenterX, it->intCenterY);
    p2 = cv::Point2f(median_x, median_y);
    d = distance_mse(p1,p2);
    dist.push_back(d);
  }
  median_dist = median_calc<double>(dist);

  if (debugMode) {
    sprintf(buffer, "median_x=%.2f, median_y=%.2f, median_dist=%.2f", median_x, median_y, median_dist);
    debug(buffer);
  }
    
  // Exclude marks with a too-high drift or interiors:
  for (unsigned k=0; k<possible_marks_list.size(); k++) {
    
    dbg = "";

    PossibleMark possible_mark = possible_marks_list.at(k);
    dr = (median_dist == 0) ? 0 : dist.at(k)/median_dist;

    is_interior = false;
    for (unsigned l=0; l<possible_marks_list.size(); l++) {
      if (l == k) {
        continue;
      }
      p1 = cv::Point2f(possible_mark.intCenterX, possible_mark.intCenterY);
      p2 = cv::Point2f(possible_marks_list.at(l).intCenterX, possible_marks_list.at(l).intCenterY);
      d = distance_mse(p1,p2);
      if ((d < 2*MaxDrift) && (possible_mark.intBoundingRectArea < possible_marks_list.at(l).intBoundingRectArea)) {
        is_interior = true;
        dbg = "(X)";
      }
    }

    if ((dr < MaxDrift) && !is_interior) {
      possible_marks_list_final.push_back(possible_mark);
      dbg = "(*)";
    }

    if (debugMode) {
      sprintf(buffer, "possible_mark=%s, dist[%d]=%.2f, disr_r=%.2f %s", possible_mark.to_string().c_str(), k, dist.at(k), dr, dbg.c_str());
      debug(buffer);
    }
  }

  return possible_marks_list_final;
}

// ------------------------------------------------------------------------------------------------------------------------------
rotation_align_t rotation_alignment(mark_list_t &possible_marks_list, bool debugMode) {

  cv::SVD svdMat;
  cv::Mat S, U, Vt;
  char buffer[1000];
  mark_list_t::iterator it;
  std::vector<cv::Point> H;
  rotation_align_t rotation_align;
  unsigned int n=1, centroid_x=0, centroid_y=0;
  double mean_x=0, mean_y=0, rotation_angle_deg=0, rotation_angle;

  // Calculate Centroid:
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {

    mean_x += (it->intCenterX - mean_x)/n;
    mean_y += (it->intCenterY - mean_y)/n;
    n++;
  }

  // Calculate covariance matrix (H):
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {
    H.push_back(cv::Point(round(it->intCenterX - mean_x), round(it->intCenterY - mean_y)));
  }
  if (debugMode) {
    debug("Covariance matrix (H):");
    print_point_vec<cv::Point>(H);
  }

  // SVD decomposition:
  if (0) {
    cv::SVD::compute(H, S, U, Vt, cv::SVD::FULL_UV);
  }
  else {
    int num = possible_marks_list.size();
    float arr[num][2];
    for(int i=0; i<num; i++) {
          arr[i][1] = possible_marks_list[i].intCenterY;
          arr[i][0] = possible_marks_list[i].intCenterX;
    }
    cv::Mat mat = cv::Mat(num, 2, CV_32FC1, &arr);
    cv::Mat cov, mean;
    cv::calcCovarMatrix(mat, cov, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS);
    cov = cov / (mat.rows - 1);
    cv::eigen(cov, S, Vt);
    Vt *= -1;
  }
  
  // Calculate rotation angle:
  rotation_angle = std::atan2(Vt.at<double>(0,1), Vt.at<double>(0,0));
  rotation_angle_deg = rotation_angle * (180 / CV_PI);
  
  // Rotation alignment:
  int k=0;
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++, k++) {
    it->intCenterX_r = (int)(H[k].x *  cos(rotation_angle) + H[k].y * sin(rotation_angle) + (int)(mean_x));
    it->intCenterY_r = (int)(H[k].x * -sin(rotation_angle) + H[k].y * cos(rotation_angle) + (int)(mean_y));
    if (debugMode) {
      sprintf(buffer, "possible_mark=%s", it->to_string().c_str());
      debug(buffer);
    }
  }

  rotation_align.rotation_angle_deg = rotation_angle_deg;
  rotation_align.centroid_x = centroid_x;
  rotation_align.centroid_y = centroid_y;

  return rotation_align;
}

// ------------------------------------------------------------------------------------------------------------------------------
void perspective_alignment(mark_list_t &possible_marks_list, unsigned int PerspectiveMode, double rotation_angle_deg, bool debugMode) {

    if (PerspectiveMode == 0) {
        return perspective_alignment_opt0(possible_marks_list, rotation_angle_deg, debugMode);
    }
    else if (PerspectiveMode == 1) {
        return perspective_alignment_opt1(possible_marks_list, debugMode);
    }
    else {
        error("Invalid PerspectiveMode (%d)" + std::to_string(PerspectiveMode));
    }
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 

void perspective_alignment_opt0(mark_list_t &possible_marks_list, double rotation_angle_deg, bool debugMode) {
  
  double mean_x = 0;
  unsigned int n = 1;
  unsigned int dist_x;
  mark_list_t::iterator it;

  // Calculate meanX:
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {

    mean_x += (it->intCenterX_r - mean_x)/n;
    n++;
  }

  // Update marks coordinates (adaptive with angle):
  for(it=possible_marks_list.begin(); it<possible_marks_list.end(); it++) {

        dist_x = std::abs(it->intCenterX_r - mean_x);
        if (std::abs(rotation_angle_deg) > 90) {
            if (std::abs(rotation_angle_deg) < 170) {
                it->intCenterY_r -= (unsigned int)(2 * sqrt(dist_x));
            }
            else {
                it->intCenterY_r -= (unsigned int)(1 * sqrt(dist_x));
            }
        }
        else {
            it->intCenterY_r += (unsigned int)(2 * sqrt(dist_x));
        }
  }

  // Debug:
  if (debugMode) {

    debug("(xR,yR) after Perspective Fix:");
    print_mark_vec<PossibleMark>(possible_marks_list);
  }
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 

void perspective_alignment_opt1(mark_list_t &possible_marks_list, bool debugMode) {

  error("PerspectiveMode=1 is Not supported at the moment in CPP edition");
}

// ------------------------------------------------------------------------------------------------------------------------------

std::vector<cv::Point> circle_to_contour(cv::Vec3f circle, unsigned int points_per_contour, float resize_factor) {
  
  cv::Point center(round(circle[0]), round(circle[1]));
  int xc = center.x;
  int yc = center.y;
  double r = cvRound(circle[2]);
  r *= resize_factor;
  std::vector<cv::Point> contour;
  const double pi2 = 2*M_PI;
  for (double i=0; i<pi2; i+=pi2/points_per_contour) {
    int y = int(yc + r * sin(i));
    int x = int(xc + r * cos(i));
    contour.push_back(cv::Point(x,y));
  }
  return contour;
}

// ------------------------------------------------------------------------------------------------------------------------------
template <class T>
double median_calc(std::vector<T> data_vec) {
 
  double median;

  sort(data_vec.begin(), data_vec.end());

  if(data_vec.size()%2==0) median = ( data_vec[ data_vec.size()/2 - 1 ] + data_vec[ data_vec.size()/2 ] )/2.0;
  else median = data_vec[ data_vec.size()/2 ];

  return median;
}

// ------------------------------------------------------------------------------------------------------------------------------
template <class T>
void print_point_vec(std::vector<T> data_vec) {

    std::cout << "[[  " << std::flush;
    for(auto it=data_vec.begin(); it<data_vec.end(); it++) {
      std::cout << it->x << "  " << std::flush;
    }
    std::cout << "]" << std::endl << "[  " << std::flush;
    for(auto it=data_vec.begin(); it<data_vec.end(); it++) {
      std::cout << it->y << "  " << std::flush;
    }
    std::cout << "]]" << std::endl << std::flush;
}

// ---------------------------------------------------------------------------------------------------------------
template <class T>
void print_mark_vec(std::vector<T> data_vec) {

    std::cout << "[[  " << std::flush;
    for(auto it=data_vec.begin(); it<data_vec.end(); it++) {
      std::cout << it->intCenterX_r << "  " << std::flush;
    }
    std::cout << "]" << std::endl << "[  " << std::flush;
    for(auto it=data_vec.begin(); it<data_vec.end(); it++) {
      std::cout << it->intCenterY_r << "  " << std::flush;
    }
    std::cout << "]]" << std::endl << std::flush;
}

