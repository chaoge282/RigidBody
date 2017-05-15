#ifndef __RIGIDSTATEA_H__
#define __RIGIDSTATEA_H__

#include "quaternion.h"
#include "Vector.h"
class StateDot {
public:
    Vector3d velocity;
    Quaternion quaterA;
    Vector3d force;
    Vector3d torque;

    StateDot();

    StateDot operator*(double h);
    StateDot operator+(const StateDot a);
};

#endif
