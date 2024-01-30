#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono> // Added for chrono
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;


int listFilesInFolder(const std::string& folderPath) {
    std::ostringstream fileNameStream;
    std::string outputVideoPath = "output_video1.avi";
    std::string filename = folderPath + "/files.txt";
    // Read the file line by line
    std::ifstream inputFile(filename);
    // Set the frame size and frames per second (FPS)
    cv::Size frameSize(640, 480);  // Adjust the size according to your frames
    int fps = 20;
    // Open the video writer
    cv::VideoWriter videoWriter(outputVideoPath, cv::VideoWriter::fourcc('X','V','I','D'), fps, frameSize);
    // Check if the video writer is successfully opened
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open the video writer." << std::endl;
        return -1;
    }

    std::string line;
    std::string img_name;
    while (std::getline(inputFile, line)) {
        auto start = std::chrono::high_resolution_clock::now();
        // Process each line here
        std::cout << line << std::endl;                         
        img_name = line;
        img_name.erase(img_name.size() - 1);
        std::filesystem::path fileName_path = img_name; // Removed basic_string; // Removed str()

        std::filesystem::path fullPath = folderPath / fileName_path; // Corrected the path construction

        // Read the image file using imread function
        cv::Mat image = cv::imread(fullPath, cv::IMREAD_UNCHANGED);

        // Check for failure
        if (image.empty()) {
            std::cout << "Could not open or find the image" << std::endl;
            return -1;
        }

        cv::Mat colorImage;
        cv::cvtColor(image, colorImage, cv::COLOR_GRAY2BGR); 
        videoWriter.write(colorImage);        

        // Get the image height and width
        int height = image.rows;
        int width = image.cols;

        // Get the number of channels in the image
        int NoOfchannels = image.channels();

        // Print the number of channels
        std::cout << "Number of Channels: " << NoOfchannels << std::endl;
        // Print image dimensions
        std::cout << "Image Height: " << height << std::endl;
        std::cout << "Image Width: " << width << std::endl;

        // Display the image in a window
        cv::namedWindow("Window");
        cv::imshow("Window", colorImage);

        // Wait for any keystroke in the window
        cv::waitKey(1);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Processing time: " << duration.count() << " seconds" << std::endl;
    }
        // Release the video writer
    videoWriter.release();

    std::cout << "Video creation completed." << std::endl;
    return 0;
}

int main() {
    //std::string folderPath = "/media/kisna/data2/city/Denmark/Copenhagen/16BitImages/output/";
    std::string folderPath = "/media/kisna/dataset/ComputerVision/Thermal_Dataset/CVC-09/CVCInfrared/DayTime/Test/FramesPos";
    int res = 0;

    res = listFilesInFolder(folderPath);
    if (res < 0)
        std::cout << "Error in listFilesInFolder" << std::endl;

    return 0;
}
