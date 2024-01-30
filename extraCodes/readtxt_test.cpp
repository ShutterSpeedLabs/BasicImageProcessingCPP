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

        // Create a vector of 3 grayscale images
        std::vector<cv::Mat> channels(3);
        channels[0] = image; // Copy gray image into the first channel
        channels[1] = image; // Copy gray image into the second channel
        channels[2] = image; // Copy gray image into the third channel

        // Merge the single channels into a 3-channel image
        cv::Mat colorImage;
        cv::merge(channels, colorImage);

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
        cv::waitKey();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Processing time: " << duration.count() << " seconds" << std::endl;
    }
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
