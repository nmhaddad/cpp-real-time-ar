//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#ifndef PROJECT_4_CAMERA_H
#define PROJECT_4_CAMERA_H

// OpenCV Libraries
#include <opencv2/core.hpp>

// Local Includes
#include "ObjectModel.h"

/**
 * Represents our camera used to represent virtual objects in scene
 */
class Camera {

    std::vector<std::vector<cv::Vec3f>> m_pointList;

    std::vector<std::vector<cv::Point2f>> m_cornerList;

    std::vector<cv::Mat> m_calibrationImages; // images used for calibrating

    std::unique_ptr<cv::VideoCapture> capdev; // video capture device

    std::vector<std::vector<cv::Mat>> m_rotationVectors, m_translationVectors;

    int m_rows, m_cols; // size of checkerboard

    int m_minCalibrationCount; // number of calibrations saved images necessary for calibration process to start.

    /**
     * getChessboardCorners
     * @param src (cv::Mat) the input image
     * @param dst (cv::Mat)
     * @return (std::vector<cv::Point2f>) a vector of two dimensional corners
     * @does identifies a chessboard in an image and returns the pixel coordinates of the corners in the
     *       chessboard
     */
    std::vector<cv::Point2f> getChessboardCorners(cv::Mat src, cv::Mat dst);

    /**
     * getChessboardCornersWorld
     * @return (std::vector<cv::Vec3f>) the 3D coordinates in world frame.
     * @does gets the 3D coordinates for the chessboard in world frame with top left corner of chessboard as
     *       origin.
     */
    std::vector<cv::Vec3f> getChessboardCornersWorld();

    /**
     * addChessBoardCalibrationImage
     * @param src (cv::Mat) an image from which the chessboard corners will be found.
     * @return bool true if calibration process worked and camera euclid/world coords were added properly.
     */
    bool addChessBoardCalibrationImage(cv::Mat src);

    /**
     * calibrate
     * @param src (cv::Mat) the input image
     * @return (bool) whether the operation was successful or not
     * @does Computes intrinsic parameters. Requires 5 images to calibrate. Provides option to save intrinsic
     *       parameters to a file.
     */
    bool calibrate(cv::Mat src);

    /**
     * setup
     * @does is used to perform calibration, display corners, etc. (non AR stuff)
     */
    void setup();

    /**
     * setupFromFiles
     * @does is used to perform calibration, display corners, etc. (non AR stuff) from a selection of files
     */
    void setupFromFiles();

    /**
     * startImage
     * @does starts the single-image based augmented reality application
     */
    void startImage();

    /**
     * startVideo
     * @does is used to start the video-based augmented reality
     */
    void startVideo();

    /**
     * startVideoWithHarrisCorners
     * @does runs the video application with harris corners (detection mode ONLY)
     */
    void startVideoWithHarrisCorners();

    /**
     * projectPoints
     * @param src (cv::Mat) image to project points to
     * @param corners (std::vector<cv::Point2f>) the corners of the detected pattern
     * @param cameraMatrix (cv::Mat) intrinsic camera matrix
     * @param distortionCoefficients (cv::Mat) intrinsic distortion coefficients
     * @param objModel (ObjectModel) an object model
     * @return (bool) success of fail
     */
    bool projectPoints(cv::Mat &src, std::vector<cv::Point2f> corners, cv::Mat cameraMatrix,
                               cv::Mat distortionCoefficients, ObjectModel &objModel);

    /**
     * animateTriangle
     * @does Runs Triangle Animation (one-off)
     */
    void animateTriangle();

    /**
     * harrisCorners
     * @param (cv::Mat &) an input image
     * @does Showcases Harris Corner Detection
     */
    void harrisCorners(cv::Mat &src);

public:

    /**
     * Constructor used to create camera objects
     */
    Camera();

    /**
     * Constructor used to create camera objects
     * @param chessBoardCalibrationSize (cv::Size) the size of the chessboard in rows and columns
     * @param minCalibrationCount (int) the minimum number of images needed to calibrate
     */
    Camera(cv::Size chessBoardCalibrationSize, int minCalibrationCount);

    /**
     * Starts the camera-based application
     */
    void run();

};

#endif //PROJECT_4_CAMERA_H
