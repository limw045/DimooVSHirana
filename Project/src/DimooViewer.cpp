#include "DimooViewer.h"

#include "CGImageLoader.hpp"
#include "DimooModel.h"

#include "../imgui-1.92.8/imgui.h"
#include "../imgui-1.92.8/backends/imgui_impl_glut.h"
#include "../imgui-1.92.8/backends/imgui_impl_opengl2.h"

#include <cstring>
#include <string>
#include <vector>
#include <direct.h>

#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>
#include <float.h>
#endif

namespace DimooViewerApp {
namespace {

static const float kViewerDefaultFaceUOffset = -0.008f;
static const float kViewerDefaultFaceVOffset = -0.002f;
static const float kViewerDefaultFaceUScale = 1.800f;
static const float kViewerDefaultFaceVScale = 1.800f;
static const float kViewerDefaultHairFrontTilt = 0.0f;
static const float kViewerDefaultHairTopTilt = 0.0f;
static const float kViewerDefaultHairSideSpread = 0.0f;

struct ViewerState {
    int width;
    int height;
    float lastTime;
    float elapsed;
    float timeScale;
    float yaw;
    float pitch;
    float distance;
    float lookY;
    float fov;
    float modelScale;
    float lightYaw;
    float lightHeight;
    float lightDistance;
    float clearColor[3];
    bool autoAnimate;
    bool useFaceTexture;
    bool showGrid;
    bool showReferenceBall;
    bool showImguiDemo;
    float faceUOffset;
    float faceVOffset;
    float faceUScale;
    float faceVScale;
    float hairFrontTilt;
    float hairTopTilt;
    float hairSideSpread;
    bool showHairClusterEditor;
    std::string faceTexturePath;
    DimooModel::DimooVisualState visual;
    GLuint faceTex;

    ViewerState()
        : width(1280),
          height(800),
          lastTime(0.0f),
          elapsed(0.0f),
          timeScale(1.0f),
          yaw(18.0f),
          pitch(10.0f),
          distance(2.8f),
          lookY(0.52f),
          fov(42.0f),
          modelScale(1.9f),
          lightYaw(38.0f),
          lightHeight(1.7f),
          lightDistance(3.5f),
          autoAnimate(true),
          useFaceTexture(true),
          showGrid(true),
          showReferenceBall(false),
          showImguiDemo(false),
          faceUOffset(kViewerDefaultFaceUOffset),
          faceVOffset(kViewerDefaultFaceVOffset),
          faceUScale(kViewerDefaultFaceUScale),
          faceVScale(kViewerDefaultFaceVScale),
          hairFrontTilt(kViewerDefaultHairFrontTilt),
          hairTopTilt(kViewerDefaultHairTopTilt),
          hairSideSpread(kViewerDefaultHairSideSpread),
          showHairClusterEditor(false),
          faceTexturePath("not loaded"),
          faceTex(0) {
        clearColor[0] = 0.05f;
        clearColor[1] = 0.07f;
        clearColor[2] = 0.10f;
        visual.x = 0.0f;
        visual.y = 0.0f;
        visual.z = 0.0f;
        visual.facingRight = true;
        visual.time = 0.0f;
        visual.moveBlend = 0.0f;
        visual.attackPulse = 0.0f;
        visual.skillPulse = 0.0f;
        visual.faceDetail = 1.0f;
        visual.faceTex = 0;
    }
};

ViewerState gViewer;

#ifdef _WIN32
ULONG_PTR gGdiToken = 0;
#endif

static float degToRad(float degrees) {
    return degrees * (float)M_PI / 180.0f;
}

static bool endsWithInsensitive(const std::string& value, const char* suffix) {
    std::string a = value;
    std::string b = suffix;
    std::transform(a.begin(), a.end(), a.begin(), ::tolower);
    std::transform(b.begin(), b.end(), b.begin(), ::tolower);
    if (a.size() < b.size()) {
        return false;
    }
    return a.compare(a.size() - b.size(), b.size(), b) == 0;
}

static std::string normalizeSlashes(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

static std::string directoryOf(const std::string& path) {
    std::string normalized = normalizeSlashes(path);
    size_t slash = normalized.find_last_of('/');
    if (slash == std::string::npos) {
        return ".";
    }
    return normalized.substr(0, slash);
}

static bool fileExists(const std::string& path) {
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}

static std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) {
        return b;
    }
    if (a[a.size() - 1] == '/' || a[a.size() - 1] == '\\') {
        return a + b;
    }
    return a + "/" + b;
}

static std::string getExecutableDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH] = {0};
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return ".";
    }
    return directoryOf(std::string(buffer, buffer + len));
#else
    return ".";
#endif
}

static std::string getCurrentDirectoryPath() {
    char buffer[2048] = {0};
    if (_getcwd(buffer, sizeof(buffer)) == NULL) {
        return ".";
    }
    return normalizeSlashes(std::string(buffer));
}

#ifdef _WIN32
static std::wstring widen(const std::string& text) {
    if (text.empty()) {
        return std::wstring();
    }
    int count = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
    if (count <= 0) {
        count = MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, NULL, 0);
        if (count <= 0) {
            return std::wstring();
        }
        std::wstring wide(count - 1, L'\0');
        MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, &wide[0], count);
        return wide;
    }
    std::wstring wide(count - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wide[0], count);
    return wide;
}

static GLuint uploadRgbTexture(int width, int height, const unsigned char* pixels) {
    if (!pixels || width <= 0 || height <= 0) {
        return 0;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

static GLuint loadTextureWithGdiPlus(const std::string& path) {
    std::wstring widePath = widen(path);
    if (widePath.empty()) {
        return 0;
    }

    Gdiplus::Bitmap bitmap(widePath.c_str());
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        return 0;
    }

    Gdiplus::Rect rect(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
    Gdiplus::BitmapData locked = {};
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &locked) != Gdiplus::Ok) {
        return 0;
    }

    const int width = (int)bitmap.GetWidth();
    const int height = (int)bitmap.GetHeight();
    std::vector<unsigned char> rgb(width * height * 3);

    for (int y = 0; y < height; ++y) {
        const unsigned char* srcRow = (const unsigned char*)locked.Scan0 + y * locked.Stride;
        unsigned char* dstRow = &rgb[(height - 1 - y) * width * 3];
        for (int x = 0; x < width; ++x) {
            const unsigned char b = srcRow[x * 4 + 0];
            const unsigned char g = srcRow[x * 4 + 1];
            const unsigned char r = srcRow[x * 4 + 2];
            dstRow[x * 3 + 0] = r;
            dstRow[x * 3 + 1] = g;
            dstRow[x * 3 + 2] = b;
        }
    }

    bitmap.UnlockBits(&locked);
    return uploadRgbTexture(width, height, &rgb[0]);
}
#endif

static void addFaceTextureCandidatesFromBase(std::vector<std::string>* candidates,
                                             const std::string& baseDir) {
    if (!candidates || baseDir.empty()) {
        return;
    }

    std::string dir = normalizeSlashes(baseDir);
    for (int level = 0; level < 6 && !dir.empty(); ++level) {
        candidates->push_back(joinPath(dir, "images/dimoo_face.bmp"));
        size_t slash = dir.find_last_of('/');
        if (slash == std::string::npos) {
            break;
        }
        dir = dir.substr(0, slash);
    }
}

static GLuint loadTextureFile(const std::string& path) {
    MyImage image;
    std::vector<char> fileName(path.begin(), path.end());
    fileName.push_back('\0');

    if (endsWithInsensitive(path, ".bmp")) {
        image.loadBMP(fileName.data());
    } else {
        image.loadJPG(fileName.data());
    }

    if (!image.buffer || image.width == 0 || image.height == 0) {
        #ifdef _WIN32
        return loadTextureWithGdiPlus(path);
        #else
        return 0;
        #endif
    }

    return uploadRgbTexture((int)image.width, (int)image.height, image.buffer);
}

static GLuint loadFaceTexture(std::string* resolvedPath) {
    std::vector<std::string> candidates;
    addFaceTextureCandidatesFromBase(&candidates, getCurrentDirectoryPath());
    addFaceTextureCandidatesFromBase(&candidates, getExecutableDirectory());
    addFaceTextureCandidatesFromBase(&candidates, directoryOf(__FILE__));
    candidates.push_back("F:/Degree/Last Sem/TCG/Project/images/dimoo_face.bmp");

    for (size_t i = 0; i < candidates.size(); ++i) {
        const std::string candidate = normalizeSlashes(candidates[i]);
        if (!fileExists(candidate)) {
            continue;
        }
        GLuint tex = loadTextureFile(candidate);
        if (tex != 0) {
            if (resolvedPath) {
                *resolvedPath = candidate;
            }
            return tex;
        }
    }

    if (resolvedPath) {
        *resolvedPath = "dimoo_face.bmp not found via cwd/exe/source parents";
    }
    return 0;
}

static void setupCamera() {
    const float yawRad = degToRad(gViewer.yaw);
    const float pitchRad = degToRad(gViewer.pitch);
    const float horizontal = cos(pitchRad) * gViewer.distance;
    const float eyeX = sin(yawRad) * horizontal;
    const float eyeY = gViewer.lookY + sin(pitchRad) * gViewer.distance;
    const float eyeZ = cos(yawRad) * horizontal;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(gViewer.fov, (float)gViewer.width / (float)std::max(1, gViewer.height), 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0f, gViewer.lookY, 0.0f, 0.0f, 1.0f, 0.0f);
}

static void setupLighting() {
    const float lightYawRad = degToRad(gViewer.lightYaw);
    GLfloat ambient[] = {0.23f, 0.23f, 0.25f, 1.0f};
    GLfloat diffuse[] = {0.95f, 0.94f, 0.92f, 1.0f};
    GLfloat specular[] = {0.78f, 0.78f, 0.80f, 1.0f};
    GLfloat position[] = {
        (GLfloat)(sin(lightYawRad) * gViewer.lightDistance),
        gViewer.lightHeight,
        (GLfloat)(cos(lightYawRad) * gViewer.lightDistance),
        1.0f
    };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

static void drawFloorGrid() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; ++i) {
        float t = (float)i * 0.25f;
        glColor4f(0.20f, 0.28f, 0.30f, i == 0 ? 0.75f : 0.35f);
        glVertex3f(-2.5f, 0.0f, t);
        glVertex3f(2.5f, 0.0f, t);
        glVertex3f(t, 0.0f, -2.5f);
        glVertex3f(t, 0.0f, 2.5f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawReferenceBall() {
    GLfloat ambient[] = {0.08f, 0.10f, 0.12f, 1.0f};
    GLfloat diffuse[] = {0.25f, 0.35f, 0.42f, 1.0f};
    GLfloat specular[] = {0.18f, 0.20f, 0.22f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);

    glPushMatrix();
    glTranslatef(0.95f, 0.38f, -0.65f);
    glutSolidSphere(0.12, 18, 18);
    glPopMatrix();
}

static void updateAnimation(float dt) {
    if (gViewer.autoAnimate) {
        gViewer.elapsed += dt * gViewer.timeScale;
        gViewer.visual.time = gViewer.elapsed;
    }
}

static void drawScene() {
    glViewport(0, 0, gViewer.width, gViewer.height);
    glClearColor(gViewer.clearColor[0], gViewer.clearColor[1], gViewer.clearColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    setupCamera();
    setupLighting();

    if (gViewer.showGrid) {
        drawFloorGrid();
    }

    if (gViewer.showReferenceBall) {
        drawReferenceBall();
    }

    gViewer.visual.faceTex = gViewer.useFaceTexture ? gViewer.faceTex : 0;
    DimooModel::setFaceTextureTuning(gViewer.faceUOffset, gViewer.faceVOffset, gViewer.faceUScale, gViewer.faceVScale);
    DimooModel::setHairAngleTuning(gViewer.hairFrontTilt, gViewer.hairTopTilt, gViewer.hairSideSpread);

    glPushMatrix();
    glScalef(gViewer.modelScale, gViewer.modelScale, gViewer.modelScale);
    DimooModel::draw(gViewer.visual);
    glPopMatrix();
}

static void drawInspectorUi() {
    ImGui::Begin("Dimoo Viewer");
    ImGui::Text("Standalone model inspector");
    ImGui::Text("Face texture: %s", gViewer.faceTex ? "loaded" : "missing");
    ImGui::TextWrapped("Path: %s", gViewer.faceTexturePath.c_str());
    ImGui::Checkbox("Use face texture", &gViewer.useFaceTexture);
    ImGui::Checkbox("Auto animate", &gViewer.autoAnimate);
    ImGui::Checkbox("Facing right", &gViewer.visual.facingRight);
    ImGui::Checkbox("Show grid", &gViewer.showGrid);
    ImGui::Checkbox("Show reference ball", &gViewer.showReferenceBall);
    ImGui::Checkbox("Show ImGui demo", &gViewer.showImguiDemo);

    ImGui::SeparatorText("Animation");
    ImGui::SliderFloat("Time", &gViewer.visual.time, 0.0f, 30.0f);
    ImGui::SliderFloat("Time scale", &gViewer.timeScale, 0.0f, 3.0f);
    ImGui::SliderFloat("Move blend", &gViewer.visual.moveBlend, 0.0f, 1.0f);
    ImGui::SliderFloat("Attack pulse", &gViewer.visual.attackPulse, 0.0f, 1.0f);
    ImGui::SliderFloat("Skill pulse", &gViewer.visual.skillPulse, 0.0f, 1.0f);
    ImGui::SliderFloat("Face detail", &gViewer.visual.faceDetail, 0.0f, 1.0f);

    ImGui::SeparatorText("Face Wrap");
    ImGui::SliderFloat("Face U Offset", &gViewer.faceUOffset, -0.30f, 0.30f);
    ImGui::SliderFloat("Face V Offset", &gViewer.faceVOffset, -0.30f, 0.30f);
    ImGui::SliderFloat("Face U Scale", &gViewer.faceUScale, 0.35f, 1.80f);
    ImGui::SliderFloat("Face V Scale", &gViewer.faceVScale, 0.35f, 1.80f);

    ImGui::SeparatorText("Hair Angles");
    ImGui::SliderFloat("Hair Front Tilt", &gViewer.hairFrontTilt, -35.0f, 35.0f);
    ImGui::SliderFloat("Hair Top Tilt", &gViewer.hairTopTilt, -35.0f, 35.0f);
    ImGui::SliderFloat("Hair Side Spread", &gViewer.hairSideSpread, -30.0f, 30.0f);
    ImGui::Checkbox("Edit hair clusters", &gViewer.showHairClusterEditor);

    if (gViewer.showHairClusterEditor) {
        const int hairCount = DimooModel::getHairClusterCount();
        for (int i = 0; i < hairCount; ++i) {
            DimooModel::HairClusterState hair = DimooModel::getHairClusterState(i);
            char label[32];
            sprintf(label, "Hair %02d", i);
            if (ImGui::TreeNode(label)) {
                ImGui::DragFloat3("Pos", hair.pos, 0.0025f, -0.60f, 0.60f);
                ImGui::DragFloat3("Scale", hair.scale, 0.0020f, 0.005f, 0.35f);
                ImGui::DragFloat3("Rot", hair.rot, 0.5f, -180.0f, 180.0f);
                // ImGui::SliderInt("Color", &hair.colorIndex, 0, 4);
                ImGui::Checkbox("Highlight", &hair.highlightTop);
                DimooModel::setHairClusterState(i, hair);
                ImGui::TreePop();
            }
        }
        if (ImGui::Button("Reset hair clusters")) {
            DimooModel::resetHairClusterStates();
        }
    }

    ImGui::SeparatorText("Camera");
    ImGui::SliderFloat("Yaw", &gViewer.yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &gViewer.pitch, -45.0f, 45.0f);
    ImGui::SliderFloat("Distance", &gViewer.distance, 1.2f, 6.0f);
    ImGui::SliderFloat("Look Y", &gViewer.lookY, -0.2f, 1.4f);
    ImGui::SliderFloat("FOV", &gViewer.fov, 20.0f, 75.0f);
    ImGui::SliderFloat("Model scale", &gViewer.modelScale, 0.7f, 3.5f);

    ImGui::SeparatorText("Lighting");
    ImGui::SliderFloat("Light yaw", &gViewer.lightYaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Light height", &gViewer.lightHeight, 0.2f, 4.0f);
    ImGui::SliderFloat("Light distance", &gViewer.lightDistance, 1.0f, 7.0f);
    ImGui::ColorEdit3("Background", gViewer.clearColor);

    if (ImGui::Button("Front view")) {
        gViewer.yaw = 0.0f;
        gViewer.pitch = 8.0f;
        gViewer.distance = 2.8f;
        gViewer.visual.facingRight = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Side view")) {
        gViewer.yaw = 90.0f;
        gViewer.pitch = 8.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        gViewer = ViewerState();
        gViewer.faceTex = loadFaceTexture(&gViewer.faceTexturePath);
        DimooModel::resetFaceTextureTuning();
        DimooModel::resetHairAngleTuning();
        DimooModel::resetHairClusterStates();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset face wrap")) {
        gViewer.faceUOffset = kViewerDefaultFaceUOffset;
        gViewer.faceVOffset = kViewerDefaultFaceVOffset;
        gViewer.faceUScale = kViewerDefaultFaceUScale;
        gViewer.faceVScale = kViewerDefaultFaceVScale;
        DimooModel::resetFaceTextureTuning();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset hair angles")) {
        gViewer.hairFrontTilt = kViewerDefaultHairFrontTilt;
        gViewer.hairTopTilt = kViewerDefaultHairTopTilt;
        gViewer.hairSideSpread = kViewerDefaultHairSideSpread;
        DimooModel::resetHairAngleTuning();
    }

    if (ImGui::Button("Reload face texture")) {
        if (gViewer.faceTex) {
            glDeleteTextures(1, &gViewer.faceTex);
            gViewer.faceTex = 0;
        }
        gViewer.faceTex = loadFaceTexture(&gViewer.faceTexturePath);
    }

    if (gViewer.faceTex) {
        ImGui::SeparatorText("Texture Preview");
        ImGui::Image((ImTextureID)(intptr_t)gViewer.faceTex, ImVec2(128.0f, 128.0f));
    }

    ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    if (gViewer.showImguiDemo) {
        ImGui::ShowDemoWindow(&gViewer.showImguiDemo);
    }
}

} // namespace

void init() {
#ifdef _WIN32
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gGdiToken, &gdiplusStartupInput, NULL);
    _fpreset();
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();

    gViewer.faceTex = loadFaceTexture(&gViewer.faceTexturePath);
    DimooModel::resetFaceTextureTuning();
    DimooModel::resetHairAngleTuning();
    DimooModel::resetHairClusterStates();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    gViewer.lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown() {
    if (gViewer.faceTex) {
        glDeleteTextures(1, &gViewer.faceTex);
        gViewer.faceTex = 0;
    }
#ifdef _WIN32
    if (gGdiToken != 0) {
        Gdiplus::GdiplusShutdown(gGdiToken);
        gGdiToken = 0;
    }
#endif
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void display() {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    drawInspectorUi();
    drawScene();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
}

void reshape(int width, int height) {
    gViewer.width = width;
    gViewer.height = std::max(1, height);
    ImGui_ImplGLUT_ReshapeFunc(width, height);
}

void idle() {
    float now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = now - gViewer.lastTime;
    gViewer.lastTime = now;
    if (dt > 0.05f) {
        dt = 0.05f;
    }
    updateAnimation(dt);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    switch (key) {
    case 27:
        shutdown();
#ifdef __FREEGLUT_EXT_H__
        glutLeaveMainLoop();
#endif
        break;
    case ' ':
        gViewer.autoAnimate = !gViewer.autoAnimate;
        break;
    case 'f':
    case 'F':
        gViewer.visual.facingRight = !gViewer.visual.facingRight;
        break;
    case 't':
    case 'T':
        gViewer.useFaceTexture = !gViewer.useFaceTexture;
        break;
    case 'r':
    case 'R':
        gViewer = ViewerState();
        gViewer.faceTex = loadFaceTexture(&gViewer.faceTexturePath);
        DimooModel::resetFaceTextureTuning();
        DimooModel::resetHairAngleTuning();
        DimooModel::resetHairClusterStates();
        break;
    default:
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
}

void special(int key, int x, int y) {
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
}

void specialUp(int key, int x, int y) {
    ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
}

void mouse(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void motion(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
}

void passiveMotion(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
}

} // namespace DimooViewerApp
