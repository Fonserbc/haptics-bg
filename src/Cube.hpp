#ifndef CUBE_HPP
#define CUBE_HPP

#include "chai3d.h"

class Cube {
public:
    double transparency;

    Cube(cWorld* world, cVector3d position, double size, cMaterial material);

    // Returns the height (on z axis) of the cube
    double getHeight();

private:
    cWorld* world;
    cMesh* mesh;

    void createCube(cMesh* mesh, double size, bool inside = false);
};
#endif // CUBE_HPP
