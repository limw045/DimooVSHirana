# Dimoo 3D 蝴蝶粒子与远程二/三技能及跳跃增强设计规格书 (Dimoo 3D Butterfly Particles & Ranged Attack Design Spec)

本文档记录了为 Dimoo 3D 模型设计的蝴蝶粒子 3D 化、二技能远程梦蝶弹射、大招（蝴蝶梦境）超宽屏判定与狂澜特效，重力/跳跃机制修复增强，以及藤蔓环（树枝）定位与自转归位机制的详细方案。

---

## 1. 3D 蝴蝶粒子渲染 (3D Butterfly Particle Rendering)

将场景中所有 `GameParticle::PARTICLE_BUTTERFLY` 类型的粒子由原始的 `GL_POINTS` 像素点升级为真实感强的 3D 振翅蝴蝶。

### 1.1 导出 `drawButterfly3D` 方法
* **修改类**：`DimooModel`
* **签名变更**：
  ```cpp
  // DimooModel.h / DimooModel.cpp
  void drawButterfly3D(float wingAngle, float scale, bool glow, float alpha = 1.0f, float r = 0.82f, float g = 0.94f, float b = 0.78f);
  ```
* **色彩与淡出支持**：
  在 `drawButterfly3D` 内部：
  - 翅膀的 Ambient 和 Diffuse 材质属性由传入的 `r, g, b` 决定，实现自定义色彩。
  - 所有反射材质属性（Ambient、Diffuse、Specular）的 `alpha` 字段乘以上述传入的 `alpha` 参数，支持粒子衰减时的平滑渐变消逝。

### 1.2 粒子渲染更新
* **修改类**：`Game`
* **逻辑**：
  在 `Game::draw` 的粒子循环中：
  - 遇到 `PARTICLE_BUTTERFLY` 粒子时，调用 `DimooModel::drawButterfly3D`。
  - 传入粒子的颜色 `p.r, p.g, p.b`。
  - 根据其运动速度方向计算 Y 轴偏航角：`atan2(vx, vz) * 180.0f / M_PI`，使蝴蝶头部指向飞行前方。
  - 根据粒子的生命相位 `phase` 控制双翼上下振动：`wingFlap = 30.0f + 40.0f * sin(phase)`。

---

## 2. 三个技能的蝴蝶配色方案 (Three Skill Color Themes)

为了体现技能的区分度，平A、二技能和大招的蝴蝶粒子分配三种不同的主题色：
1. **普通攻击 (Skill 1 - 平A)**: **浪漫粉色 (Romantic Pink)**
   - RGB: `(0.98f, 0.68f, 0.82f)`
2. **二技能 (Skill 2 - 梦蝶射击)**: **梦幻青蓝 (Dreamy Cyan)**
   - RGB: `(0.40f, 0.85f, 0.98f)`
3. **大招 (Skill 3 - 梦境狂澜)**: **圣洁淡紫/金黄 (Sacred Light Purple)**
   - RGB: `(0.78f, 0.60f, 0.98f)`

---

## 3. 二技能重构：远程梦蝶弹射 (Skill 2: Ranged Magic Butterfly)

将 Dimoo 的二技能由原本的近身范围判定重构为射出单个 3D 梦蝶弹体的远程攻击。

### 3.1 弹体结构设计
在 `Game.h` 中新增 Projectile 结构体及容器：
```cpp
struct Projectile {
    float x, y, z;
    float vx, vy, vz;
    float size;
    float phase;
    float life;
    float maxLife;
    bool facingRight;
    float r, g, b; // 继承技能主题色
};
std::vector<Projectile> dimooProjectiles;
```

### 3.2 触发逻辑 (Skill Trigger)
当施放二技能时，实例化一个 Projectile，起始位置位于 Dimoo 前方 `Y = dimooY + 0.62f`，大小 `size = 1.5f`，颜色为梦幻青蓝。飞行速度 `vx = dimooFacingRight ? 3.0f : -3.0f`。

### 3.3 飞行与碰撞判定 (Flight & Hit Detection)
在 `Game::update` 中更新位置，并检测弹体与 Hirono 中心的 3D 距离。当 `dist < 0.8f` 时判定命中：
  - Hirono HP 减少 `16.0f`，命中点爆散出 14 只 3D 青蓝小蝴蝶，相机抖动，弹体销毁。
  - 飞出边界或超时自动销毁。

---

## 4. 三技能（大招）重构：梦境狂澜远程判定 (Ultimate: Dream Torrent Ranged Hit)

* **范围扩大**：判定范围扩大至横向 `8.0f`，只要敌人在该范围内均会受击。
* **蝴蝶狂澜**：施放时向 Hirono 方向喷射 40 只高速飞行的淡紫色 3D 梦蝶。

---

## 5. 重力与跳跃闪避机制修复与增强 (Jumping & Dodging Mechanism Fix)

### 5.1 跳跃失效 Bug 修复
* **发现**：原先的重力更新代码逻辑为 `if (hironoY > 0.0f)` 和 `if (dimooY > DIMOO_REST_Y)`。当角色位于地面且被赋予上升速度 `Vy` 时，由于其 `Y` 坐标仍然等于地面高度，重力更新块不会执行，导致角色的 `Y` 坐标被锁死，无法起跳。
* **修复方法**：将条件判断重构为：
  - Hirono: `if (hironoY > 0.0f || hironoVy > 0.0f)`
  - Dimoo: `if (dimooY > DIMOO_REST_Y || dimooVy > 0.0f)`
  这能确保起跳瞬间（`Vy > 0.0f`）位置更新和重力逻辑能够被正确执行，角色可正常腾空。

### 5.2 提升跳跃高度
* **修改类**：`Game`
* **逻辑**：将双方角色起跳时的初速度 `Vy` 从 `5.5f` 提升至 `6.8f`。
  - 这样，角色的最大跳跃高度将从约 `1.0f` 提高到约 `1.54f`，能更容易滞空躲避横向飞来的远程蝴蝶以及大招判定。

### 5.3 精确 Y 轴碰撞判定
* 所有近身和远程判定逻辑均加入严格的 Y 轴高度差计算，确保玩家在跳跃到半空时，能完全闪避地面上的普攻判定与飞来的蝴蝶弹体。

---

## 6. 藤蔓环（树枝）定位与自转归位机制 (Vine Ring Placement & Return Rotation)

为了使 Dimoo 身体周围的藤蔓环在平时保持在后方，并且在施放技能时能自转且最终精准回到预设角度，设计如下机制：

### 6.1 平时靠后定位
* **修改类**：`DimooModel`
* **逻辑**：在 `DimooModel.cpp` 中将藤蔓环（Vine Ring）相对于 Body 中心的 Z 轴位置向后偏移 `-0.15f`。
  ```cpp
  glTranslatef(0.0f, 0.01f, -0.15f); // 相对身体中心微调，往后偏移 Z 轴，平时完全在身体后面
  ```

### 6.2 移除平时常态自转，改为静止
* 移除 `drawVineRing` 中原有的 `float rotationY = t * 10.0f;` 常态时间自转，使其在平时保持静态。

### 6.3 技能与大招自转并精准归位
* **逻辑**：当施放技能和大招时，使用随 `ultPulse` 或 `skillPulse` 从起步到归零的插值计算自转角度，从而保证自转的方向是向前旋转，并且在动画结束时正好转满整圈，停留在 $0^\circ$ 的初始靠后位置（不会停在奇怪的夹角）：
  - **二技能 (Skill)**：`skillPulse` 从 `1.2f` 归零。使用进度 `p = (1.2f - skillPulse) / 1.2f`，旋转角度为 `p * 720.0f`（自转 2 整圈）。
  - **大招 (Ultimate)**：`ultPulse` 从 `1.5f` 归零。使用进度 `p = (1.5f - ultPulse) / 1.5f`，旋转角度为 `p * 1080.0f`（自转 3 整圈）。
  - **公式**：
    ```cpp
    float rotationY = 0.0f;
    if (state.ultPulse > 0.0f) {
        float p = (1.5f - state.ultPulse) / 1.5f;
        rotationY = p * 1080.0f;
    } else if (state.skillPulse > 0.0f) {
        float p = (1.2f - state.skillPulse) / 1.2f;
        rotationY = p * 720.0f;
    }
    ```

---

## 7. 拟修改文件清单 (Proposed File Changes)

- **[DimooModel.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.h)**：
  - 导出 `drawButterfly3D` 并支持自定义颜色及透明度。
- **[DimooModel.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.cpp)**：
  - 将 `drawButterfly3D` 改为非静态，并接入 `alpha` 与自定义 RGB。
  - 将藤蔓环位置修改为向后偏移 `-0.15f`，且平时静止。
  - 在 `drawVineRing` 中应用归位自转插值公式。
- **[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)**：
  - 新增 `Projectile` 结构体及 `dimooProjectiles` 容器。
- **[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp)**：
  - 将双方角色跳跃初速度提升至 `6.8f`。
  - 修复重力条件判断，使跳跃生效。
  - 在 `update` 中实现远程弹体运动与碰撞检测。
  - 在 `performDimooAttack` 中重构二、三技能判定与特效。
  - 在 `draw` 中渲染所有弹体并使用 3D 蝴蝶绘制 `PARTICLE_BUTTERFLY` 粒子。
