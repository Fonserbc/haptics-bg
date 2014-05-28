//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2010 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   2.1.0 $Rev: 322 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <float.h>
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------

#include "Room.hpp"
#include "Sphere.hpp"
#include "Logic.hpp"

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 600;
const int WINDOW_SIZE_H         = 600;

// mouse menu options (right button)
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;

// maximum number of haptic devices supported in this demo
const int MAX_DEVICES           = 2;
const double DEVICE_DISTANCE    = 0.5;


//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

// a light source to illuminate the objects in the virtual scene
cLight *light;

// a little "chai3d" bitmap logo at the bottom of the screen
cBitmap* logo;

// width and height of the current window display
int displayW  = 0;
int displayH  = 0;

// a haptic device handler
cHapticDeviceHandler* handler;

// a table containing pointers to all haptic devices detected on this computer
cGenericHapticDevice* hapticDevices[MAX_DEVICES];

// a table containing pointers to label which display the position of
// each haptic device
cLabel* labels[MAX_DEVICES];
cGenericObject* rootLabels;

// number of haptic devices detected
int numHapticDevices = 0;

// table containing a list of 3D cursors for each haptic device
cShapeSphere* cursors[MAX_DEVICES];

// table containing a list of lines to display velocity
cShapeLine* velocityVectors[MAX_DEVICES];

// material properties used to render the color of the cursors
cMaterial matCursor2;
cMaterial matCursor1;
cMaterial matSun;

// status of the main simulation haptics loop
bool simulationRunning = false;

// root resource path
string resourceRoot;

// damping mode ON/OFF
bool useDamping = false;

// force field mode ON/OFF
bool useForceField = true;

// has exited haptics simulation thread
bool simulationFinished = false;

cPrecisionClock pClock;
double lastTime;

Room* room;
Sphere* sun;
Logic* logic;

// States
bool calibrationFinished = false;
bool scoreDisplayed = false;
cVector3d min, max;


int test = 1;
float coordinateFactor = 4.0f;

//---------------------------------------------------------------------------
// DECLARED MACROS
//---------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// callback when the right mouse button is pressed to select a menu item
void menuSelect(int value);

// function called before exiting the application
void close(void);

// main graphics callback
void updateGraphics(void);

// main haptics loop
void updateHaptics(void);

cVector3d deviceToWorld(cVector3d position, int deviceId);
cVector3d worldToDevice(cVector3d position, int deviceId);


//===========================================================================
/*
    DEMO:    device.cpp

    This application illustrates the use of the haptic device handler
    "cHapticDevicehandler" to access all of the haptic devices
    "cGenericHapticDevice" connected to the computer.

    In this example the application opens an OpenGL window and displays a
    3D cursor for each device. Each cursor (sphere + reference frame)
    represents the position and orientation of its respective device.
    If the operator presses the device user button (if available), the color
    of the cursor changes accordingly.

    In the main haptics loop function  "updateHaptics()" , the position,
    orientation and user switch status of each device are retrieved at
    each simulation iteration. The information is then used to update the
    position, orientation and color of the cursor. A force is then commanded
    to the haptic device to attract the end-effector towards the device origin.
*/
//===========================================================================

int main(int argc, char* argv[])
{
    //-----------------------------------------------------------------------
    // INITIALIZATION
    //-----------------------------------------------------------------------

    printf ("\n");
    printf ("-----------------------------------\n");
    printf ("Be God\n");
    printf ("DH2660 Haptics\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);

    pClock.start(true);
    lastTime = pClock.getCurrentTimeSeconds();

    //-----------------------------------------------------------------------
    // 3D - SCENEGRAPH
    //-----------------------------------------------------------------------

    // create a new world.
    world = new cWorld();

    // set the background color of the environment
    // the color is defined by its (R,G,B) components.
    world->setBackgroundColor(0.25, 0.55, 0.8);

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and oriente the camera
    camera->set( cVector3d (-1.0, 0.0, 0.3),    // camera position (eye)
                 cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
                 cVector3d (0.0, 0.0, 1.0));   // direction of the "up" vector

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);
    camera->enableMultipassTransparency(true);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam


    //-----------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //-----------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // read the number of haptic devices currently connected to the computer
    numHapticDevices = handler->getNumDevices();

    // Check if two devices are plugged in.
    if(numHapticDevices != 2) {
        std::cout << "Application shut down: Two falcon devices are needed." << std::endl;
        exit(0);
    }

    // create a node on which we will attach small labels that display the
    // position of each haptic device
    rootLabels = new cGenericObject();
    camera->m_front_2Dscene.addChild(rootLabels);

    // create a small label as title
//    cLabel* titleLabel = new cLabel();
//    rootLabels->addChild(titleLabel);

//    // define its position, color and string message
//    titleLabel->setPos(0, 30, 0);
//    titleLabel->m_fontColor.set(1.0, 1.0, 1.0);
//    titleLabel->m_string = "Haptic Device Pos [mm]:";

    // for each available haptic device, create a 3D cursor
    // and a small line to show velocity
    int i = 0;
    while (i < numHapticDevices)
    {
        // get a handle to the next haptic device
        cGenericHapticDevice* newHapticDevice;
        handler->getDevice(newHapticDevice, i);

        // open connection to haptic device
        newHapticDevice->open();

		// initialize haptic device
        newHapticDevice->initialize();

        // store the handle in the haptic device table
        hapticDevices[i] = newHapticDevice;

        // retrieve information about the current haptic device
        cHapticDeviceInfo info = newHapticDevice->getSpecifications();

        // create a cursor by setting its radius
        cShapeSphere* newCursor = new cShapeSphere(0.01);

        // add cursor to the world
        world->addChild(newCursor);

        // add cursor to the cursor table
        cursors[i] = newCursor;

        // create a small line to illustrate velocity
//        cShapeLine* newLine = new cShapeLine(cVector3d(0,0,0), cVector3d(0,0,0));
//        velocityVectors[i] = newLine;

//        // add line to the world
//        world->addChild(newLine);

        // create a string that concatenates the device number and model name.
//        string strID;
//        cStr(strID, i);
//        string strDevice = "#" + strID + " - " +info.m_modelName;

        // attach a small label next to the cursor to indicate device information
//        cLabel* newLabel = new cLabel();
//        newCursor->addChild(newLabel);
//        newLabel->m_string = strDevice;
//        newLabel->setPos(0.00, 0.02, 0.00);
//        newLabel->m_fontColor.set(1.0, 1.0, 1.0);

        // if the device provided orientation sensing (stylus), a reference
        // frame is displayed
        if (info.m_sensedRotation == true)
        {
            // display a reference frame
            newCursor->setShowFrame(true);

            // set the size of the reference frame
            newCursor->setFrameSize(0.05, 0.05);
        }

        // crate a small label to indicate the position of the device
//        cLabel* newPosLabel = new cLabel();
//        rootLabels->addChild(newPosLabel);
//        newPosLabel->setPos(0, -20 * i, 0);
//        newPosLabel->m_fontColor.set(0.6, 0.6, 0.6);
//        labels[i] = newPosLabel;

        // increment counter
        i++;
    }


    // here we define the material properties of the cursor when the
    // user button of the device end-effector is engaged (ON) or released (OFF)

    // a light orange material color
    matCursor1.m_ambient.set(0.5, 0.2, 0.0);
    matCursor1.m_diffuse.set(1.0, 0.5, 0.0);
    matCursor1.m_specular.set(1.0, 1.0, 1.0);

    // a blue material color
    matCursor2.m_ambient.set(0.1, 0.1, 0.4);
    matCursor2.m_diffuse.set(0.3, 0.3, 0.8);
    matCursor2.m_specular.set(1.0, 1.0, 1.0);

    room = new Room(world, 0.5);

    matSun.m_ambient.set(0.4, 0.3, 0.0);
    matSun.m_diffuse.set(1.0, 0.7, 0.0);
    matSun.m_specular.set(1.0, 1.0, 1.0);
    sun = new Sphere(world, 0.05, matSun, -room->getHeight()/2);

    max = cVector3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    min = cVector3d(DBL_MAX, DBL_MAX, DBL_MAX);
    logic = new Logic(world, sun);

    //-----------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //-----------------------------------------------------------------------

    // initialize GLUT
    glutInit(&argc, argv);

    // retrieve the resolution of the computer display and estimate the position
    // of the GLUT window so that it is located at the center of the screen
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int windowPosX = (screenW - WINDOW_SIZE_W) / 2;
    int windowPosY = (screenH - WINDOW_SIZE_H) / 2;

    // initialize the OpenGL GLUT window
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(WINDOW_SIZE_W, WINDOW_SIZE_H);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("Haptics - BG");

    // create a mouse menu (right button)
    glutCreateMenu(menuSelect);
    glutAddMenuEntry("full screen", OPTION_FULLSCREEN);
    glutAddMenuEntry("window display", OPTION_WINDOWDISPLAY);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    //-----------------------------------------------------------------------
    // START SIMULATION
    //-----------------------------------------------------------------------

    // simulation in now running
    simulationRunning = true;

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

    // start the main graphics rendering loop
    glutMainLoop();

    // close everything
    close();

    // exit
    return (0);
}

//---------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
    // update the size of the viewport
    displayW = w;
    displayH = h;
    glViewport(0, 0, displayW, displayH);

    // update position of labels
    rootLabels->setPos(10, displayH-30, 0);
}

//---------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
    // escape key
    if ((key == 27) || (key == 'x'))
    {
        // close everything
        close();

        // exit application
        exit(0);
    }

    // option s:
    if (key == 's')
    {
        if (calibrationFinished && logic->isReady() && !logic->gameIsOver())
            logic->spawnNewTarget();
    }

    // option d:
    if (key == 'd')
    {
        if (calibrationFinished && logic->isReady() && !logic->gameIsOver())
            logic->spawnNewTarget(true);
    }

    // option c:
    if (key == 'c')
    {
        if (!calibrationFinished) {
            calibrationFinished = true;
            std::cout << "Calibration finished: (" << min << "), (" << max << ")" << std::endl;

            logic->init(min, max);
        }
    }

    // option r:
    if (key == 'r')
    {
        if (calibrationFinished) {
            test += 1;
            if (test > 3) test = 1;
            logic->init(min, max);
            scoreDisplayed = false;
        }
    }

    if (key == 't')
    {
        test = 0;
    }
}

//---------------------------------------------------------------------------

void menuSelect(int value)
{
    switch (value)
    {
        // enable full screen display
        case OPTION_FULLSCREEN:
            glutFullScreen();
            break;

        // reshape window to original size
        case OPTION_WINDOWDISPLAY:
            glutReshapeWindow(WINDOW_SIZE_W, WINDOW_SIZE_H);
            break;
    }
}

//---------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close all haptic devices
    int i=0;
    while (i < numHapticDevices)
    {
        hapticDevices[i]->close();
        i++;
    }
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    // update content of position label
//    for (int i=0; i<numHapticDevices; i++)
//    {
//        // read position of device an convert into millimeters
//        cVector3d pos;
//        hapticDevices[i]->getPosition(pos);
//        pos.mul(1000);

//        // create a string that concatenates the device number and its position.
//        string strID;
//        cStr(strID, i);
//        string strLabel = "#" + strID + "  x: ";

//        cStr(strLabel, pos.x, 2);
//        strLabel = strLabel + "   y: ";
//        cStr(strLabel, pos.y, 2);
//        strLabel = strLabel + "  z: ";
//        cStr(strLabel, pos.z, 2);

//        labels[i]->m_string = strLabel;
//    }

    // render world
    camera->renderView(displayW, displayH);

    // Swap buffers
    glutSwapBuffers();

    // check for any OpenGL errors
    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

    // inform the GLUT window to call updateGraphics again (next frame)
    if (simulationRunning)
    {
        glutPostRedisplay();
    }
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
    double timeV = 0.0;

    cLabel* label = new cLabel();
    cLabel* label2 = new cLabel();
    rootLabels->addChild(label);
    rootLabels->addChild(label2);
    label->setPos(0, 0, 0);
    label2->setPos(0, -20, 0);
    label->m_fontColor.set(1.0, 1.0, 1.0);
    label2->m_fontColor.set(1.0, 1.0, 1.0);

    // main haptic simulation loop
    while(simulationRunning)
    {
        float deltaTime = pClock.getCurrentTimeSeconds() - lastTime;
        lastTime = pClock.getCurrentTimeSeconds();

        cVector3d equilibrium (0.0, 0.0, 0.0);
        cVector3d pos0 (0.0, 0.0, 0.0), pos1 (0.0, 0.0, 0.0);

        // for each device
        int i=0;
        while (i < numHapticDevices)
        {
            // read position of haptic device
            cVector3d newPosition;
            hapticDevices[i]->getPosition(newPosition);
            newPosition = deviceToWorld(newPosition, i);

            ((i == 0)? pos0 : pos1) = newPosition;

            // read orientation of haptic device
            cMatrix3d newRotation;
            hapticDevices[i]->getRotation(newRotation);

            // update position and orientation of cursor
            cursors[i]->setPos(newPosition);
            cursors[i]->setRot(newRotation);

            // read linear velocity from device
            cVector3d linearVelocity;
            hapticDevices[i]->getLinearVelocity(linearVelocity);

            // update arrow
//            velocityVectors[i]->m_pointA = newPosition;
//            velocityVectors[i]->m_pointB = cAdd(newPosition, linearVelocity);

            // read user button status
            bool buttonStatus;
            hapticDevices[i]->getUserSwitch(0, buttonStatus);

            // adjustthe  color of the cursor according to the status of
            // the user switch (ON = TRUE / OFF = FALSE)
            if (i == 0)
                cursors[i]->m_material = matCursor2;
            else
                cursors[i]->m_material = matCursor1;

            // increment counter
            i++;
        }

        double f0, f1;
        f0 = pos0.length();
        f1 = pos1.length();

        f0 = f0/(f0 + f1);
        f1 = 1.0 - f0;

        equilibrium = pos1 + (pos0 - pos1)*f0;

        // Update the position of the sun
        cVector3d dir = pos1 - pos0;
        double dist = dir.length();
        dir.normalize();

        double vibrationSpeed = 20.0;
        double vibrationAmount = 0.02;

        //sun->setPos(sun->getPos()*(1.0 - deltaTime*speed) + equilibrium*(deltaTime*speed));
        //timeV += deltaTime*vibrationSpeed*(0.7 - cAbs(f0 - 0.5)*2.0);
        sun->setPos(equilibrium /*+ vibrationAmount*dir*cSinRad(timeV)*/);

        // Update logic
        if (!calibrationFinished) {
            label->m_string = "Calibrating, please move the haptic devices around in order to determine their limitations in movement.";
            label2->m_string = "Press 'c' to finish calibration.";

            if (sun->getPos().x < min.x)
                min.x = sun->getPos().x;
            if (sun->getPos().y < min.y)
                min.y = sun->getPos().y;
            if (sun->getPos().z < min.z)
                min.z = sun->getPos().z;

            if (sun->getPos().x > max.x)
                max.x = sun->getPos().x;
            if (sun->getPos().y > max.y)
                max.y = sun->getPos().y;
            if (sun->getPos().z > max.z)
                max.z = sun->getPos().z;
        } else if (logic->isReady()) {
            if (logic->gameIsOver() && !scoreDisplayed) {
                std::stringstream strs;
                strs << logic->playTime();
                std::string playString = strs.str();

                // define its position, color and string message
                label->m_string = "Congratulation! Your Time: " + playString;
                label2->m_string = "Press 'r' to restart!";

                for (i = 0; i < numHapticDevices; i++) {
                    cVector3d zero(0.0, 0.0, 0.0);
                    hapticDevices[i]->setForce(zero);
                }

                scoreDisplayed = true;
            } else if (!scoreDisplayed) {
                label->m_string = "";
                label2->m_string = "";

                logic->update(deltaTime);

            }

            for (i = 0; i < numHapticDevices; i++) {
                // compute a reaction force
                cVector3d newForce (0,0,0);

                cVector3d devicePosition;
                hapticDevices[i]->getPosition(devicePosition);
                devicePosition = deviceToWorld(devicePosition, i);

                double k = 0.4;
                if (test == 1) k = 0.3;
                double dist = (devicePosition - sun->getPos()).length();
                //dist=dist-0.1;

                newForce = k*(devicePosition - sun->getPos())/(dist*dist*dist);
                //double intensity = (newForce.length())*1.0;
                //newForce.normalize();
                //newForce *= intensity;

    //            newForce = k*(devicePosition - sun->getPos())/(dist*dist*dist);

                if (i == 0) {  // Device on positive X (RIGHT)
                    newForce.x *= -1.0;
                    newForce.y *= -1.0;
                }

                // send computed force to haptic device
    //            bool status = true;
    //            if (hapticDevices[i]->getUserSwitch(0))
    //                printf("button pressed\n");

                // Check if the sphere is in the target area. If so, vibrate
                cVector3d vibrationForce(0.0, 0.0, 0.0);
                if (logic->sphereInTarget() && !logic->gameIsOver()) {
                    Cube* target = logic->getTarget();
                    double dist = target->getPos().distance(sun->getPos());
                    double factor = 1.0 - dist/(target->size/2.0);
                    timeV += deltaTime * (0.5 + factor/2.0);

                    double f = 2*cSinRad(40*timeV);
                    vibrationForce = cVector3d(f, f, f);
                }

                newForce += vibrationForce;
                if (test <= 2 || i == 0)
                    hapticDevices[i]->setForce(newForce);
                else {
                    cVector3d zero;
                    zero.zero();
                    hapticDevices[i]->setForce(zero);
                }
          }
        }
    }
    
    // exit haptics thread
    simulationFinished = true;
}

cVector3d deviceToWorld(cVector3d position, int deviceId) {
    cVector3d p = position;

    p *= coordinateFactor;

    p.x += DEVICE_DISTANCE/2.0;

    if (deviceId == 0) {  // Device on positive X (RIGHT)
        p.x *= -1.0;
        p.y *= -1.0;
    }

    return p;
}

cVector3d worldToDevice(cVector3d position, int deviceId) {
    cVector3d p = position;

    if (deviceId == 0) {  // Device on positive X (RIGHT)
        p.x *= -1.0;
        p.y *= -1.0;
    }

    p.x -= DEVICE_DISTANCE/2.0;

    p /= coordinateFactor;

    return p;
}

//---------------------------------------------------------------------------
