//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

// OpenCV Libraries
#include <opencv2/highgui.hpp>

// Local Includes
#include "Camera.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<Camera> camera(new Camera(cv::Size(6,9), 5));
    camera->run();
    return (0);
}
