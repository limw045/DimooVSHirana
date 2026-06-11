# Dimoo 头发重构实现计划

> **面向 AI 代理的工作者：** 必需子技能：使用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐任务实现此计划。步骤使用复选框（`- [ ]`）语法来跟踪进度。

**目标：** 重构 Dimoo 3D 模型的头发几何生成、配色及材质，实现高档手办粘土缎带（Sculpted Clay Ribbons）的哑光粉紫渐变视觉效果。

**架构：** 在 `DimooModel.cpp` 中，通过 Sine 双向收尖和二次曲线 Y 轴偏移公式重建发丝几何；根据发团坐标插值 Pink-to-Lavender-to-White 颜色；调低高光镜面反射率和光泽度来模拟哑光粘土材质。

**技术栈：** C++, OpenGL, GLUT

---

### 任务 1：重构发丝几何体生成 (Clay Ribbon Geometry)

**文件：**
- 修改：`Project/src/DimooModel.cpp:162-214` (修改 `drawPetal3D` 以支持双向收尖和卷曲弯曲)
- 修改：`Project/src/DimooModel.cpp:471-526` (修改 `drawHairCluster` 传入弯曲偏移参数)
- 修改：`Project/src/DimooModel.cpp:528-560` (在 `drawHairClusters` 中计算并应用每股发丝的弯曲度)

- [ ] **步骤 1：重构 `drawPetal3D` 支持双向收尖与流线卷曲**
  修改 `drawPetal3D` 函数定义，接收弯曲偏移参数 `bendX` 和 `bendZ`，并使用 Sine 函数做双端收尖。
  ```cpp
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

              // 双端收尖：使用 sine 曲线替代单端收缩，使两端自然变细
              float taper0 = sin((y0_u + 1.0f) * 0.5f * (float)M_PI);
              float taper1 = sin((y1_u + 1.0f) * 0.5f * (float)M_PI);

              // 弯曲因子：两端不弯曲，中间弯曲度最大
              float bendFactor0 = 1.0f - y0_u * y0_u;
              float bendFactor1 = 1.0f - y1_u * y1_u;

              float bx0 = bendX * bendFactor0;
              float bz0 = bendZ * bendFactor0;
              float bx1 = bendX * bendFactor1;
              float bz1 = bendZ * bendFactor1;

              float factor0 = (y0_u + 1.0f) * 0.5f;
              float factor1 = (y1_u + 1.0f) * 0.5f;

              factor0 = factor0 * factor0 * (3.0f - 2.0f * factor0);
              factor1 = factor1 * factor1 * (3.0f - 2.0f * factor1);

              glNormal3f(x0_u, y0_u, z0);
              glColor4f((baseColor[0] * (1.0f - factor0) + tipColor[0] * factor0) * 0.78f,
                        (baseColor[1] * (1.0f - factor0) + tipColor[1] * factor0) * 0.78f,
                        (baseColor[2] * (1.0f - factor0) + tipColor[2] * factor0) * 0.78f,
                        1.0f);
              glVertex3f(x0_u * rx * taper0 + bx0, y0_u * ry, z0 * rz * taper0 + bz0);

              glNormal3f(x1_u, y1_u, z1);
              glColor4f((baseColor[0] * (1.0f - factor1) + tipColor[0] * factor1) * 0.78f,
                        (baseColor[1] * (1.0f - factor1) + tipColor[1] * factor1) * 0.78f,
                        (baseColor[2] * (1.0f - factor1) + tipColor[2] * factor1) * 0.78f,
                        1.0f);
              glVertex3f(x1_u * rx * taper1 + bx1, y1_u * ry, z1 * rz * taper1 + bz1);
          }
          glEnd();
      }
  }
  ```

- [ ] **步骤 2：更新 `drawHairCluster` 以传递弯曲参数**
  修改 `drawHairCluster` 函数声明与实现，接收 `bendX` 和 `bendZ`，并更新其内部调用的 `drawPetal3D`。
  ```cpp
  static void drawHairCluster(const Vec3& pos, const Vec3& scale,
                              const Vec3& rot, int colorIndex,
                              bool highlightTop, float bendX, float bendZ) {
      glPushMatrix();
      glTranslatef(pos.x, pos.y, pos.z);
      glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
      glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
      glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

      // 调整发丝基础拉伸，使其看起来比球体更修长
      glScalef(1.12f, 1.02f, 1.40f);
      
      applyPetalMaterial(colorIndex, false);

      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

      // 此时颜色参数 colorIndex 会在外部重构为粉紫渐变色，此处直接解包
      int idx = colorIndex % 5;
      const GLfloat* baseCol = DIMOO_PALETTE[idx];
      const GLfloat* tipCol = DIMOO_PALETTE[4]; // Pearl White

      GLfloat richBaseCol[3];
      if (idx == 4) {
          richBaseCol[0] = baseCol[0]; richBaseCol[1] = baseCol[1]; richBaseCol[2] = baseCol[2];
      } else {
          float minVal = baseCol[0];
          if (baseCol[1] < minVal) minVal = baseCol[1];
          if (baseCol[2] < minVal) minVal = baseCol[2];
          richBaseCol[0] = clamp(baseCol[0] - minVal * 0.28f, 0.0f, 1.0f);
          richBaseCol[1] = clamp(baseCol[1] - minVal * 0.28f, 0.0f, 1.0f);
          richBaseCol[2] = clamp(baseCol[2] - minVal * 0.28f, 0.0f, 1.0f);
      }

      drawPetal3D(scale.x, scale.y, scale.z, 12, 16, richBaseCol, tipCol, bendX, bendZ);

      glDisable(GL_COLOR_MATERIAL);

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
  ```

- [ ] **步骤 3：更新 `drawHairClusters` 中的弯曲计算逻辑**
  在 `drawHairClusters` 循环中，根据发丝的空间位置自动计算弯曲度 `bendX` 和 `bendZ`（向头外侧及下方弯曲）。
  ```cpp
          // 弯曲计算：根据发丝的空间横向和深度位置计算自然向外弯曲
          float bendX = 0.0f;
          float bendZ = 0.0f;
          
          if (p.x < -0.1f) {
              bendX = -0.05f; // 左侧发丝向左弯曲
          } else if (p.x > 0.1f) {
              bendX = 0.05f;  // 右侧发丝向右弯曲
          }

          if (p.z > 0.1f) {
              bendZ = -0.04f; // 前额发丝略向后贴合头部
          } else if (p.z < -0.1f) {
              bendZ = 0.04f;  // 后脑发丝向前弯曲
          }

          // 如果是前额刘海 (i < 5)，加大向下/向后的弯曲，以贴合额头
          if (i < 5) {
              bendZ -= 0.02f;
          }

          drawHairCluster(p, s, r, i % 5, gHairClusters[i].highlightTop, bendX, bendZ);
  ```

- [ ] **步骤 4：运行编译命令验证几何变更**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak viewerdebug`
  预期：PASS (编译通过，暂时颜色还未修改完毕)

- [ ] **步骤 5：Commit 几何部分的重构**
  ```bash
  git add Project/src/DimooModel.cpp
  git commit -m "feat(hair): Reconstruct hair ribbon geometry and bending"
  ```

---

### 任务 2：重构配色逻辑 (Pink-to-Lavender-to-White Ombré)

**文件：**
- 修改：`Project/src/DimooModel.cpp:484-522` (在 `drawHairCluster` 中根据位置计算插值颜色，替换原有的 `colorIndex % 5`)
- 修改：`Project/src/DimooModel.cpp:528-560` (更新 `drawHairClusters` 中的颜色参数传递)

- [ ] **步骤 1：重构 `drawHairCluster` 的上色方式，实现平滑粉紫渐变**
  在 `drawHairCluster` 中根据发团局部位置 `pos` 的 X 和 Y 轴坐标计算插值系数，动态混合粉红与淡紫，并且高位发丝增加白光。
  ```cpp
      // 定义粉紫基准色
      GLfloat pinkCol[] = {0.98f, 0.82f, 0.85f};  // 珍珠粉
      GLfloat lavenderCol[] = {0.88f, 0.84f, 0.94f}; // 薰衣草紫
      GLfloat whiteCol[] = {0.98f, 0.98f, 0.99f};  // 珍珠白

      // 1. 横向渐变插值（左侧偏紫，右侧偏粉）
      float tX = clamp((pos.x + 0.35f) / 0.70f, 0.0f, 1.0f);
      GLfloat baseCol[3];
      for (int c = 0; c < 3; ++c) {
          baseCol[c] = lavenderCol[c] * (1.0f - tX) + pinkCol[c] * tX;
      }

      // 2. 纵向渐变插值（根部颜色深，顶部偏白，模拟喷笔明暗）
      float tY = clamp((pos.y - 0.0f) / 0.45f, 0.0f, 1.0f);
      // tY 越接近 1 (头顶)，颜色越浅偏白
      GLfloat finalBaseCol[3];
      for (int c = 0; c < 3; ++c) {
          finalBaseCol[c] = baseCol[c] * (1.0f - tY * 0.4f) + whiteCol[c] * (tY * 0.4f);
      }

      // 发梢始终渐变到白色
      const GLfloat* tipCol = whiteCol;

      GLfloat richBaseCol[3];
      // 适度提亮饱和度
      float minVal = finalBaseCol[0];
      if (finalBaseCol[1] < minVal) minVal = finalBaseCol[1];
      if (finalBaseCol[2] < minVal) minVal = finalBaseCol[2];
      richBaseCol[0] = clamp(finalBaseCol[0] - minVal * 0.15f, 0.0f, 1.0f);
      richBaseCol[1] = clamp(finalBaseCol[1] - minVal * 0.15f, 0.0f, 1.0f);
      richBaseCol[2] = clamp(finalBaseCol[2] - minVal * 0.15f, 0.0f, 1.0f);

      drawPetal3D(scale.x, scale.y, scale.z, 12, 16, richBaseCol, tipCol, bendX, bendZ);
  ```

- [ ] **步骤 2：更新 `drawHairClusters` 中的参数**
  使 `drawHairClusters` 渲染发团时不使用固定的调色盘，改为传入发团的局部中心坐标 `p` 以计算色彩插值。
  ```cpp
          drawHairCluster(p, s, r, i, gHairClusters[i].highlightTop, bendX, bendZ);
  ```

- [ ] **步骤 3：编译并确认配色正确**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak viewerdebug`
  预期：PASS (编译通过，颜色变成了和谐的粉紫渐变)

- [ ] **步骤 4：Commit 配色重构代码**
  ```bash
  git add Project/src/DimooModel.cpp
  git commit -m "feat(hair): Refactor hair color to pink-purple-white gradient"
  ```

---

### 任务 3：重构材质为哑光粘土 (Matte Clay Shading)

**文件：**
- 修改：`Project/src/DimooModel.cpp:114-138` (更新 `applyPetalMaterial` 函数中的反射率和光泽度)

- [ ] **步骤 1：调整材质为半哑光手办粘土**
  调低高光镜面反射 `specular` 和光泽度等级 `shininess`，同时微调环境光，营造柔和的粉感。
  ```cpp
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

      // 稍微增强环境光 ambient (从 0.16 增加到 0.26f)，使背光处更柔和
      GLfloat ambient[]  = {r * 0.26f, g * 0.26f, b * 0.26f, 1.0f};
      GLfloat diffuse[]  = {r, g, b, 1.0f};
      
      // 显著降低 specular 反射率 (从 0.45 降至 0.16f)，避免过亮斑点
      GLfloat specular[] = {
          highlight ? 0.85f : 0.16f,
          highlight ? 0.85f : 0.16f,
          highlight ? 0.88f : 0.18f,
          1.0f
      };
      
      // 大幅降低光泽度 shininess (从 84.0f 降至 24.0f)，使高光在表面更加发散平铺，形成哑光土质感
      setMaterial(ambient, diffuse, specular, highlight ? 64.0f : 24.0f);
  }
  ```

- [ ] **步骤 2：编译项目并运行测试**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak viewerdebug`
  预期：PASS (编译通过)

- [ ] **步骤 3：Commit 材质优化代码**
  ```bash
  git add Project/src/DimooModel.cpp
  git commit -m "feat(hair): Change hair material to soft matte clay"
  ```

---

### 任务 4：全局整合与视觉效果验收 (Integration & Verification)

**文件：**
- 修改：无 (仅重新生成整个项目并启动验收)

- [ ] **步骤 1：清除并重新编译全部目标**
  运行：
  ```bash
  mingw32-make -f DimooVsHirona.cbp.mak clean
  mingw32-make -f DimooVsHirona.cbp.mak viewerdebug
  mingw32-make -f DimooVsHirona.cbp.mak debug
  ```
  预期：所有目标均成功编译。

- [ ] **步骤 2：运行 Dimoo Viewer 验收**
  运行：`./DimooViewerDebug.exe` (或通过任务运行)
  预期：Dimoo 头发形状圆润且有缎带式曲线收尖，配色呈现温和的粉紫到白色渐变，材质为哑光，没有尖锐刺眼的反光。

- [ ] **步骤 3：运行游戏客户端验收**
  运行：`./FightingGameDebug.exe`
  预期：主角在盲盒舞台中头发呈现精美的高档潮玩质感，色彩表现契合暗盒宇宙环境。
