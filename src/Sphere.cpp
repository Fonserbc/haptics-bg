#include "Sphere.hpp"

Sphere::Sphere (cWorld* world, double radius, cMaterial material, double shadowOffset) {
    this->world = world;
    this->radius = radius;
    this->shadowOffset = shadowOffset + 0.0005;

    sphere = new cShapeSphere(radius);
    sphere->m_material = material;

//    shadow = new cShapeSphere(radius);
    shadow = new cMesh(this->world);
    shadow->setUseCulling(false, true);
    int vertices[4];
    vertices[0] = shadow->newVertex(radius, -radius, 0.0);
    vertices[1] = shadow->newVertex(radius, radius, 0.0);
    vertices[2] = shadow->newVertex(-radius, radius, 0.0);
    vertices[3] = shadow->newVertex(-radius, -radius, 0.0);
    shadow->newTriangle(vertices[0], vertices[1], vertices[2]);
    shadow->newTriangle(vertices[0], vertices[2], vertices[3]);

    cMaterial matShadow;
    matShadow.m_ambient.set(0.0, 0.0, 0.0);
    matShadow.m_diffuse.set(0.2, 0.2, 0.2);
    matShadow.m_specular.set(0.0, 0.0, 0.0);
    shadow->m_material = matShadow;

    this->world->addChild(sphere);
    this->world->addChild(shadow);
}

void Sphere::setPos(cVector3d pos) {
    sphere->setPos(pos);

    cVector3d shadowPos = cVector3d(pos.x, pos.y, shadowOffset);
    shadow->setPos(shadowPos);
}

cVector3d Sphere::getPos() {
    return sphere->getPos();
}

double Sphere::getRadius() {
    return radius;
}

double Sphere::getShadowOffset() {
    return shadowOffset;
}
