#ifndef STRIXEL_PREVIEW_H
#define STRIXEL_PREVIEW_H

#include "Solution.h"

#include <GL/gl3w.h>
#include <opencv2/opencv.hpp>

#include <vector>

class Preview
{
public:
    Preview(const Solution& solution);
    virtual ~Preview() = default;

    virtual void draw(const Solution& solution);

private:
    static int nextPreviewID_;
    int id_;
    bool showImage_   = true;
    bool showBorder_  = false;
    bool showPins_    = true;
    bool showStrings_ = true;

    float stringAlpha_ = 0.2;
    float pinThickness = 1.0;
    float backgroundColor_[4] = {1.0, 1.0, 1.0, 1.0};
    float stringColor_[4] = {0.0, 0.0, 0.0, 1.0};

    int pins = 200;

    GLuint textureID_;
    cv::Mat image_;
};

#endif // STRIXEL_PREVIEW_H
