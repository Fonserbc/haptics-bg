#ifndef CUBE_HPP
#define CUBE_HPP

#include "chai3d.h"

class Cube {
public:
    double transparency;

    Cube(cWorld* world, cVector3d position, double size);

    cMaterial material;
};
#endif // CUBE_HPP
