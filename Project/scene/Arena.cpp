#include "Arena.h"
#include "../CGImageLoader.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
#ifndef GL_TEXTURE_FILTER_CONTROL
#define GL_TEXTURE_FILTER_CONTROL 0x8500
#endif
#ifndef GL_TEXTURE_LOD_BIAS
#define GL_TEXTURE_LOD_BIAS 0x8501
#endif

/**
 * @brief Helper function to load a JPG or BMP texture safely.
 * @param filename Absolute or relative path to the image file.
 * @return OpenGL texture identifier, or 0 if failed.
 */
static GLuint loadTextureHelper(const char* filename) {
    if (filename == nullptr) {
        std::cerr << "[Arena] Error: Texture filename is null." << std::endl;
        return 0;
    }

    std::string fileStr(filename);
    bool isBmp = false;
    if (fileStr.length() >= 4) {
        std::string ext = fileStr.substr(fileStr.length() - 4);
        if (ext == ".bmp" || ext == ".BMP") {
            isBmp = true;
        }
    }

    MyImage img;
    if (isBmp) {
        img.loadBMP((char*)filename);
        // BMP is stored bottom-up. Flip pixels vertically to match JPEG layout.
        if (img.buffer) {
            int rowSize = img.width * 3;
            unsigned char* tempRow = (unsigned char*)malloc(rowSize);
            if (tempRow) {
                for (unsigned int i = 0; i < img.height / 2; ++i) {
                    unsigned char* row1 = img.buffer + i * rowSize;
                    unsigned char* row2 = img.buffer + (img.height - 1 - i) * rowSize;
                    memcpy(tempRow, row1, rowSize);
                    memcpy(row1, row2, rowSize);
                    memcpy(row2, tempRow, rowSize);
                }
                free(tempRow);
            }
        }
    } else {
        img.loadJPG((char*)filename);
    }

    if (!img.buffer) {
        std::cerr << "[Arena] Failed to load texture: " << filename << std::endl;
        return 0;
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Enable hardware anisotropic filtering if supported
    GLfloat maxAnisotropy = 1.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    if (maxAnisotropy > 1.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    // Set Mipmap LOD Bias for sharper textures
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -0.6f);
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, img.width, img.height, GL_RGB, GL_UNSIGNED_BYTE, img.buffer);
    
    std::cout << "[Arena] Successfully loaded texture: " << filename << " (" << img.width << "x" << img.height << ") | Anisotropy: " << maxAnisotropy << std::endl;
    return tex;
}

Arena::Arena() {
    hironoHpPct = 1.0f;
    dimooHpPct = 1.0f;
    minHpPct = 1.0f;

    leftWallOffset = 0.0f;
    rightWallOffset = 0.0f;
    leftWallShakeTime = 0.0f;
    rightWallShakeTime = 0.0f;

    cardboardTex = 0;
    woodTex = 0;
    roseLabelTex = 0;
    starrySkyTex = 0;
    hironoFaceTex = 0;
    dimooFaceTex = 0;
    floorTex = 0;
    leftRightTex = 0;
    backTex = 0;

    initHolesAndBeams();

    // Place bubble cells as a full-floor packing liner inside the box.
    const int bubbleCols = 25;
    const int bubbleRows = 9;
    const float startX = -8.10f;
    const float startZ = -2.40f;
    const float stepX = 16.20f / (float)(bubbleCols - 1);
    const float stepZ = 4.80f / (float)(bubbleRows - 1);
    for (int row = 0; row < bubbleRows; ++row) {
        for (int col = 0; col < bubbleCols; ++col) {
            BubbleCell cell;
            cell.x = startX + col * stepX;
            cell.y = 0.005f;
            cell.z = startZ + row * stepZ;
            cell.pressDepth = 0.0f;
            cell.pressVelocity = 0.0f;
            cell.popped = false;
            bubbles.push_back(cell);
        }
    }
}

Arena::~Arena() {
    if (cardboardTex) glDeleteTextures(1, &cardboardTex);
    if (woodTex) glDeleteTextures(1, &woodTex);
    if (roseLabelTex) glDeleteTextures(1, &roseLabelTex);
    if (starrySkyTex) glDeleteTextures(1, &starrySkyTex);
    if (hironoFaceTex) glDeleteTextures(1, &hironoFaceTex);
    if (dimooFaceTex) glDeleteTextures(1, &dimooFaceTex);
    if (floorTex) glDeleteTextures(1, &floorTex);
    if (leftRightTex) glDeleteTextures(1, &leftRightTex);
    if (backTex) glDeleteTextures(1, &backTex);
}

void Arena::init() {
    // Load designated arena textures (lossless BMP)
    floorTex = loadTextureHelper("images/Floor.bmp");
    leftRightTex = loadTextureHelper("images/LeftRight.bmp");
    backTex = loadTextureHelper("images/Background.bmp");

    // Load legacy textures to prevent visual brokenness
    cardboardTex = loadTextureHelper("images/cardboard.jpg");
    woodTex = loadTextureHelper("images/wood.jpg");
    roseLabelTex = loadTextureHelper("images/rose_label.jpg");
    starrySkyTex = loadTextureHelper("images/starry_sky.jpg");
    hironoFaceTex = loadTextureHelper("images/hirono_face.jpg");
    dimooFaceTex = loadTextureHelper("images/dimoo_face.jpg");
}

void Arena::update(float dt) {
    // Validate delta time first to prevent physical divergence
    if (dt < 0.0f) {
        std::cerr << "[Arena] Warning: Negative delta time ignored." << std::endl;
        return;
    }
    
    // Clamp dt to avoid frame lag spikes blowing up the physics
    dt = clamp(dt, 0.0f, 0.05f);

    // 1. Lid Oscillation Physics
    float spring = -lid.springK * (lid.angle - lid.restAngle);
    float damp = -lid.damping * lid.velocity;
    lid.velocity += (spring + damp) * dt;
    lid.angle += lid.velocity * dt;
    lid.angle = clamp(lid.angle, 90.0f, 180.0f);

    // 2. Left and Right Wall Vibration Decay
    if (leftWallShakeTime > 0.0f) {
        leftWallShakeTime -= dt;
        leftWallOffset = exp(-4.0f * (0.5f - leftWallShakeTime)) * sin(25.0f * leftWallShakeTime) * 0.12f;
        if (leftWallShakeTime <= 0.0f) leftWallOffset = 0.0f;
    }
    if (rightWallShakeTime > 0.0f) {
        rightWallShakeTime -= dt;
        rightWallOffset = exp(-4.0f * (0.5f - rightWallShakeTime)) * sin(25.0f * rightWallShakeTime) * 0.12f;
        if (rightWallShakeTime <= 0.0f) rightWallOffset = 0.0f;
    }

    // 3. Stubs: Bubble padding recovery
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float bSpring = -25.0f * bubbles[i].pressDepth;
        float bDamp = -6.0f * bubbles[i].pressVelocity;
        bubbles[i].pressVelocity += (bSpring + bDamp) * dt;
        bubbles[i].pressDepth += bubbles[i].pressVelocity * dt;
        bubbles[i].pressDepth = clamp(bubbles[i].pressDepth, 0.0f, 1.0f);
    }

    // 4. Stubs: Pamphlet flying physics
    if (pamphlet.isFlying) {
        pamphlet.x += pamphlet.vx * dt;
        pamphlet.y += pamphlet.vy * dt;
        pamphlet.vy -= 5.8f * dt;
        pamphlet.rot += pamphlet.rotV * dt;
        pamphlet.foldAngle += 0.8f * dt;
        if (pamphlet.foldAngle > 1.0f) pamphlet.foldAngle = 1.0f;

        if (pamphlet.y <= 0.1f) {
            pamphlet.y = 0.1f;
            pamphlet.vy = 0.0f;
            pamphlet.vx = 0.0f;
            pamphlet.rotV = 0.0f;
            pamphlet.rot = 0.0f;
            pamphlet.isFlying = false;
        }
    }

    // 5. Stubs: Desiccant packet sliding physics
    float speedSq = desiccant.vx * desiccant.vx;
    if (speedSq > 0.0001f) {
        float speed = sqrt(speedSq);
        float friction = 1.6f;
        desiccant.vx -= (desiccant.vx / speed) * friction * dt;
        desiccant.x += desiccant.vx * dt;

        float limitX = BOX_WIDTH / 2.0f - 0.25f;
        if (desiccant.x > limitX) {
            desiccant.x = limitX;
            desiccant.vx *= -0.6f;
            triggerWallShake(0.3f, false);
        }
        if (desiccant.x < -limitX) {
            desiccant.x = -limitX;
            desiccant.vx *= -0.6f;
            triggerWallShake(0.3f, true);
        }
    }

    // 6. Update Stateful Space Objects (Asteroids & Floating Boxes)
    {
        float halfW = BOX_WIDTH / 2.0f;
        float halfD = BOX_DEPTH / 2.0f;
        float height = BOX_HEIGHT;
        float wallThick = 0.15f;
        float tHalfW = halfW + 4.0f;
        float tHalfD = halfD + 3.0f;
        float tThick = 0.35f;

        float rest = 0.82f; // restitution

        struct AABB { float minX, maxX, minY, maxY, minZ, maxZ; };
        AABB obstacles[2] = {
            // Table Slab
            { -tHalfW, tHalfW, -tThick, -0.01f, -tHalfD, tHalfD },
            // Cardboard Box
            { -halfW - wallThick, halfW + wallThick, 0.0f, height, -halfD - wallThick, halfD }
        };

        for (auto& obj : spaceObjects) {
            // Update position
            obj.x += obj.vx * dt;
            obj.y += obj.vy * dt;
            obj.z += obj.vz * dt;

            // Update rotation
            obj.rx += obj.rvx * dt;
            obj.ry += obj.rvy * dt;
            obj.rz += obj.rvz * dt;

            float bRad = (obj.type == 0) ? obj.size : (1.8f * obj.size);

            // Collision check against AABB obstacles
            for (int k = 0; k < 2; ++k) {
                const auto& obs = obstacles[k];
                float px = clamp(obj.x, obs.minX, obs.maxX);
                float py = clamp(obj.y, obs.minY, obs.maxY);
                float pz = clamp(obj.z, obs.minZ, obs.maxZ);

                float dx = obj.x - px;
                float dy = obj.y - py;
                float dz = obj.z - pz;
                float dist = sqrt(dx*dx + dy*dy + dz*dz);

                if (dist < bRad) {
                    float nx = 0.0f, ny = 0.0f, nz = 0.0f;
                    if (dist > 0.001f) {
                        nx = dx / dist;
                        ny = dy / dist;
                        nz = dz / dist;
                    } else {
                        float cx = (obs.minX + obs.maxX) / 2.0f;
                        float cy = (obs.minY + obs.maxY) / 2.0f;
                        float cz = (obs.minZ + obs.maxZ) / 2.0f;
                        float pdx = obj.x - cx;
                        float pdy = obj.y - cy;
                        float pdz = obj.z - cz;
                        float plen = sqrt(pdx*pdx + pdy*pdy + pdz*pdz);
                        if (plen > 0.001f) {
                            nx = pdx / plen;
                            ny = pdy / plen;
                            nz = pdz / plen;
                        } else {
                            ny = 1.0f;
                        }
                    }

                    // Push out
                    obj.x = px + nx * bRad;
                    obj.y = py + ny * bRad;
                    obj.z = pz + nz * bRad;

                    // Reflect
                    float dotVal = obj.vx * nx + obj.vy * ny + obj.vz * nz;
                    obj.vx = (obj.vx - 2.0f * dotVal * nx) * rest;
                    obj.vy = (obj.vy - 2.0f * dotVal * ny) * rest;
                    obj.vz = (obj.vz - 2.0f * dotVal * nz) * rest;

                    obj.rvx += ((rand() % 100) / 100.0f - 0.5f) * 15.0f;
                    obj.rvy += ((rand() % 100) / 100.0f - 0.5f) * 15.0f;
                    obj.rvz += ((rand() % 100) / 100.0f - 0.5f) * 15.0f;
                }
            }

            // Containment inside R = 40.0f sphere
            float distFromOrigin = sqrt(obj.x*obj.x + obj.y*obj.y + obj.z*obj.z);
            if (distFromOrigin > 39.5f) {
                float nx = -obj.x / distFromOrigin;
                float ny = -obj.y / distFromOrigin;
                float nz = -obj.z / distFromOrigin;

                obj.x = -nx * 39.4f;
                obj.y = -ny * 39.4f;
                obj.z = -nz * 39.4f;

                float dotVal = obj.vx * nx + obj.vy * ny + obj.vz * nz;
                obj.vx = (obj.vx - 2.0f * dotVal * nx) * rest;
                obj.vy = (obj.vy - 2.0f * dotVal * ny) * rest;
                obj.vz = (obj.vz - 2.0f * dotVal * nz) * rest;
            }
        }
    }

    updateLightBeamsAndParticles(dt);
}

void Arena::triggerWallShake(float intensity, bool leftWall) {
    if (intensity < 0.0f) return;
    if (leftWall) leftWallShakeTime = 0.5f;
    else rightWallShakeTime = 0.5f;
}

void Arena::triggerLidShake(float intensity) {
    lid.velocity += intensity;
}

void Arena::testExplodeProps() {
    if (!pamphlet.isFlying) {
        pamphlet.isFlying = true;
        pamphlet.y = 0.3f;
        pamphlet.vy = 2.5f + (rand() % 100) / 100.0f;
        pamphlet.vx = ((rand() % 200) / 100.0f - 1.0f) * 1.8f;
        pamphlet.rotV = 360.0f * ((rand() % 2 == 0) ? 1.0f : -1.0f);
        pamphlet.foldAngle = 0.0f;
    }
    desiccant.vx = ((rand() % 200) / 100.0f - 1.0f) * 5.0f;
    desiccant.popped = false;
}

void Arena::resolveCollisionWithProps(float charX, float charY, float radius, float& outPushX) {
    outPushX = 0.0f;
    if (radius < 0.0f) return;

    // Check bubble wrap stamping
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float dx = bubbles[i].x - charX;
        float dz = bubbles[i].z;
        float footDist = sqrt(dx * dx + dz * dz);
        if (footDist < radius * 0.82f && charY < 0.1f) {
            bubbles[i].pressDepth = 1.0f;
            bubbles[i].pressVelocity = -5.0f;
            if ((rand() % 100) < 2) {
                bubbles[i].popped = true;
            }
        }
    }

    // Check sliding desiccant packet interaction
    float dx = desiccant.x - charX;
    float dy = desiccant.y - charY;
    float dist = sqrt(dx*dx + dy*dy);
    float minDist = radius + 0.16f;
    if (dist < minDist && dist > 0.001f) {
        float push = minDist - dist;
        outPushX = -(dx / dist) * push * 0.1f;
        desiccant.x += (dx / dist) * push * 0.9f;
        desiccant.vx += (dx / dist) * 3.5f;
    }
}

void Arena::drawTexturedQuad(float x1, float y1, float z1,
                             float x2, float y2, float z2,
                             float x3, float y3, float z3,
                             float x4, float y4, float z4,
                             float uMax, float vMax) {
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);
    glTexCoord2f(uMax, 0.0f); glVertex3f(x2, y2, z2);
    glTexCoord2f(uMax, vMax); glVertex3f(x3, y3, z3);
    glTexCoord2f(0.0f, vMax); glVertex3f(x4, y4, z4);
    glEnd();
}

// Static helper to draw a procedurally banded gas giant planet
static void drawProceduralGasGiant(float x, float y, float z, float radius) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(20.0f, 1.0f, 0.0f, 1.0f); // Slight axial tilt
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    int lats = 30;
    int longs = 30;
    for (int i = 0; i <= lats; ++i) {
        float lat0 = M_PI * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; ++j) {
            float lng = 2.0f * M_PI * (float)j / longs;
            float xVal = cos(lng);
            float yVal = sin(lng);

            // Compute latitudinal gas band color
            float t = (sin(lat1) + 1.0f) * 0.5f;
            float band = 0.5f + 0.5f * sin(t * 18.0f + cos(t * 7.0f) * 2.5f);

            float rColor, gColor, bColor;
            if (t < 0.15f || t > 0.85f) {
                // Beige polar caps
                rColor = 0.82f - 0.1f * band;
                gColor = 0.76f - 0.1f * band;
                bColor = 0.68f - 0.05f * band;
            } else if (t > 0.4f && t < 0.6f) {
                // Reddish band
                rColor = 0.85f - 0.1f * band;
                gColor = 0.55f - 0.15f * band;
                bColor = 0.42f - 0.1f * band;
            } else {
                // Orange-brown bands
                rColor = 0.78f + 0.1f * band;
                gColor = 0.62f + 0.08f * band;
                bColor = 0.48f + 0.05f * band;
            }

            glColor3f(rColor, gColor, bColor);
            glNormal3f(xVal * r0, yVal * r0, z0);
            glVertex3f(xVal * r0 * radius, yVal * r0 * radius, z0 * radius);

            glNormal3f(xVal * r1, yVal * r1, z1);
            glVertex3f(xVal * r1 * radius, yVal * r1 * radius, z1 * radius);
        }
        glEnd();
    }
    glPopMatrix();
    glPopAttrib();
}

// Static helper to draw Saturn-like semi-translucent rings
static void drawPlanetRings(float x, float y, float z, float minRadius, float maxRadius) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(20.0f, 1.0f, 0.0f, 1.0f); // Match planet tilt
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int steps = 65;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; ++i) {
        float angle = 2.0f * M_PI * (float)i / steps;
        float cosA = cos(angle);
        float sinA = sin(angle);

        // Procedural rings transparency bands
        float alpha = 0.28f + 0.22f * sin((float)i * 0.45f + 1.2f);
        glColor4f(0.85f, 0.80f, 0.72f, alpha);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(cosA * minRadius, 0.0f, sinA * minRadius);
        glVertex3f(cosA * maxRadius, 0.0f, sinA * maxRadius);
    }
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

// Static helper to draw procedurally deformed low-poly asteroids
static void drawProceduralAsteroid(float x, float y, float z, float size, int seed) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);

    // Time-based slow rotation
    float timeVal = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float rotAngle = seed * 15.0f + timeVal * (8.0f + (seed % 5) * 4.0f);
    glRotatef(rotAngle, 1.0f, 0.5f, 0.2f);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    float colorScale = 0.35f + 0.15f * ((seed % 7) / 7.0f);
    glColor3f(colorScale * 1.1f, colorScale * 0.95f, colorScale * 0.8f); // Rocky grey-brown

    int lats = 8;
    int longs = 8;
    for (int i = 0; i <= lats; ++i) {
        float lat0 = M_PI * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; ++j) {
            float lng = 2.0f * M_PI * (float)j / longs;
            float xVal = cos(lng);
            float yVal = sin(lng);

            // Bumpy radius displacement using sine/cosine harmonics
            float pRadius0 = size * (1.0f + 0.18f * sin(lat0 * 5.0f + lng * 3.0f + seed) + 0.08f * cos(lng * 6.0f - seed * 2.3f));
            float pRadius1 = size * (1.0f + 0.18f * sin(lat1 * 5.0f + lng * 3.0f + seed) + 0.08f * cos(lng * 6.0f - seed * 2.3f));

            glNormal3f(xVal * r0, yVal * r0, z0);
            glVertex3f(xVal * r0 * pRadius0, yVal * r0 * pRadius0, z0 * pRadius0);

            glNormal3f(xVal * r1, yVal * r1, z1);
            glVertex3f(xVal * r1 * pRadius1, yVal * r1 * pRadius1, z1 * pRadius1);
        }
        glEnd();
    }
    glPopMatrix();
    glPopAttrib();
}

// Static helper to draw procedural planets (Earth, Mars, Neptune)
static void drawProceduralPlanet(float x, float y, float z, float radius, int type) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);
    
    float timeVal = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    if (type == 1) glRotatef(timeVal * 2.0f, 0.1f, 1.0f, 0.0f);      // Earth rotation
    else if (type == 2) glRotatef(timeVal * 1.5f, 0.2f, 0.9f, 0.1f); // Mars rotation
    else if (type == 3) glRotatef(timeVal * 1.0f, 0.0f, 1.0f, 0.0f); // Neptune rotation
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    int lats = 24;
    int longs = 24;
    for (int i = 0; i <= lats; ++i) {
        float lat0 = M_PI * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; ++j) {
            float lng = 2.0f * M_PI * (float)j / longs;
            float xVal = cos(lng);
            float yVal = sin(lng);

            float rColor = 1.0f, gColor = 1.0f, bColor = 1.0f;
            
            if (type == 1) { // Earth
                float land = sin(lat1 * 4.0f) * cos(lng * 3.0f) + sin(lng * 5.0f) * cos(lat1 * 2.0f);
                if (land > 0.2f) {
                    rColor = 0.25f; gColor = 0.65f; bColor = 0.3f; // Green land
                } else if (land > 0.0f) {
                    rColor = 0.55f; gColor = 0.6f; bColor = 0.38f; // Sandy shores
                } else {
                    rColor = 0.1f; gColor = 0.35f; bColor = 0.75f; // Blue oceans
                }
                // Clouds
                float clouds = sin(lat1 * 7.0f + lng * 5.0f + timeVal * 0.12f);
                if (clouds > 0.45f) {
                    rColor = 0.95f; gColor = 0.95f; bColor = 0.95f; // White cloud patches
                }
            } else if (type == 2) { // Mars
                float craters = sin(lat1 * 6.0f) * sin(lng * 6.0f);
                rColor = 0.72f + 0.08f * craters;
                gColor = 0.30f + 0.04f * craters;
                bColor = 0.20f + 0.04f * craters;
                if (std::abs(lat1) > 1.35f) {
                    rColor = 0.95f; gColor = 0.95f; bColor = 0.95f; // Polar caps
                }
            } else if (type == 3) { // Neptune
                float band = sin(lat1 * 10.0f);
                rColor = 0.1f + 0.03f * band;
                gColor = 0.4f + 0.08f * band;
                bColor = 0.8f + 0.12f * band;
            }

            glColor3f(rColor, gColor, bColor);
            glNormal3f(xVal * r0, yVal * r0, z0);
            glVertex3f(xVal * r0 * radius, yVal * r0 * radius, z0 * radius);

            glNormal3f(xVal * r1, yVal * r1, z1);
            glVertex3f(xVal * r1 * radius, yVal * r1 * radius, z1 * radius);
        }
        glEnd();
    }
    glPopMatrix();
    glPopAttrib();
}

// Static helper to draw a closed blind box package in the background
static void drawClosedBlindBox(float x, float y, float z, float w, float h, float d, GLuint tex, float r, float g, float b) {
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(r, g, b);
    
    float hw = w / 2.0f;
    float hd = d / 2.0f;
    
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-hw, 0.0f,  hd);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( hw, 0.0f,  hd);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( hw,  h,   hd);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-hw,  h,   hd);
    
    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( hw, 0.0f, -hd);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-hw, 0.0f, -hd);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-hw,  h,  -hd);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( hw,  h,  -hd);
    
    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-hw, 0.0f, -hd);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-hw, 0.0f,  hd);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-hw,  h,   hd);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-hw,  h,  -hd);
    
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( hw, 0.0f,  hd);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( hw, 0.0f, -hd);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( hw,  h,  -hd);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( hw,  h,   hd);
    
    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-hw,  h,   hd);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( hw,  h,   hd);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( hw,  h,  -hd);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-hw,  h,  -hd);
    glEnd();
    
    glPopMatrix();
    glPopAttrib();
}

static void drawBackWallSegment(float xStart, float xEnd, float yStart, float yEnd, float halfW, float height, float halfD) {
    if (xStart >= xEnd || yStart >= yEnd) return;
    float u1 = 1.0f - (xStart + halfW) / (2.0f * halfW);
    float u2 = 1.0f - (xEnd + halfW) / (2.0f * halfW);
    float v1 = 1.0f - yStart / height;
    float v2 = 1.0f - yEnd / height;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(u1, v1); glVertex3f(xStart, yStart, -halfD);
    glTexCoord2f(u2, v1); glVertex3f(xEnd,   yStart, -halfD);
    glTexCoord2f(u2, v2); glVertex3f(xEnd,   yEnd,   -halfD);
    glTexCoord2f(u1, v2); glVertex3f(xStart, yEnd,   -halfD);
    glEnd();
}

static void drawLeftWallSegment(float zStart, float zEnd, float yStart, float yEnd, float halfW, float height, float halfD) {
    // Note: zStart corresponds to larger Z, zEnd corresponds to smaller Z
    if (zStart <= zEnd || yStart >= yEnd) return;
    float u1 = (halfD - zStart) / (2.0f * halfD);
    float u2 = (halfD - zEnd) / (2.0f * halfD);
    float v1 = yStart / height;
    float v2 = yEnd / height;

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(u1, v1); glVertex3f(-halfW, yStart, zStart);
    glTexCoord2f(u2, v1); glVertex3f(-halfW, yStart, zEnd);
    glTexCoord2f(u2, v2); glVertex3f(-halfW, yEnd,   zEnd);
    glTexCoord2f(u1, v2); glVertex3f(-halfW, yEnd,   zStart);
    glEnd();
}

static void drawBackWallTornBorder(float x1, float x2, float y1, float y2, float z) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.18f, 0.14f, 0.10f); // Dark torn cardboard edge color
    glBegin(GL_QUADS);
    
    // Top border quad
    glVertex3f(x1 - 0.04f, y2, z + 0.005f);
    glVertex3f(x2 + 0.04f, y2, z + 0.005f);
    glVertex3f(x2, y2 - 0.04f, z + 0.005f);
    glVertex3f(x1, y2 - 0.05f, z + 0.005f);
    
    // Bottom border quad
    glVertex3f(x1 - 0.04f, y1, z + 0.005f);
    glVertex3f(x2 + 0.04f, y1, z + 0.005f);
    glVertex3f(x2, y1 + 0.04f, z + 0.005f);
    glVertex3f(x1, y1 + 0.05f, z + 0.005f);

    // Left border quad
    glVertex3f(x1, y1 - 0.04f, z + 0.005f);
    glVertex3f(x1, y2 + 0.04f, z + 0.005f);
    glVertex3f(x1 + 0.04f, y2, z + 0.005f);
    glVertex3f(x1 + 0.05f, y1, z + 0.005f);

    // Right border quad
    glVertex3f(x2, y1 - 0.04f, z + 0.005f);
    glVertex3f(x2, y2 + 0.04f, z + 0.005f);
    glVertex3f(x2 - 0.04f, y2, z + 0.005f);
    glVertex3f(x2 - 0.05f, y1, z + 0.005f);
    
    glEnd();
    
    // Add some random jagged teeth triangles
    glBegin(GL_TRIANGLES);
    glVertex3f(x1 + 0.2f, y1 + 0.05f, z + 0.006f);
    glVertex3f(x1 + 0.3f, y1 + 0.12f, z + 0.006f);
    glVertex3f(x1 + 0.4f, y1 + 0.03f, z + 0.006f);

    glVertex3f(x2 - 0.3f, y2 - 0.04f, z + 0.006f);
    glVertex3f(x2 - 0.4f, y2 - 0.12f, z + 0.006f);
    glVertex3f(x2 - 0.5f, y2 - 0.03f, z + 0.006f);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
}

static void drawLeftWallTornBorder(float z1, float z2, float y1, float y2, float x) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.18f, 0.14f, 0.10f);
    glBegin(GL_QUADS);
    
    // Top border quad
    glVertex3f(x + 0.005f, y2, z1 + 0.04f);
    glVertex3f(x + 0.005f, y2, z2 - 0.04f);
    glVertex3f(x + 0.005f, y2 - 0.04f, z2);
    glVertex3f(x + 0.005f, y2 - 0.05f, z1);
    
    // Bottom border quad
    glVertex3f(x + 0.005f, y1, z1 + 0.04f);
    glVertex3f(x + 0.005f, y1, z2 - 0.04f);
    glVertex3f(x + 0.005f, y1 + 0.04f, z2);
    glVertex3f(x + 0.005f, y1 + 0.05f, z1);

    // Left border (Z positive)
    glVertex3f(x + 0.005f, y1 - 0.04f, z1);
    glVertex3f(x + 0.005f, y2 + 0.04f, z1);
    glVertex3f(x + 0.005f, y2, z1 - 0.04f);
    glVertex3f(x + 0.005f, y1, z1 - 0.05f);

    // Right border (Z negative)
    glVertex3f(x + 0.005f, y1 - 0.04f, z2);
    glVertex3f(x + 0.005f, y2 + 0.04f, z2);
    glVertex3f(x + 0.005f, y2, z2 + 0.04f);
    glVertex3f(x + 0.005f, y1, z2 + 0.05f);
    
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

static void drawFloorAtmosphere(float halfW, float halfD, float damage) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float y = 0.004f;
    float edgeA = 0.22f + damage * 0.12f;

    glBegin(GL_QUADS);
    // Back wall contact shadow.
    glColor4f(0.02f, 0.018f, 0.014f, edgeA);
    glVertex3f(-halfW, y, -halfD);
    glVertex3f( halfW, y, -halfD);
    glColor4f(0.02f, 0.018f, 0.014f, 0.0f);
    glVertex3f( halfW, y, -halfD + 0.65f);
    glVertex3f(-halfW, y, -halfD + 0.65f);

    // Side wall contact shadows.
    glColor4f(0.02f, 0.018f, 0.014f, edgeA * 0.72f);
    glVertex3f(-halfW, y, -halfD);
    glVertex3f(-halfW, y,  halfD);
    glColor4f(0.02f, 0.018f, 0.014f, 0.0f);
    glVertex3f(-halfW + 0.55f, y,  halfD);
    glVertex3f(-halfW + 0.55f, y, -halfD);

    glColor4f(0.02f, 0.018f, 0.014f, edgeA * 0.72f);
    glVertex3f(halfW, y,  halfD);
    glVertex3f(halfW, y, -halfD);
    glColor4f(0.02f, 0.018f, 0.014f, 0.0f);
    glVertex3f(halfW - 0.55f, y, -halfD);
    glVertex3f(halfW - 0.55f, y,  halfD);
    glEnd();

    // Soft tape strip and box fold guides.
    glColor4f(0.88f, 0.82f, 0.68f, 0.16f);
    glBegin(GL_QUADS);
    glVertex3f(-0.13f, y + 0.001f,  halfD - 0.25f);
    glVertex3f( 0.13f, y + 0.001f,  halfD - 0.25f);
    glVertex3f( 0.13f, y + 0.001f, -halfD + 0.25f);
    glVertex3f(-0.13f, y + 0.001f, -halfD + 0.25f);
    glEnd();

    glLineWidth(1.0f);
    glColor4f(0.18f, 0.16f, 0.13f, 0.20f);
    glBegin(GL_LINES);
    for (int i = 0; i < 24; ++i) {
        float x = -halfW + 0.45f + i * (2.0f * halfW - 0.9f) / 23.0f;
        float wobble = 0.02f * sin(i * 2.13f);
        glVertex3f(x, y + 0.002f, -halfD + 0.18f + wobble);
        glVertex3f(x + 0.12f * sin(i * 1.7f), y + 0.002f, halfD - 0.18f - wobble);
    }
    glEnd();

    glPopAttrib();
}

static void drawHoleCoreAndScorch(const BoxHole& hole) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto emitHoleVertex = [&](float dx, float dy, float inset) {
        if (hole.wall == 'B') {
            glVertex3f(hole.cx + dx, hole.cy + dy, hole.cz + inset);
        } else if (hole.wall == 'L') {
            glVertex3f(hole.cx + inset, hole.cy + dy, hole.cz + dx);
        } else {
            glVertex3f(hole.cx - inset, hole.cy + dy, hole.cz + dx);
        }
    };

    int segments = 28;
    float seedVal = (float)hole.seed;

    // Deep cosmic void visible through the punched cardboard.
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.002f, 0.004f, 0.014f, 0.96f);
    emitHoleVertex(0.0f, 0.0f, 0.010f);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * i / (float)segments;
        float noise = 1.0f + 0.16f * sin(5.0f * theta + seedVal) + 0.08f * cos(11.0f * theta - seedVal);
        float r = hole.radius * noise;
        glColor4f(0.006f, 0.010f, 0.030f, 0.92f);
        emitHoleVertex(r * cos(theta), r * sin(theta), 0.010f);
    }
    glEnd();

    // Scorched compressed paper ring around the torn edge.
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * i / (float)segments;
        float noise = 1.0f + 0.13f * sin(5.0f * theta + seedVal) + 0.07f * cos(9.0f * theta - seedVal);
        float inner = hole.radius * noise;
        float outer = hole.radius * (1.38f + 0.10f * sin(7.0f * theta + seedVal));

        glColor4f(0.05f, 0.035f, 0.020f, 0.62f);
        emitHoleVertex(inner * cos(theta), inner * sin(theta), 0.014f);
        glColor4f(0.07f, 0.050f, 0.030f, 0.0f);
        emitHoleVertex(outer * cos(theta), outer * sin(theta), 0.014f);
    }
    glEnd();

    glPopAttrib();
}

static void drawPlasticRimLine(float x1, float y1, float z1,
                               float x2, float y2, float z2,
                               float width, float nx, float ny, float nz,
                               float alpha) {
    glColor4f(0.92f, 0.98f, 1.0f, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x1 - nx * width, y1 - ny * width, z1 - nz * width);
    glVertex3f(x2 - nx * width, y2 - ny * width, z2 - nz * width);
    glVertex3f(x2 + nx * width, y2 + ny * width, z2 + nz * width);
    glVertex3f(x1 + nx * width, y1 + ny * width, z1 + nz * width);
    glEnd();
}

void Arena::drawBox() {
    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;
    float damage = clamp(1.0f - minHpPct, 0.0f, 1.0f);
    float lightFactor = 0.34f + 0.28f * damage;

    // Safe state check to avoid logic pollution
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);

    // --- A. COSMIC ENVIRONMENT (STARFIELD, PLANETS, ASTEROID BELT & FLOATING TABLE) ---

    // 1. Starry Sky Sphere Background (Depth Mask disabled, lighting disabled)
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    if (starrySkyTex > 0) {
        glBindTexture(GL_TEXTURE_2D, starrySkyTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    glColor3f(0.38f + 0.10f * damage, 0.42f + 0.10f * damage, 0.55f + 0.12f * damage);
    GLUquadric* starryQuad = gluNewQuadric();
    gluQuadricTexture(starryQuad, GL_TRUE);
    gluQuadricOrientation(starryQuad, GLU_INSIDE);
    gluSphere(starryQuad, 60.0f, 32, 32);
    gluDeleteQuadric(starryQuad);
    glPopAttrib();

    // 2. Solar System Planets (Jupiter, Earth, Mars, Neptune) in background
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
    glEnable(GL_LIGHTING);
    glDepthMask(GL_FALSE); // Distant background planets

    // Jupiter/Saturn-like gas giant with rings
    drawProceduralGasGiant(-24.0f, 15.0f, -35.0f, 6.5f);
    drawPlanetRings(-24.0f, 15.0f, -35.0f, 8.5f, 15.0f);

    // Earth
    drawProceduralPlanet(22.0f, 16.0f, -38.0f, 3.2f, 1);
    // Mars
    drawProceduralPlanet(-12.0f, -14.0f, -28.0f, 2.2f, 2);
    // Neptune
    drawProceduralPlanet(30.0f, -8.0f, -48.0f, 4.2f, 3);

    glPopAttrib();

    // 3. Stateful Space Objects (Asteroids and Floating Boxes)
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE); // Enable depth buffer writing for space objects

    for (const auto& obj : spaceObjects) {
        if (obj.type == 0) {
            // Draw Asteroid
            drawProceduralAsteroid(obj.x, obj.y, obj.z, obj.size, obj.seed);
        } else {
            // Draw Floating Blind Box (Hirono/Dimoo/Cardboard/Rose)
            GLuint tex = cardboardTex;
            float r = 0.8f, g = 0.8f, b = 0.8f;
            if (obj.type == 1) { tex = hironoFaceTex; r = 0.5f; g = 0.75f; b = 0.55f; }
            else if (obj.type == 2) { tex = dimooFaceTex; r = 0.5f; g = 0.65f; b = 0.85f; }
            else if (obj.type == 3) { tex = cardboardTex; r = 0.76f; g = 0.60f; b = 0.42f; }
            else if (obj.type == 4) { tex = roseLabelTex; r = 0.85f; g = 0.75f; b = 0.45f; }

            glPushMatrix();
            glTranslatef(obj.x, obj.y, obj.z);
            glRotatef(obj.rx, 1.0f, 0.0f, 0.0f);
            glRotatef(obj.ry, 0.0f, 1.0f, 0.0f);
            glRotatef(obj.rz, 0.0f, 0.0f, 1.0f);
            
            // Render closed box scaled by size (centered rotation by translating Y down by h/2)
            float w = 2.5f * obj.size;
            float h = 3.5f * obj.size;
            float d = 2.5f * obj.size;
            drawClosedBlindBox(0.0f, -h / 2.0f, 0.0f, w, h, d, tex, r, g, b);
            glPopMatrix();
        }
    }
    glPopAttrib();

    // 5. Floating Thick Wooden Table Slab (Replaces flat infinite table)
    float tHalfW = halfW + 4.0f;
    float tHalfD = halfD + 3.0f;
    float tThick = 0.35f;

    glEnable(GL_TEXTURE_2D);
    if (woodTex > 0) {
        glBindTexture(GL_TEXTURE_2D, woodTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    glColor3f(0.36f * lightFactor, 0.29f * lightFactor, 0.22f * lightFactor);

    glBegin(GL_QUADS);
    // Top Face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-tHalfW, -0.01f,  tHalfD);
    glTexCoord2f(10.0f, 0.0f);  glVertex3f( tHalfW, -0.01f,  tHalfD);
    glTexCoord2f(10.0f, 10.0f); glVertex3f( tHalfW, -0.01f, -tHalfD);
    glTexCoord2f(0.0f, 10.0f);  glVertex3f(-tHalfW, -0.01f, -tHalfD);

    // Bottom Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-tHalfW, -tThick, -tHalfD);
    glTexCoord2f(10.0f, 0.0f);  glVertex3f( tHalfW, -tThick, -tHalfD);
    glTexCoord2f(10.0f, 10.0f); glVertex3f( tHalfW, -tThick,  tHalfD);
    glTexCoord2f(0.0f, 10.0f);  glVertex3f(-tHalfW, -tThick,  tHalfD);

    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tHalfW, -tThick,  tHalfD);
    glTexCoord2f(5.0f, 0.0f); glVertex3f( tHalfW, -tThick,  tHalfD);
    glTexCoord2f(5.0f, 1.0f); glVertex3f( tHalfW, -0.01f,   tHalfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tHalfW, -0.01f,   tHalfD);

    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( tHalfW, -tThick, -tHalfD);
    glTexCoord2f(5.0f, 0.0f); glVertex3f(-tHalfW, -tThick, -tHalfD);
    glTexCoord2f(5.0f, 1.0f); glVertex3f(-tHalfW, -0.01f,  -tHalfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( tHalfW, -0.01f,  -tHalfD);

    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tHalfW, -tThick, -tHalfD);
    glTexCoord2f(5.0f, 0.0f); glVertex3f(-tHalfW, -tThick,  tHalfD);
    glTexCoord2f(5.0f, 1.0f); glVertex3f(-tHalfW, -0.01f,   tHalfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tHalfW, -0.01f,  -tHalfD);

    // Right Face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( tHalfW, -tThick,  tHalfD);
    glTexCoord2f(5.0f, 0.0f); glVertex3f( tHalfW, -tThick, -tHalfD);
    glTexCoord2f(5.0f, 1.0f); glVertex3f( tHalfW, -0.01f,  -tHalfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( tHalfW, -0.01f,   tHalfD);
    glEnd();


    // --- B. CARDBOARD BOX GEOMETRY ---

    // 1. Box Thickness / Outer faces (facing away from the box center)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);
    glColor3f(0.70f * lightFactor, 0.56f * lightFactor, 0.39f * lightFactor); // Modulate with lightFactor
    float wallThick = 0.22f;

    // Left Wall Outer Face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW - wallThick, 0.0f,  -halfD - wallThick);
    glTexCoord2f(6.0f, 0.0f); glVertex3f(-halfW - wallThick, 0.0f,   halfD);
    glTexCoord2f(6.0f, 5.0f); glVertex3f(-halfW - wallThick, height,  halfD);
    glTexCoord2f(0.0f, 5.0f); glVertex3f(-halfW - wallThick, height, -halfD - wallThick);
    glEnd();

    // Right Wall Outer Face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfW + wallThick, 0.0f,   halfD);
    glTexCoord2f(6.0f, 0.0f); glVertex3f(halfW + wallThick, 0.0f,  -halfD - wallThick);
    glTexCoord2f(6.0f, 5.0f); glVertex3f(halfW + wallThick, height, -halfD - wallThick);
    glTexCoord2f(0.0f, 5.0f); glVertex3f(halfW + wallThick, height,  halfD);
    glEnd();

    // Back Wall Outer Face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);  glVertex3f( halfW + wallThick, 0.0f,  -halfD - wallThick);
    glTexCoord2f(18.0f, 0.0f); glVertex3f(-halfW - wallThick, 0.0f,  -halfD - wallThick);
    glTexCoord2f(18.0f, 5.0f); glVertex3f(-halfW - wallThick, height, -halfD - wallThick);
    glTexCoord2f(0.0f, 5.0f);  glVertex3f( halfW + wallThick, height, -halfD - wallThick);
    glEnd();

    // --- Cardboard Top Rims (Horizontal quads showing cardboard thickness) ---
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.92f * lightFactor, 0.84f * lightFactor, 0.70f * lightFactor); // Raw cardboard cut edge color

    glBegin(GL_QUADS);
    // Left Wall Top Rim
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfW - wallThick, height, -halfD - wallThick);
    glVertex3f(-halfW - wallThick, height,  halfD);
    glVertex3f(-halfW,             height,  halfD);
    glVertex3f(-halfW,             height, -halfD - wallThick);

    // Right Wall Top Rim
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(halfW,             height, -halfD - wallThick);
    glVertex3f(halfW,             height,  halfD);
    glVertex3f(halfW + wallThick, height,  halfD);
    glVertex3f(halfW + wallThick, height, -halfD - wallThick);

    // Back Wall Top Rim
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfW, height, -halfD - wallThick);
    glVertex3f(-halfW, height, -halfD);
    glVertex3f( halfW, height, -halfD);
    glVertex3f( halfW, height, -halfD - wallThick);
    glEnd();

    glEnable(GL_TEXTURE_2D);


    // 2. Box Inner faces


    // --- Floor (Floor.jpg) ---
    if (floorTex > 0) {
        glBindTexture(GL_TEXTURE_2D, floorTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    GLfloat mat_floor_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Matte surface
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_floor_specular);
    glColor3f(0.78f * lightFactor, 0.72f * lightFactor, 0.62f * lightFactor); // Warm cardboard color tint modulation
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0.0f,  halfD);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( halfW, 0.0f,  halfD);
    glTexCoord2f(2.0f, 1.0f); glVertex3f( halfW, 0.0f, -halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, 0.0f, -halfD);
    glEnd();

    // Crease lines and Watermark (prevent Z-fighting by raising Y slightly)
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    drawFloorAtmosphere(halfW, halfD, damage);

    // Light-grey watermark circle
    glColor4f(0.42f, 0.40f, 0.36f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, 0.002f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 40; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / 40.0f;
        glVertex2f(0.8f * cos(theta), 0.8f * sin(theta));
    }
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();

    // Floor fold crease lines
    glColor4f(0.22f, 0.20f, 0.17f, 0.30f);
    glBegin(GL_LINES);
    glVertex3f(-halfW, 0.002f,  halfD); glVertex3f(-halfW + 0.8f, 0.002f,  halfD - 0.8f);
    glVertex3f( halfW, 0.002f,  halfD); glVertex3f( halfW - 0.8f, 0.002f,  halfD - 0.8f);
    glVertex3f( halfW, 0.002f, -halfD); glVertex3f( halfW - 0.8f, 0.002f, -halfD + 0.8f);
    glVertex3f(-halfW, 0.002f, -halfD); glVertex3f(-halfW + 0.8f, 0.002f, -halfD + 0.8f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Lambda/Helper to check if a coordinate is inside any active hole with radial noise
    auto isInsideHole = [&](float px, float py, float pz, char wallType) -> bool {
        for (const auto& hole : holes) {
            if (!hole.active || hole.wall != wallType) continue;
            float dx = 0.0f, dy = 0.0f;
            if (wallType == 'B') {
                dx = px - hole.cx;
                dy = py - hole.cy;
            } else if (wallType == 'L' || wallType == 'R') {
                dx = pz - hole.cz;
                dy = py - hole.cy;
            }
            
            // Quick bounding box check to optimize rendering performance
            float maxR = hole.radius * 1.35f;
            if (std::abs(dx) > maxR || std::abs(dy) > maxR) continue;

            float dist = sqrt(dx*dx + dy*dy);
            if (dist < 0.001f) return true;
            
            float theta = atan2(dy, dx);
            float seedVal = (float)hole.seed;
            // High-frequency radial noise to make it jagged, hand-torn cardboard
            float rPerturbed = hole.radius * (1.0f + 0.12f * sin(4.5f * theta + seedVal) + 0.08f * cos(8.0f * theta - seedVal * 1.3f));
            if (dist < rPerturbed) {
                return true;
            }
        }
        return false;
    };

    // --- Left Wall Inner Face (LeftRight.jpg) ---
    glPushMatrix();
    glTranslatef(leftWallOffset, 0.0f, 0.0f);
    if (leftRightTex > 0) {
        glBindTexture(GL_TEXTURE_2D, leftRightTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    glColor3f(0.72f * lightFactor, 0.70f * lightFactor, 0.64f * lightFactor);
    {
        int gridZ = 100;
        int gridY = 100;
        float dz = BOX_DEPTH / (float)gridZ;
        float dy = height / (float)gridY;
        for (int r = 0; r < gridY; ++r) {
            float y1 = r * dy;
            float y2 = y1 + dy;
            for (int c = 0; c < gridZ; ++c) {
                float z1 = halfD - c * dz;
                float z2 = z1 - dz;
                
                // Check cell center
                float yMid = (y1 + y2) / 2.0f;
                float zMid = (z1 + z2) / 2.0f;
                if (isInsideHole(-halfW, yMid, zMid, 'L')) continue; // Skip hole

                float u1 = (halfD - z1) / (2.0f * halfD);
                float u2 = (halfD - z2) / (2.0f * halfD);
                float v1 = y1 / height;
                float v2 = y2 / height;

                glBegin(GL_QUADS);
                glNormal3f(1.0f, 0.0f, 0.0f);
                glTexCoord2f(u1, v1); glVertex3f(-halfW, y1, z1);
                glTexCoord2f(u2, v1); glVertex3f(-halfW, y1, z2);
                glTexCoord2f(u2, v2); glVertex3f(-halfW, y2,   z2);
                glTexCoord2f(u1, v2); glVertex3f(-halfW, y2,   z1);
                glEnd();
            }
        }
    }
    glPopMatrix();

    // --- Right Wall Inner Face (LeftRight.jpg) ---
    glPushMatrix();
    glTranslatef(rightWallOffset, 0.0f, 0.0f);
    if (leftRightTex > 0) {
        glBindTexture(GL_TEXTURE_2D, leftRightTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    glColor3f(0.72f * lightFactor, 0.70f * lightFactor, 0.64f * lightFactor);
    {
        int gridZ = 100;
        int gridY = 100;
        float dz = BOX_DEPTH / (float)gridZ;
        float dy = height / (float)gridY;
        for (int r = 0; r < gridY; ++r) {
            float y1 = r * dy;
            float y2 = y1 + dy;
            for (int c = 0; c < gridZ; ++c) {
                float z1 = halfD - c * dz;
                float z2 = z1 - dz;
                
                float yMid = (y1 + y2) / 2.0f;
                float zMid = (z1 + z2) / 2.0f;
                if (isInsideHole(halfW, yMid, zMid, 'R')) continue;

                float u1 = (z1 + halfD) / (2.0f * halfD);
                float u2 = (z2 + halfD) / (2.0f * halfD);
                float v1 = y1 / height;
                float v2 = y2 / height;

                glBegin(GL_QUADS);
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glTexCoord2f(u2, v1); glVertex3f(halfW, y1, z2);
                glTexCoord2f(u1, v1); glVertex3f(halfW, y1, z1);
                glTexCoord2f(u1, v2); glVertex3f(halfW, y2,   z1);
                glTexCoord2f(u2, v2); glVertex3f(halfW, y2,   z2);
                glEnd();
            }
        }
    }
    glPopMatrix();

    // --- Back Wall Inner Face (Background.jpg with 180deg Flip, Cardboard Mural Tint) ---
    if (backTex > 0) {
        glBindTexture(GL_TEXTURE_2D, backTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardboardTex);
    }
    glColor3f(0.42f * lightFactor, 0.40f * lightFactor, 0.36f * lightFactor);
    {
        int gridX = 200;
        int gridY = 60;
        float dx = BOX_WIDTH / (float)gridX;
        float dy = height / (float)gridY;
        for (int r = 0; r < gridY; ++r) {
            float y1 = r * dy;
            float y2 = y1 + dy;
            for (int c = 0; c < gridX; ++c) {
                float x1 = -halfW + c * dx;
                float x2 = x1 + dx;

                float xMid = (x1 + x2) / 2.0f;
                float yMid = (y1 + y2) / 2.0f;
                if (isInsideHole(xMid, yMid, -halfD, 'B')) continue;

                float u1 = 1.0f - (x1 + halfW) / (2.0f * halfW);
                float u2 = 1.0f - (x2 + halfW) / (2.0f * halfW);
                float v1 = 1.0f - y1 / height;
                float v2 = 1.0f - y2 / height;

                glBegin(GL_QUADS);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glTexCoord2f(u1, v1); glVertex3f(x1, y1, -halfD);
                glTexCoord2f(u2, v1); glVertex3f(x2, y1, -halfD);
                glTexCoord2f(u2, v2); glVertex3f(x2, y2, -halfD);
                glTexCoord2f(u1, v2); glVertex3f(x1, y2, -halfD);
                glEnd();
            }
        }
    }

    // --- Dark cosmic core and compressed scorch edge behind active tears ---
    for (const auto& hole : holes) {
        if (hole.active) {
            drawHoleCoreAndScorch(hole);
        }
    }

    // --- Render 3D Torn Cardboard Flaps around active holes ---
    for (const auto& hole : holes) {
        if (hole.active) {
            draw3DTornFlaps(hole);
        }
    }

    // --- 3. FRONT WALL ---
    // Completely omitted according to optimization requirements.

    // --- 4. FLAPS (Folding Outward) ---
    // Completely removed according to user request.

    glPopAttrib();
}

void Arena::drawLid() {
    // Box lid completely removed according to user request.
}

static void drawFoldedInstructionLeaflet(float foldAngle) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    float open = clamp(foldAngle, 0.0f, 1.0f);
    float wing = 0.20f + open * 0.13f;
    float tilt = 0.04f + open * 0.08f;

    // Three connected off-white panels.
    glColor3f(0.88f, 0.86f, 0.78f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.18f, 0.0f, -0.15f);
    glVertex3f( 0.18f, 0.0f, -0.15f);
    glVertex3f( 0.18f, 0.0f,  0.15f);
    glVertex3f(-0.18f, 0.0f,  0.15f);

    glVertex3f(-0.18f, 0.0f, -0.15f);
    glVertex3f(-0.18f - wing, tilt, -0.13f);
    glVertex3f(-0.18f - wing, tilt,  0.13f);
    glVertex3f(-0.18f, 0.0f,  0.15f);

    glVertex3f( 0.18f, 0.0f, -0.15f);
    glVertex3f( 0.18f + wing, tilt, -0.13f);
    glVertex3f( 0.18f + wing, tilt,  0.13f);
    glVertex3f( 0.18f, 0.0f,  0.15f);
    glEnd();

    // Printed folds, tiny diagram blocks, and text lines.
    glColor3f(0.18f, 0.18f, 0.16f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.18f, 0.004f, -0.15f); glVertex3f(-0.18f, 0.004f, 0.15f);
    glVertex3f( 0.18f, 0.004f, -0.15f); glVertex3f( 0.18f, 0.004f, 0.15f);
    for (int i = 0; i < 5; ++i) {
        float z = -0.10f + i * 0.045f;
        glVertex3f(-0.10f, 0.006f, z); glVertex3f(0.10f, 0.006f, z);
        glVertex3f(-0.38f, 0.006f, z); glVertex3f(-0.24f, 0.006f, z);
        glVertex3f( 0.24f, 0.006f, z); glVertex3f( 0.40f, 0.006f, z);
    }
    glEnd();

    glColor3f(0.42f, 0.42f, 0.38f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.075f, 0.007f, 0.045f);
    glVertex3f( 0.075f, 0.007f, 0.045f);
    glVertex3f( 0.075f, 0.007f, 0.125f);
    glVertex3f(-0.075f, 0.007f, 0.125f);
    glEnd();

    glPopAttrib();
}

static void drawDesiccantPacketModel(bool splitOpen) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    float tear = splitOpen ? 0.05f : 0.0f;

    glColor3f(0.78f, 0.84f, 0.88f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.21f - tear, 0.0f, -0.14f);
    glVertex3f( 0.21f + tear, 0.0f, -0.12f);
    glVertex3f( 0.18f + tear, 0.0f,  0.15f);
    glVertex3f(-0.20f - tear, 0.0f,  0.13f);
    glEnd();

    // Crimped packet edges.
    glColor3f(0.46f, 0.54f, 0.58f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.21f - tear, 0.006f, -0.14f);
    glVertex3f( 0.21f + tear, 0.006f, -0.12f);
    glVertex3f( 0.18f + tear, 0.006f,  0.15f);
    glVertex3f(-0.20f - tear, 0.006f,  0.13f);
    glEnd();

    // Blue label and small "text" stripes.
    glColor3f(0.24f, 0.42f, 0.62f);
    glBegin(GL_QUADS);
    glVertex3f(-0.13f, 0.008f, -0.055f);
    glVertex3f( 0.13f, 0.008f, -0.045f);
    glVertex3f( 0.12f, 0.008f,  0.045f);
    glVertex3f(-0.13f, 0.008f,  0.040f);
    glEnd();

    glColor3f(0.92f, 0.95f, 0.96f);
    glBegin(GL_LINES);
    for (int i = 0; i < 3; ++i) {
        float z = -0.025f + i * 0.026f;
        glVertex3f(-0.09f, 0.010f, z);
        glVertex3f( 0.09f, 0.010f, z + 0.004f);
    }
    glEnd();

    if (splitOpen) {
        glColor3f(0.86f, 0.88f, 0.82f);
        for (int i = 0; i < 7; ++i) {
            glPushMatrix();
            glTranslatef(0.03f + 0.035f * i, 0.012f, 0.18f + 0.015f * sin(i * 1.7f));
            glutSolidSphere(0.014f, 6, 6);
            glPopMatrix();
        }
    }

    glPopAttrib();
}

static void drawReplacementPartsBagModel() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Soft shadow under the clear bag.
    glColor4f(0.02f, 0.02f, 0.018f, 0.16f);
    glBegin(GL_QUADS);
    glVertex3f(-0.28f, -0.002f, -0.18f);
    glVertex3f( 0.28f, -0.002f, -0.18f);
    glVertex3f( 0.28f, -0.002f,  0.18f);
    glVertex3f(-0.28f, -0.002f,  0.18f);
    glEnd();

    // Clear plastic pouch.
    glColor4f(0.78f, 0.92f, 1.0f, 0.22f);
    glBegin(GL_QUADS);
    glVertex3f(-0.25f, 0.0f, -0.16f);
    glVertex3f( 0.25f, 0.0f, -0.14f);
    glVertex3f( 0.22f, 0.0f,  0.16f);
    glVertex3f(-0.23f, 0.0f,  0.15f);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 0.42f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.25f, 0.008f, -0.16f);
    glVertex3f( 0.25f, 0.008f, -0.14f);
    glVertex3f( 0.22f, 0.008f,  0.16f);
    glVertex3f(-0.23f, 0.008f,  0.15f);
    glEnd();

    // Replacement parts inside.
    glColor3f(0.95f, 0.32f, 0.32f);
    glPushMatrix();
    glTranslatef(-0.08f, 0.015f, 0.03f);
    glutSolidSphere(0.045f, 10, 10);
    glPopMatrix();

    glColor3f(0.92f, 0.82f, 0.55f);
    glPushMatrix();
    glTranslatef(0.08f, 0.015f, -0.03f);
    glutSolidCube(0.08f);
    glPopMatrix();

    glColor4f(1.0f, 1.0f, 1.0f, 0.34f);
    glBegin(GL_QUADS);
    glVertex3f(-0.20f, 0.012f, -0.10f);
    glVertex3f(-0.05f, 0.012f, -0.12f);
    glVertex3f( 0.20f, 0.012f,  0.12f);
    glVertex3f( 0.05f, 0.012f,  0.14f);
    glEnd();

    glPopAttrib();
}

void Arena::drawOpaque() {
    drawBox();
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glPopAttrib();
}

void Arena::drawTransparent() {
    // Semi-transparent stubs like bubble wraps (close DepthMask, blend)
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // Bubble wrap liner: a cool transparent membrane covering the box floor.
    const float wrapMinX = -8.55f;
    const float wrapMaxX =  8.55f;
    const float wrapMinZ = -2.75f;
    const float wrapMaxZ =  2.75f;
    const float wrapY = 0.018f;

    glColor4f(0.72f, 0.82f, 0.88f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(wrapMinX, wrapY, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY, wrapMinZ);
    glVertex3f(wrapMinX, wrapY, wrapMinZ);
    glEnd();

    // Subtle sealed sheet edge and rolled plastic lips.
    glLineWidth(2.0f);
    glColor4f(0.95f, 0.98f, 1.0f, 0.24f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(wrapMinX, wrapY + 0.012f, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY + 0.012f, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY + 0.012f, wrapMinZ);
    glVertex3f(wrapMinX, wrapY + 0.012f, wrapMinZ);
    glEnd();

    glColor4f(0.88f, 0.96f, 1.0f, 0.16f);
    glBegin(GL_QUADS);
    glVertex3f(wrapMinX, wrapY + 0.020f, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY + 0.020f, wrapMaxZ);
    glVertex3f(wrapMaxX, wrapY + 0.006f, wrapMaxZ - 0.10f);
    glVertex3f(wrapMinX, wrapY + 0.006f, wrapMaxZ - 0.10f);
    glVertex3f(wrapMinX, wrapY + 0.020f, wrapMinZ);
    glVertex3f(wrapMaxX, wrapY + 0.020f, wrapMinZ);
    glVertex3f(wrapMaxX, wrapY + 0.006f, wrapMinZ + 0.10f);
    glVertex3f(wrapMinX, wrapY + 0.006f, wrapMinZ + 0.10f);
    glEnd();

    // Faint pressure seams and diagonal crinkles in the plastic sheet.
    glLineWidth(1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.10f);
    glBegin(GL_LINES);
    for (int c = 1; c < 25; ++c) {
        float x = -8.10f + c * (16.20f / 24.0f);
        glVertex3f(x, wrapY + 0.016f, wrapMinZ + 0.16f);
        glVertex3f(x, wrapY + 0.016f, wrapMaxZ - 0.16f);
    }
    for (int r = 1; r < 9; ++r) {
        float z = -2.40f + r * (4.80f / 8.0f);
        glVertex3f(wrapMinX + 0.20f, wrapY + 0.016f, z);
        glVertex3f(wrapMaxX - 0.20f, wrapY + 0.016f, z);
    }
    glColor4f(1.0f, 1.0f, 1.0f, 0.13f);
    glVertex3f(-8.20f, wrapY + 0.022f, -2.45f);
    glVertex3f(-4.40f, wrapY + 0.022f,  2.20f);
    glVertex3f(-1.30f, wrapY + 0.022f, -2.55f);
    glVertex3f( 2.10f, wrapY + 0.022f,  2.35f);
    glVertex3f( 4.30f, wrapY + 0.022f, -2.35f);
    glVertex3f( 8.20f, wrapY + 0.022f,  1.85f);
    glEnd();

    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float pressScale = 1.0f - bubbles[i].pressDepth * 0.76f;
        float bubbleY = wrapY + 0.112f * pressScale;

        glColor4f(0.74f, 0.86f, 0.92f, 0.31f);
        glPushMatrix();
        glTranslatef(bubbles[i].x, bubbleY, bubbles[i].z);
        glScalef(1.0f, 0.58f * pressScale, 1.0f);
        glutSolidSphere(0.178f, 18, 10);
        glPopMatrix();

        glColor4f(0.94f, 0.98f, 1.0f, 0.32f);
        glBegin(GL_LINE_LOOP);
        for (int s = 0; s < 24; ++s) {
            float a = 2.0f * (float)M_PI * (float)s / 24.0f;
            glVertex3f(bubbles[i].x + cos(a) * 0.188f,
                       wrapY + 0.034f,
                       bubbles[i].z + sin(a) * 0.188f);
        }
        glEnd();

        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);
        glBegin(GL_LINES);
        glVertex3f(bubbles[i].x - 0.060f, bubbleY + 0.070f, bubbles[i].z - 0.060f);
        glVertex3f(bubbles[i].x + 0.030f, bubbleY + 0.082f, bubbles[i].z - 0.090f);
        glVertex3f(bubbles[i].x - 0.045f, bubbleY + 0.030f, bubbles[i].z + 0.020f);
        glVertex3f(bubbles[i].x + 0.060f, bubbleY + 0.034f, bubbles[i].z + 0.062f);
        glColor4f(0.86f, 0.94f, 1.0f, 0.18f);
        glVertex3f(bubbles[i].x - 0.115f, bubbleY + 0.020f, bubbles[i].z - 0.020f);
        glVertex3f(bubbles[i].x + 0.070f, bubbleY + 0.024f, bubbles[i].z - 0.005f);
        glEnd();
    }

    drawLightBeams();
    drawDustParticles();

    // --- 5. GLOSSY PLASTIC WINDOW SHEETS (With Proximity Fade) ---
    {
        float halfW = BOX_WIDTH / 2.0f;
        float halfD = BOX_DEPTH / 2.0f;
        float height = BOX_HEIGHT;

        float mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        // Extract Camera Z coordinate in world space
        float camZ = -(mv[8]*mv[12] + mv[9]*mv[13] + mv[10]*mv[14]);

        // Smooth proximity fade for the front plastic sheet
        float fadeAlpha = 1.0f;
        if (camZ < 8.0f) {
            fadeAlpha = (camZ - 5.0f) / 3.0f;
            if (fadeAlpha < 0.0f) fadeAlpha = 0.0f;
        }

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Draw semi-transparent sheets without writing to depth buffer

        float damage = clamp(1.0f - minHpPct, 0.0f, 1.0f);
        float plasticAlpha = 0.08f + 0.04f * damage;
        float rimAlpha = 0.20f + 0.08f * damage;
        float glareAlpha = 0.040f + 0.025f * damage;
        float zFront = halfD + 0.010f;
        float yTop = height + 0.010f;

        // High gloss specular material configurations
        GLfloat plastic_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat plastic_shininess[] = { 128.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, plastic_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, plastic_shininess);

        // A. Front Plastic Cover (at Z = halfD, only draw if camera is far enough)
        if (fadeAlpha > 0.0f) {
            glColor4f(0.74f, 0.90f, 1.0f, plasticAlpha * fadeAlpha); // Faint blue-white tint
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-halfW, 0.0f,   zFront);
            glVertex3f( halfW, 0.0f,   zFront);
            glVertex3f( halfW, height,  zFront);
            glVertex3f(-halfW, height,  zFront);
            glEnd();

            // Front folded plastic lip and perimeter highlights.
            drawPlasticRimLine(-halfW, 0.0f, zFront + 0.004f, halfW, 0.0f, zFront + 0.004f, 0.030f, 0.0f, 1.0f, 0.0f, rimAlpha * fadeAlpha);
            drawPlasticRimLine(-halfW, height, zFront + 0.004f, halfW, height, zFront + 0.004f, 0.026f, 0.0f, 1.0f, 0.0f, rimAlpha * fadeAlpha);
            drawPlasticRimLine(-halfW, 0.0f, zFront + 0.004f, -halfW, height, zFront + 0.004f, 0.026f, 1.0f, 0.0f, 0.0f, rimAlpha * fadeAlpha);
            drawPlasticRimLine( halfW, 0.0f, zFront + 0.004f,  halfW, height, zFront + 0.004f, 0.026f, 1.0f, 0.0f, 0.0f, rimAlpha * fadeAlpha);

            // Front glossy reflection glare streaks.
            glColor4f(1.0f, 1.0f, 1.0f, glareAlpha * fadeAlpha);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-halfW + 0.25f, height * 0.86f, zFront + 0.006f);
            glVertex3f(-halfW + 1.05f, height,        zFront + 0.006f);
            glVertex3f( halfW - 1.25f, 0.12f,         zFront + 0.006f);
            glVertex3f( halfW - 2.05f, 0.00f,         zFront + 0.006f);
            glEnd();

            glColor4f(1.0f, 1.0f, 1.0f, glareAlpha * 0.55f * fadeAlpha);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-halfW + 3.0f, height,        zFront + 0.007f);
            glVertex3f(-halfW + 3.45f, height,       zFront + 0.007f);
            glVertex3f( halfW - 0.70f, height * 0.23f, zFront + 0.007f);
            glVertex3f( halfW - 1.15f, height * 0.23f, zFront + 0.007f);
            glEnd();
        }

        // B. Top Plastic Cover (at Y = height, always visible from above)
        glColor4f(0.74f, 0.90f, 1.0f, plasticAlpha + 0.025f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-halfW, yTop,  halfD);
        glVertex3f( halfW, yTop,  halfD);
        glVertex3f( halfW, yTop, -halfD);
        glVertex3f(-halfW, yTop, -halfD);
        glEnd();

        // Top folded edges and glossy reflection streaks.
        drawPlasticRimLine(-halfW, yTop + 0.004f,  halfD,  halfW, yTop + 0.004f,  halfD, 0.030f, 0.0f, 0.0f, 1.0f, rimAlpha);
        drawPlasticRimLine(-halfW, yTop + 0.004f, -halfD,  halfW, yTop + 0.004f, -halfD, 0.030f, 0.0f, 0.0f, 1.0f, rimAlpha);
        drawPlasticRimLine(-halfW, yTop + 0.004f, -halfD, -halfW, yTop + 0.004f,  halfD, 0.030f, 1.0f, 0.0f, 0.0f, rimAlpha);
        drawPlasticRimLine( halfW, yTop + 0.004f,  halfD,  halfW, yTop + 0.004f, -halfD, 0.030f, 1.0f, 0.0f, 0.0f, rimAlpha);

        glColor4f(1.0f, 1.0f, 1.0f, glareAlpha);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-halfW + 0.40f, yTop + 0.006f, -halfD + 0.55f);
        glVertex3f(-halfW + 2.00f, yTop + 0.006f, -halfD + 0.05f);
        glVertex3f( halfW - 0.35f, yTop + 0.006f,  halfD - 0.60f);
        glVertex3f( halfW - 1.95f, yTop + 0.006f,  halfD - 0.10f);
        glEnd();

        glColor4f(1.0f, 1.0f, 1.0f, glareAlpha * 0.45f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-halfW + 3.20f, yTop + 0.007f, -halfD);
        glVertex3f(-halfW + 3.80f, yTop + 0.007f, -halfD);
        glVertex3f( halfW - 2.40f, yTop + 0.007f,  halfD);
        glVertex3f( halfW - 3.00f, yTop + 0.007f,  halfD);
        glEnd();

        glDepthMask(GL_TRUE);
        glPopAttrib();
    }

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

// Helper for generating orthogonal vectors for the beam's cross section
static void getOrthogonalVectors(float dx, float dy, float dz, float& ux, float& uy, float& uz, float& vx, float& vy, float& vz) {
    float len = sqrt(dx*dx + dy*dy + dz*dz);
    if (len > 0.001f) {
        dx /= len; dy /= len; dz /= len;
    }
    if (std::abs(dx) < 0.9f) {
        ux = 1.0f; uy = 0.0f; uz = 0.0f;
    } else {
        ux = 0.0f; uy = 1.0f; uz = 0.0f;
    }
    float dot = ux*dx + uy*dy + uz*dz;
    ux -= dot*dx; uy -= dot*dy; uz -= dot*dz;
    float uLen = sqrt(ux*ux + uy*uy + uz*uz);
    if (uLen > 0.001f) {
        ux /= uLen; uy /= uLen; uz /= uLen;
    }
    vx = dy*uz - dz*uy;
    vy = dz*ux - dx*uz;
    vz = dx*uy - dy*ux;
}

// Helper to spawn/reset a particle inside a light beam's volume
static void resetParticle(DustParticle& p, const LightBeam& beam) {
    float t = (rand() % 1000) / 1000.0f;
    float bdx = beam.targetX - beam.sourceX;
    float bdy = beam.targetY - beam.sourceY;
    float bdz = beam.targetZ - beam.sourceZ;
    
    float ux, uy, uz, vx, vy, vz;
    getOrthogonalVectors(bdx, bdy, bdz, ux, uy, uz, vx, vy, vz);
    
    float rad = beam.sourceRadius + t * (beam.targetRadius - beam.sourceRadius);
    float theta = ((rand() % 1000) / 1000.0f) * 2.0f * M_PI;
    float rFactor = sqrt((rand() % 1000) / 1000.0f);
    
    float offX = rFactor * rad * cos(theta);
    float offY = rFactor * rad * sin(theta);
    
    p.x = beam.sourceX + t * bdx + offX * ux + offY * vx;
    p.y = beam.sourceY + t * bdy + offX * uy + offY * vy;
    p.z = beam.sourceZ + t * bdz + offX * uz + offY * vz;
    
    float speed = 0.05f + (rand() % 100) / 2000.0f;
    float len = sqrt(bdx*bdx + bdy*bdy + bdz*bdz);
    p.vx = (bdx / len) * speed + ((rand() % 100) / 1000.0f - 0.05f) * 0.05f;
    p.vy = (bdy / len) * speed + ((rand() % 100) / 1000.0f - 0.05f) * 0.05f;
    p.vz = (bdz / len) * speed + ((rand() % 100) / 1000.0f - 0.05f) * 0.05f;
    
    p.size = 0.018f + (rand() % 100) / 7000.0f;
    p.life = (rand() % 1000) / 1000.0f * 3.0f;
    p.maxLife = 2.0f + (rand() % 200) / 100.0f;
    p.alpha = 0.38f + (rand() % 100) / 250.0f;
}

void Arena::setHpPercentages(float h1Hp, float h1Max, float h2Hp, float h2Max) {
    if (h1Max > 0.0f) hironoHpPct = h1Hp / h1Max;
    else hironoHpPct = 1.0f;
    if (h2Max > 0.0f) dimooHpPct = h2Hp / h2Max;
    else dimooHpPct = 1.0f;
    minHpPct = std::min(hironoHpPct, dimooHpPct);
}

void Arena::initHolesAndBeams() {
    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    holes.clear();
    beams.clear();

    // Unified natural warm-white color
    float wr = 1.0f, wg = 0.95f, wb = 0.86f;

    // Define fewer, stronger holes progressively so the damage reads as major torn cardboard.
    float thresholds[] = { 0.86f, 0.72f, 0.60f, 0.48f, 0.36f, 0.26f, 0.18f, 0.10f };
    char walls[] = { 'B', 'L', 'B', 'R', 'B', 'L', 'R', 'B' };
    const int holeCount = 8;

    srand(54321); // Deterministic seed for randomized positions

    for (int i = 0; i < holeCount; ++i) {
        BoxHole hole;
        hole.hpThreshold = thresholds[i];
        hole.wall = walls[i];
        hole.active = false;
        hole.seed = i * 37 + 11;
        
        // Radius grows slightly on later damage stages for memorable punctures.
        hole.radius = 0.30f + 0.025f * (float)i + (rand() % 100) / 900.0f;

        bool valid = false;
        int retries = 0;
        while (!valid && retries < 50) {
            retries++;
            if (hole.wall == 'B') {
                hole.cx = -halfW + 1.8f + (rand() % 1000) / 1000.0f * (BOX_WIDTH - 3.6f);
                hole.cy = 1.2f + (rand() % 1000) / 1000.0f * (height - 2.2f);
                hole.cz = -halfD;
            } else if (hole.wall == 'L') {
                hole.cx = -halfW;
                hole.cy = 1.2f + (rand() % 1000) / 1000.0f * (height - 2.2f);
                hole.cz = -halfD + 1.2f + (rand() % 1000) / 1000.0f * (BOX_DEPTH - 2.4f);
            } else if (hole.wall == 'R') {
                hole.cx = halfW;
                hole.cy = 1.2f + (rand() % 1000) / 1000.0f * (height - 2.2f);
                hole.cz = -halfD + 1.2f + (rand() % 1000) / 1000.0f * (BOX_DEPTH - 2.4f);
            }

            valid = true;
            for (int j = 0; j < i; ++j) {
                if (holes[j].wall == hole.wall) {
                    float distSq = 0.0f;
                    if (hole.wall == 'B') {
                        distSq = (hole.cx - holes[j].cx)*(hole.cx - holes[j].cx) + (hole.cy - holes[j].cy)*(hole.cy - holes[j].cy);
                    } else {
                        distSq = (hole.cz - holes[j].cz)*(hole.cz - holes[j].cz) + (hole.cy - holes[j].cy)*(hole.cy - holes[j].cy);
                    }
                    if (distSq < 1.8f * 1.8f) { // Prevent overlap (min 1.8 units distance)
                        valid = false;
                        break;
                    }
                }
            }
        }

        LightBeam beam;
        beam.sourceX = hole.cx;
        beam.sourceY = hole.cy;
        beam.sourceZ = hole.cz;
        beam.r = wr; beam.g = wg; beam.b = wb;
        beam.maxAlpha = 0.28f + (rand() % 100) / 1300.0f; 
        beam.sourceRadius = hole.radius * 0.22f; // Tight luminous opening
        beam.targetRadius = 0.75f + (rand() % 100) / 260.0f; // Target spread 0.75f to 1.13f
        beam.flickerPhase = (float)(rand() % 6);
        beam.active = false;

        float slantX = ((rand() % 200) / 100.0f - 1.0f) * 0.6f;
        float slantZ = ((rand() % 200) / 100.0f - 1.0f) * 0.6f;
        float slantY = -1.0f - (rand() % 100) / 100.0f * 0.5f;

        if (hole.wall == 'B') {
            beam.targetX = hole.cx + slantX;
            beam.targetY = hole.cy + slantY;
            beam.targetZ = halfD + 1.0f; // Project forward along wall normal
        } else if (hole.wall == 'L') {
            beam.targetX = halfW - 2.0f; // Project rightward along wall normal
            beam.targetY = hole.cy + slantY;
            beam.targetZ = hole.cz + slantZ;
        } else if (hole.wall == 'R') {
            beam.targetX = -halfW + 2.0f; // Project leftward along wall normal
            beam.targetY = hole.cy + slantY;
            beam.targetZ = hole.cz + slantZ;
        }

        holes.push_back(hole);
        beams.push_back(beam);
    }

    // --- INITIALIZE STATEFUL PHYSICS SPACE OBJECTS (ASTEROIDS & FLOATING BOXS) ---
    {
        float wallThick = 0.15f;
        float tHalfW = halfW + 4.0f;
        float tHalfD = halfD + 3.0f;
        float tThick = 0.35f;

        // Lambda helper to check if initial placement overlaps with table or box AABBs
        auto isInsideSolid = [&](float px, float py, float pz, float radius) -> bool {
            // Table AABB check
            if (px + radius > -tHalfW && px - radius < tHalfW &&
                py + radius > -tThick && py - radius < -0.01f &&
                pz + radius > -tHalfD && pz - radius < tHalfD) {
                return true;
            }
            // Box AABB check (including wall thickness)
            if (px + radius > -halfW - wallThick && px - radius < halfW + wallThick &&
                py + radius > 0.0f && py - radius < height &&
                pz + radius > -halfD - wallThick && pz - radius < halfD) {
                return true;
            }
            return false;
        };

        spaceObjects.clear();
        srand(13579); // Separate seed for deterministic space simulation

        // 1. Generate 20 Asteroids
        for (int i = 0; i < 20; ++i) {
            SpaceObject obj;
            obj.type = 0; // Asteroid
            obj.seed = i * 79 + 17;
            obj.size = 0.20f + 0.35f * ((rand() % 100) / 100.0f); // Size 0.20f to 0.55f

            bool valid = false;
            int retries = 0;
            while (!valid && retries < 100) {
                retries++;
                float angle = ((rand() % 3600) / 3600.0f) * 2.0f * M_PI;
                float rDist = 12.0f + 25.0f * ((rand() % 1000) / 1000.0f); // Distance 12 to 37
                obj.x = rDist * cos(angle);
                obj.z = rDist * sin(angle) - 3.0f;
                obj.y = -8.0f + 18.0f * ((rand() % 1000) / 1000.0f); // Y height -8 to 10

                if (!isInsideSolid(obj.x, obj.y, obj.z, obj.size + 0.5f)) {
                    valid = true;
                }
            }

            // Slow random velocities
            float speed = 0.25f + 0.45f * ((rand() % 100) / 100.0f); // 0.25f to 0.70f units/sec
            float vAngle = ((rand() % 3600) / 3600.0f) * 2.0f * M_PI;
            obj.vx = speed * cos(vAngle);
            obj.vz = speed * sin(vAngle);
            obj.vy = ((rand() % 100) / 100.0f - 0.5f) * 0.4f;

            // Rotation angles and speeds
            obj.rx = (float)(rand() % 360);
            obj.ry = (float)(rand() % 360);
            obj.rz = (float)(rand() % 360);
            obj.rvx = ((rand() % 100) / 100.0f - 0.5f) * 40.0f;
            obj.rvy = ((rand() % 100) / 100.0f - 0.5f) * 40.0f;
            obj.rvz = ((rand() % 100) / 100.0f - 0.5f) * 40.0f;

            spaceObjects.push_back(obj);
        }

        // 2. Generate 8 Floating Boxes ( Hirono & Dimoo & cardboard & labels )
        for (int i = 0; i < 8; ++i) {
            SpaceObject obj;
            obj.type = 1 + (i % 4); // 1: Hirono, 2: Dimoo, 3: Cardboard, 4: Rose label
            obj.seed = i * 131 + 47;
            obj.size = 0.5f + 0.5f * ((rand() % 100) / 100.0f); // size scale 0.5f to 1.0f

            bool valid = false;
            int retries = 0;
            while (!valid && retries < 100) {
                retries++;
                float angle = ((rand() % 3600) / 3600.0f) * 2.0f * M_PI;
                float rDist = 9.0f + 22.0f * ((rand() % 1000) / 1000.0f); // Distance 9 to 31
                obj.x = rDist * cos(angle);
                obj.z = rDist * sin(angle) - 4.0f;
                obj.y = -4.0f + 12.0f * ((rand() % 1000) / 1000.0f); // Y height -4 to 8

                float approxRadius = 1.8f * obj.size;
                if (!isInsideSolid(obj.x, obj.y, obj.z, approxRadius + 0.5f)) {
                    valid = true;
                }
            }

            // Slower space-drift velocity for boxes
            float speed = 0.15f + 0.25f * ((rand() % 100) / 100.0f); // 0.15f to 0.40f units/sec
            float vAngle = ((rand() % 3600) / 3600.0f) * 2.0f * M_PI;
            obj.vx = speed * cos(vAngle);
            obj.vz = speed * sin(vAngle);
            obj.vy = ((rand() % 100) / 100.0f - 0.5f) * 0.2f;

            // Slow zero-gravity spin speeds
            obj.rx = (float)(rand() % 360);
            obj.ry = (float)(rand() % 360);
            obj.rz = (float)(rand() % 360);
            obj.rvx = ((rand() % 100) / 100.0f - 0.5f) * 20.0f;
            obj.rvy = ((rand() % 100) / 100.0f - 0.5f) * 20.0f;
            obj.rvz = ((rand() % 100) / 100.0f - 0.5f) * 20.0f;

            spaceObjects.push_back(obj);
        }
    }

    int particlesPerBeam = 22;
    dustParticles.resize(holeCount * particlesPerBeam);
    for (size_t i = 0; i < dustParticles.size(); ++i) {
        int beamIdx = (int)i / particlesPerBeam;
        resetParticle(dustParticles[i], beams[beamIdx]);
        dustParticles[i].life = ((rand() % 1000) / 1000.0f) * dustParticles[i].maxLife;
    }
}

void Arena::updateLightBeamsAndParticles(float dt) {
    for (size_t i = 0; i < holes.size(); ++i) {
        holes[i].active = (minHpPct <= holes[i].hpThreshold);
        beams[i].active = holes[i].active;
    }

    for (size_t i = 0; i < dustParticles.size(); ++i) {
        int beamIdx = (int)(i * beams.size() / dustParticles.size());
        const LightBeam& beam = beams[beamIdx];

        if (beam.active) {
            dustParticles[i].life -= dt;
            if (dustParticles[i].life <= 0.0f) {
                resetParticle(dustParticles[i], beam);
            } else {
                dustParticles[i].x += dustParticles[i].vx * dt;
                dustParticles[i].y += dustParticles[i].vy * dt;
                dustParticles[i].z += dustParticles[i].vz * dt;
            }
        }
    }
}

void Arena::drawLightBeams() {
    for (size_t k = 0; k < beams.size(); ++k) {
        const LightBeam& beam = beams[k];
        if (!beam.active) continue;

        float timeVal = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        float damage = clamp(1.0f - minHpPct, 0.0f, 1.0f);
        float flicker = 1.0f + 0.12f * sin(timeVal * 4.5f + beam.flickerPhase);
        float currentAlpha = beam.maxAlpha * (0.60f + 0.85f * damage) * flicker;

        float bdx = beam.targetX - beam.sourceX;
        float bdy = beam.targetY - beam.sourceY;
        float bdz = beam.targetZ - beam.sourceZ;

        float ux, uy, uz, vx, vy, vz;
        getOrthogonalVectors(bdx, bdy, bdz, ux, uy, uz, vx, vy, vz);

        int segments = 16;

        // Pass 1: Outer Volumetric Ray
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = (2.0f * M_PI * i) / segments;
            float cosT = cos(theta);
            float sinT = sin(theta);

            float sx = beam.sourceX + beam.sourceRadius * (cosT * ux + sinT * vx);
            float sy = beam.sourceY + beam.sourceRadius * (cosT * uy + sinT * vy);
            float sz = beam.sourceZ + beam.sourceRadius * (cosT * uz + sinT * vz);

            float tx = beam.targetX + beam.targetRadius * (cosT * ux + sinT * vx);
            float ty = beam.targetY + beam.targetRadius * (cosT * uy + sinT * vy);
            float tz = beam.targetZ + beam.targetRadius * (cosT * uz + sinT * vz);

            glColor4f(beam.r, beam.g, beam.b, currentAlpha * 0.42f);
            glVertex3f(sx, sy, sz);

            glColor4f(beam.r, beam.g, beam.b, 0.0f);
            glVertex3f(tx, ty, tz);
        }
        glEnd();

        // Pass 2: Inner Glow Core
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = (2.0f * M_PI * i) / segments;
            float cosT = cos(theta);
            float sinT = sin(theta);

            float sx = beam.sourceX + (beam.sourceRadius * 0.45f) * (cosT * ux + sinT * vx);
            float sy = beam.sourceY + (beam.sourceRadius * 0.45f) * (cosT * uy + sinT * vy);
            float sz = beam.sourceZ + (beam.sourceRadius * 0.45f) * (cosT * uz + sinT * vz);

            float tx = beam.targetX + (beam.targetRadius * 0.22f) * (cosT * ux + sinT * vx);
            float ty = beam.targetY + (beam.targetRadius * 0.22f) * (cosT * uy + sinT * vy);
            float tz = beam.targetZ + (beam.targetRadius * 0.22f) * (cosT * uz + sinT * vz);

            glColor4f(1.0f, 1.0f, 1.0f, currentAlpha * 0.95f);
            glVertex3f(sx, sy, sz);

            glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
            glVertex3f(tx, ty, tz);
        }
        glEnd();

        // A faint landing pool on the cardboard floor makes the beam feel grounded.
        float spotX = clamp(beam.targetX, -BOX_WIDTH / 2.0f + 0.4f, BOX_WIDTH / 2.0f - 0.4f);
        float spotZ = clamp(beam.targetZ, -BOX_DEPTH / 2.0f + 0.3f, BOX_DEPTH / 2.0f - 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.97f, 0.82f, currentAlpha * 0.20f);
        glVertex3f(spotX, 0.012f, spotZ);
        for (int i = 0; i <= 32; ++i) {
            float theta = 2.0f * (float)M_PI * i / 32.0f;
            glColor4f(1.0f, 0.96f, 0.76f, 0.0f);
            glVertex3f(spotX + cos(theta) * beam.targetRadius * 0.65f, 0.012f,
                       spotZ + sin(theta) * beam.targetRadius * 0.32f);
        }
        glEnd();
    }
}

void Arena::drawDustParticles() {
    float mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    float rx = mv[0], ry = mv[4], rz = mv[8];
    float ux = mv[1], uy = mv[5], uz = mv[9];

    for (size_t i = 0; i < dustParticles.size(); ++i) {
        const DustParticle& p = dustParticles[i];
        int beamIdx = (int)(i * beams.size() / dustParticles.size());
        if (!beams[beamIdx].active) continue;

        float lifePct = p.life / p.maxLife;
        float alpha = p.alpha;
        if (lifePct > 0.8f) {
            alpha = (1.0f - lifePct) / 0.2f;
        } else if (lifePct < 0.2f) {
            alpha = lifePct / 0.2f;
        } else {
            alpha = 1.0f;
        }
        alpha *= p.alpha * (0.55f + 0.45f * clamp(1.0f - minHpPct, 0.0f, 1.0f));

        glColor4f(1.0f, 0.98f, 0.90f, alpha);

        float sz = p.size;
        glBegin(GL_QUADS);
        glVertex3f(p.x - rx * sz - ux * sz, p.y - ry * sz - uy * sz, p.z - rz * sz - uz * sz);
        glVertex3f(p.x + rx * sz - ux * sz, p.y - ry * sz - uy * sz, p.z + rz * sz - uz * sz);
        glVertex3f(p.x + rx * sz + ux * sz, p.y + ry * sz + uy * sz, p.z + rz * sz + uz * sz);
        glVertex3f(p.x - rx * sz + ux * sz, p.y - ry * sz + uy * sz, p.z - rz * sz - uz * sz);
        glEnd();
    }
}

void Arena::draw3DTornFlaps(const BoxHole& hole) {
    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glDisable(GL_CULL_FACE);

    float damage = clamp(1.0f - minHpPct, 0.0f, 1.0f);
    float lightFactor = 0.34f + 0.28f * damage;

    int hseed = hole.seed;
    int numFlaps = 2 + (hseed % 3); // 2, 3, or 4 flaps

    // Local helper structs and functions
    struct Point3D { float x, y, z; };
    
    auto getPerturbedRadius = [&](float theta) -> float {
        float seedVal = (float)hole.seed;
        return hole.radius * (1.0f + 0.12f * sin(4.5f * theta + seedVal) + 0.08f * cos(8.0f * theta - seedVal * 1.3f));
    };

    auto getWorldPos = [&](float dx, float dy, float dz) -> Point3D {
        Point3D p;
        if (hole.wall == 'B') {
            p.x = hole.cx + dx;
            p.y = hole.cy + dy;
            p.z = hole.cz + dz;
        } else if (hole.wall == 'L') {
            p.x = hole.cx + dz;
            p.y = hole.cy + dy;
            p.z = hole.cz + dx;
        } else if (hole.wall == 'R') {
            p.x = hole.cx - dz;
            p.y = hole.cy + dy;
            p.z = hole.cz + dx;
        }
        return p;
    };

    auto getTexCoords = [&](Point3D p, float& u, float& v) {
        if (hole.wall == 'B') {
            u = 1.0f - (p.x + halfW) / (2.0f * halfW);
            v = 1.0f - p.y / height;
        } else if (hole.wall == 'L') {
            u = (halfD - p.z) / (2.0f * halfD);
            v = p.y / height;
        } else if (hole.wall == 'R') {
            u = (p.z + halfD) / (2.0f * halfD);
            v = p.y / height;
        }
    };

    auto computeNormal = [](float x0, float y0, float z0,
                            float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float& nx, float& ny, float& nz) {
        float ux = x1 - x0;
        float uy = y1 - y0;
        float uz = z1 - z0;
        float vx = x2 - x0;
        float vy = y2 - y0;
        float vz = z2 - z0;
        nx = uy * vz - uz * vy;
        ny = uz * vx - ux * vz;
        nz = ux * vy - uy * vx;
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        if (len > 0.001f) {
            nx /= len; ny /= len; nz /= len;
        }
    };

    auto getJitter = [&](float theta, float t, float& jx, float& jy, int flapIdx) {
        float seedVal = (float)hole.seed;
        float jFactor = t * 0.035f; // Jitter grows towards the flap tip
        jx = jFactor * sin(theta * 12.0f + seedVal + flapIdx * 1.7f);
        jy = jFactor * cos(theta * 12.0f - seedVal - flapIdx * 2.3f);
    };

    for (int k = 0; k < numFlaps; ++k) {
        float angleMid = (2.0f * M_PI * k) / numFlaps + 0.3f * sin(hseed * 2.3f + k * 1.5f);
        float arcWidth = 0.5f + 0.3f * sin(hseed * 1.7f - k * 2.1f);
        
        float theta1 = angleMid - arcWidth / 2.0f;
        float theta2 = angleMid + arcWidth / 2.0f;
        
        float lengthFactor = 0.8f + 0.3f * sin(hseed * 0.9f + k * 3.3f);
        float flapLen = hole.radius * lengthFactor;
        
        float bendOffset = 0.25f + 0.15f * (0.5f + 0.5f * sin(hseed * 3.1f + k * 0.7f));

        float R_base1 = getPerturbedRadius(theta1);
        float R_base2 = getPerturbedRadius(theta2);
        
        float L_flap1 = R_base1 * lengthFactor;
        float L_flap2 = R_base2 * lengthFactor;

        struct FlapSegment {
            Point3D A, B, C, D;
            float uA, vA, uB, vB, uC, vC, uD, vD;
            float nx, ny, nz;
        };
        std::vector<FlapSegment> segments;
        segments.reserve(4);

        for (int s = 0; s < 4; ++s) {
            float ta = s / 4.0f;
            float tb = (s + 1) / 4.0f;

            float phi1a = angleMid + (1.0f - ta) * (theta1 - angleMid);
            float phi2a = angleMid + (1.0f - ta) * (theta2 - angleMid);
            float phi1b = angleMid + (1.0f - tb) * (theta1 - angleMid);
            float phi2b = angleMid + (1.0f - tb) * (theta2 - angleMid);

            float r1a = R_base1 - ta * L_flap1;
            float r2a = R_base2 - ta * L_flap2;
            float r1b = R_base1 - tb * L_flap1;
            float r2b = R_base2 - tb * L_flap2;

            float jx1a, jy1a, jx2a, jy2a, jx1b, jy1b, jx2b, jy2b;
            getJitter(phi1a, ta, jx1a, jy1a, k);
            getJitter(phi2a, ta, jx2a, jy2a, k);
            getJitter(phi1b, tb, jx1b, jy1b, k);
            getJitter(phi2b, tb, jx2b, jy2b, k);

            float dza = bendOffset * sin((M_PI / 2.0f) * ta);
            float dzb = bendOffset * sin((M_PI / 2.0f) * tb);

            Point3D A = getWorldPos(r1a * cos(phi1a) + jx1a, r1a * sin(phi1a) + jy1a, dza);
            Point3D B = getWorldPos(r2a * cos(phi2a) + jx2a, r2a * sin(phi2a) + jy2a, dza);
            Point3D C = getWorldPos(r1b * cos(phi1b) + jx1b, r1b * sin(phi1b) + jy1b, dzb);
            Point3D D = getWorldPos(r2b * cos(phi2b) + jx2b, r2b * sin(phi2b) + jy2b, dzb);

            float uA, vA, uB, vB, uC, vC, uD, vD;
            getTexCoords(A, uA, vA);
            getTexCoords(B, uB, vB);
            getTexCoords(C, uC, vC);
            getTexCoords(D, uD, vD);

            float nx, ny, nz;
            computeNormal(A.x, A.y, A.z, B.x, B.y, B.z, D.x, D.y, D.z, nx, ny, nz);

            FlapSegment seg = { A, B, C, D, uA, vA, uB, vB, uC, vC, uD, vD, nx, ny, nz };
            segments.push_back(seg);
        }

        // 1. Draw outer textured face (facing wall exterior)
        glEnable(GL_TEXTURE_2D);
        if (hole.wall == 'B') {
            glBindTexture(GL_TEXTURE_2D, backTex);
        } else {
            glBindTexture(GL_TEXTURE_2D, leftRightTex);
        }
        glColor3f(0.58f * lightFactor, 0.56f * lightFactor, 0.50f * lightFactor);
        glBegin(GL_QUADS);
        for (const auto& seg : segments) {
            glNormal3f(seg.nx, seg.ny, seg.nz);
            glTexCoord2f(seg.uA, seg.vA); glVertex3f(seg.A.x, seg.A.y, seg.A.z);
            glTexCoord2f(seg.uB, seg.vB); glVertex3f(seg.B.x, seg.B.y, seg.B.z);
            glTexCoord2f(seg.uD, seg.vD); glVertex3f(seg.D.x, seg.D.y, seg.D.z);
            glTexCoord2f(seg.uC, seg.vC); glVertex3f(seg.C.x, seg.C.y, seg.C.z);
        }
        glEnd();

        // 2. Draw inner untextured cardboard face (facing wall interior)
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.95f * lightFactor, 0.86f * lightFactor, 0.68f * lightFactor); // Fresh torn cardboard underside
        glBegin(GL_QUADS);
        for (const auto& seg : segments) {
            glNormal3f(-seg.nx, -seg.ny, -seg.nz);
            glVertex3f(seg.A.x, seg.A.y, seg.A.z);
            glVertex3f(seg.C.x, seg.C.y, seg.C.z);
            glVertex3f(seg.D.x, seg.D.y, seg.D.z);
            glVertex3f(seg.B.x, seg.B.y, seg.B.z);
        }
        glEnd();
    }



    glPopAttrib();
}
