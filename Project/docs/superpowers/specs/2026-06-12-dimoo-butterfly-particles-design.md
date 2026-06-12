# Dimoo 3D 蝴蝶粒子与远程二/三技能设计规格书 (Dimoo 3D Butterfly Particles & Ranged Attack Design Spec)

本文档记录了为 Dimoo 3D 模型设计的蝴蝶粒子 3D 化、二技能远程梦蝶弹射、大招（蝴蝶梦境）超宽屏判定与狂澜特效，以及跳跃机制增强的详细方案。

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

## 5. 跳跃机制与闪避判定增强 (Jumping & Dodging Mechanism)

为了让玩家能够有效闪避上述远程攻击和判定，对游戏内的跳跃高度与伤害检测进行如下增强：

### 5.1 提升跳跃高度
* **修改类**：`Game`
* **逻辑**：将双方角色起跳时的初速度 `Vy` 从 `5.5f` 提升至 `6.8f`。
  - 这样，角色的最大跳跃高度将从约 `1.0f` 提高到约 `1.54f`，能更容易滞空躲避横向飞来的远程蝴蝶以及大招判定。

### 5.2 精确 Y 轴碰撞判定
* 所有近身和远程判定逻辑均加入严格的 Y 轴高度差计算，确保玩家在跳跃到半空时，能完全闪避地面上的普攻判定与飞来的蝴蝶弹体。

---

## 6. 拟修改文件清单 (Proposed File Changes)

- **[DimooModel.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.h)**：
  - 导出 `drawButterfly3D` 并支持自定义颜色及透明度。
- **[DimooModel.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.cpp)**：
  - 将 `drawButterfly3D` 改为非静态，并接入 `alpha` 与自定义 RGB。
- **[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)**：
  - 新增 `Projectile` 结构体及 `dimooProjectiles` 容器。
- **[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp)**：
  - 将双方角色跳跃初速度提升至 `6.8f`。
  - 在 `update` 中实现远程弹体运动与碰撞检测。
  - 在 `performDimooAttack` 中重构二、三技能判定与特效。
  - 在 `draw` 中渲染所有弹体并使用 3D 蝴蝶绘制 `PARTICLE_BUTTERFLY` 粒子。
