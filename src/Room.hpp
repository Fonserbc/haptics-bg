#ifndef ROOM_HPP
#define ROOM_HPP

#include "chai3d.h"

class Room {
public:
    Room(cWorld* world, double height);

    double getHeight();

private:
    cWorld* world;
    cMesh* floor;

    double height;
};

#endif // ROOM_HPP
