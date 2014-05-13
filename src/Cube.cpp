#include "Cube.hpp"

Cube::Cube(cWorld *world, cVector3d position, double size, cMaterial material) {
    this->world = world;

    mesh = new cMesh(this->world);
    mesh->setPos(position);
    mesh->m_material = material;
    mesh->setTransparencyLevel(0.5);
    mesh->setUseTransparency(true);
    mesh->setUseCulling(false, true);

    this->createCube(mesh, size);

    this->world->addChild(mesh);
}

void Cube::createCube(cMesh* mesh, double size, bool inside) {
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
    if (inside) {
        for (int i=0; i<6; i++) {
            mesh->newTriangle(vertices[i][2], vertices[i][1], vertices[i][0]);
            mesh->newTriangle(vertices[i][3], vertices[i][2], vertices[i][0]);
        }
    } else {
        for (int i=0; i<6; i++) {
            mesh->newTriangle(vertices[i][0], vertices[i][1], vertices[i][2]);
            mesh->newTriangle(vertices[i][0], vertices[i][2], vertices[i][3]);
        }
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
