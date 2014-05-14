#ifndef CUBE_HPP
#define CUBE_HPP

#include "chai3d.h"

class Cube {
public:
    double transparency;

    Cube(cWorld* world, cVector3d position, double size, cMaterial material);

    cVector3d getPos();
    void remove();

private:
    cWorld* world;
    cMesh* mesh;
    cMesh* shadow;

    void createCube(double size);
    void createShadow(double size);
};
#endif // CUBE_HPP
