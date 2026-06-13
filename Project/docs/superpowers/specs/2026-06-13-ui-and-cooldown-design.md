# 战斗冷却系统与 HUD 霓虹美化设计规格说明书 (UI & Cooldown Design Spec)

本设计文档旨在为主要游戏（FightingGame）引入技能冷却限制（CD），并对游戏内 UI 界面（血条、倒计时、字体与 CD 指示）进行深度的极简科浮霓虹风格（Flat Neon & Minimal）美化，彻底改善原版画面的简陋感。

---

## 1. 技能冷却 (CD) 系统设计

为了增强游戏的可玩性与公平对抗性，需要对双方角色（P1 Hirono 与 P2 Dimoo）的普通攻击、小技能和大招施加冷却时间限制，防止无间隔连发。

### 1.1 数据结构定义 (在 `src/Game.h` 中)
在 `Game` 类中新增冷却计时器变量（单位：秒）：
```cpp
// 冷却时间状态量 (秒)
float hironoAttackCD;
float hironoSkillCD;
float hironoUltCD;

float dimooAttackCD;
float dimooSkillCD;
float dimooUltCD;
```

### 1.2 冷却时间常量设定
*   **普通攻击 (Level 1)**：`CD_ATTACK = 0.3f` 秒（短 CD，控制连续平A速度）
*   **小技能 (Level 2)**：`CD_SKILL = 3.0f` 秒
*   **大招 (Level 3)**：`CD_ULT = 8.0f` 秒

### 1.3 CD 状态更新 (在 `src/Game.cpp` 的 `updateBattle` 中)
在每帧的物理更新中，递减 CD 计时器，直到归零：
```cpp
if (hironoAttackCD > 0.0f) hironoAttackCD = std::max(0.0f, hironoAttackCD - dt);
if (hironoSkillCD > 0.0f) hironoSkillCD = std::max(0.0f, hironoSkillCD - dt);
if (hironoUltCD > 0.0f) hironoUltCD = std::max(0.0f, hironoUltCD - dt);

if (dimooAttackCD > 0.0f) dimooAttackCD = std::max(0.0f, dimooAttackCD - dt);
if (dimooSkillCD > 0.0f) dimooSkillCD = std::max(0.0f, dimooSkillCD - dt);
if (dimooUltCD > 0.0f) dimooUltCD = std::max(0.0f, dimooUltCD - dt);
```

### 1.4 攻击指令拦截 (在 `src/Game.cpp` 的 `handleBattleAttacks` 中)
在检测到按键触发排队时（如 `input.dimoo_attackQueued`），首先拦截判断对应 CD 是否已归零：
*   **若 CD <= 0.0f**：允许释放技能，并重置 CD 计时器为上限。
*   **若 CD > 0.0f**：拦截，拒绝释放。
*(对 Hirono 和 Dimoo 两侧均进行对称的逻辑拦截。)*

---

## 2. Windows 高品质系统字体渲染设计

摆脱原版粗糙、不支持缩放的 GLUT 位图字体，利用 Windows + OpenGL 原生的 `wglUseFontBitmapsA` 加载系统自带的高清字体，并使用抗锯齿算法。

### 2.1 显示列表与字体加载
在 `Game` 类中新增两个字体显示列表基准（Display List Base）：
```cpp
GLuint hFontImpact;    // 用于倒计时数字、大标题等粗体字
GLuint hFontSegoe;     // 用于一般信息、名字、说明文本等极简字
```

在 `Game::init()` 中通过 Windows 接口进行创建和加载：
```cpp
HDC hdc = wglGetCurrentDC();

// 1. 创建 Impact 粗体字体（抗锯齿）
HFONT fontImpact = CreateFontA(-32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                               ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Impact");
SelectObject(hdc, fontImpact);
hFontImpact = glGenLists(256);
wglUseFontBitmapsA(hdc, 0, 256, hFontImpact);
DeleteObject(fontImpact);

// 2. 创建 Segoe UI 极简字体（抗锯齿）
HFONT fontSegoe = CreateFontA(-18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Segoe UI");
SelectObject(hdc, fontSegoe);
hFontSegoe = glGenLists(256);
wglUseFontBitmapsA(hdc, 0, 256, hFontSegoe);
DeleteObject(fontSegoe);
```

### 2.2 4 方向立体描边投影渲染算法
在 `Game.cpp` 中 define 一个重载的绘制文本函数 `drawOutlineString`：
```cpp
static void drawOutlineString(GLuint fontBase, const std::string& str, float x, float y, float r, float g, float b) {
    // 1. 绘制 4 方向黑色半透明描边影
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    float offset = 1.0f;
    float offsets[4][2] = { {-offset, -offset}, {offset, -offset}, {-offset, offset}, {offset, offset} };
    for (int i = 0; i < 4; i++) {
        glRasterPos2f(x + offsets[i][0], y + offsets[i][1]);
        glListBase(fontBase);
        glCallLists((GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str());
    }

    // 2. 绘制主体彩色字
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    glListBase(fontBase);
    glCallLists((GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str());
}
```

---

## 3. HUD 极简科幻霓虹美化设计

### 3.1 极简双层霓虹血条
*   **高度缩窄**：血条在 Y 轴的高度由原来的 `35` 像素压缩至 `12` 像素（Y 在 `650` 到 `662` 之间）。
*   **霓虹外廓**：使用 `glLineWidth(1.5f)` 绘制外廓框。
    *   **P1 (Hirono)** 霓虹框：`glColor4f(1.0f, 0.45f, 0.15f, 0.8f)` (暖橙色系)
    *   **P2 (Dimoo)** 霓虹框：`glColor4f(0.3f, 0.8f, 1.0f, 0.8f)` (青蓝色系)
*   **血条填充 (Fill)**：不再使用平滑的渐变色填充，而是用各自的明亮霓虹单色直接填充，当受到伤害时，填充条瞬间无级缩短，没有缓冲残留以呈现绝对极简与科技感。

### 3.2 头像与三段 CD 霓虹圆弧
在双方头像（P1 头像中心 `(22.5, 656.5)`，P2 头像中心 `(1257.5, 656.5)`）周围绘制半径 $R = 25$ 的圆弧，并开启线条抗锯齿 `glEnable(GL_LINE_SMOOTH)`：
*   **普通攻击 (Level 1)** 圆弧段：$0^\circ \sim 100^\circ$
*   **小技能 (Level 2)** 圆弧段：$120^\circ \sim 220^\circ$
*   **大招 (Level 3)** 圆弧段：$240^\circ \sim 340^\circ$

**状态反馈**：
1.  **冷却完成 (可用)**：对应的圆弧段呈现高亮霓虹色（P1 金黄色，P2 冰蓝色），使用 `matchTimer` 对应的 `sin` 函数做轻微的发光呼吸。
2.  **冷却中 (不可用)**：
    *   获取该技能的冷却百分比 `pct = currentCD / maxCD`。
    *   仅绘制该圆弧段的前 `(1.0f - pct)` 部分，剩余部分为暗灰色虚线。
    *   在圆弧旁边用抗锯齿极小 Segoe UI 字体绘制剩余整数秒数（例如 `2s`）。

### 3.3 悬浮霓虹计时器
*   **结构**：去除原本正方形的黑底板。在屏幕上方中央 `(640, 655)`，绘制一个纤细的半透明圆环（半径 $R = 24$），边框颜色为白蓝色霓虹 `glColor4f(0.8f, 0.95f, 1.0f, 0.6f)`。
*   **倒计时**：使用抗锯齿 **Impact** 粗体，悬浮在圆环中央。
*   **决战时刻闪烁**：当倒计时 `matchTimer <= 10.0f` 时，圆环及数字全部转为火红色 `(1.0f, 0.1f, 0.1f)`，并配合 `matchTimer` 的小数部分进行每秒的放大-收缩警报呼吸动作。
