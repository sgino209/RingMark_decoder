// Bracelet-Decoder algo app
// (c) Shahar Gino, September-2017, sgino209@gmail.com

#include "preprocess.hpp"

void preprocess(cv::Mat &imgOriginal, cv::Mat &imgGrayscale, cv::Mat &imgThresh, std::string PreprocessCvcSel, std::string PreprocessMode,
                uint_xy_t PreprocessGaussKernel, unsigned int PreprocessThreshBlockSize, unsigned int PreprocessThreshweight, 
                uint_xy_t PreprocessMorphKernel, unsigned int PreprocessMedianBlurKernel, unsigned int PreprocessCannyThr) {
    
    // Color-Space-Conversion (CSC): switch from BGR to HSV and take the requested component:
    cv::Mat imgHSV;
    std::vector<cv::Mat> vectorOfHSVImages;
    cv::Mat imgValue;
    cv::cvtColor(imgOriginal, imgHSV, cv::COLOR_BGR2HSV);
    cv::split(imgHSV, vectorOfHSVImages);
   
    if      (PreprocessCvcSel == "H") { imgGrayscale = vectorOfHSVImages[0]; }
    else if (PreprocessCvcSel == "S") { imgGrayscale = vectorOfHSVImages[1]; }
    else if (PreprocessCvcSel == "V") { imgGrayscale = vectorOfHSVImages[2]; }
    else {
        error("Unsupported PreprocessCvcSel mode: " + PreprocessCvcSel);
    }
    
    // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- ..

    cv::Mat imgBlurred;
    
    if (PreprocessMode == "Legacy") {

        // Increase contrast (morphological):
        cv::Mat imgMaxContrastGrayscale = maximizeContrast(imgGrayscale, PreprocessMorphKernel);

        // Blurring:
        cv::Size gaussKernel;
        gaussKernel = cv::Size(PreprocessGaussKernel.x, PreprocessGaussKernel.y);
        cv::GaussianBlur(imgMaxContrastGrayscale, imgBlurred, gaussKernel, 0);

        // Adaptive Threshold:
        cv::adaptiveThreshold(imgBlurred, imgThresh, 255.0, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, PreprocessThreshBlockSize, PreprocessThreshweight);
    }

    else if (PreprocessMode == "BlurAndCanny") {
        
        // Blurring:
        cv::medianBlur(imgGrayscale, imgBlurred, PreprocessMedianBlurKernel);

        // Canny Edge Detection:
        cv::Canny(imgBlurred, imgThresh, double(PreprocessCannyThr)/2, double(PreprocessCannyThr));
    }

    // -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- .. -- ..
    
    else {
        error("Unsupported PreprocessMode mode: " + PreprocessMode);
    }

    imgGrayscale = imgBlurred;
}

// ------------------------------------------------------------------------------------------------------------------------------
cv::Mat maximizeContrast(cv::Mat &imgGrayscale, uint_xy_t PreprocessMorphKernel) {
    
    cv::Mat imgTopHat;
    cv::Mat imgBlackHat;
    cv::Mat imgGrayscalePlusTopHat;
    cv::Mat imgGrayscalePlusTopHatMinusBlackHat;

    cv::Mat structuringElement = cv::getStructuringElement(CV_SHAPE_RECT, cv::Size(PreprocessMorphKernel.x, PreprocessMorphKernel.y));

    cv::morphologyEx(imgGrayscale, imgTopHat, CV_MOP_TOPHAT, structuringElement);
    cv::morphologyEx(imgGrayscale, imgBlackHat, CV_MOP_BLACKHAT, structuringElement);

    imgGrayscalePlusTopHat = imgGrayscale + imgTopHat;                           // = Image - Opening[Image] = Image - dilate[erode[Image]] --> "lowFreq"
    imgGrayscalePlusTopHatMinusBlackHat = imgGrayscalePlusTopHat - imgBlackHat;  // = Closing[Image] - Image = erode[dilate[Image]] - Image --> "highFreq"

    return(imgGrayscalePlusTopHatMinusBlackHat);
}

// ------------------------------------------------------------------------------------------------------------------------------
cv::Mat imageEnhancement(cv::Mat &imgOriginal, double clahe_clipLimit, int clahe_tileGridSize, double gamma, bool debugMode) {

    if (gamma != 3) {
      error("Currently only gamma=3 is implemented, overriding input gamma with value of 3");
    }

    // LUTs:
    float incr_ch_lut_data[256] = {
        0.26726152,    1.26844732,    2.27254198,    3.27951485,    4.2893353 , 
        5.3019727 ,    6.3173964 ,    7.33557578,    8.35648019,    9.38007899,
       10.40634155,   11.43523724,   12.46673541,   13.50080544,   14.53741667,
       15.57653849,   16.61814024,   17.66219129,   18.70866102,   19.75751877,
       20.80873391,   21.86227581,   22.91811383,   23.97621734,   25.03655568,
       26.09909824,   27.16381437,   28.23067344,   29.2996448 ,   30.37069783,
       31.44380189,   32.51892633,   33.59604052,   34.67511384,   35.75611562,
       36.83901526,   37.92378209,   39.0103855 ,   40.09879484,   41.18897947,
       42.28090876,   43.37455208,   44.46987878,   45.56685823,   46.66545979,
       47.76565282,   48.86740669,   49.97069077,   51.07547441,   52.18172698,
       53.28941784,   54.39851635,   55.50899188,   56.6208138 ,   57.73395146,
       58.84837422,   59.96405146,   61.08095254,   62.19904681,   63.31830364,
       64.43869239,   65.56018244,   66.68274313,   67.80634384,   68.93095393,
       70.05654276,   71.18307969,   72.31053409,   73.43887532,   74.56807274,
       75.69809573,   76.82891363,   77.96049582,   79.09281165,   80.22583049,
       81.35952171,   82.49385467,   83.62879872,   84.76432324,   85.90039759,
       87.03699112,   88.17407321,   89.31161322,   90.44958051,   91.58794444,
       92.72667437,   93.86573968,   95.00510972,   96.14475386,   97.28464146,
       98.42474188,   99.56502448,  100.70545864,  101.84601371,  102.98665905,
      104.12736404,  105.26809803,  106.40883038,  107.54953046,  108.69016764,
      109.83071127,  110.97113073,  112.11139536,  113.25147454,  114.39133763,
      115.53095399,  116.67029299,  117.80932399,  118.94801635,  120.08633943,
      121.22426261,  122.36175523,  123.49878667,  124.6353263 ,  125.77134346,
      126.90680753,  128.04168786,  129.17595383,  130.3095748 ,  131.44252012,
      132.57475916,  133.70626129,  134.83699587,  135.96693226,  137.09603983,
      138.22428793,  139.35164594,  140.47808321,  141.6035691 ,  142.72806795,
      143.85152387,  144.97387594,  146.09506327,  147.21502491,  148.33369997,
      149.45102753,  150.56694666,  151.68139646,  152.79431601,  153.90564438,
      155.01532068,  156.12328397,  157.22947334,  158.33382788,  159.43628667,
      160.5367888 ,  161.63527335,  162.7316794 ,  163.82594603,  164.91801234,
      166.0078174 ,  167.09530031,  168.18040013,  169.26305596,  170.34320688,
      171.42079198,  172.49575034,  173.56802103,  174.63754316,  175.7042558 ,
      176.76809803,  177.82900894,  178.88692761,  179.94179313,  180.99354458,
      182.04212104,  183.0874616 ,  184.12950535,  185.16819136,  186.20345872,
      187.23524652,  188.26349383,  189.28813975,  190.30912335,  191.32638373,
      192.33985996,  193.34949112,  194.35521631,  195.35697461,  196.35470509,
      197.34834685,  198.33783897,  199.32312053,  200.30413062,  201.28080832,
      202.25309271,  203.22092288,  204.18423791,  205.14297688,  206.09707889,
      207.04648301,  207.99112833,  208.93095393,  209.8658989 ,  210.79590231,
      211.72090326,  212.64084083,  213.5556541 ,  214.46528216,  215.36966408,
      216.26873896,  217.16244588,  218.05072392,  218.93351216,  219.8107497 ,
      220.6823756 ,  221.54832897,  222.40854888,  223.26297441,  224.11154465,
      224.95419869,  225.7908756 ,  226.62151447,  227.44605439,  228.26443444,
      229.0765937 ,  229.88247126,  230.68200621,  231.47513761,  232.26180457,
      233.04194616,  233.81550146,  234.58240957,  235.34260956,  236.09604052,
      236.84264154,  237.58235169,  238.31511006,  239.04085574,  239.75952781,
      240.47106535,  241.17540745,  241.87249318,  242.56226165,  243.24465192,
      243.91960308,  244.58705423,  245.24694443,  245.89921277,  246.54379835,
      247.18064024,  247.80967752,  248.43084929,  249.04409462,  249.6493526 ,
      250.24656231,  250.83566284,  251.41659327,  251.98929269,  252.55370017,
      253.1097548 ,  253.65739567,  254.19656187,  254.72719246,  255.24922655,
      255.7626032 
    };
    
    float decr_ch_lut_data[256] = {
      -2.67228320e-01,  -7.33877551e-02,   1.32520998e-01,   3.50368352e-01, 
       5.80024720e-01,   8.21360515e-01,   1.07424615e+00,   1.33855203e+00,
       1.61414859e+00,   1.90090621e+00,   2.19869533e+00,   2.50738636e+00,
       2.82684969e+00,   3.15695576e+00,   3.49757497e+00,   3.84857773e+00,
       4.20983446e+00,   4.58121557e+00,   4.96259148e+00,   5.35383259e+00,
       5.75480931e+00,   6.16539207e+00,   6.58545127e+00,   7.01485733e+00,
       7.45348066e+00,   7.90119167e+00,   8.35786077e+00,   8.82335838e+00,
       9.29755491e+00,   9.78032078e+00,   1.02715264e+01,   1.07710422e+01,
       1.12787385e+01,   1.17944858e+01,   1.23181546e+01,   1.28496151e+01,
       1.33887378e+01,   1.39353932e+01,   1.44894517e+01,   1.50507836e+01,
       1.56192593e+01,   1.61947494e+01,   1.67771242e+01,   1.73662540e+01,
       1.79620094e+01,   1.85642608e+01,   1.91728785e+01,   1.97877329e+01,
       2.04086945e+01,   2.10356337e+01,   2.16684210e+01,   2.23069266e+01,
       2.29510210e+01,   2.36005747e+01,   2.42554581e+01,   2.49155415e+01,
       2.55806954e+01,   2.62507902e+01,   2.69256963e+01,   2.76052841e+01,
       2.82894240e+01,   2.89779865e+01,   2.96708419e+01,   3.03678607e+01,
       3.10689133e+01,   3.17738701e+01,   3.24826014e+01,   3.31949778e+01,
       3.39108696e+01,   3.46301473e+01,   3.53526812e+01,   3.60783418e+01,
       3.68069995e+01,   3.75385247e+01,   3.82727878e+01,   3.90096592e+01,
       3.97490093e+01,   4.04907086e+01,   4.12346275e+01,   4.19806363e+01,
       4.27286055e+01,   4.34784055e+01,   4.42299067e+01,   4.49829795e+01,
       4.57374944e+01,   4.64933217e+01,   4.72503319e+01,   4.80083953e+01,
       4.87673825e+01,   4.95271637e+01,   5.02876095e+01,   5.10485901e+01,
       5.18099762e+01,   5.25716379e+01,   5.33334459e+01,   5.40952704e+01,
       5.48569819e+01,   5.56184508e+01,   5.63795475e+01,   5.71401424e+01,
       5.79001060e+01,   5.86593086e+01,   5.94176207e+01,   6.01749127e+01,
       6.09310550e+01,   6.16859179e+01,   6.24393720e+01,   6.31912877e+01,
       6.39415352e+01,   6.46899851e+01,   6.54365078e+01,   6.61809737e+01,
       6.69232531e+01,   6.76632166e+01,   6.84007345e+01,   6.91356772e+01,
       6.98679152e+01,   7.05973188e+01,   7.13237585e+01,   7.20471047e+01,
       7.27672277e+01,   7.34839981e+01,   7.41972862e+01,   7.49069625e+01,
       7.56128972e+01,   7.63149610e+01,   7.70130241e+01,   7.77069570e+01,
       7.83966301e+01,   7.90819646e+01,   7.97630851e+01,   8.04401670e+01,
       8.11133855e+01,   8.17829161e+01,   8.24489342e+01,   8.31116150e+01,
       8.37711340e+01,   8.44276665e+01,   8.50813879e+01,   8.57324735e+01,
       8.63810988e+01,   8.70274390e+01,   8.76716696e+01,   8.83139658e+01,
       8.89545031e+01,   8.95934569e+01,   9.02310024e+01,   9.08673151e+01,
       9.15025704e+01,   9.21369435e+01,   9.27706098e+01,   9.34037448e+01,
       9.40365237e+01,   9.46691220e+01,   9.53017150e+01,   9.59344780e+01,
       9.65675865e+01,   9.72012157e+01,   9.78355411e+01,   9.84707380e+01,
       9.91069819e+01,   9.97444479e+01,   1.00383312e+02,   1.01023748e+02,
       1.01665933e+02,   1.02310042e+02,   1.02956250e+02,   1.03604732e+02,
       1.04255664e+02,   1.04909221e+02,   1.05565578e+02,   1.06224912e+02,
       1.06887397e+02,   1.07553208e+02,   1.08222521e+02,   1.08895512e+02,
       1.09572355e+02,   1.10253227e+02,   1.10938302e+02,   1.11627755e+02,
       1.12321763e+02,   1.13020501e+02,   1.13724143e+02,   1.14432865e+02,
       1.15146843e+02,   1.15866252e+02,   1.16591268e+02,   1.17322065e+02,
       1.18058819e+02,   1.18801705e+02,   1.19550900e+02,   1.20306577e+02,
       1.21068913e+02,   1.21838083e+02,   1.22614262e+02,   1.23397626e+02,
       1.24188350e+02,   1.24986609e+02,   1.25792579e+02,   1.26606434e+02,
       1.27428352e+02,   1.28258506e+02,   1.29097072e+02,   1.29944226e+02,
       1.30800142e+02,   1.31664997e+02,   1.32538965e+02,   1.33422223e+02,
       1.34314945e+02,   1.35217306e+02,   1.36129482e+02,   1.37051649e+02,
       1.37983982e+02,   1.38926656e+02,   1.39879846e+02,   1.40843729e+02,
       1.41818478e+02,   1.42804270e+02,   1.43801280e+02,   1.44809684e+02,
       1.45829656e+02,   1.46861372e+02,   1.47905008e+02,   1.48960738e+02,
       1.50028739e+02,   1.51109185e+02,   1.52202251e+02,   1.53308115e+02,
       1.54426949e+02,   1.55558931e+02,   1.56704235e+02,   1.57863037e+02,
       1.59035512e+02,   1.60221835e+02,   1.61422182e+02,   1.62636728e+02,
       1.63865649e+02,   1.65109120e+02,   1.66367316e+02,   1.67640412e+02,
       1.68928584e+02,   1.70232008e+02,   1.71550859e+02,   1.72885311e+02,
       1.74235541e+02,   1.75601724e+02,   1.76984035e+02,   1.78382649e+02,
       1.79797742e+02,   1.81229490e+02,   1.82678067e+02,   1.84143649e+02,
       1.85626411e+02,   1.87126530e+02,   1.88644179e+02,   1.90179534e+02 
    };
    
    float gamma3_lut_data[256] = {
         0,  40,  50,  57,  63,  68,  73,  76,  80,  83,  86,  89,  92,  94,  96,  99, 101, 103,
       105, 107, 109, 110, 112, 114, 115, 117, 119, 120, 122, 123, 124, 126, 127, 128, 130, 131,
       132, 133, 135, 136, 137, 138, 139, 140, 141, 143, 144, 145, 146, 147, 148, 149, 150, 151,
       151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 164, 165, 166,
       167, 168, 168, 169, 170, 171, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178, 179,
       180, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186, 187, 187, 188, 189, 189, 190, 190,
       191, 192, 192, 193, 193, 194, 194, 195, 196, 196, 197, 197, 198, 198, 199, 199, 200, 201,
       201, 202, 202, 203, 203, 204, 204, 205, 205, 206, 206, 207, 207, 208, 208, 209, 209, 210,
       210, 211, 211, 212, 212, 213, 213, 214, 214, 215, 215, 216, 216, 216, 217, 217, 218, 218,
       219, 219, 220, 220, 221, 221, 221, 222, 222, 223, 223, 224, 224, 224, 225, 225, 226, 226,
       227, 227, 227, 228, 228, 229, 229, 229, 230, 230, 231, 231, 231, 232, 232, 233, 233, 233,
       234, 234, 235, 235, 235, 236, 236, 237, 237, 237, 238, 238, 239, 239, 239, 240, 240, 240,
       241, 241, 242, 242, 242, 243, 243, 243, 244, 244, 244, 245, 245, 246, 246, 246, 247, 247,
       247, 248, 248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253,
       253, 254, 254, 255
    };
    
    cv::Mat incr_ch_lut = cv::Mat(1, 256, CV_8UC1);
    cv::Mat decr_ch_lut = cv::Mat(1, 256, CV_8UC1, decr_ch_lut_data);
    cv::Mat gamma3_lut = cv::Mat(1, 256, CV_8UC1, gamma3_lut_data);
    
    for( int i = 0; i < 256; ++i) {
      incr_ch_lut.at<uchar>(0,i) = incr_ch_lut_data[i];
      decr_ch_lut.at<uchar>(0,i) = decr_ch_lut_data[i];
      gamma3_lut.at<uchar>(0,i) = gamma3_lut_data[i];
    }

    // Warming effect (R,G incr.) + CLAHE:
    cv::Mat c_r, c_b, B, G, R, imgWarm;
    std::vector<cv::Mat> vectorOfBGRImages;
    cv::split(imgOriginal, vectorOfBGRImages);
    cv::Mat b = vectorOfBGRImages[0].clone();
    cv::Mat g = vectorOfBGRImages[1].clone();
    cv::Mat r = vectorOfBGRImages[2].clone();
    cv::LUT(r, incr_ch_lut, c_r);
    c_r.convertTo(c_r, CV_8UC1);
    cv::LUT(b, decr_ch_lut, c_b);
    c_b.convertTo(c_b, CV_8UC1);
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(clahe_clipLimit);
    clahe->setTilesGridSize(cv::Size(clahe_tileGridSize,clahe_tileGridSize));
    clahe->apply(c_b, B);
    clahe->apply(g, G);
    clahe->apply(c_r, R);
    B.copyTo(vectorOfBGRImages[0]);
    G.copyTo(vectorOfBGRImages[1]);
    R.copyTo(vectorOfBGRImages[2]);
    cv::merge(vectorOfBGRImages, imgWarm);

    // Saturation effect (S incr.) + Gamma:
    cv::Mat imgHSV, c_s, imgSat, imgGamma;
    std::vector<cv::Mat> vectorOfHSVImages;
    cv::cvtColor(imgWarm, imgHSV, cv::COLOR_BGR2HSV);
    cv::split(imgHSV, vectorOfHSVImages);
    cv::Mat s = vectorOfHSVImages[1].clone();
    cv::LUT(s, incr_ch_lut, c_s);
    c_s.convertTo(c_s, CV_8UC1);
    c_s.copyTo(vectorOfHSVImages[1]);
    cv::merge(vectorOfHSVImages, imgSat);
    cv::cvtColor(imgSat.clone(), imgSat, cv::COLOR_HSV2BGR);    
    cv::LUT(imgSat, gamma3_lut, imgGamma);

    // Sharpening:
    cv::Mat imgOut;                                            // Using a gaussian smoothing filter and subtracting the
    cv::GaussianBlur(imgGamma, imgOut, cv::Size(0, 0), 3);     // smoothed version from the original image (in a weighted
    cv::addWeighted(imgGamma, 1.5, imgOut, -0.5, 0, imgOut);   // way so the values of a constant area remain constant)
    
    // Debug:
    if (debugMode) {
        cv::imwrite("img_original_before_enhancement.jpg", imgOriginal);
        cv::imwrite("img_original_after_enhancement.jpg",  imgOut);
    }

    return imgOut;
}

