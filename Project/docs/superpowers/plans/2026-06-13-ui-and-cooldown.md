# 战斗冷却系统与 HUD 霓虹美化实现计划

> **面向 AI 代理的工作者：** 必需子技能：使用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐任务实现此计划。步骤使用复选框（`- [ ]`）语法来跟踪进度。

**目标：** 在游戏中为双方角色添加普通攻击、技能和大招的冷却逻辑，并重构 HUD UI 界面为极简霓虹风格，使用 Windows 系统抗锯齿高质感字体。

**架构：**
1. 在 `Game.h` 中为双方技能添加 CD 变量，并在每帧物理更新时递减其值，在攻击排队判断时进行拦截。
2. 借用 Windows 原生的 `wglUseFontBitmapsA` 函数在 OpenGL 初始化时加载 Windows 系统的 Impact (粗体) 和 Segoe UI (普通) 字体，提升排版精度。
3. 对 `Game::drawHUD()` 进行整体视觉重构：极简血条（12px高度）+ 霓虹线框、头像外圈三段环绕霓虹 CD 进度弧线、中央计时器改用发光霓虹环表示，并在倒计时最后 10s 进行闪烁警报。

**技术栈：** C++ / OpenGL 2.0 / GLUT / Windows API (WGL / GDI)

---

## 变更文件结构规划
1. [Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)：声明冷却状态变量、字体列表基址、极简常量。
2. [Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp)：实现冷却计时递减、攻击触发拦截、Windows 字体加载生成、带四向描边立体阴影的绘制逻辑以及重构 `drawHUD()` 的所有 UI 绘图。

---

### 任务 1：定义并初始化冷却（CD）变量

**文件：**
*   修改：[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h#L79-L84)
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L60-L102)

- [ ] **步骤 1：在 Game.h 声明冷却状态量**
  在 `class Game` 中定义：
  ```cpp
  // 冷却时间状态量 (秒)
  float hironoAttackCD;
  float hironoSkillCD;
  float hironoUltCD;
  float dimooAttackCD;
  float dimooSkillCD;
  float dimooUltCD;
  ```

- [ ] **步骤 2：在 Game.cpp 中初始化所有 CD 为 0.0f**
  定位至 `Game::init()` 以及重置游戏状态的对应代码行，确保初次运行或游戏重开时 CD 清零：
  ```cpp
  hironoAttackCD = 0.0f;
  hironoSkillCD = 0.0f;
  hironoUltCD = 0.0f;
  dimooAttackCD = 0.0f;
  dimooSkillCD = 0.0f;
  dimooUltCD = 0.0f;
  ```

- [ ] **步骤 3：Commit**
  ```bash
  git add src/Game.h src/Game.cpp
  git commit -m "feat: declare and initialize skill cooldown variables"
  ```

---

### 任务 2：冷却计时器每帧递减更新

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L393-L399)

- [ ] **步骤 1：在 Game::updateBattle 中对所有 CD 进行递减更新**
  使用 `dt` (Delta Time) 更新双方 CD，限幅在 `0.0f`：
  ```cpp
  if (hironoAttackCD > 0.0f) hironoAttackCD = (hironoAttackCD - dt > 0.0f) ? hironoAttackCD - dt : 0.0f;
  if (hironoSkillCD > 0.0f) hironoSkillCD = (hironoSkillCD - dt > 0.0f) ? hironoSkillCD - dt : 0.0f;
  if (hironoUltCD > 0.0f) hironoUltCD = (hironoUltCD - dt > 0.0f) ? hironoUltCD - dt : 0.0f;

  if (dimooAttackCD > 0.0f) dimooAttackCD = (dimooAttackCD - dt > 0.0f) ? dimooAttackCD - dt : 0.0f;
  if (dimooSkillCD > 0.0f) dimooSkillCD = (dimooSkillCD - dt > 0.0f) ? dimooSkillCD - dt : 0.0f;
  if (dimooUltCD > 0.0f) dimooUltCD = (dimooUltCD - dt > 0.0f) ? dimooUltCD - dt : 0.0f;
  ```

- [ ] **步骤 2：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "feat: implement per-frame cooldown update in updateBattle"
  ```

---

### 任务 3：攻击与技能指令触发拦截

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L542-L558)

- [ ] **步骤 1：修改 Game::handleBattleAttacks**
  拦截按键动作：当检测到按键发生且 CD <= 0 时，才执行对应的 perform 函数，并且将 CD 重置为最大值。
  CD 设置为：普攻 0.3s，小技能 3.0s，大招 8.0s。
  ```cpp
  void Game::handleBattleAttacks() {
      if (currentState != STATE_BATTLE || matchOver) {
          input.hirono_attackQueued = input.hirono_skillQueued = input.hirono_ultQueued = false;
          input.dimoo_attackQueued = input.dimoo_skillQueued = input.dimoo_ultQueued = false;
          return;
      }

      // Hirono (P1)
      if (input.hirono_attackQueued) {
          if (hironoAttackCD <= 0.0f) {
              performHironoAttack(1);
              hironoAttackCD = 0.3f;
          }
      }
      if (input.hirono_skillQueued) {
          if (hironoSkillCD <= 0.0f) {
              performHironoAttack(2);
              hironoSkillCD = 3.0f;
          }
      }
      if (input.hirono_ultQueued) {
          if (hironoUltCD <= 0.0f) {
              performHironoAttack(3);
              hironoUltCD = 8.0f;
          }
      }

      // Dimoo (P2)
      if (input.dimoo_attackQueued) {
          if (dimooAttackCD <= 0.0f) {
              performDimooAttack(1);
              dimooAttackCD = 0.3f;
          }
      }
      if (input.dimoo_skillQueued) {
          if (dimooSkillCD <= 0.0f) {
              performDimooAttack(2);
              dimooSkillCD = 3.0f;
          }
      }
      if (input.dimoo_ultQueued) {
          if (dimooUltCD <= 0.0f) {
              performDimooAttack(3);
              dimooUltCD = 8.0f;
          }
      }

      input.hirono_attackQueued = input.hirono_skillQueued = input.hirono_ultQueued = false;
      input.dimoo_attackQueued = input.dimoo_skillQueued = input.dimoo_ultQueued = false;
  }
  ```

- [ ] **步骤 2：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "feat: implement skill cooldown check and interception"
  ```

---

### 任务 4：Windows 原生抗锯齿系统字体加载

**文件：**
*   修改：[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L60-L75)

- [ ] **步骤 1：在 Game.h 声明字体列表基址**
  ```cpp
  GLuint hFontImpact;
  GLuint hFontSegoe;
  ```

- [ ] **步骤 2：在 Game::init() 中加载 Windows 系统字体位图**
  ```cpp
  HDC hdc = wglGetCurrentDC();
  if (hdc) {
      // Impact 字体
      HFONT fontImpact = CreateFontA(-32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                     ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Impact");
      HFONT oldFont = (HFONT)SelectObject(hdc, fontImpact);
      hFontImpact = glGenLists(256);
      wglUseFontBitmapsA(hdc, 0, 256, hFontImpact);
      SelectObject(hdc, oldFont);
      DeleteObject(fontImpact);

      // Segoe UI 字体
      HFONT fontSegoe = CreateFontA(-18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                   ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                                   ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Segoe UI");
      oldFont = (HFONT)SelectObject(hdc, fontSegoe);
      hFontSegoe = glGenLists(256);
      wglUseFontBitmapsA(hdc, 0, 256, hFontSegoe);
      SelectObject(hdc, oldFont);
      DeleteObject(fontSegoe);
  }
  ```

- [ ] **步骤 3：Commit**
  ```bash
  git add src/Game.h src/Game.cpp
  git commit -m "feat: load high-quality Windows fonts using wglUseFontBitmaps"
  ```

---

### 任务 5：实现四向描边立体文本绘制函数

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L19-L26)

- [ ] **步骤 1：在 Game.cpp 中实现 drawOutlineString 文本渲染器**
  定位在 `drawString` 的上方，新增 `drawOutlineString` 静态函数：
  ```cpp
  static void drawOutlineString(GLuint fontBase, const std::string& str, float x, float y, float r, float g, float b) {
      if (!fontBase) return;
      
      // 1. 绘制 4 方向黑色半透明影子描边
      glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
      float offset = 1.0f;
      float offsets[4][2] = { {-offset, -offset}, {offset, -offset}, {-offset, offset}, {offset, offset} };
      for (int i = 0; i < 4; i++) {
          glRasterPos2f(x + offsets[i][0], y + offsets[i][1]);
          glListBase(fontBase);
          glCallLists((GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str());
      }

      // 2. 绘制主体字
      glColor3f(r, g, b);
      glRasterPos2f(x, y);
      glListBase(fontBase);
      glCallLists((GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str());
  }
  ```

- [ ] **步骤 2：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "feat: add drawOutlineString for outlined anti-aliased font rendering"
  ```

---

### 任务 6：重构 HUD (1) — 极简双层霓虹血条

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L1021-L1056)

- [ ] **步骤 1：在 drawHUD 中缩窄血条大小并添加霓虹线框**
  修改 `Game::drawHUD`，在战斗模式下渲染血条：
  * P1 血条范围从 `(50, 645)` 到 `(500, 670)` 重构为 `(50, 650)` 到 `(500, 662)`。
  * P2 血条范围从 `(780, 645)` 到 `(1230, 670)` 重构为 `(780, 650)` 到 `(1230, 662)`。
  * 用 `glLineWidth(1.5f)` 画霓虹线框。
  ```cpp
  // P1 血条深色底座
  glColor4f(0.08f, 0.08f, 0.08f, 0.6f);
  glBegin(GL_QUADS);
  glVertex2f(50, 650); glVertex2f(500, 650);
  glVertex2f(500, 662); glVertex2f(50, 662);
  glEnd();
  
  // P1 血条填充
  float h1Pct = hironoHp / hironoMaxHp;
  glColor4f(1.0f, 0.4f, 0.1f, 1.0f); // 亮暖橙
  glBegin(GL_QUADS);
  glVertex2f(50, 650); glVertex2f(50 + 450.0f * h1Pct, 650);
  glVertex2f(50 + 450.0f * h1Pct, 662); glVertex2f(50, 662);
  glEnd();

  // P1 霓虹描边
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.5f);
  glColor4f(1.0f, 0.45f, 0.15f, 0.8f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(49, 649); glVertex2f(501, 649);
  glVertex2f(501, 663); glVertex2f(49, 663);
  glEnd();

  // P2 同样按比例和尺寸绘制（暖蓝霓虹色系：glColor4f(0.3f, 0.8f, 1.0f, 0.8f)）
  ```

- [ ] **步骤 2：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "style: redesign health bar to flat neon slim style"
  ```

---

### 任务 7：重构 HUD (2) — 头像外圈 CD 圆弧

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L1058-L1084)

- [ ] **步骤 1：编写圆弧绘制函数并在 drawHUD 中调用**
  在 `Game.cpp` 声明并实现一个绘制霓虹圆弧段的函数：
  ```cpp
  static void drawArc(float cx, float cy, float r, float start_deg, float end_deg, int segments = 30) {
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i <= segments; ++i) {
          float t = (float)i / (float)segments;
          float deg = start_deg + (end_deg - start_deg) * t;
          float rad = deg * (float)M_PI / 180.0f;
          glVertex2f(cx + cos(rad) * r, cy + sin(rad) * r);
      }
      glEnd();
  }
  ```

- [ ] **步骤 2：在 drawHUD 中为头像周围绘制 3 段 CD 进度弧**
  * P1 头像圆心：`(22.5f, 656.5f)`，半径 `R = 25.0f`。
  * P2 头像圆心：`(1257.5f, 656.5f)`，半径 `R = 25.0f`。
  * 普攻弧：$0^\circ \sim 100^\circ$，技能弧：$120^\circ \sim 220^\circ$，大招弧：$240^\circ \sim 340^\circ$。
  * 针对 CD 的比例做截断：若 `dimooSkillCD > 0.0f`，则绘制的弧度终点限制为 `120.0f + 100.0f * (1.0f - dimooSkillCD / 3.0f)`。并以暗灰色虚弱绘制未填充部分。

- [ ] **步骤 3：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "feat: add 3-stage cooldown neon arcs around profile avatars"
  ```

---

### 任务 8：重构 HUD (3) — 悬浮霓虹计时器圆环

**文件：**
*   修改：[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp#L1085-L1100)

- [ ] **步骤 1：在 drawHUD 中绘制中央倒计时环**
  * 圆心 `(640.0f, 656.0f)`，半径 `R = 24.0f`，线宽 `1.5f`。
  * 根据 `matchTimer` 确定颜色：若 `<= 10.0f`，使用红色并按 `sin(matchTimer * M_PI * 2)` 进行警报性呼吸闪烁。否则使用白蓝色霓虹。
  * 利用 `drawOutlineString` 绘制时间数字，定位在圆内中心。
  ```cpp
  float pulse = 1.0f;
  float tr = 0.8f, tg = 0.95f, tb = 1.0f; // 白蓝
  if (matchTimer <= 10.0f) {
      tr = 1.0f; tg = 0.1f; tb = 0.1f; // 火红
      pulse = 1.0f + 0.12f * std::abs(sin(matchTimer * 3.14159f * 2.0f));
  }
  
  // 绘制时间刻度环
  glColor4f(tr, tg, tb, 0.7f);
  glLineWidth(1.5f);
  drawArc(640.0f, 656.0f, 24.0f * pulse, 0.0f, 360.0f, 40);
  
  // 绘制数字 (居中)
  std::stringstream ss;
  ss << (int)matchTimer;
  drawOutlineString(hFontImpact, ss.str(), (matchTimer >= 10.0f) ? 627.0f : 633.0f, 646.0f, tr, tg, tb);
  ```

- [ ] **步骤 2：Commit**
  ```bash
  git add src/Game.cpp
  git commit -m "style: redesign battle timer to neon floating pulse ring"
  ```

---

### 任务 9：编译、测试与最终验证

**文件：**
*   运行编译构建。

- [ ] **步骤 1：触发编译构建**
  在 CodeBlocks 目录中使用命令行对 `Release` 目标进行构建。
  命令：`mingw32-make -f DimooVsHirona.cbp.mak` (或直接在 IDE 中 Build)

- [ ] **步骤 2：运行 FightingGame.exe 验证**
  启动程序，检查以下几点：
  1. 1、2、3 键及 C、V、B 键的技能是否不能无限无冷却连发；
  2. 冷却时头像周围的圆弧是否能正确由灰色填满彩色，并显示小数字 CD；
  3. 计时器、P1、P2 名字、底部调试键文字是否都以高清、带描边投影的 Impact/Segoe UI 系统字体清晰呈现；
  4. 倒计时到最后 10s 时，中央环是否变红并开始一缩一放呼吸。

- [ ] **步骤 3：Commit**
  ```bash
  git commit -m "test: verify neon HUD and anti-spam skill cooldown locks"
  ```
