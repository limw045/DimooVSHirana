/*
 TCS2221 Computer Graphics
 CGLab04.cpp
 Objective: Lab04 on Other Primitives
*/
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "CGLab04.hpp"

using namespace CGLab04;

void MyUmbrella::draw()
{
    glDisable(GL_CULL_FACE);

    glBegin(GL_TRIANGLE_FAN);
        // --- Umbrella Top ---
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 5.0f, 0.0f); // Main center hub starting point

        glColor3f(0.0f, 0.5f, 0.1f);
        glVertex3f(-5.0f, 0.0f, 5.0f); // First fan face vertex

        glColor3f(1.0f, 0.5f, 0.5f);
        glVertex3f(0.0f, 0.0f, 7.0f); // 2nd

        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(5.0f, 0.0f, 5.0f); // 3rd

        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(7.0f, 0.0f, 0.0f); // 4th

        glColor3f(0.0f, 1.0f, 1.0f);
        glVertex3f(5.0f, 0.0f, -5.0f); // 5th

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -7.0f); // 6th

        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3f(-5.0f, 0.0f, -5.0f); // 7th

        glColor3f(1.5f, 0.5f, 0.0f);
        glVertex3f(-7.0f, 0.0f, 0.0f); // 8th

        glColor3f(0.0f, 0.5f, 0.1f);
        glVertex3f(-5.0f, 0.0f, 5.0f); // Back to first fan face vertex

        // --- Umbrella Handle ---
        // Using degenerate triangles trick to transition without calling glEnd()
        glColor3f(0.6f, 0.5f, 0.1f);
        glVertex3f(0.0f, 5.0f, 0.0f); // Restart hub at center top

        glColor3f(0.2f, 0.1f, 0.9f);
        glVertex3f(-1.0f, -10.0f, 1.0f); // Front-Left base

        glColor3f(1.0f, 0.6f, 0.2f);
        glVertex3f(1.0f, -10.0f, 1.0f); // Front-Right base

        glColor3f(0.3f, 0.8f, 0.4f);
        glVertex3f(1.0f, -10.0f, -1.0f); // Back-Right base

        glColor3f(0.9f, 0.1f, 0.2f);
        glVertex3f(-1.0f, -10.0f, -1.0f); // Back-Left base

        glColor3f(0.2f, 0.1f, 0.9f);
        glVertex3f(-1.0f, -10.0f, 1.0f); // Back to Front-Left base
    glEnd();

    glEnable(GL_CULL_FACE);
}

void MyMMU::draw1()
{
    glDisable(GL_CULL_FACE);

    // First M (Blue)
    glBegin(GL_QUAD_STRIP);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-15.0f, 0.0f, 0.0f);
        glVertex3f(-15.0f, 0.0f, 3.0f);
        glVertex3f(-15.0f, 8.0f, 0.0f);
        glVertex3f(-15.0f, 8.0f, 3.0f);
        glVertex3f(-10.0f, 5.0f, 0.0f);
        glVertex3f(-10.0f, 5.0f, 3.0f);
        glVertex3f(-5.0f, 8.0f, 0.0f);
        glVertex3f(-5.0f, 8.0f, 3.0f);
        glVertex3f(-5.0f, 0.0f, 0.0f);
        glVertex3f(-5.0f, 0.0f, 3.0f);
    glEnd();

    // Second M (Green)
    glBegin(GL_QUAD_STRIP);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 3.0f);
        glVertex3f(0.0f, 8.0f, 0.0f);
        glVertex3f(0.0f, 8.0f, 3.0f);
        glVertex3f(5.0f, 5.0f, 0.0f);
        glVertex3f(5.0f, 5.0f, 3.0f);
        glVertex3f(10.0f, 8.0f, 0.0f);
        glVertex3f(10.0f, 8.0f, 3.0f);
        glVertex3f(10.0f, 0.0f, 0.0f);
        glVertex3f(10.0f, 0.0f, 3.0f);
    glEnd();

    // U (Red)
    glBegin(GL_QUAD_STRIP);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(15.0f, 8.0f, 0.0f);
        glVertex3f(15.0f, 8.0f, 3.0f);
        glVertex3f(15.0f, 4.0f, 0.0f);
        glVertex3f(15.0f, 4.0f, 3.0f);
        glVertex3f(17.0f, 0.0f, 0.0f);
        glVertex3f(17.0f, 0.0f, 3.0f);
        glVertex3f(20.0f, 0.0f, 0.0f);
        glVertex3f(20.0f, 0.0f, 3.0f);
        glVertex3f(22.0f, 4.0f, 0.0f);
        glVertex3f(22.0f, 4.0f, 3.0f);
        glVertex3f(22.0f, 8.0f, 0.0f);
        glVertex3f(22.0f, 8.0f, 3.0f);
    glEnd();

    glEnable(GL_CULL_FACE);
}

void MyMMU::draw2()
{
    glDisable(GL_CULL_FACE);

    static GLfloat vertices[][3] =
    {
        {-15.0f, 0.0f, 0.0f}, // vertex 0
        {-15.0f, 0.0f, 3.0f}, // vertex 1
        {-15.0f, 8.0f, 0.0f}, // vertex 2
        {-15.0f, 8.0f, 3.0f}, // vertex 3
        {-10.0f, 5.0f, 0.0f}, // vertex 4
        {-10.0f, 5.0f, 3.0f}, // vertex 5
        {-5.0f, 8.0f, 0.0f},  // vertex 6
        {-5.0f, 8.0f, 3.0f},  // vertex 7
        {-5.0f, 0.0f, 0.0f},  // vertex 8
        {-5.0f, 0.0f, 3.0f}   // vertex 9
    };

    glBegin(GL_QUAD_STRIP);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3fv(vertices[0]);
        glVertex3fv(vertices[1]);
        glVertex3fv(vertices[2]);
        glVertex3fv(vertices[3]);
        glVertex3fv(vertices[4]);
        glVertex3fv(vertices[5]);
        glVertex3fv(vertices[6]);
        glVertex3fv(vertices[7]);
        glVertex3fv(vertices[8]);
        glVertex3fv(vertices[9]);
    glEnd();

    glEnable(GL_CULL_FACE);
}

// Complete implementation of MMU using loops & combined vertex list (Task 1B)
void MyMMU::draw3()
{
    glDisable(GL_CULL_FACE);

    static GLfloat vertices[][3] =
    {
        // First M
        {-15.0f, 0.0f, 0.0f }, // 0
        {-15.0f, 0.0f, 3.0f }, // 1
        {-15.0f, 8.0f, 0.0f }, // 2
        {-15.0f, 8.0f, 3.0f }, // 3
        {-10.0f, 5.0f, 0.0f }, // 4
        {-10.0f, 5.0f, 3.0f }, // 5
        { -5.0f, 8.0f, 0.0f }, // 6
        { -5.0f, 8.0f, 3.0f }, // 7
        { -5.0f, 0.0f, 0.0f }, // 8
        { -5.0f, 0.0f, 3.0f }, // 9

        // Second M
        { 0.0f, 0.0f, 0.0f },  // 10
        { 0.0f, 0.0f, 3.0f },  // 11
        { 0.0f, 8.0f, 0.0f },  // 12
        { 0.0f, 8.0f, 3.0f },  // 13
        { 5.0f, 5.0f, 0.0f },  // 14
        { 5.0f, 5.0f, 3.0f },  // 15
        { 10.0f, 8.0f, 0.0f }, // 16
        { 10.0f, 8.0f, 3.0f }, // 17
        { 10.0f, 0.0f, 0.0f }, // 18
        { 10.0f, 0.0f, 3.0f }, // 19

        // U
        { 15.0f, 8.0f, 0.0f }, // 20
        { 15.0f, 8.0f, 3.0f }, // 21
        { 15.0f, 4.0f, 0.0f }, // 22
        { 15.0f, 4.0f, 3.0f }, // 23
        { 17.0f, 0.0f, 0.0f }, // 24
        { 17.0f, 0.0f, 3.0f }, // 25
        { 20.0f, 0.0f, 0.0f }, // 26
        { 20.0f, 0.0f, 3.0f }, // 27
        { 22.0f, 4.0f, 0.0f }, // 28
        { 22.0f, 4.0f, 3.0f }, // 29
        { 22.0f, 8.0f, 0.0f }, // 30
        { 22.0f, 8.0f, 3.0f }  // 31
    };

    // First M (Blue)
    glBegin(GL_QUAD_STRIP);
        glColor3f(0.0f, 0.0f, 1.0f);
        for (int i = 0; i < 10; ++i) {
            glVertex3fv(vertices[i]);
        }
    glEnd();

    // Second M (Green)
    glBegin(GL_QUAD_STRIP);
        glColor3f(0.0f, 1.0f, 0.0f);
        for (int i = 10; i < 20; ++i) {
            glVertex3fv(vertices[i]);
        }
    glEnd();

    // U (Red)
    glBegin(GL_QUAD_STRIP);
        glColor3f(1.0f, 0.0f, 0.0f);
        for (int i = 20; i < 32; ++i) {
            glVertex3fv(vertices[i]);
        }
    glEnd();

    glEnable(GL_CULL_FACE);
}

// Complete implementation of 3D Diamond using face indices and loops (Task 3B)
void MyDiamond::draw()
{
    glDisable(GL_CULL_FACE);

    static GLfloat vertices[][3] =
    {
        // top tip
        { 0.0f, 10.0f, 0.0f}, // 0
        // central upper
        { 10.0f, 2.0f, 0.0f}, // 1
        { 5.0f, 2.0f, 8.5f},  // 2
        { -5.0f, 2.0f, 8.5f}, // 3
        {-10.0f, 2.0f, 0.0f}, // 4
        { -5.0f, 2.0f, -8.5f},// 5
        { 5.0f, 2.0f, -8.5f}, // 6
        // central lower
        { 10.0f, -2.0f, 0.0f},// 7
        { 5.0f, -2.0f, 8.5f}, // 8
        { -5.0f, -2.0f, 8.5f},// 9
        {-10.0f, -2.0f, 0.0f},// 10
        { -5.0f, -2.0f, -8.5f},// 11
        { 5.0f, -2.0f, -8.5f},// 12
        // bottom tip
        { 0.0f, -10.0f, 0.0f} // 13
    };

    // vertex indices of each polygon
    // -1 means end of polygon, -999 means end of faces list
    static int faces[] =
    {
        // 6 Top tip faces
        0, 1, 6, -1,
        0, 6, 5, -1,
        0, 5, 4, -1,
        0, 4, 3, -1,
        0, 3, 2, -1,
        0, 2, 1, -1,

        // 6 Central band faces
        1, 7, 12, 6, -1,
        6, 12, 11, 5, -1,
        5, 11, 10, 4, -1,
        4, 10, 9, 3, -1,
        3, 9, 8, 2, -1,
        2, 8, 7, 1, -1,

        // 6 Bottom tip faces (Task 3B completion)
        13, 7, 8, -1,
        13, 8, 9, -1,
        13, 9, 10, -1,
        13, 10, 11, -1,
        13, 11, 12, -1,
        13, 12, 7, -1,

        -999 // end
    };

    static GLfloat colors[][3] =
    {
        {0.0f, 0.0f, 1.0f}, // Blue
        {0.0f, 1.0f, 1.0f}, // Cyan
        {0.0f, 1.0f, 0.0f}, // Green
        {1.0f, 1.0f, 0.0f}, // Yellow
        {1.0f, 0.0f, 0.0f}, // Red
        {1.0f, 0.0f, 1.0f}, // Magenta
        {0.0f, 0.5f, 1.0f},
        {0.0f, 1.0f, 0.5f},
        {0.5f, 0.5f, 0.0f},
        {1.0f, 0.0f, 0.5f},
        {0.5f, 0.0f, 1.0f},
        {0.0f, 0.5f, 0.5f},
        {0.5f, 1.0f, 0.0f},
        {1.0f, 0.5f, 0.5f},
        {0.5f, 1.0f, 0.5f},
        {0.5f, 0.5f, 1.0f},
        {0.8f, 0.2f, 0.2f},
        {0.2f, 0.8f, 0.2f}
    };

    int n = 0;
    int facecount = 0;
    while (faces[n] != -999)
    {
        glColor3fv(colors[facecount]);
        glBegin(GL_POLYGON);
            // Sentinel loop to render current polygon
            while (faces[n] != -1)
            {
                glVertex3fv(vertices[faces[n]]);
                ++n;
            }
        glEnd();
        ++n; // Skip the -1 sentinel
        ++facecount;
    }

    glEnable(GL_CULL_FACE);
}

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
    glColor3f(0.3f, 0.3f, 0.4f); // Dim grey-blue
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
