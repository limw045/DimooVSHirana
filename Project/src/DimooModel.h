#ifndef DIMOO_MODEL_H
#define DIMOO_MODEL_H

#include "Common.hpp"

namespace DimooModel {

struct HairClusterState {
    float pos[3];
    float scale[3];
    float rot[3];
    int colorIndex;
    bool highlightTop;
};

struct DimooVisualState {
    float x, y, z;
    bool facingRight;
    float time;
    float moveBlend;
    float attackPulse;
    float skillPulse;
    float ultPulse;
    float faceDetail;
    GLuint faceTex;

    DimooVisualState()
        : x(0.0f), y(0.0f), z(0.0f),
          facingRight(true),
          time(0.0f),
          moveBlend(0.0f),
          attackPulse(0.0f),
          skillPulse(0.0f),
          ultPulse(0.0f),
          faceDetail(1.0f),
          faceTex(0) {}
};

void draw(const DimooVisualState& state);
void drawButterfly3D(float wingAngle, float scale, bool glow, float alpha = 1.0f, float r = 0.82f, float g = 0.94f, float b = 0.78f);
void setFaceTextureTuning(float uOffset, float vOffset, float uScale, float vScale);
void resetFaceTextureTuning();
void setHairAngleTuning(float frontTilt, float topTilt, float sideSpread);
void resetHairAngleTuning();
int getHairClusterCount();
HairClusterState getHairClusterState(int index);
void setHairClusterState(int index, const HairClusterState& state);
void resetHairClusterStates();

}

#endif // DIMOO_MODEL_H
