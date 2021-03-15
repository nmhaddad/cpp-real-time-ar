//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#include <iostream>

// OpenCV Libraries
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d_c.h>

// Local Includes
#include "Camera.h"
#include "Utils.h"
#include "ObjectModel.h"
#include "Transforms.h"

Camera::Camera() {
    // open the video device
    std::unique_ptr <cv::VideoCapture> tempCapDev(new cv::VideoCapture(0));
    capdev = std::move(tempCapDev);
    m_minCalibrationCount = 5;
    m_rows = 6;
    m_cols = 9;
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        exit(-1);
    }
    // get some properties of the image
    cv::Size refS((int) capdev->get(cv::CAP_PROP_FRAME_WIDTH),
                  (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);
    printf("CAMERA CREATED\n");
}

Camera::Camera(cv::Size chessBoardCalibrationSize, int minCalibrationCount) {
    // open the video device
    std::unique_ptr <cv::VideoCapture> tempCapDev(new cv::VideoCapture(0));
    capdev = std::move(tempCapDev);
    m_minCalibrationCount = minCalibrationCount;
    m_rows = chessBoardCalibrationSize.width;
    m_cols = chessBoardCalibrationSize.height;
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        exit(-1);
    }
    // get some properties of the image
    cv::Size refS((int) capdev->get(cv::CAP_PROP_FRAME_WIDTH),
                  (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);
    printf("CAMERA CREATED\n");
}

void Camera::run() {

    std::cout << "Welcome to CV-NINJAS AR Immersive 2D to 3D EXPERIENCE!\n"
                 "Please select an option below:\n"
                 "1 - setup mode (calibrate with THIS camera)\n"
                 "2 - setup mode from files (calibrate with ANY camera)\n"
                 "3 - video mode (using axes, corners, and obj file visualizations)\n"
                 "4 - video mode (with harris corners, detection only)\n"
                 "5 - video mode (with animated stars!)" << std::endl;
    char c;
    do {
        std::cout << "Please enter you selection: ";
        std::cin >> c;
    } while (!std::cin.fail() && c != '1' && c != '2' && c != '3' && c != '4' && c != '5');

    if (c == '1') {
        setup();
    } else if (c == '2') {
        setupFromFiles();
    } else if (c == '3') {
        startVideo();
    } else if (c == '4') {
        startVideoWithHarrisCorners();
    } else if (c == '5') {
        animateTriangle();
    }
}

void Camera::setup() {
    std::cout << "Starting setup" << std::endl;
    std::cout << "Press d to detect corners\n"
                 "Press s to save image for calibration\n"
                 "Press c to calibrate\n"
                 "Press q to quit" << std::endl;
    cv::namedWindow("Video", 1); // identifies a window
    cv::Mat frame;
    bool displayFlag = false;
    for (;;) {
        (*capdev) >> frame; // get a new frame from the camera, treat as a stream

        if (frame.empty()) {
            printf("frame is empty\n");
            break;
        }

        if (displayFlag) {
            std::vector <cv::Point2f> corners = getChessboardCorners(frame, frame);
            if (corners.size() > 0) {
                cv::drawChessboardCorners(frame, cv::Size(m_rows, m_cols), corners, true);
            }
        }

        cv::imshow("Video", frame);

        // see if there is a waiting keystroke
        char key = cv::waitKey(1);
        if (key == 'd') {
            displayFlag = !displayFlag;
        } else if (key == 's') {
            if (addChessBoardCalibrationImage(frame)) {
                std::cout << "Using image for calibration" << std::endl;
            } else {
                std::cout << "Could not use image for calibration" << std::endl;
            }
        } else if (key == 'c') {
            if (calibrate(frame)) {
                std::cout << "Camera calibration complete" << std::endl;
            } else {
                std::cout << "Could not complete camera calibration" << std::endl;
            }
        } else if (key == 'q') {
            break;
        }
    }
    std::cout << "Ending calibration" << std::endl;
}

void Camera::setupFromFiles() {
    std::cout << "You can now use images from a camera of your choice for calibration. "
                 "Please follow the instructions below" << std::endl;
    int remaining = m_minCalibrationCount;
    while (m_calibrationImages.size() != m_minCalibrationCount) {
        std::string imagePath;
        std::cout << "Please type the path to the image, then press ENTER" << std::endl;
        std::cin >> imagePath;
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "ERROR: image is empty" << std::endl;
            exit(-1);
        }
        if (addChessBoardCalibrationImage(image)) {
            std::cout << "Using image for calibration" << std::endl;
            remaining -= 1;
            std::cout << remaining << " more image(s) required for calibration" << std::endl;
        } else {
            std::cout << "Could not use image for calibration" << std::endl;
        }
    }
    if (calibrate(m_calibrationImages[0])) {
        std::cout << "Camera calibration complete" << std::endl;
    } else {
        std::cout << "Could not complete camera calibration" << std::endl;
    }
    std::cout << "Ending calibration" << std::endl;
}

void Camera::startImage() {
    std::string imagePath, objPath, filename;
    std::cout << "Please type the path to the image, then press ENTER" << std::endl;
    std::cin >> imagePath;
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "ERROR: could not load image" << std::endl;
        exit(-1);
    }
    ObjectModel objModel;
    // load in intrinsic parameters
    std::cout << "Enter the file path of the intrinsic parameters, then press enter" << std::endl;
    std::cin >> filename;
    std::vector <cv::Mat> extrinsicParameters = Utils::loadIntrinsicParameters(filename);
    cv::Mat cameraMatrix = extrinsicParameters[0];
    cv::Mat distortionCoefficients = extrinsicParameters[1];

    if (objModel.setObjectModel()) {
        std::vector <cv::Point2f> corners = getChessboardCorners(image, image);
        if (corners.size() > 0) {
            projectPoints(image, corners, cameraMatrix, distortionCoefficients, objModel);
        }
        cv::imshow("CV-NINJAS AR", image);
        cv::waitKey();
        std::string outfile = "../data/img_" + std::to_string(time(0)) + ".jpg";
        cv::imwrite(outfile, image);
    }
    std::cout << "Ending application" << std::endl;
}

void Camera::startVideo() {
    cv::namedWindow("Video", 1); // identifies a window
    cv::Mat frame;
    std::string filename;
    ObjectModel objModel;
    // load in intrinsic parameters
    std::cout << "Enter the file path of the intrinsic parameters, then press enter" << std::endl;
    std::cin >> filename;
    std::vector <cv::Mat> extrinsicParameters = Utils::loadIntrinsicParameters(filename);
    cv::Mat cameraMatrix = extrinsicParameters[0];
    cv::Mat distortionCoefficients = extrinsicParameters[1];
    if (objModel.setObjectModel()) {
        for (;;) {
            (*capdev) >> frame; // get a new frame from the camera, treat as a stream
            if (frame.empty()) {
                std::cerr << "ERROR: frame is empty" << std::endl;
                exit(-1);
            }
            std::vector <cv::Point2f> corners = getChessboardCorners(frame, frame);
            if (corners.size() > 0) {
                projectPoints(frame, corners, cameraMatrix, distortionCoefficients, objModel);
            }
            cv::imshow("Video", frame);
            // see if there is a waiting keystroke
            if (cv::waitKey(1) == 'q') {
                break;
            }
        }
    }
    std::cout << "Ending application" << std::endl;
}

void Camera::startVideoWithHarrisCorners() {
    cv::namedWindow("Video", 1); // identifies a window
    cv::Mat frame;
    for (;;) {
        (*capdev) >> frame; // get a new frame from the camera, treat as a stream
        if (frame.empty()) {
            std::cerr << "ERROR: frame is empty" << std::endl;
            exit(-1);
        }
        harrisCorners(frame);
        cv::imshow("Video", frame);
        // see if there is a waiting keystroke
        if (cv::waitKey(1) == 'q') {
            std::cout << "Ending application" << std::endl;
            break;
        }
    }
    std::cout << "Ending application" << std::endl;
}

std::vector <cv::Point2f> Camera::getChessboardCorners(cv::Mat src, cv::Mat dst) {
    std::vector <cv::Point2f> corners;
    std::cout << "Finding chessboard...";
    bool found = cv::findChessboardCorners(src, cv::Size(m_rows, m_cols), corners, cv::CALIB_CB_ADAPTIVE_THRESH);
    if (found) {
        std::cout << "found" << std::endl;
        std::cout << "numCorners: " << corners.size() << std::endl;
        std::cout << "firstCorner: " << corners[0] << std::endl;
        cv::Mat gray;
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        // size 11,11 , zero region null
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001));
    } else {
        corners.clear();
        std::cout << "not found" << std::endl;
    }
    return corners;
}

std::vector <cv::Vec3f> Camera::getChessboardCornersWorld() {
    std::vector <cv::Vec3f> points;
    std::cout << "Getting chessboard corners world...";
    for (int x = 0; x < m_cols; x++) {
        for (int y = 0; y < m_rows; y++) {
            points.emplace_back(cv::Vec3f(x, -y, 0));
        }
    }
    std::cout << "done" << std::endl;
    return points;
}

bool Camera::addChessBoardCalibrationImage(cv::Mat src) {
    std::vector <cv::Point2f> corners = getChessboardCorners(src, src);
    if (corners.size() > 0) {
        std::vector <cv::Vec3f> points = getChessboardCornersWorld();
        if (corners.size() == points.size()) {
            m_cornerList.emplace_back(corners);
            m_pointList.emplace_back(points);
            m_calibrationImages.emplace_back(src);
            return true;
        }
    }
    return false;
}

bool Camera::calibrate(cv::Mat src) {
    if (m_calibrationImages.size() >= m_minCalibrationCount) {
        cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cameraMatrix.at<double>(0, 2) = src.cols / 2;
        cameraMatrix.at<double>(1, 2) = src.rows / 2;
        cv::Mat distortionCoeffs = cv::Mat::zeros(m_minCalibrationCount, 1, CV_64F);
        std::vector <cv::Mat> rotationVectors, translationVectors;
        cv::Mat stdDevIntrinsics, stdDevExtrinsics, perViewErrors;
        double rms = cv::calibrateCamera(m_pointList, m_cornerList, src.size(), cameraMatrix, distortionCoeffs,
                                         rotationVectors, translationVectors, stdDevIntrinsics, stdDevExtrinsics,
                                         perViewErrors, CV_CALIB_FIX_ASPECT_RATIO);
        std::cout << "##=== CAMERA MATRIX ===============##" << std::endl;
        std::cout << cameraMatrix << std::endl;
        std::cout << "##=== DISTORTION COEFFICIENTS =====##" << std::endl;
        std::cout << distortionCoeffs << std::endl;
        std::cout << "##=== FINAL RE-PROJECTION ERROR ===##" << std::endl;
        std::cout << rms << std::endl;
        m_rotationVectors.emplace_back(rotationVectors);
        m_translationVectors.emplace_back(translationVectors);
        if (Utils::prompt("Save intrinsic parameters to file [y/n]?") == 'y') {
            Utils::saveIntrinsicParameters(cameraMatrix, distortionCoeffs);
        }
        return true;
    }
    return false;
}

bool Camera::projectPoints(cv::Mat &src, std::vector <cv::Point2f> corners, cv::Mat cameraMatrix,
                           cv::Mat distortionCoefficients, ObjectModel &objModel) {
    std::vector <cv::Vec3f> points = getChessboardCornersWorld();
    cv::Mat rotationVector, translationVector;
    cv::solvePnP(points, corners, cameraMatrix, distortionCoefficients, rotationVector, translationVector);
    std::cout << "##====== ROTATION VECTOR ======##" << std::endl;
    std::cout << rotationVector << std::endl;
    std::cout << "##===== TRANSLATION VECTOR ====##" << std::endl;
    std::cout << translationVector << std::endl;
    std::vector <cv::Vec3f> points3D;
    std::vector <cv::Point2f> projectedPoints;
    if (objModel.getObjectType() == "custom") {
        cv::Mat T_ROTZ = Transforms::rotateZ3x3(0.1);
        objModel.applyTransform(T_ROTZ);
    }
    points3D = objModel.getVertices();
    cv::projectPoints(points3D, rotationVector, translationVector, cameraMatrix, distortionCoefficients,
                      projectedPoints);
    if (objModel.getObjectType() == "corners") {
        ObjectModel::drawCircles(src, projectedPoints);
    } else if (objModel.getObjectType() == "axes") {
        ObjectModel::drawAxes(src, projectedPoints);
    } else if (objModel.getObjectType() == "custom") {
        ObjectModel::draw(src, projectedPoints, objModel.getIndices());
    }
    return true;
}

void Camera::harrisCorners(cv::Mat &src) {
    std::cout << "Running Harris Corners...";
    cv::Mat grayscale, dst_norm, dst_norm_scaled;
    cv::cvtColor(src, grayscale, cv::COLOR_BGR2GRAY);
    cv::Mat dst = cv::Mat::zeros(src.size(), CV_32FC1);
    cv::cornerHarris(grayscale, dst, 2, 3, 0.04);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1);
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);
    for (int i = 0; i < dst_norm.rows; i++) {
        for (int j = 0; j < dst_norm.cols; j++) {
            if ((int) dst_norm.at<float>(i, j) > 195) {
                cv::circle(src, cv::Point(j, i), 5, cv::Scalar(0, 0, 255), 2, 8, 0);
            }
        }
    }
    std::cout << "Done" << std::endl;
}

void Camera::animateTriangle() {
    cv::Mat frame;
    std::string filename;
    // load in intrinsic parameters
    std::cout << "Enter the file path of the intrinsic parameters, then press enter" << std::endl;
    std::cin >> filename;
    std::vector <cv::Mat> extrinsicParameters = Utils::loadIntrinsicParameters(filename);
    cv::Mat cameraMatrix = extrinsicParameters[0];
    cv::Mat distortionCoefficients = extrinsicParameters[1];

    double dx = 0.2;
    cv::Point3f origin = cv::Point3f(0.0, 0.0, 0.0);
    double triangleSize = 1;
    cv::namedWindow("Animation", 1);
    double theta = 0;
    while (cv::waitKey(50) != 'q') {
        (*capdev) >> frame;

        std::vector <cv::Point2f> corners = getChessboardCorners(frame, frame);
        if (corners.size() > 0) {
            std::vector <cv::Vec3f> points = getChessboardCornersWorld();
            cv::Mat rotationVector, translationVector;
            cv::solvePnP(points, corners, cameraMatrix, distortionCoefficients, rotationVector, translationVector);
            theta += 5;
            std::vector <std::vector<cv::Vec3f>> starCoordinateVec = ObjectModel::starCoordinates(
                    triangleSize * 2, origin, theta);
            std::vector <cv::Vec3f> t1_points = starCoordinateVec[0];
            std::vector <cv::Point2f> projectedPoints;
            cv::projectPoints(t1_points, rotationVector, translationVector, cameraMatrix, distortionCoefficients,
                              projectedPoints);

            // Draw Points
            cv::circle(frame, projectedPoints[0], 1, cv::Scalar(255, 0, 0), 10);
            cv::circle(frame, projectedPoints[1], 1, cv::Scalar(255, 0, 0), 10);
            cv::circle(frame, projectedPoints[2], 1, cv::Scalar(255, 0, 0), 10);
            cv::line(frame, projectedPoints[0], projectedPoints[1], cv::Scalar(0, 255, 0), 3);
            cv::line(frame, projectedPoints[1], projectedPoints[2], cv::Scalar(0, 255, 0), 3);
            cv::line(frame, projectedPoints[2], projectedPoints[0], cv::Scalar(0, 255, 0), 3);
            projectedPoints.clear();
            std::vector <cv::Vec3f> t2_points = starCoordinateVec[1];
            cv::projectPoints(t2_points, rotationVector, translationVector, cameraMatrix, distortionCoefficients,
                              projectedPoints);
            // Draw Points
            cv::circle(frame, projectedPoints[0], 1, cv::Scalar(0, 255, 0), 10);
            cv::circle(frame, projectedPoints[1], 1, cv::Scalar(0, 255, 0), 10);
            cv::circle(frame, projectedPoints[2], 1, cv::Scalar(0, 255, 0), 10);
            cv::line(frame, projectedPoints[0], projectedPoints[1], cv::Scalar(0, 0, 0), 3);
            cv::line(frame, projectedPoints[1], projectedPoints[2], cv::Scalar(0, 0, 0), 3);
            cv::line(frame, projectedPoints[2], projectedPoints[0], cv::Scalar(0, 0, 0), 3);
            origin.x += dx;
            if (origin.x > 5) {
                origin.y -= 1;
                origin.x = 0;
            }
            if (origin.y < -7) {
                origin = cv::Point3f(0, 0, 0);
            }
        }
        cv::imshow("Animation", frame);
    }
    exit(0);
}
