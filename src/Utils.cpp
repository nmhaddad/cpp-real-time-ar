//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#include <iostream>

// Local Includes
#include "Utils.h"
#include "Transforms.h"

void Utils::saveIntrinsicParameters(cv::Mat cameraMatrix, cv::Mat distortionCoefficients) {
    std::cout << "Saving intrinsic parameters...";
    std::string filename = "../data/intrinsics_" + std::to_string(time(0)) + ".yml";
    cv::FileStorage file(filename, cv::FileStorage::WRITE);
    file << "cameraMatrix" << cameraMatrix;
    file << "distortionCoefficients" << distortionCoefficients;
    std::cout << "Done!" << std::endl;
}

std::vector<cv::Mat> Utils::loadIntrinsicParameters(std::string filename) {
    cv::Mat cameraMatrix, distortionCoefficients;
    cv::FileStorage file(filename, cv::FileStorage::READ);
    file["cameraMatrix"] >> cameraMatrix;
    file["distortionCoefficients"] >> distortionCoefficients;
    file.release();
    std::cout << "##======== CAMERA MATRIX ==========##" << std::endl;
    std::cout << cameraMatrix << std::endl;
    std::cout << "##=== DISTORTION COEFFICIENTS =====##" << std::endl;
    std::cout << distortionCoefficients << std::endl;
    std::vector<cv::Mat> intrinsicParameters{cameraMatrix, distortionCoefficients};
    return intrinsicParameters;
}

char Utils::prompt(std::string text) {
    char character;
    do {
        std::cout << text << std::endl;
        std::cin >> character;
    }
    while(!std::cin.fail() && character != 'y' && character != 'n');
    return character;
}
