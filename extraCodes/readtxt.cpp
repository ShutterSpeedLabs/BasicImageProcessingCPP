#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Replace "your_file.txt" with the actual path to your text file
    const std::string filename = "/media/kisna/dataset/ComputerVision/Thermal_Dataset/CVC-09/CVCInfrared/DayTime/Test/FramesPos/files.txt";

    // Open the file
    std::ifstream inputFile(filename);

    // Check if the file is open
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1; // Return an error code
    }

    // Read the file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        // Process each line here
        std::cout << line << std::endl;
    }

    // Close the file
    inputFile.close();

    return 0;
}
