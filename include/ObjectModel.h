//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#ifndef PROJECT_4_OBJECTMODEL_H
#define PROJECT_4_OBJECTMODEL_H

// OpenCV Libraries
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * A class that represents an object model. Used from AR applications
 */
class ObjectModel {

    std::string m_objectType;

    std::string m_PATH;

    std::vector<cv::Vec3f> m_vertices;

    std::vector<cv::Vec3f> m_indices;

public:

    /**
     * The default constructor for an object model
     */
    ObjectModel() = default;

    /**
     * An object model constructor that takes in a path and loads a model
     * @param PATH (const std::string) a filepath
     */
    ObjectModel(const std::string PATH);

    /**
     * getVertices
     * @return (std::vector<cv::Vec3f>) gets the vertices for this object model
     */
    std::vector<cv::Vec3f> getVertices();

    /**
     * getIndices
     * @return (std::vector<cv::Vec3f>) gets the indices for this object model
     */
    std::vector<cv::Vec3f> getIndices();

    /**
     * loadObj
     * @param PATH (const std::string) the path to the obj file
     * @return (bool) whether or not the obj was loaded successfully
     * @does parses an obj file and saves its vertices and indices
     */
    bool loadObj(const std::string PATH);

    /**
     * loadCorners
     * @param rows (int) the number of rows used to calculate corners
     * @param cols (int) the number of columns used to calculate corners
     * @return (bool) whether the 4 corners of the chessboard were loaded successfully
     */
    bool loadCorners(int rows, int cols);

    /**
     * loadAxes
     * @return (bool) whether the 3 axes were loaded successfully
     */
    bool loadAxes();

    /**
     * draw
     * @param src (cv::Mat &) a reference to a image to draw to
     * @param points (std::vector<cv::Point2f>) 2D points in an image
     * @param indices (std::vector<cv::Vec3f>) the indices for the vertices (taken from obj file)
     * @does draws a wireframe image
     */
    static void draw(cv::Mat &src, std::vector<cv::Point2f> points, std::vector<cv::Vec3f> indices);

    /**
     * drawAxes
     * @param src (cv::Mat &) a reference to a image to draw to
     * @param points (std::vector<cv::Point2f>) 2D points in an image
     * @does draws 3D axes in RGB colors
     */
    static void drawAxes(cv::Mat &src, std::vector<cv::Point2f> points);

    /**
     * drawCircles
     * @param src (cv::Mat &) a reference to a image to draw to
     * @param points (std::vector<cv::Point2f>) 2D points in an image
     * @does draws circles in the four corners of the chessboard
     */
    static void drawCircles(cv::Mat &src, std::vector<cv::Point2f> points);

    /**
     * setObjectModel
     * @return (bool) whether or not the object was successfully created
     * @does this is a prompt for selecting what to draw to the screen, then loads it
     */
    bool setObjectModel(const int rows = 6, const int cols = 9);

    /**
     * getObjectType
     * @return (std::string) an object string
     */
    std::string getObjectType();

    /**
     * applyTransform
     * @param transform (cv::Mat) the transformation matrix used to multiply each point
     * @param homogeneous (bool) whether or not to use homogeneous transformations
     */
    void applyTransform(cv::Mat transform, bool homogeneous = false);

    /**
     * eqTriangleVerticesAndCentroid
     * @param sideLength (double) side length of equalateral triangle
     * @param bottomLeft (cv::Point3f) bottom left vertex of equalateral triangle
     * @return (std::vector<cv::Point3d>) 3 vertices in the triangle and centroid
     */
    static std::vector<cv::Point3f> eqTriangleVerticesAndCentroid(double sideLength, cv::Point3f bottomLeft);

    /**
     * centroidTriangleXY
     * @param a (cv::Point3f) triangle vertex a
     * @param b (cv::Point3f) triangle vertex b
     * @param c (cv::Point3f) triangle vertex c
     * @return (cv::Point3f) mean of three vertices
     */
    static cv::Point3f centroidTriangleXY(cv::Point3f a, cv::Point3f b, cv::Point3f c);

    /**
     * starCoordinates
     * @param sideSize (double) size of triangle
     * @param bottomLeft (double) bottom left vertex of equalateral triangle
     * @return (std::vector<std::vector<cv::Vec3f>>) vertices of triangle one (base) and vertices of rotated triangle
     * @does 6 Star of David coordinates paris {(x1,y1),(x2,y,2)} for each line, based on each of the two
     *       "triangles" being equilateral.
     */
    static std::vector<std::vector<cv::Vec3f>> starCoordinates(double sideSize, cv::Vec3f bottomLeft, double theta);

};

#endif //PROJECT_4_OBJECTMODEL_H
