/*
 TCS2221 Computer Graphics
 CGLab02.cpp
 Objective: Lab02 on Triangles, Quads and Polygons
*/
#include <GL/glut.h>
#include <cmath>
#include "CGLab02.hpp"

using namespace CGLab02;

// Draw 3D Umbrella (Extra Activity 2)
void MyTriangles::draw()
{
    glDisable(GL_CULL_FACE);

    // Canopy top point
    GLfloat tx = -15.0f, ty = 10.0f, tz = 5.0f;
    // Canopy bottom circle y coordinate and radius
    GLfloat cy = 8.0f;
    GLfloat R = 4.0f;

    // 8 vertices of the canopy circle
    GLfloat cx[8], cz[8];
    for (int i = 0; i < 8; i++) {
        float angle = i * 45.0f * 3.14159265f / 180.0f;
        cx[i] = tx + R * cos(angle);
        cz[i] = tz + R * sin(angle);
    }

    // Draw the 8 canopy triangles
    glBegin(GL_TRIANGLES);
        // Triangle 0: Green
        glColor3f(0.0f, 0.8f, 0.0f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[1], cy, cz[1]);
        glVertex3f(cx[0], cy, cz[0]);

        // Triangle 1: Red
        glColor3f(0.9f, 0.0f, 0.0f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[2], cy, cz[2]);
        glVertex3f(cx[1], cy, cz[1]);

        // Triangle 2: Orange
        glColor3f(0.9f, 0.5f, 0.0f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[3], cy, cz[3]);
        glVertex3f(cx[2], cy, cz[2]);

        // Triangle 3: Yellow
        glColor3f(0.9f, 0.9f, 0.0f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[4], cy, cz[4]);
        glVertex3f(cx[3], cy, cz[3]);

        // Triangle 4: Cyan
        glColor3f(0.0f, 0.8f, 0.8f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[5], cy, cz[5]);
        glVertex3f(cx[4], cy, cz[4]);

        // Triangle 5: Blue
        glColor3f(0.0f, 0.0f, 0.8f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[6], cy, cz[6]);
        glVertex3f(cx[5], cy, cz[5]);

        // Triangle 6: Magenta
        glColor3f(0.8f, 0.0f, 0.8f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[7], cy, cz[7]);
        glVertex3f(cx[6], cy, cz[6]);

        // Triangle 7: Purple
        glColor3f(0.8f, 0.4f, 0.6f);
        glVertex3f(tx, ty, tz);
        glVertex3f(cx[0], cy, cz[0]);
        glVertex3f(cx[7], cy, cz[7]);
    glEnd();

    // Draw the umbrella stick (thin long prism)
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.3f, 0.1f); // Brown color
        
        // Front face
        glVertex3f(tx - 0.15f,  8.0f, tz + 0.15f);
        glVertex3f(tx - 0.15f, -0.1f, tz + 0.15f);
        glVertex3f(tx + 0.15f, -0.1f, tz + 0.15f);
        glVertex3f(tx + 0.15f,  8.0f, tz + 0.15f);

        // Back face
        glVertex3f(tx + 0.15f,  8.0f, tz - 0.15f);
        glVertex3f(tx + 0.15f, -0.1f, tz - 0.15f);
        glVertex3f(tx - 0.15f, -0.1f, tz - 0.15f);
        glVertex3f(tx - 0.15f,  8.0f, tz - 0.15f);

        // Left face
        glVertex3f(tx - 0.15f,  8.0f, tz - 0.15f);
        glVertex3f(tx - 0.15f, -0.1f, tz - 0.15f);
        glVertex3f(tx - 0.15f, -0.1f, tz + 0.15f);
        glVertex3f(tx - 0.15f,  8.0f, tz + 0.15f);

        // Right face
        glVertex3f(tx + 0.15f,  8.0f, tz + 0.15f);
        glVertex3f(tx + 0.15f, -0.1f, tz + 0.15f);
        glVertex3f(tx + 0.15f, -0.1f, tz - 0.15f);
        glVertex3f(tx + 0.15f,  8.0f, tz - 0.15f);
    glEnd();

    // Draw the handle at the bottom
    glBegin(GL_QUADS);
        glColor3f(0.2f, 0.2f, 0.2f); // Dark grey
        // Front
        glVertex3f(tx - 0.3f, -0.1f, tz + 0.3f);
        glVertex3f(tx - 0.3f, -0.6f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.1f, tz + 0.3f);

        // Back
        glVertex3f(tx + 0.3f, -0.1f, tz - 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx - 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx - 0.3f, -0.1f, tz - 0.3f);

        // Left
        glVertex3f(tx - 0.3f, -0.1f, tz - 0.3f);
        glVertex3f(tx - 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx - 0.3f, -0.6f, tz + 0.3f);
        glVertex3f(tx - 0.3f, -0.1f, tz + 0.3f);

        // Right
        glVertex3f(tx + 0.3f, -0.1f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx + 0.3f, -0.1f, tz - 0.3f);

        // Top
        glVertex3f(tx - 0.3f, -0.1f, tz - 0.3f);
        glVertex3f(tx - 0.3f, -0.1f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.1f, tz + 0.3f);
        glVertex3f(tx + 0.3f, -0.1f, tz - 0.3f);

        // Bottom
        glVertex3f(tx - 0.3f, -0.6f, tz + 0.3f);
        glVertex3f(tx - 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz - 0.3f);
        glVertex3f(tx + 0.3f, -0.6f, tz + 0.3f);
    glEnd();

    glEnable(GL_CULL_FACE);
}

void MyRoom::draw()
{
    drawWalls();
    drawTable();
    drawDustbin();
    drawShelves();
}

void MyRoom::drawWalls()
{
    glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        // Back wall
        glColor3f(0.5f, 0.1f, 0.3f);
        glVertex3f(-25.1f, 15.1f, -5.1f);
        glVertex3f(-25.1f, -0.1f, -5.1f);
        glVertex3f( 25.1f, -0.1f, -5.1f);
        glVertex3f( 25.1f, 15.1f, -5.1f);

        // Left wall
        glColor3f(0.5f, 0.1f, 0.8f);
        glVertex3f(-25.1f, 15.1f,  15.1f);
        glVertex3f(-25.1f, -0.1f,  15.1f);
        glColor3f(0.3f, 0.8f, 0.1f);
        glVertex3f(-25.1f, -0.1f, -5.1f);
        glVertex3f(-25.1f, 15.1f, -5.1f);

        // Floor (Task 3B) - CCW viewed from above
        glColor3f(0.2f, 0.8f, 0.9f); // Cyan/light-blue
        glVertex3f(-25.1f, -0.1f,  15.1f);
        glVertex3f( 25.1f, -0.1f,  15.1f);
        glVertex3f( 25.1f, -0.1f, -5.1f);
        glVertex3f(-25.1f, -0.1f, -5.1f);
    glEnd();

    // Red Hexagon on the back wall (Extra Activity 1 & decoration)
    // Placed slightly in front of the back wall (z = -5.0) to avoid z-fighting.
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glVertex3f( 2.0f, 7.5f, -5.0f);
        glVertex3f( 1.0f, 9.23f, -5.0f);
        glVertex3f(-1.0f, 9.23f, -5.0f);
        glVertex3f(-2.0f, 7.5f, -5.0f);
        glVertex3f(-1.0f, 5.77f, -5.0f);
        glVertex3f( 1.0f, 5.77f, -5.0f);
    glEnd();

    glEnable(GL_CULL_FACE);
}

void MyRoom::drawTable()
{
    glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        // Table Top - Top face
        glColor3f(1.0f, 1.0f, 0.5f);
        glVertex3f(-10.0f, 7.0f,  5.0f);
        glVertex3f( 10.0f, 7.0f,  5.0f);
        glVertex3f( 10.0f, 7.0f, -5.0f);
        glVertex3f(-10.0f, 7.0f, -5.0f);

        // Table Top - Bottom face
        glVertex3f(-10.0f, 6.0f,  5.0f);
        glVertex3f(-10.0f, 6.0f, -5.0f);
        glVertex3f( 10.0f, 6.0f, -5.0f);
        glVertex3f( 10.0f, 6.0f,  5.0f);

        // Table Top - Front face
        glVertex3f(-10.0f, 7.0f,  5.0f);
        glVertex3f(-10.0f, 6.0f,  5.0f);
        glVertex3f( 10.0f, 6.0f,  5.0f);
        glVertex3f( 10.0f, 7.0f,  5.0f);

        // Table Top - Back face
        glVertex3f(-10.0f, 7.0f, -5.0f);
        glVertex3f( 10.0f, 7.0f, -5.0f);
        glVertex3f( 10.0f, 6.0f, -5.0f);
        glVertex3f(-10.0f, 6.0f, -5.0f);

        // --- Table Left Leg ---
        // Left
        glColor3f(0.5f, 0.5f, 1.0f);
        glVertex3f(-11.0f, 7.0f, -5.0f);
        glVertex3f(-11.0f, 0.0f, -5.0f);
        glVertex3f(-11.0f, 0.0f,  5.0f);
        glVertex3f(-11.0f, 7.0f,  5.0f);

        // Right
        glVertex3f(-10.0f, 7.0f, -5.0f);
        glVertex3f(-10.0f, 7.0f,  5.0f);
        glVertex3f(-10.0f, 0.0f,  5.0f);
        glVertex3f(-10.0f, 0.0f, -5.0f);

        // Front
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-11.0f, 7.0f,  5.0f);
        glVertex3f(-11.0f, 0.0f,  5.0f);
        glVertex3f(-10.0f, 0.0f,  5.0f);
        glVertex3f(-10.0f, 7.0f,  5.0f);

        // Back
        glVertex3f(-11.0f, 7.0f, -5.0f);
        glVertex3f(-10.0f, 7.0f, -5.0f);
        glVertex3f(-10.0f, 0.0f, -5.0f);
        glVertex3f(-11.0f, 0.0f, -5.0f);

        // Top
        glVertex3f(-11.0f, 7.0f, -5.0f);
        glVertex3f(-11.0f, 7.0f,  5.0f);
        glVertex3f(-10.0f, 7.0f,  5.0f);
        glVertex3f(-10.0f, 7.0f, -5.0f);

        // Bottom
        glVertex3f(-11.0f, 0.0f, -5.0f);
        glVertex3f(-10.0f, 0.0f, -5.0f);
        glVertex3f(-10.0f, 0.0f,  5.0f);
        glVertex3f(-11.0f, 0.0f,  5.0f);

        // --- Table Right Leg (Task 3B completion) ---
        // Left
        glColor3f(0.5f, 0.5f, 1.0f);
        glVertex3f(10.0f, 7.0f, -5.0f);
        glVertex3f(10.0f, 7.0f,  5.0f);
        glVertex3f(10.0f, 0.0f,  5.0f);
        glVertex3f(10.0f, 0.0f, -5.0f);

        // Right
        glVertex3f(11.0f, 7.0f, -5.0f);
        glVertex3f(11.0f, 0.0f, -5.0f);
        glVertex3f(11.0f, 0.0f,  5.0f);
        glVertex3f(11.0f, 7.0f,  5.0f);

        // Front
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(10.0f, 7.0f,  5.0f);
        glVertex3f(10.0f, 0.0f,  5.0f);
        glVertex3f(11.0f, 0.0f,  5.0f);
        glVertex3f(11.0f, 7.0f,  5.0f);

        // Back
        glVertex3f(11.0f, 7.0f, -5.0f);
        glVertex3f(10.0f, 7.0f, -5.0f);
        glVertex3f(10.0f, 0.0f, -5.0f);
        glVertex3f(11.0f, 0.0f, -5.0f);

        // Top
        glVertex3f(10.0f, 7.0f, -5.0f);
        glVertex3f(10.0f, 7.0f,  5.0f);
        glVertex3f(11.0f, 7.0f,  5.0f);
        glVertex3f(11.0f, 7.0f, -5.0f);

        // Bottom
        glVertex3f(10.0f, 0.0f, -5.0f);
        glVertex3f(11.0f, 0.0f, -5.0f);
        glVertex3f(11.0f, 0.0f,  5.0f);
        glVertex3f(10.0f, 0.0f,  5.0f);
    glEnd();

    glEnable(GL_CULL_FACE);
}

// Draw 3D Dustbin (Task 3B completion)
void MyRoom::drawDustbin()
{
    glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        // Orange color for dustbin
        glColor3f(0.9f, 0.3f, 0.2f);

        // Bottom face (y = 0.0)
        glVertex3f(14.0f, 0.0f, -2.0f);
        glVertex3f(18.0f, 0.0f, -2.0f);
        glVertex3f(18.0f, 0.0f,  2.0f);
        glVertex3f(14.0f, 0.0f,  2.0f);

        // Front face (tapered)
        glVertex3f(13.0f, 5.0f,  3.0f);
        glVertex3f(14.0f, 0.0f,  2.0f);
        glVertex3f(18.0f, 0.0f,  2.0f);
        glVertex3f(19.0f, 5.0f,  3.0f);

        // Back face (tapered)
        glVertex3f(19.0f, 5.0f, -3.0f);
        glVertex3f(18.0f, 0.0f, -2.0f);
        glVertex3f(14.0f, 0.0f, -2.0f);
        glVertex3f(13.0f, 5.0f, -3.0f);

        // Left face (tapered)
        glVertex3f(13.0f, 5.0f, -3.0f);
        glVertex3f(13.0f, 5.0f,  3.0f);
        glVertex3f(14.0f, 0.0f,  2.0f);
        glVertex3f(14.0f, 0.0f, -2.0f);

        // Right face (tapered)
        glVertex3f(19.0f, 5.0f,  3.0f);
        glVertex3f(19.0f, 5.0f, -3.0f);
        glVertex3f(18.0f, 0.0f, -2.0f);
        glVertex3f(18.0f, 0.0f,  2.0f);
    glEnd();

    glEnable(GL_CULL_FACE);
}

void MyRoom::drawShelves()
{
    glDisable(GL_CULL_FACE);

    // Bottom shelf: Yellowish
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 0.2f);
        glVertex3f(-25.0f, 2.0f, -5.0f);
        glVertex3f(-25.0f, 2.0f,  3.0f);
        glVertex3f(-21.0f, 2.0f,  3.0f);
        glVertex3f(-17.0f, 2.0f, -1.0f);
        glVertex3f(-17.0f, 2.0f, -5.0f);
    glEnd();

    // Middle shelf: Blue
    glBegin(GL_POLYGON);
        glColor3f(0.1f, 0.0f, 1.0f);
        glVertex3f(-25.0f, 5.0f, -5.0f);
        glVertex3f(-25.0f, 5.0f,  3.0f);
        glVertex3f(-21.0f, 5.0f,  3.0f);
        glVertex3f(-17.0f, 5.0f, -1.0f);
        glVertex3f(-17.0f, 5.0f, -5.0f);
    glEnd();

    // Top shelf: Red (Task 3C completion)
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-25.0f, 8.0f, -5.0f);
        glVertex3f(-25.0f, 8.0f,  3.0f);
        glVertex3f(-21.0f, 8.0f,  3.0f);
        glVertex3f(-17.0f, 8.0f, -1.0f);
        glVertex3f(-17.0f, 8.0f, -5.0f);
    glEnd();

    glEnable(GL_CULL_FACE);
}
