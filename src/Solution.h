#ifndef STRIXEL_SOLUTION_H
#define STRIXEL_SOLUTION_H

#include <opencv2/opencv.hpp>

#include <vector>

class Solution
{
public:
    Solution(const cv::Mat image);

    virtual void draw();

    bool empty() const {
        return pattern_.empty();
    }

    cv::Mat getImage() const {
        return image_;
    }

    int getPasses() const {
        return passes_;
    }

    int getPins() const {
        return pins_;
    }

    void push_back(int pin) {
        pattern_.push_back(pin);
    }

    std::vector<int>::iterator begin() {
        return pattern_.begin();
    }

    std::vector<int>::iterator end() {
        return pattern_.end();
    }

    std::vector<int>::const_iterator begin() const {
        return pattern_.cbegin();
    }

    std::vector<int>::const_iterator end() const {
        return pattern_.cend();
    }

    std::vector<int>::const_iterator cbegin() const {
        return pattern_.cbegin();
    }

    std::vector<int>::const_iterator cend() const {
        return pattern_.cend();
    }

private:
    cv::Mat image_;
    int passes_ = 3000;
    float pinThickness_ = 1.0;
    int pins_ = 200;
    std::vector<int> pattern_;
};

#endif // STRIXEL_SOLUTION_H
