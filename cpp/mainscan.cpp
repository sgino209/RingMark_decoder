// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#include "mainscan.hpp"

const char* bracelet_decoder_version = "1.3";

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
      case 'h':                            usage(argv[0]);  exit(0);                                       break;
      case 'v':                            sprintf(buffer, "Bracelet_Decoder version: %s", bracelet_decoder_version );
                                           info(buffer); 
                                           exit(0);                                                        break;
      case 'i':                            if (optarg) { args.ImageFile = optarg;                       }  break;
      case ARG_PREPROCESSCVCSEL:           if (optarg) { args.PreprocessCvcSel = optarg;                }  break;
      case ARG_PREPROCESSMODE:             if (optarg) { args.PreprocessMode = optarg;                  }  break;
      case ARG_PREPROCESSGAUSSKERNEL:      if (optarg) { set_xy(&args.PreprocessGaussKernel, optarg);   }  break;
      case ARG_PREPROCESSTHRESHBLOCKSIZE:  if (optarg) { args.PreprocessThreshBlockSize = atoi(optarg); }  break;
      case ARG_PREPROCESSTHRESHWEIGHT:     if (optarg) { args.PreprocessThreshweight = atoi(optarg);    }  break;
      case ARG_PREPROCESSMORPHKERNEL:      if (optarg) { set_xy(&args.PreprocessMorphKernel, optarg);   }  break;
      case ARG_PREPROCESSMEDIANBLURKERNEL: if (optarg) { args.PreprocessMedianBlurKernel = atoi(optarg);}  break;
      case ARG_PREPROCESSCANNYTHR:         if (optarg) { args.PreprocessCannyThr = atoi(optarg);        }  break;
      case ARG_IMGENHANCEMENTEN:           args.imgEnhancementEn = true;                                   break;
      case ARG_MINPIXELWIDTH:              if (optarg) { args.MinPixelWidth = atoi(optarg);             }  break;
      case ARG_MAXPIXELWIDTH:              if (optarg) { args.MaxPixelWidth = atoi(optarg);             }  break;
      case ARG_MINPIXELHEIGHT:             if (optarg) { args.MinPixelHeight = atoi(optarg);            }  break;
      case ARG_MAXPIXELHEIGHT:             if (optarg) { args.MaxPixelHeight = atoi(optarg);            }  break;
      case ARG_MINASPECTRATIO:             if (optarg) { args.MinAspectRatio = atof(optarg);            }  break;
      case ARG_MAXASPECTRATIO:             if (optarg) { args.MaxAspectRatio = atof(optarg);            }  break;
      case ARG_MINPIXELAREA:               if (optarg) { args.MinPixelArea = atoi(optarg);              }  break;
      case ARG_MAXPIXELAREA:               if (optarg) { args.MaxPixelArea = atoi(optarg);              }  break;
      case ARG_MINEXTENT:                  if (optarg) { args.MinExtent = atof(optarg);                 }  break;
      case ARG_MAXEXTENT:                  if (optarg) { args.MaxExtent = atof(optarg);                 }  break;
      case ARG_MAXDRIFT:                   if (optarg) { args.MaxDrift = atof(optarg);                  }  break;
      case ARG_MARKROWS:                   if (optarg) { args.MarksRows = atoi(optarg);                 }  break;
      case ARG_MARKCOLS:                   if (optarg) { args.MarksCols = atoi(optarg);                 }  break;
      case ARG_ROI:                        if (optarg) { set_x4(&args.ROI, optarg);                     }  break;
      case ARG_FINDCONTOURSMODE:           if (optarg) { args.FindContoursMode = optarg;                }  break;
      case ARG_HOUGHPARAMS:                if (optarg) { set_x6(&args.HoughParams, optarg);             }  break;
      case ARG_PERSPECTIVEMODE:            if (optarg) { args.PerspectiveMode = atoi(optarg);           }  break;
      case ARG_DEBUG:                      args.debugMode = true;                                          break;
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
    std::string envpath = "bracelet_decoder_results_" + timestr + "_" + img_str;

    return envpath;
}

// ------------------------------------------------------------------------------------------------------------------------------
void usage(char *script_name) {
  
  std::cout << std::endl;
  std::cout << script_name << "-i [image_file]" << std::endl;
  std::cout << std::endl;
  std::cout << "Optional preprocssing flags:                  --PreprocessCvcSel --PreprocessMode --PreprocessGaussKernel --PreprocessThreshBlockSize" << std::endl;
  std::cout << "Optional preprocssing flags (cont.):          --PreprocessThreshweight --PreprocessMorphKernel --PreprocessMedianBlurKernel" << std::endl;
  std::cout << "Optional preprocssing flags (cont.):          --PreprocessCannyThr --imgEnhancementEn" << std::endl;
  std::cout << "Optional marks-detection flags:               --MinPixelWidth --MaxPixelWidth --MinPixelHeight --MaxPixelHeight --MinAspectRatio --MaxAspectRatio" << std::endl;
  std::cout << "Optional marks-detection flags (cont.):       --MinPixelArea --MaxPixelArea --MaxDrift --MarksRows --MarksCols --ROI" << std::endl;
  std::cout << "Optional marks-detection flags (cont.):       --FindContoursMode --HoughParams --PerspectiveMode" << std::endl;
  std::cout << "Optional misc. flags:                         --debug --version" << std::endl;
  std::cout << std::endl;
  std::cout << "Note about HoughParams settings (relevant only when FindContoursMode=\"Hough\"):" << std::endl;
  std::cout << "   HoughParams = (dp, minDist, param1, param2, minRadius, maxRadius)" << std::endl;
  std::cout << "   (-) dp ---------> Large dp values -->  smaller accumulator array" << std::endl;
  std::cout << "   (-) minDist ----> Min distance between the detected circles centers" << std::endl;
  std::cout << "   (-) param1 -----> Gradient value used to handle edge detection" << std::endl;
  std::cout << "   (-) param2 -----> Accumulator thresh val (smaller = more circles)" << std::endl;
  std::cout << "   (-) minRadius --> Minimum size of the radius (in pixels)" << std::endl;
  std::cout << "   (-) maxRadius --> Maximum size of the radius (in pixels)" << std::endl;
  std::cout << std::endl;
  std::cout << "Note about ROI settings:" << std::endl;
  std::cout << "   (-) Option 1:  ROI = (startX, startY, width, height)" << std::endl;
  std::cout << "   (-) Option 2:  ROI = (startX, startY, R)  --->  width=height=R" << std::endl;
  std::cout << "   (-) Option 3:  ROI = (0,0) --->  ROI equals to whole input image" << std::endl;
  std::cout << std::endl;
  std::cout << "Note that input image is automatically being resized for 1600x1200, so ROI shall be set accordingly" << std::endl;
  std::cout << std::endl;
}

