#include <iostream>
#include <filesystem>
#include <iostream>
#include<opencv2/opencv.hpp>

namespace fs = std::filesystem;

void listFilesInFolder(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.path())) {
            std::cout << "File: " << entry.path().filename() << std::endl;
            // Join the directory path and file name
            std::filesystem::path fullPath = folderPath / entry.path().filename();
                // Read the image file using imread function
            cv::Mat image = cv::imread(fullPath, cv::IMREAD_UNCHANGED);

            // Check for failure
            if (image.empty()) {
                std::cout << "Could not open or find the image" << std::endl;
                //return -1;
            }
            // Get the image height and width
            int height = image.rows;
            int width = image.cols;
            // Get the number of channels in the image
            int channels = image.channels();

            // Print the number of channels
            std::cout << "Number of Channels: " << channels << std::endl;
            // Print image dimensions
            std::cout << "Image Height: " << height << std::endl;
            std::cout << "Image Width: " << width << std::endl;
            
            // Display the image in a window
            cv::namedWindow("Window");
            cv::imshow("Window", image);

            // Wait for any keystroke in the window
            cv::waitKey(0);
        }
    }
}

int main() {
    std::string folderPath = "/media/kisna/data2/city/Denmark/Copenhagen/16BitImages/output/";
    
    listFilesInFolder(folderPath);

    return 0;
}

