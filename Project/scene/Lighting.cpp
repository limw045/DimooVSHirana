#include "Lighting.h"

// 主光源位置（定向光，从前方斜射入盒子内部）
static GLfloat mainLightPos[] = { 1.5f, 6.0f, 4.0f, 0.0f }; // w=0.0 表示无限远定向光

Lighting::Lighting() {}
Lighting::~Lighting() {}

void Lighting::init() {
    // 1. 设置主光源 (GL_LIGHT0 - 暖白光)
    GLfloat mainAmbient[]  = { 0.25f, 0.24f, 0.22f, 1.0f };
    GLfloat mainDiffuse[]  = { 0.85f, 0.83f, 0.78f, 1.0f };
    GLfloat mainSpecular[] = { 0.50f, 0.50f, 0.45f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  mainAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  mainDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, mainSpecular);
    glEnable(GL_LIGHT0);

    // 2. 设置小野的跟随光源 (GL_LIGHT1 - 金黄色点光源)
    GLfloat hironoAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat hironoDiffuse[]  = { 1.0f, 0.85f, 0.2f, 1.0f };
    GLfloat hironoSpecular[] = { 1.0f, 0.85f, 0.2f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT,  hironoAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  hironoDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, hironoSpecular);
    // 设置二次距离衰减，控制点光源范围
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  0.5f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.2f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3f);
    glEnable(GL_LIGHT1);

    // 3. 设置 Dimoo 的跟随光源 (GL_LIGHT2 - 蓝白色点光源)
    GLfloat dimooAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat dimooDiffuse[]  = { 0.3f, 0.6f, 1.0f, 1.0f };
    GLfloat dimooSpecular[] = { 0.3f, 0.6f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT2, GL_AMBIENT,  dimooAmbient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  dimooDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, dimooSpecular);
    // 设置二次距离衰减
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,  0.5f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.2f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2f);
    glEnable(GL_LIGHT2);

    // 4. 全局材质属性与法线归一化设置
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    GLfloat specRef[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specRef);
    glMateriali(GL_FRONT, GL_SHININESS, 64); // 高光反光系数

    glEnable(GL_NORMALIZE); // 缩放物体时自动重算并规范化法向量
    glEnable(GL_LIGHTING);  // 启用光照系统
}

void Lighting::applyMainLight() {
    glLightfv(GL_LIGHT0, GL_POSITION, mainLightPos);
}

void Lighting::updateCharacterLights(float hX, float hY, float hZ, 
                                     float dX, float dY, float dZ, 
                                     bool hpLow) {
    // 更新点光源位置 (跟随角色胸部位置，Y值加0.3f)
    GLfloat hPos[] = { hX, hY + 0.3f, hZ, 1.0f }; // w=1.0 表示位置点光源
    glLightfv(GL_LIGHT1, GL_POSITION, hPos);

    GLfloat dPos[] = { dX, dY + 0.3f, dZ, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, dPos);

    // 动态氛围灯效：当任意一方血量低于40% (hpLow) 时，降低主环境光源亮度，突出点光源
    if (hpLow) {
        GLfloat dimAmbient[] = { 0.08f, 0.08f, 0.1f, 1.0f };
        GLfloat dimDiffuse[] = { 0.40f, 0.38f, 0.38f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, dimAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dimDiffuse);
    } else {
        GLfloat normalAmbient[]  = { 0.25f, 0.24f, 0.22f, 1.0f };
        GLfloat normalDiffuse[]  = { 0.85f, 0.83f, 0.78f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, normalAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, normalDiffuse);
    }
}

void Lighting::toggleLighting(bool enable) {
    if (enable) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }
}
