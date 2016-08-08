#ifndef STRIXEL_ALGSIMPLE_H
#define STRIXEL_ALGSIMPLE_H

#include <atomic>

class Solution;

class AlgSimple
{
public:
    AlgSimple();
    virtual ~AlgSimple() = default;

    virtual void draw();
    virtual void calculate(Solution& solution);

private:
    std::atomic<bool> calculating_;
    std::atomic<int> currentPass_;
    bool morePassesPerPinPair_ = false;
    int penalty_ = 10;
    int minSpace_ = 0;
};

#endif // STRIXEL_ALGIMPLE_H
