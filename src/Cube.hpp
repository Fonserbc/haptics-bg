#ifndef CUBE_HPP
#define CUBE_HPP

#include "chai3d.h"

class Cube {
public:
    double size;

    Cube(cWorld* world, cVector3d position, double size, cMaterial material, double shadowOffset);

    cVector3d getPos();
    void setMaterial(cMaterial material);
    void remove();

private:
    cWorld* world;
    cMesh* mesh;
    cMesh* shadow;

    void createCube(double size);
    void createShadow(double size);
};
#endif // CUBE_HPP
