//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

// Local Includes
#include "Transforms.h"

cv::Mat Transforms::uniformScale3x3(const double factor) {
    cv::Mat T_SCALE = cv::Mat::eye(3, 3, CV_32F) * factor;
    return T_SCALE;
}

cv::Mat Transforms::rotateX3x3(const double theta) {
    cv::Mat T_X = cv::Mat::eye(3, 3, CV_32F);
    T_X.at<float>(1,1) = cos(theta);
    T_X.at<float>(1,2) = -sin(theta);
    T_X.at<float>(2,1) = sin(theta);
    T_X.at<float>(2,2) = cos(theta);
    return T_X;
}

cv::Mat Transforms::rotateY3x3(const double theta) {
    cv::Mat T_Y = cv::Mat::eye(3, 3, CV_32F);
    T_Y.at<float>(0,0) = cos(theta);
    T_Y.at<float>(0,2) = sin(theta);
    T_Y.at<float>(2,0) = -sin(theta);
    T_Y.at<float>(2,2) = cos(theta);
    return T_Y;
}

cv::Mat Transforms::rotateZ3x3(const double theta) {
    cv::Mat T_Z = cv::Mat::eye(3, 3, CV_32F);
    T_Z.at<float>(0,0) = cos(theta);
    T_Z.at<float>(0,1) = -sin(theta);
    T_Z.at<float>(1,0) = sin(theta);
    T_Z.at<float>(1,1) = cos(theta);
    return T_Z;
}

cv::Mat Transforms::createHomogeneousTransform(cv::Mat rotation, cv::Mat translation, const double uniformScale) {
    cv::Mat T = cv::Mat::eye(4, 4, CV_32F);
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col <3; col++) {
            T.at<float>(row, col) = rotation.at<float>(row, col);
        }
    }
    T.at<float>(0, 3) = translation.at<float>(0, 1);
    T.at<float>(1, 3) = translation.at<float>(0, 2);
    T.at<float>(2, 3) = translation.at<float>(0, 3);
    return T * uniformScale;
}

void Transforms::translateH(const cv::Mat src, cv::Mat dst, const cv::Vec3f translationXYZ) {
    cv::Mat T = cv::Mat::eye(4, 4, CV_64FC1);
    // Populate T Mat
    T.at<double>(0, 3) = translationXYZ[0];
    T.at<double>(1, 3) = translationXYZ[1];
    T.at<double>(2, 3) = translationXYZ[2];
    dst = T * src;
}

void Transforms::rotateZ(cv::Mat src, cv::Mat dst, double theta, bool aboutOrigin, cv::Vec3f translationToOrigin) {
    const double halfC = M_PI / 180.0;
    double radians = theta * halfC;
    // To simulate rotating point about an arbitrary point, we translate it to the origin
    // (in the triangle case we want to translate it such that the center of the triangle is at
    // the origin.
    if (!aboutOrigin) {
        translateH(dst, dst, translationToOrigin);
    }
    cv::Mat Rz = cv::Mat::eye(4, 4, CV_64FC1);
    Rz.at<double>(0, 0) = cos(radians);
    Rz.at<double>(0, 1) = -1 * sin(radians);
    Rz.at<double>(1, 0) = sin(radians);
    Rz.at<double>(1, 1) = cos(radians);
    dst = Rz * dst;
    // If we are simulating rotation about a certain point, once we rotate it (about the origin)
    // we translate it back to where it was.
    if (!aboutOrigin) {
        translateH(dst, dst, cv::Point3f(translationToOrigin[0] * -1, translationToOrigin[1] * -1, 0.0));
    }
}

void Transforms::scaleH(const cv::Mat src, cv::Mat dst, const cv::Vec3f scaleXYZ) {
    // Populate S Mat
    cv::Mat S = cv::Mat::eye(4, 4, CV_64FC1);
    S.at<double>(0, 0) = scaleXYZ[0];
    S.at<double>(1, 1) = scaleXYZ[1];
    S.at<double>(2, 2) = scaleXYZ[2];
    dst = src * S;
}
