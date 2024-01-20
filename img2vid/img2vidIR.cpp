#include <iostream>
#include <filesystem>
#include<opencv2/opencv.hpp>
#include<iostream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

#define noOfBins 65535
#define noOfrows   240
#define noOfcolms  320
#define PlateauPercentage 0.1
#define moving_avg_fr   16  

int histMinValue_arr[moving_avg_fr] = {0};
int histMaxValue_arr[moving_avg_fr] = {0};
bool histAvg = false;
int histAvgCounter = 0;

int minThresold = 5;
int maxThresold = 5;
float minMaxDiff = 0;
uint8_t agcLUT[noOfBins] = {0};
uint32_t histogram[noOfBins] = {0};

void byteswapimg(const uchar *imgptr, const uchar *imgOut, int rows, int colms);
int CalculateMean(const uchar *imgptrIn, int rows, int colms);
void calculateOffset(const uchar *imgptrIn, const uchar *offsetOut, int rows, int colms);
void applyNUC(const uchar *imgptrIn, const uchar *offsetOut, int rows, int colms);
void applyAGC(const uchar *imgptrIn, const uchar *offsetOut, int rows, int colms);
void calculateAGCLUT();
void applyHE(const uchar *imgptrIn, const uchar *offsetOut, int rows, int colms);
void calculateHELUT();
void calculatePlateauHELUT();

Mat outpuMat (noOfrows,noOfcolms, CV_16UC1);
Mat offsetMat (noOfrows,noOfcolms, CV_16UC1);
Mat nucOutMat (noOfrows,noOfcolms, CV_16UC1);
Mat agcOutMat (noOfrows, noOfcolms, CV_8UC1);
Mat vid_frame (480, 640, CV_8UC3);

int frameCounter = 0;
std::string folderPath = "/media/kisna/data2/city/Denmark/Copenhagen/16BitImages/output/";

int main() {
    Mat frame;//Declaring a matrix to load the frames//
    cv::Mat zoomedImage;
    int fileNumber = 1; // Example integer
    std::ostringstream fileNameStream;
    std::string outputVideoPath = "output_video.avi";
    // Set the frame size and frames per second (FPS)
    cv::Size frameSize(640, 480);  // Adjust the size according to your frames
    int fps = 30;
    // Open the video writer
    cv::VideoWriter videoWriter(outputVideoPath, cv::VideoWriter::fourcc('X','V','I','D'), fps, frameSize);

    // Check if the video writer is successfully opened
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open the video writer." << std::endl;
        return -1;
    }
    
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.path())) {
            auto start = std::chrono::high_resolution_clock::now();
            fileNameStream.str("");
            fileNameStream.clear();
            fileNameStream <<  fileNumber << ".png";
            std::cout << "File: " << fileNameStream.str() << std::endl;

            std::filesystem::path fileName = fileNameStream.str();
            std::filesystem::path fullPath = folderPath / fileName;

            outpuMat = imread(fullPath, IMREAD_UNCHANGED);
            if (outpuMat.empty()) {
                std::cout << "Could not open or find the image" << std::endl;
                break;
            }
            applyAGC(outpuMat.datastart, agcOutMat.datastart, noOfrows, noOfcolms);
            //applyHE(outpuMat.datastart, agcOutMat.datastart, noOfrows, noOfcolms);

            cv::resize(agcOutMat, zoomedImage, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);
            //imshow("Raw Output", outpuMat);
            //imshow("Agc Output", agcOutMat);
  
            cv::Mat colorImage;
            cv::cvtColor(zoomedImage, colorImage, cv::COLOR_GRAY2BGR); 
            if(fileNumber>50){
                videoWriter.write(colorImage);
            }           
            imshow("Zoomed Output", colorImage);
            char c = (char)waitKey(1); 
            if (c == 27){ //If 'Esc' is entered break the loop//
                break;
            }
            fileNumber = fileNumber + 1;

            auto end = std::chrono::high_resolution_clock::now();

            // Calculate the time difference in milliseconds
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // Output the time difference
            std::cout << "Time taken: " << duration << " milliseconds" << std::endl;
            if(fileNumber > 14000){
                break;
            }
        }
    }
    // Release the video writer
    videoWriter.release();

    std::cout << "Video creation completed." << std::endl;
}

void byteswapimg(const uchar *imgptr, const uchar *imgOut, int rows, int colms)
{  
      uint16_t *tempptr = (uint16_t *) imgptr;
      uint16_t *tempptr_out = (uint16_t *) imgOut;
      for(int i=0;i <colms; i++)
      {
         for(int j=0;j<rows;j++)
         {
            uint16_t tempVar = 0;
            tempVar = *tempptr;
            *tempptr_out =( *tempptr << 8) | (tempVar >> 8);
            tempptr++;
            tempptr_out++;
         }
      }
}

void calculateOffset(const uchar *imgptrIn, const uchar *offsetOut, int rows, int colms)
{
    uint16_t *tempptr = (uint16_t *) imgptrIn;
    uint16_t *tempptr_out = (uint16_t *) offsetOut;
    int imgMean = CalculateMean(imgptrIn, rows, colms);
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            uint16_t tempVar = 0;
            tempVar = *tempptr;
            *tempptr_out = tempVar - imgMean;
            tempptr++;
            tempptr_out++;
        }
    }
}

int CalculateMean(const uchar *imgptrIn, int rows, int colms)
{
    uint16_t *tempptr = (uint16_t *) imgptrIn;
    double imgSum = 0;
    int imgMean = 0;
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            imgSum += *tempptr;
            tempptr++;
        }
    }
    imgMean = int (imgSum/(rows*colms));
    cout << "Mean value is : " << imgMean << endl;
    return imgMean;
}


void applyNUC(const uchar *imgptrIn, const uchar *offsetIn, int rows, int colms)
{
    uint16_t *tempptr = (uint16_t *) imgptrIn;
    uint16_t *tempptr_out = (uint16_t *) offsetIn;
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            *tempptr = *tempptr - *tempptr_out;
            tempptr++;
            tempptr_out++;
        }
    }
}

void applyAGC(const uchar *imgptrIn, const uchar *agcOut, int rows, int colms)
{

    for(int i=0;i<noOfBins;i++)
    {
        histogram[i] = 0;
    }

    uint16_t *tempptr = (uint16_t *) imgptrIn;
    uint8_t *tempptr_out = (uint8_t *) agcOut;
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            uint16_t tmpPix = *tempptr;
            histogram[tmpPix]++;
            *tempptr_out = agcLUT[tmpPix];
            tempptr++;
            tempptr_out++;
        }
    }
    calculateAGCLUT();
}

void calculateAGCLUT()
{   
    int histMinValue = 0;
    int histMaxValue = 0;
    float minMaxDiff = 0;
    for(int i=0;i<noOfBins;i++)
    {
        if (histogram[i] > minThresold)
        {
            histMinValue = i;
            break;
        }
    }

    for(int i=noOfBins;i>1;i--)
    {
        if (histogram[i] > maxThresold)
        {
            histMaxValue = i;
            break;
        }
    }
    if(histAvgCounter>=(moving_avg_fr-2)){
        histMinValue_arr[histAvgCounter] = histMinValue;
        histMaxValue_arr[histAvgCounter] = histMaxValue;
        histAvgCounter = 0;
    }
    else{
        histMinValue_arr[histAvgCounter] = histMinValue;
        histMaxValue_arr[histAvgCounter] = histMaxValue;  
        histAvgCounter++;      
    }


    for(int i=0;i<moving_avg_fr;i++){
        histMinValue = histMinValue + histMinValue_arr[i];
        histMaxValue = histMaxValue + histMaxValue_arr[i];
    }
    histMinValue = histMinValue / moving_avg_fr;
    histMaxValue = histMaxValue / moving_avg_fr;

    minMaxDiff = histMaxValue - histMinValue;
    if (minMaxDiff < 1)
    {
        minMaxDiff = 1;
    }
    cout <<"Min Value is :" << histMinValue<<endl;
    cout << "Max Value is :" << histMaxValue << endl;
    cout << "difference is :" << minMaxDiff << endl;
    for(int i=0;i<noOfBins;i++)
    {
        int tmpPix = ((i - histMinValue)/minMaxDiff)*255;
        if (tmpPix < 0)
        {
            agcLUT[i] = 0;
        }
        else if(tmpPix > 255)
        {
            agcLUT[i] = 255;
        }
        else
        {
            agcLUT[i] = uint8_t(tmpPix);
        }
    }

}


void applyHE(const uchar *imgptrIn, const uchar *agcOut, int rows, int colms)
{

    for(int i=0;i<noOfBins;i++)
    {
        histogram[i] = 0;
    }

    uint16_t *tempptr = (uint16_t *) imgptrIn;
    uint8_t *tempptr_out = (uint8_t *) agcOut;
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            uint16_t tmpPix = *tempptr;
            histogram[tmpPix]++;
            *tempptr_out = agcLUT[tmpPix];
            tempptr++;
            tempptr_out++;
        }
    }
    calculateHELUT();
}

void calculateHELUT()
{   
    int histMinValue = 0;
    int histMaxValue = 0;
    float minMaxDiff = 0;
    for(int i=0;i<noOfBins;i++)
    {
        if (histogram[i] > minThresold)
        {
            histMinValue = i;
            break;
        }
    }

    for(int i=noOfBins;i>1;i--)
    {
        if (histogram[i] > maxThresold)
        {
            histMaxValue = i;
            break;
        }
    }
    minMaxDiff = histMaxValue - histMinValue;
    if (minMaxDiff < 1)
    {
        minMaxDiff = 1;
    }
    cout <<"Min Value is :" << histMinValue<<endl;
    cout << "Max Value is :" << histMaxValue << endl;
    cout << "difference is :" << minMaxDiff << endl;
    for(int i=0;i<noOfBins;i++)
    {
        int tmpPix = ((i - histMinValue)/minMaxDiff)*255;
        if (tmpPix < 0)
        {
            agcLUT[i] = 0;
        }
        else if(tmpPix > 255)
        {
            agcLUT[i] = 255;
        }
        else
        {
            agcLUT[i] = uint8_t(tmpPix);
        }
    }

}


void applyPlateauHE(const uchar *imgptrIn, const uchar *agcOut, int rows, int colms)
{

    for(int i=0;i<noOfBins;i++)
    {
        histogram[i] = 0;
    }

    uint16_t *tempptr = (uint16_t *) imgptrIn;
    uint8_t *tempptr_out = (uint8_t *) agcOut;
    for(int i=0;i <colms; i++)
    {
        for(int j=0;j<rows;j++)
        {
            uint16_t tmpPix = *tempptr;
            histogram[tmpPix]++;
            *tempptr_out = agcLUT[tmpPix];
            tempptr++;
            tempptr_out++;
        }
    }
    calculatePlateauHELUT();
}

void calculatePlateauHELUT()
{   
    int histMinValue = 0;
    int histMaxValue = 0;
    float minMaxDiff = 0;
    for(int i=0;i<noOfBins;i++)
    {
        if (histogram[i] > minThresold)
        {
            histMinValue = i;
            break;
        }
    }

    for(int i=noOfBins;i>1;i--)
    {
        if (histogram[i] > maxThresold)
        {
            histMaxValue = i;
            break;
        }
    }
    minMaxDiff = histMaxValue - histMinValue;
    if (minMaxDiff < 1)
    {
        minMaxDiff = 1;
    }
    cout <<"Min Value is :" << histMinValue<<endl;
    cout << "Max Value is :" << histMaxValue << endl;
    cout << "difference is :" << minMaxDiff << endl;
    for(int i=0;i<noOfBins;i++)
    {
        int tmpPix = ((i - histMinValue)/minMaxDiff)*255;
        if (tmpPix < 0)
        {
            agcLUT[i] = 0;
        }
        else if(tmpPix > 255)
        {
            agcLUT[i] = 255;
        }
        else
        {
            agcLUT[i] = uint8_t(tmpPix);
        }
    }

}