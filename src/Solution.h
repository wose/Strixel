#ifndef STRIXEL_SOLUTION_H
#define STRIXEL_SOLUTION_H

#include <vector>

class Solution
{
public:
    virtual void draw();

    void push_back(int pin) {
        pattern_.push_back(pin);
    }

    std::vector<int>::iterator begin() {
        return pattern_.begin();
    }
    std::vector<int>::iterator end() {
        return pattern_.end();
    }
    std::vector<int>::const_iterator cbegin() const {
        return pattern_.cbegin();
    }
    std::vector<int>::const_iterator cend() const {
        return pattern_.cend();
    }

private:
    int passes_ = 3000;
    float pinThickness_ = 1.0;
    int pins_ = 200;
    std::vector<int> pattern_;
};

#endif // STRIXEL_SOLUTION_H
