// C++ program for the above approach
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat claheOutMat (512, 640, CV_8UC1);

// Driver code
int main(void)
{
	// Read the image file as
	// imread("default.jpg");
	Mat image = imread("../images/FLIR_video_00001.tiff", IMREAD_UNCHANGED);

	// Error Handling
	if (image.empty()) {
		cout << "Image File "
			<< "Not Found" << endl;

		// wait for any key press
		cin.get();
		return -1;
	}
	normalize(image, claheOutMat, 0, 255, NORM_MINMAX, CV_8U);
	// Show Image inside a window with
	// the name provided
	imshow("Window Name", claheOutMat);

	// Wait for any keystroke
	waitKey(0);
	return 0;
}