#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "../Common.hpp"

class Lighting {
public:
    Lighting();
    ~Lighting();

    void init();
    
    // 应用主环境光源位置（应在每帧设置好摄像机 Modelview 矩阵后调用）
    void applyMainLight();

    // 更新角色跟随点光源位置
    void updateCharacterLights(float hX, float hY, float hZ, 
                               float dX, float dY, float dZ, 
                               bool hpLow);

    // 禁用/开启光源
    void toggleLighting(bool enable);
};

#endif // LIGHTING_HPP
