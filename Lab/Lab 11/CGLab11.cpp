/*
 TCG6223 Computer Graphics
 CGLab11.cpp
 Objective: Lab11 on Textures
 Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
*/
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include "CGLab11.hpp"

using namespace CGLab11;

// ==========================================
// MySpotLights Implementation
// ==========================================
MySpotLights::MySpotLights()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    rotateangle = 0.0f;
    rotatespeed = 90.0f; // degrees per second
    lighton[0] = lighton[1] = lighton[2] = true;
}

MySpotLights::~MySpotLights()
{
    gluDeleteQuadric(pObj);
}

void MySpotLights::setupLights()
{
    // Configure Spotlight 1 (Red)
    glLightfv(GL_LIGHT1, GL_AMBIENT, myred);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, myred);
    glLightfv(GL_LIGHT1, GL_SPECULAR, myred);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f);
    glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);
    glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.04f);

    // Configure Spotlight 2 (Green)
    glLightfv(GL_LIGHT2, GL_AMBIENT, mygreen);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, mygreen);
    glLightfv(GL_LIGHT2, GL_SPECULAR, mygreen);
    glLightf (GL_LIGHT2, GL_SPOT_CUTOFF, 35.0f);
    glLightf (GL_LIGHT2, GL_SPOT_EXPONENT, 2.0f);
    glLightf (GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.04f);

    // Configure Spotlight 3 (Blue)
    glLightfv(GL_LIGHT3, GL_AMBIENT, myblue);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, myblue);
    glLightfv(GL_LIGHT3, GL_SPECULAR, myblue);
    glLightf (GL_LIGHT3, GL_SPOT_CUTOFF, 35.0f);
    glLightf (GL_LIGHT3, GL_SPOT_EXPONENT, 2.0f);
    glLightf (GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.04f);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    lighton[0] = lighton[1] = lighton[2] = true;
}

void MySpotLights::toggleLight(int lightno)
{
    static GLenum tag[] = {GL_LIGHT1, GL_LIGHT2, GL_LIGHT3};
    lighton[lightno] = !lighton[lightno];
    if (lighton[lightno])
        glEnable(tag[lightno]);
    else
        glDisable(tag[lightno]);
}

void MySpotLights::draw()
{
    GLboolean cullingIsOn;
    glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
    glDisable(GL_CULL_FACE);

    static GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static GLfloat direction[] = {0.0f, 0.0f, 1.0f, 1.0f};
    GLfloat radius = 9.0f;

    glPushMatrix();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(rotateangle, 0.0f, 0.0f, 1.0f);

        // Draw central mounting disk ring
        glColor3f(1.0f, 0.5f, 0.3f);
        gluDisk(pObj, radius - 1.0f, radius + 1.0f, 20, 5);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -0.1f);
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(pObj, radius - 1.0f, radius + 1.0f, 20, 5);
        glPopMatrix();

        // Spotlight 1 (Red)
        glPushMatrix();
            glTranslatef(radius, 0.0f, 0.0f);
            glDisable(GL_COLOR_MATERIAL);
            if (lighton[0])
                glMaterialfv(GL_FRONT, GL_EMISSION, myred);
            else
                glMaterialfv(GL_FRONT, GL_EMISSION, myblack);
            glColor3f(1.0f, 0.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, myred);
            gluCylinder(pObj, 1.0f, 2.0f, 3.0f, 30, 5);
            glLightfv(GL_LIGHT1, GL_POSITION, position);
            glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
        glPopMatrix();

        // Spotlight 2 (Green)
        GLfloat dx = radius * 0.5f;
        GLfloat dy = radius * 0.8660f;
        glPushMatrix();
            glTranslatef(-radius - dx, dy, 0.0f);
            glDisable(GL_COLOR_MATERIAL);
            if (lighton[1])
                glMaterialfv(GL_FRONT, GL_EMISSION, mygreen);
            else
                glMaterialfv(GL_FRONT, GL_EMISSION, myblack);
            glColor3f(0.0f, 1.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mygreen);
            gluCylinder(pObj, 1.0f, 2.0f, 3.0f, 30, 5);
            glLightfv(GL_LIGHT2, GL_POSITION, position);
            glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction);
        glPopMatrix();

        // Spotlight 3 (Blue)
        glPushMatrix();
            glTranslatef(-dx, -dy, 0.0f);
            glDisable(GL_COLOR_MATERIAL);
            if (lighton[2])
                glMaterialfv(GL_FRONT, GL_EMISSION, myblue);
            else
                glMaterialfv(GL_FRONT, GL_EMISSION, myblack);
            glColor3f(0.0f, 0.0f, 1.0f);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, myblue);
            gluCylinder(pObj, 1.0f, 2.0f, 3.0f, 30, 5);
            glLightfv(GL_LIGHT3, GL_POSITION, position);
            glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction);
        glPopMatrix();

        // Restore Color Material settings
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_EMISSION);
        glColor3f(0.0f, 0.0f, 0.0f);
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glPopMatrix();

    if (cullingIsOn == GL_TRUE)
        glEnable(GL_CULL_FACE);
}

void MySpotLights::tickTime(long int elapseTime)
{
    rotateangle += elapseTime * rotatespeed / 1000.0f;
    if (rotateangle >= 360.0f) rotateangle -= 360.0f;
}

// ==========================================
// MySwingLights Implementation
// ==========================================
MySwingLights::MySwingLights()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    length = 25.0f;
    timestart = glutGet(GLUT_ELAPSED_TIME);
    lighton[0] = lighton[1] = true;
    horizDisp1 = horizDisp2 = 0.0f;
}

MySwingLights::~MySwingLights()
{
    gluDeleteQuadric(pObj);
}

void MySwingLights::setupLights()
{
    glLightfv(GL_LIGHT4, GL_AMBIENT, mywhite);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, mywhite);
    glLightfv(GL_LIGHT4, GL_SPECULAR, mywhite);
    glLightf (GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT4, GL_LINEAR_ATTENUATION, 0.2f);

    glLightfv(GL_LIGHT5, GL_AMBIENT, mywhite);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, mywhite);
    glLightfv(GL_LIGHT5, GL_SPECULAR, mywhite);
    glLightf (GL_LIGHT5, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT5, GL_LINEAR_ATTENUATION, 0.2f);

    glEnable(GL_LIGHT4);
    glEnable(GL_LIGHT5);
    lighton[0] = lighton[1] = true;
}

void MySwingLights::toggleLight(int lightno)
{
    static GLenum tag[] = {GL_LIGHT4, GL_LIGHT5};
    lighton[lightno] = !lighton[lightno];
    if (lighton[lightno])
        glEnable(tag[lightno]);
    else
        glDisable(tag[lightno]);
}

void MySwingLights::draw()
{
    float degreePerRad = 180.0f / M_PI;
    static GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};

    // Pendulum 1
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -20.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(asin(horizDisp1 / length) * degreePerRad, 0.0f, 1.0f, 0.0f);

        glColor3f(1.0f, 1.0f, 1.0f);
        gluCylinder(pObj, 0.1f, 0.1f, length, 10, 10);
        
        glTranslatef(0.0f, 0.0f, length);
        glLightfv(GL_LIGHT4, GL_POSITION, position);

        glDisable(GL_COLOR_MATERIAL);
        if (lighton[0])
            glMaterialfv(GL_FRONT, GL_EMISSION, mywhite);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, myblack);
        glColor3f(1.0f, 1.0f, 1.0f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mywhite);
        gluSphere(pObj, 0.5f, 10, 10);
        
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_EMISSION);
        glColor3f(0.0f, 0.0f, 0.0f);
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glPopMatrix();

    // Pendulum 2
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 20.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(asin(horizDisp2 / length) * degreePerRad, 0.0f, 1.0f, 0.0f);

        glColor3f(1.0f, 1.0f, 1.0f);
        gluCylinder(pObj, 0.1f, 0.1f, length, 10, 10);
        
        glTranslatef(0.0f, 0.0f, length);
        glLightfv(GL_LIGHT5, GL_POSITION, position);

        glDisable(GL_COLOR_MATERIAL);
        if (lighton[1])
            glMaterialfv(GL_FRONT, GL_EMISSION, mywhite);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, myblack);
        glColor3f(1.0f, 1.0f, 1.0f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mywhite);
        gluSphere(pObj, 0.5f, 10, 10);
        
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_EMISSION);
        glColor3f(0.0f, 0.0f, 0.0f);
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glPopMatrix();
}

void MySwingLights::tickTime(long int elapseTime)
{
    float currentTime = (float)(glutGet(GLUT_ELAPSED_TIME) - timestart);
    float currentTimeInSec = currentTime / 1000.0f;
    float D = 25.0f;
    float g = 50.0f;
    float freq = sqrt(g / length);
    float k = freq * currentTimeInSec;

    horizDisp1 = D * sin(k - M_PI);
    horizDisp2 = D * sin(k);
}

// ==========================================
// MyFlyingLights Implementation
// ==========================================
MyFlyingLights::MyFlyingLights()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    posx = 0.0f;
    posy = 15.0f;
    posz = 0.0f;
    timeAccumulator = 0.0f;
    lighton = true;
}

MyFlyingLights::~MyFlyingLights()
{
    gluDeleteQuadric(pObj);
}

void MyFlyingLights::setupLights()
{
    glLightfv(GL_LIGHT6, GL_AMBIENT, myyellow);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, myyellow);
    glLightfv(GL_LIGHT6, GL_SPECULAR, myyellow);
    glLightf (GL_LIGHT6, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT6, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf (GL_LIGHT6, GL_QUADRATIC_ATTENUATION, 0.02f);
    glEnable(GL_LIGHT6);
    lighton = true;
}

void MyFlyingLights::toggleLight()
{
    lighton = !lighton;
    if (lighton)
        glEnable(GL_LIGHT6);
    else
        glDisable(GL_LIGHT6);
}

void MyFlyingLights::draw()
{
    glPushMatrix();
        glTranslatef(posx, posy, posz);
        static GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT6, GL_POSITION, position);

        glDisable(GL_LIGHTING);
        glColor3fv(myyellow);
        gluSphere(pObj, 0.3f, 10, 10);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void MyFlyingLights::tickTime(long int elapseTime)
{
    float dt = elapseTime / 1000.0f;
    timeAccumulator += dt;
    
    posx = 35.0f * sin(timeAccumulator * 0.8f);
    posz = 25.0f * sin(timeAccumulator * 1.6f);
    posy = 20.0f + 10.0f * sin(timeAccumulator * 1.2f);
}

// ==========================================
// MyVirtualWorld Implementation
// ==========================================
MyVirtualWorld::MyVirtualWorld()
{
    xmin = zmin = -50.0f;
    xmax = zmax = 50.0f;
    height = 50.0f;
    xdim = xmax - xmin;
    zdim = zmax - zmin;
    ydim = height;
    timeold = timenew = elapseTime = 0;
    
    numberOfTexture = 0;
    textureNumbers = NULL;
    textureison = false;
    
    for (int i = 0; i < 7; ++i) 
        lighton[i] = true;
}

MyVirtualWorld::~MyVirtualWorld()
{
    if (textureNumbers != NULL)
        delete[] textureNumbers;
}

void MyVirtualWorld::init()
{
    setupLights();
    setupTextures();
    timeold = glutGet(GLUT_ELAPSED_TIME);
}

void MyVirtualWorld::setupTextures()
{
    numberOfTexture = 3;
    textureNumbers = new GLuint[numberOfTexture];
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glGenTextures(numberOfTexture, textureNumbers);
    
    MyImage myimage[3];
    myimage[0].loadJPG("images/marble1.jpg");
    myimage[1].loadJPG("images/wood1.jpg");
    myimage[2].loadJPG("images/brick1.jpg");
    
    for (int i = 0; i < numberOfTexture; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textureNumbers[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 
                     0, 
                     GL_RGBA, 
                     myimage[i].width, 
                     myimage[i].height, 
                     0, 
                     GL_RGB, 
                     GL_UNSIGNED_BYTE, 
                     myimage[i].buffer);
    }
    textureison = true;
}

void MyVirtualWorld::toggleTextures()
{
    textureison = !textureison;
}

void MyVirtualWorld::setupLights()
{
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // Initialize components
    myspotlights.setupLights();
    myswinglights.setupLights();
    myflyinglights.setupLights();

    // Configure Directional Light (GL_LIGHT0)
    GLfloat mycolor[] = {0.15f, 0.15f, 0.15f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, mycolor);
    glEnable(GL_LIGHT0);

    for (int i = 0; i < 7; ++i) 
        lighton[i] = true;
}

void MyVirtualWorld::toggleLight(int lightno)
{
    if (lightno == 0)
    {
        lighton[0] = !lighton[0];
        if (lighton[0])
            glEnable(GL_LIGHT0);
        else
            glDisable(GL_LIGHT0);
    }
    else if (lightno >= 1 && lightno <= 3)
    {
        myspotlights.toggleLight(lightno - 1);
        lighton[lightno] = !lighton[lightno];
    }
    else if (lightno >= 4 && lightno <= 5)
    {
        myswinglights.toggleLight(lightno - 4);
        lighton[lightno] = !lighton[lightno];
    }
    else if (lightno == 6)
    {
        myflyinglights.toggleLight();
        lighton[6] = !lighton[6];
    }
}

void MyVirtualWorld::drawSpotLights()
{
    myspotlights.draw();
}

void MyVirtualWorld::draw()
{
    static GLfloat position[] = {1.0f, 1.0f, 0.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glPushMatrix();
        mymovingsmiley.draw();
        myfan.draw();
        drawFourTeapot();

        glPushMatrix();
            glTranslatef(0.0f, -0.01f, 0.0f);
            drawFloor();
            drawRoof();
            drawWalls();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, height, 0.0f);
            myswinglights.draw();
        glPopMatrix();

        myflyinglights.draw();
    glPopMatrix();
}

void MyVirtualWorld::drawFloor()
{
    GLfloat texSMax = 5.0f;
    GLfloat texTMax = 5.0f;
    GLfloat quadSizeX = 0.25f;
    GLfloat quadSizeZ = 0.25f;
    GLfloat x1, z1, x2, z2;
    GLfloat s1, t1, s2, t2;
    
    GLfloat texScaleX = texSMax / xdim;
    GLfloat texScaleZ = texTMax / zdim;
    
    if (textureison)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureNumbers[0]);
    }
    
    glPushMatrix();
        for (z1 = zmin; z1 < zmax; z1 += quadSizeZ)
        {
            z2 = z1 + quadSizeZ;
            t1 = texScaleZ * (z1 - zmin);
            t2 = texScaleZ * (z2 - zmin);
            for (x1 = xmin; x1 < xmax; x1 += quadSizeX)
            {
                x2 = x1 + quadSizeX;
                s1 = texScaleX * (x1 - xmin);
                s2 = texScaleX * (x2 - xmin);
                
                glColor3fv(mywhite);
                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glTexCoord2f(s1, t1); glVertex3f(x1, 0.0f, z1);
                    glTexCoord2f(s1, t2); glVertex3f(x1, 0.0f, z2);
                    glTexCoord2f(s2, t2); glVertex3f(x2, 0.0f, z2);
                    glTexCoord2f(s2, t1); glVertex3f(x2, 0.0f, z1);
                glEnd();
            }
        }
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void MyVirtualWorld::drawRoof()
{
    GLfloat texSMax = 10.0f;
    GLfloat texTMax = 10.0f;
    GLfloat checkerboxSizeX = 5.0f;
    GLfloat checkerboxSizeZ = 5.0f;
    GLint quadsPerCheckerboxX = 2;
    GLint quadsPerCheckerboxZ = 2;
    GLfloat quadSizeX = checkerboxSizeX / quadsPerCheckerboxX;
    GLfloat quadSizeZ = checkerboxSizeZ / quadsPerCheckerboxZ;
    GLfloat x1, z1, x2, z2;
    GLfloat s1, t1, s2, t2;
    
    GLfloat texScaleX = texSMax / xdim;
    GLfloat texScaleZ = texTMax / zdim;
    static GLfloat* color[] = {myblack, mywhite};
    
    if (textureison)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureNumbers[1]);
    }
    
    glPushMatrix();
        int startcolorno = 0;
        for (GLfloat z = zmin; z < zmax; z += checkerboxSizeZ)
        {
            int colorno = startcolorno;
            for (GLfloat x = xmin; x < xmax; x += checkerboxSizeX)
            {
                GLint i, j;
                glColor3fv(color[colorno]);
                for (j = 0, z1 = z, z2 = z + quadSizeZ; j < quadsPerCheckerboxZ; ++j, z1 += quadSizeZ, z2 += quadSizeZ)
                {
                    t1 = texScaleZ * (z1 - zmin);
                    t2 = texScaleZ * (z2 - zmin);
                    for (i = 0, x1 = x, x2 = x + quadSizeX; i < quadsPerCheckerboxX; ++i, x1 += quadSizeX, x2 += quadSizeX)
                    {
                        s1 = texScaleX * (x1 - xmin);
                        s2 = texScaleX * (x2 - xmin);
                        
                        glBegin(GL_QUADS);
                            glNormal3f(0.0f, -1.0f, 0.0f);
                            glTexCoord2f(s1, t1); glVertex3f(x1, height, z1);
                            glTexCoord2f(s2, t1); glVertex3f(x2, height, z1);
                            glTexCoord2f(s2, t2); glVertex3f(x2, height, z2);
                            glTexCoord2f(s1, t2); glVertex3f(x1, height, z2);
                        glEnd();
                    }
                }
                ++colorno;
                if (colorno > 1) colorno = 0;
            }
            ++startcolorno;
            if (startcolorno > 1) startcolorno = 0;
        }
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void MyVirtualWorld::drawWalls()
{
    static GLfloat* color[] = {
        myblack, myred, myyellow, mygreen,
        mycyan, myblue, mymagenta, mywhite
    };

    if (textureison)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureNumbers[2]);
    }
    
    GLfloat texSMax = 5.0f;
    GLfloat texTMax = 5.0f;
    GLfloat brickSizeY = 5.0f;
    GLfloat brickSizeZ = 5.0f;
    GLint quadsPerBrickY = 2;
    GLint quadsPerBrickZ = 2;
    GLfloat quadSizeY = brickSizeY / quadsPerBrickY;
    GLfloat quadSizeZ = brickSizeZ / quadsPerBrickZ;
    GLfloat y1, z1, y2, z2;
    GLfloat s1, t1, s2, t2;
    GLfloat texScaleY = texSMax / ydim;
    GLfloat texScaleZ = texTMax / zdim;
    
    glPushMatrix();
        int startcolorno = 0;
        
        // 1. Right (positive-x) and Left (negative-x) walls
        for (GLfloat z = zmin; z < zmax; z += brickSizeZ)
        {
            int colorno = startcolorno;
            for (GLfloat y = 0.0f; y < height; y += brickSizeY)
            {
                GLint i, j;
                if (textureison) colorno = 7;
                glColor3fv(color[colorno]);
                for (j = 0, z1 = z, z2 = z + quadSizeZ; j < quadsPerBrickZ; ++j, z1 += quadSizeZ, z2 += quadSizeZ)
                {
                    t1 = texScaleZ * (z1 - zmin);
                    t2 = texScaleZ * (z2 - zmin);
                    for (i = 0, y1 = y, y2 = y + quadSizeY; i < quadsPerBrickY; ++i, y1 += quadSizeY, y2 += quadSizeY)
                    {
                        s1 = texScaleY * y1;
                        s2 = texScaleY * y2;
                        
                        glBegin(GL_QUADS);
                            // Wall on positive-x
                            glNormal3f(-1.0f, 0.0f, 0.0f);
                            glTexCoord2f(s1, t1); glVertex3f(xmax, y1, z1);
                            glTexCoord2f(s1, t2); glVertex3f(xmax, y1, z2);
                            glTexCoord2f(s2, t2); glVertex3f(xmax, y2, z2);
                            glTexCoord2f(s2, t1); glVertex3f(xmax, y2, z1);

                            // Wall on negative-x
                            glNormal3f(1.0f, 0.0f, 0.0f);
                            glTexCoord2f(s1, t1); glVertex3f(xmin, y1, z1);
                            glTexCoord2f(s2, t1); glVertex3f(xmin, y2, z1);
                            glTexCoord2f(s2, t2); glVertex3f(xmin, y2, z2);
                            glTexCoord2f(s1, t2); glVertex3f(xmin, y1, z2);
                        glEnd();
                    }
                }
                ++colorno;
                if (colorno > 7) colorno = 0;
            }
            ++startcolorno;
            if (startcolorno > 7) startcolorno = 0;
        }

        // 2. Front (positive-z) and Back (negative-z) walls
        GLfloat brickSizeX = 5.0f;
        GLint quadsPerBrickX = 2;
        GLfloat quadSizeX = brickSizeX / quadsPerBrickX;
        GLfloat x1, x2;
        GLfloat texScaleX = texSMax / xdim;
        
        startcolorno = 0;
        for (GLfloat x = xmin; x < xmax; x += brickSizeX)
        {
            int colorno = startcolorno;
            for (GLfloat y = 0.0f; y < height; y += brickSizeY)
            {
                GLint i, j;
                if (textureison) colorno = 7;
                glColor3fv(color[colorno]);
                for (j = 0, x1 = x, x2 = x + quadSizeX; j < quadsPerBrickX; ++j, x1 += quadSizeX, x2 += quadSizeX)
                {
                    t1 = texScaleX * (x1 - xmin);
                    t2 = texScaleX * (x2 - xmin);
                    for (i = 0, y1 = y, y2 = y + quadSizeY; i < quadsPerBrickY; ++i, y1 += quadSizeY, y2 += quadSizeY)
                    {
                        s1 = texScaleY * y1;
                        s2 = texScaleY * y2;
                        
                        glBegin(GL_QUADS);
                            // Wall on positive-z
                            glNormal3f(0.0f, 0.0f, -1.0f);
                            glTexCoord2f(t1, s1); glVertex3f(x1, y1, zmax);
                            glTexCoord2f(t1, s2); glVertex3f(x1, y2, zmax);
                            glTexCoord2f(t2, s2); glVertex3f(x2, y2, zmax);
                            glTexCoord2f(t2, s1); glVertex3f(x2, y1, zmax);

                            // Wall on negative-z
                            glNormal3f(0.0f, 0.0f, 1.0f);
                            glTexCoord2f(t1, s1); glVertex3f(x1, y1, zmin);
                            glTexCoord2f(t2, s1); glVertex3f(x2, y1, zmin);
                            glTexCoord2f(t2, s2); glVertex3f(x2, y2, zmin);
                            glTexCoord2f(t1, s2); glVertex3f(x1, y2, zmin);
                        glEnd();
                    }
                }
                ++colorno;
                if (colorno > 7) colorno = 0;
            }
            ++startcolorno;
            if (startcolorno > 7) startcolorno = 0;
        }
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void MyVirtualWorld::drawFourTeapot()
{
    glFrontFace(GL_CW);
    GLfloat dx = xmax - xmin - 20.0f;
    GLfloat dz = zmax - zmin - 20.0f;
    
    glPushMatrix();
        glTranslatef(0.0f, 5.0f, 0.0f);
        glTranslatef(xmax - 10.0f, 0.0f, zmax - 10.0f);
        
        // Red teapot
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidTeapot(5.0f);
        
        // Green teapot
        glTranslatef(-dx, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glutSolidTeapot(5.0f);
        
        // Blue teapot
        glTranslatef(0.0f, 0.0f, -dz);
        glColor3f(0.0f, 0.0f, 1.0f);
        glutSolidTeapot(5.0f);
        
        // Cyan teapot
        glTranslatef(dx, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 1.0f);
        glutSolidTeapot(5.0f);
    glPopMatrix();
    
    glFrontFace(GL_CCW);
}
