#ifndef STRIXEL_ALGSIMPLE_H
#define STRIXEL_ALGSIMPLE_H

class Solution;

class AlgSimple
{
public:
    virtual void draw();
    virtual void calculate(Solution& solution);

private:
    int penalty_ = 10;
    int minSpace_ = 0;
};

#endif // STRIXEL_ALGIMPLE_H
