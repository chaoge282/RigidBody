
#ifndef __RIGIDSTATE_H__
#define __RIGIDSTATE_H__

#include "quaternion.h"
#include "Vector.h"
#include "stateDot.h"
class Rigidstate {
public:
    Vector3d xposition;
    Quaternion quater;
    Vector3d pfmom;
    Vector3d lamom;

    Rigidstate();

    Rigidstate operator+(const Rigidstate);

    Rigidstate operator+(const StateDot);

};


#endif
