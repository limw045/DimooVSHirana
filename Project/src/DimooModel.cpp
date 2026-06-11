#include "DimooModel.h"

#include <vector>

namespace DimooModel {

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

static const float kDefaultFaceUOffset = -0.008f;
static const float kDefaultFaceVOffset = -0.002f;
static const float kDefaultFaceUScale = 1.800f;
static const float kDefaultFaceVScale = 1.800f;
static const float kDefaultHairFrontTilt = -35.0f;
static const float kDefaultHairTopTilt = -35.0f;
static const float kDefaultHairSideSpread = 0.0f;

static float gFaceUOffset = kDefaultFaceUOffset;
static float gFaceVOffset = kDefaultFaceVOffset;
static float gFaceUScale = kDefaultFaceUScale;
static float gFaceVScale = kDefaultFaceVScale;
static float gHairFrontTilt = kDefaultHairFrontTilt;
static float gHairTopTilt = kDefaultHairTopTilt;
static float gHairSideSpread = kDefaultHairSideSpread;

static const HairClusterState kDefaultHairClusters[] = {
    {{-0.25f, 0.16f, 0.19f}, {0.188f, 0.124f, 0.060f}, {58.0f, -24.0f, 10.0f}, 0, false},
    {{-0.13f, 0.20f, 0.22f}, {0.182f, 0.134f, 0.065f}, {54.0f, -10.0f, 5.0f}, 0, false},
    {{ 0.00f, 0.22f, 0.24f}, {0.194f, 0.142f, 0.070f}, {52.0f,   0.0f, 0.0f}, 0, false},
    {{ 0.13f, 0.20f, 0.22f}, {0.182f, 0.134f, 0.065f}, {54.0f,  10.0f,-5.0f}, 0, false},
    {{ 0.25f, 0.16f, 0.19f}, {0.188f, 0.124f, 0.060f}, {58.0f,  24.0f,-10.0f}, 0, false},
    {{-0.28f, 0.30f, 0.09f}, {0.162f, 0.116f, 0.060f}, {34.0f, -20.0f, 4.0f}, 1, false},
    {{-0.11f, 0.36f, 0.11f}, {0.154f, 0.112f, 0.055f}, {30.0f,  -9.0f, 2.0f}, 3, true },
    {{ 0.09f, 0.37f, 0.11f}, {0.154f, 0.112f, 0.055f}, {30.0f,   9.0f,-2.0f}, 2, false},
    {{ 0.27f, 0.30f, 0.09f}, {0.162f, 0.116f, 0.060f}, {34.0f,  20.0f,-4.0f}, 1, true },
    {{-0.30f, 0.25f,-0.08f}, {0.152f, 0.110f, 0.055f}, { 8.0f, -24.0f, 5.0f}, 0, false},
    {{-0.14f, 0.32f,-0.15f}, {0.146f, 0.108f, 0.050f}, { 4.0f, -12.0f, 2.0f}, 1, true },
    {{ 0.05f, 0.33f,-0.16f}, {0.146f, 0.108f, 0.050f}, { 4.0f,   8.0f,-2.0f}, 3, true },
    {{ 0.23f, 0.27f,-0.11f}, {0.146f, 0.108f, 0.050f}, { 8.0f,  22.0f,-5.0f}, 2, true },
    {{ 0.00f, 0.25f,-0.23f}, {0.152f, 0.110f, 0.055f}, { 4.0f,   0.0f, 0.0f}, 0, false},
    {{-0.33f, 0.02f, 0.09f}, {0.090f, 0.132f, 0.055f}, {38.0f, -28.0f, 8.0f}, 0, false},
    {{-0.30f,-0.12f, 0.00f}, {0.088f, 0.124f, 0.050f}, {24.0f, -20.0f, 5.0f}, 0, false},
    {{ 0.33f, 0.02f, 0.09f}, {0.090f, 0.132f, 0.055f}, {38.0f,  28.0f,-8.0f}, 0, false},
    {{ 0.30f,-0.12f, 0.00f}, {0.088f, 0.124f, 0.050f}, {24.0f,  20.0f,-5.0f}, 0, false},
    {{-0.22f, 0.15f,-0.22f}, {0.160f, 0.120f, 0.060f}, {12.0f, -40.0f,  8.0f}, 0, false},
    {{ 0.00f, 0.12f,-0.28f}, {0.170f, 0.130f, 0.065f}, { 6.0f,   0.0f,  0.0f}, 0, false},
    {{ 0.22f, 0.15f,-0.22f}, {0.160f, 0.120f, 0.060f}, {12.0f,  40.0f, -8.0f}, 0, false},
    {{-0.23f, 0.00f,-0.24f}, {0.150f, 0.115f, 0.055f}, {15.0f, -50.0f, 12.0f}, 0, false},
    {{ 0.00f,-0.04f,-0.27f}, {0.160f, 0.125f, 0.060f}, { 8.0f,   0.0f,  0.0f}, 0, false},
    {{ 0.23f, 0.00f,-0.24f}, {0.150f, 0.115f, 0.055f}, {15.0f,  50.0f,-12.0f}, 0, false},
    {{-0.32f, 0.10f,-0.10f}, {0.150f, 0.120f, 0.055f}, {20.0f, -60.0f, 15.0f}, 0, false},
    {{ 0.32f, 0.10f,-0.10f}, {0.150f, 0.120f, 0.055f}, {20.0f,  60.0f,-15.0f}, 0, false},
    {{-0.28f,-0.08f,-0.10f}, {0.135f, 0.110f, 0.050f}, {22.0f, -70.0f, 18.0f}, 0, false},
    {{ 0.28f,-0.08f,-0.10f}, {0.135f, 0.110f, 0.050f}, {22.0f,  70.0f,-18.0f}, 0, false},
    {{ 0.00f, 0.32f,-0.06f}, {0.170f, 0.125f, 0.060f}, {24.0f,   0.0f,  0.0f}, 2, false},
    {{ 0.00f, 0.30f,-0.16f}, {0.160f, 0.120f, 0.055f}, {16.0f,   0.0f,  0.0f}, 3, false}
};

static HairClusterState gHairClusters[sizeof(kDefaultHairClusters) / sizeof(kDefaultHairClusters[0])] = {};

static const GLfloat DIMOO_PALETTE[][3] = {
    {0.98f, 0.85f, 0.87f}, // Pearl Pink
    {0.90f, 0.88f, 0.95f}, // Lavender
    {0.82f, 0.90f, 0.82f}, // Sage
    {0.88f, 0.95f, 0.90f}, // Mint
    {0.97f, 0.95f, 0.97f}  // Pearl White
};

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

static void applyPetalMaterial(int colorIndex, bool highlight) {
    colorIndex = colorIndex % 5;
    float r = DIMOO_PALETTE[colorIndex][0];
    float g = DIMOO_PALETTE[colorIndex][1];
    float b = DIMOO_PALETTE[colorIndex][2];

    if (highlight) {
        r = saturate(r + 0.06f);
        g = saturate(g + 0.06f);
        b = saturate(b + 0.07f);
    }

    GLfloat ambient[]  = {r * 0.16f, g * 0.16f, b * 0.16f, 1.0f};
    GLfloat diffuse[]  = {r, g, b, 1.0f};
    
    // Reduce specular glare for non-highlights to prevent washing out colors
    GLfloat specular[] = {
        highlight ? 0.98f : 0.45f,
        highlight ? 0.98f : 0.45f,
        highlight ? 1.00f : 0.48f,
        1.0f
    };
    setMaterial(ambient, diffuse, specular, highlight ? 92.0f : 84.0f);
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
                        const GLfloat baseColor[3], const GLfloat tipColor[3]) {
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

            // Unit coordinates on the sphere
            float x0_u = x * zr0;
            float y0_u = y * zr0;
            float x1_u = x * zr1;
            float y1_u = y * zr1;

            // Taper along the local Y-axis (tip at positive Y, base at negative Y)
            float taper0 = 1.0f - y0_u * 0.35f;
            float taper1 = 1.0f - y1_u * 0.35f;

            // Gradient interpolation along the local Y-axis
            float factor0 = (y0_u + 1.0f) * 0.5f;
            float factor1 = (y1_u + 1.0f) * 0.5f;

            // Smoothstep curve for softer transition
            factor0 = factor0 * factor0 * (3.0f - 2.0f * factor0);
            factor1 = factor1 * factor1 * (3.0f - 2.0f * factor1);

            // Vertex 0 normal, color, and position
            glNormal3f(x0_u, y0_u, z0);
            glColor4f((baseColor[0] * (1.0f - factor0) + tipColor[0] * factor0) * 0.78f,
                      (baseColor[1] * (1.0f - factor0) + tipColor[1] * factor0) * 0.78f,
                      (baseColor[2] * (1.0f - factor0) + tipColor[2] * factor0) * 0.78f,
                      1.0f);
            glVertex3f(x0_u * rx * taper0, y0_u * ry, z0 * rz);

            // Vertex 1 normal, color, and position
            glNormal3f(x1_u, y1_u, z1);
            glColor4f((baseColor[0] * (1.0f - factor1) + tipColor[0] * factor1) * 0.78f,
                      (baseColor[1] * (1.0f - factor1) + tipColor[1] * factor1) * 0.78f,
                      (baseColor[2] * (1.0f - factor1) + tipColor[2] * factor1) * 0.78f,
                      1.0f);
            glVertex3f(x1_u * rx * taper1, y1_u * ry, z1 * rz);
        }
        glEnd();
    }
}

static void drawSphere3D(float r, int lats, int longs) {
    drawEllipsoid3D(r, r, r, lats, longs);
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

static void drawCylinderBetween(const Vec3& from, const Vec3& to,
                                float radiusBottom, float radiusTop, int slices) {
    Vec3 delta = to - from;
    float height = sqrt(dot(delta, delta));
    if (height <= 0.0001f) {
        return;
    }

    glPushMatrix();
    orientAlongSegment(from, to);
    drawCylinderY(radiusBottom, radiusTop, height, slices);
    glPopMatrix();
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
    drawSphere3D(radius, 10, slices);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);
    drawSphere3D(radius, 10, slices);
    glPopMatrix();
    glPopMatrix();
}

static void drawLeaf3D(float sx, float sy, float thickness) {
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, sy * 0.55f, thickness);
    glVertex3f(-sx * 0.46f, 0.0f, thickness);
    glVertex3f(0.0f, -sy * 0.55f, thickness);
    glVertex3f(0.0f, sy * 0.55f, thickness);
    glVertex3f(sx * 0.46f, 0.0f, thickness);
    glVertex3f(0.0f, -sy * 0.55f, thickness);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, sy * 0.55f, -thickness);
    glVertex3f(0.0f, -sy * 0.55f, -thickness);
    glVertex3f(-sx * 0.46f, 0.0f, -thickness);
    glVertex3f(0.0f, sy * 0.55f, -thickness);
    glVertex3f(0.0f, -sy * 0.55f, -thickness);
    glVertex3f(sx * 0.46f, 0.0f, -thickness);
    glEnd();
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

static void drawAdditiveDisc(float rx, float rz, float alpha,
                             float cr, float cg, float cb) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(cr, cg, cb, alpha);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 28; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / 28.0f;
        glColor4f(cr, cg, cb, 0.0f);
        glVertex3f(cos(theta) * rx, 0.0f, sin(theta) * rz);
    }
    glEnd();
}

static void drawFaceWrap(GLuint faceTex, float alpha) {
    if (alpha <= 0.001f || !faceTex) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
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

static void drawButterflyWingPlate(float sx, float sy, float thickness) {
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, thickness);
    glVertex3f(sx * 0.78f, sy * 0.64f, thickness);
    glVertex3f(sx * 0.55f, -sy * 0.22f, thickness);
    glVertex3f(0.0f, -sy * 0.06f, thickness);
    glVertex3f(sx * 0.55f, -sy * 0.22f, thickness);
    glVertex3f(sx * 0.38f, -sy * 0.72f, thickness);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, -thickness);
    glVertex3f(sx * 0.55f, -sy * 0.22f, -thickness);
    glVertex3f(sx * 0.78f, sy * 0.64f, -thickness);
    glVertex3f(0.0f, -sy * 0.06f, -thickness);
    glVertex3f(sx * 0.38f, -sy * 0.72f, -thickness);
    glVertex3f(sx * 0.55f, -sy * 0.22f, -thickness);
    glEnd();
}

static void drawButterfly3D(float wingAngle, float scale, bool glow) {
    glPushMatrix();
    glScalef(scale, scale, scale);

    GLfloat bodyAmbient[]  = {0.14f, 0.16f, 0.12f, 1.0f};
    GLfloat bodyDiffuse[]  = {0.24f, 0.28f, 0.20f, 1.0f};
    GLfloat bodySpecular[] = {0.12f, 0.14f, 0.10f, 1.0f};
    setMaterial(bodyAmbient, bodyDiffuse, bodySpecular, 18.0f);
    drawCapsuleBetween(Vec3(0.0f, -0.04f, 0.0f), Vec3(0.0f, 0.07f, 0.0f), 0.011f, 8);

    GLfloat wingAmbient[]  = {0.60f, 0.72f, 0.58f, 1.0f};
    GLfloat wingDiffuse[]  = {0.82f, 0.94f, 0.78f, 1.0f};
    GLfloat wingSpecular[] = {0.18f, 0.22f, 0.16f, 1.0f};
    if (glow) {
        wingAmbient[0] = 0.76f; wingAmbient[1] = 0.88f; wingAmbient[2] = 0.78f;
        wingDiffuse[0] = 0.94f; wingDiffuse[1] = 1.00f; wingDiffuse[2] = 0.90f;
    }
    setMaterial(wingAmbient, wingDiffuse, wingSpecular, 24.0f);

    for (int side = -1; side <= 1; side += 2) {
        float sign = (float)side;
        glPushMatrix();
        glTranslatef(sign * 0.018f, 0.02f, 0.0f);
        glRotatef(sign * wingAngle, 0.0f, 1.0f, 0.0f);

        glPushMatrix();
        glScalef(sign, 1.0f, 1.0f);
        drawButterflyWingPlate(0.080f, 0.078f, 0.006f);
        glTranslatef(0.005f, -0.018f, 0.0f);
        drawButterflyWingPlate(0.056f, 0.050f, 0.005f);
        glPopMatrix();

        glPopMatrix();
    }

    glPopMatrix();
}

static void drawFaceFeatures(const DimooVisualState& state) {
    float detail = saturate(state.faceDetail);
    drawFaceWrap(state.faceTex, detail);
}

static void drawHairCluster(const Vec3& pos, const Vec3& scale,
                            const Vec3& rot, int colorIndex,
                            bool highlightTop) {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

    // Flatten each hair chunk into a petal-like plate instead of a round puff.
    // Set scale.z multiplier to 1.40f for plump, rounded volume.
    glScalef(1.12f, 1.02f, 1.40f);
    
    // Set material specular and shininess first
    applyPetalMaterial(colorIndex, false);

    // Enable GL_COLOR_MATERIAL to combine vertex color gradients with lighting
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Base color from the palette, tip color transitions to Pearl White
    int idx = colorIndex % 5;
    const GLfloat* baseCol = DIMOO_PALETTE[idx];
    const GLfloat* tipCol = DIMOO_PALETTE[4]; // Pearl White

    // Boost saturation of the base pastel color under bright directional light
    GLfloat richBaseCol[3];
    if (idx == 4) {
        richBaseCol[0] = baseCol[0];
        richBaseCol[1] = baseCol[1];
        richBaseCol[2] = baseCol[2];
    } else {
        float minVal = baseCol[0];
        if (baseCol[1] < minVal) minVal = baseCol[1];
        if (baseCol[2] < minVal) minVal = baseCol[2];
        richBaseCol[0] = clamp(baseCol[0] - minVal * 0.28f, 0.0f, 1.0f);
        richBaseCol[1] = clamp(baseCol[1] - minVal * 0.28f, 0.0f, 1.0f);
        richBaseCol[2] = clamp(baseCol[2] - minVal * 0.28f, 0.0f, 1.0f);
    }

    drawPetal3D(scale.x, scale.y, scale.z, 12, 16, richBaseCol, tipCol);

    glDisable(GL_COLOR_MATERIAL);

    // Keep the gloss very close to the petal surface to avoid spiky intersections.
    if (highlightTop) {
        glPushMatrix();
        glTranslatef(0.0f, scale.y * 0.06f, scale.z * 0.015f);
        glScalef(0.94f, 0.34f, 0.18f);
        applyPetalMaterial(4, true);
        drawEllipsoid3D(scale.x, scale.y, scale.z, 10, 14);
        glPopMatrix();
    }

    glPopMatrix();
}

static void drawHairClusters(const DimooVisualState& state, float t, float moveLean) {
    float sway = sin(t * 2.2f) * (1.0f + state.moveBlend * 0.8f);
    const int hairCount = (int)(sizeof(gHairClusters) / sizeof(gHairClusters[0]));
    for (int i = 0; i < hairCount; ++i) {
        Vec3 p(gHairClusters[i].pos[0], gHairClusters[i].pos[1], gHairClusters[i].pos[2]);
        Vec3 s(gHairClusters[i].scale[0], gHairClusters[i].scale[1], gHairClusters[i].scale[2]);
        Vec3 r(gHairClusters[i].rot[0], gHairClusters[i].rot[1], gHairClusters[i].rot[2]);

        p.y += sin(t * 2.0f + i * 0.37f) * 0.004f;
        p.x += state.moveBlend * ((i % 2 == 0) ? -0.006f : 0.006f);
        r.z += sway * ((i < 5) ? 0.28f : ((i < 14) ? 0.20f : 0.16f)) + moveLean * 0.10f;

        if (i < 5) {
            r.x += gHairFrontTilt;
        } else {
            // Apply top tilt to all other clusters (top and back)
            r.x += gHairTopTilt;
        }

        // Side spread for left-side clusters (including new back-left clusters)
        if (i == 0 || i == 5 || i == 9 || i == 14 || i == 15 || i == 18 || i == 21 || i == 24 || i == 26) {
            r.y -= gHairSideSpread;
            r.z += gHairSideSpread * 0.35f;
        } 
        // Side spread for right-side clusters (including new back-right clusters)
        else if (i == 4 || i == 8 || i == 12 || i == 16 || i == 17 || i == 20 || i == 23 || i == 25 || i == 27) {
            r.y += gHairSideSpread;
            r.z -= gHairSideSpread * 0.35f;
        }

        drawHairCluster(p, s, r, i % 5, gHairClusters[i].highlightTop);
    }
}

static void drawHeadAndFace(const DimooVisualState& state, float t, float moveLean) {
    setPearlMaterial(1.0f, 0.94f, 0.92f, 60.0f);
    drawEllipsoid3D(0.37f, 0.37f, 0.35f, 18, 24);

    drawFaceFeatures(state);
    drawHairClusters(state, t, moveLean);
}

static void drawMascotBody(const DimooVisualState& state, float t) {
    float handBob = sin(t * 2.8f) * 0.010f;
    float attackReach = state.attackPulse * 0.07f;

    setPearlMaterial(0.98f, 0.95f, 0.94f, 54.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    drawEllipsoid3D(0.38f, 0.38f, 0.36f, 18, 24);
    glPopMatrix();

    setPearlMaterial(0.99f, 0.91f, 0.90f, 42.0f);
    glPushMatrix();
    glTranslatef(-0.36f, 0.35f + handBob, 0.07f);
    glRotatef(26.0f, 0.0f, 0.0f, 1.0f);
    drawEllipsoid3D(0.070f, 0.052f, 0.055f, 10, 14);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.35f + attackReach, 0.34f - handBob * 0.6f, 0.11f + state.attackPulse * 0.03f);
    glRotatef(-18.0f - state.attackPulse * 18.0f, 0.0f, 0.0f, 1.0f);
    drawEllipsoid3D(0.074f, 0.054f, 0.058f, 10, 14);
    glPopMatrix();
}

static void drawVineRing(const DimooVisualState& state, float t) {
    const int nodeCount = 24;
    std::vector<Vec3> nodes;
    nodes.reserve(nodeCount);

    float drag = state.moveBlend * 0.06f;
    for (int i = 0; i < nodeCount; ++i) {
        float angle = 2.0f * (float)M_PI * (float)i / (float)nodeCount;
        float radiusNoise = sin(angle * 3.2f + 0.8f) * 0.07f
                          + cos(angle * 5.4f + 1.3f) * 0.045f
                          + sin(t * 0.9f + i * 0.6f) * 0.010f;
        float radius = 0.63f + radiusNoise;
        float yScale = 1.00f + 0.12f * sin(angle * 2.0f + 0.5f);
        float zOffset = sin(angle * 3.5f + 0.2f) * 0.11f
                      + cos(angle * 6.0f + t * 0.8f) * 0.03f
                      - cos(angle) * drag;
        float x = cos(angle) * radius;
        float y = 0.46f + sin(angle) * radius * yScale;
        nodes.push_back(Vec3(x, y, zOffset));
    }

    GLfloat barkAmbient[]  = {0.16f, 0.12f, 0.08f, 1.0f};
    GLfloat barkDiffuse[]  = {0.36f, 0.27f, 0.16f, 1.0f};
    GLfloat barkSpecular[] = {0.07f, 0.05f, 0.03f, 1.0f};
    setMaterial(barkAmbient, barkDiffuse, barkSpecular, 9.0f);

    for (int i = 0; i < nodeCount; ++i) {
        int next = (i + 1) % nodeCount;
        float radiusA = 0.036f + 0.012f * pseudo01((float)i * 0.63f + 0.7f);
        float radiusB = 0.036f + 0.012f * pseudo01((float)next * 0.63f + 0.7f);
        drawCylinderBetween(nodes[i], nodes[next], radiusA, radiusB, 9);
        glPushMatrix();
        glTranslatef(nodes[i].x, nodes[i].y, nodes[i].z);
        drawSphere3D(radiusA * 0.92f, 8, 8);
        glPopMatrix();
    }

    GLfloat leafAmbient[]  = {0.16f, 0.20f, 0.10f, 1.0f};
    GLfloat leafDiffuse[]  = {0.34f, 0.45f, 0.20f, 1.0f};
    GLfloat leafSpecular[] = {0.08f, 0.10f, 0.05f, 1.0f};
    static const int branchIndices[] = {2, 6, 10, 15, 20};
    for (int k = 0; k < 5; ++k) {
        int idx = branchIndices[k];
        Vec3 base = nodes[idx];
        Vec3 tangent = normalize(nodes[(idx + 1) % nodeCount] - nodes[(idx + nodeCount - 1) % nodeCount]);
        Vec3 branchDir = normalize(Vec3(tangent.x * 0.32f + cos((float)idx * 1.4f) * 0.72f,
                                        0.36f + 0.06f * (float)(k % 3),
                                        tangent.z * 0.20f + sin((float)idx * 0.9f) * 0.68f));
        Vec3 mid = base + branchDir * (0.09f + 0.02f * (float)(k % 2));
        Vec3 tip = base + branchDir * (0.15f + 0.03f * pseudo01((float)k * 0.9f));

        setMaterial(barkAmbient, barkDiffuse, barkSpecular, 9.0f);
        drawCylinderBetween(base, mid, 0.024f, 0.014f, 8);
        drawCylinderBetween(mid, tip, 0.014f, 0.008f, 8);

        setMaterial(leafAmbient, leafDiffuse, leafSpecular, 16.0f);
        for (int leaf = 0; leaf < ((k % 2 == 0) ? 2 : 1); ++leaf) {
            float along = 0.55f + 0.22f * leaf;
            Vec3 leafPos = base + branchDir * ((0.12f + 0.04f * leaf) * along);
            glPushMatrix();
            glTranslatef(leafPos.x, leafPos.y, leafPos.z);
            glRotatef((float)(idx * 21 + leaf * 47), 0.0f, 1.0f, 0.0f);
            glRotatef((leaf % 2 == 0) ? 34.0f : -26.0f, 0.0f, 0.0f, 1.0f);
            glRotatef((float)(8 + k * 4), 1.0f, 0.0f, 0.0f);
            drawLeaf3D(0.08f + 0.01f * leaf, 0.11f + 0.01f * (k % 3), 0.0035f);
            glPopMatrix();
        }
    }

    if (state.skillPulse > 0.01f) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < nodeCount; ++i) {
            glColor4f(0.70f, 0.96f, 0.78f, 0.10f + state.skillPulse * 0.14f);
            glVertex3f(nodes[i].x * 1.03f, nodes[i].y * 1.02f, nodes[i].z);
        }
        glEnd();
        glPopAttrib();
    }
}

static void drawDreamParticleSwarm(const DimooVisualState& state, float t) {
    int particleCount = 8 + (int)(state.skillPulse * 4.0f);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int i = 0; i < particleCount; ++i) {
        float seed = 0.37f + (float)i * 1.618f;
        float life = fmod(t * (0.22f + 0.01f * (float)(i % 4)) + pseudo01(seed), 1.0f);
        float spread = 0.08f + 0.12f * pseudo01(seed * 0.8f) + state.skillPulse * 0.04f;
        float theta = seed * 3.4f + t * (0.55f + 0.04f * (float)(i % 5));
        float wobble = sin(t * 2.1f + seed * 5.0f) * 0.030f;
        float x = cos(theta) * spread + wobble;
        float y = 0.20f + life * (0.68f + state.skillPulse * 0.18f);
        float z = sin(theta * 1.3f) * spread * 0.50f;
        float alpha = (1.0f - life) * (0.14f + state.skillPulse * 0.10f);
        float size = 2.0f + 1.2f * pseudo01(seed * 2.0f + t);
        float green = (i % 3 == 0) ? 0.95f : 1.0f;

        glColor4f(0.74f, green, 0.86f, alpha * 0.40f);
        glPointSize(size * 1.9f);
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();

        glColor4f(0.98f, 1.0f, 0.98f, alpha);
        glPointSize(size);
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();
    }

    glPointSize(1.0f);
    glPopAttrib();
}

static void drawSkillAura(float skillPulse, float t) {
    if (skillPulse <= 0.01f) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float ring = 0.34f + skillPulse * 0.12f + sin(t * 5.5f) * 0.02f;
    glPushMatrix();
    glTranslatef(0.0f, 0.12f, 0.0f);
    drawAdditiveDisc(ring, ring * 0.58f, 0.08f + skillPulse * 0.08f, 0.72f, 0.98f, 0.82f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    drawAdditiveDisc(ring * 0.58f, ring * 0.30f, 0.06f + skillPulse * 0.08f, 0.88f, 1.0f, 0.94f);
    glPopMatrix();

    glPopAttrib();
}

static void drawButterflySystem(const DimooVisualState& state, float t) {
    float bodyFloat = sin(t * 2.0f) * 0.010f;
    float perchWing = 12.0f + 8.0f * sin(t * 4.0f);
    glPushMatrix();
    glTranslatef(0.17f, 0.80f + bodyFloat, 0.23f);
    glRotatef(12.0f * sin(t * 2.0f), 0.0f, 1.0f, 0.0f);
    drawButterfly3D(perchWing, 0.78f, false);
    glPopMatrix();

    if (state.attackPulse > 0.02f) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        float trail = 0.22f + state.attackPulse * 0.16f;
        glBegin(GL_TRIANGLE_STRIP);
        glColor4f(0.80f, 1.0f, 0.88f, 0.16f * state.attackPulse);
        glVertex3f(0.18f, 0.62f, 0.10f);
        glVertex3f(0.18f, 0.67f, 0.10f);
        glColor4f(0.96f, 1.0f, 0.98f, 0.0f);
        glVertex3f(trail, 0.64f, 0.08f);
        glVertex3f(trail + 0.05f, 0.69f, 0.05f);
        glEnd();
        glPopAttrib();

        glPushMatrix();
        glTranslatef(trail + 0.01f, 0.67f, 0.06f);
        glRotatef(-8.0f, 0.0f, 1.0f, 0.0f);
        drawButterfly3D(30.0f + 10.0f * sin(t * 22.0f), 0.78f, true);
        glPopMatrix();
    }
}

void draw(const DimooVisualState& state) {
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
    float floatLift = sin(t * 2.2f) * (0.034f + state.skillPulse * 0.010f);
    float moveDrift = walk * 0.014f * state.moveBlend;
    float moveLean = walk * 4.0f * state.moveBlend;
    float bodyBreath = 1.0f + sin(t * 2.0f) * 0.014f;

    glTranslatef(moveDrift, floatLift, 0.0f);
    glRotatef(moveLean, 0.0f, 0.0f, 1.0f);
    glScalef(bodyBreath, bodyBreath + state.moveBlend * 0.025f, bodyBreath);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0.0f, -0.02f - floatLift * 0.35f, 0.0f);
    drawSoftShadow(0.36f - floatLift * 0.15f, 0.22f - floatLift * 0.10f, 0.18f - floatLift * 0.40f);
    glPopMatrix();
    glPopAttrib();

    drawSkillAura(state.skillPulse, t);
    drawVineRing(state, t);

    drawMascotBody(state, t);

    glPushMatrix();
    glTranslatef(0.0f, 0.45f + 0.008f * fabs(walk) * state.moveBlend, 0.0f);
    drawHeadAndFace(state, t, moveLean);
    glPopMatrix();

    drawButterflySystem(state, t);
    drawDreamParticleSwarm(state, t);

    glEnable(GL_COLOR_MATERIAL);
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
