//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#ifndef PROJECT_4_UTILS_H
#define PROJECT_4_UTILS_H

// OpenCV Libraries
#include <opencv2/core.hpp>

/**
 * A class for utility methods
 */
class Utils {

public:

    /**
     * saveIntrinsicParameters
     * @param cameraMatrix (cv::Mat) the camera matrix
     * @param distortionCoefficients (cv::Mat) the distortion coefficients
     * @does writes the intrinsic parameters to a file (titled instrinsic_{TIME}.yml)
     */
    static void saveIntrinsicParameters(cv::Mat cameraMatrix, cv::Mat distortionCoefficients);

    /**
     * loadIntrinsicParameters
     * @param filename (std::string) the filepath to a yml file containing intrinsic parameters
     * @return (std::vector<cv::Mat>) a vector of two instrinsic parameters (camera matrix and
     *         distortion coefficients
     */
    static std::vector<cv::Mat> loadIntrinsicParameters(std::string filename);

    /**
     * prompt
     * @param text (std::string) the text part of the prompt that should appear to the user
     * @return (char) 'y' or 'n' depending on user input
     * @does a resuable prompt that allows text to be passed in to be displayed
     */
    static char prompt(std::string text);

};

#endif //PROJECT_4_UTILS_H
