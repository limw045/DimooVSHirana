/*
 TCG6223 Computer Graphics
 CGLab10.cpp
 Objective: Lab10 on Lighting
 Copyright (C) 2011 by Ya-Ping Wong <ypwong@mmu.edu.my>
*/
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include "CGLab10.hpp"

using namespace CGLab10;

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
            glTranslatef(0.0f, -2.0f * dy, 0.0f); // translated from root
            // Wait, since we are using glPushMatrix, we translate relative to root:
            // The green spotlight was translated relative to root (if we pushed matrix) or relative to cylinder 1.
            // In the PDF page 38:
            // "glTranslatef(radius, 0.0f, 0.0f);"
            // "glTranslatef(-radius-dx, dy, 0.0f);" -> cumulative is (-dx, dy, 0)
            // "glTranslatef(0.0f, -2*dy, 0.0f);" -> cumulative is (-dx, -dy, 0)
            // Wait, let's verify positions:
            // Spotlight 1 is at (radius, 0)
            // Spotlight 2 is at (radius - radius - dx, 0 + dy) = (-dx, dy)
            // Spotlight 3 is at (-dx + 0, dy - 2*dy) = (-dx, -dy)
            // Yes! The cumulative translations in the PDF:
            // root -> radius -> (-dx, dy) -> (-dx, -dy)
            // Let's implement it using glPushMatrix/glPopMatrix for each, but translate to their exact absolute spots:
            // Spot 1: (radius, 0)
            // Spot 2: (-radius * cos(60 deg), radius * sin(60 deg)) = (-radius*0.5, radius*0.866) = (-dx, dy)
            // Spot 3: (-radius * cos(60 deg), -radius * sin(60 deg)) = (-radius*0.5, -radius*0.866) = (-dx, -dy)
            // This is exactly correct!
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

    // Pendulum 1 (rendered at Z = -20)
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

    // Pendulum 2 (rendered at Z = 20)
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

        // Draw the firefly as a bright yellow glowing sphere
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
    
    // Flying path in a 3D Lissajous infinity curve pattern inside the room
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
    
    for (int i = 0; i < 7; ++i) 
        lighton[i] = true;
}

MyVirtualWorld::~MyVirtualWorld()
{
}

void MyVirtualWorld::init()
{
    setupLights();
    timeold = glutGet(GLUT_ELAPSED_TIME);
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
    // Directional light position (homogenous component 0.0 represents a vector pointing from x=1.0, y=1.0)
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
    glPushMatrix();
        GLfloat tilesize = 0.25f;
        for (GLfloat z = zmin; z < zmax; z += tilesize)
        {
            for (GLfloat x = xmin; x < xmax; x += tilesize)
            {
                glColor3fv(mywhite);
                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(x,             0.0f, z);
                    glVertex3f(x,             0.0f, z + tilesize);
                    glVertex3f(x + tilesize,  0.0f, z + tilesize);
                    glVertex3f(x + tilesize,  0.0f, z);
                glEnd();
            }
        }
    glPopMatrix();
}

void MyVirtualWorld::drawRoof()
{
    static GLfloat* color[] = {myblack, mywhite};
    glPushMatrix();
        GLfloat xstep = 5.0f;
        GLfloat zstep = 5.0f;
        GLfloat tilesize = 2.5f;
        int startcolorno = 0;
        
        for (GLfloat z = zmin; z < zmax; z += zstep)
        {
            int colorno = startcolorno;
            for (GLfloat x = xmin; x < xmax; x += xstep)
            {
                glColor3fv(color[colorno]);
                GLfloat xmax_tile = x + xstep;
                GLfloat zmax_tile = z + zstep;
                
                for (GLfloat xx = x; xx < xmax_tile; xx += tilesize)
                {
                    for (GLfloat zz = z; zz < zmax_tile; zz += tilesize)
                    {
                        glBegin(GL_QUADS);
                            glNormal3f(0.0f, -1.0f, 0.0f);
                            glVertex3f(xx,            height, zz);
                            glVertex3f(xx + tilesize, height, zz);
                            glVertex3f(xx + tilesize, height, zz + tilesize);
                            glVertex3f(xx,            height, zz + tilesize);
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
}

void MyVirtualWorld::drawWalls()
{
    static GLfloat* color[] = {
        myblack, myred, myyellow, mygreen,
        mycyan, myblue, mymagenta, mywhite
    };

    glPushMatrix();
        GLfloat ystep = 5.0f;
        GLfloat zstep = 5.0f;
        GLfloat tilesize = 2.5f;
        int startcolorno = 0;

        // 1. Left (xmin) and Right (xmax) walls
        for (GLfloat z = zmin; z < zmax; z += zstep)
        {
            int colorno = startcolorno;
            for (GLfloat y = 0.0f; y < height; y += ystep)
            {
                glColor3fv(color[colorno]);
                GLfloat ymax_tile = y + ystep;
                GLfloat zmax_tile = z + zstep;
                
                for (GLfloat yy = y; yy < ymax_tile; yy += tilesize)
                {
                    for (GLfloat zz = z; zz < zmax_tile; zz += tilesize)
                    {
                        glBegin(GL_QUADS);
                            // Right wall
                            glNormal3f(-1.0f, 0.0f, 0.0f);
                            glVertex3f(xmax, yy,              zz);
                            glVertex3f(xmax, yy,              zz + tilesize);
                            glVertex3f(xmax, yy + tilesize,   zz + tilesize);
                            glVertex3f(xmax, yy + tilesize,   zz);

                            // Left wall
                            glNormal3f(1.0f, 0.0f, 0.0f);
                            glVertex3f(xmin, yy,              zz);
                            glVertex3f(xmin, yy + tilesize,   zz);
                            glVertex3f(xmin, yy + tilesize,   zz + tilesize);
                            glVertex3f(xmin, yy,              zz + tilesize);
                        glEnd();
                    }
                }
                ++colorno;
                if (colorno > 7) colorno = 0;
            }
            ++startcolorno;
            if (startcolorno > 7) startcolorno = 0;
        }

        // 2. Front (zmax) and Back (zmin) walls
        GLfloat xstep = 5.0f;
        startcolorno = 0;
        for (GLfloat x = xmin; x < xmax; x += xstep)
        {
            int colorno = startcolorno;
            for (GLfloat y = 0.0f; y < height; y += ystep)
            {
                glColor3fv(color[colorno]);
                GLfloat ymax_tile = y + ystep;
                GLfloat xmax_tile = x + xstep;
                
                for (GLfloat yy = y; yy < ymax_tile; yy += tilesize)
                {
                    for (GLfloat xx = x; xx < xmax_tile; xx += tilesize)
                    {
                        glBegin(GL_QUADS);
                            // Back wall
                            glNormal3f(0.0f, 0.0f, 1.0f);
                            glVertex3f(xx,            yy,              zmin);
                            glVertex3f(xx + tilesize, yy,              zmin);
                            glVertex3f(xx + tilesize, yy + tilesize,   zmin);
                            glVertex3f(xx,            yy + tilesize,   zmin);

                            // Front wall
                            glNormal3f(0.0f, 0.0f, -1.0f);
                            glVertex3f(xx,            yy,              zmax);
                            glVertex3f(xx,            yy + tilesize,   zmax);
                            glVertex3f(xx + tilesize, yy + tilesize,   zmax);
                            glVertex3f(xx + tilesize, yy,              zmax);
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
