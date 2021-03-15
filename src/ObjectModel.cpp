//
// CS 5330 - Project 4
// Nathaniel Haddad and Stephen Dorris
//

#include <iostream>

// Local Includes
#include "ObjectModel.h"
#include "Transforms.h"

ObjectModel::ObjectModel(const std::string PATH) : m_PATH(PATH) {
    if (loadObj(PATH)) {
        std::cout << "Successfully loaded object from file" << std::endl;
    } else {
        std::cerr << "ERROR: could not load object from file" << std::endl;
        exit(-1);
    }
    std::cout << "Successfully created object model" << std::endl;
}

std::vector<cv::Vec3f> ObjectModel::getVertices() {
    return m_vertices;
}

std::vector<cv::Vec3f> ObjectModel::getIndices() {
    return m_indices;
}

std::string ObjectModel::getObjectType() {
    return m_objectType;
}

bool ObjectModel::loadCorners(int rows, int cols) {
    // four corners of chessboard (order: top left, top right, bottom left, bottom right)
    m_vertices = std::vector <cv::Vec3f>{cv::Vec3f(0, 0, 0), cv::Vec3f(cols - 1, 0, 0),
                                       cv::Vec3f(0, -(rows - 1), 0),
                                       cv::Vec3f(cols - 1, -(rows - 1), 0)};
    m_objectType = "corners";
    return true;
}

bool ObjectModel::loadAxes() {
    m_vertices = std::vector <cv::Vec3f> {cv::Vec3f(0,0,0), cv::Vec3f(1,0,0), cv::Vec3f(0,1,0), cv::Vec3f(0,0,-1)};
    m_objectType = "axes";
    return true;
}

bool ObjectModel::loadObj(const std::string PATH) {
    m_PATH = PATH;
    std::vector <cv::Vec3f> vertices;
    std::vector<unsigned int> vertexIndices;
    FILE *file = fopen(PATH.c_str(), "r");
    if (file == nullptr) {
        std::cout << "Error: cannot open given file" << std::endl;
        return false;
    }
    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "v") == 0) {
            cv::Vec3f vertex;
            fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
            m_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1],
                                 &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
            if (matches != 6) {
                std::cerr << "Error: file with texture indices can't be read the parser" << std::endl;
                return false;
            }
            m_indices.push_back(cv::Vec3f(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
        }
    }
    m_objectType = "custom";
    return true;
}

void ObjectModel::draw(cv::Mat &src, std::vector<cv::Point2f> points, std::vector<cv::Vec3f> indices) {
    for (cv::Vec3f &indexVec : indices) {
        unsigned int v1 = indexVec[0] - 1;
        unsigned int v2 = indexVec[1] - 1;
        unsigned int v3 = indexVec[2] - 1;
        cv::line(src, points[v1], points[v2], cv::Scalar(255,0,0), 1);
        cv::line(src, points[v2], points[v3], cv::Scalar(255,0,0), 1);
        cv::line(src, points[v3], points[v1], cv::Scalar(255,0,0), 1);
    }
}

void ObjectModel::drawAxes(cv::Mat &src, std::vector<cv::Point2f> points) {
    cv::Point2f origin = points[0];
    cv::line(src, origin, points[1], cv::Scalar(255,0,0),4);
    cv::line(src, origin, points[2], cv::Scalar(0,255,0),4);
    cv::line(src, origin, points[3], cv::Scalar(0,0,255),4);
}

void ObjectModel::drawCircles(cv::Mat &src, std::vector<cv::Point2f> points) {
    for (cv::Point2f &point : points) {
        cv::circle(src, point, 10, cv::Scalar(0,0,255),cv::FILLED);
    }
}

bool ObjectModel::setObjectModel(const int rows, const int cols) {
    std::string objPath;
    char character;
    std::cout << "Choose a object to display:\n"
                 "1 - four corners\n"
                 "2 - colored axes\n"
                 "3 - load an object model from a file (.obj ext)" << std::endl;
    do {
        std::cin >> character;
    }
    while(!std::cin.fail() && character != '1' && character != '2' && character != '3');

    if (character == '1') {
        loadCorners(rows, cols);
    } else if (character == '2') {
        loadAxes();
    } else if (character == '3') {
        std::cout << "Please enter a path to an obj file" << std::endl;
        std::cin >> objPath;
        if (loadObj(objPath)) {
            std::cout << "Successfully loaded object from file" << std::endl;
        } else {
            std::cerr << "ERROR: could not load object from file" << std::endl;
            return false;
        }
        std::cout << "Successfully created object model" << std::endl;
        cv::Mat T_ROTX = Transforms::rotateX3x3(-(M_PI/2.0f));
        cv::Mat T_TRAN(cv::Vec3f(0, 0, 0));
        cv::Mat T = Transforms::createHomogeneousTransform(T_ROTX, T_TRAN, 5);
        applyTransform(T, true);
    }
    return true;
}

void ObjectModel::applyTransform(cv::Mat T_MATRIX, bool homogeneous) {
    if (homogeneous) {
        for (cv::Vec3f &v : m_vertices) {
            cv::Mat vec(cv::Vec4f(v[0], v[1], v[2], 1));
            cv::Mat vTransformed = T_MATRIX * vec;
            v = cv::Vec3f(vTransformed.at<float>(0,0), vTransformed.at<float>(0,1), vTransformed.at<float>(0,2));
        }
    } else {
        for (cv::Vec3f &v : m_vertices) {
            cv::Mat vTransformed = T_MATRIX * cv::Mat(v);
            v = cv::Vec3f(vTransformed);
        }
    }
}

cv::Point3f ObjectModel::centroidTriangleXY(cv::Point3f a, cv::Point3f b, cv::Point3f c) {
    return cv::Point3f((a.x + b.x + c.x) / 3.0, (a.y + b.y + c.y) / 3.0, (a.z + b.z + c.z) / 3.0);
}

std::vector<cv::Point3f> ObjectModel::eqTriangleVerticesAndCentroid(double sideSize, cv::Point3f bottomLeft) {
    std::vector<cv::Point3f> ret(4);
    ret[0] = bottomLeft;
    double topCornerHeight =
            bottomLeft.y + sqrt((sideSize * sideSize) - ((.5 * sideSize) * (.5 * sideSize))); // pythagorean theorem
    ret[1] = cv::Point3f(bottomLeft.x + (sideSize / 2.0), topCornerHeight, bottomLeft.z);
    ret[2] = cv::Point3f(bottomLeft.x + sideSize, bottomLeft.y, bottomLeft.z);
    ret[3] = centroidTriangleXY(ret[0], ret[1], ret[2]);
    return ret;
}

std::vector<std::vector<cv::Vec3f>> ObjectModel::starCoordinates(double sideSize, cv::Vec3f bottomLeft, double theta) {
    std::vector<cv::Point3f> triangleAndCentroid = eqTriangleVerticesAndCentroid((double) sideSize, bottomLeft);
    std::vector<cv::Mat> t1_vec;
    for (int i = 0; i < 3; i++) {
        cv::Mat vert = cv::Mat::ones(4, 1, CV_64FC1);
        vert.at<double>(0, 0) = triangleAndCentroid[i].x;
        vert.at<double>(1, 0) = triangleAndCentroid[i].y;
        vert.at<double>(2, 0) = triangleAndCentroid[i].z;
        vert.at<double>(3, 0) = 1.0;
        t1_vec.emplace_back(vert);
    }
    // compute translation necessary for rotation about center of t1 w/
    cv::Point3f centerT1 = triangleAndCentroid[3];
    centerT1.x *= -1;
    centerT1.y *= -1;
    std::vector<cv::Mat> t2_vec;
    for (cv::Mat t1Point :t1_vec) {
        cv::Mat t2point = cv::Mat::ones(t1Point.size(), t1Point.type());
        for (int r = 0; r < t1Point.rows; r++) {
            t2point.at<double>(r, 0) = t1Point.at<double>(r, 0);
        }
        Transforms::rotateZ(t1Point, t2point, theta, false, centerT1);
        t2_vec.emplace_back(t2point);
    }
    std::vector<std::vector<cv::Vec3f>> vertices(2);
    for (int i = 0; i < t1_vec.size(); i++) {
        cv::Mat t1Mat = t1_vec[i];
        cv::Mat t2Mat = t2_vec[i];
        cv::Vec3f t1_i = cv::Point3f(t1Mat.at<double>(0, 0), t1Mat.at<double>(1, 0), t1Mat.at<double>(2, 0));
        cv::Vec3f t2_i = cv::Point3f(t2Mat.at<double>(0, 0), t2Mat.at<double>(1, 0), t2Mat.at<double>(2, 0));
        vertices[0].emplace_back(t1_i);
        vertices[1].emplace_back(t2_i);
    }
    return vertices;
}
