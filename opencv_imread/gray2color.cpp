#include <opencv2/opencv.hpp>

int main() {
    // Load a single-channel grayscale image
    cv::Mat grayImage = cv::imread("../images/FLIR_video_00001.tiff", cv::IMREAD_UNCHANGED);

   // Check if the image has been loaded properly
    if (grayImage.empty()) {
        std::cerr << "Error: Image cannot be loaded." << std::endl;
        return -1;
    }

    // Create a vector of 3 grayscale images
    std::vector<cv::Mat> channels(3);
    channels[0] = grayImage; // Copy gray image into the first channel
    channels[1] = grayImage; // Copy gray image into the second channel
    channels[2] = grayImage; // Copy gray image into the third channel

    // Merge the single channels into a 3-channel image
    cv::Mat colorImage;
    cv::merge(channels, colorImage);

    // Display the result
    cv::imshow("Original Grayscale Image", grayImage);
    cv::imshow("Converted Color Image", colorImage);

    // Wait for a key press indefinitely
    cv::waitKey(0);

    return 0;
}