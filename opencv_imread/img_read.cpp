#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // The path to the image file
    std::string imagePath = "/media/kisna/dataset/codes/VSCode/BasicImageProcessingCPP/images/FLIR_video_00001.tiff";

    // Read the image file using imread function
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_UNCHANGED);

    // Check for failure
    if (image.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    // Get the image height and width
    int height = image.rows;
    int width = image.cols;
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

    return 0;
}