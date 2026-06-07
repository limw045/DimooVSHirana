/*
 TCG6223 Computer Graphics
 CGLab09.cpp
 Objective: Lab09 on Animation (Three Bounding Boxes)
*/
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include "CGLab09.hpp"

using namespace CGLab09;

// ==========================================
// MyMovingSmiley Implementation
// ==========================================
MyMovingSmiley::MyMovingSmiley()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    
    // Initial position inside left box local coordinates
    posx = -5.0f; 
    posy = 0.0f; 
    posz = 2.0f;
    roty = 30.0f;

    // Initial velocities (units per second)
    velx = 12.0f;
    vely = 20.0f; // vertical velocity for bouncing
    velz = 10.0f;

    rotyspeed = 60.0f; // rotation speed (degrees per second)
    gravity = -40.0f;  // gravity acceleration (units per second^2)
}

MyMovingSmiley::~MyMovingSmiley()
{
    gluDeleteQuadric(pObj);
}

void MyMovingSmiley::draw()
{
    glDisable(GL_CULL_FACE);
    glPushMatrix();
        glTranslatef(posx, posy, posz);
        glRotatef(roty, 0.0f, 1.0f, 0.0f);
        
        glPushMatrix();
            // head
            glTranslatef(0.0f, 4.0f, 0.0f);
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow head
            gluSphere(pObj, 4.0f, 20, 20);

            // nose
            glTranslatef(0.0f, 0.0f, 4.0f);
            glColor3f(1.0f, 0.5f, 0.0f); // Orange nose
            gluCylinder(pObj, 1.0f, 0.0f, 2.0f, 5, 5);

            // eyes
            glPushMatrix();
                glTranslatef(1.5f, 1.5f, -1.2f);
                glColor3f(1.0f, 1.0f, 1.0f); // White eye back
                gluSphere(pObj, 1.0f, 10, 10);
                glTranslatef(0.0f, 0.2f, 0.5f);
                glColor3f(0.0f, 0.5f, 1.0f); // Blue iris
                gluSphere(pObj, 0.7f, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-1.5f, 1.5f, -1.2f);
                glColor3f(1.0f, 1.0f, 1.0f); // White eye back
                gluSphere(pObj, 1.0f, 10, 10);
                glTranslatef(0.0f, 0.2f, 0.5f);
                glColor3f(0.0f, 0.5f, 1.0f); // Blue iris
                gluSphere(pObj, 0.7f, 10, 10);
            glPopMatrix();

            // mouth
            GLboolean cullingIsOn;
            glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
            glDisable(GL_CULL_FACE);
            
            glTranslatef(0.0f, -1.8f, -1.2f);
            glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(1.0f, 0.0f, 0.0f); // Red mouth disk
            gluDisk(pObj, 0.0f, 1.5f, 10, 10);

            if (cullingIsOn == GL_TRUE) 
                glEnable(GL_CULL_FACE);
        glPopMatrix();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void MyMovingSmiley::tickTime(long int elapseTime)
{
    float dt = elapseTime / 1000.0f;

    // Update Smiley position and rotation
    posx += dt * velx;
    posz += dt * velz;

    // Apply gravity
    vely += dt * gravity;
    posy += dt * vely;

    roty += dt * rotyspeed;
    if (roty >= 360.0f) roty -= 360.0f;

    // Local box dimensions: [-15.0f, 15.0f] on all axes
    // Smiley head is a sphere of radius 4.0 centered at (posx, posy + 4.0, posz)
    
    // Collision check for X (left/right walls)
    // Head radius is 4.0, so center posx limits are [-11.0f, 11.0f]
    if (posx > 11.0f) {
        posx = 11.0f - (posx - 11.0f);
        velx = -velx;
    } else if (posx < -11.0f) {
        posx = -11.0f - (posx + 11.0f);
        velx = -velx;
    }

    // Collision check for Z (front/back walls)
    // Center posz limits are [-11.0f, 11.0f]
    if (posz > 11.0f) {
        posz = 11.0f - (posz - 11.0f);
        velz = -velz;
    } else if (posz < -11.0f) {
        posz = -11.0f - (posz + 11.0f);
        velz = -velz;
    }

    // Collision check for Y (floor bounce)
    // The bottom of the head is at posy. The floor is at -15.0f
    if (posy < -15.0f) {
        posy = -15.0f;
        // Perfect elastic bounce
        vely = -vely;
    }

    // Collision check for Y (ceiling bounce)
    // The top of the head is at posy + 8.0f. The ceiling is at +15.0f
    if (posy + 8.0f > 15.0f) {
        posy = 7.0f - (posy + 8.0f - 15.0f);
        vely = -vely;
    }
}

// ==========================================
// MyFan Implementation
// ==========================================
MyFan::MyFan()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    
    pitchangle = -20.0f;
    swingangle = 45.0f;
    wingsangle = 30.0f;
    
    swingspeed = 90.0f;  // swing oscillation speed (degrees per second)
    wingsspeed = 720.0f; // blade spin speed (degrees per second)

    wingsOn = true;
    swingOn = true;
}

MyFan::~MyFan()
{
    gluDeleteQuadric(pObj);
}

void MyFan::tickTime(long int elapseTime)
{
    float dt = elapseTime / 1000.0f;

    // Update blade spin
    if (wingsOn) {
        wingsangle += dt * wingsspeed;
        if (wingsangle >= 360.0f) wingsangle -= 360.0f;
    }

    // Update head swing (oscillate between -45 and +45)
    if (swingOn) {
        swingangle += dt * swingspeed;
        if (swingangle > 45.0f) {
            swingangle = 45.0f;
            swingspeed = -swingspeed;
        } else if (swingangle < -45.0f) {
            swingangle = -45.0f;
            swingspeed = -swingspeed;
        }
    }
}

void MyFan::draw()
{
    glDisable(GL_CULL_FACE);
    glPushMatrix();
        // Shift local origin to sit on the floor of the box (Y = -15.0f)
        glTranslatef(0.0f, -15.0f, 0.0f);
        
        drawBase();
        
        // Translate to top of the middle column (Y = 10.0f)
        glTranslatef(0.0f, 10.0f, 0.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(swingangle, 0.0f, 1.0f, 0.0f);
        glRotatef(pitchangle, 1.0f, 0.0f, 0.0f);
        
        // Translate to middle joint
        glTranslatef(0.0f, 0.0f, 2.0f);
        drawMiddle();
        
        // Translate to fan blades position
        glTranslatef(0.0f, 0.0f, 6.0f);
        glRotatef(wingsangle, 0.0f, 0.0f, 1.0f);
        drawWings();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void MyFan::drawBase()
{
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        
        // Red tapered bottom base
        glColor3f(1.0f, 0.0f, 0.0f);
        gluCylinder(pObj, 5.0f, 1.0f, 2.0f, 10, 10);
        
        // Blue middle stand column
        glColor3f(0.0f, 0.0f, 1.0f);
        gluCylinder(pObj, 1.0f, 1.0f, 10.0f, 10, 10);
        
        // Bottom disk base cover
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        gluDisk(pObj, 0.0f, 5.0f, 10, 10);
    glPopMatrix();
}

void MyFan::drawMiddle()
{
    glPushMatrix();
        glScalef(0.3f, 0.3f, 1.0f);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow sphere
        gluSphere(pObj, 5.0f, 10, 10);
        
        glTranslatef(0.0f, 0.0f, 5.0f);
        gluCylinder(pObj, 0.5f, 0.5f, 1.5f, 10, 10);
    glPopMatrix();
}

void MyFan::drawWings()
{
    GLboolean cullingIsOn;
    glGetBooleanv(GL_CULL_FACE, &cullingIsOn);
    glDisable(GL_CULL_FACE);
    
    GLboolean normalizeIsOn;
    glGetBooleanv(GL_NORMALIZE, &normalizeIsOn);
    glEnable(GL_NORMALIZE);

    glColor3f(0.5f, 1.0f, 0.5f); // Green blades
    glPushMatrix();
        glBegin(GL_TRIANGLES);
            // Blade 1 (pointing up)
            glNormal3f( 1.0f, 0.0f, 2.0f);
            glVertex3f( 0.0f, 0.0f, 0.0f);
            glVertex3f( 0.0f, 4.0f, 0.0f);
            glVertex3f(-2.0f, 4.0f, 1.0f);

            // Blade 2 (pointing left)
            glNormal3f( 0.0f, 1.0f, 2.0f);
            glVertex3f( 0.0f, 0.0f, 0.0f);
            glVertex3f(-4.0f, 0.0f, 0.0f);
            glVertex3f(-4.0f,-2.0f, 1.0f);

            // Blade 3 (pointing down)
            glNormal3f(-1.0f, 0.0f, 2.0f);
            glVertex3f( 0.0f, 0.0f, 0.0f);
            glVertex3f( 0.0f,-4.0f, 0.0f);
            glVertex3f( 2.0f,-4.0f, 1.0f);

            // Blade 4 (pointing right)
            glNormal3f( 0.0f,-1.0f, 2.0f);
            glVertex3f( 0.0f, 0.0f, 0.0f);
            glVertex3f( 4.0f, 0.0f, 0.0f);
            glVertex3f( 4.0f, 2.0f, 1.0f);
        glEnd();
    glPopMatrix();

    if (cullingIsOn == GL_TRUE) 
        glEnable(GL_CULL_FACE);
    if (normalizeIsOn == GL_FALSE) 
        glDisable(GL_NORMALIZE);
}

void MyFan::toggleRotation()
{
    wingsOn = !wingsOn;
    std::cout << "[Fan Control] Blade rotation toggled " << (wingsOn ? "ON" : "OFF") << std::endl;
}

void MyFan::toggleSwing()
{
    swingOn = !swingOn;
    std::cout << "[Fan Control] Oscillating swing toggled " << (swingOn ? "ON" : "OFF") << std::endl;
}

// ==========================================
// MyDancingSnowman Implementation
// ==========================================
MyDancingSnowman::MyDancingSnowman()
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    bounceY = 0.0f;
    rotY = 0.0f;
    tiltZ = 0.0f;

    bounceSpeed = 4.5f;
    rotSpeed = 80.0f;
    tiltSpeed = 3.5f;
    timeAccumulator = 0.0f;
}

MyDancingSnowman::~MyDancingSnowman()
{
    gluDeleteQuadric(pObj);
}

void MyDancingSnowman::tickTime(long int elapseTime)
{
    float dt = elapseTime / 1000.0f;
    timeAccumulator += dt;

    // Cute hopping animation (absolute sine for floor bounce effect)
    bounceY = 4.0f * (float)fabs(sin(timeAccumulator * bounceSpeed));

    // Dynamic rotation
    rotY += dt * rotSpeed;
    if (rotY >= 360.0f) rotY -= 360.0f;

    // Side-to-side body wobble
    tiltZ = 12.0f * sin(timeAccumulator * tiltSpeed);
}

void MyDancingSnowman::draw()
{
    glDisable(GL_CULL_FACE);
    glPushMatrix();
        // Translate snowman so bottom sits on floor (Y = -15.0f) + bounce
        // Bottom sphere center in local is -6.0, radius 5.0, so base is at -11.0.
        // Therefore, translating down by -4.0 places the base at -15.0.
        glTranslatef(0.0f, -4.0f + bounceY, 0.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(tiltZ, 0.0f, 0.0f, 1.0f);

        // 1. Bottom sphere (large, white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(0.0f, -6.0f, 0.0f);
            gluSphere(pObj, 5.0f, 32, 32);
        glPopMatrix();

        // 2. Middle sphere (medium, white)
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f);
            gluSphere(pObj, 3.5f, 32, 32);
        glPopMatrix();

        // 3. Head sphere (small, white)
        glPushMatrix();
            glTranslatef(0.0f, 4.5f, 0.0f);
            gluSphere(pObj, 2.5f, 32, 32);
        glPopMatrix();

        // 4. Eyes (black dots)
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.8f, 5.2f, 2.1f);
            gluSphere(pObj, 0.3f, 16, 16);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.8f, 5.2f, 2.1f);
            gluSphere(pObj, 0.3f, 16, 16);
        glPopMatrix();

        // 5. Nose (orange cone pointing along +z)
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, 4.5f, 2.0f);
            gluCylinder(pObj, 0.5f, 0.0f, 2.5f, 16, 16);
        glPopMatrix();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

// ==========================================
// MyBoundingBox Implementation
// ==========================================
void MyBoundingBox::draw()
{
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow outline

        // Top loop
        glVertex3f(x[0], y[0], z[0]); glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[1], y[1], z[1]); glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[2], y[2], z[2]); glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[3], y[3], z[3]); glVertex3f(x[0], y[0], z[0]);

        // Bottom loop
        glVertex3f(x[4], y[4], z[4]); glVertex3f(x[5], y[5], z[5]);
        glVertex3f(x[5], y[5], z[5]); glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[6], y[6], z[6]); glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[7], y[7], z[7]); glVertex3f(x[4], y[4], z[4]);

        // Vertical edges
        glVertex3f(x[0], y[0], z[0]); glVertex3f(x[4], y[4], z[4]);
        glVertex3f(x[1], y[1], z[1]); glVertex3f(x[5], y[5], z[5]);
        glVertex3f(x[2], y[2], z[2]); glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[3], y[3], z[3]); glVertex3f(x[7], y[7], z[7]);
    glEnd();
}

void MyBoundingBox::drawGrid()
{
    glLineWidth(0.5f);
    glColor3f(0.3f, 0.3f, 0.4f); // Dim gray-blue
    glBegin(GL_LINES);
        GLfloat hx = dx / 2.0f;
        GLfloat hy = dy / 2.0f;
        GLfloat hz = dz / 2.0f;

        // Compute division coordinates inside the box (at 1/4 intervals)
        float div_x[] = { -hx / 2.0f, 0.0f, hx / 2.0f };
        float div_y[] = { -hy / 2.0f, 0.0f, hy / 2.0f };
        float div_z[] = { -hz / 2.0f, 0.0f, hz / 2.0f };

        // Front (z = cz + hz) & Back (z = cz - hz) faces
        for (int i = 0; i < 3; i++) {
            float vx = cx + div_x[i];
            float vy = cy + div_y[i];
            // Vertical lines
            glVertex3f(vx, cy - hy, cz + hz); glVertex3f(vx, cy + hy, cz + hz);
            glVertex3f(vx, cy - hy, cz - hz); glVertex3f(vx, cy + hy, cz - hz);
            // Horizontal lines
            glVertex3f(cx - hx, vy, cz + hz); glVertex3f(cx + hx, vy, cz + hz);
            glVertex3f(cx - hx, vy, cz - hz); glVertex3f(cx + hx, vy, cz - hz);
        }

        // Left (x = cx - hx) & Right (x = cx + hx) faces
        for (int i = 0; i < 3; i++) {
            float vy = cy + div_y[i];
            float vz = cz + div_z[i];
            // Vertical lines
            glVertex3f(cx - hx, cy - hy, vz); glVertex3f(cx - hx, cy + hy, vz);
            glVertex3f(cx + hx, cy - hy, vz); glVertex3f(cx + hx, cy + hy, vz);
            // Horizontal lines
            glVertex3f(cx - hx, vy, cz - hz); glVertex3f(cx - hx, vy, cz + hz);
            glVertex3f(cx + hx, vy, cz - hz); glVertex3f(cx + hx, vy, cz + hz);
        }

        // Top (y = cy + hy) & Bottom (y = cy - hy) faces
        for (int i = 0; i < 3; i++) {
            float vx = cx + div_x[i];
            float vz = cz + div_z[i];
            // Parallel to Z
            glVertex3f(vx, cy + hy, cz - hz); glVertex3f(vx, cy + hy, cz + hz);
            glVertex3f(vx, cy - hy, cz - hz); glVertex3f(vx, cy - hy, cz + hz);
            // Parallel to X
            glVertex3f(cx - hx, cy + hy, vz); glVertex3f(cx + hx, cy + hy, vz);
            glVertex3f(cx - hx, cy - hy, vz); glVertex3f(cx + hx, cy - hy, vz);
        }
    glEnd();
}

// ==========================================
// MyVirtualWorld Keyboard Processing
// ==========================================
void MyVirtualWorld::keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'f':
        case 'F':
            myfan.toggleRotation();
            break;
        case 'g':
        case 'G':
            myfan.toggleSwing();
            break;
        default:
            break;
    }
}
