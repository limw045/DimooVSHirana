#ifndef HIRONO_MODEL_H
#define HIRONO_MODEL_H

#include "Common.hpp"

namespace HironoModel {

struct HairClusterState {
    float pos[3];
    float scale[3];
    float rot[3];
    int colorIndex;
    bool highlightTop;
};

struct HironoVisualState {
    float x, y, z;
    bool facingRight;
    float time;
    float moveBlend;
    float attackPulse;
    float skillPulse;
    float ultPulse;
    float faceDetail;
    GLuint faceTex;
    bool showBase;

    HironoVisualState()
        : x(0.0f), y(0.0f), z(0.0f),
          facingRight(true),
          time(0.0f),
          moveBlend(0.0f),
          attackPulse(0.0f),
          skillPulse(0.0f),
          ultPulse(0.0f),
          faceDetail(1.0f),
          faceTex(0),
          showBase(true) {}
};

void draw(const HironoVisualState& state);
void setFaceTextureTuning(float uOffset, float vOffset, float uScale, float vScale);
void resetFaceTextureTuning();
void setHairAngleTuning(float frontTilt, float topTilt, float sideSpread);
void resetHairAngleTuning();
int getHairClusterCount();
HairClusterState getHairClusterState(int index);
void setHairClusterState(int index, const HairClusterState& state);
void resetHairClusterStates();

}

#endif // HIRONO_MODEL_H
