# Dimoo 3D 蝴蝶粒子与远程二/三技能及跳跃增强实现计划

> **面向 AI 代理的工作者：** 必需子技能：使用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐任务实现此计划。步骤使用复选框（`- [ ]`）语法来跟踪进度。

**目标：** 重构 Dimoo 粒子系统，使蝴蝶粒子升级为 3D 骨骼振翅模型；将二技能重构为可跳跃闪避的远程梦蝶弹体攻击；将大招横向攻击范围扩大至 `8.0f` 并喷射铺天盖地的蝴蝶狂澜；将两名角色的跳跃速度提升至 `6.8f`，确保可通过跳跃精确闪避蝴蝶弹体和普攻。

**架构：**
1. **DimooModel 接口化**：将 `drawButterfly3D` 从 `static` 改为全局导出，新增 `alpha` 及 `r, g, b` 材质渐变和色彩参数。
2. **弹体系统实现**：在 `Game` 类中增加弹体 vector 容器。在二技能施放时实例化弹体。每一帧在 `Game::update` 中更新其坐标（包含 X 轴平移与 Y 轴正弦飞舞），并检测与对手的 3D 距离，命中后触发伤害与爆炸特效。
3. **粒子 3D 化与配色**：在 `Game::draw` 渲染粒子时，将 `PARTICLE_BUTTERFLY` 渲染替换为 `drawButterfly3D`，由速度决定其 Yaw 朝向，由生命周期决定淡出与扇翅，并根据平A（粉色）、二技能（青蓝色）、大招（淡紫色）应用三种不同的色彩。
4. **跳跃高度提升与 Y 轴碰撞**：在 `Game::handleBattleMovement` 中将起跳初速度从 `5.5f` 提升至 `6.8f`，同时对所有近战和远程判定进行严格的 Y 轴高度差计算，确保跳跃可以闪避。

**技术栈：** C++, OpenGL, GLUT

---

## 拟创建/修改的文件与职责

1. **[DimooModel.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.h)**: 导出 `drawButterfly3D` 接口，支持自定义颜色与透明度。
2. **[DimooModel.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.cpp)**: 去除 `drawButterfly3D` 的 `static` 限制，修改材质设置使其支持传入的 `alpha` 与 `r, g, b` 渐变色。
3. **[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)**: 声明弹体结构体 `Projectile` 和 `dimooProjectiles` 容器，以及辅助函数。
4. **[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp)**: 
   - 提升起跳速度；
   - 更新弹体物理运动、生命周期及 3D 距离碰撞检测（包括 Y 轴高度差）；
   - 在二技能、大招触发时分别执行发射弹体与狂澜喷射逻辑；
   - 在绘制粒子和弹体时，调用 3D 蝴蝶绘制函数并设定对应的颜色、Yaw 旋转角和翅膀扇动相位。

---

### 任务 1：导出并增强 3D 蝴蝶渲染函数 (DimooModel)

**文件：**
- 修改：`Project/src/DimooModel.h:37-49`
- 修改：`Project/src/DimooModel.cpp:443-480`

- [ ] **步骤 1：在 `DimooModel.h` 中导出 `drawButterfly3D` 函数**
  修改 `DimooModel.h`，在 `DimooModel` 命名空间中加入 `drawButterfly3D` 声明，参数包含可选的 `alpha` 及自定义 RGB 颜色（默认值为原本的淡绿配色）。
  ```cpp
  // 修改 DimooModel.h 约第 39 行
  void draw(const DimooVisualState& state);
  void drawButterfly3D(float wingAngle, float scale, bool glow, float alpha = 1.0f, float r = 0.82f, float g = 0.94f, float b = 0.78f);
  ```

- [ ] **步骤 2：在 `DimooModel.cpp` 中去除 static 并支持 alpha 材质淡出与自定义 RGB**
  定位到 `DimooModel.cpp` 中的 `drawButterfly3D`，去除前面的 `static`，并在函数声明及材质配置中增加对应参数，使其能够支持动态的 alpha 透明度渲染与颜色传递。
  ```cpp
  // 修改 DimooModel.cpp 中的 drawButterfly3D 函数
  void drawButterfly3D(float wingAngle, float scale, bool glow, float alpha, float r, float g, float b) {
      glPushMatrix();
      glScalef(scale, scale, scale);

      GLfloat bodyAmbient[]  = {0.14f * alpha, 0.16f * alpha, 0.12f * alpha, alpha};
      GLfloat bodyDiffuse[]  = {0.24f * alpha, 0.28f * alpha, 0.20f * alpha, alpha};
      GLfloat bodySpecular[] = {0.12f * alpha, 0.14f * alpha, 0.10f * alpha, alpha};
      setMaterial(bodyAmbient, bodyDiffuse, bodySpecular, 18.0f);
      drawCapsuleBetween(Vec3(0.0f, -0.04f, 0.0f), Vec3(0.0f, 0.07f, 0.0f), 0.011f, 8);

      GLfloat wingAmbient[]  = {r * 0.73f, g * 0.76f, b * 0.74f, alpha};
      GLfloat wingDiffuse[]  = {r, g, b, alpha};
      GLfloat wingSpecular[] = {0.18f, 0.22f, 0.16f, alpha};
      if (glow) {
          wingAmbient[0] = r * 0.90f; wingAmbient[1] = g * 0.92f; wingAmbient[2] = b * 0.90f;
          wingDiffuse[0] = saturate(r + 0.12f); wingDiffuse[1] = saturate(g + 0.06f); wingDiffuse[2] = saturate(b + 0.12f);
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
  ```

- [ ] **步骤 3：编译验证**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak debug`
  预期：编译无报错，生成 `FightingGameDebug.exe`。

- [ ] **步骤 4：Commit**
  ```bash
  git add Project/src/DimooModel.h Project/src/DimooModel.cpp
  git commit -m "feat: export drawButterfly3D with alpha and RGB custom styling"
  ```

---

### 任务 2：实现远程弹体数据结构与初速提升 (Game)

**文件：**
- 修改：`Project/src/Game.h:75-84`
- 修改：`Project/src/Game.cpp:480-492` (handleBattleMovement)

- [ ] **步骤 1：在 `Game.h` 中定义 Projectile 结构体和容器**
  修改 `Game.h`，在粒子系统和计时器定义下方，声明弹体结构体。
  ```cpp
  // 修改 Game.h 约第 80 行
  struct Projectile {
      float x, y, z;
      float vx, vy, vz;
      float size;
      float phase;
      float life;
      float maxLife;
      bool facingRight;
      float r, g, b;
  };
  std::vector<Projectile> dimooProjectiles;
  ```

- [ ] **步骤 2：提升双方起跳速度为 6.8f**
  修改 `Game.cpp` 中 `handleBattleMovement` 方法（约第 481-487 行），将 Hirono 与 Dimoo 的起跳垂直初速度修改为 `6.8f`。
  ```cpp
  // 修改 Game.cpp
  if (input.hirono_upQueued && hironoY <= 0.001f) {
      hironoVy = 6.8f;
  }
  if (input.dimoo_upQueued && dimooY <= 0.121f) {
      dimooVy = 6.8f;
  }
  ```

- [ ] **步骤 3：编译验证**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak debug`
  预期：成功通过编译。

- [ ] **步骤 4：Commit**
  ```bash
  git add Project/src/Game.h Project/src/Game.cpp
  git commit -m "feat: add Projectile container and boost character jump velocity to 6.8f"
  ```

---

### 任务 3：重构二技能与大招动作判定与特效发射逻辑 (Game)

**文件：**
- 修改：`Project/src/Game.cpp:541-583` (performDimooAttack)

- [ ] **步骤 1：重构 `performDimooAttack` 二技能发射弹体与大招 8.0f 判定**
  修改 `performDimooAttack`：
  - 平A（attackLevel == 1）：粒子颜色使用浪漫粉色 `(0.98f, 0.68f, 0.82f)`。
  - 二技能（attackLevel == 2）：改为在 Dimoo 身体前方实例化并射出一只速度为 `3.0f`（或 `-3.0f`）的青蓝色 `(0.40f, 0.85f, 0.98f)` Projectile，并在起点爆散 6 个同色小蝴蝶粒子，不做即时近战伤害判定。
  - 大招（attackLevel == 3）：大招判定范围扩大到 `8.0f`，但额外要求受击判定必须在高度上可闪避（即高度差在一定范围内）。同时从 Dimoo 处产生 40 只高速朝向 Hirono 飞去的淡紫色 `(0.78f, 0.60f, 0.98f)` 3D 蝴蝶。
  ```cpp
  // 修改 Game.cpp 中的 performDimooAttack
  void Game::performDimooAttack(int attackLevel) {
      if (attackLevel == 1) {
          dimooAttackPulse = 1.0f;
          // 平A：爆散 4 只粉色蝴蝶 (0.98f, 0.68f, 0.82f)
          spawnDimooButterflies(dimooX + (dimooFacingRight ? 0.22f : -0.22f), dimooY + 0.56f, 0.0f, 4, 0.26f, false);
          // 在粒子池中更新刚生成的粒子颜色为粉色
          for (size_t idx = particles.size() - 4; idx < particles.size(); ++idx) {
              particles[idx].r = 0.98f; particles[idx].g = 0.68f; particles[idx].b = 0.82f;
          }
          
          // 判定：近战检查 X/Y 轴，Y 轴高度差需小于 1.0f 才能击中
          if (std::abs(dimooX - hironoX) < 1.1f && std::abs(dimooY - hironoY) < 1.0f) {
              hironoHp -= 8.0f;
              spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 15, 0.98f, 0.68f, 0.82f);
              camera.applyShake(0.18f);
              std::cout << "[Combat] Dimoo Melee hit Hirono! Hirono HP: " << hironoHp << std::endl;
          } else {
              spawnHitSparks(dimooX + (dimooFacingRight ? 0.5f : -0.5f), dimooY + 0.5f, 0.0f, 3, 0.98f, 0.68f, 0.82f);
          }
      } else if (attackLevel == 2) {
          dimooSkillPulse = 1.2f;
          dimooAttackPulse = 0.6f;
          
          // 二技能：实例化一个青蓝色梦蝶远程弹体 (0.40f, 0.85f, 0.98f)
          Projectile proj;
          proj.x = dimooX + (dimooFacingRight ? 0.35f : -0.35f);
          proj.y = dimooY + 0.62f;
          proj.z = dimooZ;
          proj.vx = dimooFacingRight ? 4.2f : -4.2f; // 远程弹速
          proj.vy = 0.0f;
          proj.vz = 0.0f;
          proj.size = 1.5f; // 巨型幻影
          proj.phase = 0.0f;
          proj.life = proj.maxLife = 2.0f; // 最长飞 2 秒
          proj.facingRight = dimooFacingRight;
          proj.r = 0.40f; proj.g = 0.85f; proj.b = 0.98f;
          dimooProjectiles.push_back(proj);

          // 起点放出 6 只小蝴蝶
          spawnDimooButterflies(proj.x, proj.y, proj.z, 6, 0.25f, false);
          for (size_t idx = particles.size() - 6; idx < particles.size(); ++idx) {
              particles[idx].r = 0.40f; particles[idx].g = 0.85f; particles[idx].b = 0.98f;
          }
      } else if (attackLevel == 3) {
          dimooUltPulse = 1.5f;
          dimooSkillPulse = 1.35f;
          dimooAttackPulse = 0.8f;
          float ultLift = 0.23f * clamp(dimooUltPulse, 0.0f, 1.2f);
          
          // 判定：横向扩大到 8.0f，但若敌人起跳过高（高度差 >= 1.5f）则可以通过跳跃闪避
          if (std::abs(dimooX - hironoX) < 8.0f && std::abs(dimooY - hironoY) < 1.5f) {
              hironoHp -= 30.0f;
              spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 40, 0.78f, 0.60f, 0.98f);
              camera.applyShake(0.55f);
              arena.triggerLidShake(60.0f);
              std::cout << "[Combat] Dimoo Ultimate Torrent hit Hirono! Hirono HP: " << hironoHp << std::endl;
          } else {
              camera.applyShake(0.35f);
              arena.triggerLidShake(40.0f);
          }
          
          // 大招特效：从 Dimoo 处召唤 40 只高速飞向 Hirono 的淡紫色梦蝶 (0.78f, 0.60f, 0.98f)
          float dir = (hironoX > dimooX) ? 1.0f : -1.0f;
          spawnDimooButterflies(dimooX, dimooY + ultLift + 0.55f, dimooZ, 40, 0.88f, true);
          size_t startIdx = particles.size() - 40;
          for (size_t idx = startIdx; idx < particles.size(); ++idx) {
              particles[idx].r = 0.78f; particles[idx].g = 0.60f; particles[idx].b = 0.98f;
              // 让其主要向 Hirono 方向水平快速飞行
              particles[idx].vx = dir * (2.8f + (rand() % 100) / 100.0f * 1.5f);
              particles[idx].vy = ((rand() % 100) / 100.0f - 0.25f) * 0.8f;
              particles[idx].vz = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;
              particles[idx].maxLife = particles[idx].life = 0.8f + (rand() % 100) / 200.0f; // 缩短飞行存活时间
          }
          spawnDust(dimooX, 0.005f, 0.0f, 12);
      }
  }
  ```

- [ ] **步骤 2：编译验证**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak debug`
  预期：无报错生成二进制文件。

- [ ] **步骤 3：Commit**
  ```bash
  git add Project/src/Game.cpp
  git commit -m "feat: implement performDimooAttack with projectile spawning, wide ult range, and color customization"
  ```

---

### 任务 4：实现弹体更新物理逻辑与 Y 轴碰撞判定 (Game)

**文件：**
- 修改：`Project/src/Game.cpp:187-217` (update)

- [ ] **步骤 1：在 `Game::update` 中更新弹体坐标与碰撞判定**
  在 `Game::update` 粒子系统更新下方，加入对 `dimooProjectiles` 的循环物理移动更新与与 Hirono 的 3D 碰撞检测。
  ```cpp
  // 在 Game::update 粒子系统更新循环之后加入弹体更新
  for (auto it = dimooProjectiles.begin(); it != dimooProjectiles.end(); ) {
      it->phase += dt * 12.0f;
      it->x += it->vx * dt;
      // 蝴蝶飞行正弦波动起伏
      it->y += (it->vy + sin(it->phase * 1.5f) * 0.06f) * dt;
      it->z += it->vz * dt;
      it->life -= dt;

      // 碰撞检测：与 Hirono 进行 3D 距离检测
      float dx = it->x - hironoX;
      float dy = it->y - (hironoY + 0.5f); // 目标中心在 Y+0.5
      float dz = it->z - hironoZ;
      float dist = sqrt(dx*dx + dy*dy + dz*dz);

      bool hit = false;
      if (dist < 0.82f) { // 碰撞半径
          hironoHp -= 16.0f;
          spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 25, it->r, it->g, it->b);
          // 命中处爆散 14 只带有同色彩的 3D 小蝴蝶
          spawnDimooButterflies(it->x, it->y, it->z, 14, 0.62f, true);
          for (size_t idx = particles.size() - 14; idx < particles.size(); ++idx) {
              particles[idx].r = it->r; particles[idx].g = it->g; particles[idx].b = it->b;
          }
          camera.applyShake(0.26f);
          std::cout << "[Combat] Dimoo Ranged Butterfly Projectile hit Hirono! Hirono HP: " << hironoHp << std::endl;
          hit = true;
      }

      // 越界销毁：横向超过 4.5 并且不能飞太远，或者生命周期耗尽，或者命中
      if (hit || it->life <= 0.0f || std::abs(it->x) > 4.5f) {
          it = dimooProjectiles.erase(it);
      } else {
          ++it;
      }
  }
  ```

- [ ] **步骤 2：同步更新 `updateDimooVisuals` 中的定时蝴蝶粒子配色**
  在 `updateDimooVisuals` 中（约 370-398 行），由于该函数在玩家移动/大招中会自动定时喷发少量尾焰蝴蝶，我们需要将这里产生的蝴蝶按动作配色：
  - 大招状态：使用淡紫色 `(0.78f, 0.60f, 0.98f)`。
  - 二技能状态：使用青蓝色 `(0.40f, 0.85f, 0.98f)`。
  - 移动状态：使用浪漫粉色 `(0.98f, 0.68f, 0.82f)`。
  ```cpp
  // 修改 Game::updateDimooVisuals 后半部分
  float ultLift = 0.23f * clamp(dimooUltPulse, 0.0f, 1.2f);
  spawnDimooButterflies(dimooX, dimooY + ultLift + 0.55f, dimooZ, count, spread, burst);
  
  // 改变新产生的这批粒子的颜色
  size_t startIdx = particles.size() - count;
  float pr = 0.98f, pg = 0.68f, pb = 0.82f; // 默认粉
  if (dimooUltPulse > 0.18f) {
      pr = 0.78f; pg = 0.60f; pb = 0.98f; // 紫
  } else if (dimooSkillPulse > 0.18f) {
      pr = 0.40f; pg = 0.85f; pb = 0.98f; // 青蓝
  }
  for (size_t idx = startIdx; idx < particles.size(); ++idx) {
      particles[idx].r = pr; particles[idx].g = pg; particles[idx].b = pb;
  }
  dimooButterflyTimer = nextDelay;
  ```

- [ ] **步骤 3：编译验证**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak debug`
  预期：无编译错误。

- [ ] **步骤 4：Commit**
  ```bash
  git add Project/src/Game.cpp
  git commit -m "feat: implement projectile physical update, collision detection, and timed particle coloration"
  ```

---

### 任务 5：实现 3D 蝴蝶弹体绘制与粒子 3D 化渲染 (Game)

**文件：**
- 修改：`Project/src/Game.cpp:784-817` (draw)

- [ ] **步骤 1：重构 `Game::draw` 粒子绘制循环并增加 Projectile 渲染**
  修改粒子绘制部分，将原本绘制 `PARTICLE_BUTTERFLY` 时的 `GL_POINTS` 逻辑全面修改为调用接口的 3D 梦蝶模型绘制。同时在半透明物体渲染前，循环绘制所有的远程弹体 `dimooProjectiles`。
  ```cpp
  // 修改 Game::draw 约第 784-817 行的粒子循环
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  
  // 先绘制非蝴蝶粒子（火花、落地灰尘）
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (const auto& p : particles) {
      if (p.kind != GameParticle::PARTICLE_BUTTERFLY) {
          glColor4f(p.r, p.g, p.b, p.a);
          glPointSize(p.size);
          glBegin(GL_POINTS);
          glVertex3f(p.x, p.y, p.z);
          glEnd();
      }
  }
  glPointSize(1.0f);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  // 渲染 3D 蝴蝶粒子 (3D 几何，需要开启 Lighting)
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  for (const auto& p : particles) {
      if (p.kind == GameParticle::PARTICLE_BUTTERFLY) {
          glPushMatrix();
          glTranslatef(p.x, p.y, p.z);
          // 计算 Y 轴偏航朝向
          float angleY = atan2(p.vx, p.vz) * 180.0f / (float)M_PI;
          glRotatef(angleY, 0.0f, 1.0f, 0.0f);
          
          // 计算翅膀振动相位
          float wingFlap = 30.0f + 40.0f * sin(p.phase);
          // 渲染 3D 蝴蝶模型，使用其粒子分配的色彩和 alpha 状态
          DimooModel::drawButterfly3D(wingFlap, p.size * 0.016f, true, p.a, p.r, p.g, p.b);
          glPopMatrix();
      }
  }

  // 渲染二技能远程蝴蝶弹体
  for (const auto& proj : dimooProjectiles) {
      glPushMatrix();
      glTranslatef(proj.x, proj.y, proj.z);
      if (!proj.facingRight) {
          glScalef(-1.0f, 1.0f, 1.0f);
      }
      // 计算快速振翅
      float wingAngle = 30.0f + 45.0f * sin(proj.phase * 2.2f);
      DimooModel::drawButterfly3D(wingAngle, proj.size * 0.16f, true, 1.0f, proj.r, proj.g, proj.b);
      glPopMatrix();
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);
  ```

- [ ] **步骤 2：清理与编译验证**
  运行：`mingw32-make -f DimooVsHirona.cbp.mak clean`
  运行：`mingw32-make -f DimooVsHirona.cbp.mak debug`
  预期：清理并重新编译成功，最终链接成功并生成二进制文件。

- [ ] **步骤 3：Commit**
  ```bash
  git add Project/src/Game.cpp
  git commit -m "feat: render 3D butterflies for particles and ranged projectiles in Game::draw"
  ```

---

## 验证与测试方案

1. **自动构建验证**：
   - 运行 `mingw32-make -f DimooVsHirona.cbp.mak debug` 确保代码可正确编译出 `FightingGameDebug.exe`。
2. **战斗状态验证**：
   - 运行对战程序 `.\FightingGame.exe`（或 `.\FightingGameDebug.exe`）。
   - 按键盘进行跳跃测试，观察跳跃高度是否增加（是否能明显跳起闪避地面攻击）。
   - **普通攻击测试**：按下平A键，检查是否有 4 只粉色 3D 蝴蝶爆出，并检查近距离命中是否判定成功。
   - **二技能（远程弹射）测试**：按下二技能键，检查是否有一只较大尺寸的青蓝色 3D 梦蝶从手中飞出，且飞行时有振翅和正弦曲线的灵动起伏。检查当梦蝶飞向远处的 Hirono 并接触时，是否爆散出 14 只青蓝色小蝴蝶、触发相机震动和 16 点伤害。
   - **二技能闪避测试**：在敌方射来青蓝梦蝶时，精确按下跳跃键，查看当角色处于空中时，蝴蝶弹体是否能从脚底平滑滑过，完全不产生伤害或受击表现。
   - **大招（梦境狂澜）测试**：按下大招键，检查空中施法后，是否产生了横向大判定，且伴随全屏梦境泛白，并有 40 只淡紫色 3D 梦蝶如同狂澜般高速飞向前方的对手，造成 30 点伤害。
