#include <iostream>
#include <sstream>
#include <iomanip>


int main() {
    int number = 42; // Replace this with your integer

    // Use std::ostringstream to convert the integer to a string
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << number;

    // Get the resulting string
    std::string result = oss.str();

    // Print the result
    std::cout << "Original integer: " << number << std::endl;
    std::cout << "Converted string: " << result << std::endl;

    return 0;
}
