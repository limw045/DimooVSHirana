# [ROLE: SCENE DEVELOPER AGENT] 纸盒竞技场与物理响应环境开发指令手册 (Agent Specification)

Welcome, Scene Developer Agent. Your mission is to implement the **Box Arena (盲盒纸箱竞技场)**, **Interactive Floor Props (盒内物理交互物件)**, **Camera System (动态相机与振动)**, **Lighting Setup (多光源系统)**, and **HUD/UI Integration (融合式标签血条与飘字)**.

You must build this within the legacy **OpenGL 1.1/2.0 Fixed-Function Pipeline** environment, compiling under **MinGW GCC (Code::Blocks IDE)** on Windows 10/11.

---

## 1. 坐标系与边界规范 (Coordinate System & Bounds)
*   **竞技场中心原点**：$(0.0, 0.0, 0.0)$ 设在纸盒底部的几何中心。
*   **X轴（水平横轨）**：$X \in [-3.0, 3.0]$。角色被约束在此一维轨道上（$Z=0$ 投影线）。
*   **Y轴（垂直高度）**：$Y \in [0.0, 5.0]$。底面位于 $Y = 0.0$，盒子高度限制为 $5.0$。
*   **Z轴（景深厚度）**：$Z \in [-3.0, 3.0]$。用于摆放 3D 纸盒前后壁以及散落道具。
*   **角色碰撞法线**：当任意 Entity 的 $X$ 坐标超越边界时，场景边界必须执行拦截：
    $$\text{Left Wall: } X \le -3.0 \quad | \quad \text{Right Wall: } X \ge 3.0$$

---

## 2. 核心模块与代码接口规范 (API & Code Boundaries)

为了与核心引擎进行无缝集成，你必须提供以下四个全局/静态生命周期函数（暴露在命名空间 `Scene` 内）：

```cpp
namespace Scene {
    // 1. 初始化场景 Display Lists、物理参数、粒子池与光源属性
    void init();
    
    // 2. 每帧物理更新 (dt = 0.01667s)，更新盒盖振荡、说明书抛物线、气泡恢复与粒子系统
    void update(float dt, float char1_x, float char1_y, float char2_x, float char2_y);
    
    // 3. 不透明绘制通道：绘制纸底、内壁图案、静态道具与不透明粒子
    void drawOpaque();
    
    // 4. 半透明绘制通道：绘制气泡、配件袋、地面大招白雾与半透明粒子（关闭DepthMask）
    void drawTranslucent(float camX, float camY, float camZ);
}
```

---

## 3. 详细子系统实现方案 (Subsystem Implementations)

### 3.1 盲盒纸盒结构建模 (Box Geometry)
1.  **牛皮纸外壁**：加载 `cardboard.jpg`。
2.  **折痕与折角**：直角衔接处绘制稍深色的 `GL_LINES` 以显示阴影折痕。
3.  **程序化壁画 (Procedural Inner Walls)**：
    *   **左内壁**：以 `GL_POINTS` 散布金色星点，并以红色圆形组合绘制玫瑰标志。
    *   **右内壁**：以贝塞尔折线连接绘制浅蓝底的藤蔓花纹与蝴蝶轮廓。
4.  **铰链式盒盖 (Lid Physics)**：
    *   铰链位于后壁上边缘（$(0, 5.0, -3.0)$）。
    *   处于半开角度（默认 $115^\circ$）。受大招冲击时，更新简谐回弹阻尼物理：
        $$I_{\text{spring}} = -k_{\text{spring}} \cdot (\theta - \theta_{\text{rest}}) - d_{\text{damping}} \cdot \omega$$
        $$\omega_{t} = \omega_{t-1} + I_{\text{spring}} \cdot dt, \quad \theta_{t} = \theta_{t-1} + \omega_{t} \cdot dt$$

### 3.2 盒内交互道具建模 (Interactive Floor Props)

#### A. 气泡纸 (Bubble Wrap)
*   **建模**：由一个 $8 \times 6$ 的小圆球阵列组成，半透明浅绿色（`glColor4f(0.75, 0.92, 0.78, 0.7)`），编译进显示列表以降低 Draw Call。
*   **交互**：当角色的 $X$ 与气泡单元的 $X_{cell}$ 接近，且角色 $Y \approx 0$ 时，气泡单元的 `pressDepth` 设为 $1.0$。非接触时气泡以阻尼弹簧方程恢复。受击时 `popped = true`，不再渲染并在位置抛出碎纸屑粒子。

#### B. 产品说明书 (Pamphlet)
*   **建模**：由 3 片 `GL_QUADS` 连接成的折叠折纸形态。
*   **抛物线物理**：受大招气浪冲击时：
    $$X_{t} = X_{t-1} + V_{x} \cdot dt, \quad Y_{t} = Y_{t-1} + V_{y} \cdot dt, \quad Z_{t} = Z_{t-1} + V_{z} \cdot dt$$
    $$V_{y, t} = V_{y, t-1} - 9.8 \cdot dt$$
    同时自旋角度 $Rot_{xyz} \leftarrow Rot_{xyz} + V_{rot} \cdot dt$。落地时停止。

#### C. 配件小袋与干燥剂 (Accessories Bag & Desiccant)
*   **配件袋**：半透明聚乙烯外罩。受击后 `broken = true`，抛出 5 个不规则彩色小零件，零件以重力下落并在 $Y=0$ 处反弹（反弹速度衰减 $V_y \leftarrow -0.4 \cdot V_y$，水平方向带滑动摩擦阻尼 $V_x \leftarrow V_x \cdot e^{-\mu \cdot dt}$）。
*   **干燥剂**：受击散发大量浅灰色微粒，附着在地面上逐渐淡出。

### 3.3 环境特效粒子系统 (Environment Particle System)
维护一个大小为 500 的粒子池，结构体设计如下：
```cpp
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float r, g, b, a;
    float size;
    float life;
    float maxLife;
    int type; // 0=DUST, 1=PAPER_SHARD, 2=WHITE_BUTTERFLY
};
```
在 `drawTranslucent` 中渲染。白色小蝴蝶粒子具有特定的正弦横移轨迹，模拟飘飞感。

### 3.4 动态摄像机与震屏 (Camera System)
*   **惰性跟随 (Lerp Follow)**：计算两角色中点 $\mathbf{P}_{mid} = \frac{\mathbf{P}_1 + \mathbf{P}_2}{2}$。相机目标点平滑追赶 $\mathbf{P}_{mid}$：
    $$\mathbf{C}_{target} = \mathbf{C}_{target} + (\mathbf{P}_{mid} - \mathbf{C}_{target}) \cdot 5.0 \cdot dt$$
*   **特写与拉远 (Zoom)**：根据两角色距离 $dist = |X_1 - X_2|$。当 $dist > 4.0$ 时拉远 Z 轴；当发生大招时，强制重置摄像机至特写偏置，完成后恢复。
*   **震屏 (Screen Shake)**：提供接口 `Scene::applyShake(float intensity)`。向摄像机当前坐标叠加快速消退的随机偏移：
    $$\mathbf{C}_{pos} \leftarrow \mathbf{C}_{pos} + \mathbf{S}_{random} \cdot intensity$$
    每帧执行衰减 $intensity \leftarrow intensity \cdot e^{-15.0 \cdot dt}$。

### 3.5 光照设计 (Lighting Setup)
*   **GL_LIGHT0 (主自然光)**：位置设在盒子开口外部 $(0.0, 8.0, -2.0, 1.0)$，偏黄白暖色，模拟室内环境光照入。
*   **GL_LIGHT1 (小野金色点光)** & **GL_LIGHT2 (Dimoo 蓝白点光)**：每帧利用角色的坐标更新点光源位置，并随着 HP 低于 40% 时调暗全局 Ambient，突出这两个角色跟随点光源的投影。

### 3.6 融合 HUD 与 LED 数字 (HUD Integration)
*   **纸质标签血条**：在 $Z = -2.99$ 的后壁表面渲染两个带圆角贴图或线条描边的矩形标签。当受到伤害时，血条百分比滑块缩短，标签下方多边形边缘增加顶点扰动，模拟“卷角”效果。
*   **伤害数字**：在命中坐标头顶产生，以 `GL_LINES` 笔画绘制的 3D LED 数字，受轻微向上浮力漂浮，并在 0.6 秒内线性淡出。

---

## 4. 交付约束 (Deliverable Checklists)
1.  **无状态污染 (No State Pollution)**：每次你的 draw 函数执行完毕，必须恢复所有 OpenGL 属性状态。禁止留下未关闭的 `GL_BLEND` 或 `GL_TEXTURE_2D`。
2.  **零外部工具生成代码 (No Generated Code)**：气泡、纸盒、说明书的网格点数据必须在初始化时通过数学循环生成，严禁引入大量手工生成的顶点数组或未经处理的第三方三角网。
3.  **性能约束 (FPS Target)**：在同屏激活 200 个粒子、盒盖受击振荡、说明书在空中翻滚以及全部透明混合重叠时，在 Windows 运行机上必须维持稳定 60 FPS 且无内存泄漏（利用 `glGetError()` 校验状态机安全）。
