#include "Sphere.hpp"

Sphere::Sphere (cWorld* world, Cube* room, double radius, cMaterial material) {
    this->world = world;

    sphere = new cShapeSphere(radius);
    sphere->m_material = material;

    shadow = new cShapeSphere(radius);
    cVector3d scale = cVector3d(1.0, 1.0, 0.01);
    shadow->scaleObject(scale);
    cMaterial matShadow;
    matShadow.m_ambient.set(0.5, 0.05, 0.05);
    matShadow.m_diffuse.set(0.5, 0.0, 0.0);
    matShadow.m_specular.set(0.0, 0.0, 0.0);
    shadow->m_material = matShadow;

    this->world->addChild(sphere);
    this->world->addChild(shadow);
}

void Sphere::setPos(cVector3d pos) {
    sphere->setPos(pos);

    cVector3d shadowPos = cVector3d(pos.x, pos.y, -0.5);//room->getHeight() / 2);
    shadow->setPos(shadowPos);
}

cVector3d Sphere::getPos() {
    return sphere->getPos();
}
