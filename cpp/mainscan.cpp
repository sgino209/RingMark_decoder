// RingMark-Decoder algo app
// (c) Shahar Gino, May-2020, sgino209@gmail.com

#include "mainscan.hpp"

const char* ringmark_decoder_version = "1.0";

int main(int argc, char** argv) {
  
  int index, iarg = 0;
  args_t args;
  std::clock_t t0,t1;
  double t_elapsed_sec;
  char buffer[1000];
  std::string code;
  
  t0 = std::clock();
  printf("Start\n");
  info("OpenCV version: " + std::string(CV_VERSION));
 
  args = load_default_args();

  while (iarg != -1)
  {
    iarg = getopt_long(argc, argv, "hvi:", longopts, &index);
    
    switch (iarg) {
      case 'h':                              usage(argv[0]);  exit(0);                                       break;
      case 'v':                              sprintf(buffer, "RingMark_Decoder version: %s", ringmark_decoder_version );
                                             info(buffer); 
                                             exit(0);                                                        break;
      case 'i':                              if (optarg) { args.ImageFile = optarg;                       }  break;
      case ARG_TEMPLATE_FILE:                if (optarg) { args.TemplateFile = optarg;                    }  break;
      case ARG_MARKS_NUM_EXTERNAL:           if (optarg) { args.MarksNumExternal = atoi(optarg);          }  break;
      case ARG_MARKS_NUM_INTERNAL:           if (optarg) { args.MarksNumInternal = atoi(optarg);          }  break;
      case ARG_TEMPLATE_MATCHING_THR:        if (optarg) { args.TemplateMatchingThr = atof(optarg);       }  break;
      case ARG_TEMPLATE_MATCHING_MIN_SCALE:  if (optarg) { args.TemplateMatchingMinScale = atof(optarg);  }  break;
      case ARG_TEMPLATE_MATCHING_MAX_SCALE:  if (optarg) { args.TemplateMatchingMaxScale = atof(optarg);  }  break;
      case ARG_TEMPLATE_TO_MARK_X_SCALE:     if (optarg) { args.TemplateToMarkXScale = atof(optarg);      }  break;
      case ARG_TEMPLATE_TO_MARK_Y_SCALE:     if (optarg) { args.TemplateToMarkYScale = atof(optarg);      }  break;
      case ARG_CROP_MARGIN_R:                if (optarg) { args.CropMarginR = atof(optarg);               }  break;
      case ARG_CROP_MARGIN_X:                if (optarg) { args.CropMarginX = atoi(optarg);               }  break;
      case ARG_CROP_MARGIN_Y:                if (optarg) { args.CropMarginY = atoi(optarg);               }  break;
      case ARG_ROI_R1_SCALE:                 if (optarg) { args.RoiR1Scale = atof(optarg);                }  break;
      case ARG_ROI_R2_SCALE:                 if (optarg) { args.RoiR2Scale = atof(optarg);                }  break;
      case ARG_MARK_DECODE_THR:              if (optarg) { args.MarkDecodeThr = atoi(optarg);             }  break;
      case ARG_ROI:                          if (optarg) { set_x4(&args.ROI, optarg);                     }  break;
      case ARG_DEBUG:                        args.debugMode = true;                                          break;
    }
  }
  
  // Create a working environment:
  if (args.debugMode) {

    std::string envpath = get_envpath(args.ImageFile);  // results_%d%m%y_%H%M%S_imgFile

    if (opendir(envpath.c_str()) == NULL) {
      std::string cmd = "mkdir " + envpath;
      system(cmd.c_str());
    }
    chdir(envpath.c_str());
  }

  if (args.debugMode) {
    print_args(args);
  }

  // Frame Docoder (algo):
  code = decode_frame(args);

  info("Code = " + code);

  t1 = std::clock();
  t_elapsed_sec = (t1 - t0) / (double)CLOCKS_PER_SEC;
  std::cout << "Done! (" << std::setprecision(2) << t_elapsed_sec << " sec)" << std::endl;
  
  return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------
std::string get_envpath(std::string ImageFile) {

    time_t rawtime = std::time(nullptr);
    time(&rawtime);
    const auto timeinfo = localtime(&rawtime);
    char day_buf[50];  sprintf(day_buf,  "%02d", timeinfo->tm_mday);
    char mon_buf[50];  sprintf(mon_buf,  "%02d", timeinfo->tm_mon);
    char hour_buf[50]; sprintf(hour_buf, "%02d", timeinfo->tm_hour);
    char min_buf[50];  sprintf(min_buf,  "%02d", timeinfo->tm_min);
    char sec_buf[50];  sprintf(sec_buf,  "%02d", timeinfo->tm_sec);
    std::string timestr = std::string(day_buf) + std::string(mon_buf) + std::to_string(timeinfo->tm_year%100) + "_" +
                          std::string(hour_buf) + std::string(min_buf) + std::string(sec_buf);
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::size_t found1 = ImageFile.rfind("\");
    #else
    std::size_t found1 = ImageFile.rfind("/");
    #endif
    std::string img_str = "imgNA";
    if (found1 != std::string::npos) {
        img_str = ImageFile.substr(found1+1);
        std::size_t found2 = img_str.rfind(".");
        if (found2 != std::string::npos) {
            img_str.replace(found2,1,"_");
        }
    }
    std::string envpath = "ringmark_decoder_results_" + timestr + "_" + img_str;

    return envpath;
}

// ------------------------------------------------------------------------------------------------------------------------------
void usage(char *script_name) {
  
  std::cout << ""                                                                                                                       << std::endl;
  std::cout << script_name << "-i [image_file]"                                                                                         << std::endl;
  std::cout << ""                                                                                                                       << std::endl;
  std::cout << "Marks Number (external/internal rings):    --MarksNumExternal --MarksNumInternal"                                       << std::endl;
  std::cout << "Template File path:                        --TemplateFile"                                                              << std::endl;
  std::cout << "Template Matching (thr/min/max):           --TemplateMatchingThr --TemplateMatchingMinScale --TemplateMatchingMaxScale" << std::endl;
  std::cout << "Template to RingMark distance (x/y):       --TemplateToMarkXScale --TemplateToMarkYScale"                               << std::endl;
  std::cout << "Crop RingMark Margins (x/y/R):             --CropMarginR --CropMarginX --CropMarginY"                                   << std::endl;
  std::cout << "RinkMark Scale (external/internal rings):  --RoiR1Scale --RoiR2Scale"                                                   << std::endl;
  std::cout << "RinkMark decode threshold:                 --MarkDecodeThr"                                                             << std::endl;
  std::cout << "Region Of Interest (ROI) selection:        --ROI"                                                                       << std::endl;
  std::cout << "Optional misc. flags:                      --debug --version"                                                           << std::endl;
  std::cout << ""                                                                                                                       << std::endl;
  std::cout << "ROI settings:"                                                                                                          << std::endl;
  std::cout << "   (-) Option 1:  ROI = (startX, startY, width, height)"                                                                << std::endl;
  std::cout << "   (-) Option 2:  ROI = (startX, startY, R)  --->  width=height=R"                                                      << std::endl;
  std::cout << "   (-) Option 3:  ROI = (0,0) --->  ROI equals to whole input image"                                                    << std::endl;
  std::cout << ""                                                                                                                       << std::endl;
  std::cout << "Note: input image is automatically being resized for 1600x1200, so ROI shall be set accordingly"                        << std::endl;
  std::cout << ""                                                                                                                       << std::endl;
}

