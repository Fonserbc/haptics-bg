#include "Cube.hpp"

Cube::Cube(cWorld *world, cVector3d position, double size, cMaterial material, double shadowOffset) {
    this->world = world;

    mesh = new cMesh(this->world);
    mesh->setPos(position);
    mesh->setUseTransparency(true);
    mesh->setTransparencyLevel(0.4);
    mesh->m_material = material;
    mesh->setUseCulling(false, true);

    createCube(size);

    shadow = new cMesh(this->world);
    cVector3d shadowPos(position.x, position.y, shadowOffset);
    shadow->setPos(shadowPos);
    shadow->setUseCulling(false, true);

    createShadow(size);

    this->world->addChild(mesh);
    this->world->addChild(shadow);
}

void Cube::createCube(double size) {
    const double offset = size / 2.0;
    int vertices [6][6];

    // face -x
    vertices[0][0] = mesh->newVertex(-offset,  offset, -offset);
    vertices[0][1] = mesh->newVertex(-offset, -offset, -offset);
    vertices[0][2] = mesh->newVertex(-offset, -offset,  offset);
    vertices[0][3] = mesh->newVertex(-offset,  offset,  offset);

    // face +x
    vertices[1][0] = mesh->newVertex( offset, -offset, -offset);
    vertices[1][1] = mesh->newVertex( offset,  offset, -offset);
    vertices[1][2] = mesh->newVertex( offset,  offset,  offset);
    vertices[1][3] = mesh->newVertex( offset, -offset,  offset);

    // face -y
    vertices[2][0] = mesh->newVertex(-offset,  -offset, -offset);
    vertices[2][1] = mesh->newVertex( offset,  -offset, -offset);
    vertices[2][2] = mesh->newVertex( offset,  -offset,  offset);
    vertices[2][3] = mesh->newVertex(-offset,  -offset,  offset);

    // face +y
    vertices[3][0] = mesh->newVertex( offset,   offset, -offset);
    vertices[3][1] = mesh->newVertex(-offset,   offset, -offset);
    vertices[3][2] = mesh->newVertex(-offset,   offset,  offset);
    vertices[3][3] = mesh->newVertex( offset,   offset,  offset);

    // face -z
    vertices[4][0] = mesh->newVertex(-offset,  -offset, -offset);
    vertices[4][1] = mesh->newVertex(-offset,   offset, -offset);
    vertices[4][2] = mesh->newVertex( offset,   offset, -offset);
    vertices[4][3] = mesh->newVertex( offset,  -offset, -offset);

    // face +z
    vertices[5][0] = mesh->newVertex( offset,  -offset,  offset);
    vertices[5][1] = mesh->newVertex( offset,   offset,  offset);
    vertices[5][2] = mesh->newVertex(-offset,   offset,  offset);
    vertices[5][3] = mesh->newVertex(-offset,  -offset,  offset);

    // create triangles   
    for (int i=0; i<6; i++) {
        mesh->newTriangle(vertices[i][0], vertices[i][1], vertices[i][2]);
        mesh->newTriangle(vertices[i][0], vertices[i][2], vertices[i][3]);
    }

//    for (int i=0; i<6; i++) {
//        cShapeLine* l1 = new cShapeLine(mesh->getVertex(vertices[i][0])->getPos(), mesh->getVertex(vertices[i][1])->getPos());
//        cShapeLine* l2 = new cShapeLine(mesh->getVertex(vertices[i][1])->getPos(), mesh->getVertex(vertices[i][2])->getPos());
//        cShapeLine* l3 = new cShapeLine(mesh->getVertex(vertices[i][2])->getPos(), mesh->getVertex(vertices[i][3])->getPos());
//        cShapeLine* l4 = new cShapeLine(mesh->getVertex(vertices[i][3])->getPos(), mesh->getVertex(vertices[i][0])->getPos());
//        this->world->addChild(l1);
//        this->world->addChild(l2);
//        this->world->addChild(l3);
//        this->world->addChild(l4);
//    }

    // compute normals
    mesh->computeAllNormals();
}

void Cube::createShadow(double size) {
    const double offset = size / 2.0;
    int vertices[4];
    vertices[0] = shadow->newVertex(offset, -offset, 0.0);
    vertices[1] = shadow->newVertex(offset, offset, 0.0);
    vertices[2] = shadow->newVertex(-offset, offset, 0.0);
    vertices[3] = shadow->newVertex(-offset, -offset, 0.0);
    shadow->newTriangle(vertices[0], vertices[1], vertices[2]);
    shadow->newTriangle(vertices[0], vertices[2], vertices[3]);

    cMaterial matShadow;
    matShadow.m_ambient.set(0.0, 0.0, 0.0);
    matShadow.m_diffuse.set(0.2, 0.2, 0.35);
    matShadow.m_specular.set(0.0, 0.0, 0.0);
    shadow->m_material = matShadow;
}

void Cube::setMaterial(cMaterial material) {
    mesh->m_material = material;
    mesh->setTransparencyLevel(0.4);
}

cVector3d Cube::getPos() {
    return mesh->getPos();
}

void Cube::remove() {
//    world->deleteChild(mesh);
//    world->deleteChild(shadow);
    mesh->clear();
    shadow->clear();
}
