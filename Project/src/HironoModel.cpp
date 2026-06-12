#include "HironoModel.h"

#include <vector>

namespace HironoModel {

struct Vec3 {
    float x, y, z;
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float px, float py, float pz) : x(px), y(py), z(pz) {}
};

static Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static Vec3 operator*(const Vec3& a, float s) { return Vec3(a.x * s, a.y * s, a.z * s); }

static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

static Vec3 normalize(const Vec3& v) {
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len <= 0.0001f) {
        return Vec3(0.0f, 1.0f, 0.0f);
    }
    return Vec3(v.x / len, v.y / len, v.z / len);
}

static float saturate(float v) {
    return clamp(v, 0.0f, 1.0f);
}

static float pseudo01(float seed) {
    return 0.5f + 0.5f * sin(seed * 12.9898f + cos(seed * 4.1414f) * 78.233f);
}

static const float kDefaultFaceUOffset = 0.000f;
static const float kDefaultFaceVOffset = 0.040f;
static const float kDefaultFaceUScale = 1.080f;
static const float kDefaultFaceVScale = 1.080f;
static const float kDefaultHairFrontTilt = -25.0f;
static const float kDefaultHairTopTilt = -15.0f;
static const float kDefaultHairSideSpread = 0.0f;

static float gFaceUOffset = kDefaultFaceUOffset;
static float gFaceVOffset = kDefaultFaceVOffset;
static float gFaceUScale = kDefaultFaceUScale;
static float gFaceVScale = kDefaultFaceVScale;
static float gHairFrontTilt = kDefaultHairFrontTilt;
static float gHairTopTilt = kDefaultHairTopTilt;
static float gHairSideSpread = kDefaultHairSideSpread;

static const HairClusterState kDefaultHairClusters[] = {
    // Front bangs
    {{-0.22f, 0.18f, 0.20f}, {0.180f, 0.120f, 0.060f}, {55.0f, -20.0f, 8.0f}, 0, false},
    {{-0.10f, 0.22f, 0.23f}, {0.175f, 0.130f, 0.065f}, {50.0f,  -8.0f, 4.0f}, 0, false},
    {{ 0.00f, 0.24f, 0.25f}, {0.185f, 0.135f, 0.070f}, {48.0f,   0.0f, 0.0f}, 0, false},
    {{ 0.10f, 0.22f, 0.23f}, {0.175f, 0.130f, 0.065f}, {50.0f,   8.0f,-4.0f}, 0, false},
    {{ 0.22f, 0.18f, 0.20f}, {0.180f, 0.120f, 0.060f}, {55.0f,  20.0f,-8.0f}, 0, false},
    // Top clusters
    {{-0.25f, 0.32f, 0.08f}, {0.155f, 0.110f, 0.060f}, {30.0f, -18.0f, 3.0f}, 0, false},
    {{-0.08f, 0.38f, 0.10f}, {0.150f, 0.105f, 0.055f}, {28.0f,  -7.0f, 1.0f}, 0, true },
    {{ 0.08f, 0.38f, 0.10f}, {0.150f, 0.105f, 0.055f}, {28.0f,   7.0f,-1.0f}, 0, false},
    {{ 0.25f, 0.32f, 0.08f}, {0.155f, 0.110f, 0.060f}, {30.0f,  18.0f,-3.0f}, 0, true },
    // Back and sides
    {{-0.28f, 0.24f,-0.10f}, {0.145f, 0.105f, 0.055f}, { 6.0f, -22.0f, 4.0f}, 0, false},
    {{ 0.00f, 0.24f,-0.20f}, {0.145f, 0.105f, 0.055f}, { 4.0f,   0.0f, 0.0f}, 0, false},
    {{ 0.28f, 0.24f,-0.10f}, {0.145f, 0.105f, 0.055f}, { 6.0f,  22.0f,-4.0f}, 0, false}
};

static HairClusterState gHairClusters[sizeof(kDefaultHairClusters) / sizeof(kDefaultHairClusters[0])] = {};

static void setMaterial(const GLfloat ambient[4], const GLfloat diffuse[4],
                        const GLfloat specular[4], GLfloat shininess) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

static void setPearlMaterial(float r, float g, float b, float shine) {
    GLfloat ambient[]  = {r * 0.82f, g * 0.82f, b * 0.82f, 1.0f};
    GLfloat diffuse[]  = {r, g, b, 1.0f};
    GLfloat specular[] = {0.34f, 0.34f, 0.34f, 1.0f};
    setMaterial(ambient, diffuse, specular, shine);
}

static void drawEllipsoid3D(float rx, float ry, float rz, int lats, int longs) {
    for (int i = 0; i < lats; ++i) {
        float lat0 = (float)M_PI * (-0.5f + (float)i / (float)lats);
        float lat1 = (float)M_PI * (-0.5f + (float)(i + 1) / (float)lats);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; ++j) {
            float lng = 2.0f * (float)M_PI * (float)j / (float)longs;
            float x = cos(lng);
            float y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0 * rx, y * zr0 * ry, z0 * rz);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1 * rx, y * zr1 * ry, z1 * rz);
        }
        glEnd();
    }
}

static void drawPetal3D(float rx, float ry, float rz, int lats, int longs,
                        const GLfloat baseColor[3], const GLfloat tipColor[3],
                        float bendX, float bendZ) {
    for (int i = 0; i < lats; ++i) {
        float lat0 = (float)M_PI * (-0.5f + (float)i / (float)lats);
        float lat1 = (float)M_PI * (-0.5f + (float)(i + 1) / (float)lats);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; ++j) {
            float lng = 2.0f * (float)M_PI * (float)j / (float)longs;
            float x = cos(lng);
            float y = sin(lng);

            float x0_u = x * zr0;
            float y0_u = y * zr0;
            float x1_u = x * zr1;
            float y1_u = y * zr1;

            float taper0 = 1.0f - clamp(y0_u, 0.0f, 1.0f) * 0.25f;
            float taper1 = 1.0f - clamp(y1_u, 0.0f, 1.0f) * 0.25f;

            float factor0 = (y0_u + 1.0f) * 0.5f;
            float factor1 = (y1_u + 1.0f) * 0.5f;

            factor0 = factor0 * factor0 * (3.0f - 2.0f * factor0);
            factor1 = factor1 * factor1 * (3.0f - 2.0f * factor1);

            float nx0 = x0_u;
            float ny0 = y0_u + 2.0f * bendX * y0_u * x0_u + 2.0f * bendZ * y0_u * z0;
            float nz0 = z0;
            Vec3 n0 = normalize(Vec3(nx0, ny0, nz0));
            glNormal3f(n0.x, n0.y, n0.z);
            glColor4f(baseColor[0] * (1.0f - factor0) + tipColor[0] * factor0,
                      baseColor[1] * (1.0f - factor0) + tipColor[1] * factor0,
                      baseColor[2] * (1.0f - factor0) + tipColor[2] * factor0,
                      1.0f);
            float bendFactor0 = 1.0f - y0_u * y0_u;
            float bx0 = bendX * bendFactor0;
            float bz0 = bendZ * bendFactor0;
            glVertex3f(x0_u * rx * taper0 + bx0, y0_u * ry, z0 * rz * taper0 + bz0);

            float nx1 = x1_u;
            float ny1 = y1_u + 2.0f * bendX * y1_u * x1_u + 2.0f * bendZ * y1_u * z1;
            float nz1 = z1;
            Vec3 n1 = normalize(Vec3(nx1, ny1, nz1));
            glNormal3f(n1.x, n1.y, n1.z);
            glColor4f(baseColor[0] * (1.0f - factor1) + tipColor[0] * factor1,
                      baseColor[1] * (1.0f - factor1) + tipColor[1] * factor1,
                      baseColor[2] * (1.0f - factor1) + tipColor[2] * factor1,
                      1.0f);
            float bendFactor1 = 1.0f - y1_u * y1_u;
            float bx1 = bendX * bendFactor1;
            float bz1 = bendZ * bendFactor1;
            glVertex3f(x1_u * rx * taper1 + bx1, y1_u * ry, z1 * rz * taper1 + bz1);
        }
        glEnd();
    }
}

static void drawSphere3D(float r, int lats, int longs) {
    drawEllipsoid3D(r, r, r, lats, longs);
}

static void drawCylinderY(float radiusBottom, float radiusTop, float height, int slices) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)slices;
        float c = cos(theta);
        float s = sin(theta);
        glNormal3f(c, 0.0f, s);
        glVertex3f(c * radiusBottom, 0.0f, s * radiusBottom);
        glVertex3f(c * radiusTop, height, s * radiusTop);
    }
    glEnd();
}

static void orientAlongSegment(const Vec3& from, const Vec3& to) {
    Vec3 dir = to - from;
    float len = sqrt(dot(dir, dir));
    glTranslatef(from.x, from.y, from.z);
    if (len <= 0.0001f) {
        return;
    }

    dir = normalize(dir);
    Vec3 up(0.0f, 1.0f, 0.0f);
    float c = clamp(dot(up, dir), -1.0f, 1.0f);
    float angle = acos(c) * 180.0f / (float)M_PI;
    Vec3 axis = cross(up, dir);
    float axisLen = sqrt(dot(axis, axis));
    if (axisLen <= 0.0001f) {
        if (c < 0.0f) {
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        }
    } else {
        axis = axis * (1.0f / axisLen);
        glRotatef(angle, axis.x, axis.y, axis.z);
    }
}

static void drawCapsuleBetween(const Vec3& from, const Vec3& to, float radius, int slices) {
    Vec3 delta = to - from;
    float height = sqrt(dot(delta, delta));
    if (height <= 0.0001f) {
        return;
    }

    glPushMatrix();
    orientAlongSegment(from, to);
    drawCylinderY(radius, radius, height, slices);
    glPushMatrix();
    drawSphere3D(radius, 8, slices);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);
    drawSphere3D(radius, 8, slices);
    glPopMatrix();
    glPopMatrix();
}

static void drawSoftShadow(float radiusX, float radiusZ, float alpha) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.06f, 0.08f, 0.08f, alpha);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 32; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / 32.0f;
        glColor4f(0.04f, 0.06f, 0.06f, 0.0f);
        glVertex3f(cos(theta) * radiusX, 0.0f, sin(theta) * radiusZ);
    }
    glEnd();
}

static void drawFaceWrap(GLuint faceTex, float alpha) {
    if (alpha <= 0.001f || !faceTex) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, faceTex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.0f, -2.0f);
    glDepthMask(GL_TRUE);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    const int lats = 24;
    const int longs = 40;
    const float rx = 0.384f;
    const float ry = 0.384f;
    const float rz = 0.364f;
    float uCenter = 0.50f + gFaceUOffset;
    float vCenter = 0.53f + gFaceVOffset;
    float uSpan = 0.60f * gFaceUScale;
    float vSpan = 0.74f * gFaceVScale;
    float uMin = clamp(uCenter - uSpan * 0.5f, 0.0f, 1.0f);
    float uMax = clamp(uCenter + uSpan * 0.5f, 0.0f, 1.0f);
    float vMin = clamp(vCenter - vSpan * 0.5f, 0.0f, 1.0f);
    float vMax = clamp(vCenter + vSpan * 0.5f, 0.0f, 1.0f);

    for (int iy = 0; iy < lats; ++iy) {
        float lat0 = -0.5f * (float)M_PI + (float)iy / (float)lats * (float)M_PI;
        float lat1 = -0.5f * (float)M_PI + (float)(iy + 1) / (float)lats * (float)M_PI;

        float y0 = sin(lat0);
        float r0 = cos(lat0);
        float y1 = sin(lat1);
        float r1 = cos(lat1);
        float v0 = 0.5f - lat0 / (float)M_PI;
        float v1 = 0.5f - lat1 / (float)M_PI;

        glBegin(GL_QUAD_STRIP);
        for (int ix = 0; ix <= longs; ++ix) {
            float lon = -1.0f * (float)M_PI + (float)ix / (float)longs * 2.0f * (float)M_PI;
            float sx = sin(lon);
            float cz = cos(lon);
            float rawU = (lon + (float)M_PI) / (2.0f * (float)M_PI);
            float u = uMin + rawU * (uMax - uMin);
            float vv0 = vMax - v0 * (vMax - vMin);
            float vv1 = vMax - v1 * (vMax - vMin);

            float nx0 = sx * r0;
            float ny0 = y0;
            float nz0 = cz * r0;
            float nx1 = sx * r1;
            float ny1 = y1;
            float nz1 = cz * r1;

            glTexCoord2f(u, vv0);
            glNormal3f(nx0, ny0, nz0);
            glVertex3f(nx0 * rx, ny0 * ry, nz0 * rz);

            glTexCoord2f(u, vv1);
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(nx1 * rx, ny1 * ry, nz1 * rz);
        }
        glEnd();
    }

    glPopAttrib();
}

static void drawHairCluster(const Vec3& pos, const Vec3& scale,
                            const Vec3& rot, int colorIndex,
                            bool highlightTop, float bendX, float bendZ) {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

    glScalef(1.12f, 1.02f, 1.40f);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Hirono's golden-yellow palette (Blond, Mustard Yellow, Cream)
    GLfloat mustardCol[] = {0.95f, 0.72f, 0.08f};
    GLfloat creamCol[]   = {0.98f, 0.95f, 0.88f};

    // Y height color gradient
    float tY = clamp((pos.y - 0.0f) / 0.45f, 0.0f, 1.0f);
    GLfloat richBaseCol[3];
    for (int c = 0; c < 3; ++c) {
        richBaseCol[c] = mustardCol[c] * (1.0f - tY * 0.35f) + creamCol[c] * (tY * 0.35f);
    }
    const GLfloat* tipCol = creamCol;

    drawPetal3D(scale.x, scale.y, scale.z, 12, 16, richBaseCol, tipCol, bendX, bendZ);

    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

static void drawHairClusters(const HironoVisualState& state, float t, float moveLean) {
    glDisable(GL_TEXTURE_2D);
    float sway = sin(t * 2.5f) * (1.0f + state.moveBlend * 0.8f);
    const int hairCount = (int)(sizeof(gHairClusters) / sizeof(gHairClusters[0]));
    for (int i = 0; i < hairCount; ++i) {
        Vec3 p(gHairClusters[i].pos[0], gHairClusters[i].pos[1], gHairClusters[i].pos[2]);
        Vec3 s(gHairClusters[i].scale[0], gHairClusters[i].scale[1], gHairClusters[i].scale[2]);
        Vec3 r(gHairClusters[i].rot[0], gHairClusters[i].rot[1], gHairClusters[i].rot[2]);

        p.y += sin(t * 2.1f + i * 0.42f) * 0.003f;
        p.x += state.moveBlend * ((i % 2 == 0) ? -0.005f : 0.005f);
        r.z += sway * ((i < 5) ? 0.26f : ((i < 9) ? 0.18f : 0.14f)) + moveLean * 0.08f;

        if (i < 5) {
            r.x += gHairFrontTilt;
        } else {
            r.x += gHairTopTilt;
        }

        if (i == 0 || i == 5 || i == 9) {
            r.y -= gHairSideSpread;
            r.z += gHairSideSpread * 0.35f;
        } else if (i == 4 || i == 8 || i == 11) {
            r.y += gHairSideSpread;
            r.z -= gHairSideSpread * 0.35f;
        }

        float bendX = clamp(p.x * 0.16f, -0.04f, 0.04f);
        float bendZ = clamp(-p.z * 0.16f, -0.03f, 0.03f) - 0.015f;

        drawHairCluster(p, s, r, i, gHairClusters[i].highlightTop, bendX, bendZ);
    }
}

static void drawScarf(float t) {
    // Yellow Scarf: RGB(0.95, 0.72, 0.08)
    GLfloat scarfAmbient[]  = {0.30f, 0.22f, 0.02f, 1.0f};
    GLfloat scarfDiffuse[]  = {0.95f, 0.72f, 0.08f, 1.0f};
    GLfloat scarfSpecular[] = {0.15f, 0.12f, 0.02f, 1.0f};
    setMaterial(scarfAmbient, scarfDiffuse, scarfSpecular, 15.0f);
    
    // Collar torus
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    glScalef(1.1f, 0.4f, 1.1f);
    drawEllipsoid3D(0.24f, 0.24f, 0.24f, 8, 12);
    glPopMatrix();
    
    // Blowing scarf tail
    glPushMatrix();
    glTranslatef(0.18f, 0.28f, 0.10f);
    
    float wave = sin(t * 5.0f) * 12.0f;
    glRotatef(25.0f + wave, 0.0f, 0.0f, 1.0f);
    glRotatef(15.0f + wave * 0.4f, 1.0f, 0.0f, 0.0f);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f,   0.0f,   0.0f);
    glVertex3f(0.0f,  -0.08f,  0.05f);
    glVertex3f(0.28f, -0.12f,  0.08f);
    glVertex3f(0.25f,  0.02f,  0.02f);
    
    glVertex3f(0.25f,  0.02f,  0.02f);
    glVertex3f(0.28f, -0.12f,  0.08f);
    glVertex3f(0.48f, -0.16f,  0.12f);
    glVertex3f(0.45f, -0.02f,  0.04f);
    glEnd();
    
    glPopMatrix();
}

static void drawCape(float moveLean, float ultFactor) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    // Red Cape: RGB(0.72, 0.08, 0.08)
    GLfloat capeAmbient[]  = {0.22f, 0.02f, 0.02f, 0.85f};
    GLfloat capeDiffuse[]  = {0.72f, 0.08f, 0.08f, 0.85f};
    GLfloat capeSpecular[] = {0.20f, 0.05f, 0.05f, 0.85f};
    setMaterial(capeAmbient, capeDiffuse, capeSpecular, 20.0f);
    
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -0.18f);
    
    glRotatef(-moveLean * 0.8f, 0.0f, 0.0f, 1.0f);
    glRotatef(20.0f + ultFactor * 25.0f, 1.0f, 0.0f, 0.0f);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.1f, -1.0f);
    
    // Left side
    glVertex3f(-0.10f,  0.0f,  0.0f);
    glVertex3f( 0.00f,  0.0f,  0.0f);
    glVertex3f(-0.05f, -0.65f, -0.15f);
    glVertex3f(-0.35f, -0.60f, -0.10f);
    
    // Right side
    glVertex3f( 0.00f,  0.0f,  0.0f);
    glVertex3f( 0.10f,  0.0f,  0.0f);
    glVertex3f( 0.35f, -0.60f, -0.10f);
    glVertex3f( 0.05f, -0.65f, -0.15f);
    glEnd();
    
    glPopMatrix();
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

static void drawRoseDome(float t, float actionPulse) {
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.24f);
    
    // Wood base
    GLfloat baseAmbient[]  = {0.16f, 0.12f, 0.08f, 1.0f};
    GLfloat baseDiffuse[]  = {0.36f, 0.27f, 0.16f, 1.0f};
    GLfloat baseSpecular[] = {0.05f, 0.04f, 0.02f, 1.0f};
    setMaterial(baseAmbient, baseDiffuse, baseSpecular, 10.0f);
    drawCylinderY(0.14f, 0.14f, 0.02f, 12);
    
    // Rose inside
    // Green stalk
    GLfloat stalkAmbient[]  = {0.02f, 0.10f, 0.03f, 1.0f};
    GLfloat stalkDiffuse[]  = {0.10f, 0.50f, 0.15f, 1.0f};
    setMaterial(stalkAmbient, stalkDiffuse, stalkDiffuse, 5.0f);
    drawCylinderY(0.008f, 0.008f, 0.12f, 6);
    
    // Red rose flower head
    glPushMatrix();
    glTranslatef(0.0f, 0.12f, 0.0f);
    GLfloat flowerAmbient[]  = {0.26f, 0.02f, 0.02f, 1.0f};
    GLfloat flowerDiffuse[]  = {0.85f, 0.05f, 0.05f, 1.0f};
    setMaterial(flowerAmbient, flowerDiffuse, flowerDiffuse, 15.0f);
    drawEllipsoid3D(0.028f, 0.028f, 0.028f, 8, 8);
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glRotatef(i * 90.0f + t * 5.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.01f, 0.012f);
        drawEllipsoid3D(0.018f, 0.012f, 0.007f, 6, 6);
        glPopMatrix();
    }
    glPopMatrix();
    
    // Transparent glass dome
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    GLfloat glassAmbient[]  = {0.10f, 0.15f, 0.20f, 0.35f};
    GLfloat glassDiffuse[]  = {0.40f, 0.60f, 0.80f, 0.35f};
    GLfloat glassSpecular[] = {0.80f, 0.90f, 1.00f, 0.35f};
    setMaterial(glassAmbient, glassDiffuse, glassSpecular, 90.0f);
    
    // Dome cylinder
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    drawCylinderY(0.12f, 0.12f, 0.16f, 16);
    glPopMatrix();
    
    // Dome spherical cap
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.0f);
    drawEllipsoid3D(0.12f, 0.08f, 0.12f, 12, 16);
    glPopMatrix();
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    glPopMatrix();
}

static void drawHeadAndFace(const HironoVisualState& state, float t, float moveLean) {
    // Skin: Warm cream
    setPearlMaterial(1.0f, 0.94f, 0.92f, 60.0f);
    drawEllipsoid3D(0.37f, 0.37f, 0.35f, 18, 24);

    float detail = saturate(state.faceDetail);
    drawFaceWrap(state.faceTex, detail);
    drawHairClusters(state, t, moveLean);
}

static void drawMascotBody(const HironoVisualState& state, float t) {
    float walk = sin(t * 5.6f);
    float handBob = sin(t * 2.8f) * 0.010f;
    float skillFactor = clamp(state.skillPulse, 0.0f, 1.0f);
    float ultFactor = clamp(state.ultPulse, 0.0f, 1.0f);

    // Green jacket body
    GLfloat bodyAmbient[]  = {0.05f, 0.15f, 0.08f, 1.0f};
    GLfloat bodyDiffuse[]  = {0.15f, 0.38f, 0.22f, 1.0f};
    GLfloat bodySpecular[] = {0.08f, 0.12f, 0.08f, 1.0f};
    setMaterial(bodyAmbient, bodyDiffuse, bodySpecular, 12.0f);
    drawEllipsoid3D(0.32f, 0.38f, 0.30f, 18, 24);

    // Sleeves/arms
    setPearlMaterial(0.99f, 0.91f, 0.90f, 42.0f);
    
    // Left Arm
    glPushMatrix();
    glTranslatef(-0.25f, -0.05f + handBob, 0.12f);
    float leftRotX = -10.0f + walk * 15.0f * state.moveBlend;
    float leftRotZ = 12.0f;
    glRotatef(leftRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(leftRotZ, 0.0f, 0.0f, 1.0f);
    drawEllipsoid3D(0.065f, 0.050f, 0.055f, 10, 14);
    glPopMatrix();

    // Right Arm
    glPushMatrix();
    glTranslatef(0.25f, -0.05f - handBob * 0.6f, 0.12f);
    float rightRotX = -10.0f - walk * 15.0f * state.moveBlend;
    float rightRotZ = -12.0f;
    glRotatef(rightRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rightRotZ, 0.0f, 0.0f, 1.0f);
    drawEllipsoid3D(0.065f, 0.050f, 0.055f, 10, 14);
    glPopMatrix();
}

static void drawSkillAura(float skillPulse, float t) {
    if (skillPulse <= 0.01f) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Expand concentric circles
    float ring = 0.25f + skillPulse * 0.85f + sin(t * 6.0f) * 0.03f;
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    
    // Draw wireframe loop
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 36; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / 36.0f;
        glColor4f(0.95f, 0.72f, 0.08f, 1.0f - (ring / 1.2f));
        glVertex3f(cos(theta) * ring, sin(theta) * ring, 0.0f);
    }
    glEnd();
    
    glPopMatrix();
    glPopAttrib();
}

void draw(const HironoVisualState& state) {
    static bool gHairClustersInitialized = false;
    if (!gHairClustersInitialized) {
        resetHairClusterStates();
        gHairClustersInitialized = true;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_LINE_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(state.x, state.y, state.z);

    if (!state.facingRight) {
        glScalef(-1.0f, 1.0f, 1.0f);
    }

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float t = state.time;
    float walk = sin(t * 5.6f);
    float floatLift = 0.0f;
    float moveDrift = walk * 0.012f * state.moveBlend;
    float moveLean = walk * 3.5f * state.moveBlend;
    float bodyBreath = 1.0f + sin(t * 1.57f) * 0.012f;

    glTranslatef(moveDrift, 0.0f, 0.0f);

    // Draw shadow
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0.0f, -0.02f, 0.0f);
    drawSoftShadow(0.32f, 0.20f, 0.18f);
    glPopMatrix();
    glPopAttrib();

    glPushMatrix();
    // Body root transform
    glTranslatef(0.0f, 0.40f + floatLift, 0.0f);
    glRotatef(moveLean, 0.0f, 0.0f, 1.0f);
    glScalef(bodyBreath, bodyBreath + state.moveBlend * 0.02f, bodyBreath);

    // Draw suit body & arms
    drawMascotBody(state, t);

    // Draw head (child of body)
    glPushMatrix();
    float headBob = 0.006f * fabs(walk) * state.moveBlend;
    glTranslatef(0.0f, headBob, 0.0f);
    drawHeadAndFace(state, t, moveLean);
    glPopMatrix();

    // Draw scarf (child of body)
    drawScarf(t);

    // Draw cape (child of body)
    drawCape(moveLean, state.ultPulse);

    // Draw rose dome (child of body)
    drawRoseDome(t, state.attackPulse);

    glPopMatrix(); // Pop body root transform

    // Skill aura (drawn in world coordinates)
    drawSkillAura(state.skillPulse, t);

    glPopMatrix();
    glPopAttrib();
}

void setFaceTextureTuning(float uOffset, float vOffset, float uScale, float vScale) {
    gFaceUOffset = clamp(uOffset, -0.45f, 0.45f);
    gFaceVOffset = clamp(vOffset, -0.45f, 0.45f);
    gFaceUScale = clamp(uScale, 0.20f, 2.50f);
    gFaceVScale = clamp(vScale, 0.20f, 2.50f);
}

void resetFaceTextureTuning() {
    gFaceUOffset = kDefaultFaceUOffset;
    gFaceVOffset = kDefaultFaceVOffset;
    gFaceUScale = kDefaultFaceUScale;
    gFaceVScale = kDefaultFaceVScale;
}

void setHairAngleTuning(float frontTilt, float topTilt, float sideSpread) {
    gHairFrontTilt = clamp(frontTilt, -45.0f, 45.0f);
    gHairTopTilt = clamp(topTilt, -45.0f, 45.0f);
    gHairSideSpread = clamp(sideSpread, -35.0f, 35.0f);
}

void resetHairAngleTuning() {
    gHairFrontTilt = kDefaultHairFrontTilt;
    gHairTopTilt = kDefaultHairTopTilt;
    gHairSideSpread = kDefaultHairSideSpread;
}

int getHairClusterCount() {
    return (int)(sizeof(gHairClusters) / sizeof(gHairClusters[0]));
}

HairClusterState getHairClusterState(int index) {
    if (index < 0 || index >= getHairClusterCount()) {
        HairClusterState state = {};
        return state;
    }
    return gHairClusters[index];
}

void setHairClusterState(int index, const HairClusterState& state) {
    if (index < 0 || index >= getHairClusterCount()) {
        return;
    }
    gHairClusters[index] = state;
    gHairClusters[index].scale[0] = clamp(gHairClusters[index].scale[0], 0.04f, 0.35f);
    gHairClusters[index].scale[1] = clamp(gHairClusters[index].scale[1], 0.04f, 0.35f);
    gHairClusters[index].scale[2] = clamp(gHairClusters[index].scale[2], 0.006f, 0.250f);
    gHairClusters[index].colorIndex = clamp(gHairClusters[index].colorIndex, 0, 4);
}

void resetHairClusterStates() {
    for (int i = 0; i < getHairClusterCount(); ++i) {
        gHairClusters[i] = kDefaultHairClusters[i];
    }
}

}
