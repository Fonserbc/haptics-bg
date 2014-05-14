#include "Room.hpp"

Room::Room(cWorld *world, double height) {
    this->world = world;
    this->height = height;

    floor = new cMesh(this->world);
    floor->setUseCulling(false, true);

    double roomSize = 10.0;
    int vertices[4];
    vertices[0] = floor->newVertex(roomSize, -roomSize, 0.0);
    vertices[1] = floor->newVertex(roomSize, roomSize, 0.0);
    vertices[2] = floor->newVertex(-roomSize, roomSize, 0.0);
    vertices[3] = floor->newVertex(-roomSize, -roomSize, 0.0);
    floor->newTriangle(vertices[0], vertices[1], vertices[2]);
    floor->newTriangle(vertices[0], vertices[2], vertices[3]);

    floor ->setPos(0.0, 0.0, -height/2.0);

    cMaterial matFloor;
    matFloor.m_ambient.set(0.9, 0.9, 0.9);
    matFloor.m_diffuse.set(0.8, 0.8, 0.8);
    matFloor.m_specular.set(0.6, 0.6, 0.6);
    floor->m_material = matFloor;

    this->world->addChild(floor);
}

double Room::getHeight() {
    return height;
}
