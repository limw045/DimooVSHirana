# Dimoo 3D 蝴蝶粒子与远程二/三技能设计规格书 (Dimoo 3D Butterfly Particles & Ranged Attack Design Spec)

本文档记录了为 Dimoo 3D 模型设计的蝴蝶粒子 3D 化、二技能远程梦蝶弹射以及大招（蝴蝶梦境）超宽屏判定与狂澜特效的详细方案。

---

## 1. 3D 梦蝶渲染接口化与淡出支持 (DimooModel)

将场景中所有 `GameParticle::PARTICLE_BUTTERFLY` 类型的粒子由原始的 `GL_POINTS` 像素点升级为真实感强的 3D 振翅蝴蝶。

### 1.1 导出 `drawButterfly3D` 方法
* **修改类**：`DimooModel`
* **签名变更**：
  ```cpp
  // DimooModel.h / DimooModel.cpp
  void drawButterfly3D(float wingAngle, float scale, bool glow, float alpha = 1.0f);
  ```
* **材质与淡出支持**：
  在 `drawButterfly3D` 内部，所有反射材质属性（Ambient、Diffuse、Specular）的 `alpha` 字段乘以上述传入 of `alpha` 参数，支持粒子衰减时的平滑渐变消逝。

### 1.2 粒子渲染更新
* **修改类**：`Game`
* **逻辑**：
  在 `Game::draw` 的粒子循环中：
  - 遇到 `PARTICLE_BUTTERFLY` 粒子时，调用 `DimooModel::drawButterfly3D`。
  - 根据其运动速度方向计算 Y 轴偏航角：`atan2(vx, vz) * 180.0f / M_PI`，使蝴蝶头部指向飞行前方。
  - 根据粒子的生命相位 `phase` 控制双翼上下振动：`wingFlap = 30.0f + 40.0f * sin(phase)`。

---

## 2. 二技能重构：远程梦蝶弹射 (Skill 2: Ranged Magic Butterfly)

将 Dimoo 的二技能由原本的近身范围判定重构为射出单个 3D 梦蝶弹体的远程攻击。

### 2.1 弹体结构设计
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
};
std::vector<Projectile> dimooProjectiles;
```

### 2.2 触发逻辑 (Skill Trigger)
在 `Game::performDimooAttack(2)` 中：
* 触发施法前摇与音效。
* 实例化一个 Projectile，起始位置位于 Dimoo 前方 `Y = dimooY + 0.62f`，大小 `size = 1.5f`。
* 飞行速度 `vx = dimooFacingRight ? 3.0f : -3.0f`。
* 在起始处爆散 6 只小型 3D 梦蝶作为辅助蓄力特效。

### 2.3 飞行与碰撞判定 (Flight & Hit Detection)
在 `Game::update` 中：
* **运动物理**：`x` 轴横移，同时 `y` 轴施加 `sin(phase * 1.5f) * 0.05f` 的正弦扰动以模拟真实的飞舞姿态。
* **碰撞检测**：检测弹体与 Hirono 中心的 3D 距离，当 `dist < 0.8f` 时判定命中：
  - Hirono HP 减少 `16.0f`。
  - 命中点爆散出 `14` 只 3D 小蝴蝶作为命中效果，产生蓝色火花，相机抖动。
  - 弹体销毁。
* **边界销毁**：若弹体横向超出 `±4.5f` 或生命周期 `2.0s` 耗尽，则自动销毁。

---

## 3. 三技能（大招）重构：梦境狂澜远程判定 (Ultimate: Dream Torrent Ranged Hit)

解决大招在空中施放时由于高度或距离差难以命中敌人的问题。

### 3.1 判定范围扩大
* 在 `Game::performDimooAttack(3)` 中，将大招命中判定范围由 `3.0f` 扩大至横向 `8.0f`（几乎覆盖整个对战木台）。

### 3.2 梦境狂澜特效 (Butterfly Torrent)
* 施放大招时，不再采用默认的四周爆散，而是直接从 Dimoo 处召唤 40 只高速飞行的 3D 梦蝶（横向速度极快且指向 Hirono），如浪潮般铺天盖地压向对手，实现完美的视觉引导与冲击感。

---

## 4. 拟修改文件清单 (Proposed File Changes)

- **[DimooModel.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.h)**：
  - 导出 `drawButterfly3D` 函数。
- **[DimooModel.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.cpp)**：
  - 将 `drawButterfly3D` 去除 `static` 限制，并支持 `alpha` 渐变。
- **[Game.h](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.h)**：
  - 新增 `Projectile` 结构体及 `dimooProjectiles` 容器。
- **[Game.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/Game.cpp)**：
  - 在 `update` 中实现远程弹体的前进、正弦振荡、生命衰减和与 Hirono 的碰撞检测。
  - 在 `performDimooAttack` 中，将二技能改为发射弹体，大招扩大范围至 `8.0f` 并喷射指向敌人的快速蝴蝶雨。
  - 在 `draw` 中渲染所有 `dimooProjectiles` 并更新粒子循环中的 `PARTICLE_BUTTERFLY` 渲染逻辑为 3D 蝴蝶。
