//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#ifndef PROJECT_4_TRANSFORMS_H
#define PROJECT_4_TRANSFORMS_H

// OpenCV Libraries
#include <opencv2/core.hpp>

/**
 * A class used for creating transforms
 */
class Transforms {

public:

    /**
     * uniformScale3x3
     * @param factor (const double) a factor at which to scale vertices
     * @return (cv::Mat) a matrix used for uniform scaling
     */
    static cv::Mat uniformScale3x3(const double factor);

    /**
     * rotateX3x3
     * @param theta (const double) an angle theta used to rotate (radians)
     * @return (cv::Mat) a 3x3 rotation matrix about the x-axis
     */
    static cv::Mat rotateX3x3(const double theta);

    /**
     * rotateY3x3
     * @param theta (const double) an angle theta used to rotate (radians)
     * @return (cv::Mat) a 3x3 rotation matrix about the y-axis
     */
    static cv::Mat rotateY3x3(const double theta);

    /**
     * rotateZ3x3
     * @param theta (const double) an angle theta used to rotate (radians)
     * @return (cv::Mat) a 3x3 rotation matrix about the z-axis
     */
    static cv::Mat rotateZ3x3(const double theta);

    /**
     * createHomogeneousTransform
     * @param rotation (cv::Mat) rotation matrix (3x3)
     * @param translation (cv::Mat) translation matrix (1x3)
     * @param uniformScale (const double) uniform scaling value
     * @return (cv::Mat) a homogeneous transformation matrix
     */
    static cv::Mat createHomogeneousTransform(cv::Mat rotation, cv::Mat translation, const double uniformScale = 1);

    /**
     * translate
     * Translate a homogenous 3D Point by  tx,ty,tz
     * @param src (const cv::Mat) a 4x1 Matrix CV_64FC1 to be translated.
     * @param dst   (const cv::Mat) translated 4x1 Matrix CV_64FC1
     * @param translationXYZ (const cv::Vec3f) tx, ty, tz.
     */
    static void translateH(const cv::Mat src, cv::Mat dst, const cv::Vec3f translationXYZ);

    /**
      * scale
      * Scale a homogenous 3D Point by a sx,sy,sz
      * @param src (const cv::Mat) a 4x1 Matrix CV_64FC1 to be scaled.
      * @param dst   (const cv::Mat) translated 4x1 Matrix CV_64FC1
      * @param scaleXYZ (const cv::Vec3f) sx, sy, sz.
      */
    static void scaleH(const cv::Mat src, cv::Mat dst, const cv::Vec3f scaleXYZ);

    /**
     * Rotate a homogenous 3D Point by about the Z axis for degree amount, either about origin, or about an
     * arbitrary point (which is done by a translation (from the desired center of rotation)
     * to the origin,a rotation, then with the negated translation that (-1 * translation).
     * @param src (cv::Mat)
     * @param dst (cv::Mat)
     * @param theta (double) the degree amount of the rotation.
     * @param aboutOrigin (bool) determines whether translations occur to rorate src by an artbitrary point.
     * @param translationToOrigin (cv::Vec3f) populated with values only if aboutOrigin is true.
     */
    static void rotateZ(cv::Mat src, cv::Mat dst, double theta, bool aboutOrigin, cv::Vec3f translationToOrigin);

};

#endif //PROJECT_4_TRANSFORMS_H
